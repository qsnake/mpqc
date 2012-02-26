//
// shellpairs.cc
//
// Copyright (C) 2001 Edward Valeev
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
#pragma implementation
#endif

#include <util/state/statein.h>
#include <util/state/stateout.h>
#include <util/misc/formio.h>
#include <util/ref/ref.h>
#include <chemistry/qc/basis/gaussshell.h>
#include <chemistry/qc/basis/integral.h>
#include <chemistry/qc/cints/shellpairs.h>
#include <chemistry/qc/cints/primpairs.h>

using namespace std;
using namespace sc;

inline int max(int a,int b) { return (a > b) ? a : b;}
inline void fail()
{
  ExEnv::errn() << scprintf("failing module:\n%s",__FILE__) << endl;
  abort();
}

/*----------------
  ShellPairsCints
  ----------------*/
static ClassDesc ShellPairCints_cd(
  typeid(ShellPairCints),"ShellPairCints",1,"virtual public SavableState",
  0, 0, create<ShellPairCints>);

ShellPairCints::ShellPairCints(Ref<PrimPairsCints>& p)
{
  prim_pairs_ = p;
}

ShellPairCints::ShellPairCints(StateIn& si) :
  SavableState(si)
{
  si.get(prim1_offset_);
  si.get(prim2_offset_);
}

ShellPairCints::~ShellPairCints()
{
}

void ShellPairCints::save_data_state(StateOut& so)
{
  so.put(prim1_offset_);
  so.put(prim2_offset_);
}

void ShellPairCints::init(const int p1_offset, const int p2_offset)
{
  prim1_offset_ = p1_offset;
  prim2_offset_ = p2_offset;
}

/*----------------
  ShellPairsCints
  ----------------*/
static ClassDesc ShellPairsCints_cd(
  typeid(ShellPairsCints),"ShellPairsCints",1,"virtual public SavableState",
  0, 0, create<ShellPairsCints>);

ShellPairsCints::ShellPairsCints(const Ref<GaussianBasisSet>& bs1,
				 const Ref<GaussianBasisSet>& bs2)
{
  bs1_ = bs1;
  bs2_ = bs2;
  prim_pairs_ = new PrimPairsCints(bs1_,bs2_);
  shell_pair_ = new ShellPairCints(prim_pairs_);
}

ShellPairsCints::ShellPairsCints(const Ref<ShellPairsCints>& sp)
{
  bs1_ = sp->bs1_;
  bs2_ = sp->bs2_;
  prim_pairs_ = sp->prim_pairs_;
  shell_pair_ = new ShellPairCints(prim_pairs_);
}

ShellPairsCints::ShellPairsCints(StateIn& si) :
  SavableState(si)
{
  bs1_ << SavableState::restore_state(si);
  bs2_ << SavableState::restore_state(si);
  prim_pairs_ = new PrimPairsCints(bs1_,bs2_);
  shell_pair_ << SavableState::restore_state(si);
  init_shell_pair(*this);
}

ShellPairsCints::~ShellPairsCints()
{
}

void ShellPairsCints::save_data_state(StateOut& so)
{
  SavableState::save_state(bs1_.pointer(),so);
  SavableState::save_state(bs2_.pointer(),so);
  SavableState::save_state(shell_pair_.pointer(),so);
}

namespace sc {
void init_shell_pair(ShellPairsCints& sp)
{ sp.shell_pair_->prim_pairs_ = sp.prim_pairs_; }
}


/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End:
