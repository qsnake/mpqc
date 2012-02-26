//
// density.cc
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

#include <stdexcept>

#include <util/misc/formio.h>
#include <util/render/polygons.h>
#include <math/scmat/local.h>
#include <math/scmat/vector3.h>
#include <chemistry/molecule/molecule.h>
#include <chemistry/qc/wfn/density.h>

using namespace std;
using namespace sc;

/////////////////////////////////////////////////////////////////////////////
// ElectronDensity

static ClassDesc ElectronDensity_cd(
  typeid(ElectronDensity),"ElectronDensity",1,"public Volume",
  0, create<ElectronDensity>, 0);

ElectronDensity::ElectronDensity(const Ref<KeyVal> &keyval):
  Volume(keyval)
{
  wfn_ << keyval->describedclassvalue("wfn");
}

ElectronDensity::ElectronDensity(const Ref<Wavefunction>& wfn):
  Volume(),
  wfn_(wfn)
{
}

ElectronDensity::~ElectronDensity()
{
}

void
ElectronDensity::compute()
{
  SCVector3 r;
  get_x(r);
  // do_gradient will automatically cause the value to be computed
  if (gradient_needed()) {
      double v[3];
      set_value(wfn_->density_gradient(r,v));
      set_actual_value_accuracy(desired_value_accuracy());
      SCVector3 d(v);
      set_gradient(d);
      set_actual_gradient_accuracy(desired_gradient_accuracy());
    }
  else if (value_needed()) {
      set_value(wfn_->density(r));
      set_actual_value_accuracy(desired_value_accuracy());
    }
  if (hessian_needed()) {
      ExEnv::err0() << indent
           << "ElectronDensity::compute(): hessian isn't yet implemented\n";
      abort();
    }
}

// make a wild guess about the bounding box
void
ElectronDensity::boundingbox(double valuemin,
                             double valuemax,
                             SCVector3& p1, SCVector3& p2)
{
  Molecule& mol = *wfn_->molecule();

  if (mol.natom() == 0) {
      for (int i=0; i<3; i++) p1[i] = p2[i] = 0.0;
    }

  int i;
  for (i=0; i<3; i++) p1[i] = p2[i] = mol.r(0,i);
  for (i=1; i<mol.natom(); i++) {
      for (int j=0; j<3; j++) {
          if (mol.r(i,j) < p1[j]) p1[j] = mol.r(i,j);
          if (mol.r(i,j) > p2[j]) p2[j] = mol.r(i,j);
        }
    }
  for (i=0; i<3; i++) {
      p1[i] = p1[i] - 3.0;
      p2[i] = p2[i] + 3.0;
    }
}

/////////////////////////////////////////////////////////////////////////////
// BatchElectronDensity

static ClassDesc BatchElectronDensity_cd(
  typeid(BatchElectronDensity),"BatchElectronDensity",1,"public Volume",
  0, create<BatchElectronDensity>, 0);

BatchElectronDensity::BatchElectronDensity(const Ref<Wavefunction> &wfn,
                                           double accuracy):
  Volume()
{
  wfn_ = wfn;
  accuracy_ = accuracy;
  zero_pointers();
  using_shared_data_ = false;
  linear_scaling_ = true;
  use_dmat_bound_ = true;
  need_basis_gradient_ = false;
  need_basis_hessian_ = false;
}

BatchElectronDensity::BatchElectronDensity(const Ref<KeyVal> &keyval):
  Volume(keyval)
{
  wfn_ << keyval->describedclassvalue("wfn");
  accuracy_ = keyval->doublevalue("accuracy");
  zero_pointers();
  using_shared_data_ = false;
  linear_scaling_ = true;
  use_dmat_bound_ = true;
  need_basis_gradient_ = false;
  need_basis_hessian_ = false;
}

BatchElectronDensity::BatchElectronDensity(const Ref<BatchElectronDensity>&d,
                                           bool reference_parent_data):
  Volume()
{
  wfn_ = d->wfn_;
  zero_pointers();
  using_shared_data_ = reference_parent_data;
  accuracy_ = d->accuracy_;
  need_basis_gradient_ = d->need_basis_gradient_;
  need_basis_hessian_ = d->need_basis_hessian_;
  if (using_shared_data_) {
      if (d->alpha_dmat_ == 0) {
          throw std::runtime_error("BatchElectronDensity: attempted to use shared data, but parent data not initialized");
        }

      spin_polarized_ = d->spin_polarized_;
      nshell_ = d->nshell_;
      nbasis_ = d->nbasis_;
      basis_ = d->basis_;
      extent_ = d->extent_;
      alpha_dmat_ = d->alpha_dmat_;
      beta_dmat_ = d->beta_dmat_;
      dmat_bound_ = d->dmat_bound_;
      linear_scaling_ = d->linear_scaling_;
      use_dmat_bound_ = d->use_dmat_bound_;

      init_scratch_data();
    }
}

BatchElectronDensity::~BatchElectronDensity()
{
  clear();
}

void
BatchElectronDensity::zero_pointers()
{
  valdat_ = 0;
  extent_ = 0;

  alpha_dmat_ = 0;
  beta_dmat_ = 0;
  dmat_bound_ = 0;
  contrib_ = 0;
  contrib_bf_ = 0;
  bs_values_ = 0;
  bsg_values_ = 0;
  bsh_values_ = 0;
}

void
BatchElectronDensity::clear()
{
  if (!using_shared_data_) {
      delete extent_;
      delete[] alpha_dmat_;
      delete[] beta_dmat_;
      delete[] dmat_bound_;
    }

  delete[] contrib_;
  delete[] contrib_bf_;
  delete[] bs_values_;
  delete[] bsg_values_;
  delete[] bsh_values_;
  delete valdat_;

  zero_pointers();
}

void
BatchElectronDensity::init(bool initialize_density_matrices)
{
  if (using_shared_data_)
      throw std::runtime_error("BatchElectronDensity::init: should not be called if using_shared_data");

  clear();
  init_common_data(initialize_density_matrices);
  init_scratch_data();
}

void
BatchElectronDensity::init_common_data(bool initialize_density_matrices)
{
  spin_polarized_ = wfn_->spin_polarized();
  nshell_ = wfn_->basis()->nshell();
  nbasis_ = wfn_->basis()->nbasis();
  basis_ = wfn_->basis();

  if (linear_scaling_) {
      extent_ = new ShellExtent;
      extent_->init(wfn_->basis());
    }

  alpha_dmat_ = new double[(nbasis_*(nbasis_+1))/2];

  beta_dmat_ = 0;
  if (spin_polarized_) {
      beta_dmat_ = new double[(nbasis_*(nbasis_+1))/2];
    }

  dmat_bound_ = new double[(nshell_*(nshell_+1))/2];

  if (initialize_density_matrices) {
      RefSymmSCMatrix beta_ao_density;
      if (spin_polarized_) beta_ao_density = wfn_->beta_ao_density();
      set_densities(wfn_->alpha_ao_density(), beta_ao_density);
    }
}

void
BatchElectronDensity::set_densities(const RefSymmSCMatrix &aden,
                                    const RefSymmSCMatrix &bden)
{
  RefSymmSCMatrix ad = aden;
  RefSymmSCMatrix bd = bden;
  if (ad.null()) ad = wfn_->alpha_ao_density();
  if (bd.null()) bd = wfn_->beta_ao_density();

  ad->convert(alpha_dmat_);
  if (spin_polarized_) bd->convert(beta_dmat_);

  int ij = 0;
  for (int i=0; i<nshell_; i++) {
      int ni = wfn_->basis()->shell(i).nfunction();
      for (int j=0; j<=i; j++,ij++) {
          int nj = wfn_->basis()->shell(j).nfunction();
          double bound = 0.0;
          int ibf = wfn_->basis()->shell_to_function(i);
          for (int k=0; k<ni; k++,ibf++) {
              int lmax = nj-1;
              if (i==j) lmax = k;
              int jbf = wfn_->basis()->shell_to_function(j);
              int ijbf = (ibf*(ibf+1))/2 + jbf;
              for (int l=0; l<=lmax; l++,ijbf++) {
                  double a = fabs(alpha_dmat_[ijbf]);
                  if (a > bound) bound = a;
                  if (beta_dmat_) {
                      double b = fabs(beta_dmat_[ijbf]);
                      if (b > bound) bound = b;
                    }
                }
            }
          dmat_bound_[ij] = bound;
        }
    }
}

void
BatchElectronDensity::init_scratch_data()
{
  contrib_ = new int[nshell_];
  contrib_bf_ = new int[nbasis_];
  bs_values_ = new double[nbasis_];
  bsg_values_ = new double[3*nbasis_];
  bsh_values_ = new double[6*nbasis_];
  valdat_ = new GaussianBasisSet::ValueData(basis_, wfn_->integral());
}

void
BatchElectronDensity::compute_basis_values(const SCVector3&r)
{
  // only consider those shells for which phi_i * (Max_j D_ij phi_j) > tol
  if (linear_scaling_ && use_dmat_bound_ && extent_ != 0) {
      const std::vector<ExtentData> &cs = extent_->contributing_shells(r[0],r[1],r[2]);
      ncontrib_ = 0;
      for (int i=0; i<cs.size(); i++) {
          int ish = cs[i].shell;
          int contrib = 0;
          for (int j=0; j<cs.size(); j++) {
              int jsh = cs[j].shell;
              int ijsh = (ish>jsh)?((ish*(ish+1))/2+jsh):((jsh*(jsh+1))/2+ish);
//               std::cout << "cs[i].bound = " << cs[i].bound << std::endl;
//               std::cout << "cs[j].bound = " << cs[j].bound << std::endl;
//               std::cout << "dmat_bound_[ijsh] = " << dmat_bound_[ijsh] << std::endl;
//               std::cout << "accuracy_ = " << accuracy_ << std::endl;
              if (cs[i].bound*cs[j].bound*dmat_bound_[ijsh] > 0.00001*accuracy_) {
                  contrib = 1;
                  break;
                }
            }
          if (contrib) {
              contrib_[ncontrib_++] = ish;
            }
        }
    }
  else if (linear_scaling_ && extent_ != 0) {
      const std::vector<ExtentData> &cs = extent_->contributing_shells(r[0],r[1],r[2]);
      ncontrib_ = cs.size();
      for (int i=0; i<ncontrib_; i++) {
          contrib_[i] = cs[i].shell;
        }
    }
  else {
      ncontrib_ = nshell_;
      for (int i=0; i<nshell_; i++) contrib_[i] = i;
    }

  ncontrib_bf_ = 0;
  for (int i=0; i<ncontrib_; i++) {
      int nbf = basis_->shell(contrib_[i]).nfunction();
      int bf = basis_->shell_to_function(contrib_[i]);
      for (int j=0; j<nbf; j++, bf++) {
          contrib_bf_[ncontrib_bf_++] = bf;
        }
    }

  // compute the basis set values
  double *bsv = bs_values_;
  double *bsg = ((need_basis_gradient_||need_gradient_)?bsg_values_:0);
  double *bsh = ((need_basis_hessian_||need_hessian_)?bsh_values_:0);
  for (int i=0; i<ncontrib_; i++) {
      basis_->hessian_shell_values(r,contrib_[i],valdat_,bsh,bsg,bsv);
      int shsize = basis_->shell(contrib_[i]).nfunction();

      if (bsh) bsh += 6 * shsize;
      if (bsg) bsg += 3 * shsize;
      if (bsv) bsv += shsize;
    }
}

void
BatchElectronDensity::compute_spin_density(const double *dmat,
                                           double *rho,
                                           double *grad,
                                           double *hess)
{
  int i, j;

  double tmp = 0.0;
  double densij;
  double bvi, bvix, bviy, bviz;
  double bvixx, bviyx, bviyy, bvizx, bvizy, bvizz;

  if (need_gradient_) for (i=0; i<3; i++) grad[i] = 0.0;
  if (need_hessian_) for (i=0; i<6; i++) hess[i] = 0.0;

  if (need_gradient_ || need_hessian_) {
      for (i=0; i < ncontrib_bf_; i++) {
          int it = contrib_bf_[i];
          bvi = bs_values_[i];
          if (need_gradient_) {
              bvix = bsg_values_[i*3+X];
              bviy = bsg_values_[i*3+Y];
              bviz = bsg_values_[i*3+Z];
            }
          if (need_hessian_) {
              bvixx = bsh_values_[i*6+XX];
              bviyx = bsh_values_[i*6+YX];
              bviyy = bsh_values_[i*6+YY];
              bvizx = bsh_values_[i*6+ZX];
              bvizy = bsh_values_[i*6+ZY];
              bvizz = bsh_values_[i*6+ZZ];
            }
          int j3 = 0, j6 = 0;
          int itoff = (it*(it+1))>>1;
          int itjt;
          double t = 0.0;
          for (j=0; j < i; j++) {
              int jt = contrib_bf_[j];
              itjt = itoff+jt;

              densij = dmat[itjt];
              double bvj = bs_values_[j];

              t += densij*bvi*bvj;

              double bvjx, bvjy, bvjz;
              if (need_gradient_) {
                  bvjx = bsg_values_[j3+X];
                  bvjy = bsg_values_[j3+Y];
                  bvjz = bsg_values_[j3+Z];
                  grad[X] += densij*(bvi*bvjx + bvj*bvix);
                  grad[Y] += densij*(bvi*bvjy + bvj*bviy);
                  grad[Z] += densij*(bvi*bvjz + bvj*bviz);
                  j3 += 3;
                }

              if (need_hessian_) {
                  double bvjxx = bsh_values_[j6+XX];
                  double bvjyx = bsh_values_[j6+YX];
                  double bvjyy = bsh_values_[j6+YY];
                  double bvjzx = bsh_values_[j6+ZX];
                  double bvjzy = bsh_values_[j6+ZY];
                  double bvjzz = bsh_values_[j6+ZZ];

                  hess[XX] += densij*(bvi*bvjxx+bvix*bvjx+bvjx*bvix+bvixx*bvj);
                  hess[YX] += densij*(bvi*bvjyx+bviy*bvjx+bvjy*bvix+bviyx*bvj);
                  hess[YY] += densij*(bvi*bvjyy+bviy*bvjy+bvjy*bviy+bviyy*bvj);
                  hess[ZX] += densij*(bvi*bvjzx+bviz*bvjx+bvjz*bvix+bvizx*bvj);
                  hess[ZY] += densij*(bvi*bvjzy+bviz*bvjy+bvjz*bviy+bvizy*bvj);
                  hess[ZZ] += densij*(bvi*bvjzz+bviz*bvjz+bvjz*bviz+bvizz*bvj);

                  j6 += 6;
                }
            }
          densij = dmat[itoff+it]*bvi;
          tmp += t + 0.5*densij*bvi;
          if (need_gradient_) {
              grad[X] += densij*bvix;
              grad[Y] += densij*bviy;
              grad[Z] += densij*bviz;
            }
          if (need_hessian_) {
              hess[XX] += densij*bvixx;
              hess[YX] += densij*bviyx;
              hess[YY] += densij*bviyy;
              hess[ZX] += densij*bvizx;
              hess[ZY] += densij*bvizy;
              hess[ZZ] += densij*bvizz;
            }
        }
    }
  else {
      for (i=0; i < ncontrib_bf_; i++) {
          int it = contrib_bf_[i];
          bvi = bs_values_[i];
          int itoff = (it*(it+1))>>1;
          int itjt;
          double t = 0.0;
          for (j=0; j < i; j++) {
              int jt = contrib_bf_[j];
              itjt = itoff+jt;

              densij = dmat[itjt];
              double bvj = bs_values_[j];

              t += densij*bvi*bvj;
            }
          densij = dmat[itoff+it]*bvi;
          tmp += t + 0.5*densij*bvi;
        }
    }
  if (rho!=0) *rho = tmp;

}

void
BatchElectronDensity::compute_density(const SCVector3 &r,
                                      double *adens,
                                      double *agrad,
                                      double *ahess,
                                      double *bdens,
                                      double *bgrad,
                                      double *bhess)
{
  if (alpha_dmat_ == 0) init();

  need_gradient_ = (agrad!=0) || (bgrad!=0);
  need_hessian_ = (ahess!=0) || (bhess!=0);

  compute_basis_values(r);

  compute_spin_density(alpha_dmat_,
                       adens,
                       agrad,
                       ahess);

  bool mismatch = (adens==0 && bdens!=0)
                  ||(agrad==0 && bgrad!=0)
                  ||(ahess==0 && bhess!=0);

  if (spin_polarized_ || mismatch) {
      compute_spin_density(beta_dmat_,
                           bdens,
                           bgrad,
                           bhess);
    }
  else {
      if (bdens!=0) *bdens = *adens;
      if (bgrad!=0)
          for (int i=0;i<3;i++) bgrad[i] = agrad[i];
      if (bhess!=0)
          for (int i=0;i<6;i++) bhess[i] = ahess[i];
    }

  if (adens!=0) *adens *= 2.0;
  if (agrad!=0)
      for (int i=0;i<3;i++) agrad[i] *= 2.0;
  if (ahess!=0)
      for (int i=0;i<6;i++) ahess[i] *= 2.0;
  if (bdens!=0) *bdens *= 2.0;
  if (bgrad!=0)
      for (int i=0;i<3;i++) bgrad[i] *= 2.0;
  if (bhess!=0)
      for (int i=0;i<6;i++) bhess[i] *= 2.0;

//   if (agrad) {
//       cout << scprintf("compute_density: agrad = %12.8f %12.8f %12.8f",
//                        agrad[0], agrad[1], agrad[2])
//            << endl;
//     }

//   cout << "compute_density: exiting"
//        << std::endl;

}

