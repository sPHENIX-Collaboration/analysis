
AUTOMAKE_OPTIONS = foreign
CXXFLAGS += -g -O2

lib_LTLIBRARIES = \
    libPHFlowJetMaker.la

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -I$(G4_MAIN)/include/Geant4 \
   -I$(HOME)/work/install/include

noinst_HEADERS = \
  PHFlowJetMakerLinkDef.h \
  PHFlowJetMaker.h

libPHFlowJetMaker_la_SOURCES = \
  PHFlowJetMaker.C 

nodist_libPHFlowJetMaker_la_SOURCES = \
  PHFlowJetMaker_Dict.C

libPHFlowJetMaker_la_LDFLAGS = \
   -L$(libdir) \
   -L$(OFFLINE_MAIN)/lib \
   -L$(HOME)/work/install/lib \
   -lcalo_util \
   -lfun4all \
   -lg4jets \
   -lphg4hit \
   -lg4detectors \
   -lg4testbench \
   -lfastjet \
   -lg4hough \
   -lCGAL

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libPHFlowJetMaker.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

CLEANFILES = PHFlowJetMaker_Dict.C

# rules to make root dictionaries
PHFlowJetMaker_Dict.C: \
  PHFlowJetMaker.h 
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

