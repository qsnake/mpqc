TOPDIR=../..
ifndef SRCDIR
  SRCDIR=$(shell pwd)
endif

include $(SRCDIR)/$(TOPDIR)/lib/GlobalMakefile

include $(SRCDIR)/$(TOPDIR)/lib/GlobalRules

install::
	$(INSTALL) $(INSTALLDIROPT) $(installroot)$(scdatadir)/elisp
	$(INSTALL) $(INSTALLLIBOPT) $(SRCDIR)/*.el $(installroot)$(scdatadir)/elisp