void
BatchElectronDensity::compute()
{
  SCVector3 r;
  get_x(r);

  double val;
  double grad[3];
  double hess[6];
  double aval;
  double agrad[3];
  double ahess[6];
  double bval;
  double bgrad[3];
  double bhess[6];
  compute_density(r,
                  &aval,
                  (gradient_needed()?agrad:0),
                  (hessian_needed()?ahess:0),
                  &bval,
                  (gradient_needed()?bgrad:0),
                  (hessian_needed()?bhess:0));
  val = aval + bval;
  for (int i=0; i<3; i++) grad[i] = agrad[i] + bgrad[i];
  for (int i=0; i<6; i++) hess[i] = ahess[i] + bhess[i];

  if (value_needed()) {
      set_value(val);
      set_actual_value_accuracy(desired_value_accuracy());
    }
  if (gradient_needed()) {
      set_value(val);
      set_actual_value_accuracy(desired_value_accuracy());
      SCVector3 d(grad);
      set_gradient(d);
      set_actual_gradient_accuracy(desired_gradient_accuracy());
    }
  if (hessian_needed()) {
      ExEnv::err0() << indent
           << "BatchElectronDensity::compute(): hessian isn't yet implemented\n";
      abort();
    }
}

void
BatchElectronDensity::boundingbox(double valuemin,
                             double valuemax,
                             SCVector3& p1, SCVector3& p2)
{
#if 0
  // this is a very conservative bounding box
  // also, this code is not correct since extent is not
  // necessarily initialized
  if (alpha_dmat_ == 0) init();
  for (int i=0; i<3; i++) p1[i] = extent_->lower(i);
  for (int i=0; i<3; i++) p2[i] = extent_->upper(i);
#else
  Molecule& mol = *wfn_->molecule();

  if (mol.natom() == 0) {
      for (int i=0; i<3; i++) p1[i] = p2[i] = 0.0;
    }

  int i;
  for (i=0; i<3; i++) p1[i] = p2[i] = mol.r(0,i);
  for (i=1; i<mol.natom(); i++) {
      for (int j=0; j<3; j++) {
          if (mol.r(i,j) < p1[j]) p1[j] = mol.r(i,j);
          if (mol.r(i,j) > p2[j]) p2[j] = mol.r(i,j);
        }
    }
  for (i=0; i<3; i++) {
      p1[i] = p1[i] - 3.0;
      p2[i] = p2[i] + 3.0;
    }
#endif
}

/////////////////////////////////////////////////////////////////////////////
// DensityColorizer

static ClassDesc DensityColorizer_cd(
  typeid(DensityColorizer),"DensityColorizer",1,"public MoleculeColorizer",
  0, create<DensityColorizer>, 0);

DensityColorizer::DensityColorizer(const Ref<KeyVal>&keyval):
  MoleculeColorizer(keyval)
{
  wfn_ << keyval->describedclassvalue("wfn");
  reference_ = keyval->doublevalue("reference");
  if (keyval->error() == KeyVal::OK) have_reference_ = 1;
  else have_reference_ = 0;
  scale_ = keyval->doublevalue("scale");
  if (keyval->error() == KeyVal::OK) have_scale_ = 1;
  else have_scale_ = 0;
}

DensityColorizer::~DensityColorizer()
{
}

