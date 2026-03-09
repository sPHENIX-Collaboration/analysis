AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libroofithm.la

# Tell the linker where to find RooFit libs
AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -L$(ROOTSYS)/lib

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

pkginclude_HEADERS = \
  RooFitHM.h

libroofithm_la_SOURCES = \
  RooFitHM.cc 

libroofithm_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lqautils


################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = \
  libroofithm.la \
  -L$(ROOTSYS)/lib \
  -lRooFit \
  -lRooFitCore \
  -lMinuit2

  

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
	rm -f $(BUILT_SOURCES)