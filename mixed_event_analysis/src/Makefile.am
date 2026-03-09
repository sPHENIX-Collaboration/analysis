AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

lib_LTLIBRARIES = \
  libmixedjet.la

pkginclude_HEADERS = \
  MixedJetAnalysis.h

ROOTDICTS = 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = 

libmixedjet_la_SOURCES = \
  MixedJetAnalysis.cc

libmixedjet_la_LIBADD = \
  -lcentrality_io \
  -leventplaneinfo_io \
  -lglobalvertex \
  -lmbd_io \
  -lcalotrigger_io \
  -lSubsysReco \
  -ljetbackground\
  -ljetbase \
  -lffamodules \
  -lffarawobjects

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_mixedjet

testexternals_mixedjet_SOURCES = testexternals.cc
testexternals_mixedjet_LDADD   = libmixedjet.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
