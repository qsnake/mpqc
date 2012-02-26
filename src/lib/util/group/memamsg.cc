//
// memamsg.cc
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

#ifndef _util_group_memamsg_cc
#define _util_group_memamsg_cc

#ifdef __GNUC__
#pragma implementation
#endif

#include <util/misc/formio.h>
#include <util/group/pool.h>
#include <util/group/memamsg.h>
#include <util/group/memiter.h>

using namespace std;
using namespace sc;

#ifdef HAVE_HRECV
#  define DISABLE do { masktrap(1); ExEnv::outn().flush(); } while(0)
#  define ENABLE do { ExEnv::outn().flush(); masktrap(0); } while(0)
   extern "C" {
       long masktrap(long state);
     }
#else
#  define DISABLE ExEnv::outn().flush()
#  define ENABLE ExEnv::outn().flush()
#endif

#define PRINTF(args) do { DISABLE; \
                          ExEnv::outn() << scprintf args ; \
                          ExEnv::outn().flush(); \
                          ENABLE; \
                         } while(0)

#undef PRINTF
#define PRINTF(args)

///////////////////////////////////////////////////////////////////////
// The MemoryDataRequest class

MemoryDataRequest::MemoryDataRequest(Request r, int node,
                                     int offset, int size, int lock,
                                     int serial_number)
{
  assign(r, node, offset, size, lock, serial_number);
}

void
MemoryDataRequest::assign(Request r, int node,
                          int offset, int size, int lock,
                          int serial_number)
{
  data_[0] = (int) r;
  data_[1] = node;
  data_[2] = offset;
  data_[3] = size;
  data_[4] = serial_number;
  data_[5] = lock;
}

const char *
MemoryDataRequest::request_string() const
{
  switch (request()) {
  case MemoryDataRequest::Deactivate:
      return "Deactivate";
  case MemoryDataRequest::Retrieve:
      return "Retrieve";
  case MemoryDataRequest::Replace:
      return "Replace";
  case MemoryDataRequest::DoubleSum:
      return "DoubleSum";
  case MemoryDataRequest::Sync:
      return "Sync";
  default:
      return "BadRequest";
    }
}

void
MemoryDataRequest::print(const char *msg, ostream & o) const
{
  if (msg == 0) msg = "";

  o.flush();
  if (request() == Sync) {
      o << scprintf("%s \"%s\" %d-%d reactivate = %d\n",
              msg, request_string(), node(), serial_number(), reactivate());
    }
  else {
      o << scprintf("%s \"%s\" offset = %5d, %5d bytes, %d-%d, %s\n",
              msg, request_string(),
              offset(), size(), node(), serial_number(),
              (lock()?"lock":"nolock"));
    }
  o.flush();
}

void
MemoryDataRequest::operator =(const MemoryDataRequest &r)
{
  for (int i=0; i<NData; i++) {
      data_[i] = r.data_[i];
    }
}

///////////////////////////////////////////////////////////////////////
// The MemoryDataRequestQueue class

void
MemoryDataRequestQueue::push(MemoryDataRequest&r)
{
  if (n_ == MaxDepth) {
      ExEnv::errn() << scprintf("MemoryDataRequestQueue: MaxDepth exceeded\n");
      abort();
    }
  q_[n_] = r;
  n_++;
}

void
MemoryDataRequestQueue::pop(MemoryDataRequest&r)
{
  if (n_ == 0) {
      ExEnv::errn() << scprintf("MemoryDataRequestQueue: nothing to pop\n");
      abort();
    }
  n_--;
  r = q_[n_];
}

///////////////////////////////////////////////////////////////////////
// Members for ActiveMsgMemoryGrp

static ClassDesc ActiveMsgMemoryGrp_cd(
  typeid(ActiveMsgMemoryGrp),"ActiveMsgMemoryGrp",1,"public MsgMemoryGrp",
  0, 0, 0);

ActiveMsgMemoryGrp::ActiveMsgMemoryGrp(const Ref<MessageGrp>& msg):
  MsgMemoryGrp(msg)
{
  data_ = 0;
}

ActiveMsgMemoryGrp::ActiveMsgMemoryGrp(const Ref<KeyVal>& keyval):
  MsgMemoryGrp(keyval)
{
  data_ = 0;
}

void
ActiveMsgMemoryGrp::set_localsize(size_t localsize)
{
  if (debug_) {
      ExEnv::out0() << "ActiveMsgMemoryGrp::set_localsize(" << localsize << ")" << endl;
    }
  deactivate();
  MsgMemoryGrp::set_localsize(localsize);
  delete[] data_;
  data_ = new char[localsize];
  activate();
  if (debug_) {
      ExEnv::out0() << "ActiveMsgMemoryGrp::set_localsize done: offsets:";
      for (int i=0; i<=n(); i++) {
          ExEnv::out0() << " " << double(offset(i));
        }
      ExEnv::out0() << endl;
    }
}

void *
ActiveMsgMemoryGrp::localdata()
{
  return data_;
}

ActiveMsgMemoryGrp::~ActiveMsgMemoryGrp()
{
  deactivate();
  delete[] data_;
}

void *
ActiveMsgMemoryGrp::obtain_writeonly(distsize_t offset, int size)
{
  void *data = (void *) new char[size];
  return data;
}

void *
ActiveMsgMemoryGrp::obtain_readwrite(distsize_t offset, int size)
{
  PRINTF(("ActiveMsgMemoryGrp::obtain_readwrite entered\n"));
  void *data = (void *) new char[size];
  MemoryIter i(data, offsets_, n());
  for (i.begin(offset, size); i.ready(); i.next()) {
      if (i.node() == me()) {
          PRINTF(("ActiveMsgMemoryGrp::obtain_readwrite: local copy\n"));
          obtain_local_lock(i.offset(), i.offset()+i.size());
          memcpy(i.data(), &data_[i.offset()], i.size());
        }
      else {
          PRINTF(("ActiveMsgMemoryGrp::obtain_readwrite: node = %d, "
                  "int offset = %d, int size = %d\n",
                  i.node(), i.offset()/sizeof(int), i.size()/sizeof(int)));
          retrieve_data(i.data(), i.node(), i.offset(), i.size(), 1);
        }
    }
  PRINTF(("ActiveMsgMemoryGrp::obtain_readwrite exiting\n"));
  return data;
}

void *
ActiveMsgMemoryGrp::obtain_readonly(distsize_t offset, int size)
{
  void *data = (void *) new char[size];
  PRINTF(("%d: ActiveMsgMemoryGrp::obtain_readonly:"
          "overall: offset = %d size = %d\n",
          me(), offset, size));
  MemoryIter i(data, offsets_, n());
  for (i.begin(offset, size); i.ready(); i.next()) {
      PRINTF(("%d: ActiveMsgMemoryGrp::obtain_readonly:working on:"
              "node = %d offset = %d size = %d\n",
              me(), i.node(), i.offset(), i.size()));
      if (i.node() == me()) {
          PRINTF(("%d: ActiveMsgMemoryGrp::obtain_readonly: local: "
                  "offset = %d size = %d\n", me(), i.offset(), i.size()));
          memcpy(i.data(), &data_[i.offset()], i.size());
        }
      else {
          PRINTF(("ActiveMsgMemoryGrp::obtain_readonly: node = %d, "
                  "int offset = %d, int size = %d\n",
                  i.node(), i.offset()/sizeof(int), i.size()/sizeof(int)));
          retrieve_data(i.data(), i.node(), i.offset(), i.size(), 0);
        }
    }
  return data;
}

void
ActiveMsgMemoryGrp::sum_reduction(double *data, distsize_t doffset, int dsize)
{
  distsize_t offset = doffset * sizeof(double);
  int size = dsize * sizeof(double);
  MemoryIter i(data, offsets_, n());
  for (i.begin(offset, size); i.ready(); i.next()) {
      if (i.node() == me()) {
          int chunkdsize = i.size()/sizeof(double);
          double *chunkdata = (double*) &data_[i.offset()];
          double *tmp = (double*) i.data();
          PRINTF(("%d: summing %d doubles from 0x%x to 0x%x\n",
                  me(), chunkdsize, tmp, chunkdata));
          obtain_local_lock(i.offset(), i.offset()+i.size());
          for (int j=0; j<chunkdsize; j++) {
              *chunkdata++ += *tmp++;
            }
          release_local_lock(i.offset(), i.offset()+i.size());
        }
      else {
          sum_data((double*)i.data(), i.node(), i.offset(), i.size());
        }
    }
}

void
ActiveMsgMemoryGrp::sum_reduction_on_node(double *data, size_t doffset,
                                          int dlength, int node)
{
  if (node == -1) node = me();

  if (node == me()) {
      double *localdata = (double*) &data_[sizeof(double)*doffset];
      obtain_local_lock(sizeof(double)*doffset,
                        sizeof(double)*(doffset+dlength));
      for (int j=0; j<dlength; j++) {
          *localdata++ += *data++;
        }
      release_local_lock(sizeof(double)*doffset,
                         sizeof(double)*(doffset+dlength));
    }
  else {
      sum_data(data, node, sizeof(double)*doffset, sizeof(double)*dlength);
    }
}

void
ActiveMsgMemoryGrp::release_readonly(void *data, distsize_t offset, int size)
{
  delete[] (char*) data;
}

void
ActiveMsgMemoryGrp::release_writeonly(void *data, distsize_t offset, int size)
{
  MemoryIter i(data, offsets_, n());
  for (i.begin(offset, size); i.ready(); i.next()) {
      if (i.node() == me()) {
          PRINTF(("ActiveMsgMemoryGrp::release_write: local\n"));
          PRINTF(("  i.offset() = %d i.data() = 0x%x i.size() = %d\n",
                  i.offset(), i.data(), i.size()));
          PRINTF(("  &data_[i.offset()] = 0x%x\n", &data_[i.offset()]));
          memcpy(&data_[i.offset()], i.data(), i.size());
        }
      else {
          PRINTF(("ActiveMsgMemoryGrp::release_write: node = %d, "
                  "int offset = %d, int size = %d\n",
                  i.node(), i.offset()/sizeof(int), i.size()/sizeof(int)));
          replace_data(i.data(), i.node(), i.offset(), i.size(), 0);
        }
    }
  delete[] (char*) data;
}

void
ActiveMsgMemoryGrp::release_readwrite(void *data, distsize_t offset, int size)
{
  MemoryIter i(data, offsets_, n());
  for (i.begin(offset, size); i.ready(); i.next()) {
      if (i.node() == me()) {
          memcpy(&data_[i.offset()], i.data(), i.size());
          release_local_lock(i.offset(), i.offset()+i.size());
        }
      else {
          replace_data(i.data(), i.node(), i.offset(), i.size(), 1);
        }
    }
  delete[] (char*) data;
}

void
ActiveMsgMemoryGrp::print(ostream &o) const
{
  MemoryGrp::print(o);
}

#endif

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
