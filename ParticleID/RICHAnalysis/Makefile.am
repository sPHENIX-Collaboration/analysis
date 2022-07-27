AUTOMAKE_OPTIONS = foreign

INCLUDES = \
	-I$(includedir) \
	-I$(OFFLINE_MAIN)/include \
	-I$(OFFLINE_MAIN)/include/eigen3 \
	-I$(ROOTSYS)/include

lib_LTLIBRARIES = \
	librichana.la

AM_CXXFLAGS = -Wall -Werror -msse2

librichana_la_LDFLAGS = \
	-L$(libdir) \
	-L$(OFFLINE_MAIN)/lib

librichana_la_LIBADD = \
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
        PidInfo.h \
        PidInfo_RICH_v1.h \
        PidInfoContainer.h \
	FastPid_RICH.h \
	RICHEvaluator.h \
	RICHParticleID.h \
	dualrich_analyzer.h \
	SetupDualRICHAnalyzer.h \
	TrackProjectorPid.h \
	PIDProbabilities.h \
	Poisson.h

librichana_la_SOURCES = \
        PidInfo_Dict.C \
        PidInfo_RICH_v1.cc \
        PidInfo_RICH_v1_Dict.C \
        PidInfoContainer.cc \
        PidInfoContainer_Dict.C \
	FastPid_RICH.C \
	FastPid_RICH_Dict.C \
	RICHEvaluator.C \
	RICHEvaluator_Dict.C \
	RICHParticleID.C \
	RICHParticleID_Dict.C \
	dualrich_analyzer.cc \
	SetupDualRICHAnalyzer.cc \
	TrackProjectorPid.cc \
	PIDProbabilities.cc \
	Poisson.cc

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^


################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = \
	testexternals.C

testexternals_LDADD = \
	librichana.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
