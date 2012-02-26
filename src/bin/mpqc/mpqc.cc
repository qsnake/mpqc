//
// mpqc.cc
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Edward Seidl <seidl@janed.com>
// Maintainer: LPS
//
// This file is part of MPQC.
//
// MPQC is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// MPQC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the MPQC; see the file COPYING.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

// This is needed to make GNU extensions available, such as
// feenableexcept and fedisableexcept.
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include <scconfig.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <scdirlist.h>

#include <new>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>

#include <scconfig.h>
#ifdef HAVE_SSTREAM
#  include <sstream>
#else
#  include <strstream.h>
#endif

#ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
#endif
#ifdef HAVE_SYS_TIME_H
#  include <sys/time.h>
#endif

#include <util/options/GetLongOpt.h>
#include <util/class/scexception.h>
#include <util/misc/newstring.h>
#include <util/keyval/keyval.h>
#include <util/state/state_bin.h>
#include <util/group/message.h>
#include <util/group/memory.h>
#include <util/group/mstate.h>
#include <util/group/thread.h>
#include <util/group/pregtime.h>
#include <util/misc/bug.h>
#include <util/misc/formio.h>
#include <util/misc/exenv.h>
#ifdef HAVE_CHEMISTRY_CCA
  #include <util/misc/ccaenv.h>
#endif
#include <util/render/render.h>

#include <math/optimize/opt.h>

#include <chemistry/molecule/coor.h>
#include <chemistry/molecule/energy.h>
#include <chemistry/molecule/molfreq.h>
#include <chemistry/molecule/fdhess.h>
#include <chemistry/molecule/formula.h>
#include <chemistry/qc/wfn/wfn.h>

// Force linkages:
#include <util/group/linkage.h>
#include <chemistry/qc/wfn/linkage.h>
#include <chemistry/qc/scf/linkage.h>
#include <chemistry/qc/dft/linkage.h>
#include <chemistry/qc/mbpt/linkage.h>
#ifdef HAVE_SC_SRC_LIB_CHEMISTRY_QC_MBPTR12
#  include <chemistry/qc/mbptr12/linkage.h>
#endif
#ifdef HAVE_SC_SRC_LIB_CHEMISTRY_QC_CINTS
#  include <chemistry/qc/cints/linkage.h>
#endif
//#include <chemistry/qc/psi/linkage.h>
#include <util/state/linkage.h>
#ifdef HAVE_SC_SRC_LIB_CHEMISTRY_QC_CC
#  include <chemistry/qc/cc/linkage.h>
#endif
#ifdef HAVE_SC_SRC_LIB_CHEMISTRY_QC_PSI
#  include <chemistry/qc/psi/linkage.h>
#endif
#ifdef HAVE_SC_SRC_LIB_CHEMISTRY_QC_INTCCA
#  include <chemistry/qc/intcca/linkage.h>
#endif

#ifdef HAVE_MPI
#define MPICH_SKIP_MPICXX
#include <mpi.h>
#include <util/group/messmpi.h>
#endif

using namespace std;
using namespace sc;

#include "mpqcin.h"

//////////////////////////////////////////////////////////////////////////

static void
trash_stack_b(int &i, char *&ichar)
{
  char stack;
  ichar = &stack;
  ichar -= 10;
  for (i=0; i<1000; i++) {
    *ichar-- = 0xfe;
  }
}

static void
trash_stack()
{
  int i;
  char *ichar;
  trash_stack_b(i,ichar);
}

static void
clean_up(void)
{
  MemoryGrp::set_default_memorygrp(0);
  MessageGrp::set_default_messagegrp(0);
  ThreadGrp::set_default_threadgrp(0);
  SCMatrixKit::set_default_matrixkit(0);
  Integral::set_default_integral(0);
  RegionTimer::set_default_regiontimer(0);
}

#include <signal.h>

#ifdef HAVE_FENV_H
#  include <fenv.h>
#endif

static void
print_unseen(const Ref<ParsedKeyVal> &parsedkv,
             const char *input)
{
  if (parsedkv->have_unseen()) {
    ExEnv::out0() << endl;
    ExEnv::out0() << indent
         << "The following keywords in \"" << input << "\" were ignored:"
         << endl;
    ExEnv::out0() << incindent;
    parsedkv->print_unseen(ExEnv::out0());
    ExEnv::out0() << decindent;
  }
}

int
try_main(int argc, char *argv[])
{
  //trash_stack();

  KeyValValueboolean truevalue(1), falsevalue(0);
  int i;
  const char *devnull = "/dev/null";
  atexit(clean_up);

#ifdef HAVE_FEENABLEEXCEPT
  // this uses a glibc extension to trap on individual exceptions
# ifdef FE_DIVBYZERO
  feenableexcept(FE_DIVBYZERO);
# endif
# ifdef FE_INVALID
  feenableexcept(FE_INVALID);
# endif
# ifdef FE_OVERFLOW
  feenableexcept(FE_OVERFLOW);
# endif
#endif

#ifdef HAVE_FEDISABLEEXCEPT
  // this uses a glibc extension to not trap on individual exceptions
# ifdef FE_UNDERFLOW
  fedisableexcept(FE_UNDERFLOW);
# endif
# ifdef FE_INEXACT
  fedisableexcept(FE_INEXACT);
# endif
#endif

#if defined(HAVE_SETRLIMIT)
  struct rlimit rlim;
  rlim.rlim_cur = 0;
  rlim.rlim_max = 0;
  setrlimit(RLIMIT_CORE,&rlim);
#endif

  ExEnv::init(argc, argv);

  Ref<MessageGrp> grp;
#if defined(HAVE_MPI) && defined(ALWAYS_USE_MPI)
  grp = new MPIMessageGrp(&argc, &argv);
#endif

  // parse commandline options
  GetLongOpt options;

  options.usage("[options] [filename]");
  options.enroll("f", GetLongOpt::MandatoryValue,
                 "the name of an object format input file", 0);
  options.enroll("o", GetLongOpt::MandatoryValue,
                 "the name of the output file", 0);
  options.enroll("messagegrp", GetLongOpt::MandatoryValue,
                 "which message group to use", 0);
  options.enroll("threadgrp", GetLongOpt::MandatoryValue,
                 "which thread group to use", 0);
  options.enroll("memorygrp", GetLongOpt::MandatoryValue,
                 "which memory group to use", 0);
  options.enroll("integral", GetLongOpt::MandatoryValue,
                 "which integral evaluator to use", 0);
  options.enroll("l", GetLongOpt::MandatoryValue, "basis set limit", "0");
  options.enroll("W", GetLongOpt::MandatoryValue,
                 "set the working directory", ".");
  options.enroll("c", GetLongOpt::NoValue, "check input then exit", 0);
  options.enroll("v", GetLongOpt::NoValue, "print the version number", 0);
  options.enroll("w", GetLongOpt::NoValue, "print the warranty", 0);
  options.enroll("L", GetLongOpt::NoValue, "print the license", 0);
  options.enroll("k", GetLongOpt::NoValue, "print key/value assignments", 0);
  options.enroll("i", GetLongOpt::NoValue, "convert simple to OO input", 0);
  options.enroll("d", GetLongOpt::NoValue, "debug", 0);
  options.enroll("h", GetLongOpt::NoValue, "print this message", 0);
  options.enroll("cca-path", GetLongOpt::OptionalValue, 
                 "cca component path", "");
  options.enroll("cca-load", GetLongOpt::OptionalValue,
                 "cca components to load", "");

  int optind = options.parse(argc, argv);

  const char *output = options.retrieve("o");
  ostream *outstream = 0;
  if (output != 0) {
    outstream = new ofstream(output);
    ExEnv::set_out(outstream);
  }

  if (options.retrieve("h")) {
    ExEnv::out0()
         << indent << "MPQC version " << SC_VERSION << endl
         << indent << "compiled for " << TARGET_ARCH << endl
         << SCFormIO::copyright << endl;
    options.usage(ExEnv::out0());
    exit(0);
  }
  
  if (options.retrieve("v")) {
    ExEnv::out0()
         << indent << "MPQC version " << SC_VERSION << endl
         << indent << "compiled for " << TARGET_ARCH << endl
         << SCFormIO::copyright;
    exit(0);
  }
  
  if (options.retrieve("w")) {
    ExEnv::out0()
         << indent << "MPQC version " << SC_VERSION << endl
         << indent << "compiled for " << TARGET_ARCH << endl
         << SCFormIO::copyright << endl
         << SCFormIO::warranty;
    exit(0);
  }
  
  if (options.retrieve("L")) {
    ExEnv::out0()
         << indent << "MPQC version " << SC_VERSION << endl
         << indent << "compiled for " << TARGET_ARCH << endl
         << SCFormIO::copyright << endl
         << SCFormIO::license;
    exit(0);
  }

  // set the working dir
  if (strcmp(options.retrieve("W"),"."))
    chdir(options.retrieve("W"));

  // initialize keyval input
  const char *object_input = options.retrieve("f");
  const char *generic_input;
  if (argc - optind == 0) {
    generic_input = 0;
  }
  else if (argc - optind == 1) {
    generic_input = argv[optind];
  }
  else {
    options.usage();
    throw invalid_argument("extra arguments given");
  }

  // get the message group.  first try the commandline and environment
  if (grp.null()) grp = MessageGrp::initial_messagegrp(argc, argv);
  if (grp.nonnull())
    MessageGrp::set_default_messagegrp(grp);
  else
    grp = MessageGrp::get_default_messagegrp();

  if (object_input == 0 && generic_input == 0) {
    generic_input = "mpqc.in";
    }
  else if (object_input && generic_input) {
    options.usage();
    throw invalid_argument("only one of -f and a file argument can be given");
    }

  const char *input;
  if (object_input) input = object_input;
  if (generic_input) input = generic_input;

  Ref<ParsedKeyVal> parsedkv;
  // read the input file on only node 0
  char *in_char_array;
  if (grp->me() == 0) {
    ifstream is(input);
#ifdef HAVE_SSTREAM
    ostringstream ostrs;
    is >> ostrs.rdbuf();
    int n = 1 + strlen(ostrs.str().c_str());
    in_char_array = strcpy(new char[n],ostrs.str().c_str());
#else
    ostrstream ostrs;
    is >> ostrs.rdbuf();
    ostrs << ends;
    in_char_array = ostrs.str();
    int n = ostrs.pcount();
#endif
    grp->bcast(n);
    grp->bcast(in_char_array, n);
    }
  else {
    int n;
    grp->bcast(n);
    in_char_array = new char[n];
    grp->bcast(in_char_array, n);
    }

  int use_simple_input;
  if (generic_input && grp->me() == 0) {
    MPQCIn mpqcin;
    use_simple_input = mpqcin.check_string(in_char_array);
  }
  else {
    use_simple_input = 0;
  }
  grp->bcast(use_simple_input);

  if (use_simple_input) {
    MPQCIn mpqcin;
    char *simple_input_text = mpqcin.parse_string(in_char_array);
    if (options.retrieve("i")) {
      ExEnv::out0() << "Generated object-oriented input file:" << endl
                   << simple_input_text
                   << endl;
      exit(0);
    }
    parsedkv = new ParsedKeyVal();
    parsedkv->parse_string(simple_input_text);
    delete[] simple_input_text;
    }
  else {
    parsedkv = new ParsedKeyVal();
    parsedkv->parse_string(in_char_array);
    }
  delete[] in_char_array;

  if (options.retrieve("k")) parsedkv->verbose(1);
  Ref<KeyVal> keyval = new PrefixKeyVal(parsedkv.pointer(),"mpqc");

  // get the basename for output files
  const char *basename_source;
  if (output) basename_source = output;
  else        basename_source = input;
  int nfilebase = (int) (::strrchr(basename_source, '.') - basename_source);
  char *basename = new char[nfilebase + 1];
  strncpy(basename, basename_source, nfilebase);
  basename[nfilebase] = '\0';
  SCFormIO::set_default_basename(basename);

  // set up output classes
  SCFormIO::setindent(ExEnv::outn(), 2);
  SCFormIO::setindent(ExEnv::errn(), 2);
  SCFormIO::setindent(cout, 2);
  SCFormIO::setindent(cerr, 2);

  SCFormIO::set_printnode(0);
  if (grp->n() > 1)
    SCFormIO::init_mp(grp->me());

  if (options.retrieve("d"))
    SCFormIO::set_debug(1);

  // initialize timing for mpqc
  grp->sync(); // make sure nodes are sync'ed before starting timings
  Ref<RegionTimer> tim;
  if (keyval->exists("timer")) tim << keyval->describedclassvalue("timer");
  else                         tim = new ParallelRegionTimer(grp,"mpqc",1,1);
  RegionTimer::set_default_regiontimer(tim);

  if (tim.nonnull()) tim->enter("input");
  
  // announce ourselves
  const char title1[] = "MPQC: Massively Parallel Quantum Chemistry";
  int ntitle1 = sizeof(title1);
  const char title2[] = "Version " SC_VERSION;
  int ntitle2 = sizeof(title2);
  ExEnv::out0() << endl;
  ExEnv::out0() << indent;
  for (i=0; i<(80-ntitle1)/2; i++) ExEnv::out0() << ' ';
  ExEnv::out0() << title1 << endl;
  ExEnv::out0() << indent;
  for (i=0; i<(80-ntitle2)/2; i++) ExEnv::out0() << ' ';
  ExEnv::out0() << title2 << endl << endl;

  const char *tstr = 0;
#if defined(HAVE_TIME) && defined(HAVE_CTIME)
  time_t t;
  time(&t);
  tstr = ctime(&t);
#endif
  if (!tstr) {
    tstr = "UNKNOWN";
  }

  ExEnv::out0()
       << indent << scprintf("Machine:    %s", TARGET_ARCH) << endl
       << indent << scprintf("User:       %s@%s",
                             ExEnv::username(), ExEnv::hostname()) << endl
       << indent << scprintf("Start Time: %s", tstr) << endl;

  // get the thread group.  first try the commandline and environment
  Ref<ThreadGrp> thread = ThreadGrp::initial_threadgrp(argc, argv);
  
  // if we still don't have a group, try reading the thread group
  // from the input
  if (thread.null()) {
    thread << keyval->describedclassvalue("thread");
  }

  if (thread.nonnull())
    ThreadGrp::set_default_threadgrp(thread);
  else
    thread = ThreadGrp::get_default_threadgrp();

  // get the memory group.  first try the commandline and environment
  Ref<MemoryGrp> memory = MemoryGrp::initial_memorygrp(argc, argv);
  
  // if we still don't have a group, try reading the memory group
  // from the input
  if (memory.null()) {
    memory << keyval->describedclassvalue("memory");
  }

  if (memory.nonnull())
    MemoryGrp::set_default_memorygrp(memory);
  else
    memory = MemoryGrp::get_default_memorygrp();

  ExEnv::out0() << indent
       << "Using " << grp->class_name()
       << " for message passing (number of nodes = " << grp->n() << ")." << endl
       << indent
       << "Using " << thread->class_name()
       << " for threading (number of threads = " << thread->nthread() << ")." << endl
       << indent
       << "Using " << memory->class_name()
       << " for distributed shared memory." << endl
       << indent
       << "Total number of processors = " << grp->n() * thread->nthread() << endl;

#ifdef HAVE_CHEMISTRY_CCA
  // initialize cca framework
  KeyValValuestring emptystring("");
  bool do_cca = keyval->booleanvalue("do_cca",falsevalue);

  string cca_path(options.retrieve("cca-path"));
  string cca_load(options.retrieve("cca-load"));
  if(cca_path.size()==0)
    cca_path = keyval->stringvalue("cca_path",emptystring);
  if(cca_load.size()==0)
    cca_load = keyval->stringvalue("cca_load",emptystring);

  if( !do_cca && (cca_load.size() > 0 || cca_path.size() > 0) )
    do_cca = true;

  if(cca_path.size()==0) {
    #ifdef CCA_PATH
      cca_path = CCA_PATH;                                 
    #endif
  }
  if(cca_load.size()==0) {
    cca_load += "MPQC.IntegralEvaluatorFactory";
  }

  if( cca_load.size() > 0 && cca_path.size() > 0 && do_cca ) {
    string cca_args = "--path " + cca_path + " --load " + cca_load;
    ExEnv::out0() << endl << indent << "Initializing CCA framework with args: "
                  << endl << indent << cca_args << endl;
    CCAEnv::init( cca_args );
  }
#endif

  // now set up the debugger
  Ref<Debugger> debugger; debugger << keyval->describedclassvalue("debug");
  if (debugger.nonnull()) {
    Debugger::set_default_debugger(debugger);
    debugger->set_exec(argv[0]);
    debugger->set_prefix(grp->me());
    if (options.retrieve("d"))
      debugger->debug("Starting debugger because -d given on command line.");
  }

  // now check to see what matrix kit to use
  if (keyval->exists("matrixkit"))
    SCMatrixKit::set_default_matrixkit(
      dynamic_cast<SCMatrixKit*>(
        keyval->describedclassvalue("matrixkit").pointer()));

  // get the integral factory. first try commandline and environment
  Ref<Integral> integral = Integral::initial_integral(argc, argv);
  
  // if we still don't have a integral, try reading the integral
  // from the input
  if (integral.null()) {
    integral << keyval->describedclassvalue("integrals");
  }

  if (integral.nonnull())
    Integral::set_default_integral(integral);
  else
    integral = Integral::get_default_integral();

  ExEnv::out0() << endl << indent
       << "Using " << integral->class_name()
       << " by default for molecular integrals evaluation" << endl << endl;
  
  // check for a molecular energy and optimizer
  KeyValValueString molnamedef(basename);
  char * molname = keyval->pcharvalue("filename", molnamedef);
  if (strcmp(molname, basename))
    SCFormIO::set_default_basename(molname);

  char * ckptfile = new char[strlen(molname)+6];
  sprintf(ckptfile,"%s.ckpt",molname);
  
  KeyValValueString restartfiledef(ckptfile);
  char * restartfile = keyval->pcharvalue("restart_file", restartfiledef);
  
  char * wfn_file = keyval->pcharvalue("wfn_file");
  if (wfn_file == 0) {
    wfn_file = new char[strlen(molname)+6];
    sprintf(wfn_file,"%s.wfn",molname);
  }
  char *mole_ckpt_file = new char[strlen(wfn_file)+1];
  sprintf(mole_ckpt_file,"%s",wfn_file);

  int restart = keyval->booleanvalue("restart",truevalue);

  int checkpoint = keyval->booleanvalue("checkpoint",truevalue);
  int checkpoint_freq = keyval->intvalue("checkpoint_freq",KeyValValueint(1));
  
  int savestate = keyval->booleanvalue("savestate",truevalue);

  struct stat sb;
  Ref<MolecularEnergy> mole;
  Ref<Optimize> opt;

  int statresult, statsize;
  if (restart) {
    if (grp->me() == 0) {
      statresult = stat(restartfile,&sb);
      statsize = (statresult==0) ? sb.st_size : 0;
    }
    grp->bcast(statresult);
    grp->bcast(statsize);
  }
  if (restart && statresult==0 && statsize) {
    BcastStateInBin si(grp,restartfile);
    if (keyval->exists("override")) {
      si.set_override(new PrefixKeyVal(keyval,"override"));
    }
    char *suf = strrchr(restartfile,'.');
    if (!strcmp(suf,".wfn")) {
      mole << SavableState::key_restore_state(si,"mole");
      ExEnv::out0() << endl
                   << indent << "Restored <" << mole->class_name()
                   << "> from " << restartfile << endl;

      opt << keyval->describedclassvalue("opt");
      if (opt.nonnull())
        opt->set_function(mole.pointer());
    }
    else {
      opt << SavableState::key_restore_state(si,"opt");
      if (opt.nonnull()) {
        mole << opt->function();
        ExEnv::out0() << endl << indent
             << "Restored <Optimize> from " << restartfile << endl;
      }
    }
  } else {
    mole << keyval->describedclassvalue("mole");
    opt << keyval->describedclassvalue("opt");
  }

  if (mole.nonnull()) {
    MolecularFormula mf(mole->molecule());
    ExEnv::out0() << endl << indent
         << "Molecular formula " << mf.formula() << endl;
    if (checkpoint) {
      mole->set_checkpoint();
      if (grp->me() == 0) mole->set_checkpoint_file(mole_ckpt_file);
      else mole->set_checkpoint_file(devnull);
      mole->set_checkpoint_freq(checkpoint_freq);
    }
  }
  delete[] mole_ckpt_file;

  if (checkpoint && opt.nonnull()) {
    opt->set_checkpoint();
    if (grp->me() == 0) opt->set_checkpoint_file(ckptfile);
    else opt->set_checkpoint_file(devnull);
  }

  // see if frequencies are wanted

  Ref<MolecularHessian> molhess;
  molhess << keyval->describedclassvalue("hess");
  Ref<MolecularFrequencies> molfreq;
  molfreq << keyval->describedclassvalue("freq");
  
  int check = (options.retrieve("c") != 0);
  int limit = atoi(options.retrieve("l"));
  if (limit) {
    Ref<Wavefunction> wfn; wfn << mole;
    if (wfn.nonnull() && wfn->ao_dimension()->n() > limit) {
      ExEnv::out0() << endl << indent
           << "The limit of " << limit << " basis functions has been exceeded."
           << endl;
      check = 1;
    }
  }

  if (check) {
    ExEnv::out0() << endl << indent
         << "Exiting since the check option is on." << endl;
    exit(0);
  }
  
  if (tim.nonnull()) tim->change("calc");

  int do_energy = keyval->booleanvalue("do_energy",truevalue);
  
  int do_grad = keyval->booleanvalue("do_gradient",falsevalue);

  int do_opt = keyval->booleanvalue("optimize",truevalue);
  
  int do_pdb = keyval->booleanvalue("write_pdb",falsevalue);
  
  int print_mole = keyval->booleanvalue("print_mole",truevalue);
  
  int print_timings = keyval->booleanvalue("print_timings",truevalue);

  // see if any pictures are desired
  Ref<Render> renderer;
  renderer << keyval->describedclassvalue("renderer");

  // If we have a renderer, then we will read in some more info
  // below.  Otherwise we can get rid of the keyval's, to eliminate
  // superfluous references to objects that we might otherwise be
  // able to delete.  We cannot read in the remaining rendering
  // objects now, since some of their KeyVal CTOR's are heavyweight,
  // requiring optimized geometries, etc.
  if (renderer.null()) {
    if (parsedkv.nonnull()) print_unseen(parsedkv, input);
    keyval = 0;
    parsedkv = 0;
  }

  // sanity checks for the benefit of reasonable looking output
  if (opt.null()) do_opt=0;
  
  ExEnv::out0() << endl << indent
       << "MPQC options:" << endl << incindent
       << indent << "matrixkit     = <"
       << SCMatrixKit::default_matrixkit()->class_name() << ">" << endl
       << indent << "filename      = " << molname << endl
       << indent << "restart_file  = " << restartfile << endl
       << indent << "restart       = " << (restart ? "yes" : "no") << endl
       << indent << "checkpoint    = " << (checkpoint ? "yes" : "no") << endl
       << indent << "savestate     = " << (savestate ? "yes" : "no") << endl
       << indent << "do_energy     = " << (do_energy ? "yes" : "no") << endl
       << indent << "do_gradient   = " << (do_grad ? "yes" : "no") << endl
       << indent << "optimize      = " << (do_opt ? "yes" : "no") << endl
       << indent << "write_pdb     = " << (do_pdb ? "yes" : "no") << endl
       << indent << "print_mole    = " << (print_mole ? "yes" : "no") << endl
       << indent << "print_timings = " << (print_timings ? "yes" : "no")
       << endl << decindent;

  delete[] restartfile;
  delete[] ckptfile;

  int ready_for_freq = 1;
  if (mole.nonnull()) {
    if (((do_opt && opt.nonnull()) || do_grad)
        && !mole->gradient_implemented()) {
      ExEnv::out0() << indent
           << "WARNING: optimization or gradient requested but the given"
           << endl
           << "         MolecularEnergy object cannot do gradients."
           << endl;
    }

    if (do_opt && opt.nonnull() && mole->gradient_implemented()) {
      int result = opt->optimize();
      if (result) {
        ExEnv::out0() << indent
             << "The optimization has converged." << endl << endl;
        ExEnv::out0() << indent
             << scprintf("Value of the MolecularEnergy: %15.10f",
                         mole->energy())
             << endl << endl;
      } else {
        ExEnv::out0() << indent
             << "The optimization has NOT converged." << endl << endl;
        ready_for_freq = 0;
      }
    } else if (do_grad && mole->gradient_implemented()) {
      mole->do_gradient(1);
      ExEnv::out0() << endl << indent
           << scprintf("Value of the MolecularEnergy: %15.10f",
                       mole->energy())
           << endl;
      if (mole->value_result().actual_accuracy()
          > mole->value_result().desired_accuracy()) {
        ExEnv::out0() << indent
             << "WARNING: desired accuracy not achieved in energy" << endl;
      }
      ExEnv::out0() << endl;
      // Use result_noupdate since the energy might not have converged
      // to the desired accuracy in which case grabbing the result will
      // start up the calculation again.  However the gradient might
      // not have been computed (if we are restarting and the gradient
      // isn't in the save file for example).
      RefSCVector grad;
      if (mole->gradient_result().computed()) {
        grad = mole->gradient_result().result_noupdate();
      }
      else {
        grad = mole->gradient();
      }
      if (grad.nonnull()) {
        grad.print("Gradient of the MolecularEnergy:");
        if (mole->gradient_result().actual_accuracy()
            > mole->gradient_result().desired_accuracy()) {
          ExEnv::out0() << indent
               << "WARNING: desired accuracy not achieved in gradient" << endl;
        }
      }
    } else if (do_energy && mole->value_implemented()) {
      ExEnv::out0() << endl << indent
           << scprintf("Value of the MolecularEnergy: %15.10f",
                       mole->energy())
           << endl << endl;
    }
  }

  if (tim.nonnull()) tim->exit("calc");

  // save this before doing the frequency stuff since that obsoletes the
  // function stuff
  if (savestate) {
    if (opt.nonnull()) {
      if (grp->me() == 0) {
        ckptfile = new char[strlen(molname)+6];
        sprintf(ckptfile,"%s.ckpt",molname);
      }
      else {
        ckptfile = new char[strlen(devnull)+1];
        strcpy(ckptfile, devnull);
      }

      StateOutBin so(ckptfile);
      SavableState::save_state(opt.pointer(),so);
      so.close();

      delete[] ckptfile;
    }

    if (mole.nonnull()) {
      if (grp->me() == 0) {
        if (wfn_file == 0) {
          wfn_file = new char[strlen(molname)+6];
          sprintf(wfn_file,"%s.wfn",molname);
        }
      }
      else {
        delete[] wfn_file;
        wfn_file = new char[strlen(devnull)+1];
        strcpy(wfn_file, devnull);
      }
  
      StateOutBin so(wfn_file);
      SavableState::save_state(mole.pointer(),so);
      so.close();

    }
  }
  delete[] wfn_file;

  // Frequency calculation.
  if (ready_for_freq && molfreq.nonnull()) {
    RefSymmSCMatrix xhessian;
    if (molhess.nonnull()) {
      // if "hess" input was given, use it to compute the hessian
      xhessian = molhess->cartesian_hessian();
    }
    else if (mole->hessian_implemented()) {
      // if mole can compute the hessian, use that hessian
      xhessian = mole->get_cartesian_hessian();
    }
    else if (mole->gradient_implemented()) {
      // if mole can compute gradients, use gradients at finite
      // displacements to compute the hessian
      molhess = new FinDispMolecularHessian(mole);
      xhessian = molhess->cartesian_hessian();
    }
    else {
      ExEnv::out0() << "mpqc: WARNING: Frequencies cannot be computed" << endl;
    }

    if (xhessian.nonnull()) {
      char *hessfile = SCFormIO::fileext_to_filename(".hess");
      MolecularHessian::write_cartesian_hessian(hessfile,
                                                mole->molecule(), xhessian);
      delete[] hessfile;

      molfreq->compute_frequencies(xhessian);
      // DEGENERACY IS NOT CORRECT FOR NON-SINGLET CASES:
      molfreq->thermochemistry(1);
    }
  }

  if (renderer.nonnull()) {
    Ref<RenderedObject> rendered;
    rendered << keyval->describedclassvalue("rendered");
    Ref<AnimatedObject> animated;
    animated << keyval->describedclassvalue("rendered");
    if (rendered.nonnull()) {
      if (tim.nonnull()) tim->enter("render");
      if (grp->me() == 0) renderer->render(rendered);
      if (tim.nonnull()) tim->exit("render");
    }
    else if (animated.nonnull()) {
      if (tim.nonnull()) tim->enter("render");
      if (grp->me() == 0) renderer->animate(animated);
      if (tim.nonnull()) tim->exit("render");
    }
    else {
      if (tim.nonnull()) tim->enter("render");
      int n = keyval->count("rendered");
      for (i=0; i<n; i++) {
        rendered << keyval->describedclassvalue("rendered",i);
        animated << keyval->describedclassvalue("rendered",i);
        if (rendered.nonnull()) {
          // make sure the object has a name so we don't overwrite its file
          if (rendered->name() == 0) {
            char ic[64];
            sprintf(ic,"%02d",i);
            rendered->set_name(ic);
          }
          if (grp->me() == 0) renderer->render(rendered);
        }
        else if (animated.nonnull()) {
          // make sure the object has a name so we don't overwrite its file
          if (animated->name() == 0) {
            char ic[64];
            sprintf(ic,"%02d",i);
            animated->set_name(ic);
          }
          if (grp->me() == 0) renderer->animate(animated);
        }
      }
      if (tim.nonnull()) tim->exit("render");
    }
    Ref<MolFreqAnimate> molfreqanim;
    molfreqanim << keyval->describedclassvalue("animate_modes");
    if (ready_for_freq && molfreq.nonnull()
        && molfreqanim.nonnull()) {
      if (tim.nonnull()) tim->enter("render");
      molfreq->animate(renderer, molfreqanim);
      if (tim.nonnull()) tim->exit("render");
    }
  }

  if (mole.nonnull()) {
    if (print_mole)
      mole->print(ExEnv::out0());

    if (do_pdb && grp->me() == 0) {
      ckptfile = new char[strlen(molname)+5];
      sprintf(ckptfile, "%s.pdb", molname);
      ofstream pdbfile(ckptfile);
      mole->molecule()->print_pdb(pdbfile);
      delete[] ckptfile;
    }
    
  }
  else {
    ExEnv::out0() << "mpqc: The molecular energy object is null" << endl
         << " make sure \"mole\" specifies a MolecularEnergy derivative"
         << endl;
  }

  if (parsedkv.nonnull()) print_unseen(parsedkv, input);

  if (print_timings)
    if (tim.nonnull()) tim->print(ExEnv::out0());

  delete[] basename;
  delete[] molname;
  SCFormIO::set_default_basename(0);

  renderer = 0;
  molfreq = 0;
  molhess = 0;
  opt = 0;
  mole = 0;
  integral = 0;
  debugger = 0;
  thread = 0;
  tim = 0;
  keyval = 0;
  parsedkv = 0;
  grp = 0;
  memory = 0;
  clean_up();

#if defined(HAVE_TIME) && defined(HAVE_CTIME)
  time(&t);
  tstr = ctime(&t);
#endif
  if (!tstr) {
    tstr = "UNKNOWN";
  }
  ExEnv::out0() << endl
               << indent << scprintf("End Time: %s", tstr) << endl;

  if (output != 0) {
    ExEnv::set_out(&cout);
    delete outstream;
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  try {
    try_main(argc, argv);
  }
  catch (SCException &e) {
    cout << argv[0] << ": ERROR: SC EXCEPTION RAISED:" << endl
         << e.what()
         << endl;
    clean_up();
    throw;
  }
  catch (bad_alloc &e) {
    cout << argv[0] << ": ERROR: MEMORY ALLOCATION FAILED:" << endl
         << e.what()
         << endl;
    clean_up();
    throw;
  }
  catch (exception &e) {
    cout << argv[0] << ": ERROR: EXCEPTION RAISED:" << endl
         << e.what()
         << endl;
    clean_up();
    throw;
  }
  catch (...) {
    cout << argv[0] << ": ERROR: UNKNOWN EXCEPTION RAISED" << endl;
    clean_up();
    throw;
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
