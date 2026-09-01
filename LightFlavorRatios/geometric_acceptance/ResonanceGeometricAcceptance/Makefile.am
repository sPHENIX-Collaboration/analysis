AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  ResonanceGeometricAcceptance.h \
  ../../util/binning.h

lib_LTLIBRARIES = \
  libResonanceGeometricAcceptance.la

libResonanceGeometricAcceptance_la_SOURCES = \
  ResonanceGeometricAcceptance.cc

libResonanceGeometricAcceptance_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lKFParticle \
  -ltrackbase_historic \
  -lglobalvertex_io \
  -ltrack_io \
  -lg4eval

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libResonanceGeometricAcceptance.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
