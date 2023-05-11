
AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libPHAJMaker.la

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

noinst_HEADERS = \
  PHAJMakerLinkDef.h \
  PHAJMaker.h \
  PseudoJetPlus.h \
  JetAnalyzer.hh \
  PJTranslator.h

libPHAJMaker_la_SOURCES = \
  PHAJMaker.cc \
  PseudoJetPlus.cxx \
  JetAnalyzer.cxx \
  PJTranslator.cc

nodist_libPHAJMaker_la_SOURCES = \
  PHAJMaker_Dict.cc \
  PJTranslator_Dict.cc

libPHAJMaker_la_LDFLAGS = \
   -L$(libdir) \
   -L$(OFFLINE_MAIN)/lib \
   -lg4dst \
   -lcalo_util \
   -lfun4all \
   -lphg4hit \
   -lg4detectors \
   -lg4testbench \
   -lfastjet \
   -lfastjettools \
   -lg4hough \
   -lCGAL

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libPHAJMaker.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

CLEANFILES = PHAJMaker_Dict.cc PJTranslator_Dict.cc

# rules to make root dictionaries
PHAJMaker_Dict.cc: \
  PHAJMaker.h \
  PHAJMakerLinkDef.h 
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

PJTranslator_Dict.cc: \
  PJTranslator.h \
  PJTranslatorLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^
