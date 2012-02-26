//
// message.h
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
#pragma interface
#endif

#ifndef _util_group_message_h
#define _util_group_message_h

#include <map>

#include <math.h>
#include <util/class/class.h>
#include <util/state/state.h>
#include <util/keyval/keyval.h>
#include <util/group/topology.h>

namespace sc {

template <class T>
class GrpReduce {
  public:
    virtual ~GrpReduce() {};
    virtual void reduce(T*target, T*data, int n) = 0;
};

template <class T>
class GrpSumReduce: public GrpReduce<T> {
  public:
    ~GrpSumReduce() {};
    void reduce(T*target, T*data, int nelement);
};

template <class T>
class GrpMinReduce: public GrpReduce<T> {
  public:
    ~GrpMinReduce() {};
    void reduce(T*target, T*data, int nelement);
};

template <class T>
class GrpMaxReduce: public GrpReduce<T> {
  public:
    ~GrpMaxReduce() {};
    void reduce(T*target, T*data, int nelement);
};

template <class T>
class GrpArithmeticAndReduce: public GrpReduce<T> {
  public:
    void reduce(T*target, T*data, int nelement);
};

template <class T>
class GrpArithmeticOrReduce: public GrpReduce<T> {
  public:
    void reduce(T*target, T*data, int nelement);
};

template <class T>
class GrpArithmeticXOrReduce: public GrpReduce<T> {
  public:
    void reduce(T*target, T*data, int nelement);
};

template <class T>
class GrpProductReduce: public GrpReduce<T> {
  public:
    void reduce(T*target, T*data, int nelement);
};

template <class T>
class GrpFunctionReduce: public GrpReduce<T> {
  private:
    void (*func_)(T*target,T*data,int nelement);
  public:
    GrpFunctionReduce(void(*func)(T*,T*,int)):func_(func) {}
    void reduce(T*target, T*data, int nelement);
};

/** The MessageGrp abstract class provides
    a mechanism for moving data and objects between
    nodes in a parallel machine. */
class MessageGrp: public DescribedClass {
  private:
    // These are initialized by the initialize() member (see below).
    int me_;
    int n_;
    int nclass_;
    int gop_max_;
    std::map<ClassDescP,int> classdesc_to_index_;
    ClassDescP *index_to_classdesc_;
  protected:
    /** The classdesc_to_index_ and index_to_classdesc_ arrays
        cannot be initialized by the MessageGrp CTOR, because
        the MessageGrp specialization has not yet been initialized
        and communication is not available.  CTOR's of specializations
        of MessageGrp must call the initialize member in their body
        to complete the initialization process. */
    void initialize(int me, int n);

    Ref<MachineTopology> topology_;

    int debug_;
  public:
    MessageGrp();
    MessageGrp(const Ref<KeyVal>&);
    virtual ~MessageGrp();
    
    /// Returns the number of processors.
    int n() { return n_; }
    /// Returns my processor number.  In the range [0,n()).
    int me() { return me_; }

    /** Returns a copy of this MessageGrp specialization that provides
        an independent communication context. */
    virtual Ref<MessageGrp> clone(void)=0;
    
    /** The default message group contains the primary message group to
        be used by an application. */
    static void set_default_messagegrp(const Ref<MessageGrp>&);
    /// Returns the default message group.
    static MessageGrp* get_default_messagegrp();

    /** Create a message group.  This routine looks for a -messagegrp
        argument, then the environmental variable MESSAGEGRP to decide which
        specialization of MessageGrp would be appropriate.  The
        argument to -messagegrp should be either string for a
        ParsedKeyVal constructor or a classname.  If this returns
        null, it is up to the programmer to create a MessageGrp. */
    static MessageGrp* initial_messagegrp(int &argc, char** &argv);

    /** Send messages sequentially to the target processor.
        Similar members exist for each of the basic types. */
    virtual void send(int target, const double* data, int ndata);
    virtual void send(int target, const unsigned int* data, int ndata);
    virtual void send(int target, const int* data, int ndata);
    virtual void send(int target, const char* data, int nbyte);
    virtual void send(int target, const unsigned char* data, int nbyte);
    virtual void send(int target, const signed char* data, int nbyte);
    virtual void send(int target, const short* data, int ndata);
    virtual void send(int target, const long* data, int ndata);
    virtual void send(int target, const float* data, int ndata);
    void send(int target, double data) { send(target,&data,1); }
    void send(int target, int data) { send(target,&data,1); }
    virtual void raw_send(int target, const void* data, int nbyte) = 0;

    /** Send typed messages to the target processor.
        Similar members exist for each of the basic types. */
    virtual void sendt(int target, int type, const double* data, int ndata);
    virtual void sendt(int target, int type, const unsigned int* data, int ndata);
    virtual void sendt(int target, int type, const int* data, int ndata);
    virtual void sendt(int target, int type, const char* data, int nbyte);
    virtual void sendt(int target, int type, const unsigned char* data, int nbyte);
    virtual void sendt(int target, int type, const signed char* data, int nbyte);
    virtual void sendt(int target, int type, const short* data, int ndata);
    virtual void sendt(int target, int type, const long* data, int ndata);
    virtual void sendt(int target, int type, const float* data, int ndata);
    void sendt(int target, int type, double data) {sendt(target,type,&data,1);}
    void sendt(int target, int type, int data) {sendt(target,type,&data,1);}
    virtual void raw_sendt(int target, int type, const void* data, int nbyte) = 0;

    /** Receive messages sent sequentually from the sender.
        Similar members exist for each of the basic types. */
    virtual void recv(int sender, double* data, int ndata);
    virtual void recv(int sender, unsigned int* data, int ndata);
    virtual void recv(int sender, int* data, int ndata);
    virtual void recv(int sender, char* data, int nbyte);
    virtual void recv(int sender, unsigned char* data, int nbyte);
    virtual void recv(int sender, signed char* data, int nbyte);
    virtual void recv(int sender, short* data, int ndata);
    virtual void recv(int sender, long* data, int ndata);
    virtual void recv(int sender, float* data, int ndata);
    void recv(int sender, double& data) { recv(sender,&data,1); }
    void recv(int sender, int& data) { recv(sender,&data,1); }
    virtual void raw_recv(int sender, void* data, int nbyte) = 0;

    /** Receive messages sent by type.
        Similar members exist for each of the basic types. */
    virtual void recvt(int type, double* data, int ndata);
    virtual void recvt(int type, unsigned int* data, int ndata);
    virtual void recvt(int type, int* data, int ndata);
    virtual void recvt(int type, char* data, int nbyte);
    virtual void recvt(int type, unsigned char* data, int nbyte);
    virtual void recvt(int type, signed char* data, int nbyte);
    virtual void recvt(int type, short* data, int ndata);
    virtual void recvt(int type, long* data, int ndata);
    virtual void recvt(int type, float* data, int ndata);
    void recvt(int type, double& data) { recvt(type,&data,1); }
    void recvt(int type, int& data) { recvt(type,&data,1); }
    virtual void raw_recvt(int type, void* data, int nbyte) = 0;

    /// Ask if a given typed message has been received.
    virtual int probet(int type) = 0;

    /** Do broadcasts of various types of data.
        Similar members exist for each of the basic types. */
    virtual void bcast(double* data, int ndata, int from = 0);
    virtual void bcast(unsigned int* data, int ndata, int from = 0);
    virtual void bcast(int* data, int ndata, int from = 0);
    virtual void bcast(char* data, int nbyte, int from = 0);
    virtual void bcast(unsigned char* data, int nbyte, int from = 0);
    virtual void bcast(signed char* data, int nbyte, int from = 0);
    virtual void bcast(short* data, int ndata, int from = 0);
    virtual void bcast(long* data, int ndata, int from = 0);
    virtual void bcast(float* data, int ndata, int from = 0);
    virtual void raw_bcast(void* data, int nbyte, int from = 0);
    void bcast(double& data, int from = 0) { bcast(&data, 1, from); }
    void bcast(int& data, int from = 0) { bcast(&data, 1, from); }

    /** Collect data distributed on the nodes to a big array replicated
        on each node. */
    virtual void raw_collect(const void *part, const int *lengths,
                             void *whole, int bytes_per_datum=1);
    void collect(const double *part, const int *lengths, double *whole);

    /** Global sum reduction.
        Similar members exist for each of the basic types. */
    virtual void sum(double* data, int n, double* = 0, int target = -1);
    virtual void sum(unsigned int* data, int n, unsigned int* = 0, int target = -1);
    virtual void sum(int* data, int n, int* = 0, int target = -1);
    virtual void sum(char* data, int n, char* = 0, int target = -1);
    virtual void sum(unsigned char* data, int n,
                     unsigned char* = 0, int target = -1);
    virtual void sum(signed char* data, int n,
                     signed char* = 0, int target = -1);
    void sum(double& data) { sum(&data, 1); }
    void sum(int& data) { sum(&data, 1); }
    /** Global maximization.
        Similar members exist for each of the basic types. */
    virtual void max(double* data, int n, double* = 0, int target = -1);
    virtual void max(int* data, int n, int* = 0, int target = -1);
    virtual void max(unsigned int* data, int n, unsigned int* = 0, int target = -1);
    virtual void max(char* data, int n, char* = 0, int target = -1);
    virtual void max(unsigned char* data, int n,
                     unsigned char* = 0, int target = -1);
    virtual void max(signed char* data, int n,
                     signed char* = 0, int target = -1);
    void max(double& data) { max(&data, 1); }
    void max(int& data) { max(&data, 1); }
    /** Global minimization.
        Similar members exist for each of the basic types. */
    virtual void min(double* data, int n, double* = 0, int target = -1);
    virtual void min(int* data, int n, int* = 0, int target = -1);
    virtual void min(unsigned int* data, int n, unsigned int* = 0, int target = -1);
    virtual void min(char* data, int n, char* = 0, int target = -1);
    virtual void min(unsigned char* data, int n,
                     unsigned char* = 0, int target = -1);
    virtual void min(signed char* data, int n,
                     signed char* = 0, int target = -1);
    void min(double& data) { min(&data, 1); }
    void min(int& data) { min(&data, 1); }
    /** Global generic reduction.
        Similar members exist for each of the basic types. */
    virtual void reduce(double*, int n, GrpReduce<double>&,
                        double*scratch = 0, int target = -1);
    virtual void reduce(int*, int n, GrpReduce<int>&,
                        int*scratch = 0, int target = -1);
    virtual void reduce(unsigned int*, int n, GrpReduce<unsigned int>&,
                        unsigned int*scratch = 0, int target = -1);
    virtual void reduce(char*, int n, GrpReduce<char>&,
                        char*scratch = 0, int target = -1);
    virtual void reduce(unsigned char*, int n, GrpReduce<unsigned char>&,
                        unsigned char*scratch = 0, int target = -1);
    virtual void reduce(signed char*, int n, GrpReduce<signed char>&,
                        signed char*scratch = 0, int target = -1);
    virtual void reduce(short*, int n, GrpReduce<short>&,
                        short*scratch = 0, int target = -1);
    virtual void reduce(float*, int n, GrpReduce<float>&,
                        float*scratch = 0, int target = -1);
    virtual void reduce(long*, int n, GrpReduce<long>&,
                        long*scratch = 0, int target = -1);
    void reduce(double& data, GrpReduce<double>& r) { reduce(&data, 1, r); }
    void reduce(int& data, GrpReduce<int>& r) { reduce(&data, 1, r); }

    /// Synchronize all of the processors.
    virtual void sync();

    /// Return the MachineTopology object.
    Ref<MachineTopology> topology() { return topology_; }

    /** Each message group maintains an association of ClassDesc with
        a global index so SavableState information can be sent between
        nodes without needing to send the classname and look up the
        ClassDesc with each transfer.  These routines return information
        about that mapping. */
    int classdesc_to_index(const ClassDesc*);
    const ClassDesc* index_to_classdesc(int);
    int nclass() const { return nclass_; }
};

struct message_struct {
  void *buf;
  int size;
  int type;
  struct message_struct *p;
  };
typedef struct message_struct message_t;


/** ProcMessageGrp provides a concrete specialization
    of MessageGrp that supports only one node. */
class ProcMessageGrp: public MessageGrp {
  private:
    // Messages are stored in these linked lists
    message_t *sync_messages;
    message_t *type_messages;

    void sendit(message_t *& messages, int dest, int msgtype, const void* buf, int bytes);
    void recvit(message_t *& messages, int source, int type, void* buf, int bytes,
                int& last_size, int& last_type);
        
  public:
    ProcMessageGrp();
    ProcMessageGrp(const Ref<KeyVal>&);
    ~ProcMessageGrp();

    Ref<MessageGrp> clone(void);
    
    void raw_send(int target, const void* data, int nbyte);
    void raw_sendt(int target, int type, const void* data, int nbyte);
    void raw_recv(int sender, void* data, int nbyte);
    void raw_recvt(int type, void* data, int nbyte);
    void raw_bcast(void* data, int nbyte, int from);
    int probet(int type);
    void sync();
};

/** Uses integer message types to send and receive messages.
    Message group specializations that use the MPI library
    and the Paragon NX can be conveniently implemented in terms
    of this. */
class intMessageGrp: public MessageGrp {
  protected:
    int msgtype_nbit; // the total number of bits available
    int ctl_nbit; // control information bits
    int seq_nbit; // sequence information bits
    int typ_nbit; // type information bits
    int src_nbit; // source information bits

    // Masks for the fields in the type.
    int ctl_mask;
    int seq_mask;
    int typ_mask;
    int src_mask;

    // Shifts to construct a message type.
    int ctl_shift;
    int seq_shift;
    int typ_shift;
    int src_shift;

    int msgtype_typ(int msgtype);
    int typ_msgtype(int usrtype);
    int seq_msgtype(int source, int seq);

    // The next sequence number for each node is stored in these.
    int *source_seq;
    int *target_seq;
    
    /// Must be implemented by specializations.
    virtual void basic_send(int target, int type, const void* data, int nbyte) = 0;
    /// Must be implemented by specializations.
    virtual void basic_recv(int type, void* data, int nbyte) = 0;
    /// Must be implemented by specializations.
    virtual int basic_probe(int type) = 0;

    intMessageGrp();
    intMessageGrp(const Ref<KeyVal>&);

    void initialize(int me, int n, int nbits);
  public:
    ~intMessageGrp();

    void raw_send(int target, const void* data, int nbyte);
    void raw_recv(int sender, void* data, int nbyte);
    void raw_sendt(int target, int type, const void* data, int nbyte);
    void raw_recvt(int type, void* data, int nbyte);

    int probet(int);

    int leftover_ctl_bits();
};

}

#include <util/group/messaget.h>

#endif


// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
