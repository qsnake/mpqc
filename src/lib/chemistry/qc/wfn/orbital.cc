//
// orbital.cc
//
// Copyright (C) 1997 Limit Point Systems, Inc.
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

#include <math.h>

#include <util/misc/formio.h>

#include <math/scmat/local.h>
#include <math/scmat/vector3.h>
#include <chemistry/molecule/molecule.h>
#include <chemistry/qc/wfn/orbital.h>

using namespace sc;

static ClassDesc Orbital_cd(
  typeid(Orbital),"Orbital",1,"public Volume",
  0, create<Orbital>, 0);

Orbital::Orbital(const Ref<KeyVal> &keyval):
  Volume(keyval)
{
  wfn_ << keyval->describedclassvalue("wfn");
  orbital_ = keyval->intvalue("orbital");
}

Orbital::Orbital(const Ref<OneBodyWavefunction>& wfn, int orbital):
  Volume(),
  wfn_(wfn),
  orbital_(orbital)
{
}

Orbital::~Orbital()
{
}

void
Orbital::compute()
{
  SCVector3 r;
  get_x(r);
  if (value_needed()) {
      set_value(fabs(wfn_->orbital(r, orbital_)));
      set_actual_value_accuracy(desired_value_accuracy());
    }
  if (gradient_needed() || hessian_needed()) {
      ExEnv::err0() << indent
           << "Orbital::compute(): gradient & hessian not implemented\n";
      abort();
    }
}

// make a wild guess about the bounding box
void
Orbital::boundingbox(double valuemin,
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
          if (mol.r(i,j) < p1[i]) p1[i] = mol.r(i,j);
          if (mol.r(i,j) > p2[i]) p2[i] = mol.r(i,j);
        }
    }
  for (i=0; i<3; i++) {
      p1[i] = p1[i] - 3.0;
      p2[i] = p2[i] + 3.0;
    }
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
