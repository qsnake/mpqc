//
// natorbval.cc
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

#include <math/scmat/matrix.h>
#include <util/keyval/keyval.h>
#include <chemistry/qc/basis/gaussshell.h>
#include <chemistry/qc/basis/gaussbas.h>
#include <chemistry/qc/wfn/wfn.h>

using namespace sc;

// Function for returning an orbital value at a point
double Wavefunction::natural_orbital(const SCVector3& r, int iorb)
{
  return orbital(r,iorb,natural_orbitals());
}

// Function for returning an orbital value at a point
double Wavefunction::natural_orbital_density(const SCVector3& r,
                                             int iorb,
                                             double* orbval)
{
  return orbital_density(r,iorb,natural_orbitals(),orbval);
}

// Function for returning an orbital value at a point
double Wavefunction::orbital(const SCVector3& r,
                             int iorb,
                             const RefSCMatrix& orbs)
{
    int nbasis = basis()->nbasis();
    if (!bs_values) bs_values=new double[nbasis];

    // compute the basis set values
    GaussianBasisSet::ValueData *valdat
        = new GaussianBasisSet::ValueData(basis(), integral_);
    basis()->values(r,valdat,bs_values);
    delete valdat;
    
    // loop over basis functions
    double orb_value = 0;
    for (int i=0; i<nbasis; i++)
        orb_value += orbs.get_element(i,iorb)*bs_values[i];

    return orb_value;
}     

double Wavefunction::orbital_density(const SCVector3& r,
                                     int iorb,
                                     const RefSCMatrix& orbs,
                                     double* orbvalue)
{
  double tmp = orbital(r,iorb,orbs);
  if (orbvalue) *orbvalue = tmp;
  return 2.0 * tmp * tmp;
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
