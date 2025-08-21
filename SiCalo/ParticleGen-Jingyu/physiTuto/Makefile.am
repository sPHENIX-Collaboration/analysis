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
  tutorial.h \
  CaloOnly.h \
  caloHistGen.h \
  TruthToSvtxTrack.h

lib_LTLIBRARIES = \
  libtutorial.la

libtutorial_la_SOURCES = \
  tutorial.cc \
  CaloOnly.cc \
  caloHistGen.cc \
  TruthToSvtxTrack.cc
  
libtutorial_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lHepMC \
  -lCLHEP \
  -ltrack_io \
  -ltrack \
  -lcalo_io \
  -lfun4all \
  -lg4detectors \
  -lg4detectors_io \
  -lcdbobjects \
  -lphg4hit \
  -lg4eval

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libtutorial.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
