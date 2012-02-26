//
// integral.cc --- implementation of the Integral class
//
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

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdexcept>
#include <sstream>

#include <util/state/stateio.h>
#include <chemistry/qc/basis/integral.h>
#include <chemistry/qc/basis/shellrot.h>
#include <chemistry/qc/basis/petite.h>
#include <chemistry/qc/basis/obint.h>

using namespace std;
using namespace sc;

static ClassDesc Integral_cd(
  typeid(Integral),"Integral",2,"public SavableState",
  0, 0, 0);

Integral::Integral(const Ref<GaussianBasisSet> &b1,
                   const Ref<GaussianBasisSet> &b2,
                   const Ref<GaussianBasisSet> &b3,
                   const Ref<GaussianBasisSet> &b4)
{
  storage_ = 0;
  storage_used_ = 0;
  grp_ = MessageGrp::get_default_messagegrp();
  set_basis(b1,b2,b3,b4);
}

Integral::~Integral()
{
}

Integral::Integral(StateIn& s) :
  SavableState(s)
{
  storage_used_ = 0;
  bs1_ << SavableState::restore_state(s);
  bs2_ << SavableState::restore_state(s);
  bs3_ << SavableState::restore_state(s);
  bs4_ << SavableState::restore_state(s);
  if (s.version(::class_desc<Integral>()) >= 2) {
    double dstorage;
    s.get(dstorage);
    storage_ = size_t(dstorage);
  }
  else {
    unsigned int istorage;
    s.get(istorage);
    storage_ = istorage;
  }
  grp_ = MessageGrp::get_default_messagegrp();
}

Integral::Integral(const Ref<KeyVal>&)
{
  storage_used_ = 0;
  storage_ = 0;
  grp_ = MessageGrp::get_default_messagegrp();
}

void
Integral::save_data_state(StateOut&o)
{
  SavableState::save_state(bs1_.pointer(),o);
  SavableState::save_state(bs2_.pointer(),o);
  SavableState::save_state(bs3_.pointer(),o);
  SavableState::save_state(bs4_.pointer(),o);
  double dstorage = storage_;
  o.put(dstorage);
}

Ref<Integral> default_integral;

void
Integral::set_default_integral(const Ref<Integral>& intf)
{
  default_integral = intf;
}

// Liberally borrowed from ThreadGrp
Integral*
Integral::initial_integral(int& argc, char ** argv)
{
  Integral *intf = 0;
  char * keyval_string = 0;
  
  // see if an integral factory is given on the command line
  if (argc && argv) {
    for (int i=0; i < argc; i++) {
      if (argv[i] && !strcmp(argv[i], "-integral")) {
        char *integral_string = argv[i];
        i++;
        if (i >= argc) {
          throw runtime_error("-integral must be followed by an argument");
        }
        keyval_string = argv[i];
        // move the integral arguments to the end of argv
        int j;
        for (j=i+1; j<argc; j++) {
          argv[j-2] = argv[j];
        }
        argv[j++] = integral_string;
        argv[j++] = keyval_string;
        // decrement argc to hide the last two arguments
        argc -= 2;
        break;
      }
    }
  }

  if (!keyval_string) {
    // find out if the environment gives the containing integral
    keyval_string = getenv("INTEGRAL");
    if (keyval_string) {
      if (!strncmp("INTEGRAL=", keyval_string, 11)) {
        keyval_string = strchr(keyval_string, '=');
      }
      if (*keyval_string == '=') keyval_string++;
    }
  }

  // if keyval input for a integral was found, then
  // create it.
  if (keyval_string) {
    if (keyval_string[0] == '\0') return 0;
    Ref<ParsedKeyVal> strkv = new ParsedKeyVal();
    strkv->parse_string(keyval_string);
    Ref<DescribedClass> dc = strkv->describedclassvalue();
    intf = dynamic_cast<Integral*>(dc.pointer());
    if (dc.null()) {
      ostringstream errmsg;
      errmsg << "Integral::initial_integral: couldn't find a Integral in " << keyval_string << ends;
      throw runtime_error(errmsg.str());
    } else if (!intf) {
      ostringstream errmsg;
      errmsg << "initial_integral: wanted Integral but got " << dc->class_name() << ends;
      throw runtime_error(errmsg.str());
    }
    // prevent an accidental delete
    intf->reference();
    strkv = 0;
    dc = 0;
    // accidental delete not a problem anymore since all smart pointers
    // to intf are dead
    intf->dereference();
    return intf;
  }

  return 0;
}

