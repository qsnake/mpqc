//
// thpthd.h
//
// Copyright (C) 1997 Limit Point Systems, Inc.
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

#ifndef _util_group_thpthd_h
#define _util_group_thpthd_h

#ifdef __GNUC__
#pragma interface
#endif

#include <pthread.h>
#include <util/group/thread.h>

namespace sc {

/** The PthreadThreadGrp class privides a concrete thread group
    appropriate for an environment where pthreads is available. */
class PthreadThreadGrp: public ThreadGrp {
  private:
    pthread_t *pthreads_;
    pthread_attr_t *attr_;

    void init_attr();
    void init_priority(int, int);
  public:
    PthreadThreadGrp();
    PthreadThreadGrp(const PthreadThreadGrp&, int nthread = -1);
    PthreadThreadGrp(const Ref<KeyVal>&);
    ~PthreadThreadGrp();

    int start_threads();
    int wait_threads();
    void add_thread(int i, Thread* t) { ThreadGrp::add_thread(i,t); }
    void add_thread(int i, Thread* t, int priority);
    
    Ref<ThreadLock> new_lock();

    ThreadGrp* clone(int nthread = -1);
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "ETS"
// End:
