AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -lHepMC \
  -lCLHEP \
  -lcalo_io \
  -lcalo_reco \
  -lmbd_io \
  -lg4dst \
  -lg4eval \
  -lcalo_reco 

pkginclude_HEADERS = \
  BeamWaveform.h \
  PROTOTYPE4_FEM.h

lib_LTLIBRARIES = \
  libBeamWaveform.la

libBeamWaveform_la_SOURCES = \
  PROTOTYPE4_FEM.cc \
  BeamWaveform.cc

libBeamWaveform_la_LIBADD = \
  -lphool \
  -lcalo_reco \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libBeamWaveform.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
