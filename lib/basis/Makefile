TOPDIR=../..
ifndef SRCDIR
  SRCDIR=$(shell pwd)
endif

include $(SRCDIR)/$(TOPDIR)/lib/GlobalMakefile

include $(SRCDIR)/$(TOPDIR)/lib/GlobalRules

install::
	$(INSTALL) $(INSTALLDIROPT) $(installroot)$(scdatadir)/basis
	$(INSTALL) $(INSTALLLIBOPT) $(SRCDIR)/*.kv $(installroot)$(scdatadir)/basis

install_devel::
