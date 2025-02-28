AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libbeambackgroundfilterandqa.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

pkginclude_HEADERS = \
  BeamBackgroundFilterAndQA.h \
  BeamBackgroundFilterAndQADefs.h \
  BaseBeamBackgroundFilter.h \
  NullFilter.h \
  StreakSidebandFilter.h \
  TestPHFlags.h

if ! MAKEROOT6
  ROOT5_DICTS = \
    BeamBackgroundFilterAndQA_Dict.cc
endif

libbeambackgroundfilterandqa_la_SOURCES = \
  $(ROOT5_DICTS) \
  BeamBackgroundFilterAndQA.cc \
  NullFilter.cc \
  StreakSidebandFilter.cc \
  TestPHFlags.cc

libbeambackgroundfilterandqa_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lpdbcalBase \
  -lphg4hit \
  -lphparameter \
  -lg4dst \
  -lg4eval \
  -lqautils \
  `fastjet-config --libs`


################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libbeambackgroundfilterandqa.la

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
