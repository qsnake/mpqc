//
// gbuild.h --- definition of the abstract G matrix builder
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

#ifndef _chemistry_qc_scf_gbuild_h
#define _chemistry_qc_scf_gbuild_h

#ifdef __GNUC__
#pragma interface
#endif

#include <util/group/thread.h>
#include <chemistry/qc/scf/scf.h>

namespace sc {

template<class T>
class GBuild : public Thread {
  protected:
    T& contribution;

    double accuracy_;
  public:
    GBuild(T&t) : contribution(t) {}
    virtual ~GBuild() {}
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