void
DensityColorizer::colorize(const Ref<RenderedPolygons> &poly)
{
  const double base = 0.3;
  int i;
  int nvertex = poly->nvertex();

  if (nvertex) {
      double *data = new double[nvertex];

      for (i=0; i<nvertex; i++) {
          SCVector3 v(poly->vertex(i));
          data[i] = wfn_->density(v);
        }

      double min = data[0], max = data[0];
      for (i=1; i<nvertex; i++) {
          if (min > data[i]) min = data[i];
          if (max < data[i]) max = data[i];
        }

      double center, scale;

      if (have_reference_) center = reference_;
      else center = (max+min)/2.0; 

      double maxdiff = fabs(max - center);
      double mindiff = fabs(min - center);

      if (have_scale_) {
          scale = scale_;
        }
      else {
          if (maxdiff>mindiff && maxdiff>1.0e-6) scale = (1.0-base)/maxdiff;
          else if (mindiff>1.0e-6) scale = (1.0-base)/mindiff;
          else scale = (1.0-base);
        }

      ExEnv::out0() << indent << "DensityColorizer:"
           << scprintf(" reference=%6.5f", center)
           << scprintf(" scale=%8.4f",scale)
           << scprintf(" (%6.5f<=rho<=%6.5f)", max, min)
           << endl;
      for (i=0; i<nvertex; i++) {
          data[i] = (data[i]-center)*scale;
        }

      for (i=0; i<nvertex; i++) {
          Color c;
          if (data[i] < 0.0) c.set_rgb(-data[i]+base,0.3,0.3);
          else c.set_rgb(0.3,0.3,data[i]+base);
          poly->set_vertex_color(i,c);
        }

      delete[] data;
    }
}

/////////////////////////////////////////////////////////////////////////////
// GradDensityColorizer

static ClassDesc GradDensityColorizer_cd(
  typeid(GradDensityColorizer),"GradDensityColorizer",1,"public MoleculeColorizer",
  0, create<GradDensityColorizer>, 0);

GradDensityColorizer::GradDensityColorizer(const Ref<KeyVal>&keyval):
  MoleculeColorizer(keyval)
{
  wfn_ << keyval->describedclassvalue("wfn");
  reference_ = keyval->doublevalue("reference");
  if (keyval->error() == KeyVal::OK) have_reference_ = 1;
  else have_reference_ = 0;
  scale_ = keyval->doublevalue("scale");
  if (keyval->error() == KeyVal::OK) have_scale_ = 1;
  else have_scale_ = 0;
}

GradDensityColorizer::~GradDensityColorizer()
{
}

void
GradDensityColorizer::colorize(const Ref<RenderedPolygons> &poly)
{
  const double base = 0.3;
  int i;
  int nvertex = poly->nvertex();

  Ref<BatchElectronDensity> den = new BatchElectronDensity(wfn_);

  if (nvertex) {
      double *data = new double[nvertex];

      for (i=0; i<nvertex; i++) {
          SCVector3 v(poly->vertex(i));
          SCVector3 g;
          den->set_x(v);
          den->get_gradient(g);
          data[i] = g.norm();
        }

      double min = data[0], max = data[0];
      for (i=1; i<nvertex; i++) {
          if (min > data[i]) min = data[i];
          if (max < data[i]) max = data[i];
        }

      double center, scale;

      if (have_reference_) center = reference_;
      else center = (max+min)/2.0; 

      double maxdiff = fabs(max - center);
      double mindiff = fabs(min - center);

      if (have_scale_) {
          scale = scale_;
        }
      else {
          if (maxdiff>mindiff && maxdiff>1.0e-6) scale = (1.0-base)/maxdiff;
          else if (mindiff>1.0e-6) scale = (1.0-base)/mindiff;
          else scale = (1.0-base);
        }

      ExEnv::out0() << indent << "GradDensityColorizer:"
           << scprintf(" reference=%6.5f", center)
           << scprintf(" scale=%6.2f",scale)
           << scprintf(" (%6.5f<=rho<=%6.5f)", max, min)
           << endl;
      for (i=0; i<nvertex; i++) {
          data[i] = (data[i]-center)*scale;
        }

      for (i=0; i<nvertex; i++) {
          Color c;
          if (data[i] > 0.0) c.set_rgb(data[i]+base,0.3,0.3);
          else c.set_rgb(0.3,0.3,-data[i]+base);
          poly->set_vertex_color(i,c);
        }

      delete[] data;
    }
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
