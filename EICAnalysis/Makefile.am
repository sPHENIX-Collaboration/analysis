AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(ROOTSYS)/include

lib_LTLIBRARIES = \
   libeicana.la

AM_CXXFLAGS = -Wall -Werror -msse2

libeicana_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libeicana_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4eval \
  -lphhepmc \
  -lg4detectors \
  -lgenfit2 \
  -lphfield \
  -lgenfit2exp \
  -lPHGenFit


pkginclude_HEADERS = \
  Leptoquarks.h \
  LeptoquarksReco.h \
  DISKinematicsReco.h \
  Quarkonia2LeptonsMC.h\
  PidCandidate.h\
  PidCandidatev1.h\
  TrackProjectionTools.h \
  TrackProjectorPid.h \
  TruthTrackerHepMC.h

libeicana_la_SOURCES = \
  Leptoquarks.C \
  Leptoquarks_Dict.C \
  LeptoquarksReco.C \
  LeptoquarksReco_Dict.C \
  DISKinematicsReco.C \
  DISKinematicsReco_Dict.C \
  Quarkonia2LeptonsMC.C \
  Quarkonia2LeptonsMC_Dict.C\
  PidCandidate.C\
  PidCandidate_Dict.C\
  PidCandidatev1.C\
  PidCandidatev1_Dict.C\
  TrackProjectionTools.C \
  TrackProjectorPid.cc \
  TruthTrackerHepMC.C

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^


################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = \
  testexternals.C

testexternals_LDADD = \
  libeicana.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
