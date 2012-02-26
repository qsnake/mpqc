//
// hsosscf.cc --- implementation of the high-spin open shell SCF class
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Edward Seidl <seidl@janed.com>
// Maintainer: LPS
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

#ifdef __GNUC__
#pragma implementation
#endif

#include <math.h>

#include <util/misc/timer.h>
#include <util/misc/formio.h>
#include <util/state/stateio.h>

#include <math/scmat/block.h>
#include <math/scmat/blocked.h>
#include <math/scmat/blkiter.h>
#include <math/scmat/local.h>

#include <math/optimize/scextrapmat.h>

#include <chemistry/qc/basis/petite.h>

#include <chemistry/qc/scf/scflocal.h>
#include <chemistry/qc/scf/scfops.h>
#include <chemistry/qc/scf/effh.h>
#include <chemistry/qc/scf/hsosscf.h>

#include <chemistry/qc/scf/ltbgrad.h>
#include <chemistry/qc/scf/hsoshftmpl.h>

using namespace std;
using namespace sc;

///////////////////////////////////////////////////////////////////////////
// HSOSSCF

static ClassDesc HSOSSCF_cd(
  typeid(HSOSSCF),"HSOSSCF",2,"public SCF",
  0, 0, 0);

HSOSSCF::HSOSSCF(StateIn& s) :
  SavableState(s),
  SCF(s),
  cl_fock_(this),
  op_fock_(this)
{
  cl_fock_.result_noupdate() =
    basis_matrixkit()->symmmatrix(so_dimension());
  cl_fock_.restore_state(s);
  cl_fock_.result_noupdate().restore(s);
  
  op_fock_.result_noupdate() =
    basis_matrixkit()->symmmatrix(so_dimension());
  op_fock_.restore_state(s);
  op_fock_.result_noupdate().restore(s);
  
  s.get(user_occupations_);
  s.get(tndocc_);
  s.get(tnsocc_);
  s.get(nirrep_);
  s.get(ndocc_);
  s.get(nsocc_);
  if (s.version(::class_desc<HSOSSCF>()) >= 2) {
    s.get(initial_ndocc_);
    s.get(initial_nsocc_);
    most_recent_pg_ << SavableState::restore_state(s);
  } else {
    initial_ndocc_ = new int[nirrep_];
    memcpy(initial_ndocc_, ndocc_, sizeof(int)*nirrep_);
    initial_nsocc_ = new int[nirrep_];
    memcpy(initial_nsocc_, nsocc_, sizeof(int)*nirrep_);
  }

  // now take care of memory stuff
  init_mem(4);
}

HSOSSCF::HSOSSCF(const Ref<KeyVal>& keyval) :
  SCF(keyval),
  cl_fock_(this),
  op_fock_(this)
{
  int i;
  
  cl_fock_.compute()=0;
  cl_fock_.computed()=0;
  
  op_fock_.compute()=0;
  op_fock_.computed()=0;
  
  // calculate the total nuclear charge
  double Znuc=molecule()->nuclear_charge();

  // check to see if this is to be a charged molecule
  double charge = keyval->doublevalue("total_charge");
  int nelectrons = (int)(Znuc-charge+1.0e-4);

  // first let's try to figure out how many open shells there are
  if (keyval->exists("nsocc")) {
    tnsocc_ = keyval->intvalue("nsocc");
  } else if (keyval->exists("multiplicity")) {
    tnsocc_ = keyval->intvalue("multiplicity")-1;
  } else {
    // if there's an odd number of electrons, then do a doublet, otherwise
    // do a triplet
    if (nelectrons%2)
      tnsocc_=1;
    else
      tnsocc_=2;
  }
  
  // now do the same for the number of doubly occupied shells
  if (keyval->exists("ndocc")) {
    tndocc_ = keyval->intvalue("ndocc");
  } else {
    tndocc_ = (nelectrons-tnsocc_)/2;
    if ((nelectrons-tnsocc_)%2) {
      ExEnv::err0() << endl << indent
           << "HSOSSCF::init: Warning, there's a leftover electron.\n"
           << incindent << indent << "total_charge = " << charge << endl
           << indent << "total nuclear charge = " << Znuc << endl
           << indent << "ndocc_ = " << tndocc_ << endl
           << indent << "nsocc_ = " << tnsocc_ << endl << decindent;
    }
  }

  ExEnv::out0() << endl << indent
       << "HSOSSCF::init: total charge = " << Znuc-2*tndocc_-tnsocc_
       << endl << endl;

  nirrep_ = molecule()->point_group()->char_table().ncomp();

  ndocc_ = read_occ(keyval, "docc", nirrep_);
  nsocc_ = read_occ(keyval, "socc", nirrep_);
  if (ndocc_ && nsocc_) {
    user_occupations_=1;
    initial_ndocc_ = new int[nirrep_];
    memcpy(initial_ndocc_, ndocc_, sizeof(int)*nirrep_);
    initial_nsocc_ = new int[nirrep_];
    memcpy(initial_nsocc_, nsocc_, sizeof(int)*nirrep_);
  }
  else if (ndocc_ && !nsocc_ || !ndocc_ && nsocc_) {
    ExEnv::outn() << "ERROR: HSOSSCF: only one of docc and socc specified: "
                 << "give both or none" << endl;
    abort();
  }
  else {
    ndocc_=0;
    nsocc_=0;
    initial_ndocc_=0;
    initial_nsocc_=0;
    user_occupations_=0;
    set_occupations(0);
  }

  ExEnv::out0() << indent << "docc = [";
  for (i=0; i < nirrep_; i++)
    ExEnv::out0() << " " << ndocc_[i];
  ExEnv::out0() << " ]\n";

  ExEnv::out0() << indent << "socc = [";
  for (i=0; i < nirrep_; i++)
    ExEnv::out0() << " " << nsocc_[i];
  ExEnv::out0() << " ]\n";

  // check to see if this was done in SCF(keyval)
  if (!keyval->exists("maxiter"))
    maxiter_ = 100;

  if (!keyval->exists("level_shift"))
    level_shift_ = 0.25;

  // now take care of memory stuff
  init_mem(4);
}

