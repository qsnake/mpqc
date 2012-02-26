//
// transform_ikjy.cc
//
// Copyright (C) 2004 Edward Valeev
//
// Author: Edward Valeev <edward.valeev@chemistry.gatech.edu>
// Maintainer: EV
//
// This file is part of the SC Toolkit.
//
// The SC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The SC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#ifdef __GNUG__
#pragma implementation
#endif

#include <stdexcept>

#include <util/class/scexception.h>
#include <util/misc/formio.h>
#include <util/state/state_bin.h>
#include <util/ref/ref.h>
#include <math/scmat/local.h>
#include <chemistry/qc/mbptr12/transform_ikjy.h>
#include <chemistry/qc/mbptr12/r12ia_memgrp.h>
#include <chemistry/qc/mbptr12/r12ia_node0file.h>
#ifdef HAVE_MPIIO
  #include <chemistry/qc/mbptr12/r12ia_mpiiofile.h>
#endif
#include <chemistry/qc/mbptr12/transform_123inds.h>

using namespace std;
using namespace sc;

inline int max(int a,int b) { return (a > b) ? a : b;}

/*-----------
  TwoBodyMOIntsTransform_ikjy
 -----------*/
static ClassDesc TwoBodyMOIntsTransform_ikjy_cd(
  typeid(TwoBodyMOIntsTransform_ikjy),"TwoBodyMOIntsTransform_ikjy",1,"public TwoBodyMOIntsTransform",
  0, 0, create<TwoBodyMOIntsTransform_ikjy>);

TwoBodyMOIntsTransform_ikjy::TwoBodyMOIntsTransform_ikjy(const std::string& name, const Ref<MOIntsTransformFactory>& factory,
                                                         const Ref<MOIndexSpace>& space1, const Ref<MOIndexSpace>& space2,
                                                         const Ref<MOIndexSpace>& space3, const Ref<MOIndexSpace>& space4) :
  TwoBodyMOIntsTransform(name,factory,space1,space2,space3,space4)
{
  init_vars();
}

TwoBodyMOIntsTransform_ikjy::TwoBodyMOIntsTransform_ikjy(StateIn& si) : TwoBodyMOIntsTransform(si)
{
  init_vars();
}

TwoBodyMOIntsTransform_ikjy::~TwoBodyMOIntsTransform_ikjy()
{
}

void
TwoBodyMOIntsTransform_ikjy::save_data_state(StateOut& so)
{
  TwoBodyMOIntsTransform::save_data_state(so);
}

//////////////////////////////////////////////////////
// Compute required (dynamic) memory
// for a given batch size of the transformation
//
// Only arrays allocated before exiting the loop over
// i-batches are included here  - only these arrays
// affect the batch size.
//////////////////////////////////////////////////////
distsize_t
TwoBodyMOIntsTransform_ikjy::compute_transform_dynamic_memory_(int ni) const
{
  int nproc = msg_->n();
  int nthread = thr_->nthread();

  int rank2 = space2_->rank();
  int rank3 = space3_->rank();
  int nbasis2 = space2_->basis()->nbasis();
  int nbasis4 = space4_->basis()->nbasis();
  int nfuncmax3 = space3_->basis()->max_nfunction_in_shell();
  int nfuncmax4 = space4_->basis()->max_nfunction_in_shell();
  
  // If basis3 == basis4 then permutational symmetry will be used in second step
  bool basis3_eq_basis4 = (space3_->basis() == space4_->basis());

  // compute nij as nij on node 0, since nij on node 0 is >= nij on other nodes
  int nij = compute_nij(ni, rank3, nproc, 0);
  
  distsize_t memsize = sizeof(double)*(num_te_types_*((distsize_t)nthread * ni * nbasis2 * nfuncmax3 * nfuncmax4 // iqrs
						     + (distsize_t)ni * rank2 * nfuncmax3 * nfuncmax4  // ikrs
						     + (distsize_t)nij * rank2 * nbasis4 // ikjs - buffer of 3 q.t. and higher
						     // transformed integrals
						     )
				       + (distsize_t)rank2 * nfuncmax4 // ks
				       + (distsize_t)rank2 * nbasis4 // kx
				       );
  return memsize;
}

const size_t
TwoBodyMOIntsTransform_ikjy::memgrp_blksize() const
{
  const int nbasis4 = space4_->basis()->nbasis();
  const int rank4 = space4_->rank();
  const int dim4 = (nbasis4 > rank4) ? nbasis4 : rank4;
  return space2_->rank()*dim4*sizeof(double);
}

