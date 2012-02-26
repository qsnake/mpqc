//
// energy.cc
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Curtis Janssen <cljanss@limitpt.com>
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

#include <stdlib.h>
#include <math.h>
#include <stdexcept>
#include <util/misc/string.h>

#include <util/class/scexception.h>
#include <util/misc/formio.h>
#include <util/state/stateio.h>
#include <math/scmat/local.h>
#include <util/keyval/keyval.h>
#include <chemistry/molecule/energy.h>

using namespace std;
using namespace sc;

/////////////////////////////////////////////////////////////////
// MolecularEnergy

static ClassDesc MolecularEnergy_cd(
  typeid(MolecularEnergy),"MolecularEnergy",6,"public Function",
  0, 0, 0);

MolecularEnergy::MolecularEnergy(const MolecularEnergy& mole):
  Function(mole)
{
  print_molecule_when_changed_ = mole.print_molecule_when_changed_;
  mc_ = mole.mc_;
  moldim_ = mole.moldim_;
  mol_ = mole.mol_;
  initial_pg_ = new PointGroup(mol_->point_group());
  ckpt_ = mole.ckpt_;
  ckpt_file_ = strdup(mole.ckpt_file_);
  ckpt_freq_ = mole.ckpt_freq_;
}

MolecularEnergy::MolecularEnergy(const Ref<KeyVal>&keyval):
  Function(keyval,1.0e-6,1.0e-6,1.0e-4)
{
  // The following code is a solaris workshop 5.0 hack, since it doesn't
  // seem to pass the right arguments in the Function CTOR.  This code can
  // be deleted with other C++ compilers.
  KeyValValuedouble funcaccval(1.0e-6);
  value_.set_desired_accuracy(keyval->doublevalue("value_accuracy",
                                                  funcaccval));
  if (value_.desired_accuracy() < DBL_EPSILON)
    value_.set_desired_accuracy(DBL_EPSILON);
  KeyValValuedouble gradaccval(1.0e-6);
  gradient_.set_desired_accuracy(keyval->doublevalue("gradient_accuracy",
                                                     gradaccval));
  if (gradient_.desired_accuracy() < DBL_EPSILON)
    gradient_.set_desired_accuracy(DBL_EPSILON);
  KeyValValuedouble hessaccval(1.0e-4);
  hessian_.set_desired_accuracy(keyval->doublevalue("hessian_accuracy",
                                                    hessaccval));
  if (hessian_.desired_accuracy() < DBL_EPSILON)
    hessian_.set_desired_accuracy(DBL_EPSILON);
  // end of solaris workshop 5.0 hack

  print_molecule_when_changed_
      = keyval->booleanvalue("print_molecule_when_changed");
  if (keyval->error() != KeyVal::OK) print_molecule_when_changed_ = 1;

  mol_ << keyval->describedclassvalue("molecule");
  if (mol_.null()) {
      throw InputError("missing required input of type Molecule",
                       __FILE__, __LINE__, "molecule", 0,
                       class_desc());
    }

  initial_pg_ = new PointGroup(mol_->point_group());

  hess_ << keyval->describedclassvalue("hessian");

  guesshess_ << keyval->describedclassvalue("guess_hessian");

  moldim_ = new SCDimension(3 * mol_->natom(), "3Natom");

  // the molecule coordinate object needs moldim_
  // so constract a keyval that has it
  Ref<AssignedKeyVal> assignedkeyval = new AssignedKeyVal;
  Ref<DescribedClass> dc = moldim_.pointer();
  assignedkeyval->assign("natom3", dc);
  dc = matrixkit();
  assignedkeyval->assign("matrixkit", dc);
  Ref<KeyVal> asskeyval(assignedkeyval.pointer());
  Ref<KeyVal> aggkeyval = new AggregateKeyVal(asskeyval, keyval);

  // Don't bother with internal coordinates if there is only 1 atom
  if (mol_->natom() > 1) {
      mc_ << aggkeyval->describedclassvalue("coor");
    }

  RefSCDimension dim;
  if (mc_.null()) {
      dim = moldim_;
    }
  else {
      dim = mc_->dim();
    }
  set_dimension(dim);

  ckpt_ = keyval->booleanvalue("checkpoint");
  if (keyval->error() != KeyVal::OK) ckpt_ = false;
  ckpt_file_ = keyval->pcharvalue("checkpoint_file");
  if (keyval->error() != KeyVal::OK) {
    char* filename = SCFormIO::fileext_to_filename(".wfn.ckpt");
    ckpt_file_ = strdup(filename);
    delete[] filename;
  }
  ckpt_freq_ = keyval->intvalue("checkpoint_freq");
  if (keyval->error() != KeyVal::OK) {
    ckpt_freq_ = 1;
  }
  
  do_value(1);
  do_gradient(0);
  do_hessian(0);

  molecule_to_x();
}

