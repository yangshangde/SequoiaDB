# Generated automatically from Makefile.in by configure.
# Makefile for Autoconf.
# Copyright (C) 1992, 1993, 1994, 1998 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

#### Start of system configuration section. ####

srcdir = .

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
MAKEINFO = makeinfo
TEXI2DVI = texi2dvi
M4 = /usr/bin/m4
AWK = gawk
PERL = /usr/bin/perl

# Programs that are ALWAYS installed (and are created in the build dir).
ASCRIPTS = autoconf autoheader autoreconf autoupdate ifnames 

# M4 input that is frozen.
M4FROZEN = autoconf.m4f autoheader.m4f

# All programs, including those only installed if you have perl.
SCRIPTS = $(ASCRIPTS) autoscan

transform=s,x,x,

prefix = /usr/local
exec_prefix = ${prefix}

# Directory in which to install scripts.
bindir = ${exec_prefix}/bin

# Directory in which to install library files.
datadir = ${prefix}/share
acdatadir = $(datadir)/autoconf

# Directory in which to install documentation info files.
infodir = ${prefix}/info

#### End of system configuration section. ####

SHELL = /bin/sh

SUBDIRS = testsuite

M4FILES = autoconf.m4 acgeneral.m4 acoldnames.m4 acspecific.m4 autoheader.m4

