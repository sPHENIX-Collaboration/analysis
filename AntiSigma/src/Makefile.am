AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libantisigma.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -I$(G4_MAIN)/include

noinst_HEADERS = \
  EdepNtupleLinkDef.h

libantisigma_la_SOURCES = \
  EdepNtuple.cc \
  EdepNtuple_Dict.C \
  HijingCountNtuple.cc \
  HijingCountNtuple_Dict.C \
  HijingShowerSize.cc \
  HijingShowerSize_Dict.C \
  HitCountNtuple.cc \
  HitCountNtuple_Dict.C \
  ShowerSize.cc \
  ShowerSize_Dict.C \
  SigmaTimingNtuple.cc \
  SigmaTimingNtuple_Dict.C \
  TimingNtuple.cc \
  TimingNtuple_Dict.C \
  TowerTiming.cc \
  TowerTiming_Dict.C

libantisigma_la_LDFLAGS = \
   -L$(libdir) \
   -L$(OFFLINE_MAIN)/lib \
   -lphg4hit \
   -lgsl \
   -lgslcblas \
   -lcalo_util


################################################
# linking tests

BUILT_SOURCES = testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libantisigma.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict* $(BUILT_SOURCES)
