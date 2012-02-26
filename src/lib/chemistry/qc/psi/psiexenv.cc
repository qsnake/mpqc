
#ifdef __GNUG__
#pragma implementation
#endif

#include <string>
#include <string.h>
#include <stdlib.h>
#include <scconfig.h>
#include <util/ref/ref.h>
#include <util/keyval/keyval.h>
#include <util/misc/formio.h>
#include <chemistry/qc/psi/psiexenv.h>

using namespace std;

namespace sc {

static ClassDesc PsiExEnv_cd(
  typeid(PsiExEnv),"PsiExEnv",1,"public DescribedClass",
  0, create<PsiExEnv>, 0);

string PsiExEnv::inputname_("input.dat");
string PsiExEnv::file11name_("file11.dat");
int PsiExEnv::ckptfile_(32);
string PsiExEnv::defaultcwd_("/tmp");
string PsiExEnv::defaultfileprefix_("psi");
string PsiExEnv::defaultpsiprefix_("/usr/local/psi/bin");
string PsiExEnv::defaultstdout_("psi.stdout");
string PsiExEnv::defaultstderr_("psi.stderr");

PsiExEnv::PsiExEnv(const Ref<KeyVal>& keyval)
{
  // Find Psi
  char *psibin = getenv("PSIBIN");
  if (psibin)
    psiprefix_ = string(psibin);
  else
    psiprefix_ = string(defaultpsiprefix_);
  add_to_path(psiprefix_);

  char *cwdchar = keyval->pcharvalue("cwd");
  if (cwdchar)
    cwd_ = string(cwdchar);
  else
    cwd_ = string(defaultcwd_);
  char *fileprefixchar = keyval->pcharvalue("fileprefix");
  if (fileprefixchar)
    fileprefix_ = string(fileprefixchar);
  else
    fileprefix_ = string(defaultfileprefix_);

  char *stdout_char = keyval->pcharvalue("stdout");
  if (stdout_char)
    stdout_ = string(stdout_char);
  else
    stdout_ = string(defaultstdout_);
  delete[] stdout_char;
  char *stderr_char = keyval->pcharvalue("stderr");
  if (stderr_char)
    stderr_ = string(stderr_char);
  else
    stderr_ = string(defaultstderr_);
  delete[] stderr_char;

  nscratch_ = keyval->intvalue("nscratch");
  if (nscratch_ != keyval->count("scratch")) {
    ExEnv::err0() << indent
		  << "PsiExEnv: number of scratch directories != nscratch\n";
    abort();
  }
  scratch_ = new string[nscratch_];
  for (int i=0; i<nscratch_; i++)
    scratch_[i] = string(keyval->pcharvalue("scratch",i));

  char *s = new char[cwd_.size() + inputname_.size() + 2];
  sprintf(s,"%s/%s",cwd_.c_str(),inputname_.c_str());
  psiinput_ = new PsiInput(s);
  delete[] s;

  s = new char[cwd_.size() + fileprefix_.size() + file11name_.size() + 3];
  sprintf(s,"%s/%s.%s",cwd_.c_str(),fileprefix_.c_str(),file11name_.c_str());
  psifile11_ = new PsiFile11(s);
  delete[] s;
}

PsiExEnv::PsiExEnv(char *cwd, char *fileprefix, int nscratch, char **scratch):
    cwd_(cwd), fileprefix_(fileprefix), nscratch_(nscratch)
{
  // Find Psi
  char *psibin = 0;
  psibin = getenv("PSIBIN");
  if (psibin)
    psiprefix_ = string(psibin);
  else
    psiprefix_ = string(defaultpsiprefix_);
  add_to_path(psiprefix_);

  scratch_ = new string[nscratch_];
  for(int i=0; i<nscratch_; i++)
    scratch_[i] = string(scratch[i]);
  
  char *s = new char[cwd_.size() + inputname_.size() + 2];
  sprintf(s,"%s/%s",cwd_.c_str(),inputname_.c_str());
  psiinput_ = new PsiInput(s);
  delete[] s;

  s = new char[cwd_.size() + fileprefix_.size() + file11name_.size() + 3];
  sprintf(s,"%s/%s.%s",cwd_.c_str(),fileprefix_.c_str(),file11name_.c_str());
  psifile11_ = new PsiFile11(s);
  delete[] s;
}

PsiExEnv::~PsiExEnv()
{
  delete[] scratch_;
}

void PsiExEnv::add_to_path(const string& dir)
{
  if (dir.size()) {
    char *path = getenv("PATH");
    int newpath_len = strlen(path) + dir.size() + 1;
    char *newpath = new char[newpath_len];
    sprintf(newpath,"%s:%s",dir.c_str(),path);
#ifdef HAVE_SETENV
    setenv("PATH",newpath,1);
#else
    string putenvstr("PATH=");
    putenvstr += newpath;
    char *putenvcstr = strcpy(new char[putenvstr.size()+1], putenvstr.c_str());
    putenv(putenvcstr);
#endif
    delete[] newpath;
  }
}

int PsiExEnv::run_psi()
{
  int errcod;
  if (errcod = run_psi_module("psi3")) {
    return errcod;
  }
  if (errcod = run_psi_module("psiclean")) {
    return errcod;
  }
  return 0;
}

int PsiExEnv::run_psi_module(char *module)
{
  int errcod;
  char *module_cmd = new char[2*cwd_.size()+strlen(module)+psiprefix_.size()+fileprefix_.size()+stdout_.size()+stderr_.size()+40];
  sprintf(module_cmd,"cd %s; %s/%s -p %s/%s 1>> %s 2>> %s",cwd_.c_str(),psiprefix_.c_str(),module,cwd_.c_str(),
	  fileprefix_.c_str(),stdout_.c_str(),stderr_.c_str());
  if (errcod = system(module_cmd)) {
      ExEnv::outn() << "PsiExEnv::run_psi_module -- module " << module << " failed" << endl;
      abort();
  }
  return errcod;
}

void PsiExEnv::print(std::ostream&o) const
{
  o << endl;
  o << indent << "PsiExEnv:" << endl << incindent;
  o << indent << "Location of Psi: " << psiprefix_ << endl;
  o << indent << "Current Psi Working Directory: " << cwd_ << endl;
  o << indent << "Current Psi File Prefix: " << fileprefix_ << endl;
  o << indent << "Number of Scratch Groups: " << nscratch_ << endl;
  for(int i=0; i<nscratch_; i++)
    o << indent << "Scratch Group " << i << ": " << scratch_[i] << endl;
  o << endl << decindent;
}

}
