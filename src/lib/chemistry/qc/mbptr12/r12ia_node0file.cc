//
// r12ia_node0file.cc
//
// Copyright (C) 2002 Edward Valeev
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

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdexcept>
#include <util/class/scexception.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <util/misc/string.h>
#include <util/misc/formio.h>
#include <util/misc/exenv.h>
#include <chemistry/qc/mbptr12/r12ia_node0file.h>

#define CREATE_FILE_ON_NODE0_ONLY 1

using namespace std;
using namespace sc;

///////////////////////////////////////////////////////////////

static ClassDesc R12IntsAcc_Node0File_cd(
  typeid(R12IntsAcc_Node0File),"R12IntsAcc_Node0File",1,"public R12IntsAcc",
  0, 0, create<R12IntsAcc_Node0File>);

R12IntsAcc_Node0File::R12IntsAcc_Node0File(Ref<MemoryGrp>& mem, const char* filename, int num_te_types,
					   int ni, int nj, int nx, int ny) :
  R12IntsAcc(num_te_types, ni, nj, nx, ny)
{
  mem_ = mem;
  filename_ = strdup(filename);
  
  init(false);
}

R12IntsAcc_Node0File::R12IntsAcc_Node0File(StateIn& si) :
  R12IntsAcc(si)
{
  mem_ = MemoryGrp::get_default_memorygrp();
  si.getstring(filename_);
  
  init(true);
}

R12IntsAcc_Node0File::~R12IntsAcc_Node0File()
{
  for(int i=0;i<ni_;i++)
    for(int j=0;j<nj_;j++) {
      if (!is_avail(i,j)) {
	int ij = ij_index(i,j);
	for(int oper_type=0; oper_type<num_te_types(); oper_type++)
	  if (pairblk_[ij].ints_[oper_type] != NULL) {
	    ExEnv::outn() << indent << taskid() << ": i = " << i << " j = " << j << " oper_type = " << oper_type << endl;
	    throw std::runtime_error("Logic error: R12IntsAcc_Node0File::~ : some nonlocal blocks have not been released!");
	  }
      }
    }
  delete[] pairblk_;
   
  // Destroy the file
#if  CREATE_FILE_ON_NODE0_ONLY
  if (taskid() == 0)
#endif
    unlink(filename_);
  free(filename_);
}

void
R12IntsAcc_Node0File::save_data_state(StateOut& so)
{
  R12IntsAcc::save_data_state(so);
  so.putstring(filename_);
}

void
R12IntsAcc_Node0File::init(bool restart)
{
  pairblk_ = new struct PairBlkInfo[ni_*nj_];
  int i, j, ij;
  for(i=0,ij=0;i<ni_;i++)
    for(j=0;j<nj_;j++,ij++) {
      pairblk_[ij].ints_[eri] = NULL;
      pairblk_[ij].ints_[r12] = NULL;
      pairblk_[ij].ints_[r12t1] = NULL;
      pairblk_[ij].ints_[r12t2] = NULL;
      pairblk_[ij].refcount_[eri] = 0;
      pairblk_[ij].refcount_[r12] = 0;
      pairblk_[ij].refcount_[r12t1] = 0;
      pairblk_[ij].refcount_[r12t2] = 0;
      pairblk_[ij].offset_ = (off_t)ij*blocksize_;
    }

  // node 0 will have the file
#if CREATE_FILE_ON_NODE0_ONLY
  if (taskid() != 0)
    return;
#endif

  // See if can open/create the file
  if (restart) {
    datafile_ = open(filename_,O_WRONLY|O_APPEND,0644);
  }
  else {
    datafile_ = creat(filename_,0644);
  }
  // Check if the file was opened correctly
  check_filedescr_();
  // If everything is fine close it and proceed
  close(datafile_);
}

void
R12IntsAcc_Node0File::check_filedescr_()
{
  // If the file was not opened correctly - throw an exception
  if (datafile_ == -1) {
    switch (errno) {
      case EACCES:
      throw std::runtime_error("R12IntsAcc_Node0File::R12IntsAcc_Node0File -- access to the requested file is not allowed");
      case ENOSPC:
      throw std::runtime_error("R12IntsAcc_Node0File::R12IntsAcc_Node0File -- no space left in the filesystem");
      case ENOENT:
      throw std::runtime_error("R12IntsAcc_Node0File::R12IntsAcc_Node0File -- file does not exist");
      case ENOTDIR:
      throw std::runtime_error("R12IntsAcc_Node0File::R12IntsAcc_Node0File -- not a directory");
      default:
      perror(0);
      throw std::runtime_error("R12IntsAcc_Node0File::R12IntsAcc_Node0File -- failed to open POSIX file on node 0");
    }
  }
}

