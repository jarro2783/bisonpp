# Generated automatically from Makefile.in by configure.
# Makefile for bison
# Copyright (C) 1988, 1989, 1991, 1993 Bob Corbett and Free Software Foundation, Inc.
# 
# This file is part of Bison, the GNU Compiler Compiler.
# 
# Bison is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
# 
# Bison is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Bison; see the file COPYING.  If not, write to
# the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

#### Start of system configuration section. ####

srcdir = .
VPATH = .

CC = cc
INSTALL = /bin/install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 644
MAKEINFO = makeinfo

# Things you might add to DEFS:
# -DSTDC_HEADERS	If you have ANSI C headers and libraries.
# -DHAVE_STRING_H	If you don't have ANSI C headers but have string.h.
# -DHAVE_MEMORY_H	If you don't have ANSI C headers and have memory.h.
# -DHAVE_STRERROR	If you have strerror function.
DEFS =  -Dconst= -DHAVE_STRING_H=1 -DHAVE_STDLIB_H=1 -DHAVE_MEMORY_H=1 -DHAVE_ALLOCA_H=1

CFLAGS = -g
LDFLAGS = -g

LIBS = 

# Some System V machines do not come with libPW.  If this is true, use
# the GNU alloca.o here.
ALLOCA = 

prefix = $(ENV_DIR)/tools
exec_prefix = $(prefix)

# where the installed binary goes
bindir = $(exec_prefix)/bin

# where the parsers go
datadir = $(prefix)/lib

# where the info files go
infodir = $(ENV_DIR)/doc/info

# where manual pages go and what their extensions should be
manext = 1
mandir = $(ENV_DIR)/doc/man/man$(manext)

# dman documentation parser
.SUFFIXES : .dman
.dman :
	dman <$*.dman >$@
	pman -t $@ >$*.ps
default : all

backup :
	find . \( -name '*.o' -o -name '*%' -o -name 'bison++' -o -name '.tar.excludes' \) -print >.tar.excludes
	tar cvfX - .tar.excludes . | compress -cv >../bison++.tar.Z

#### End of system configuration section. ####

SHELL = /bin/sh
DISTFILES = COPYING ChangeLog Makefile.in configure configure.in \
    REFERENCES bison.1 bison++.1 bison.rnh configure.bat \
    bison.cc bison.h bison.hairy \
    lr0.c allocate.c closure.c conflict.c derives.c \
    files.c getargs.c gram.c lalr.c lex.c main.c nullable.c \
    output.c print.c reader.c reduce.c symtab.c version.c \
    warshall.c files.h gram.h lex.h machine.h new.h state.h \
    symtab.h system.h types.h bison.cld build.com vmsgetargs.c \
    vmshlp.mar README INSTALL bison.texinfo bison.info* texinfo.tex \
    getopt.c getopt.h getopt1.c alloca.c mkinstalldirs

# This rule allows us to supply the necessary -D options
# in addition to whatever the user asks for.
.c.o:
	$(CC) -c $(DEFS) -I$(srcdir)/../include $(CPPFLAGS) $(CFLAGS) $<

# names of parser files
PFILE = bison.cc
PFILE1 = bison.hairy
HFILE = bison.h

PFILES = -DXPFILE=\"$(datadir)/$(PFILE)\" \
	 -DXHFILE=\"$(datadir)/$(HFILE)\" \
	 -DXPFILE1=\"$(datadir)/$(PFILE1)\"

OBJECTS = lr0.o allocate.o closure.o conflict.o derives.o files.o	\
	  getargs.o gram.o lalr.o lex.o					\
	  main.o nullable.o output.o print.o reader.o reduce.o symtab.o	\
	  warshall.o version.o						\
	  getopt.o getopt1.o $(ALLOCA)

all: bison++ bison.info

Makefile: Makefile.in config.status
	./config.status

config.status: configure
	./config.status --recheck

configure: configure.in
	cd $(srcdir); autoconf

clean:
	rm -f *.o core bison++

mostlyclean: clean

distclean: clean
	rm -f Makefile config.status

realclean: distclean
	rm -f TAGS *.info*

# Most of these deps are in case using RCS.
install: all bison++.1 bison.1 $(srcdir)/$(PFILE) $(srcdir)/$(PFILE1) $(srcdir)/$(HFILE) installdirs uninstall
	$(INSTALL_PROGRAM) bison++ $(bindir)/bison++
	-cd $(datadir); rm -f $(PFILE) $(HFILE)
	#-cd $(datadir); rm -f $(PFILE1) 
	$(INSTALL_DATA) $(srcdir)/$(PFILE) $(datadir)/$(PFILE)
	$(INSTALL_DATA) $(srcdir)/$(HFILE) $(datadir)/$(HFILE)
	-chmod a+r $(datadir)/$(PFILE) $(datadir)/$(HFILE) 
	#$(INSTALL_DATA) $(srcdir)/$(PFILE1) $(datadir)/$(PFILE1)
	-chmod a+r $(datadir)/$(PFILE) $(datadir)/$(HFILE) 
	#-chmod a+r $(datadir)/$(PFILE1) 
	#-$(INSTALL_DATA) $(srcdir)/bison.1 $(mandir)/bison.$(manext)
	#-chmod a+r $(mandir)/bison.$(manext)
	-$(INSTALL_DATA) $(srcdir)/bison++.1 $(mandir)/bison++.$(manext)
	-chmod a+r $(mandir)/bison++.$(manext)
	#for file in $(srcdir)/bison.info*; \
	#do $(INSTALL_DATA) $$file $(infodir)/$$file; \
	#done
# Make sure all installation directories, e.g. $(bindir) actually exist by
# making them if necessary.
installdirs:
	-sh $(srcdir)/mkinstalldirs $(bindir) $(datadir) $(libdir) $(infodir) $(mandir)

uninstall:
	rm -f $(bindir)/bison++
	-cd $(datadir); rm -f $(PFILE) $(HFILE)
	#-cd $(datadir); rm -f $(PFILE1)
	rm -f $(mandir)/bison++.$(manext)
	#rm -f $(mandir)/bison.$(manext) $(infodir)/bison.info*

bison++: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBS)

dist: bison.info
	echo bison-`sed -e '/version_string/!d' -e 's/[^0-9.]*\([0-9.]*\).*/\1/' -e q version.c` > .fname
	-rm -rf `cat .fname`
	mkdir `cat .fname`
	cp -p getopt.[ch] getopt1.c alloca.c `cat .fname`
	tar chZf `cat .fname`.tar.Z `cat .fname`
	dst=`cat .fname`; for f in $(DISTFILES); do \
	   ln $(srcdir)/$$f $$dst/$$f || { echo copying $$f; cp -p $(srcdir)/$$f $$dst/$$f ; } \
	done
	tar --gzip -chf `cat .fname`.tar.gz `cat .fname`
	-rm -rf `cat .fname` .fname

bison.info: bison.texinfo
	$(MAKEINFO) $(srcdir)/bison.texinfo

TAGS: *.c *.h
	etags *.c *.h

# This file is different to pass the parser file names to the compiler.
files.o: files.c
	$(CC) -c $(PFILES) $(DEFS) $(CPPFLAGS) $(CFLAGS) \
	   $(srcdir)/files.c $(OUTPUT_OPTION)

lr0.o: system.h machine.h new.h gram.h state.h
closure.o: system.h machine.h new.h gram.h
conflict.o: system.h machine.h new.h files.h gram.h state.h
derives.o: system.h new.h types.h gram.h
files.o: system.h files.h new.h gram.h
getargs.o: system.h files.h
lalr.o: system.h machine.h types.h state.h new.h gram.h
lex.o: system.h files.h symtab.h lex.h
main.o: system.h machine.h
nullable.o: system.h types.h gram.h new.h
output.o: system.h machine.h new.h files.h gram.h state.h
print.o: system.h machine.h new.h files.h gram.h state.h
reader.o: system.h files.h new.h symtab.h lex.h gram.h
reduce.o: system.h machine.h files.h new.h gram.h
symtab.o: system.h new.h symtab.h gram.h
warshall.o: system.h machine.h

# Prevent GNU make v3 from overflowing arg limit on SysV.
.NOEXPORT:
