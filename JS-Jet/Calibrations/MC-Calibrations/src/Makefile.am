AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64\
  -lHepMC \
  -lCLHEP \
  -lg4eval

pkgincludedir = $(includedir)/JESMCTreeGen

pkginclude_HEADERS = \
  JESMCTreeGen.h \
  JESMCTreeGenLinkDef.h 

pcmdir = $(libdir)

lib_LTLIBRARIES = \
  libJESMCTreeGen.la

libJESMCTreeGen_la_SOURCES = \
  JESMCTreeGen.cc


libJESMCTreeGen_la_LIBADD = \
  -lcalotrigger_io \
  -lcalotrigger \
  -lcalo_io \
  -lmbd_io \
  -lCLHEP \
  -lffarawmodules \
  -lglobalvertex \
  -lglobalvertex_io \
  -lphool \
  -lSubsysReco \
  -lfun4all \
  -lHepMC

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libJESMCTreeGen.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
