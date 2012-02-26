//
// memiter.h
//
// derived from memasmg.cc
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Curtis Janssen <cljanss@limitpt.com>
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
#pragma interface
#endif

#ifndef _util_group_memiter_h
#define _util_group_memiter_h

#include <util/group/memory.h>

namespace sc {

class MemoryIter {
  private:
    distsize_t *offsets_;
    int n_;

    void *data_;

    char *current_data_;
    int current_size_;
    int current_offset_;
    int node_;

    int ready_;

    distsize_t offset_;
    int size_;
  public:
    MemoryIter(void *data, distsize_t *offsets, int n);

    // iteration control
    void begin(distsize_t offset, int size);
    int ready() { return ready_; }
    void next();

    // info about the current piece of data
    void *data() { return (void*) current_data_; }
    int node() { return node_; }
    int offset() { return current_offset_; }
    int size() { return current_size_; }

    // returns true if all data is local to node
    int local(distsize_t offset, int size, int node);
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
