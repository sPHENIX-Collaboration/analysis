AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/g4eval \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -lg4eval

pkginclude_HEADERS = \
  CaloEvaluatorPositionCorrection.h \
  RawClusterPositionCorrectionFull.h

lib_LTLIBRARIES = \
  libEMCalPositionDependentCalibration.la

libEMCalPositionDependentCalibration_la_SOURCES = \
  CaloEvaluatorPositionCorrection.cc \
  RawClusterPositionCorrectionFull.cc

libEMCalPositionDependentCalibration_la_LIBADD = \
  -lphool \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libEMCalPositionDependentCalibration.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
