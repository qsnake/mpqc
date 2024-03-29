#
# Makefile
#
# Copyright (C) 1996 Limit Point Systems, Inc.
#
# Author: Curtis Janssen <cljanss@ca.sandia.gov>
# Maintainer: LPS
#
# This file is part of the SC Toolkit.
#
# The SC Toolkit is free software; you can redistribute it and/or modify
# it under the terms of the GNU Library General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# The SC Toolkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
#
# The U.S. Government is granted a limited license as per AL 91-7.
#

TOPDIR=../../../../..
ifndef SRCDIR
  SRCDIR=$(shell pwd)
endif

include $(SRCDIR)/$(TOPDIR)/lib/GlobalMakefile
ifeq ($(HAVE_LIBINT),yes)
  DEFINES += -DHAVE_CINTS
endif
ifeq ($(INTV3_ORDER),yes)
  DEFINES += -DINTV3_ORDER
endif

TARGET_TO_MAKE = libSCintcca
BIN_OR_LIB = LIB

DEFINES += -DDMALLOC

CXXSRC = int1e.cc int2e.cc obintcca.cc tbintcca.cc intcca.cc

INC = int1e.h int2e.h obintcca.h tbintcca.h intcca.h cartit.h tform.h

CPPFLAGS += -I../../cca
LTLINKLIBOPTS += -L$(CCAFE_LIB) -R$(CCAFE_LIB) -lccafeCore \
  -L$(BABEL_LIB) -R$(BABEL_LIB) -lsidl \
  -L$(CCA_CHEM_LIB) -R$(CCA_CHEM_LIB) -lccachem_cxx_client -lccachem_cxx_server

LIBSRC = $(CXXSRC) $(CSRC) $(XCSRC)
LIBOBJ = $(LIBSRC:%.c=%.$(OBJSUF))
LIBOBJ := $(LIBOBJ:%.cc=%.$(OBJSUF))

DISTFILES = $(INC) Makefile

DEPENDINCLUDE = $(INC) $(GENINC) $(SGENINC)

#############################################################

default:: $(DEPENDINCLUDE)
interface:: $(DEPENDINCLUDE)

include $(SRCDIR)/$(TOPDIR)/lib/GlobalRules

targetclean::

$(TESTOBJ:.$(OBJSUF)=.d) $(LIBOBJ:.$(OBJSUF)=.d): $(DEPENDINCLUDE)
ifneq ($(DODEPEND),no)
include $(LIBOBJ:.$(OBJSUF)=.d) $(TESTOBJ:.$(OBJSUF)=.d)
endif

#############################################################

# need babel headers generated
ifneq ($(DODEPEND),no)
include ../../cca/.babel-stamp
endif
../../cca/.babel-stamp:
	cd ../../cca; $(MAKE) .babel-stamp
