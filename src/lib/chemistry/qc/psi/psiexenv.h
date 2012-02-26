
#ifdef __GNUC__
#pragma interface
#endif

#ifndef _chemistry_qc_psi_exenv_h
#define _chemistry_qc_psi_exenv_h

using namespace std;

#include <string>
#include <chemistry/qc/psi/psiinput.h>
#include <chemistry/qc/psi/psifile11.h>

namespace sc {

/// PsiExEnv specifies a Psi calculation

class PsiExEnv: public DescribedClass {

    // Static Psi info
    static string inputname_;
    static string file11name_;
    static int ckptfile_;

    // Defaults
    static string defaultpsiprefix_;
    static string defaultcwd_;
    static string defaultfileprefix_;
    static string defaultstdout_;
    static string defaultstderr_;

    // Calculation-specific info
    string psiprefix_;
    string cwd_;        // working directory where all files will be placed
    string fileprefix_;
    string stdout_;     // Standard output of psi modules
    string stderr_;     // Standard error of psi modules
    int nscratch_;
    string *scratch_;
    Ref<PsiInput> psiinput_;
    Ref<PsiFile11> psifile11_;

    // Add the following to the PATH environmental variable
    void add_to_path(const string &);

  public:
    PsiExEnv(const Ref<KeyVal>&);
    PsiExEnv(char *cwd, char *fileprefix, int nscratch, char **scratch);
    ~PsiExEnv();

    /// Returns the PsiInput object which PsiExEnv uses
    Ref<PsiInput> get_psi_input() const { return psiinput_;};
    /// Returns the PsiFile11 object which PsiExEnv uses
    Ref<PsiFile11> get_psi_file11() const { return psifile11_;};
    
    /// Executes Psi input+driver
    int run_psi();
    /// Executes a Psi module
    int run_psi_module(char *);

    /// Returns current working directory
    string get_cwd() const { return cwd_;};
    /// Returns the Psi file prefix
    string get_fileprefix() const { return fileprefix_; };
    /// Returns the number of scratch locations
    int get_nscratch() const { return nscratch_; };
    /// Returns the ith scratch location
    string get_scratch(int i) const { return scratch_[i]; };
    
    void print(std::ostream&o=ExEnv::out0()) const;
};

}

#endif