MolecularEnergy::~MolecularEnergy()
{
  if (ckpt_file_) free(ckpt_file_);
  ckpt_file_ = 0;
}

MolecularEnergy::MolecularEnergy(StateIn&s):
  SavableState(s),
  Function(s)
{
  mc_ << SavableState::restore_state(s);
  moldim_ << SavableState::restore_state(s);
  mol_ << SavableState::restore_state(s);
  if (s.version(::class_desc<MolecularEnergy>()) >= 2)
      s.get(print_molecule_when_changed_);
  else print_molecule_when_changed_ = 1;
  if (s.version(::class_desc<MolecularEnergy>()) >= 3) {
      hess_ << SavableState::restore_state(s);
      guesshess_ << SavableState::restore_state(s);
    }
  if (s.version(::class_desc<MolecularEnergy>()) >= 4)
      initial_pg_ << SavableState::restore_state(s);
  else initial_pg_ = new PointGroup(mol_->point_group());
  if (s.version(::class_desc<MolecularEnergy>()) >= 5) {
    int ckpt; s.get(ckpt); ckpt_ = (bool)ckpt;
    s.getstring(ckpt_file_);
  }
  else {
    ckpt_ = false;
    char* filename = SCFormIO::fileext_to_filename(".wfn.ckpt");
    ckpt_file_ = strdup(filename);
    delete[] filename;
  }
  if (s.version(::class_desc<MolecularEnergy>()) >= 6)
    s.get(ckpt_freq_);
  else
    ckpt_freq_ = 1;
}

MolecularEnergy&
MolecularEnergy::operator=(const MolecularEnergy& mole)
{
  Function::operator=(mole);
  mc_ = mole.mc_;
  moldim_ = mole.moldim_;
  mol_ = mole.mol_;
  print_molecule_when_changed_ = mole.print_molecule_when_changed_;
  initial_pg_ = new PointGroup(mole.initial_pg_);
  return *this;
}

void
MolecularEnergy::save_data_state(StateOut&s)
{
  Function::save_data_state(s);
  SavableState::save_state(mc_.pointer(),s);
  SavableState::save_state(moldim_.pointer(),s);
  SavableState::save_state(mol_.pointer(),s);
  s.put(print_molecule_when_changed_);
  SavableState::save_state(hess_.pointer(),s);
  SavableState::save_state(guesshess_.pointer(),s);
  SavableState::save_state(initial_pg_.pointer(),s);
  s.put((int)ckpt_);
  s.putstring(ckpt_file_);
  s.put(ckpt_freq_);
}

void
MolecularEnergy::set_checkpoint()
{
  ckpt_ = true;
}

void
MolecularEnergy::set_checkpoint_file(const char *path)
{
  if (ckpt_file_) free(ckpt_file_);
  if (path) {
    ckpt_file_ = strdup(path);
  } else
    ckpt_file_ = 0;
}

void
MolecularEnergy::set_checkpoint_freq(int freq)
{
  if (freq >= 1)
    ckpt_freq_ = freq;
  else
    throw std::runtime_error("MolecularEnergy::set_checkpoint_freq() -- invalid checkpointing frequency");
}

bool
MolecularEnergy::if_to_checkpoint() const
{
  return ckpt_;
}

const char*
MolecularEnergy::checkpoint_file() const
{
  return strdup(ckpt_file_);
}

int
MolecularEnergy::checkpoint_freq() const
{
  return ckpt_freq_;
}

void
MolecularEnergy::failure(const char * msg)
{
  throw SCException(msg, __FILE__, __LINE__, class_desc());
}

void
MolecularEnergy::set_energy(double e)
{
  set_value(e);
}

double
MolecularEnergy::energy()
{
  return value();
}

