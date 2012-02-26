//
// eri.h
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

#ifdef __GNUG__
#pragma interface
#endif

#ifndef _chemistry_qc_cints_eri_h
#define _chemistry_qc_cints_eri_h

#include <limits.h>

#include <util/ref/ref.h>
#include <chemistry/qc/basis/basis.h>
#include <chemistry/qc/cints/shellpairs.h>
#include <chemistry/qc/intv3/fjt.h>
#include <chemistry/qc/cints/int2e.h>
extern "C" {
#include <libint/libint.h>
}

namespace sc {

class Integral;

/** EriCints is a specialization of Int2eCints that computes electron repulsion integrals */
class EriCints: public Int2eCints {
  private:

    // Storage for target integrals
    double *target_ints_buffer_;

    /*--- Intermediate scratch arrays (may be used in new[] and delete[]) ---*/
    double *cart_ints_;       // cartesian integrals, in by-contraction-quartet order
    double *sphharm_ints_;    // transformed integrals, in by-contraction-quartet order
    double *perm_ints_;       // redundant target integrals in shell quartet order, shells permuted

    /*--- Pointers to scratch arrays (never used in new[] and delete[]) ---*/
    double *prim_ints_;       // this points to the appropriate location for raw integrals
    double *contr_quartets_;
    double *shell_quartet_;

    /*--- Precomputed data ---*/
    Ref<ShellPairsCints> shell_pairs12_;
    Ref<ShellPairsCints> shell_pairs34_;

    /*--- Internally used "interfaces" ---*/
    struct {
      int p12, p34, p13p24;           // flags indicating if functions were permuted
      ShellPairCints *shell_pair12, *shell_pair34;   // Shell pairs corresponding to the original
                                                     // (before permutation) order of shell
      int *op1, *op2, *op3, *op4;     // pointers to the primitive indices in the original order
      /////////// The rest of data has been permuted according to p12, p34, p13p24
      double A[3], B[3], C[3], D[3];
      double AB2, CD2;
      int gc1, gc2, gc3, gc4;
      int p1, p2, p3, p4;
      int am;
    } quartet_info_;
    void eri_quartet_data_(prim_data *Data, double scale);
    /*--- Compute engines ---*/
    Libint_t Libint_;
    Ref<FJT> Fm_Eval_;
  
  public:
    EriCints(Integral *,
	     const Ref<GaussianBasisSet>&,
	     const Ref<GaussianBasisSet>&,
	     const Ref<GaussianBasisSet>&,
	     const Ref<GaussianBasisSet>&,
	     size_t storage);
    ~EriCints();

    double *buffer(TwoBodyInt::tbint_type te_type) const {
      if (te_type == TwoBodyInt::eri) return target_ints_buffer_;
      else return 0;
    }

    static size_t storage_required(const Ref<GaussianBasisSet>& b1,
				   const Ref<GaussianBasisSet>& b2 = 0,
				   const Ref<GaussianBasisSet>& b3 = 0,
				   const Ref<GaussianBasisSet>& b4 = 0);
    
    // evaluate ERIs (Coulomb)
    void compute_quartet(int*, int*, int*, int*);
};

#include <chemistry/qc/cints/eri_quartet_data.h>

/* LibintStaticInterface is an initializer class for the static part
   of libint's interface (one per executable) */
class LibintStaticInterface {
    bool ready;

    public:
    LibintStaticInterface() { init_libint_base(); ready = true; }
    ~LibintStaticInterface() { ready = false; }
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
