//
// obwfn.h
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

#ifndef _chemistry_qc_wfn_obwfn_h
#define _chemistry_qc_wfn_obwfn_h

#ifdef __GNUC__
#pragma interface
#endif

#include <chemistry/qc/wfn/wfn.h>

namespace sc {

/**A OneBodyWavefunction is a MolecularEnergy that solves an effective
one-body problem. */
class OneBodyWavefunction: public Wavefunction {
 protected:
    ResultRefSymmSCMatrix density_;
    AccResultRefSCMatrix oso_eigenvectors_;
    AccResultRefDiagSCMatrix eigenvalues_;
    int nirrep_;
    int *nvecperirrep_;
    double *occupations_;
    double *alpha_occupations_;
    double *beta_occupations_;

    void init_sym_info();

    // oldocc is converted to newocc using the correlation
    // table between initial_pg_ and the current point group
    // returns 1 if successful and 0 otherwise.  newocc is
    // delete[]'ed and new'ed.
    int form_occupations(int *&newocc, const int *oldocc);

 public:
    OneBodyWavefunction(StateIn&);
    /** The KeyVal constructor.
        <dl>

        <dt><tt>eigenvector_accuracy</tt><dd> Gives the accuracy to which
        eigenvectors are initially computed.  The default 1.0e-7.
        Accuracies are usually adjusted as needed anyway, so it should not
        be necessary to change this.

        </dl>
    */
    OneBodyWavefunction(const Ref<KeyVal>&);
    ~OneBodyWavefunction();

    void save_data_state(StateOut&);

    int nelectron();

    /** Overload of Function::set_desired_value_accuracy(). Must update
        accuracy of the eigenvectors and the eigenvalues */
    void set_desired_value_accuracy(double eps);

    // Following is a proposed interface to make the meaning of
    // the various transformation matrices less confusing.
//     /** These members give metrics and basis transformations
//         using the covariant/contravariant tensor notation. */
//     //@{
//     /** Returns the transformation matrix that converts
//         a contravariant SO tensor index to a contravariant
//         MO tensor index.
//      */
//     RefSCMatrix t_mo_so_I_J();
//     /** Returns the transformation matrix that converts a covariant SO
//         tensor index to a covariant MO tensor index.
//      */
//     RefSCMatrix t_mo_so_i_j();
//     /** Returns the transformation matrix that converts
//         a contravariant MO tensor index to a contravariant
//         SO tensor index.
//      */
//     RefSCMatrix t_mo_so_I_J();
//     /** Returns the transformation matrix that converts a covariant MO
//         tensor index to a covariant SO tensor index.
//      */
//     RefSCMatrix t_mo_so_i_j();
//     /** Returns the metric for converting a covariant SO index into
//         a contravariant one. */
//     RefSCMatrix g_so_I_j();
//     /** Returns the metric for converting a contravariant SO index into
//         a covariant one. */
//     RefSCMatrix g_so_i_J();
//     //@}

    /// Returns the SO to MO transformation matrix.
    RefSCMatrix so_to_mo();
    /// Returns the orthogonal-SO to MO transformation matrix.
    RefSCMatrix orthog_so_to_mo();
    /// Returns the MO to SO transformation matrix.
    RefSCMatrix mo_to_so();
    /** Returns the MO to orthogonal-SO transformation matrix.
        This returns the same matrix as oso_eigenvectors(). */
    RefSCMatrix mo_to_orthog_so();

    /** Deprecated.  Use so_to_mo().t() instead. */
    RefSCMatrix eigenvectors();
    /** Returns the orthogonal MO (columns) to orthogonal-SO (rows) transformation
        matrix. */
    virtual RefSCMatrix oso_eigenvectors() = 0;
    /** Returns the MO basis eigenvalues. */
    virtual RefDiagSCMatrix eigenvalues() = 0;
    /** Returns the occupation.  The irreducible representation and the
        vector number within that representation are given as arguments. */
    virtual double occupation(int irrep, int vectornum) = 0;
    /** Returns the occupation. The vector number in the MO basis is given
        as an argument. */
    double occupation(int vectornum);

    /// Return 1 if the alpha orbitals are not equal to the beta orbitals.
    virtual int spin_unrestricted() = 0;

    /** Returns the alpha occupation.  The irreducible representation and the
        vector number within that representation are given as arguments. */
    virtual double alpha_occupation(int irrep, int vectornum);
    /** Returns the beta occupation.  The irreducible representation and the
        vector number within that representation are given as arguments. */
    virtual double beta_occupation(int irrep, int vectornum);
    /** Returns the alpha occupation. The vector number in the MO basis is
        given as an argument. */
    double alpha_occupation(int vectornum);
    /** Returns the beta occupation. The vector number in the MO basis is
        given as an argument. */
    double beta_occupation(int vectornum);
    
    // Return alpha and beta electron densities
    virtual RefSCMatrix oso_alpha_eigenvectors();
    virtual RefSCMatrix oso_beta_eigenvectors();
    virtual RefSCMatrix alpha_eigenvectors();
    virtual RefSCMatrix beta_eigenvectors();
    virtual RefDiagSCMatrix alpha_eigenvalues();
    virtual RefDiagSCMatrix beta_eigenvalues();

    virtual RefDiagSCMatrix
      projected_eigenvalues(const Ref<OneBodyWavefunction>&, int alp=1);
    /** Projects the density into the current basis set.  Returns an
        orthogonalized SO to MO transformation with the orbitals. */
    virtual RefSCMatrix projected_eigenvectors(const Ref<OneBodyWavefunction>&,
                                               int alp=1);
    /** Return a guess vector.  The guess transforms the orthogonal SO
        basis to the MO basis. */
    virtual RefSCMatrix hcore_guess();
    /** Return a guess vector and the eigenvalues.  The guess ransforms the
        orthogonal SO basis to the MO basis. Storage for the eigenvalues
        will be allocated. */
    virtual RefSCMatrix hcore_guess(RefDiagSCMatrix &val);

    void symmetry_changed();
    
    double orbital(const SCVector3& r, int iorb);
    double orbital_density(const SCVector3& r, int iorb, double* orbval = 0);

    void print(std::ostream&o=ExEnv::out0()) const;
};


// This is useful as an initial guess for other one body wavefunctions
class HCoreWfn: public OneBodyWavefunction {
  private:
    int nirrep_;
    int *docc_;
    int *socc_;
    int total_charge_;
    int user_occ_;

    void fill_occ(const RefDiagSCMatrix &evals,
                  int ndocc, int *docc, int nsocc = 0, int *socc = 0);

    void compute();

  public:
    HCoreWfn(StateIn&);
    HCoreWfn(const Ref<KeyVal>&);
    ~HCoreWfn();

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
