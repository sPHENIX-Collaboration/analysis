AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libtagandprobe.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include \
  -DHomogeneousField

pkginclude_HEADERS = \
  TagAndProbe.h

libtagandprobe_la_SOURCES = \
  TagAndProbe.cc

libtagandprobe_la_LIBADD = \
  -lcalo_io \
  -lfun4all \
  -lg4eval \
  -lphhepmc \
  -lKFParticle \
  -ltrack_io \
  -ltrackeralign \
  -ltrackbase_historic_io

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

BUILT_SOURCES = testexternals.cc

testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libtagandprobe.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
