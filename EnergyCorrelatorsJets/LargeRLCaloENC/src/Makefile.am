AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  HelperStructs.h \
  DijetEventCuts.h \
  LargeRLENC.h \
  LargeRLENC_LEDPedestalScan.h

lib_LTLIBRARIES = \
  libLargeRLENC.la

libLargeRLENC_la_SOURCES = \
  LargeRLENC.cc \
  LargeRLENC_LEDPedestalScan.cc


libLargeRLENC_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lfun4all \
  -lg4eval \
  -lg4jets \
  -lphhepmc \
  -lcalotrigger_io \
  -lcalotrigger \
  -lCalorimeterTowerENC 

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libLargeRLENC.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
