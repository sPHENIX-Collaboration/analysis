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
  INTTvtxZLinkDef.h \
  INTTvtxZF4AObj.h \
  InttZVertexFinderTrapezoidal.h

lib_LTLIBRARIES = \
  libInttZVertexFinderTrapezoidal.la

libInttZVertexFinderTrapezoidal_la_SOURCES = \
  INTTvtxZF4AObjDict.cc \
  INTTvtxZF4AObj.cc \
  InttZVertexFinderTrapezoidal.cc

libInttZVertexFinderTrapezoidal_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -ltrackbase_historic_io \
  -ltrack_io

libInttZVertexFinderTrapezoidal_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -I$(ROOTSYS)/include \
  -lfun4all \
  -lg4eval

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  INTTvtxZF4AObjDict_rdict.pcm

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libInttZVertexFinderTrapezoidal.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
