
//
// orthog.h -- orthogonalize the basis set
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Curtis Janssen <cljanss@ca.sandia.gov>
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

#ifndef _chemistry_qc_basis_orthog_h
#define _chemistry_qc_basis_orthog_h

#include <util/state/state.h>
#include <math/scmat/matrix.h>

namespace sc {

/// This class computes the orthogonalizing transform for a basis set.
class OverlapOrthog: virtual public SavableState {
  public:
    
    /// An enum for the types of orthogonalization.
    enum OrthogMethod { Symmetric=1, Canonical=2, GramSchmidt=3 };

  private:
    int debug_;

    RefSCDimension dim_;
    RefSCDimension orthog_dim_;

    // The tolerance for linearly independent basis functions.
    // The intepretation depends on the orthogonalization method.
    double lindep_tol_;
    // The number of linearly dependent functions
    int nlindep_;
    // The orthogonalization method
    OrthogMethod orthog_method_;
    // The orthogonalization matrices
    RefSCMatrix orthog_trans_;
    RefSCMatrix orthog_trans_inverse_;
    // The maximum and minimum residuals from the orthogonalization
    // procedure.  The interpretation depends on the method used.
    // For symmetry and canonical, these are the min and max overlap
    // eigenvalues.  These are the residuals for the basis functions
    // that actually end up being used.
    double min_orthog_res_;
    double max_orthog_res_;

    void compute_overlap_eig(RefSCMatrix& overlap_eigvec,
                             RefDiagSCMatrix& overlap_isqrt_eigval,
                             RefDiagSCMatrix& overlap_sqrt_eigval);
    void compute_symmetric_orthog();
    void compute_canonical_orthog();
    void compute_gs_orthog();
    void compute_orthog_trans();

    // WARNING: after a SavableState save/restore, these two members will
    // be null.  There is really no need to store these anyway--should be
    // removed.
    RefSymmSCMatrix overlap_;
    Ref<SCMatrixKit> result_kit_; // this kit is used for the result matrices

  public:
    OverlapOrthog(OrthogMethod method,
                  const RefSymmSCMatrix &overlap,
                  const Ref<SCMatrixKit> &result_kit,
                  double lindep_tolerance,
                  int debug = 0);

    OverlapOrthog(StateIn&);

    virtual ~OverlapOrthog();

    void save_data_state(StateOut&);

    void reinit(OrthogMethod method,
                const RefSymmSCMatrix &overlap,
                const Ref<SCMatrixKit> &result_kit,
                double lindep_tolerance,
                int debug = 0);

    double min_orthog_res() const { return min_orthog_res_; }
    double max_orthog_res() const { return max_orthog_res_; }

    Ref<OverlapOrthog> copy() const;

    /// Returns the orthogonalization method
    OrthogMethod orthog_method() const { return orthog_method_; }

    /// Returns the tolerance for linear dependencies.
    double lindep_tol() const { return lindep_tol_; }

    /** Returns a matrix which does the requested transform from a basis to
        an orthogonal basis.  This could be either the symmetric or
        canonical orthogonalization matrix.  The row dimension is the basis
        dimension and the column dimension is orthogonal basis dimension.
        An operator \f$O\f$ in the orthogonal basis is given by \f$X O
        X^T\f$ where \f$X\f$ is the return value of this function. */
    RefSCMatrix basis_to_orthog_basis();

    /** Returns the inverse of the transformation returned by
     * basis_to_orthog_basis.
     */
    RefSCMatrix basis_to_orthog_basis_inverse();

    RefSCDimension dim();
    RefSCDimension orthog_dim();

    /** Returns the number of linearly dependent functions eliminated from
        the orthogonal basis.
      */
    int nlindep();
};

}

#endif