HSOSSCF::~HSOSSCF()
{
  if (ndocc_) {
    delete[] ndocc_;
    ndocc_=0;
  }
  if (nsocc_) {
    delete[] nsocc_;
    nsocc_=0;
  }
  delete[] initial_ndocc_;
  delete[] initial_nsocc_;
}

void
HSOSSCF::save_data_state(StateOut& s)
{
  SCF::save_data_state(s);

  cl_fock_.save_data_state(s);
  cl_fock_.result_noupdate().save(s);
  
  op_fock_.save_data_state(s);
  op_fock_.result_noupdate().save(s);
  
  s.put(user_occupations_);
  s.put(tndocc_);
  s.put(tnsocc_);
  s.put(nirrep_);
  s.put(ndocc_,nirrep_);
  s.put(nsocc_,nirrep_);
  s.put(initial_ndocc_,nirrep_);
  s.put(initial_nsocc_,nirrep_);
  SavableState::save_state(most_recent_pg_.pointer(),s);
}

double
HSOSSCF::occupation(int ir, int i)
{
  if (i < ndocc_[ir]) return 2.0;
  else if (i < ndocc_[ir] + nsocc_[ir]) return 1.0;
  return 0.0;
}

double
HSOSSCF::alpha_occupation(int ir, int i)
{
  if (i < ndocc_[ir] + nsocc_[ir]) return 1.0;
  return 0.0;
}

double
HSOSSCF::beta_occupation(int ir, int i)
{
  if (i < ndocc_[ir]) return 1.0;
  return 0.0;
}

int
HSOSSCF::n_fock_matrices() const
{
  return 2;
}

RefSymmSCMatrix
HSOSSCF::fock(int n)
{
  if (n > 1) {
    ExEnv::err0() << indent
         << "HSOSSCF::fock: there are only two fock matrices, "
         << scprintf("but fock(%d) was requested\n",n);
    abort();
  }

  if (n==0)
    return cl_fock_.result();
  else
    return op_fock_.result();
}

int
HSOSSCF::spin_polarized()
{
  return 1;
}

void
HSOSSCF::print(ostream&o) const
{
  int i;
  
  SCF::print(o);
  o << indent << "HSOSSCF Parameters:\n" << incindent
    << indent << "charge = " << molecule()->nuclear_charge()
                                - 2*tndocc_ - tnsocc_ << endl
    << indent << "ndocc = " << tndocc_ << endl
    << indent << "nsocc = " << tnsocc_ << endl
    << indent << "docc = [";
  for (i=0; i < nirrep_; i++)
    o << " " << ndocc_[i];
  o << " ]" << endl;

  o << indent << "socc = [";
  for (i=0; i < nirrep_; i++)
    o << " " << nsocc_[i];
  o << " ]" << endl << decindent << endl;
}

//////////////////////////////////////////////////////////////////////////////

void
HSOSSCF::set_occupations(const RefDiagSCMatrix& ev)
{
  if (user_occupations_ || (initial_ndocc_ && initial_nsocc_ && ev.null())) {
    if (form_occupations(ndocc_, initial_ndocc_)
        &&form_occupations(nsocc_, initial_nsocc_)) {
      most_recent_pg_ = new PointGroup(molecule()->point_group());
      return;
    }
    delete[] ndocc_; ndocc_ = 0;
    delete[] nsocc_; nsocc_ = 0;
    ExEnv::out0() << indent
         << "HSOSSCF: WARNING: reforming occupation vectors from scratch"
         << endl;
  }
  
  if (nirrep_==1) {
    delete[] ndocc_;
    ndocc_=new int[1];
    ndocc_[0]=tndocc_;
    if (!initial_ndocc_) {
      initial_ndocc_=new int[1];
      initial_ndocc_[0]=tndocc_;
    }

    delete[] nsocc_;
    nsocc_=new int[1];
    nsocc_[0]=tnsocc_;
    if (!initial_nsocc_) {
      initial_nsocc_=new int[1];
      initial_nsocc_[0]=tnsocc_;
    }

    return;
  }
  
  int i,j;
  
  RefDiagSCMatrix evals;
  
  if (ev.null()) {
    initial_vector(0);
    evals = eigenvalues_.result_noupdate();
  }
  else
    evals = ev;

  // first convert evals to something we can deal with easily
  BlockedDiagSCMatrix *evalsb = require_dynamic_cast<BlockedDiagSCMatrix*>(evals,
                                                 "HSOSSCF::set_occupations");
  
  double **vals = new double*[nirrep_];
  for (i=0; i < nirrep_; i++) {
    int nf=oso_dimension()->blocks()->size(i);
    if (nf) {
      vals[i] = new double[nf];
      evalsb->block(i)->convert(vals[i]);
    } else {
      vals[i] = 0;
    }
  }

  // now loop to find the tndocc_ lowest eigenvalues and populate those
  // MO's
  int *newdocc = new int[nirrep_];
  memset(newdocc,0,sizeof(int)*nirrep_);

  for (i=0; i < tndocc_; i++) {
    // find lowest eigenvalue
    int lir=0,ln=0;
    double lowest=999999999;

    for (int ir=0; ir < nirrep_; ir++) {
      int nf=oso_dimension()->blocks()->size(ir);
      if (!nf)
        continue;
      for (j=0; j < nf; j++) {
        if (vals[ir][j] < lowest) {
          lowest=vals[ir][j];
          lir=ir;
          ln=j;
        }
      }
    }
    vals[lir][ln]=999999999;
    newdocc[lir]++;
  }

  int *newsocc = new int[nirrep_];
  memset(newsocc,0,sizeof(int)*nirrep_);

  for (i=0; i < tnsocc_; i++) {
    // find lowest eigenvalue
    int lir=0,ln=0;
    double lowest=999999999;

    for (int ir=0; ir < nirrep_; ir++) {
      int nf=oso_dimension()->blocks()->size(ir);
      if (!nf)
        continue;
      for (j=0; j < nf; j++) {
        if (vals[ir][j] < lowest) {
          lowest=vals[ir][j];
          lir=ir;
          ln=j;
        }
      }
    }
    vals[lir][ln]=999999999;
    newsocc[lir]++;
  }

  // get rid of vals
  for (i=0; i < nirrep_; i++)
    if (vals[i])
      delete[] vals[i];
  delete[] vals;

  if (!ndocc_) {
    ndocc_=newdocc;
    nsocc_=newsocc;
  } else if (most_recent_pg_.nonnull()
             && most_recent_pg_->equiv(molecule()->point_group())) {
    // test to see if newocc is different from ndocc_
    for (i=0; i < nirrep_; i++) {
      if (ndocc_[i] != newdocc[i]) {
        ExEnv::err0() << indent << "HSOSSCF::set_occupations:  WARNING!!!!\n"
             << incindent << indent
             << scprintf("occupations for irrep %d have changed\n",i+1)
             << indent
             << scprintf("ndocc was %d, changed to %d", ndocc_[i], newdocc[i])
             << endl << decindent;
      }
      if (nsocc_[i] != newsocc[i]) {
        ExEnv::err0() << indent << "HSOSSCF::set_occupations:  WARNING!!!!\n"
             << incindent << indent
             << scprintf("occupations for irrep %d have changed\n",i+1)
             << indent
             << scprintf("nsocc was %d, changed to %d", nsocc_[i], newsocc[i])
             << endl << decindent;
      }
    }

    memcpy(ndocc_,newdocc,sizeof(int)*nirrep_);
    memcpy(nsocc_,newsocc,sizeof(int)*nirrep_);
    delete[] newdocc;
    delete[] newsocc;
  }

  if (!initial_ndocc_
      || initial_pg_->equiv(molecule()->point_group())) {
    delete[] initial_ndocc_;
    initial_ndocc_ = new int[nirrep_];
    memcpy(initial_ndocc_,ndocc_,sizeof(int)*nirrep_);
  }

  if (!initial_nsocc_
      || initial_pg_->equiv(molecule()->point_group())) {
    delete[] initial_nsocc_;
    initial_nsocc_ = new int[nirrep_];
    memcpy(initial_nsocc_,nsocc_,sizeof(int)*nirrep_);
  }

  most_recent_pg_ = new PointGroup(molecule()->point_group());
}

void
HSOSSCF::symmetry_changed()
{
  SCF::symmetry_changed();
  cl_fock_.result_noupdate()=0;
  op_fock_.result_noupdate()=0;
  nirrep_ = molecule()->point_group()->char_table().ncomp();
  set_occupations(0);
}

//////////////////////////////////////////////////////////////////////////////
//
// scf things
//

void
HSOSSCF::init_vector()
{
  init_threads();

  // allocate storage for other temp matrices
  cl_dens_ = hcore_.clone();
  cl_dens_.assign(0.0);
  
  cl_dens_diff_ = hcore_.clone();
  cl_dens_diff_.assign(0.0);

  op_dens_ = hcore_.clone();
  op_dens_.assign(0.0);
  
  op_dens_diff_ = hcore_.clone();
  op_dens_diff_.assign(0.0);

  // gmat is in AO basis
  cl_gmat_ = basis()->matrixkit()->symmmatrix(basis()->basisdim());
  cl_gmat_.assign(0.0);

  op_gmat_ = cl_gmat_.clone();
  op_gmat_.assign(0.0);

  if (cl_fock_.result_noupdate().null()) {
    cl_fock_ = hcore_.clone();
    cl_fock_.result_noupdate().assign(0.0);
    op_fock_ = hcore_.clone();
    op_fock_.result_noupdate().assign(0.0);
  }

  // set up trial vector
  initial_vector(1);
    
  oso_scf_vector_ = oso_eigenvectors_.result_noupdate();
}

void
HSOSSCF::done_vector()
{
  done_threads();

  cl_gmat_ = 0;
  cl_dens_ = 0;
  cl_dens_diff_ = 0;
  op_gmat_ = 0;
  op_dens_ = 0;
  op_dens_diff_ = 0;

  oso_scf_vector_ = 0;
}

RefSymmSCMatrix
HSOSSCF::alpha_density()
{
  RefSymmSCMatrix dens1(so_dimension(), basis_matrixkit());
  RefSymmSCMatrix dens2(so_dimension(), basis_matrixkit());

  so_density(dens1, 2.0);
  so_density(dens2, 1.0);
  dens1.accumulate(dens2);
  dens2=0;

  return dens1;
}

RefSymmSCMatrix
HSOSSCF::beta_density()
{
  RefSymmSCMatrix dens(so_dimension(), basis_matrixkit());
  so_density(dens, 2.0);
  return dens;
}

void
HSOSSCF::reset_density()
{
  cl_gmat_.assign(0.0);
  cl_dens_diff_.assign(cl_dens_);

  op_gmat_.assign(0.0);
  op_dens_diff_.assign(op_dens_);
}

double
HSOSSCF::new_density()
{
  // copy current density into density diff and scale by -1.  later we'll
  // add the new density to this to get the density difference.
  cl_dens_diff_.assign(cl_dens_);
  cl_dens_diff_.scale(-1.0);

  op_dens_diff_.assign(op_dens_);
  op_dens_diff_.scale(-1.0);

  so_density(cl_dens_, 2.0);
  cl_dens_.scale(2.0);

  so_density(op_dens_, 1.0);

  cl_dens_.accumulate(op_dens_);
  
  cl_dens_diff_.accumulate(cl_dens_);
  op_dens_diff_.accumulate(op_dens_);

  Ref<SCElementScalarProduct> sp(new SCElementScalarProduct);
  cl_dens_diff_.element_op(sp.pointer(), cl_dens_diff_);
  
  double delta = sp->result();
  delta = sqrt(delta/i_offset(cl_dens_diff_.n()));

  return delta;
}

RefSymmSCMatrix
HSOSSCF::density()
{
  if (!density_.computed()) {
    RefSymmSCMatrix dens(so_dimension(), basis_matrixkit());
    RefSymmSCMatrix dens1(so_dimension(), basis_matrixkit());
    so_density(dens, 2.0);
    dens.scale(2.0);

    so_density(dens1, 1.0);
    dens.accumulate(dens1);
    dens1=0;
    
    density_ = dens;
    // only flag the density as computed if the calc is converged
    if (!value_needed()) density_.computed() = 1;
  }

  return density_.result_noupdate();
}

double
HSOSSCF::scf_energy()
{
  RefSymmSCMatrix t = cl_fock_.result_noupdate().copy();
  t.accumulate(hcore_);

  RefSymmSCMatrix go = op_fock_.result_noupdate().copy();
  go.scale(-1.0);
  go.accumulate(cl_fock_.result_noupdate());
  
  SCFEnergy *eop = new SCFEnergy;
  eop->reference();
  Ref<SCElementOp2> op = eop;
  t.element_op(op, cl_dens_);

  double cl_e = eop->result();
  
  eop->reset();
  go.element_op(op, op_dens_);
  double op_e = eop->result();

  op=0;
  eop->dereference();
  delete eop;

  return cl_e-op_e;
}

Ref<SCExtrapData>
HSOSSCF::extrap_data()
{
  Ref<SCExtrapData> data =
    new SymmSCMatrix2SCExtrapData(cl_fock_.result_noupdate(),
                                  op_fock_.result_noupdate());
  return data;
}

RefSymmSCMatrix
HSOSSCF::effective_fock()
{
  // use fock() instead of cl_fock_ just in case this is called from
  // someplace outside SCF::compute_vector()
  RefSymmSCMatrix mofock(oso_dimension(), basis_matrixkit());
  mofock.assign(0.0);

  RefSymmSCMatrix mofocko(oso_dimension(), basis_matrixkit());
  mofocko.assign(0.0);

  // use eigenvectors if oso_scf_vector_ is null
  if (oso_scf_vector_.null()) {
    mofock.accumulate_transform(eigenvectors(), fock(0),
                                SCMatrix::TransposeTransform);
    mofocko.accumulate_transform(eigenvectors(), fock(1),
                                 SCMatrix::TransposeTransform);
  } else {
    RefSCMatrix so_to_oso_tr = so_to_orthog_so().t();
    mofock.accumulate_transform(so_to_oso_tr * oso_scf_vector_, fock(0),
                                SCMatrix::TransposeTransform);
    mofocko.accumulate_transform(so_to_oso_tr * oso_scf_vector_, fock(1),
                                 SCMatrix::TransposeTransform);
  }

  Ref<SCElementOp2> op = new GSGeneralEffH(this);
  mofock.element_op(op, mofocko);

  return mofock;
}

/////////////////////////////////////////////////////////////////////////////

void
HSOSSCF::init_gradient()
{
  // presumably the eigenvectors have already been computed by the time
  // we get here
  oso_scf_vector_ = oso_eigenvectors_.result_noupdate();
}

void
HSOSSCF::done_gradient()
{
  cl_dens_=0;
  op_dens_=0;
  oso_scf_vector_ = 0;
}

/////////////////////////////////////////////////////////////////////////////

