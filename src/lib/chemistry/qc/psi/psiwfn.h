
#ifdef __GNUC__
#pragma interface
#endif

#ifndef _chemistry_qc_psi_psiwfn_h
#define _chemistry_qc_psi_psiwfn_h

#include <chemistry/qc/wfn/wfn.h>
#include <chemistry/qc/psi/psiexenv.h>

namespace sc {

///////////////////////////////////////////////////////////////////
/** PsiWavefunction is an abstract base for all Psi wave functions.
Its KeyVal constructor is invoked by all KeyVal constructors of
concrete implementations of PsiWavefunction.
*/

class PsiWavefunction: public Wavefunction {

    Ref<PsiExEnv> exenv_;

    int* read_occ(const Ref<KeyVal> &keyval, const char *name, int nirrep);

  protected:
    int nirrep_;
    int *docc_;
    int *socc_;
    int *frozen_docc_;
    int *frozen_uocc_;
    int multp_;
    int charge_;
    char *memory_;
    /// Prepares a complete Psi input file. The input file is assumed to have been opened.
    virtual void write_input(int conv) =0;

  public:
    /** The KeyVal constructor.

        <dl>

        <dt><tt>psienv</tt><dd> Specifies a PsiExEnv object.  There
        is no default.

	<dt><tt>memory</tt><dd> This integer specifies the amount of memory
	(in bytes) for Psi to use. The default is 2000000.

        <dt><tt>debug</tt><dd> This integer can be used to produce output
        for debugging.  The default is 0.

        </dl> */
    PsiWavefunction(const Ref<KeyVal>&);
    PsiWavefunction(StateIn&);
    ~PsiWavefunction();

    void save_data_state(StateOut&);

    /** Writes out Psi input file entries specific to this PsiWavefunction.
	The input file is assumed to have been opened. */
    virtual void write_basic_input(int conv);
    void compute();
    void print(std::ostream&o=ExEnv::out0()) const;
    RefSymmSCMatrix density();
    int nelectron();

    /// Return an associated PsiExEnv object
    Ref<PsiExEnv> get_psi_exenv() const { return exenv_; };
    /// Return an associated PsiInput object
    Ref<PsiInput> get_psi_input() const { return exenv_->get_psi_input(); };
};

///////////////////////////////////////////////////////////////////
/// PsiSCF is an abstract base for all Psi SCF wave functions

class PsiSCF: public PsiWavefunction {
  public:
    PsiSCF(const Ref<KeyVal>&);
    PsiSCF(StateIn&);
    ~PsiSCF();
    void save_data_state(StateOut&);

    enum RefType {rhf, hsoshf, uhf};
    /// Returns the PsiSCF::RefType of this particular Psi SCF wave function
    virtual PsiSCF::RefType reftype() const =0;
};

///////////////////////////////////////////////////////////////////
/// PsiCLHF is a concrete implementation of Psi RHF wave function

class PsiCLHF: public PsiSCF {
  protected:
    void write_input(int conv);
  public:
    PsiCLHF(const Ref<KeyVal>&);
    PsiCLHF(StateIn&);
    ~PsiCLHF();

    void write_basic_input(int conv);
    int spin_polarized() { return 0;};
    int gradient_implemented() const { return 1;};
    PsiSCF::RefType reftype() const { return rhf;};
};

///////////////////////////////////////////////////////////////////
/// PsiHSOSHF is a concrete implementation of Psi ROHF wave function

class PsiHSOSHF: public PsiSCF {
  protected:
    void write_input(int conv);
  public:
    PsiHSOSHF(const Ref<KeyVal>&);
    PsiHSOSHF(StateIn&);
    ~PsiHSOSHF();

    void write_basic_input(int conv);
    int spin_polarized() { return 0;};
    int gradient_implemented() const { return 1;};
    PsiSCF::RefType reftype() const { return hsoshf;};
};

///////////////////////////////////////////////////////////////////
/// PsiUHF is a concrete implementation of Psi UHF wave function

class PsiUHF: public PsiSCF {
  protected:
    void write_input(int conv);
  public:
    PsiUHF(const Ref<KeyVal>&);
    PsiUHF(StateIn&);
    ~PsiUHF();

    void write_basic_input(int conv);
    int spin_polarized() { return 1;};
    int gradient_implemented() const { return 1;};
    PsiSCF::RefType reftype() const { return uhf;};
};

///////////////////////////////////////////////////////////////////
/// PsiCCSD is a concrete implementation of Psi CCSD wave function

class PsiCCSD: public PsiWavefunction {
    Ref<PsiSCF> reference_;
  protected:
    void write_input(int conv);
  public:
    PsiCCSD(const Ref<KeyVal>&);
    PsiCCSD(StateIn&);
    ~PsiCCSD();
    void save_data_state(StateOut&);
    int spin_polarized() { return reference_->spin_polarized();};
    int gradient_implemented() const;
};

///////////////////////////////////////////////////////////////////
/// PsiCCSD_T is a concrete implementation of Psi CCSD(T) wave function

class PsiCCSD_T: public PsiWavefunction {
    Ref<PsiSCF> reference_;
  protected:
    void write_input(int conv);
  public:
    PsiCCSD_T(const Ref<KeyVal>&);
    PsiCCSD_T(StateIn&);
    ~PsiCCSD_T();

    void save_data_state(StateOut&);
    int spin_polarized() { return reference_->spin_polarized();};
    int gradient_implemented() const;
};

}

#endif
