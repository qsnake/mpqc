//
// svd.h
//
// Copyright (C) 2005 Edward Valeev
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

#ifndef _chemistry_qc_mbptr12_svd_h
#define _chemistry_qc_mbptr12_svd_h

#include <math/scmat/matrix.h>

using namespace std;

namespace sc {

  namespace exp {

    /** Uses LAPACK's DGESVD to perform SVD of A:
        A = U * Sigma * V
    */
    void lapack_svd(const RefSCMatrix& A,
                    RefSCMatrix& U,
                    RefDiagSCMatrix& Sigma,
                    RefSCMatrix& V);

    /** Uses LAPACK's DSPSVX to solve symmetric non-definite linear system AX = B, where
        B is a single vector
    */
    void lapack_linsolv_symmnondef(const RefSymmSCMatrix& A,
                                   RefSCVector& X,
                                   const RefSCVector& B);

    /** Uses LAPACK's DSPSVX to solve symmetric non-definite linear system AX = B, where
        B is a set of vectors
    */
    void lapack_linsolv_symmnondef(const RefSymmSCMatrix& A,
                                   RefSCMatrix& X,
                                   const RefSCMatrix& B);

  }
}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:


