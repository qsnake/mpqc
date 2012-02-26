//
// bug.h
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

#ifdef __GNUG__
#pragma interface
#endif

#ifndef _util_misc_bug_h
#define _util_misc_bug_h

#include <util/class/class.h>
#include <util/state/state.h>
#include <util/ref/ref.h>

namespace sc {

/** The Debugger class describes what should be done when a catastrophic
error causes unexpected program termination.  It can try things such as
start a debugger running where the program died or it can attempt to
produce a stack traceback showing roughly where the program died.  These
attempts will not always succeed.  */
class Debugger: public SavableState {
  protected:
    char *prefix_;
    char *exec_;
    char *cmd_;
    volatile int debugger_ready_;

    int debug_;
    int traceback_;
    int exit_on_signal_;
    int sleep_;
    int wait_for_debugger_;
    int handle_sigint_;
    int *mysigs_;

    void init();

    static Debugger *default_debugger_;
  public:
    Debugger(const char *exec = 0);

    /** The KeyVal constructor understands the following keywords:
        <dl>
        <dt><tt>debug</tt><dd> Try to start a debugger when an error occurs.
        Doesn't work on all machines. The default is true, if possible.

        <dt><tt>traceback</tt><dd> Try to print out a traceback extracting
        return addresses from the call stack.  Doesn't work on most
        machines.  The default is true, if possible.

        <dt><tt>exit</tt><dd> Exit on errors.  The default is true.

        <dt><tt>wait_for_debugger</tt><dd> When starting a debugger go into
        an infinite loop to give the debugger a chance to attach to the
        process.  The default is true.

        <dt><tt>sleep</tt><dd> When starting a debugger wait this many
        seconds to give the debugger a chance to attach to the process.
        The default is 0.

        <dt><tt>handle_defaults</tt><dd> Handle a standard set of signals
        such as SIGBUS, SIGSEGV, etc.  The default is true.

        <dt><tt>prefix</tt><dd> Gives a string that is printed before each
        line that is printed by Debugger. The default is nothing.

        <dt><tt>cmd</tt><dd> Gives a command to be executed to start the
        debugger.  The default varies with machine.
        
        </dl> */
    Debugger(const Ref<KeyVal>&);

    Debugger(StateIn&);
    ~Debugger();

    /** The debug member attempts to start a debugger
        running on the current process. */
    virtual void debug(const char *reason = 0);
    /** The traceback member attempts a stack traceback
     for the current process.  A symbol table must be saved for
     the executable if any sense is to be made of the traceback.
     Tracebacks are currently available only for a limited number
     of architectures. */
    virtual void traceback(const char *reason = 0);
    /// Turn on or off debugging on a signel.  The default is on.
    virtual void set_debug_on_signal(int);
    /// Turn on or off traceback on a signel.  The default is on.
    virtual void set_traceback_on_signal(int);
    /// Turn on or off exit after a signel.  The default is on.
    virtual void set_exit_on_signal(int);
    /** Turn on or off running an infinite loop after the debugger is started.
        This loop gives the debugger a chance to attack to the process.
        The default is on. */
    virtual void set_wait_for_debugger(int);

    /// The Debugger will be actived when sig is caught.
    virtual void handle(int sig);
    /// This calls handle(int) with all of the major signals.
    virtual void handle_defaults();

    /// This sets a prefix which preceeds all messages printing by Debugger.
    virtual void set_prefix(const char *p);
    /// Set the prefix to the decimal represention of p followed by a ": ".
    virtual void set_prefix(int p);

    /** Sets the command to be exectuted when debug is called.
        The character sequence "$(EXEC)" is replaced by the executable
        name (see set_exec), "$(PID)" is replaced by the
        current process id, and "$(PREFIX)" is replaced by the
        prefix. */
    virtual void set_cmd(const char *);
    /// Calls set_cmd with a hopefully suitable default.
    virtual void default_cmd();
    /** Set the name of the exectuble for the current process.
        It is up to the programmer to set this, even if the Debugger
        is initialized with the KeyVal constructor. */
    virtual void set_exec(const char *);

    /// Called with signal sig is received.  This is mainly for internal use.
    virtual void got_signal(int sig);

    /// Set the global default debugger.  The initial value is null.
    static void set_default_debugger(const Ref<Debugger> &);
    /// Return the global default debugger.
    static Debugger *default_debugger();

    void save_data_state(StateOut&);
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