// MO lagrangian
//       c    o   v
//  c  |2*FC|2*FC|0|
//     -------------
//  o  |2*FC| FO |0|
//     -------------
//  v  | 0  |  0 |0|
//
RefSymmSCMatrix
HSOSSCF::lagrangian()
{
  RefSCMatrix so_to_oso_tr = so_to_orthog_so().t();

  RefSymmSCMatrix mofock(oso_dimension(), basis_matrixkit());
  mofock.assign(0.0);
  mofock.accumulate_transform(so_to_oso_tr * oso_scf_vector_,
                              cl_fock_.result_noupdate(),
                              SCMatrix::TransposeTransform);

  RefSymmSCMatrix mofocko(oso_dimension(), basis_matrixkit());
  mofocko.assign(0.0);
  mofocko.accumulate_transform(so_to_oso_tr * oso_scf_vector_,
                               op_fock_.result_noupdate(),
                               SCMatrix::TransposeTransform);

  mofock.scale(2.0);
  
  Ref<SCElementOp2> op = new MOLagrangian(this);
  mofock.element_op(op, mofocko);
  mofocko=0;

  // transform MO lagrangian to SO basis
  RefSymmSCMatrix so_lag(so_dimension(), basis_matrixkit());
  so_lag.assign(0.0);
  so_lag.accumulate_transform(so_to_oso_tr * oso_scf_vector_, mofock);
  
  // and then from SO to AO
  Ref<PetiteList> pl = integral()->petite_list();
  RefSymmSCMatrix ao_lag = pl->to_AO_basis(so_lag);

  ao_lag.scale(-1.0);

  return ao_lag;
}

RefSymmSCMatrix
HSOSSCF::gradient_density()
{
  cl_dens_ = basis_matrixkit()->symmmatrix(so_dimension());
  op_dens_ = cl_dens_.clone();
  
  so_density(cl_dens_, 2.0);
  cl_dens_.scale(2.0);
  
  so_density(op_dens_, 1.0);
  
  Ref<PetiteList> pl = integral()->petite_list(basis());
  
  cl_dens_ = pl->to_AO_basis(cl_dens_);
  op_dens_ = pl->to_AO_basis(op_dens_);

  RefSymmSCMatrix tdens = cl_dens_.copy();
  tdens.accumulate(op_dens_);

  op_dens_.scale(2.0);
  
  return tdens;
}

/////////////////////////////////////////////////////////////////////////////

void
HSOSSCF::init_hessian()
{
}

void
HSOSSCF::done_hessian()
{
}

/////////////////////////////////////////////////////////////////////////////

void
HSOSSCF::two_body_deriv_hf(double * tbgrad, double exchange_fraction)
{
  Ref<SCElementMaxAbs> m = new SCElementMaxAbs;
  cl_dens_.element_op(m.pointer());
  op_dens_.element_op(m.pointer());
  double pmax = m->result();
  m=0;

  // now try to figure out the matrix specialization we're dealing with.
  // if we're using Local matrices, then there's just one subblock, or
  // see if we can convert P to local matrices

  if (local_ || local_dens_) {
    // grab the data pointers from the P matrices
    double *pmat, *pmato;
    RefSymmSCMatrix ptmp = get_local_data(cl_dens_, pmat, SCF::Read);
    RefSymmSCMatrix potmp = get_local_data(op_dens_, pmato, SCF::Read);
  
    Ref<PetiteList> pl = integral()->petite_list();
    LocalHSOSGradContribution l(pmat,pmato);
    
    int i;
    int na3 = molecule()->natom()*3;
    int nthread = threadgrp_->nthread();
    double **grads = new double*[nthread];
    Ref<TwoBodyDerivInt> *tbis = new Ref<TwoBodyDerivInt>[nthread];
    for (i=0; i < nthread; i++) { 
      tbis[i] = integral()->electron_repulsion_deriv();
      grads[i] = new double[na3];
      memset(grads[i], 0, sizeof(double)*na3);
    }

    LocalTBGrad<LocalHSOSGradContribution> **tblds =
      new LocalTBGrad<LocalHSOSGradContribution>*[nthread];

    for (i=0; i < nthread; i++) {
      tblds[i] = new LocalTBGrad<LocalHSOSGradContribution>(
        l, tbis[i], pl, basis(), scf_grp_, grads[i], pmax,
        desired_gradient_accuracy(), nthread, i, exchange_fraction);
      threadgrp_->add_thread(i, tblds[i]);
    }

    if (threadgrp_->start_threads() < 0
        ||threadgrp_->wait_threads() < 0) {
      ExEnv::err0() << indent
           << "HSOSSCF: error running threads" << endl;
      abort();
    }

    for (i=0; i < nthread; i++) {
      for (int j=0; j < na3; j++)
        tbgrad[j] += grads[i][j];

      delete[] grads[i];
      delete tblds[i];
    }

    scf_grp_->sum(tbgrad, na3);
  }

  // for now quit
  else {
    ExEnv::err0() << indent
         << "HSOSSCF::two_body_deriv: can't do gradient yet\n";
    abort();
  }
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
