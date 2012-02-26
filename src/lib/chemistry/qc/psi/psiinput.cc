/*
**
** PSI Input Class
**
** This helper class will set up input decks for the PSI suite of
** ab initio quantum chemistry programs.
**
** David Sherrill & Justin Fermann
** Center for Computational Quantum Chemistry, University of Georgia
**
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include <iostream>
#include <math.h>

#include <util/misc/string.h>
#include <util/misc/formio.h>
#include <math/symmetry/corrtab.h>
#include <chemistry/qc/wfn/obwfn.h>
#include <chemistry/molecule/molecule.h>
#include <chemistry/molecule/atominfo.h>
#include <chemistry/qc/basis/basis.h>
#include <chemistry/qc/basis/petite.h>
#include <chemistry/qc/psi/psiexenv.h>
#include <chemistry/qc/psi/psiinput.h>

using namespace std;

namespace sc {

PsiInput::PsiInput(const string& name) : file_()
{
  filename_ = string(name);
  indentation_ = 0;
}

PsiInput::~PsiInput()
{
}

void
PsiInput::open()
{
  file_.open(filename_.c_str(),ios::out);
  indentation_ = 0;
}

void
PsiInput::close()
{
  file_.close();
  indentation_ = 0;
}


void 
PsiInput::write_indent()
{
  for (int i=0; i<indentation_; i++)
    file_ << " ";
}

void
PsiInput::incindent(int i)
{
  if (i > 0)
    indentation_ += i;
}

void
PsiInput::decindent(int i)
{
  if (i > 0)
    indentation_ -= i;
}

void
PsiInput::begin_section(const char * s)
{
   write_indent();
   file_ << s << ":(" << endl;
   incindent(2);
}

void
PsiInput::end_section(void)
{
   decindent(2);
   write_indent();
   file_ << ")" << endl;
   write_string("\n");
}

void
PsiInput::write_keyword(const char *keyword, const char *value)
{
   write_indent();
   file_ << scprintf("%s = %s",keyword,value) << endl;
}

void
PsiInput::write_keyword(const char *keyword, int value)
{
   write_indent();
   file_ << scprintf("%s = %d",keyword,value) << endl;
}

void
PsiInput::write_keyword(const char *keyword, double value)
{
   write_indent();
   file_ << scprintf("%s = %20.15lf",keyword,value) << endl;
}

void
PsiInput::write_keyword_array(const char *keyword, int num, int *values)
{
  write_indent();
  file_ << scprintf("%s = (", keyword);
  for (int i=0; i<num; i++) {
    file_ << scprintf(" %d", values[i]);
  }
  file_ << ")" << endl;
}

void
PsiInput::write_keyword_array(const char *keyword, int num, double *values)
{
   write_indent();
   file_ << scprintf("%s = (", keyword);
  for (int i=0; i<num; i++) {
    file_ << scprintf(" %20.15lf", values[i]);
  }
  file_ << ")" << endl;
}

void 
PsiInput::write_string(const char *s)
{
   write_indent();
   file_ << s;
}

void
PsiInput::write_key_wq(const char *keyword, const char *value)
{
   write_indent();
   file_ << scprintf("%s = \"%s\"", keyword, value) << endl;
}


void
PsiInput::write_geom(const Ref<Molecule>& mol)
{
  // If the highest symmetry group is not the actual group - use subgroup keyword
  if (!mol->point_group()->equiv(mol->highest_point_group())) {
    write_keyword("subgroup",mol->point_group()->symbol());
  }

  write_keyword("units","bohr");
  write_string("geometry = (\n");
  for (int i=0; i < mol->natom(); i++) {
    write_string("  (");
    char *s;
    file_ << mol->atom_symbol(i) <<
	scprintf(" %14.12lf %14.12lf %14.12lf",mol->r(i,0),mol->r(i,1),mol->r(i,2))
	  << ")" << endl;
  } 
  write_string(")\n");
}


void
PsiInput::write_basis(const Ref<GaussianBasisSet>& basis)
{
  Ref<Molecule> molecule = basis->molecule();
  int natom = molecule->natom();

  write_string("basis = (\n");
  incindent(2);
  for(int atom=0; atom<natom; atom++) {
    int uatom = molecule->atom_to_unique(atom);

    // Replace all spaces with underscores in order for Psi libipv1 to parse properly
    char *name = strdup(basis->name());
    int len = strlen(name);
    for (int i=0; i<len; i++)
      if (name[i] == ' ')
	name[i] = '_';

    char *basisname = new char[strlen(basis->name()) + ((int)ceil(log10((long double)uatom+2))) + 5];
    sprintf(basisname,"\"%s%d\" \n",name,uatom);
    write_string(basisname);
    delete[] name;
  }
  decindent(2);
  write_string(")\n");
}

void
PsiInput::write_basis_sets(const Ref<GaussianBasisSet>& basis)
{
  begin_section("basis");
  Ref<Molecule> molecule = basis->molecule();
  Ref<AtomInfo> atominfo = basis->molecule()->atominfo();
  int nunique = molecule->nunique();

  for(int uatom=0; uatom<nunique; uatom++) {
    int atom = molecule->unique(uatom);
    std::string atomname = atominfo->name(molecule->Z(atom));

    // Replace all spaces with underscores in order for Psi libipv1 to parse properly
    char *name = strdup(basis->name());
    int len = strlen(name);
    for (int i=0; i<len; i++)
      if (name[i] == ' ')
	name[i] = '_';

    char *psibasisname = new char[atomname.size() + strlen(basis->name()) + ((int)ceil(log10((long double)uatom+2))) + 9];
    sprintf(psibasisname,"%s:\"%s%d\" = (\n",atomname.c_str(),name,uatom);
    write_string(psibasisname);
    delete[] name;
    incindent(2);
    int nshell = basis->nshell_on_center(atom);
    for(int sh=0;sh<nshell;sh++) {
      int shell = basis->shell_on_center(atom,sh);
      GaussianShell& Shell = basis->shell(shell);
      int ncon = Shell.ncontraction();
      int nprim = Shell.nprimitive();
      for(int con=0; con<ncon; con++) {
	char amstring[4];
	sprintf(amstring,"(%c\n",Shell.amchar(con));
	write_string(amstring);
	incindent(2);
	for(int prim=0; prim<nprim; prim++) {
	  char primstring[50];
	  sprintf(primstring,"(%20.10lf    %20.10lf)\n",
		  Shell.exponent(prim),
		  Shell.coefficient_norm(con,prim));
	  write_string(primstring);
	}
	decindent(2);
	write_string(")\n");
      }
    }
    decindent(2);
    write_string(")\n");
    delete[] psibasisname;
  }
  end_section();
}

void
PsiInput::write_defaults(const Ref<PsiExEnv>& exenv, const char *dertype)
{
  begin_section("psi");
  
  write_key_wq("label"," ");
  write_keyword("dertype",dertype);
  begin_section("files");
  begin_section("default");
  write_key_wq("name",(exenv->get_fileprefix()).c_str());
  int nscratch = exenv->get_nscratch();
  write_keyword("nvolume",nscratch);
  char *scrname; scrname = new char[10];
  for(int i=0; i<nscratch; i++) {
    sprintf(scrname,"volume%d",i+1);
    write_key_wq(scrname,(exenv->get_scratch(i)).c_str());
  }
  delete[] scrname;
  end_section();
  write_string("file32: ( nvolume = 1 volume1 = \"./\" )\n");
  end_section();

  end_section();
}


void
PsiInput::print(ostream& o)
{
}

}
