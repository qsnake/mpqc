//
// chartab.cc
//
// Modifictions are
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

/* chartab.cc -- implementation of the point group classes
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

#include <ctype.h>

#include <util/misc/formio.h>
#include <util/misc/newstring.h>
#include <math/symmetry/pointgrp.h>

using namespace std;
using namespace sc;

////////////////////////////////////////////////////////////////////////

CharacterTable::CharacterTable()
  : g(0), nt(0), pg(C1), nirrep_(0), gamma_(0), symop(0), _inv(0), symb(0)
{
}

CharacterTable::CharacterTable(const CharacterTable& ct)
  : g(0), nt(0), pg(C1), nirrep_(0), gamma_(0), symop(0), _inv(0), symb(0)
{
  *this = ct;
}

CharacterTable::~CharacterTable()
{
  if (symb) delete[] symb; symb=0;
  if (gamma_) delete[] gamma_; gamma_=0;
  if (symop) delete[] symop; symop=0;
  if (_inv) delete[] _inv; _inv=0;
  g=nt=nirrep_=0;
}

CharacterTable&
CharacterTable::operator=(const CharacterTable& ct)
{
  g=ct.g; nt=ct.nt; pg=ct.pg; nirrep_=ct.nirrep_;
  
  if (symb)
    delete[] symb;

  symb = new_string(ct.symb);

  if (gamma_) delete[] gamma_; gamma_=0;
  if (ct.gamma_) {
    gamma_ = new IrreducibleRepresentation[nirrep_];
    for (int i=0; i < nirrep_; i++) {
      gamma_[i].init();
      gamma_[i] = ct.gamma_[i];
    }
  }

  if (symop)
    delete[] symop;
  symop=0;

  if (ct.symop) {
    symop = new SymmetryOperation[g];
    for (int i=0; i < g; i++) {
      symop[i] = ct.symop[i];
    }
  }

  if (_inv)
    delete[] _inv;
  _inv=0;

  if (ct._inv) {
    _inv = new int[g];
    memcpy(_inv,ct._inv,sizeof(int)*g);
  }

  return *this;
}

void
CharacterTable::print(ostream& os) const
{
  if (!g || !nirrep_) return;

  int i;

  os << indent << "point group " << symb << endl << endl;

  for (i=0; i < nirrep_; i++)
    gamma_[i].print(os);

  os << endl << indent << "symmetry operation matrices:"
     << endl << endl << incindent;
  for (i=0; i < g; i++)
    symop[i].print(os);

  os << decindent << indent << "inverse symmetry operation matrices:"
     << endl << endl << incindent;
  for (i=0; i < g; i++)
    symop[inverse(i)].print(os);

  os << decindent;
}

CharacterTable::CharacterTable(const char *cpg, const SymmetryOperation& frame)
  : g(0), nt(0), pg(C1), nirrep_(0), gamma_(0), symop(0), _inv(0), symb(0)
{
  // first parse the point group symbol, this will give us the order of the
  // point group(g), the type of point group (pg), the order of the principle
  // rotation axis (nt), and the number of irreps (nirrep_)

  if (!cpg) {
    ExEnv::errn() << "CharacterTable::CharacterTable: null point group" << endl;
    exit(1);
  }

  symb = new char[strlen(cpg)+1];
  size_t i;
  for (i=0; i < strlen(cpg); i++) symb[i] = tolower(cpg[i]);
  symb[i] = '\0';

  if (parse_symbol() < 0) {
    ExEnv::errn() << "CharacterTable::CharacterTable: invalid point group "
         << cpg << endl;
    exit(1);
  }

  if (make_table() < 0) {
    ExEnv::errn() << "CharacterTable::CharacterTable: could not make table" << endl;
    exit(1);
  }

  int ig;
  for (ig=0; ig < g; ig++)
    symop[ig] = symop[ig].transform(frame);
}

CharacterTable::CharacterTable(const char *cpg)
  : g(0), nt(0), pg(C1), nirrep_(0), gamma_(0), symop(0), _inv(0), symb(0)
{
  // first parse the point group symbol, this will give us the order of the
  // point group(g), the type of point group (pg), the order of the principle
  // rotation axis (nt), and the number of irreps (nirrep_)

  if (!cpg) {
    ExEnv::errn() << "CharacterTable::CharacterTable: null point group" << endl;
    exit(1);
  }

  symb = new char[strlen(cpg)+1];
  size_t i;
  for (i=0; i < strlen(cpg); i++) symb[i] = tolower(cpg[i]);
  symb[i] = '\0';

  if (parse_symbol() < 0) {
    ExEnv::errn() << "CharacterTable::CharacterTable: invalid point group "
         << cpg << endl;
    exit(1);
  }

  if (make_table() < 0) {
    ExEnv::errn() << "CharacterTable::CharacterTable: could not make table" << endl;
    exit(1);
  }
}

int
CharacterTable::parse_symbol()
{
  // default to C1 symmetry
  g=1; pg=C1; nt=1; nirrep_=1;

  if (!symb) return 0;

  if (!strcmp(symb,"c1")) return 0;

  if (!strcmp(symb,"ci")) {
    g = 2; pg = CI; nirrep_ = 2; nt = 2;
    return 0;
  }

  if(!strcmp(symb,"cs")) {
    g = 2; pg = CS; nirrep_ = 2; nt = 0;
    return 0;
  }

  if (symb[0] == 'c') {
    int nab,ne;

    if (symb[1] == '\0') return -1;

    nt = atoi(&symb[1]);
    ne = (nt%2) ? nt/2 : nt/2-1;
    nab = (nt%2) ? 1 : 2;

    char *vhd = &symb[1];
    while (*vhd && isdigit(*vhd))
      vhd++;
    
    if (*vhd) {
      if (*vhd == 'v') {
        g  = 2*nt; pg = CNV; nirrep_ = 2*nab + ne;
      } else if (*vhd == 'h') {
        g  = 2*nt; pg = CNH; nirrep_ = 2*(nab+ne);
      } else {
        return -1;
      }
    } else {
      g = nt; pg = CN; nirrep_ = nab+ne;
    }

    return 0;
  }

  if (symb[0] == 'd') {
    int nab,ne;

    if (symb[1] == '\0') return -1;

    nt = atoi(&symb[1]);
    ne = (nt%2) ? nt/2 : nt/2-1;
    nab = (nt%2) ? 1 : 2;

    char *vhd = &symb[1];
    while (*vhd && isdigit(*vhd))
      vhd++;
    
    if (*vhd) {
      if (*vhd == 'd') {
        g = 4*nt; pg = DND; nirrep_ = nt+3;
      } else if (*vhd == 'h') {
        g = 4*nt; pg = DNH; nirrep_ = 4*nab + 2*ne;
      } else {
        return -1;
      }
    } else {
      g = 2*nt; pg = DN; nirrep_ = 2*nab + ne;
    }

    return 0;
  }

  if (symb[0] == 's') {
    if (symb[1] == '\0') return -1;

    nt = atoi(&symb[1]);

    // only S2n groups make sense
    if (nt%2) return -1;

    g = nt; pg = SN; nirrep_ = nt/2+1;

    return 0;
  }

  if (symb[0] == 't') {
    if (symb[1] != '\0') {
      if (symb[1] == 'd') {
        g = 24; pg = TD; nirrep_ = 5;
      } else if(symb[1] == 'h') {
        g = 24; pg = TH; nirrep_ = 6;
      } else {
        return -1;
      }
    } else {
      g = 12; pg = T; nirrep_ = 3;
    }

    return 0;
  }

  if (symb[0] == 'o') {
    if (symb[1] != '\0') {
      if (symb[1] == 'h') {
        pg = OH; g = 48; nirrep_ = 10;
      } else {
        return -1;
      }
    } else {
      g = 24; pg = O; nirrep_ = 5;
    }

    return 0;
  }

  if (symb[0] == 'i') {
    if (symb[1] != '\0') {
      if (symb[1] == 'h') {
        g = 120; pg = IH; nirrep_ = 10;
      } else {
        return -1;
      }
    } else {
      g = 60; pg = I; nirrep_ = 5;
    }

    return 0;
  }

  return -1;
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
