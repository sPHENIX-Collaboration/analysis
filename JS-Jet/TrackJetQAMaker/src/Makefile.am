AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libtrackjetqamaker.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

pkginclude_HEADERS = \
  TrackJetQAMaker.h \
  TrackJetQAMakerConfig.h \
  TrackJetQAMakerHelper.h \
  TrackJetQAMakerHistDef.h \
  HitQAMaker.h \
  ClustQAMaker.h \
  TrackQAMaker.h \
  JetQAMaker.h

if ! MAKEROOT6
  ROOT5_DICTS = \
    TrackJetQAMaker_Dict.cc
endif

libtrackjetqamaker_la_SOURCES = \
  $(ROOT5_DICTS) \
  TrackJetQAMaker.cc \
  HitQAMaker.cc \
  ClustQAMaker.cc \
  TrackQAMaker.cc \
  JetQAMaker.cc

libtrackjetqamaker_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lg4dst \
  -lg4eval \
  `fastjet-config --libs`


################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libtrackjetqamaker.la

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
