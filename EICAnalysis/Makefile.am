AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(ROOTSYS)/include

lib_LTLIBRARIES = \
   libeicana.la

AM_CXXFLAGS = -Wall -Werror -msse2

AM_LDFLAGS = \
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
  -lPHGenFit \
  -ltrackbase_historic_io

pkginclude_HEADERS = \
  Leptoquarks.h \
  LeptoquarksReco.h \
  DISKinematicsReco.h \
  Quarkonia2LeptonsMC.h\
  PidCandidate.h\
  PidCandidatev1.h\
  TrackProjectionTools.h \
  TrackProjectorPlaneECAL.h \
  TruthTrackerHepMC.h \
  DVMPHelper.h \
  ExclusiveReco.h

# I/O dictionaries have to exist for root5 and root6. For ROOT6 we need
# pcm files in addition. If someone can figure out how to make a list
# so this list of dictionaries is transformed into a list of pcm files
# following a simple naming convention, please change this accordingly and
# let me know
ROOTDICTS = \
  PidCandidate_Dict.C \
  PidCandidatev1_Dict.C
# for root6 we need pcm and dictionaries but only for
# i/o classes. For root5 we need only dictionaries but
# those for i/o and classes available on the cmd line
# MAKEROOT6 is set in the configure.ac, true for root6
if MAKEROOT6
# this is a tweak to install files in $(libdir), automake refuses
# to install other files in libdir, this construct gets around this
pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  PidCandidate_Dict_rdict.pcm \
  PidCandidatev1_Dict_rdict.pcm
else
  ROOT5_DICTS = \
    DISKinematicsReco_Dict.C \
    DVMPHelper_Dict.C \
    ExclusiveReco_Dict.C \
    Leptoquarks_Dict.C \
    LeptoquarksReco_Dict.C \
    Quarkonia2LeptonsMC_Dict.C \
    TrackProjectionTools_Dict.C \
    TrackProjectorPlaneECAL_Dict.C \
    TruthTrackerHepMC_Dict.C
endif

libeicana_la_SOURCES = \
  $(ROOTDICTS) \
  $(ROOT5_DICTS) \
  Leptoquarks.C \
  LeptoquarksReco.C \
  DISKinematicsReco.C \
  Quarkonia2LeptonsMC.C \
  PidCandidate.C\
  PidCandidatev1.C\
  TrackProjectionTools.C \
  TrackProjectorPlaneECAL.cc \
  TruthTrackerHepMC.C \
  DVMPHelper.C \
  ExclusiveReco.C

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

# just to get the dependency                                                                                     
%_Dict_rdict.pcm: %_Dict.C ;

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
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
