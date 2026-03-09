AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -lcentrality_io \
  -lcalotrigger_io

pkginclude_HEADERS = \
  mbdAnalysis.h

lib_LTLIBRARIES = \
  libmbdAnalysis.la

libmbdAnalysis_la_SOURCES = \
  mbdAnalysis.cc

libmbdAnalysis_la_LIBADD = \
  -lphool \
  -lmbd_io \
  -lSubsysReco

libmbdAnalysis_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lcentrality_io 

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libmbdAnalysis.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
