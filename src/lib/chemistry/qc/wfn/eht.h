//
// eht.h
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

#ifndef _chemistry_qc_wfn_eht_h
#define _chemistry_qc_wfn_eht_h

#ifdef __GNUC__
#pragma interface
#endif

#include <chemistry/qc/wfn/obwfn.h>

namespace sc {

/* This computes the extended Huckel energy and wavefunction.  It is useful
   as a quick initial guess for other one body wavefunctions.  */
class ExtendedHuckelWfn: public OneBodyWavefunction {
  private:
    int nirrep_;
    int *docc_;
    int *socc_;
    int total_charge_;
    int user_occ_;

    void fill_occ(const RefDiagSCMatrix &evals,
                  int ndocc, int *docc, int nsocc = 0, int *socc = 0);

    void compute();

    RefSymmSCMatrix h_eht_oso();

  public:
    ExtendedHuckelWfn(StateIn&);
    ExtendedHuckelWfn(const Ref<KeyVal>&);
    ~ExtendedHuckelWfn();

    void save_data_state(StateOut&);

    double occupation(int irrep, int vectornum);

    RefSCMatrix oso_eigenvectors();
    RefDiagSCMatrix eigenvalues();
    RefSymmSCMatrix density();
    int spin_polarized();
    int spin_unrestricted();

    int value_implemented() const;
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