void
R12IntsAcc_Node0File::store_memorygrp(Ref<MemoryGrp>& mem, int ni, const size_t blksize)
{
  if (committed_) {
    ExEnv::out0() << "R12IntsAcc_Node0File::store_memorygrp(mem,ni) called after all data has been committed" << endl;
    abort();
  }
  // mem must be the same as mem_
  else if (mem_ != mem) {
    ExEnv::out0() << "R12IntsAcc_MemoryGrp::store_memorygrp(mem,ni) called with invalid argument:" << endl <<
      "mem != R12IntsAcc_MemoryGrp::mem_" << endl;
    abort();
  }
  // Will store integrals on node 0
  else if (taskid() != 0)
    return;
  else if (ni > ni_) {
    ExEnv::out0() << "R12IntsAcc_Node0File::store_memorygrp(mem,ni) called with invalid argument:" << endl <<
      "ni > R12IntsAcc_Node0File::ni_" << endl;
    abort();
  }
  else if (next_orbital() + ni > ni_) {
    ExEnv::out0() << "R12IntsAcc_Node0File::store_memorygrp(mem,ni) called with invalid argument:" << endl <<
      "ni+next_orbital() > R12IntsAcc_Node0File::ni_" << endl;
    abort();
  }
  else {
    size_t blksize_memgrp = blksize;
    if (blksize_memgrp == 0)
      blksize_memgrp = blksize_;

    // Now do some extra work to figure layout of data in MemoryGrp
    // Compute global offsets to each processor's data
    int i,j,ij;
    int me = taskid();
    int nproc = mem->n();

    // Append the data to the file
    datafile_ = open(filename_,O_WRONLY|O_APPEND,0644);
    for (int i=0; i<ni; i++)
      for (int j=0; j<nj_; j++) {
	double *data;
	int ij = ij_index(i,j);
	int proc = ij%nproc;
	int local_ij_index = ij/nproc;
	if (proc != me) {
	  distsize_t moffset = (distsize_t)local_ij_index*blksize_memgrp*num_te_types() + mem->offset(proc);
          for(int te_type=0; te_type < num_te_types(); te_type++) {
	    data = (double *) mem->obtain_readonly(moffset, blksize_);
	    ssize_t wrote_this_much = write(datafile_, data, blksize_);
            if (wrote_this_much != blksize_)
              throw FileOperationFailed("R12IntsAcc_Node0File::store_memorygrp() failed",
                                        __FILE__,
                                        __LINE__,
                                        filename_,
                                        FileOperationFailed::Write);
	    mem->release_readonly(data, moffset, blksize_);
            moffset += blksize_memgrp;
          }
	}
	else {
          data = (double *) ((size_t)mem->localdata() + blksize_memgrp*num_te_types()*local_ij_index);
          for(int te_type=0; te_type < num_te_types(); te_type++) {
            ssize_t wrote_this_much = write(datafile_, data, blksize_);
            if (wrote_this_much != blksize_)
              throw FileOperationFailed("R12IntsAcc_Node0File::store_memorygrp() failed",
                                        __FILE__,
                                        __LINE__,
                                        filename_,
                                        FileOperationFailed::Write);
            data = (double*) ((size_t) data + blksize_memgrp);
          }
	}
      }
    // Close the file and update the i counter
    close(datafile_);
  }

  inc_next_orbital(ni);
}

void
R12IntsAcc_Node0File::store_pair_block(int i, int j, double *ints)
{
  ExEnv::err0() << "R12IntsAcc_Node0File::store_pair_block() called: error" << endl;
  abort();
}

void
R12IntsAcc_Node0File::commit()
{
  mem_->set_localsize(0);
  mem_->sync();
  mem_->deactivate();
  R12IntsAcc::commit();
}

void
R12IntsAcc_Node0File::activate()
{
  R12IntsAcc::activate();
#if CREATE_FILE_ON_NODE0_ONLY
  if (taskid() == 0)
#endif
    datafile_ = open(filename_, O_RDONLY);
}

void
R12IntsAcc_Node0File::deactivate()
{
  mem_->activate();
#if CREATE_FILE_ON_NODE0_ONLY
  if (taskid() == 0)
#endif
    close(datafile_);
  R12IntsAcc::deactivate();
}

double *
R12IntsAcc_Node0File::retrieve_pair_block(int i, int j, tbint_type oper_type)
{
  // Can retrieve blocks on node 0 only
  if (is_avail(i,j)) {
    int ij = ij_index(i,j);
    struct PairBlkInfo *pb = &pairblk_[ij];
    // Always first check if it's already in memory
    if (pb->ints_[oper_type] == 0) {
      off_t offset = pb->offset_ + (off_t)oper_type*blksize_;
      off_t result_offset = lseek(datafile_,offset,SEEK_SET);
      if (offset == (off_t)-1 || result_offset != offset)
        throw FileOperationFailed("R12IntsAcc_Node0File::retrieve_pair_block() failed",
                                  __FILE__,
                                  __LINE__,
                                  filename_,
                                  FileOperationFailed::Other);
      pb->ints_[oper_type] = new double[nxy_];
      ssize_t read_this_much = read(datafile_,pb->ints_[oper_type],blksize_);
      if (read_this_much != blksize_)
        throw FileOperationFailed("R12IntsAcc_Node0File::retrieve_pair_block() failed",
                                  __FILE__,
                                  __LINE__,
                                  filename_,
                                  FileOperationFailed::Read);
    }
    pb->refcount_[oper_type] += 1;
    return pb->ints_[oper_type];
  }
  else
    throw std::runtime_error("R12IntsAcc_Node0File::retrieve_pair_block() called on node other than 0");

  return 0;
}

void
R12IntsAcc_Node0File::release_pair_block(int i, int j, tbint_type oper_type)
{
  if (is_avail(i,j)) {
    int ij = ij_index(i,j);
    struct PairBlkInfo *pb = &pairblk_[ij];
    if (pb->refcount_[oper_type] <= 0) {
      ExEnv::outn() << indent << taskid() << ":refcount=0: i = " << i << " j = " << j << " tbint_type = " << oper_type << endl;
      throw std::runtime_error("Logic error: R12IntsAcc_Node0File::release_pair_block: refcount is already zero!");
    }
    if (pb->ints_[oper_type] != NULL && pb->refcount_[oper_type] == 1) {
      delete[] pb->ints_[oper_type];
      pb->ints_[oper_type] = NULL;
    }
    pb->refcount_[oper_type] -= 1;
  }
}


// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
