//
// transform_ixjy.h
//
// Copyright (C) 2004 Edward Valeev
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
#pragma interface
#endif

#ifndef _chemistry_qc_mbptr12_transformixjy_h
#define _chemistry_qc_mbptr12_transformixjy_h

#include <string>
#include <util/ref/ref.h>
#include <util/class/scexception.h>
#include <chemistry/qc/mbptr12/transform_tbint.h>

using namespace std;

namespace sc {

  /** TwoBodyMOIntsTransform_ixjy computes (ix|jy) integrals
      using parallel integrals-direct AO->MO transformation. */

class TwoBodyMOIntsTransform_ixjy : public TwoBodyMOIntsTransform {

  // Initialize the MO integrals accumulator
  void init_acc();
  // Compute required dynamic memory for a given batch size
  distsize_t compute_transform_dynamic_memory_(int ni) const;

public:

  TwoBodyMOIntsTransform_ixjy(StateIn&);
  TwoBodyMOIntsTransform_ixjy(const std::string& name, const Ref<MOIntsTransformFactory>& factory,
                              const Ref<MOIndexSpace>& space1, const Ref<MOIndexSpace>& space2,
                              const Ref<MOIndexSpace>& space3, const Ref<MOIndexSpace>& space4);
  ~TwoBodyMOIntsTransform_ixjy();

  void save_data_state(StateOut&);

  /// Implementation of TwoBodyMOIntsTransform::type()
  std::string type() const { return "ixjy"; }

  /** Returns the number of bytes allocated for each ij-block of integrals of one type
    in MemoryGrp */
  const size_t memgrp_blksize() const;

  /// Computes transformed integrals
  void compute();
  /// Check symmetry of transformed integrals
  void check_int_symm(double threshold = TwoBodyMOIntsTransform::zero_integral) const throw (ProgrammingError);
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:


