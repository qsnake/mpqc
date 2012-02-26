//
// gaussbas.h
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

#ifndef _chemistry_qc_basis_gaussbas_h
#define _chemistry_qc_basis_gaussbas_h

#ifdef __GNUC__
#pragma interface
#endif

#include <vector>
#include <iostream>

#include <util/state/state.h>
#include <util/keyval/keyval.h>
#include <math/scmat/matrix.h>
#include <math/scmat/vector3.h>
#include <chemistry/molecule/molecule.h>

namespace sc {

class GaussianShell;
class BasisFileSet;
class Integral;

class CartesianIter;
class SphericalTransformIter;

/** The GaussianBasisSet class is used describe a basis set composed of
atomic gaussian orbitals.  Inputs for common basis sets are included in the
MPQC distribution.  They have been obtained from the EMSL Basis Set
Database and translated into the MPQC format.  The citation for this
database is below.  The technical citation for each basis set is listed in
the individual basis set data files, in MPQC's <tt>lib/basis</tt>
directory.

Following is a table with available basis sets listing the supported
elements for each basis and the number of basis functions for H, \f$n_0\f$,
first row, \f$n_1\f$, and second row, \f$n_2\f$, atoms.  Basis sets with
non-alpha-numerical characters in their name must be given in quotes.

<table>
<tr><td>Basis Set<td>Elements<td>\f$n_0\f$<td>\f$n_1\f$<td>\f$n_2\f$
<tr><td><tt>STO-2G</tt><td>H-Ca<td>1<td>5<td>9
<tr><td><tt>STO-3G</tt><td>H-Kr<td>1<td>5<td>9
<tr><td><tt>STO-3G*</tt><td>H-Ar<td>1<td>5<td>14
<tr><td><tt>STO-6G</tt><td>H-Kr<td>1<td>5<td>9
<tr><td><tt>MINI (Huzinaga)</tt><td>H-Ca<td>1<td>5<td>9
<tr><td><tt>MINI (Scaled)</tt><td>H-Ca<td>1<td>5<td>9
<tr><td><tt>MIDI (Huzinaga)</tt><td>H-Na, Al-K<td>2<td>9<td>13
<tr><td><tt>DZ (Dunning)</tt><td>H, Li, B-Ne, Al-Cl<td>2<td>10<td>18
<tr><td><tt>DZP (Dunning)</tt><td>H, Li, B-Ne, Al-Cl<td>5<td>16<td>24
<tr><td><tt>DZP + Diffuse (Dunning)</tt><td>H, B-Ne<td>6<td>19<td>
<tr><td><tt>3-21G</tt><td>H-Kr<td>2<td>9<td>13
<tr><td><tt>3-21G*</tt><td>H-Ar<td>2<td>9<td>19
<tr><td><tt>3-21++G</tt><td>H-Ar<td>3<td>13<td>17
<tr><td><tt>3-21++G*</tt><td>H-Ar<td>3<td>13<td>23
<tr><td><tt>4-31G</tt><td>H-Ne, P-Cl<td>2<td>9<td>13
<tr><td><tt>6-31G</tt><td>H-Zn<td>2<td>9<td>13
<tr><td><tt>6-31G*</tt><td>H-Zn<td>2<td>15<td>19
<tr><td><tt>6-31G**</tt><td>H-Zn<td>5<td>15<td>19
<tr><td><tt>6-31+G*</tt><td>H-Ar<td>2<td>19<td>23
<tr><td><tt>6-31++G</tt><td>H-Ca<td>3<td>13<td>17
<tr><td><tt>6-31++G*</tt><td>H-Ar<td>3<td>19<td>23
<tr><td><tt>6-31++G**</tt><td>H-Ar<td>6<td>19<td>23
<tr><td><tt>6-311G</tt><td>H-Ca, Ga-Kr<td>3<td>13<td>21
<tr><td><tt>6-311G*</tt><td>H-Ca, Ga-Kr<td>3<td>18<td>26
<tr><td><tt>6-311G**</tt><td>H-Ca, Ga-Kr<td>6<td>18<td>26
<tr><td><tt>6-311G(2df,2pd)</tt><td>H-Ne, K, Ca<td>14<td>30<td>
<tr><td><tt>6-311++G**</tt><td>H-Ne<td>7<td>22<td>
<tr><td><tt>6-311++G(2d,2p)</tt><td>H-Ca<td>10<td>27<td>35
<tr><td><tt>6-311++G(3df,3pd)</tt><td>H-Ar<td>18<td>39<td>47
<tr><td><tt>cc-pVDZ</tt><td>H-Ar, Ca, Ga-Kr<td>5<td>14<td>18
<tr><td><tt>cc-pVTZ</tt><td>H-Ar, Ca, Ga-Kr<td>14<td>30<td>34
<tr><td><tt>cc-pVQZ</tt><td>H-Ar, Ca, Ga-Kr<td>30<td>55<td>59
<tr><td><tt>cc-pV5Z</tt><td>H-Ar, Ca, Ga-Kr<td>55<td>91<td>95
<tr><td><tt>cc-pV6Z</tt><td>H, He, B-Ne, Al-Ar<td>91<td>140<td>144
<tr><td><tt>aug-cc-pVDZ</tt><td>H, He, B-Ne, Al-Ar, Ga-Kr<td>9<td>23<td>27
<tr><td><tt>aug-cc-pVTZ</tt><td>H, He, B-Ne, Al-Ar, Ga-Kr<td>23<td>46<td>50
<tr><td><tt>aug-cc-pVQZ</tt><td>H, He, B-Ne, Al-Ar, Ga-Kr<td>46<td>80<td>84
<tr><td><tt>aug-cc-pV5Z</tt><td>H, He, B-Ne, Al-Ar, Ga-Kr<td>80<td>127<td>131
<tr><td><tt>aug-cc-pV6Z</tt><td>H, He, B-Ne, Al-Ar<td>127<td>189<td>193
<tr><td><tt>cc-pCVDZ</tt><td>Li, B-Ar<td><td>18<td>27
<tr><td><tt>cc-pCVTZ</tt><td>Li, B-Ar<td><td>43<td>59
<tr><td><tt>cc-pCVQZ</tt><td>Li, B-Ar<td><td>84<td>109
<tr><td><tt>cc-pCV5Z</tt><td>B-Ne<td><td>145<td>
<tr><td><tt>aug-cc-pCVDZ</tt><td>B-F, Al-Ar<td><td>27<td>36
<tr><td><tt>aug-cc-pCVTZ</tt><td>B-Ne, Al-Ar<td><td>59<td>75
<tr><td><tt>aug-cc-pCVQZ</tt><td>B-Ne, Al-Ar<td><td>109<td>134
<tr><td><tt>aug-cc-pCV5Z</tt><td>B-F<td><td>181<td>
<tr><td><tt>NASA Ames ANO</tt><td>H, B-Ne, Al, P, Ti, Fe, Ni<td>30<td>55<td>59
<tr><td><tt>pc-0</tt><td>H, C-F, Si-Cl<td>2<td>9<td>13
<tr><td><tt>pc-1</tt><td>H, C-F, Si-Cl<td>5<td>14<td>18
<tr><td><tt>pc-2</tt><td>H, C-F, Si-Cl<td>14<td>30<td>34
<tr><td><tt>pc-3</tt><td>H, C-F, Si-Cl<td>34<td>64<td>64
<tr><td><tt>pc-4</tt><td>H, C-F, Si-Cl<td>63<td>109<td>105
<tr><td><tt>pc-0-aug</tt><td>H, C-F, Si-Cl<td>3<td>13<td>17
<tr><td><tt>pc-1-aug</tt><td>H, C-F, Si-Cl<td>9<td>23<td>27
<tr><td><tt>pc-2-aug</tt><td>H, C-F, Si-Cl<td>23<td>46<td>50
<tr><td><tt>pc-3-aug</tt><td>H, C-F, Si-Cl<td>50<td>89<td>89
<tr><td><tt>pc-4-aug</tt><td>H, C-F, Si-Cl<td>88<td>145<td>141
</table>

All basis sets except for the pc-<i>n</i> and pc-<i>n</i>-aug basis sets
were obtained from the Extensible Computational Chemistry
Environment Basis Set Database, Version 12/03/03, as developed and
distributed by the Molecular Science Computing Facility, Environmental and
Molecular Sciences Laboratory which is part of the Pacific Northwest
Laboratory, P.O. Box 999, Richland, Washington 99352, USA, and funded by
the U.S. Department of Energy. The Pacific Northwest Laboratory is a
multi-program laboratory operated by Battelle Memorial Institute for the
U.S. Department of Energy under contract DE-AC06-76RLO 1830. Contact David
Feller or Karen Schuchardt for further information.

The pc-<i>n</i> and pc-<i>n</i>-aug basis sets are the polarization
consistent basis sets of Frank Jensen.  See J. Chem. Phys. 115 (2001) 9113;
J. Chem. Phys. 116 (2002) 7372; J. Chem. Phys. 117 (2002) 9234; and
J. Chem. Phys. 121 (2004) 3463.

*/
class GaussianBasisSet: public SavableState
{
  private:
    // nonnull if keyword "name" was provided
    char* name_;
    // same as name_ if name_!=0, else something else
    char* label_;
    GaussianShell** shell_;
    std::vector<int> shell_to_function_;
    std::vector<int> function_to_shell_;

    Ref<Molecule> molecule_;

    Ref<SCMatrixKit> matrixkit_;
    Ref<SCMatrixKit> so_matrixkit_;
    RefSCDimension basisdim_;

    int ncenter_;

    std::vector<int> shell_to_center_;
    std::vector<int> shell_to_primitive_;
    std::vector<int> center_to_shell_;
    std::vector<int> center_to_nshell_;
    std::vector<int> center_to_nbasis_;

    int nshell_;
    int nbasis_;
    int nprim_;
    bool has_pure_;

    GaussianBasisSet(const char* name, const char* label, const Ref<Molecule>& molecule,
                     const Ref<SCMatrixKit>& matrixkit,
                     const RefSCDimension& basisdim,
                     const int ncenter, const int nshell,
                     GaussianShell** shell,
                     const std::vector<int>& center_to_nshell);

    // Counts shells in this basis for this chemical element
    int count_shells_(Ref<KeyVal>& keyval, const char* elemname, const char* sbasisname, BasisFileSet& bases,
		      int havepure, int pure, bool missing_ok);
    // Constructs this basis
    void get_shells_(int& ishell, Ref<KeyVal>& keyval, const char* elemname, const char* sbasisname, BasisFileSet& bases,
		     int havepure, int pure, bool missing_ok);
    // Counts shells in an even-tempered primitive basis
    int count_even_temp_shells_(Ref<KeyVal>& keyval, const char* elemname, const char* sbasisname,
                                int havepure, int pure);
    // Constructs an even-tempered primitive basis
    void get_even_temp_shells_(int& ishell, Ref<KeyVal>& keyval, const char* elemname, const char* sbasisname,
                               int havepure, int pure);
    // Constructs basis set specified as an array of shells
    void recursively_get_shell(int&,Ref<KeyVal>&,
                               const char*,const char*,BasisFileSet&,
                               int,int,int,bool missing_ok);

    void init(Ref<Molecule>&,Ref<KeyVal>&,
              BasisFileSet&,
              int have_userkeyval,
              int pure);
    void init2(int skip_ghosts=0,bool include_q=0);
    
  protected:
    GaussianBasisSet(const GaussianBasisSet&);
    virtual void set_matrixkit(const Ref<SCMatrixKit>&);
    
  public:
    /** This holds scratch data needed to compute basis function values. */
    class ValueData {
      protected:
        CartesianIter **civec_;
        SphericalTransformIter **sivec_;
        int maxam_;
      public:
        ValueData(const Ref<GaussianBasisSet> &, const Ref<Integral> &);
        ~ValueData();
        CartesianIter **civec() { return civec_; }
        SphericalTransformIter **sivec() { return sivec_; }
    };

    /// This can be given to a CTOR to construct a unit basis function.
    enum UnitType {Unit};

    /** The KeyVal constructor.

        <dl>

        <dt><tt>molecule</tt><dd> The gives a Molecule object.  The is no
        default.

        <dt><tt>puream</tt><dd> If this boolean parameter is true then 5D,
        7F, etc. will be used.  Otherwise all cartesian functions will be
        used.  The default depends on the particular basis set.

        <dt><tt>name</tt><dd> This is a string giving the name of the basis
        set.  The above table of basis sets gives some of the recognized
        basis set names.  It may be necessary to put the name in double
        quotes. There is no default.

        <dt><tt>basis</tt><dd> This is a vector of basis set names that can
        give a different basis set to each atom in the molecule.  If the
        element vector is given, then it gives different basis sets to
        different elements.  The default is to give every atom the basis
        set specified in name.

        <dt><tt>element</tt><dd> This is a vector of elements.  If it is
        given then it must have the same number of entries as the basis
        vector.

        <dt><tt>basisdir</tt><dd> A string giving a directory where basis
        set data files are to be sought.  See the text below for a complete
        description of what directories are consulted.

        <dt><tt>basisfiles</tt><dd> Each keyword in this vector of files is
        appended to the directory specified with basisdir and basis set
        data is read from them.

        <dt><tt>matrixkit</tt><dd> Specifies a SCMatrixKit object.  It is
        usually not necessary to give this keyword, as the default action
        should get the correct SCMatrixKit.

        </dl>

        Several files in various directories are checked for basis set
        data.  First, basis sets can be given by the user in the basis
        section at the top level of the main input file.  Next, if a path
        is given with the basisdir keyword, then all of the files given
        with the basisfiles keyword are read in after appending their names
        to the value of basisdir.  Basis sets can be given in these files
        in the basis section at the top level as well.  If the named basis
        set still cannot be found, then GaussianBasisSet will try convert
        the basis set name to a file name and check first in the directory
        given by basisdir.  Next it checks for the environment variable
        SCLIBDIR.  If it is set it will look for the basis file in
        $SCLIBDIR/basis.  Otherwise it will look in the source code
        distribution in the directory SC/lib/basis.  If the executable has
        changed machines or the source code has be moved, then it may be
        necessary to copy the library files to your machine and set the
        SCLIBDIR environmental variable.

        The basis set itself is also given in the ParsedKeyVal format. There are two
        recognized formats for basis sets:
        <dl>

        <dt>array of shells<dd> One must specify the keyword :basis: followed by the
        lowercase atomic name followed by : followed by the basis set name
        (which may need to be placed inside double quotes). The value for the keyword
        is an array of shells. Each shell
        reads the following keywords:

        <dl>

        <dt><tt>type</tt><dd> This is a vector that describes each
        component of this shell.  For each element the following two
        keywords are read:

        <dl>

          <dt><tt>am</tt><dd> The angular momentum of the component.  This
          can be given as the letter designation, s, p, d, etc.  There is
          no default.

          <dt><tt>puream</tt><dd> If this boolean parameter is true then
          5D, 7F, etc. shells are used.  The default is false.  This
          parameter can be overridden in the GaussianBasisSet
          specification.

        </dl>

        <dt><tt>exp</tt><dd> This is a vector giving the exponents of the
        primitive Gaussian functions.

        <dt><tt>coef</tt><dd> This is a matrix giving the coeffients of the
        primitive Gaussian functions.  The first index gives the component
        number of the shell and the second gives the primitive number.

        </dl>

        <dt><dd>An example might be easier to understand.  This is a basis set
        specificition for STO-2G carbon:

        <pre>
        basis: (
         carbon: "STO-2G": [
          (type: [(am = s)]
           {      exp      coef:0 } = {
              27.38503303 0.43012850
               4.87452205 0.67891353
           })
          (type: [(am = p) (am = s)]
           {     exp      coef:1     coef:0 } = {
               1.13674819 0.04947177 0.51154071
               0.28830936 0.96378241 0.61281990
           })
         ]
        )
        </pre>

        <dt>basis set of even-tempered primitive Gaussians<dd>
        Such basis set format is given as a group of keywords. The name of the group is :basis: followed by the
        lowercase atomic name followed by : followed by the basis set name
        (which may need to be placed inside double quotes).
        The group of keywords must contain vectors <tt>am</tt> and <tt>nprim</tt>,
        which specify the angular momentum and the number of shells in each
        block of even-tempered primitives. In addition, one must provide any
        two of the following vectors:

        <dl>
          <dt><tt>first_exp</tt><dd> The exponent of the "tightest" primitive Gaussian in the block.

          <dt><tt>last_exp</tt><dd> The exponent of the most "diffuse" primitive Gaussian in the block.

          <dt><tt>exp_ratio</tt><dd> The ratio of exponents of consecutive primitive Gaussians
          in the block.

        </dl>

        <dt><dd>Note that the dimensions of each vector must be the same.

        Here's an example of a basis set composed of 2 blocks of even-tempered s-functions
        and 1 block of even-tempered p-functions.

        <pre>
        basis: (
         neon: "20s5s13p":(

           am = [ 0 0 1 ]
           nprim = [ 20 5 13 ]
           first_exp = [ 1000.0 0.1  70.0 ]
           last_exp =  [    1.0 0.01  0.1 ]

         )
        )
        </pre>

        </dl>

        */
    GaussianBasisSet(const Ref<KeyVal>&);
    /** This can be given GaussianBasisSet::Unit to construct a basis
        set with a single basis function that is one everywhere.  This
        can be used with integral evaluators to compute certain classes
        of integrals, with limitations. */
    GaussianBasisSet(UnitType);
    GaussianBasisSet(StateIn&);
    virtual ~GaussianBasisSet();

    /** Returns a GaussianBasisSet object that consists of the basis
        functions for each atom in this followed by the basis functions in
        B for the corresponding atom.  The Molecule object for the two
        basis sets must be identical.  */
    Ref<GaussianBasisSet> operator+(const Ref<GaussianBasisSet>& B);

    void save_data_state(StateOut&);

    /// Return the name of the basis set (is nonnull only if keyword "name" was provided)
    const char* name() const { return name_; }
    /** Return the label of the basis set. label() return the same string as name() if
        keyword "name" was provided, otherwise it is a unique descriptive string which
        can be arbitrarily long. */
    const char* label() const { if (name()) { return name(); } else { return label_; } }

    /// Return the Molecule object.
    Ref<Molecule> molecule() const { return molecule_; }
    /// Returns the SCMatrixKit that is to be used for AO bases.
    Ref<SCMatrixKit> matrixkit() { return matrixkit_; }
    /// Returns the SCMatrixKit that is to be used for SO bases.
    Ref<SCMatrixKit> so_matrixkit() { return so_matrixkit_; }
    /// Returns the SCDimension object for the dimension.
    RefSCDimension basisdim() { return basisdim_; }

    /// Return the number of centers.
    int ncenter() const;
    /// Return the number of shells.
    int nshell() const { return nshell_; }
    /// Return the number of shells on the given center.
    int nshell_on_center(int icenter) const;
    /** Return an overall shell number, given a center and the shell number
        on that center. */
    int shell_on_center(int icenter, int shell) const;
    /// Return the center on which the given shell is located.
    int shell_to_center(int ishell) const { return shell_to_center_[ishell]; }
    /// Return the overall index of the first primitive from the given shell
    int shell_to_primitive(int ishell) const {return shell_to_primitive_[ishell]; }
    /// Return the number of basis functions.
    int nbasis() const { return nbasis_; }
    /// Return the number of basis functions on the given center.
    int nbasis_on_center(int icenter) const;
    /// Return the number of primitive Gaussians.
    int nprimitive() const { return nprim_; }
    /// Return true if basis contains solid harmonics Gaussians
    int has_pure() const { return has_pure_; }

    /// Return the maximum number of functions that any shell has.
    int max_nfunction_in_shell() const;
    /** Return the maximum number of Cartesian functions that any shell has.
        The optional argument is an angular momentum increment. */
    int max_ncartesian_in_shell(int aminc=0) const;
    /// Return the maximum number of primitive Gaussian that any shell has.
    int max_nprimitive_in_shell() const;
    /// Return the highest angular momentum in any shell.
    int max_angular_momentum() const;
    /// Return the maximum number of Gaussians in a contraction in any shell.
    int max_ncontraction() const;
    /** Return the maximum angular momentum found in the given contraction
        number for any shell.  */
    int max_am_for_contraction(int con) const;
    /// Return the maximum number of Cartesian functions in any shell.
    int max_cartesian() const;

    /// Return the number of the first function in the given shell.
    int shell_to_function(int i) const { return shell_to_function_[i]; }
    /// Return the shell to which the given function belongs.
    int function_to_shell(int i) const;

    /// Return a reference to GaussianShell number i.
    const GaussianShell& operator()(int i) const { return *shell_[i]; }
    /// Return a reference to GaussianShell number i.
    GaussianShell& operator()(int i) { return *shell_[i]; }
    /// Return a reference to GaussianShell number i.
    const GaussianShell& operator[](int i) const { return *shell_[i]; }
    /// Return a reference to GaussianShell number i.
    GaussianShell& operator[](int i) { return *shell_[i]; }
    /// Return a reference to GaussianShell number i.
    const GaussianShell& shell(int i) const { return *shell_[i]; }
    /// Return a reference to GaussianShell number i.
    GaussianShell& shell(int i) { return *shell_[i]; }

    /// Return a reference to GaussianShell number ishell on center icenter.
    const GaussianShell& operator()(int icenter,int ishell) const;
    /// Return a reference to GaussianShell number ishell on center icenter.
    GaussianShell& operator()(int icenter,int ishell);
    /// Return a reference to GaussianShell number j on center i.
    const GaussianShell& shell(int i,int j) const { return operator()(i,j); }
    /// Return a reference to GaussianShell number j on center i.
    GaussianShell& shell(int i,int j) { return operator()(i,j); }

    /** The location of center icenter.  The xyz argument is 0 for x, 1 for
        y, and 2 for z. */
    double r(int icenter,int xyz) const;
    
    /** Compute the values for this basis set at position r.  The
        basis_values argument must be vector of length nbasis. */
    int values(const SCVector3& r, ValueData *, double* basis_values) const;
    /** Like values(...), but computes gradients of the basis function
        values, too.  The g_values argument must be vector of length
        3*nbasis.  The data will be written in the order bf1_x, bf1_y,
        bf1_z, ... */
    int grad_values(const SCVector3& r, ValueData *,
                    double*g_values,double* basis_values=0) const;
    /** Like values(...), but computes first and second derivatives of the
        basis function values, too.  h_values must be vector of length
        6*nbasis.  The data will be written in the order bf1_xx, bf1_yx,
        bf1_yy, bf1_zx, bf1_zy, bf1_zz, ... */
    int hessian_values(const SCVector3& r, ValueData *, double *h_values,
                       double*g_values=0,double* basis_values=0) const;
    /** Compute the values for the given shell functions at position r.
        See the other values(...) members for more information.  */
    int shell_values(const SCVector3& r, int sh,
                     ValueData *, double* basis_values) const;
    /** Like values(...), but computes gradients of the shell function
        values, too.  See the other grad_values(...)
        members for more information.  */
    int grad_shell_values(const SCVector3& r, int sh,
                          ValueData *,
                          double*g_values, double* basis_values=0) const;
    /** Like values(...), but computes first and second derivatives of the
        shell function values, too.  See the other hessian_values(...)
        members for more information. */
    int hessian_shell_values(const SCVector3& r, int sh,
                       ValueData *, double *h_values,
                       double*g_values=0,double* basis_values=0) const;

    /// Returns true if this and the argument are equivalent.
    int equiv(const Ref<GaussianBasisSet> &b);

    /// Print a brief description of the basis set.
    void print_brief(std::ostream& =ExEnv::out0()) const;
    /// Print a detailed description of the basis set.
    void print(std::ostream& =ExEnv::out0()) const;
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
