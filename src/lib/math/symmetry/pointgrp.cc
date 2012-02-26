//
// pointgrp.cc
//
// Modifications are
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Edward Seidl <seidl@janed.com>
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

/* pointgrp.cc -- implementation of the point group classes
 *
 *      THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
 *      "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
 *      AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
 *      CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
 *      PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
 *      RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.
 *
 *  Author:
 *      E. T. Seidl
 *      Bldg. 12A, Rm. 2033
 *      Computer Systems Laboratory
 *      Division of Computer Research and Technology
 *      National Institutes of Health
 *      Bethesda, Maryland 20892
 *      Internet: seidl@alw.nih.gov
 *      June, 1993
 */

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <util/misc/formio.h>
#include <util/state/stateio.h>
#include <math/symmetry/pointgrp.h>

using namespace std;
using namespace sc;

////////////////////////////////////////////////////////////////////////

static ClassDesc PointGroup_cd(
  typeid(PointGroup),"PointGroup",2,"public SavableState",
  create<PointGroup>, create<PointGroup>, create<PointGroup>);

PointGroup::PointGroup()
  : symb(0)
{
  set_symbol("c1");
  frame(0,0) = frame(1,1) = frame(2,2) = 1;
  origin_[0] = origin_[1] = origin_[2] =0;
}

PointGroup::PointGroup(const char *s)
  : symb(0)
{
  set_symbol(s);
  frame(0,0) = frame(1,1) = frame(2,2) = 1;
  origin_[0] = origin_[1] = origin_[2] =0;
}

PointGroup::PointGroup(const char *s, SymmetryOperation& so)
  : symb(0)
{
  set_symbol(s);
  frame = so;
  origin_[0] = origin_[1] = origin_[2] =0;
}

PointGroup::PointGroup(const char *s, SymmetryOperation& so,
                       const SCVector3& origin)
  : symb(0)
{
  set_symbol(s);
  frame = so;
  origin_ = origin;
}

PointGroup::PointGroup(const Ref<KeyVal>& kv)
  : symb(0)
{
  if (kv->exists("symmetry")) {
    char *tmp = kv->pcharvalue("symmetry");
    set_symbol(tmp);
    delete[] tmp;
  }
  else
    set_symbol("c1");

  if (kv->exists("symmetry_frame")) {
    for (int i=0; i < 3; i++)
      for (int j=0; j < 3; j++) 
        frame(i,j) = kv->doublevalue("symmetry_frame",i,j);
  } else {
    frame(0,0) = frame(1,1) = frame(2,2) = 1;
  }

  if (kv->exists("origin")) {
    for (int i=0; i < 3; i++)
      origin_[i] = kv->doublevalue("origin",i);
  } else {
    origin_[0] = origin_[1] = origin_[2] =0;
  }
}

PointGroup::PointGroup(StateIn& si) :
  SavableState(si),
  symb(0)
{
  int i;
  if (si.version(::class_desc<PointGroup>()) < 2) {
    ExEnv::errn() << "PointGroup: checkpoint file is too old: cannot read"
                 << endl;
    abort();
  }
  else {
    for (i=0; i<3; i++) si.get(origin_[i]);
  }

  si.getstring(symb);
  for (i=0; i < 3; i++)
    for (int j=0; j < 3; j++)
      si.get(frame(i,j));
}

PointGroup::PointGroup(const PointGroup& pg)
  : symb(0)
{
  *this = pg;
}

PointGroup::PointGroup(const Ref<PointGroup>& pg)
  : symb(0)
{
  *this = *pg.pointer();
}

PointGroup::~PointGroup()
{
  if (symb) { delete[] symb; symb=0; }
}

PointGroup&
PointGroup::operator=(const PointGroup& pg)
{
  set_symbol(pg.symb);
  frame = pg.frame;
  origin_ = pg.origin_;
  return *this;
}

void
PointGroup::set_symbol(const char *sym)
{
  if (sym) {
    if (symb) delete[] symb;
    int len;
    symb = new char[(len=strlen(sym))+1];
    for (int i=0; i<len; i++) symb[i] = (char) tolower(sym[i]);
    symb[len] = '\0';
  } else {
    set_symbol("c1");
  }
}

void
PointGroup::save_data_state(StateOut& so)
{
  int i;
  for (i=0; i<3; i++) so.put(origin_[i]);

  so.putstring(symb);

  for (i=0; i < 3; i++)
    for (int j=0; j < 3; j++)
      so.put(frame(i,j));
}

CharacterTable
PointGroup::char_table() const
{
  CharacterTable ret(symb,frame);
  return ret;
}

int
PointGroup::equiv(const Ref<PointGroup> &grp, double tol) const
{
  if (strcmp(symb,grp->symb)) return 0;

  for (int i=0; i < 3; i++) {
    // origin isn't realy used, so don't check
    //if (fabs(origin_[i] - grp->origin_[i]) > tol) return 0;
    for (int j=0; j < 3; j++) {
      if (fabs(frame(i,j) - grp->frame(i,j)) > tol) return 0;
    }
  }

  return 1;
}

void
PointGroup::print(ostream &o) const
{
  int i,j;

  o << indent << "symmetry = " << symb << endl;

  int unit_frame = 1;
  int zero_origin = 1;
  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      if (i==j && fabs(frame(i,j)-1.0) > 1.0e-10) unit_frame = 0;
      else if (i != j && fabs(frame(i,j)) > 1.0e-10) unit_frame = 0;
    }
    if (fabs(origin_[i]) > 1.0e-10) zero_origin = 0;
  }

  if (!unit_frame) {
    o << indent << "symmetry_frame = [";
    o << incindent;
    for (i=0; i<3; i++) {
      o << endl << indent;
      o << "[";
      for (j=0; j<3; j++) {
        o << scprintf(" % 18.16f", frame(i,j));
      }
      o << "]";
    }
    o << "]" << endl;
    o << decindent;
  }

  if (!zero_origin) {
    o << indent << "origin = [";
    for (i=0; i<3; i++) {
      o << scprintf(" % 18.16f", origin_[i]);
    }
    o << "]" << endl;
  }
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
