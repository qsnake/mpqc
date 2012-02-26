//
// memory.cc
//
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
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <scconfig.h>
#endif

#include <scconfig.h>
#include <util/misc/formio.h>
#include <util/group/memory.h>

#include <util/group/memproc.h>

#ifdef HAVE_SYSV_IPC
#  include <util/group/messshm.h>
#  include <util/group/memshm.h>
#endif

#if defined(HAVE_MPI)
#  include <util/group/messmpi.h>
#  include <util/group/memmtmpi.h>
#endif

#if defined(HAVE_ARMCI)
#  include <util/group/memarmci.h>
#endif

using namespace std;
using namespace sc;

//////////////////////////////////////////////////////////////////////
// MemoryGrpBuf template instantiations

#ifdef EXPLICIT_TEMPLATE_INSTANTIATION
template class MemoryGrpBuf<double>;
template class MemoryGrpBuf<int>;
template class MemoryGrpBuf<char>;
template class MemoryGrpBuf<unsigned char>;
#endif

//////////////////////////////////////////////////////////////////////
// MemoryGrp members

static ClassDesc MemoryGrp_cd(
  typeid(MemoryGrp),"MemoryGrp",1,"public DescribedClass",
  0, 0, 0);

MemoryGrp::MemoryGrp()
{
  debug_ = 0;

  offsets_ = 0;

  init_locks();
}

MemoryGrp::MemoryGrp(const Ref<KeyVal>& keyval)
{
  debug_ = keyval->intvalue("debug");

  offsets_ = 0;

  init_locks();
}

MemoryGrp::~MemoryGrp()
{
  delete[] offsets_;
  delete[] locks_;
}

void
MemoryGrp::init_locks()
{
  Ref<ThreadGrp> thgrp = ThreadGrp::get_default_threadgrp();
  nlock_ = 2 * thgrp->nthread();
  locks_ = new Ref<ThreadLock>[nlock_];
  for (int i=0; i<nlock_; i++) locks_[i] = thgrp->new_lock();
}

MemoryGrp *
MemoryGrp::initial_memorygrp()
{
  int argc = 0;
  return initial_memorygrp(argc,0);
}

MemoryGrp *
MemoryGrp::initial_memorygrp(int &argc, char *argv[])
{
  MemoryGrp *grp = 0;

  char *keyval_string = 0;

  // see if a memory group is given on the command line
  if (argc && argv) {
      for (int i=0; i<argc; i++) {
	  if (argv[i] && !strcmp(argv[i], "-memorygrp")) {
              char *memorygrp_string = argv[i];
              i++;
              if (i >= argc) {
                  ExEnv::errn() << "-memorygrp must be following by an argument"
                       << endl;
                  abort();
                }
              keyval_string = argv[i];
              // move the memorygrp arguments to the end of argv
              int j;
              for (j=i+1; j<argc; j++) {
                  argv[j-2] = argv[j];
                }
              argv[j++] = memorygrp_string;
              argv[j++] = keyval_string;
              // decrement argc to hide the last two arguments
              argc -= 2;
              break;
            }
        }
    }

  if (!keyval_string) {
      // find out if the environment gives the containing memory group
      keyval_string = getenv("MEMORYGRP");
      if (keyval_string) {
          if (!strncmp("MEMORYGRP=", keyval_string, 11)) {
              keyval_string = strchr(keyval_string, '=');
            }
          if (*keyval_string == '=') keyval_string++;
        }
    }

  // if keyval input for a memory group was found, then
  // create it.
  if (keyval_string) {
      //ExEnv::outn() << "Creating MemoryGrp from \"" << keyval_string << "\"" << endl;
      Ref<ParsedKeyVal> strkv = new ParsedKeyVal();
      strkv->parse_string(keyval_string);
      Ref<DescribedClass> dc = strkv->describedclassvalue();
      grp = dynamic_cast<MemoryGrp*>(dc.pointer());
      if (dc.null()) {
          ExEnv::errn() << "initial_memorygrp: couldn't find a MemoryGrp in "
               << keyval_string << endl;
          abort();
        }
      else if (!grp) {
          ExEnv::errn() << "initial_memorygrp: wanted MemoryGrp but got "
               << dc->class_name() << endl;
          abort();
        }
      // prevent an accidental delete
      grp->reference();
      strkv = 0;
      dc = 0;
      // accidental delete not a problem anymore since all smart pointers
      // to grp are dead
      grp->dereference();
      return grp;
    }

  return grp;
}

void
MemoryGrp::activate()
{
}

void
MemoryGrp::deactivate()
{
}

void
MemoryGrp::print(ostream&o) const
{
  o << scprintf("MemoryGrp (node %d):\n", me());
  o << scprintf("%d: n = %d\n", me(), n());
  for (int i=0; i<=n_; i++) {
      o << scprintf("%d: offset[%d] = %5d\n", me(), i, offsets_[i]);
    }
}

void
MemoryGrp::sum_reduction(double *data, distsize_t doffset, int dlength)
{
  distsize_t offset = doffset * sizeof(double);
  int length = dlength * sizeof(double);

  if (offset + length > totalsize()) {
      ExEnv::errn() << "MemoryGrp::sum_reduction: arg out of range:"
                    << " offset = " << double(offset)
                    << " length = " << length
                    << " totalsize() = " << double(totalsize())
                    << endl;
      abort();
    }

  double *source_data = (double*) obtain_readwrite(offset, length);

  for (int i=0; i<dlength; i++) {
      source_data[i] += data[i];
    }

  release_readwrite((void*) source_data, offset, length);
}

void
MemoryGrp::sum_reduction_on_node(double *data, size_t doffset, int dlength,
                                 int node)
{
  if (node == -1) node = me();

  sum_reduction(data, doffset + offset(node)/sizeof(double),
                dlength);
}

void*
MemoryGrp::malloc_local(size_t nbyte)
{
  return new char[nbyte];
}

void
MemoryGrp::free_local(void *data)
{
  delete[] reinterpret_cast<char*>(data);
}

double*
MemoryGrp::malloc_local_double(size_t ndouble)
{
  return reinterpret_cast<double*>(malloc_local(ndouble*sizeof(double)));
}

void
MemoryGrp::free_local_double(double *data)
{
  free_local(data);
}

void
MemoryGrp::catchup()
{
  return;
}

void
MemoryGrp::obtain_local_lock(size_t start, size_t fence)
{
  distsize_t locked_region_size = 1 + localsize()/nlock_;
  int lstart = start/locked_region_size;
  int llast = fence/locked_region_size;
  for (int i=lstart; i<=llast; i++) {
      locks_[i]->lock();
    }
}

void
MemoryGrp::release_local_lock(size_t start, size_t fence)
{
  distsize_t locked_region_size = 1 + localsize()/nlock_;
  int lstart = start/locked_region_size;
  int llast = fence/locked_region_size;
  for (int i=lstart; i<=llast; i++) {
      locks_[i]->unlock();
    }
}

static Ref<MemoryGrp> default_memorygrp;

void
MemoryGrp::set_default_memorygrp(const Ref<MemoryGrp>& grp)
{
  default_memorygrp = grp;
}

MemoryGrp*
MemoryGrp::get_default_memorygrp()
{
  if (default_memorygrp.nonnull()) return default_memorygrp.pointer();

  Ref<MessageGrp> msg = MessageGrp::get_default_messagegrp();

#if defined(HAVE_MPI) && defined(DEFAULT_MTMPI)
  Ref<ThreadGrp> thr = ThreadGrp::get_default_threadgrp();
  default_memorygrp = new MTMPIMemoryGrp(msg,thr);
  return default_memorygrp.pointer();
#endif

#if defined(DEFAULT_ARMCI)
  default_memorygrp = new ARMCIMemoryGrp(msg);
  return default_memorygrp.pointer();
#endif

  if (msg.null()) {
      ExEnv::errn() << scprintf("MemoryGrp::get_default_memorygrp: requires default MessageGrp if default behavior not configured\n");
      abort();
    }
#if defined(HAVE_MPI)
  else if (msg->class_desc() == ::class_desc<MPIMessageGrp>()) {
      Ref<ThreadGrp> thr = ThreadGrp::get_default_threadgrp();
      default_memorygrp = new MTMPIMemoryGrp(msg,thr);
      return default_memorygrp.pointer();
    }
#endif
#if defined(HAVE_ARMCI)
  else if (msg->class_desc() == ::class_desc<MPIMessageGrp>()) {
      default_memorygrp = new ARMCIMemoryGrp(msg);
      return default_memorygrp.pointer();
    }
#endif
#ifdef HAVE_SYSV_IPC
  else if (msg->class_desc() == ::class_desc<ShmMessageGrp>()) {
      default_memorygrp = new ShmMemoryGrp(msg);
      return default_memorygrp.pointer();
    }
#endif
  else if (msg->n() == 1) {
      default_memorygrp = new ProcMemoryGrp();
      return default_memorygrp.pointer();
    }
  else {
      ExEnv::errn() << scprintf("MemoryGrp::get_default_memorygrp: cannot create "
              "default for \"%s\"\n.", msg->class_name());
      abort();
    }

  if (default_memorygrp.null()) {
      ExEnv::err0() << scprintf("WARNING: MemoryGrp::get_default_memorygrp(): failed\n");
      default_memorygrp = new ProcMemoryGrp;
    }
  return default_memorygrp.pointer();
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