int
Integral::equiv(const Ref<Integral> &integral)
{
  return eq(class_desc(),integral->class_desc());
}

Ref<PetiteList>
Integral::petite_list()
{
  return new PetiteList(bs1_, this);
}

Ref<PetiteList>
Integral::petite_list(const Ref<GaussianBasisSet>& gbs)
{
  return new PetiteList(gbs, this);
}

ShellRotation
Integral::shell_rotation(int am, SymmetryOperation& so, int pure)
{
  this->reference();
  ShellRotation r(am, so, this, pure);
  this->dereference();
  return r;
}

void
Integral::set_basis(const Ref<GaussianBasisSet> &b1,
                    const Ref<GaussianBasisSet> &b2,
                    const Ref<GaussianBasisSet> &b3,
                    const Ref<GaussianBasisSet> &b4)
{
  bs1_ = b1;
  bs2_ = b2;
  bs3_ = b3;
  bs4_ = b4;
  if (bs2_.null()) bs2_ = bs1_;
  if (bs3_.null()) bs3_ = bs2_;
  if (bs4_.null()) bs4_ = bs3_;
}

size_t
Integral::storage_required_eri(const Ref<GaussianBasisSet> &b1,
			       const Ref<GaussianBasisSet> &b2,
			       const Ref<GaussianBasisSet> &b3,
			       const Ref<GaussianBasisSet> &b4)
{
  // By default, generated ERI evaluator will not need
  // any significant amount of memory
  return 0;
}

size_t
Integral::storage_required_eri_deriv(const Ref<GaussianBasisSet> &b1,
				     const Ref<GaussianBasisSet> &b2,
				     const Ref<GaussianBasisSet> &b3,
				     const Ref<GaussianBasisSet> &b4)
{
  // By default, generated derivative ERI evaluator will not need
  // any significant amount of memory
  return 0;
}

size_t
Integral::storage_required_grt(const Ref<GaussianBasisSet> &b1,
			       const Ref<GaussianBasisSet> &b2,
			       const Ref<GaussianBasisSet> &b3,
			       const Ref<GaussianBasisSet> &b4)
{
  // By default, generated GRT evaluator will not need
  // any significant amount of memory
  return 0;
}

size_t
Integral::storage_unused()
{
  ptrdiff_t tmp=storage_-storage_used_;
  return (tmp<0?0:tmp);
}

Ref<TwoBodyInt>
Integral::grt()
{
  throw std::runtime_error("Integral::grt(): not implemented in this particular integrals factory.");
}

Ref<OneBodyOneCenterInt>
Integral::point_charge1(const Ref<PointChargeData>&)
{
  throw std::runtime_error("Integral::point_charge1(): not implemented in this particular integrals factory.");
}

Ref<TwoBodyThreeCenterInt>
Integral::electron_repulsion3()
{
  throw std::runtime_error("Integral::electron_repulsion3(): not implemented in this particular integrals factory.");
}

Ref<TwoBodyThreeCenterDerivInt>
Integral::electron_repulsion3_deriv()
{
  throw std::runtime_error("Integral::electron_repulsion3_deriv(): not implemented in this particular integrals factory.");
}

Ref<TwoBodyTwoCenterInt>
Integral::electron_repulsion2()
{
  throw std::runtime_error("Integral::electron_repulsion2(): not implemented in this particular integrals factory.");
}

Ref<TwoBodyTwoCenterDerivInt>
Integral::electron_repulsion2_deriv()
{
  throw std::runtime_error("Integral::electron_repulsion2_deriv(): not implemented in this particular integrals factory.");
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
