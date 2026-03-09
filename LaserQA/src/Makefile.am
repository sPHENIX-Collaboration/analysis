AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libLaserQA.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ROOTSYS)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 

libLaserQA_la_LIBADD = \
  -lfun4all \
  -lFROG \
  -lffaobjects \
  -lphool \
  -lSubsysReco \
  -lg4detectors_io \
  -ltrack_io \
  -ltrackbase_historic_io \
  -ltrack_reco \
  -ltpc_io

libLaserQA_la_SOURCES = \
  laserQA.cc 

pkginclude_HEADERS = \
  laserQA.h 

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libLaserQA.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
