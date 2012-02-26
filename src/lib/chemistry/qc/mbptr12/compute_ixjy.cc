//
// compute_ixjy.cc
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

#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include <scconfig.h>
#include <util/misc/formio.h>
#include <util/misc/timer.h>
#include <util/class/class.h>
#include <util/state/state.h>
#include <util/state/state_text.h>
#include <util/state/state_bin.h>
#include <math/scmat/matrix.h>
#include <chemistry/qc/mbpt/bzerofast.h>
#include <chemistry/qc/mbptr12/transform_ixjy.h>
#include <chemistry/qc/mbptr12/blas.h>
#include <chemistry/qc/mbptr12/transform_13inds.h>

using namespace std;
using namespace sc;

#define SINGLE_THREAD_E13   0
#define PRINT2Q 0
#define PRINT3Q 0
#define PRINT4Q 0
#define PRINT_NUM_TE_TYPES 1
#define CHECK_INTS_SYMM 1

/*-------------------------------------
  Based on MBPT2::compute_mp2_energy()
 -------------------------------------*/
void
TwoBodyMOIntsTransform_ixjy::compute()
{
  init_acc();
  if (ints_acc_->is_committed())
    return;
  
  Ref<Integral> integral = factory_->integral();
  Ref<GaussianBasisSet> bs1 = space1_->basis();
  Ref<GaussianBasisSet> bs2 = space2_->basis();
  Ref<GaussianBasisSet> bs3 = space3_->basis();
  Ref<GaussianBasisSet> bs4 = space4_->basis();
  int rank1 = space1_->rank();
  int rank2 = space2_->rank();
  int rank3 = space3_->rank();
  int rank4 = space4_->rank();
  int nbasis1 = bs1->nbasis();
  int nbasis2 = bs2->nbasis();
  int nbasis3 = bs3->nbasis();
  int nbasis4 = bs4->nbasis();
  int nshell1 = bs1->nshell();
  int nshell2 = bs2->nshell();
  int nshell3 = bs3->nshell();
  int nshell4 = bs4->nshell();
  int nfuncmax1 = bs1->max_nfunction_in_shell();
  int nfuncmax2 = bs2->max_nfunction_in_shell();
  int nfuncmax3 = bs3->max_nfunction_in_shell();
  int nfuncmax4 = bs4->max_nfunction_in_shell();
  enum te_types {eri=0, r12=1, r12t1=2};
  const size_t memgrp_blocksize = memgrp_blksize();

  // log2 of the erep tolerance
  // (erep < 2^tol => discard)
  const int tol = (int) (-10.0/log10(2.0));  // discard ints smaller than 10^-20

  int aoint_computed = 0; 

  std::string tim_label("tbint_tform_ikjy ");
  tim_label += name_;
  tim_enter(tim_label.c_str());

  print_header();

  // Compute the storage remaining for the integral routines
  size_t dyn_mem = distsize_to_size(compute_transform_dynamic_memory_(batchsize_));

  int me = msg_->me();
  int nproc = msg_->n();
  const int restart_orb = restart_orbital();
  int nijmax = compute_nij(batchsize_,rank3,nproc,me);
  
  vector<int> mosym1 = space1_->mosym();
  vector<int> mosym2 = space2_->mosym();
  vector<int> mosym3 = space3_->mosym();
  vector<int> mosym4 = space4_->mosym();
  double** vector2 = new double*[nbasis2];
  double** vector4 = new double*[nbasis4];
  vector2[0] = new double[rank2*nbasis2];
  vector4[0] = new double[rank4*nbasis4];
  for(int i=1; i<nbasis2; i++) vector2[i] = vector2[i-1] + rank2;
  for(int i=1; i<nbasis4; i++) vector4[i] = vector4[i-1] + rank4;
  space2_->coefs().convert(vector2);
  space4_->coefs().convert(vector4);

  /////////////////////////////////////
  //  Begin transformation loops
  /////////////////////////////////////

  // debug print
  if (debug_ >= 2) {
    ExEnv::outn() << indent
		  << scprintf("node %i, begin loop over i-batches",me) << endl;
  }
  // end of debug print

  // Initialize the integrals
  integral->set_storage(memory_ - dyn_mem);
  integral->set_basis(space1_->basis(),space2_->basis(),space3_->basis(),space4_->basis());
  Ref<TwoBodyInt>* tbints = new Ref<TwoBodyInt>[thr_->nthread()];
  for (int i=0; i<thr_->nthread(); i++) {
    tbints[i] = integral->grt();
  }
  if (debug_ >= 1)
    ExEnv::out0() << indent << scprintf("Memory used for integral storage:       %i Bytes",
      integral->storage_used()) << endl;

  Ref<ThreadLock> lock = thr_->new_lock();
  TwoBodyMOIntsTransform_13Inds** e13thread = new TwoBodyMOIntsTransform_13Inds*[thr_->nthread()];
  for (int i=0; i<thr_->nthread(); i++) {
    e13thread[i] = new TwoBodyMOIntsTransform_13Inds(this,i,thr_->nthread(),lock,tbints[i],-100.0,debug_);
  }
  
  /*-----------------------------------

    Start the integrals transformation

   -----------------------------------*/
  tim_enter("mp2-r12/a passes");
  if (me == 0 && top_mole_.nonnull() && top_mole_->if_to_checkpoint() && ints_acc_->can_restart()) {
    StateOutBin stateout(top_mole_->checkpoint_file());
    SavableState::save_state(top_mole_,stateout);
    ExEnv::out0() << indent << "Checkpointed the wave function" << endl;
  }

  for (int pass=0; pass<npass_; pass++) {

    ExEnv::out0() << indent << "Beginning pass " << pass+1 << endl;

    int ni = batchsize_;
    int i_offset = restart_orb + pass*ni;
    if (pass == npass_ - 1)
      ni = rank1 - batchsize_*(npass_-1);

    // Compute number of of i,j pairs on each node during current pass for
    // two-el integrals
    int nij = compute_nij(ni,rank3,nproc,me);

    // debug print
    if (debug_)
      ExEnv::outn() << indent << "node " << me << ", nij = " << nij << endl;
    // end of debug print

    // Allocate and initialize some arrays
    // (done here to avoid having these arrays
    // overlap with arrays allocated later)

    // Allocate (and initialize) some arrays

    double* integral_ijsq = (double*) mem_->localdata();
    //bzerofast(integral_ijsx, (num_te_types_*nij*memgrp_blocksize/sizeof(double)));
    memset(integral_ijsq, 0, num_te_types_*nij*memgrp_blocksize);
    integral_ijsq = 0;
    mem_->sync();
    ExEnv::out0() << indent
		  << scprintf("Begin loop over shells (ints, 1+2 q.t.)") << endl;

    // Do the two electron integrals and the first two quarter transformations
    tim_enter("ints+1qt+2qt");
    shell_pair_data()->init();
    for (int i=0; i<thr_->nthread(); i++) {
      e13thread[i]->set_i_offset(i_offset);
      e13thread[i]->set_ni(ni);
      thr_->add_thread(i,e13thread[i]);
#     if SINGLE_THREAD_E13
      e13thread[i]->run();
#     endif
    }
#   if !SINGLE_THREAD_E13
    thr_->start_threads();
    thr_->wait_threads();
#   endif
    tim_exit("ints+1qt+2qt");
    ExEnv::out0() << indent << "End of loop over shells" << endl;

    mem_->sync();  // Make sure ijsq is complete on each node before continuing
    integral_ijsq = (double*) mem_->localdata();

#if PRINT2Q
    {
      for(int te_type=0; te_type<PRINT_NUM_TE_TYPES; te_type++) {
        for (int i = 0; i<ni; i++) {
          for (int q = 0; q<nbasis2; q++) {
            for (int j = 0; j<rank3; j++) {
              int ij = i*rank3+j;
              int ij_local = ij/nproc;
              if (ij%nproc == me) {
                const double* ijsq_ints = (const double*) ((size_t)integral_ijsq + (ij_local*num_te_types_+te_type)*memgrp_blocksize);
                for (int s = 0; s<nbasis4; s++) {
                  double value = ijsq_ints[s*rank2+q];
                  printf("2Q: type = %d (%d %d|%d %d) = %12.8f\n",
                         te_type,i+i_offset,q,j,s,value);
                }
              }
            }
          }
        }
      }
    }
#endif

    // Third quarter transform
    ExEnv::out0() << indent << "Begin third q.t." << endl;
    tim_enter("3. q.t.");
    // Begin third quarter transformation;
    // from (iq|js) stored as ijsq
    // generate (ix|js) stored as ijsx

    const int sx_size = nbasis4 * rank2 * sizeof(double);
    double* sx_ints = new double[nbasis4 * rank2];
    for (int i = 0; i<ni; i++) {
      for (int j = 0; j<rank3; j++) {
        int ij = i*rank3+j;
        int ij_local = ij/nproc;
        if (ij%nproc == me) {

          for(int te_type=0; te_type<num_te_types_; te_type++) {

            const double *sq_ptr = (const double*) ((size_t)integral_ijsq + (ij_local*num_te_types_+te_type)*memgrp_blocksize);

            // fourth quarter transform
            // sx = sq * qx
            const char notransp = 'n';
            const double one = 1.0;
            const double zero = 0.0;
            F77_DGEMM(&notransp,&notransp,&rank2,&nbasis4,&nbasis2,&one,vector2[0],&rank2,
                      sq_ptr,&nbasis2,&zero,sx_ints,&rank2);

            // copy the result back to integrals_ijsq
            memcpy((void*)sq_ptr,(const void*)sx_ints,sx_size);
          }
        }
      }
    }
    delete[] sx_ints;
    tim_exit("3. q.t.");
    ExEnv::out0() << indent << "End of third q.t." << endl;
    integral_ijsq = 0;

    double* integral_ijsx = (double*) mem_->localdata();
#if PRINT3Q
    {
      for(int te_type=0; te_type<PRINT_NUM_TE_TYPES; te_type++) {
        for (int i = 0; i<ni; i++) {
          for (int x = 0; x<rank2; x++) {
            for (int j = 0; j<rank3; j++) {
              int ij = i*rank3+j;
              int ij_local = ij/nproc;
              if (ij%nproc == me) {
                const double* ijsx_ints = (const double*) ((size_t)integral_ijsx + (ij_local*num_te_types_+te_type)*memgrp_blocksize);
                for (int s = 0; s<nbasis4; s++) {
                  double value = ijsx_ints[s*rank2+x];
                  printf("3Q: type = %d (%d %d|%d %d) = %12.8f\n",
                         te_type,i+i_offset,x,j,s,value);
                }
              }
            }
          }
        }
      }
    }
#endif

    // Fourth quarter transform
    ExEnv::out0() << indent << "Begin fourth q.t." << endl;
    tim_enter("4. q.t.");
    // Begin fourth quarter transformation;
    // generate (ix|jy) stored as ijxy

    double* ijxy_ints = new double[rank2*rank4];
    const size_t xy_size = rank2*rank4*sizeof(double);
    for(int te_type=0; te_type<num_te_types_; te_type++) {

      for (int i = 0; i<ni; i++) {
        for (int j = 0; j<rank3; j++) {
          int ij = i*rank3+j;
          int ij_local = ij/nproc;
          if (ij%nproc == me) {

            const double *sx_ptr = (const double*) ((size_t)integral_ijsx + (ij_local*num_te_types_+te_type)*memgrp_blocksize);

            // fourth quarter transform
            // xy = sx^t * sy
            const char notransp = 'n';
            const char transp = 't';
            const double one = 1.0;
            const double zero = 0.0;
            F77_DGEMM(&notransp,&transp,&rank4,&rank2,&nbasis4,&one,vector4[0],&rank4,
                      sx_ptr,&rank2,&zero,ijxy_ints,&rank4);

            // copy the result back to integrals_ijsx
            memcpy((void*)sx_ptr,(const void*)ijxy_ints,xy_size);
          }
        }
      }
    }
    delete[] ijxy_ints;
    tim_exit("4. q.t.");
    ExEnv::out0() << indent << "End of fourth q.t." << endl;

    integral_ijsx = 0;
    double* integral_ijxy = (double*) mem_->localdata();

    // Zero out nonsymmetric integrals -- Pitzer theorem in action
    {
      for (int i = 0; i<ni; i++) {
        for (int j = 0; j<rank3; j++) {
          int ij = i*rank3+j;
          int ij_local = ij/nproc;
          if (ij%nproc == me) {
            const int ij_sym = mosym1[i+i_offset] ^ mosym3[j];
            for(int te_type=0; te_type<num_te_types_; te_type++) {
              double* ijxy_ptr = (double*) ((size_t)integral_ijxy + (ij_local*num_te_types_+te_type)*memgrp_blocksize);
              for (int x = 0; x<rank2; x++) {
                const int ijx_sym = ij_sym ^ mosym2[x];
                for (int y = 0; y<rank4; y++, ijxy_ptr++) {
                  if (ijx_sym ^ mosym4[y]) {
                    *ijxy_ptr = 0.0;
                  }
                }
              }
            }
          }
        }
      }
    }
    // Sync up tasks before integrals are committed
    mem_->sync();

#if PRINT4Q
    {
      for(int te_type=0; te_type<PRINT_NUM_TE_TYPES; te_type++) {
        for (int i = 0; i<ni; i++) {
          for (int x = 0; x<rank2; x++) {
            for (int j = 0; j<rank3; j++) {
              int ij = i*rank3+j;
              int ij_local = ij/nproc;
              if (ij%nproc == me) {
                const double* ijxy_ints = (const double*)((size_t)integral_ijxy + (ij_local*num_te_types_+te_type)*memgrp_blocksize);
                for (int y = 0; y<rank4; y++) {
                  double value = ijxy_ints[x*rank4+y];
                  printf("4Q: type = %d (%d %d|%d %d) = %12.8f\n",
                         te_type,i+i_offset,x,j,y,value);
                }
              }
            }
          }
        }
      }
    }
#endif

    // Push locally stored integrals to an accumulator
    // This could involve storing the data to disk or simply remembering the pointer
    tim_enter("MO ints store");
    ints_acc_->store_memorygrp(mem_,ni,memgrp_blocksize);
    tim_exit("MO ints store");
    mem_->sync();

    if (me == 0 && top_mole_.nonnull() && top_mole_->if_to_checkpoint() && ints_acc_->can_restart()) {
      StateOutBin stateout(top_mole_->checkpoint_file());
      SavableState::save_state(top_mole_,stateout);
      ExEnv::out0() << indent << "Checkpointed the wave function" << endl;
    }

  } // end of loop over passes
  tim_exit("mp2-r12/a passes");
  if (debug_)
    ExEnv::out0() << indent << "End of mp2-r12/a transformation" << endl;
  // Done storing integrals - commit the content
  // WARNING: it is not safe to use mem until deactivate has been called on the accumulator
  //          After that deactivate the size of mem will be 0 [mem->set_localsize(0)]
  ints_acc_->commit();

  
  for (int i=0; i<thr_->nthread(); i++) {
    delete e13thread[i];
  }
  delete[] e13thread;
  delete[] tbints; tbints = 0;
  delete[] vector2[0]; delete[] vector2;
  delete[] vector4[0]; delete[] vector4;

  tim_exit(tim_label.c_str());

  if (me == 0 && top_mole_.nonnull() && top_mole_->if_to_checkpoint()) {
    StateOutBin stateout(top_mole_->checkpoint_file());
    SavableState::save_state(top_mole_,stateout);
    ExEnv::out0() << indent << "Checkpointed the wave function" << endl;
  }

  print_footer();

#if CHECK_INTS_SYMM
  ExEnv::out0() << indent << "Detecting non-totally-symmetric integrals ... ";
  check_int_symm();
  ExEnv::out0() << "none" << endl;
#endif

}


////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End:
