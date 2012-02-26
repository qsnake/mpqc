//
// mbptr12test.cc
//
// Copyright (C) Edward Valeev
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

#ifdef HAVE_CONFIG_H
#include <scconfig.h>
#endif

#include <string.h>

#include <sys/stat.h>
#include <unistd.h>
#include <new>

#include <util/keyval/keyval.h>
#include <util/group/message.h>
#include <util/group/pregtime.h>
#include <util/misc/bug.h>
#include <util/misc/formio.h>
#include <util/state/stateio.h>
#include <util/state/state_bin.h>

#include <math/scmat/repl.h>
#include <math/scmat/dist.h>

#include <chemistry/molecule/coor.h>
#include <chemistry/molecule/energy.h>

#include <chemistry/qc/scf/clhf.h>

#include <chemistry/qc/mbptr12/mbptr12.h>

using namespace std;
using namespace sc;

// Force linkages:
#ifndef __PIC__
static ForceLink<CLHF> fl0a;

static ForceLink<MBPT2_R12> fl0e;

static ForceLink<ReplSCMatrixKit> fl6;
static ForceLink<DistSCMatrixKit> fl7;

# ifdef HAVE_SYSV_IPC
#   include <util/group/messshm.h>
    static ForceLink<ShmMessageGrp> fl8;
# endif
static ForceLink<ProcMessageGrp> fl9;
# ifdef HAVE_NX_H
#  include <util/group/messpgon.h>
    static ForceLink<ParagonMessageGrp> fl10;
# endif
#endif

Ref<RegionTimer> tim;
Ref<MessageGrp> grp;

static Ref<MessageGrp>
init_mp(const Ref<KeyVal>& keyval)
{
  // if we are on a paragon then use a ParagonMessageGrp
  // otherwise read the message group from the input file
  grp << keyval->describedclassvalue("message");

  if (grp.nonnull()) MessageGrp::set_default_messagegrp(grp);
  else grp = MessageGrp::get_default_messagegrp();

  Ref<Debugger> debugger; debugger << keyval->describedclassvalue(":debug");
  // Let the debugger know the name of the executable and the node
  if (debugger.nonnull()) {
    debugger->set_exec("mbptr12test");
    debugger->set_prefix(grp->me());
    debugger->debug("curt is a hog");
  }
  
  tim = new ParallelRegionTimer(grp,"mbptr12test",1,0);
  RegionTimer::set_default_regiontimer(tim);

  SCFormIO::set_printnode(0);
  SCFormIO::init_mp(grp->me());
  //SCFormIO::set_debug(1);

  SCFormIO::setindent(ExEnv::out0(), 2);
  SCFormIO::setindent(cerr, 2);
  
  return grp;
}

main(int argc, char**argv)
{
  const char *input =      (argc > 1)? argv[1] : SRCDIR "/mbptr12test.in";
  const char *keyword =    (argc > 2)? argv[2] : "mole";
  const char *optkeyword = (argc > 3)? argv[3] : "opt";

  // open keyval input
  Ref<KeyVal> rpkv(new ParsedKeyVal(input));

  init_mp(rpkv);

  tim->enter("input");
  
  if (rpkv->exists("matrixkit")) {
    Ref<SCMatrixKit> kit; kit << rpkv->describedclassvalue("matrixkit");
    SCMatrixKit::set_default_matrixkit(kit);
  }
  
  struct stat sb;
  Ref<MolecularEnergy> mole;

  if (stat("mbptr12test.ckpt",&sb)==0 && sb.st_size) {
    StateInBin si("mbptr12test.ckpt");
    //    opt << SavableState::restore_state(si);
    //    mole << opt->function();
  } else {
    mole << rpkv->describedclassvalue(keyword);
  }

  tim->exit("input");

  if (mole.nonnull()) {
    ExEnv::out0() << indent << "energy: " << mole->energy() << endl;
    if (mole->value_implemented()) {
      ExEnv::out0() << indent
		   << scprintf("value of mole is %20.15f\n\n", mole->energy());
    }

    mole->print(ExEnv::out0());
  }

  StateOutBin so("mbptr12test.wfn");
  SavableState::save_state(mole.pointer(),so);
  
  tim->print(ExEnv::out0());

  tim = 0;
  grp = 0;
  RegionTimer::set_default_regiontimer(0);
  MessageGrp::set_default_messagegrp(0);

  return 0;
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End:
