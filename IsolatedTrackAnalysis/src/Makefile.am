AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  IsolatedTrackAnalysis.h

lib_LTLIBRARIES = \
  libIsolatedTrackAnalysis.la

libIsolatedTrackAnalysis_la_SOURCES = \
  IsolatedTrackAnalysis.cc

libIsolatedTrackAnalysis_la_LIBADD = \
  -lphool \
	-ltrackbase_historic_io \
	-lcalo_io \
	-lg4eval \
	-lSubsysReco \
	-lg4dst

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libIsolatedTrackAnalysis.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
