AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

lib_LTLIBRARIES = \
  libLaserClusterQA.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ROOTSYS)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 

libLaserClusterQA_la_LIBADD = \
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

libLaserClusterQA_la_SOURCES = \
  laserClusterQA.cc \
  laserQA.cc \
  laserQA_multiple.cc \
  laminationClusterQA.cc

pkginclude_HEADERS = \
  laserClusterQA.h \
  laserQA.h \
  laserQA_multiple.h \
  laminationClusterQA.h

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libLaserClusterQA.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