void
MolecularEnergy::set_gradient(RefSCVector&g)
{
  cartesian_gradient_ = g.copy();
  if (mc_.null()) {
    Function::set_gradient(g);
  } else {
    RefSCVector grad(dimension(), matrixkit());
    mc_->to_internal(grad,g);
    Function::set_gradient(grad);
  }
}

void
MolecularEnergy::set_hessian(RefSymmSCMatrix&h)
{
  cartesian_hessian_ = h.copy();
  if (mc_.null()) {
    Function::set_hessian(h);
  } else {
    RefSymmSCMatrix hess(dimension(), matrixkit());
    mc_->to_internal(hess,h);
    Function::set_hessian(hess);
  }
}

void
MolecularEnergy::x_to_molecule()
{
  RefSCVector x = get_x_no_copy();

  if (mc_.null()) {
    int c = 0;
    
    for (int i=0; i<mol_->natom(); i++) {
      mol_->r(i,0) = x(c); c++;
      mol_->r(i,1) = x(c); c++;
      mol_->r(i,2) = x(c); c++;
    }
  } else {
    mc_->to_cartesian(get_x_no_copy());
  }
  mol_->cleanup_molecule(0.000001);
}

void
MolecularEnergy::molecule_to_x()
{
  if (mc_.null()) {
    RefSCVector cartesian(moldim(),matrixkit());
    int c = 0;
    for (int i=0; i < mol_->natom(); i++) {
      cartesian(c) = mol_->r(i,0); c++;
      cartesian(c) = mol_->r(i,1); c++;
      cartesian(c) = mol_->r(i,2); c++;
    }
    Function::set_x(cartesian);
  } else {
    mc_->to_internal(get_x_reference());
  }
}

void
MolecularEnergy::set_x(const RefSCVector&v)
{
  Function::set_x(v);
  x_to_molecule();
  if (print_molecule_when_changed_) {
      ExEnv::out0() << endl << indent << class_name()
           << ": changing atomic coordinates:" << endl;
      molecule()->print();
    }
}

RefSCVector
MolecularEnergy::get_cartesian_x()
{
  RefSCVector cartesian(moldim(),matrixkit());
  int c = 0;
  for (int i=0; i < mol_->natom(); i++) {
      cartesian(c) = mol_->r(i,0); c++;
      cartesian(c) = mol_->r(i,1); c++;
      cartesian(c) = mol_->r(i,2); c++;
    }
  return cartesian;
}

RefSCVector
MolecularEnergy::get_cartesian_gradient()
{
  gradient();
  if (cartesian_gradient_.null()) {
      throw ProgrammingError("get_cartesian_gradient(): not available",
                             __FILE__, __LINE__, class_desc());
    }
  return cartesian_gradient_;
}

RefSymmSCMatrix
MolecularEnergy::get_cartesian_hessian()
{
  hessian();
  if (cartesian_hessian_.null()) {
      throw ProgrammingError("get_cartesian_hessian(): not available",
                             __FILE__, __LINE__, class_desc());
    }
  return cartesian_hessian_;
}

RefSCDimension
MolecularEnergy::moldim() const
{
  return moldim_;
}

Ref<Molecule>
MolecularEnergy::molecule() const
{
  return mol_;
}

void
MolecularEnergy::guess_hessian(RefSymmSCMatrix&hessian)
{
  if (guesshess_.nonnull()) {
      int nullmole = (guesshess_->energy() == 0);
      this->reference();
      if (nullmole) guesshess_->set_energy(this);
      RefSymmSCMatrix xhess = guesshess_->cartesian_hessian();
      if (nullmole) guesshess_->set_energy(0);
      this->dereference();
      if (mc_.nonnull()) {
          mc_->to_internal(hessian, xhess);
        }
      else {
          hessian.assign(xhess);
        }
    }
  else if (mc_.nonnull()) {
      mc_->guess_hessian(hessian);
    }
  else {
      Function::guess_hessian(hessian);
    }
}

RefSymmSCMatrix
MolecularEnergy::inverse_hessian(RefSymmSCMatrix&hessian)
{
  if (mc_.nonnull()) {
      return mc_->inverse_hessian(hessian);
    }
  else {
      return Function::inverse_hessian(hessian);
    }
}

RefSymmSCMatrix
MolecularEnergy::hessian()
{
  if (hess_.null()) return hessian_.result();

  if (hessian_.computed()) return hessian_.result();

  int nullmole = (hess_->energy() == 0);
  this->reference();
  if (nullmole) hess_->set_energy(this);
  RefSymmSCMatrix xhess = hess_->cartesian_hessian();
  if (nullmole) hess_->set_energy(0);
  this->dereference();
  set_hessian(xhess);
  return hessian_.result();
}

