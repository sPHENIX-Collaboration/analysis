AUTOMAKE_OPTIONS = foreign

CXXFLAGS = -Wno-error

lib_LTLIBRARIES = \
    libsphanalysis_calo.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

pkginclude_HEADERS = \
  sPHAnalysis_calo.h

if ! MAKEROOT6
  ROOT5_DICTS = \
    sPHAnalysis_calo_Dict.cc
endif

libsphanalysis_calo_la_SOURCES = \
  $(ROOT5_DICTS) \
  sPHAnalysis_calo.cc 

libsphanalysis_calo_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lg4dst 

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libsphanalysis_calo.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm

