TOPDIR=../../../../..
ifndef SRCDIR
  SRCDIR=$(shell pwd)
endif
include $(SRCDIR)/$(TOPDIR)/lib/GlobalMakefile

BIN_OR_LIB = LIB
TARGET_TO_MAKE=libSCoint3

LIBSRC1 = \
build.cc \
i0100.cc \
i0101.cc \
i0111.cc \
i0200.cc \
i0201.cc \
i0202.cc \
i0211.cc \
i0212.cc \
i0222.cc \
i1100.cc \
i1111.cc \
i1200.cc \
i1201.cc \
i1211.cc \
i1212.cc \
i1222.cc \
i2200.cc \
i2201.cc \
i2211.cc \
i2222.cc \
i0100AB.cc \
i0101AB.cc \
i0111AB.cc \
i0200AB.cc \
i0201AB.cc \
i0202AB.cc \
i0211AB.cc \
i0212AB.cc \
i0222AB.cc \
i1100AB.cc \
i1111AB.cc \
i1200AB.cc \
i1201AB.cc \
i1211AB.cc \
i1212AB.cc \
i1222AB.cc \
i2200AB.cc \
i2201AB.cc \
i2211AB.cc \
i2222AB.cc

LIBSRC2= \
i0300.cc \
i0301.cc \
i0302.cc \
i0303.cc \
i0311.cc \
i0312.cc \
i0313.cc \
i0322.cc \
i0323.cc \
i0333.cc \
i1300.cc \
i1301.cc \
i1302.cc \
i1311.cc \
i1312.cc \
i1313.cc \
i1322.cc \
i1323.cc \
i1333.cc \
i2300.cc \
i2301.cc \
i2302.cc \
i2311.cc \
i2312.cc \
i2322.cc \
i2323.cc \
i2333.cc \
i3300.cc \
i3301.cc \
i3302.cc \
i3311.cc \
i3312.cc \
i3322.cc \
i3333.cc \
i0300AB.cc \
i0301AB.cc \
i0302AB.cc \
i0303AB.cc \
i0311AB.cc \
i0312AB.cc \
i0313AB.cc \
i0322AB.cc \
i0323AB.cc \
i0333AB.cc \
i1300AB.cc \
i1301AB.cc \
i1302AB.cc \
i1311AB.cc \
i1312AB.cc \
i1313AB.cc \
i1322AB.cc \
i1323AB.cc \
i1333AB.cc \
i2300AB.cc \
i2301AB.cc \
i2302AB.cc \
i2311AB.cc \
i2312AB.cc \
i2322AB.cc \
i2323AB.cc \
i2333AB.cc \
i3300AB.cc \
i3301AB.cc \
i3302AB.cc \
i3311AB.cc \
i3312AB.cc \
i3322AB.cc \
i3333AB.cc


LIBSRC3=

LIBSRC := $(LIBSRC1)

ifneq ($(ARCH),NCUBE_V2)
  LIBSRC := $(LIBSRC) $(LIBSRC2)

  LIBSRC := $(LIBSRC) $(LIBSRC3)
endif

LIBOBJ = $(LIBSRC:%.cc=%.$(OBJSUF))

DISTFILES = Makefile build.h $(LIBSRC1) $(LIBSRC2) $(LIBSRC3)

include $(SRCDIR)/$(TOPDIR)/lib/GlobalRules

$(LIBOBJ:.$(OBJSUF)=.d): $(DEPENDINCLUDE)
ifneq ($(DODEPEND),no)
include $(LIBOBJ:.$(OBJSUF)=.d) $(TESTOBJ:.o=.d)
endif