int
MolecularEnergy::hessian_implemented() const
{
  return hess_.nonnull();
}

void
MolecularEnergy::symmetry_changed()
{
  obsolete();
}

Ref<NonlinearTransform>
MolecularEnergy::change_coordinates()
{
  if (!mc_) return 0;
  Ref<NonlinearTransform> t = mc_->change_coordinates();
  do_change_coordinates(t);
  return t;
}

void
MolecularEnergy::print_natom_3(const RefSCVector &v,
                               const char *title, ostream&o) const
{
  int precision = 10;
  int lwidth = precision + 4;
  int n = v.n()/3;
  if (title) {
    o << indent << title << endl;
    o << incindent;
  }
  for (int i=0,ii=0; i<n; i++) {
    std::string symbol(molecule()->atom_symbol(i));
    o << indent
      << scprintf("%4d %3s",
                  i+1,symbol.c_str());
    for (int j=0; j<3; j++,ii++) {
      o << scprintf(" % *.*f", lwidth,precision,double(v(ii)));
    }
    o << endl;
  }
  if (title) {
    o << decindent;
  }
  o.flush();
}

void
MolecularEnergy::print_natom_3(double **vn3,
                               const char *title, ostream&o) const
{
  int precision = 10;
  int lwidth = precision + 4;
  int n = molecule()->natom();
  if (title) {
    o << indent << title << endl;
    o << incindent;
  }
  for (int i=0; i<n; i++) {
    std::string symbol(molecule()->atom_symbol(i));
    o << indent
      << scprintf("%4d %3s",
                  i+1,symbol.c_str());
    for (int j=0; j<3; j++) {
      o << scprintf(" % *.*f", lwidth,precision,double(vn3[i][j]));
    }
    o << endl;
  }
  if (title) {
    o << decindent;
  }
  o.flush();
}

void
MolecularEnergy::print_natom_3(double *vn3,
                               const char *title, ostream&o) const
{
  int precision = 10;
  int lwidth = precision + 4;
  int n = molecule()->natom();
  if (title) {
    o << indent << title << endl;
    o << incindent;
  }
  for (int i=0; i<n; i++) {
    std::string symbol(molecule()->atom_symbol(i));
    o << indent
      << scprintf("%4d %3s",
                  i+1,symbol.c_str());
    for (int j=0; j<3; j++) {
      o << scprintf(" % *.*f", lwidth,precision,double(vn3[3*i+j]));
    }
    o << endl;
  }
  if (title) {
    o << decindent;
  }
  o.flush();
}

void
MolecularEnergy::print(ostream&o) const
{
  Function::print(o);
  if (mc_.nonnull()) {
      o << indent << "Molecular Coordinates:\n" << incindent;
      mc_->print(o);
      o << decindent;
    }
  else {
      o << indent << "Molecule:\n" << incindent;
      mol_->print(o);
      o << decindent << endl;
    }
}

/////////////////////////////////////////////////////////////////
// SumMolecularEnergy

static ClassDesc SumMolecularEnergy_cd(
  typeid(SumMolecularEnergy),"SumMolecularEnergy",1,"public MolecularEnergy",
  0, create<SumMolecularEnergy>, create<SumMolecularEnergy>);

SumMolecularEnergy::SumMolecularEnergy(const Ref<KeyVal> &keyval):
  MolecularEnergy(keyval)
{
  n_ = keyval->count("mole");
  mole_ = new Ref<MolecularEnergy>[n_];
  coef_ = new double[n_];
  for (int i=0; i<n_; i++) {
      mole_[i] << keyval->describedclassvalue("mole",i);
      coef_[i] = keyval->intvalue("coef",i);
      if (mole_[i].null()) {
          throw InputError("a mole is null",
                           __FILE__, __LINE__, "mole", 0, class_desc());
        }
      else if (mole_[i]->molecule()->natom() != molecule()->natom()) {
          throw InputError("a mole has the wrong number of atoms",
                           __FILE__, __LINE__, "mole", 0, class_desc());
        }
    }
}

SumMolecularEnergy::SumMolecularEnergy(StateIn&s):
  MolecularEnergy(s)
{
  s.get(n_);
  coef_ = new double[n_];
  mole_ = new Ref<MolecularEnergy>[n_];
  s.get_array_double(coef_,n_);
  for (int i=0; i<n_; i++) {
      mole_[i] << SavableState::restore_state(s);
    }
}

void
SumMolecularEnergy::save_data_state(StateOut&s)
{
  MolecularEnergy::save_data_state(s);
  s.put(n_);
  s.put_array_double(coef_,n_);
  for (int i=0; i<n_; i++) {
      SavableState::save_state(mole_[i].pointer(),s);
    }
}

SumMolecularEnergy::~SumMolecularEnergy()
{
  delete[] mole_;
  delete[] coef_;
}

int
SumMolecularEnergy::value_implemented() const
{
  for (int i=0; i<n_; i++) {
      if (!mole_[i]->value_implemented()) return 0;
    }
  return 1;
}

int
SumMolecularEnergy::gradient_implemented() const
{
  for (int i=0; i<n_; i++) {
      if (!mole_[i]->gradient_implemented()) return 0;
    }
  return 1;
}

int
SumMolecularEnergy::hessian_implemented() const
{
  for (int i=0; i<n_; i++) {
      if (!mole_[i]->hessian_implemented()) return 0;
    }
  return 1;
}

void
SumMolecularEnergy::set_x(const RefSCVector&v)
{
  MolecularEnergy::set_x(v);
  for (int i=0; i<n_; i++) {
      mole_[i]->set_x(v);
    }
}

void
SumMolecularEnergy::compute()
{
  int i;

  int *old_do_value = new int[n_];
  int *old_do_gradient = new int[n_];
  int *old_do_hessian = new int[n_];

  for (i=0; i<n_; i++)
      old_do_value[i] = mole_[i]->do_value(value_.compute());
  for (i=0; i<n_; i++)
      old_do_gradient[i]=mole_[i]->do_gradient(gradient_.compute());
  for (i=0; i<n_; i++)
      old_do_hessian[i] = mole_[i]->do_hessian(hessian_.compute());

  ExEnv::out0() << indent
       << "SumMolecularEnergy: compute" << endl;

  ExEnv::out0() << incindent;

  if (value_needed()) {
      double val = 0.0;
      for (i=0; i<n_; i++) {
          val += coef_[i] * mole_[i]->value();
        }
      ExEnv::out0() << endl << indent
           << "SumMolecularEnergy =" << endl;
      for (i=0; i<n_; i++) {
          ExEnv::out0() << indent
               << scprintf("  %c % 16.12f * % 16.12f",
                           (i==0?' ':'+'),
                           coef_[i], mole_[i]->value())
               << endl;
        }
      ExEnv::out0() << indent
           << scprintf("  = % 16.12f", val) << endl;
      set_energy(val);
    }
  if (gradient_needed()) {
      RefSCVector gradientvec = matrixkit()->vector(moldim());
      gradientvec->assign(0.0);
      for (i=0; i<n_; i++)
          gradientvec.accumulate(coef_[i] * mole_[i]->gradient());
      set_gradient(gradientvec);
    }
  if (hessian_needed()) {
      RefSymmSCMatrix hessianmat = matrixkit()->symmmatrix(moldim());
      hessianmat->assign(0.0);
      for (i=0; i<n_; i++)
          hessianmat.accumulate(coef_[i] * mole_[i]->hessian());
      set_hessian(hessianmat);
    }

  ExEnv::out0() << decindent;

  for (i=0; i<n_; i++) mole_[i]->do_value(old_do_value[i]);
  for (i=0; i<n_; i++) mole_[i]->do_gradient(old_do_gradient[i]);
  for (i=0; i<n_; i++) mole_[i]->do_hessian(old_do_hessian[i]);

  delete[] old_do_value;
  delete[] old_do_gradient;
  delete[] old_do_hessian;
}

/////////////////////////////////////////////////////////////////
// MolEnergyConvergence

static ClassDesc MolEnergyConvergence_cd(
  typeid(MolEnergyConvergence),"MolEnergyConvergence",3,"public Convergence",
  0, create<MolEnergyConvergence>, create<MolEnergyConvergence>);

MolEnergyConvergence::MolEnergyConvergence()
{
  set_defaults();
}

