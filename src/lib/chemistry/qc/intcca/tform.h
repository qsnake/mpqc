//
// tform.h
//
// Copyright (C) 2001 Edward Valeev
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

#ifndef _chemistry_qc_cints_tform_h
#define _chemistry_qc_cints_tform_h

#include <chemistry/qc/basis/gaussshell.h>
#include <chemistry/qc/basis/transform.h>
#include <chemistry/qc/intcca/macros.h>

namespace sc {

class Integral;

class SphericalTransformComponentCCA : public SphericalTransformComponent {
  public:
    void init(int a, int b, int c, double coef, int pureindex) {
      a_ = a;
      b_ = b;
      c_ = c;
      // Modify the coefficient here to conform the normalization 
      // convention of cints
      coef_ = coef;

      pureindex_ = pureindex;
      cartindex_ = INT_CARTINDEX(a+b+c,a,b);
    }
};

class SphericalTransformCCA : public SphericalTransform {
  public:
    SphericalTransformCCA(int l, int subl=-1):SphericalTransform(l,subl) {
      init();
    }

    SphericalTransformComponent * new_components() {
      return new SphericalTransformComponentCCA[n_+1];
    }
};

class ISphericalTransformCCA : public ISphericalTransform {
  public:
    ISphericalTransformCCA(int l, int subl=-1):ISphericalTransform(l,subl) {
      init();
    }

    SphericalTransformComponent * new_components() {
      return new SphericalTransformComponentCCA[n_+1];
    }
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
