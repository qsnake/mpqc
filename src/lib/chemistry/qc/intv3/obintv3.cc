//
// obintv3.cc
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

#include <chemistry/qc/intv3/obintv3.h>

using namespace sc;

////////////////////////////////////////////////////////////////////////////
// OneBodyIntV3

OneBodyIntV3::OneBodyIntV3(Integral* integral,
                           const Ref<GaussianBasisSet>&bs1,
                           const Ref<GaussianBasisSet>&bs2,
                           IntegralFunction ifunc):
  OneBodyInt(integral,bs1,bs2)
{
  int1ev3_ = new Int1eV3(integral,bs1,bs2,0);
  intfunc_ = ifunc;
  buffer_ = int1ev3_->buffer();
}

OneBodyIntV3::~OneBodyIntV3()
{
}

void
OneBodyIntV3::compute_shell(int i, int j)
{
  (int1ev3_.pointer()->*intfunc_)(i, j);
}

bool
OneBodyIntV3::cloneable()
{
  return true;
}

Ref<OneBodyInt>
OneBodyIntV3::clone()
{
  return new OneBodyIntV3(integral_, bs1_, bs2_, intfunc_);
}

////////////////////////////////////////////////////////////////////////////
// PointChargeIntV3

PointChargeIntV3::PointChargeIntV3(
    Integral *integral,
    const Ref<GaussianBasisSet>&bs1,
    const Ref<GaussianBasisSet>&bs2,
    const Ref<PointChargeData>&dat):
  OneBodyInt(integral,bs1,bs2),
  data_(dat)
{
  int1ev3_ = new Int1eV3(integral,bs1,bs2,0);
  buffer_ = int1ev3_->buffer();
}

PointChargeIntV3::~PointChargeIntV3()
{
}

void
PointChargeIntV3::compute_shell(int i,int j)
{
  int1ev3_->point_charge(i,j,
                         data_->ncharges(),
                         data_->charges(),
                         data_->positions());
}

////////////////////////////////////////////////////////////////////////////
// EfieldDotVectorIntV3

EfieldDotVectorIntV3::EfieldDotVectorIntV3(
    Integral *integral,
    const Ref<GaussianBasisSet>&bs1,
    const Ref<GaussianBasisSet>&bs2,
    const Ref<EfieldDotVectorData>&dat) :
  OneBodyInt(integral,bs1,bs2),
  data_(dat)
{
  int1ev3_ = new Int1eV3(integral,bs1,bs2,0);
  buffer_ = int1ev3_->buffer();
}

EfieldDotVectorIntV3::~EfieldDotVectorIntV3()
{
}

void
EfieldDotVectorIntV3::compute_shell(int i,int j)
{
  int nbfi = basis1()->shell(i).nfunction();
  int nbfj = basis2()->shell(j).nfunction();
  int nint = nbfi*nbfj;
  double *tmp;
  int ii,jj;

  int1ev3_->efield(i,j,data_->position);

  tmp = int1ev3_->buffer();
  for (ii=0; ii<nint; ii++) {
      double tmpval = 0.0;
      for (jj=0; jj<3; jj++) {
          tmpval += *tmp++ * data_->vector[jj];
        }
      buffer_[ii] = tmpval;
    }
}

////////////////////////////////////////////////////////////////////////////
// DipoleIntV3

DipoleIntV3::DipoleIntV3(Integral *integral,
                         const Ref<GaussianBasisSet>&bs1,
                         const Ref<GaussianBasisSet>&bs2,
                         const Ref<DipoleData>&dat) :
  OneBodyInt(integral,bs1,bs2),
  data_(dat)
{
  int1ev3_ = new Int1eV3(integral,bs1,bs2,0);
  buffer_ = int1ev3_->buffer();
  if (data_.null()) {
      data_ = new DipoleData;
    }
}

DipoleIntV3::~DipoleIntV3()
{
}

void
DipoleIntV3::compute_shell(int i,int j)
{
  int1ev3_->dipole(i,j,data_->origin);
}

////////////////////////////////////////////////////////////////////////////
// OneBodyDerivIntV3

OneBodyDerivIntV3::OneBodyDerivIntV3(Integral *integral,
                                     const Ref<GaussianBasisSet>&bs1,
                                     const Ref<GaussianBasisSet>&bs2,
                                     IntegralFunction ifunc):
  OneBodyDerivInt(integral,bs1,bs2)
{
  int1ev3_ = new Int1eV3(integral,bs1,bs2,1);
  intfunc_ = ifunc;
  buffer_ = int1ev3_->buffer();
}

OneBodyDerivIntV3::~OneBodyDerivIntV3()
{
}

void
OneBodyDerivIntV3::compute_shell(int i, int j, DerivCenters& c)
{
  (int1ev3_.pointer()->*intfunc_)(i,j,0,basis1()->shell_to_center(i));
  c.clear();
  c.add_center(0,basis1(),i);
  c.add_omitted(1,basis2(),j);

// temporary debugging stuff for cca integrals comparison
//  if( 1 ) {
//    std::cerr << "buffer for shell doublet (with dc):\n";
//    std::cerr << "shellnum1: " << i << std::endl;
//    GaussianShell* s1 = &( bs1->shell(i) );
//    int nc1 = s1->ncontraction();
//    for (int ii=0; ii<nc1; ++ii)
//      std::cerr << "am: " << s1->am(ii) << std::endl;
//    std::cerr << "shellnum2: " << j << std::endl;
//    GaussianShell* s2 = &( bs2->shell(j) );
//    int nc2 = s2->ncontraction();
//    for (int ii=0; ii<nc2; ++ii)
//      std::cerr << "am: " << s2->am(ii) << std::endl;
//
//    int nfunc = s1->max_cartesian() * s2->max_cartesian();
//    std::cerr << "dx\n";
//    for( int ii=0; ii<nfunc; ++ii)
//      std::cerr << buffer_[ii] << std::endl;
//    std::cerr << "dy\n";
//    for( int ii=nfunc; ii<nfunc*2; ++ii)
//      std::cerr << buffer_[ii] << std::endl;
//    std::cerr << "dz\n";
//    for( int ii=nfunc*2; ii<nfunc*3; ++ii)
//      std::cerr << buffer_[ii] << std::endl;
//  }

}

void
OneBodyDerivIntV3::compute_shell(int i, int j, int c)
{
  (int1ev3_.pointer()->*intfunc_)(i,j,0,c);

// temporary debuging stuff for cca integrals comparison
//  if( 1 ) {
//    std::cerr << "doing center " << c << std::endl;
//    std::cerr << "buffer for shell doublet:\n";
//    std::cerr << "shellnum1: " << i << std::endl;
//    GaussianShell* s1 = &( bs1->shell(i) );
//    int nc1 = s1->ncontraction();
//    for (int ii=0; ii<nc1; ++ii)
//      std::cerr << "am: " << s1->am(ii) << std::endl;
//    std::cerr << "shellnum2: " << j << std::endl;
//    GaussianShell* s2 = &( bs2->shell(j) );
//    int nc2 = s2->ncontraction();
//    for (int ii=0; ii<nc2; ++ii)
//      std::cerr << "am: " << s2->am(ii) << std::endl;
//
//    int nfunc = s1->max_cartesian() * s2->max_cartesian();
//    std::cerr << "dx\n";
//    for( int ii=0; ii<nfunc; ++ii)
//      std::cerr << buffer_[ii] << std::endl;
//    std::cerr << "dy\n";
//    for( int ii=nfunc; ii<nfunc*2; ++ii)
//      std::cerr << buffer_[ii] << std::endl;
//    std::cerr << "dz\n";
//    for( int ii=nfunc*2; ii<nfunc*3; ++ii)
//      std::cerr << buffer_[ii] << std::endl;
//  }

}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