# Files that can be generated, but should be up to date for a distribution.
DISTDEP = info Makefile
# Files to distribute.
DISTFILES = AUTHORS COPYING ChangeLog ChangeLog.1 INSTALL \
	Makefile.in NEWS README TODO $(M4FILES) \
	acconfig.h acfunctions acheaders acidentifiers \
	acmakevars acprograms autoconf.info* \
	autoconf.sh autoconf.texi install.texi \
	autoheader.sh autoscan.pl autoreconf.sh autoupdate.sh ifnames.sh \
	config.guess config.sub configure configure.in \
	install-sh mkinstalldirs texinfo.tex \
	testsuite/Makefile.in testsuite/config/*.exp \
	testsuite/lib/*.exp testsuite/autoconf.[gs]/*.exp \
	standards.texi make-stds.texi standards.info*

editsh = sed -e 's,@''datadir''@,$(acdatadir),g' -e \
	's,@''M4''@,$(M4),g'  -e 's,@''AWK''@,$(AWK),g' \
	-e 's,@''SHELL''@,$(SHELL),g'
editpl = sed -e 's,@''datadir''@,$(acdatadir),g' -e 's,@''PERL''@,$(PERL),g'

all: ${SCRIPTS} ${M4FROZEN} info

.SUFFIXES:
.SUFFIXES: .sh .pl .m4 .m4f

.sh:
	rm -f $@ $@.tmp
	$(editsh) $< > $@.tmp && chmod +x $@.tmp && mv $@.tmp $@

.pl:
	rm -f $@ $@.tmp
	$(editpl) $< > $@.tmp && chmod +x $@.tmp && mv $@.tmp $@

.m4.m4f:
	@case `$(M4) --help </dev/null 2>&1` in \
	*reload-state*) echo freezing $*.m4; \
	  $(M4) -F $*.m4f -I${srcdir} ${srcdir}/$*.m4 ;; \
	*traditional*) ;; \
	*) echo Error: Autoconf requires GNU m4 1.1 or later; exit 1 ;; \
	esac

autoconf.m4f: autoconf.m4 acgeneral.m4 acspecific.m4 acoldnames.m4
autoheader.m4f: autoheader.m4 acgeneral.m4 acspecific.m4 acoldnames.m4

info: autoconf.info standards.info INSTALL

# Use --no-split to avoid creating filenames > 14 chars.
autoconf.info: autoconf.texi install.texi
	$(MAKEINFO) -I$(srcdir) $(srcdir)/autoconf.texi --no-split --output=$@

INSTALL: install.texi
	$(MAKEINFO) -I$(srcdir) $(srcdir)/install.texi --output=$@ \
	--no-headers --no-validate

standards.info: standards.texi make-stds.texi
	$(MAKEINFO) -I$(srcdir) $(srcdir)/standards.texi --no-split --output=$@

dvi: autoconf.dvi standards.dvi

autoconf.dvi: autoconf.texi
	$(TEXI2DVI) $(srcdir)/autoconf.texi

standards.dvi: standards.texi make-stds.texi
	$(TEXI2DVI) $(srcdir)/standards.texi

check: all
	rootme=`pwd`; srcrootme=`cd $(srcdir); pwd`; \
	test -r install-sh || cp $(srcdir)/install-sh .; \
	cd testsuite && ${MAKE} $@ AUTOCONF=$$rootme/autoconf \
	AUTOCONFFLAGS="-m $$srcrootme"

installcheck: all install
	cd testsuite && ${MAKE} AUTOCONF=${bindir}/autoconf $@

installdirs:
	$(SHELL) ${srcdir}/mkinstalldirs $(bindir) $(infodir) $(acdatadir)

install: all $(M4FILES) acconfig.h installdirs install-info
	for p in $(ASCRIPTS); do \
	  $(INSTALL_PROGRAM) $$p $(bindir)/`echo $$p|sed '$(transform)'`; \
	done
	for i in $(M4FROZEN); do \
	  $(INSTALL_DATA) $$i $(acdatadir)/$$i; \
	done
	for i in $(M4FILES) acconfig.h; do \
	  $(INSTALL_DATA) $(srcdir)/$$i $(acdatadir)/$$i; \
	done
	-if test -f autoscan; then \
	$(INSTALL_PROGRAM) autoscan $(bindir)/`echo autoscan|sed '$(transform)'`; \
	for i in acfunctions acheaders acidentifiers acprograms \
	  acmakevars; do \
	$(INSTALL_DATA) $(srcdir)/$$i $(acdatadir)/$$i; \
	done; \
	else :; fi

# Don't cd, to avoid breaking install-sh references.
install-info: info installdirs
	if test -f autoconf.info; then \
	  for i in *.info*; do \
	    $(INSTALL_DATA) $$i $(infodir)/$$i; \
	  done; \
	else \
	  for i in $(srcdir)/*.info*; do \
	    $(INSTALL_DATA) $$i $(infodir)/`echo $$i | sed 's|^$(srcdir)/||'`; \
	  done; \
	fi

uninstall:
	for p in $(SCRIPTS); do \
	  rm -f $(bindir)/`echo $$p|sed '$(transform)'`; \
	done
	rm -fr $(acdatadir)
	cd $(infodir) && rm -f autoconf.info*
	if test -f standards.info || test -f $(srcdir)/standards.info; \
	then cd $(infodir) && rm -f standards.info*; fi

${srcdir}/configure: configure.in $(M4FILES)
	cd $(srcdir) && \
	rm -f configure configure.tmp && \
	$(M4) autoconf.m4 configure.in > configure.tmp && \
	chmod +x configure.tmp && mv configure.tmp configure
Makefile: Makefile.in config.status
	$(SHELL) ./config.status
config.status: configure
	$(SHELL) ./config.status --recheck

maintainer-clean::
	@echo "This command is intended for maintainers to use;"
	@echo "rebuilding the deleted files requires makeinfo."
	rm -f TAGS *.info* INSTALL

clean mostlyclean distclean maintainer-clean::
	for dir in $(SUBDIRS); do \
	  echo making $@ in $$dir ; \
	  (cd $$dir && $(MAKE) $@) ; \
	done

clean mostlyclean distclean maintainer-clean::
	rm -f $(SCRIPTS) *.tmp 
	rm -f *.aux *.cp *.cps *.dvi *.fn *.fns *.ky *.kys *.log
	rm -f *.pg *.pgs *.toc *.tp *.tps *.vr *.vrs
	rm -f *.ev *.evs *.ov *.ovs *.cv *.cvs *.ma *.mas

distclean maintainer-clean::
	rm -f Makefile config.status config.cache config.log

TAGS:
	etags ${srcdir}/*.m4 ${srcdir}/*.sh ${srcdir}/[a-z]*.in ${srcdir}/*.texi

# Don't depend on DISTFILES because there's no rule for "standards.info*".
dist: $(DISTDEP)
	distname=`sed -e '/define(AC_ACVERSION,/!d' \
	-e 's/[^0-9.]*\([0-9.]*\).*/autoconf-\1/' -e q acgeneral.m4`; \
	rm -fr $$distname; \
	mkdir $$distname $$distname/testsuite $$distname/testsuite/config \
	$$distname/testsuite/lib $$distname/testsuite/autoconf.g \
	$$distname/testsuite/autoconf.s; \
	for file in $(DISTFILES); do \
	  ln $$file $$distname/$$file \
	  || { echo copying $$file instead; cp -p $$file $$distname/$$file;}; \
	done; \
	chmod -R a+rX $$distname; \
	tar -chz -f $$distname.tar.gz $$distname; \
	rm -fr $$distname
