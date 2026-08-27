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
  sEPDWaveform.h \
  sEPDTowerAnalysis.h \
  sEPDTowerPairQA.h

lib_LTLIBRARIES = \
  libsEPDWaveform.la

libsEPDWaveform_la_SOURCES = \
  sEPDWaveform.cc \
  sEPDTowerAnalysis.cc \
  sEPDTowerPairQA.cc

libsEPDWaveform_la_LIBADD = \
  -lphool \
  -lcalo_io \
  -lcalo_reco \
  -lcdbobjects \
  -lffarawobjects \
  -lSubsysReco \
  -lfun4all

nodist_libsEPDWaveform_la_SOURCES = \
  sEPDWaveform_Dict.cc \
  sEPDTowerAnalysis_Dict.cc \
  sEPDTowerPairQA_Dict.cc

BUILT_SOURCES = \
  testexternals.cc \
  sEPDWaveform_Dict.cc \
  sEPDTowerAnalysis_Dict.cc \
  sEPDTowerPairQA_Dict.cc

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  sEPDWaveform_Dict_rdict.pcm \
  sEPDTowerAnalysis_Dict_rdict.pcm \
  sEPDTowerPairQA_Dict_rdict.pcm \
  libsEPDWaveform.rootmap

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libsEPDWaveform.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

sEPDWaveform_Dict.cc: sEPDWaveform.h sEPDWaveformLinkDef.h
	rootcling -f $@ -rml libsEPDWaveform.so -rmf libsEPDWaveform.rootmap -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

sEPDTowerAnalysis_Dict.cc: sEPDTowerAnalysis.h sEPDTowerAnalysisLinkDef.h
	rootcling -f $@ -rml libsEPDWaveform.so -rmf libsEPDWaveform.rootmap -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

sEPDTowerPairQA_Dict.cc: sEPDTowerPairQA.h sEPDTowerPairQALinkDef.h
	rootcling -f $@ -rml libsEPDWaveform.so -rmf libsEPDWaveform.rootmap -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f $(BUILT_SOURCES) *.pcm *.rootmap