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
  sEPDValidation.h \
  geometry_constants.h \
  MedianFinder.h \
  EventSkip.h \
  CaloCheck.h \
  CalibrateQVec.h \
  JetValidation.h \
  JetUtils.h \
  sEPD_DataMC_Validation.h

lib_LTLIBRARIES = \
  libsEPDValidation.la

libsEPDValidation_la_SOURCES = \
  sEPDValidation.cc \
  MedianFinder.cc \
  EventSkip.cc \
  CaloCheck.cc \
  CalibrateQVec.cc \
  JetValidation.cc \
  JetUtils.cc \
  sEPD_DataMC_Validation.cc

libsEPDValidation_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lcentrality_io \
  -lfun4all \
  -lffamodules \
  -lglobalvertex_io \
  -lcalotrigger_io \
  -lcalotrigger \
  -leventplaneinfo_io \
  -lepd_io \
  -ljetbase_io \
  -ljetbackground_io \
  -lphparameter_io

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libsEPDValidation.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
