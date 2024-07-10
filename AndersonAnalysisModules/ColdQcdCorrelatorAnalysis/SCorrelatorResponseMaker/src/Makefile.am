AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libscorrelatorresponsemaker.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  $(MYINSTALL)/lib/libscorrelatorutilities.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

pkginclude_HEADERS = \
  SCorrelatorResponseMaker.h \
  SCorrelatorResponseMakerConfig.h \
  SCorrelatorResponseMakerInput.h \
  SCorrelatorResponseMakerOutput.h

if ! MAKEROOT6
  ROOT5_DICTS = \
    SCorrelatorResponseMaker_Dict.cc
endif

libscorrelatorresponsemaker_la_SOURCES = \
  $(ROOT5_DICTS) \
  SCorrelatorResponseMaker.cc

libscorrelatorresponsemaker_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lg4dst \
  -lg4eval


################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libscorrelatorresponsemaker.la

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
