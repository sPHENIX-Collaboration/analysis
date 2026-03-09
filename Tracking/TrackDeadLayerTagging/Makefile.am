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
  TrackDeadLayerTagging.h
 
lib_LTLIBRARIES = \
  libtrackdeadlayertagging.la

libtrackdeadlayertagging_la_SOURCES = \
  TrackDeadLayerTagging.cc

libtrackdeadlayertagging_la_LIBADD = \
  -lphool \
  -ltrack_io \
  -lg4detectors \
  -lintt \
  -lmvtx \
  -ltpc \
  -lmicromegas \
  -lglobalvertex \
  -ltrackbase_historic_io \
  -ltrack_reco \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libtrackdeadlayertagging.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
