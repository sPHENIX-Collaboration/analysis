AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  FullJetFinder.h

lib_LTLIBRARIES = \
  libFullJetFinder.la

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  FullJetFinder_Dict_rdict.pcm

libFullJetFinder_la_SOURCES = \
  FullJetFinder.cc \
  FullJetFinder_Dict.cc

libFullJetFinder_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lfun4all \
  -lg4dst \
  -lg4eval \
  -ljetbase

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@  $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libFullJetFinder.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@	
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