void
TwoBodyMOIntsTransform_ikjy::init_acc()
{
  if (ints_acc_.nonnull())
    return;

  int nij = compute_nij(batchsize_, space3_->rank(), msg_->n(), msg_->me());
  alloc_mem((size_t)num_te_types_*nij*memgrp_blksize());

  switch (ints_method_) {

  case MOIntsTransformFactory::mem_only:
    if (npass_ > 1)
      throw std::runtime_error("TwoBodyMOIntsTransform_ikjy::init_acc() -- cannot use MemoryGrp-based accumulator in multi-pass transformations");
    ints_acc_ = new R12IntsAcc_MemoryGrp(mem_, num_te_types_, space1_->rank(), space3_->rank(), space2_->rank(), space4_->rank());  // Hack to avoid using nfzc and nocc
    break;

  case MOIntsTransformFactory::mem_posix:
    if (npass_ == 1) {
      ints_acc_ = new R12IntsAcc_MemoryGrp(mem_, num_te_types_, space1_->rank(), space3_->rank(), space2_->rank(), space4_->rank());
      break;
    }
    // else use the next case
      
  case MOIntsTransformFactory::posix:
    ints_acc_ = new R12IntsAcc_Node0File(mem_, (file_prefix_+"."+name_).c_str(), num_te_types_,
                                         space1_->rank(), space3_->rank(), space2_->rank(), space4_->rank());
    break;

#if HAVE_MPIIO
  case MOIntsTransformFactory::mem_mpi:
    if (npass_ == 1) {
      ints_acc_ = new R12IntsAcc_MemoryGrp(mem_, num_te_types_, space1_->rank(), space3_->rank(), space2_->rank(), space4_->rank());
      break;
    }
    // else use the next case

  case MOIntsTransformFactory::mpi:
    ints_acc_ = new R12IntsAcc_MPIIOFile_Ind(mem_, (file_prefix_+"."+name_).c_str(), num_te_types_,
                                             space1_->rank(), space3_->rank(), space2_->rank(), space4_->rank());
    break;
#endif
  
  default:
    throw std::runtime_error("TwoBodyMOIntsTransform_ikjy::init_acc() -- invalid integrals store method");
  }
}

void
TwoBodyMOIntsTransform_ikjy::check_int_symm(double threshold) const throw (ProgrammingError)
{
  Ref<R12IntsAcc> iacc = ints_acc();
  if (!iacc->is_committed())
    throw ProgrammingError("TwoBodyMOIntsTransform_ikjy::check_int_symm() is called but integrals not computed yet",
                           __FILE__, __LINE__);

  int num_te_types = iacc->num_te_types();
  int ni = iacc->ni();
  int nj = iacc->nj();
  int nk = iacc->nx();
  int ny = iacc->ny();
  vector<int> isyms = space1_->mosym();
  vector<int> jsyms = space3_->mosym();
  vector<int> ksyms = space2_->mosym();
  vector<int> ysyms = space4_->mosym();
  
  int me = msg_->me();
  vector<int> twi_map;
  int ntasks_with_ints = iacc->tasks_with_access(twi_map);
  if (!iacc->has_access(me))
    return;
  
  int ij=0;
  for(int i=0; i<ni; i++) {
    int isym = isyms[i];
    for(int j=0; j<nj; j++, ij++) {
      int jsym = jsyms[j];
      if (ij%ntasks_with_ints != twi_map[me])
        continue;
      
      for(int t=0; t<num_te_types; t++) {
        const double* ints = iacc->retrieve_pair_block(i,j,static_cast<R12IntsAcc::tbint_type>(t));
        int ky=0;
        for(int k=0; k<nk; k++) {
          int ksym = ksyms[k];
          for(int y=0; y<ny; y++, ky++) {
            int ysym = ysyms[y];
            if ( (isym^jsym^ksym^ysym) != 0 && fabs(ints[ky]) > threshold) {
              ExEnv::outn() << scprintf("Integral type=%d i=%d k=%d j=%d y=%d should be zero\n",t,i,k,j,y);
              throw ProgrammingError("TwoBodyMOIntsTransform_ikjy::check_int_symm() -- nonzero nonsymmetric integrals are detected",
                                     __FILE__, __LINE__);
            }
          }
        }
        iacc->release_pair_block(i,j,static_cast<R12IntsAcc::tbint_type>(t));
      }
    }
  }
}

/*void
TwoBodyMOIntsTransform_ikjy::compute()
{
  init_acc();

  Ref<ThreadLock> lock = thr_->new_lock();
  Ref<Integral> int_factory = factory_->integral();
  int_factory->set_basis(space1_->basis(),space2_->basis(),space3_->basis(),space4_->basis());
  Ref<TwoBodyInt> tbint = int_factory->grt();
  Ref<TwoBodyMOIntsTransform> this_tform = this;
  TwoBodyMOIntsTransform_123Inds* tform_123 = new TwoBodyMOIntsTransform_123Inds(this_tform,0,1,lock,tbint,-100.0,0);
  tform_123->run();
}*/

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End:
