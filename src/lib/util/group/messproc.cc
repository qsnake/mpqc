//
// messproc.cc
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

#include <util/misc/formio.h>
#include <util/group/message.h>

using namespace sc;

static ClassDesc ProcMessageGrp_cd(
  typeid(ProcMessageGrp),"ProcMessageGrp",1,"public MessageGrp",
  0, create<ProcMessageGrp>, 0);

ProcMessageGrp::ProcMessageGrp(const Ref<KeyVal>& keyval):
  MessageGrp(keyval)
{
  sync_messages=0;
  type_messages=0;
  initialize(0,1);
}

ProcMessageGrp::ProcMessageGrp()
{
  sync_messages=0;
  type_messages=0;
  initialize(0,1);
}

ProcMessageGrp::~ProcMessageGrp()
{
}

Ref<MessageGrp> ProcMessageGrp::clone(void)
{
  Ref<MessageGrp> pmg = new ProcMessageGrp;
  return pmg;
}

void ProcMessageGrp::sendit(message_t *& messages, int dest, int msgtype, const void* buf,
                            int bytes)
{
  message_t *msg;
  message_t *I;

  if (dest != 0) {
      ExEnv::errn() << scprintf("messproc.cc:sendit: can only send to 0\n");
      abort();
    }

  msg = (message_t *) malloc(sizeof(message_t));
  if (msg) msg->buf = (char *) malloc(bytes);
  if (!msg || !msg->buf) {
      ExEnv::errn() << scprintf("messproc.cc:sendit: allocation failed\n");
      abort();
    }

  // Put msg at the end of the linked list, because of some bad
  // assumptions made by the mpscf program and libraries.
  msg->p = 0;
  if (!messages) {
      messages = msg;
    }
  else {
      for (I=messages; I->p != 0; I=I->p);
      I->p = msg;
    }

  memcpy(msg->buf,buf,bytes);
  msg->type = msgtype;
  msg->size = bytes;
}

void ProcMessageGrp::recvit(message_t *& messages, int source, int type, void* buf,
                            int bytes, int& last_size, int& last_type)
{
  message_t *i;
  message_t *last;

  last = 0;
  for (i=messages; i!=0; i = i->p) {
    if (i->type == type || type == -1) {
      if (i->size > bytes) {
        ExEnv::errn() << scprintf(
                "messproc.cc:recvit: message buffer isn't big enough\n");
        abort();
        }
      memcpy(buf,i->buf,i->size);

      // Remove the message from the list.
      if (last) {
          last->p = i->p;
        }
      else {
          messages = messages->p;
        }
      free(i->buf);
      free(i);

      return;
      }
    last = i;
    }

  ExEnv::errn() << scprintf(
          "messproc.cc:recvit: tried to receive something that isn't there\n");
  ExEnv::errn() << scprintf("messproc:recvit: tried %d bytes of type %d, ",bytes,type);
  abort();
}

void
ProcMessageGrp::raw_send(int target, const void* data, int nbyte)
{
  sendit(sync_messages, target, -1, data, nbyte);
}

void
ProcMessageGrp::raw_sendt(int target, int type, const void* data, int nbyte)
{
  sendit(type_messages, target, type, data, nbyte);
}

void
ProcMessageGrp::raw_recv(int sender, void* data, int nbyte)
{
  int last_size, last_type;
  recvit(sync_messages, sender, -1, data, nbyte, last_size, last_type);
}

void
ProcMessageGrp::raw_recvt(int type, void* data, int nbyte)
{
  int last_size, last_type;
  recvit(type_messages, -1, type, data, nbyte, last_size, last_type);
}

void
ProcMessageGrp::raw_bcast(void* data, int nbyte, int from)
{
}

int
ProcMessageGrp::probet(int type)
{
  message_t *i;

  for (i=type_messages; i!=0; i = i->p) {
      if (i->type == type || type == -1) {
          return 1;
        }
    }

  return 0;
}

void
ProcMessageGrp::sync()
{
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
