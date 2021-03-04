##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

# list of shared libraries to produce
lib_LTLIBRARIES = \
  libeventmix.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

pkginclude_HEADERS = \
  PairMaker.h \
  sPHElectron.h \
  sPHElectronv1.h \
  sPHElectronPair.h \
  sPHElectronPairv1.h \
  sPHElectronPairContainer.h \
  sPHElectronPairContainerv1.h 

ROOTDICTS = \
  PairMaker_Dict.cc \
  sPHElectron_Dict.cc \
  sPHElectronv1_Dict.cc \
  sPHElectronPair_Dict.cc \
  sPHElectronPairv1_Dict.cc \ 
  sPHElectronPairContainer_Dict.cc \
  sPHElectronPairContainerv1_Dict.cc 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  PairMaker_Dict_rdict.pcm \
  sPHElectron_Dict_rdict.pcm \
  sPHElectronv1_Dict_rdict.pcm \
  sPHElectronPair_Dict_rdict.pcm \
  sPHElectronPairv1_Dict_rdict.pcm \
  sPHElectronPairContainer_Dict_rdict.pcm \
  sPHElectronPairContainerv1_Dict_rdict.pcm 

# sources for io library
libeventmix_la_SOURCES = \
  $(ROOTDICTS) \
  PairMaker.cc \
  sPHElectronv1.cc \
  sPHElectronPairv1.cc \
  sPHElectronPairContainerv1.cc 

libeventmix_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lfun4all \
  -lg4dst \
  -lSubsysReco \
  -lphg4hit \
  -ltrackpid \
  -lphool 

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

################################################
# linking tests

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_eventmix

testexternals_eventmix_SOURCES = testexternals.cc
testexternals_eventmix_LDADD = libeventmix.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