MolEnergyConvergence::MolEnergyConvergence(StateIn&s):
  SavableState(s),
  Convergence(s)
{
  if (s.version(::class_desc<MolEnergyConvergence>()) >= 2)
      s.get(cartesian_);
  if (s.version(::class_desc<MolEnergyConvergence>()) >= 3)
      mole_ << SavableState::restore_state(s);
}

MolEnergyConvergence::MolEnergyConvergence(const Ref<KeyVal>&keyval)
{
  mole_ << keyval->describedclassvalue("energy");
  if (mole_.null()) {
      throw InputError("required keyword missing",
                       __FILE__, __LINE__, "energy", 0,
                       class_desc());
    }

  cartesian_ = keyval->booleanvalue("cartesian");
  if (keyval->error() != KeyVal::OK) cartesian_ = 1;

  use_max_disp_ = keyval->exists("max_disp");
  use_max_grad_ = keyval->exists("max_grad");
  use_rms_disp_ = keyval->exists("rms_disp");
  use_rms_grad_ = keyval->exists("rms_grad");
  use_graddisp_ = keyval->exists("graddisp");
  if (use_max_disp_) max_disp_ = keyval->doublevalue("max_disp");
  if (use_max_grad_) max_grad_ = keyval->doublevalue("max_grad");
  if (use_rms_disp_) rms_disp_ = keyval->doublevalue("rms_disp");
  if (use_rms_grad_) rms_grad_ = keyval->doublevalue("rms_grad");
  if (use_graddisp_) graddisp_ = keyval->doublevalue("graddisp");

  if (!use_max_disp_ && !use_max_grad_
      && !use_rms_disp_ && !use_rms_grad_
      && !use_graddisp_) {
      set_defaults();
    }
}

MolEnergyConvergence::~MolEnergyConvergence()
{
}

void
MolEnergyConvergence::save_data_state(StateOut&s)
{
  Convergence::save_data_state(s);
  s.put(cartesian_);
  SavableState::save_state(mole_.pointer(),s);
}

void
MolEnergyConvergence::set_defaults()
{
  use_max_disp_ = 1;
  use_max_grad_ = 1;
  use_rms_disp_ = 0;
  use_rms_grad_ = 0;
  use_graddisp_ = 1;
  max_disp_ = 1.0e-4;
  max_grad_ = 1.0e-4;
  graddisp_ = 1.0e-4;
}

void
MolEnergyConvergence::get_x(const Ref<Function> &f)
{
  Ref<MolecularEnergy> m; m << f;
  if (cartesian_ && m.nonnull() && m->molecularcoor().nonnull()) {
      x_ = m->get_cartesian_x();
    }
  else {
      x_ = f->get_x();
    }
}


void
MolEnergyConvergence::set_nextx(const RefSCVector& x)
{
  if (cartesian_ && mole_.nonnull() && mole_->molecularcoor().nonnull()) {
      Ref<Molecule> mol = new Molecule(*(mole_->molecule().pointer()));
      mole_->molecularcoor()->to_cartesian(mol, x);
      nextx_ = mole_->matrixkit()->vector(mole_->moldim());
      int c = 0;
      for (int i=0; i < mol->natom(); i++) {
          nextx_(c) = mol->r(i,0); c++;
          nextx_(c) = mol->r(i,1); c++;
          nextx_(c) = mol->r(i,2); c++;
        }
    }
  else if (mole_.null()) {
      // this only happens after restoring state from old versions
      // of MolEnergyConvergence
      nextx_ = 0;
    }
  else {
      nextx_ = x.copy();
    }
}

void
MolEnergyConvergence::get_grad(const Ref<Function> &f)
{
  Ref<MolecularEnergy> m; m << f;
  if (cartesian_ && m.nonnull() && m->molecularcoor().nonnull()) {
      RefSCVector cartesian_grad = m->get_cartesian_gradient()->copy();
      if (m->molecularcoor()->nconstrained()) {
          // convert the gradient to internal coordinates and back
          // this will project out the fixed coordinates
          RefSCVector internal_grad(m->dimension(), m->matrixkit());
          m->molecularcoor()->to_internal(internal_grad,cartesian_grad);
          m->molecularcoor()->to_cartesian(cartesian_grad,internal_grad);
        }
      grad_ = cartesian_grad;
    }
  else {
      grad_ = f->gradient();
    }
}

int
MolEnergyConvergence::converged()
{
  return Convergence::converged();
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
