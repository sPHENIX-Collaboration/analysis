##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

# list of shared libraries to produce
lib_LTLIBRARIES = \
  libelectronid.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

pkginclude_HEADERS = \
  ElectronID.h

ROOTDICTS = \
  ElectronID_Dict.cc 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  ElectronID_Dict_rdict.pcm

# sources for io library
libelectronid_la_SOURCES = \
  $(ROOTDICTS) \
  ElectronID.cc

libelectronid_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -ltrackbase_historic_io 

##added on Apr. 19, 2021 by Weihu
libelectronid_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(ROOTSYS)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lg4dst \
  -lg4eval \
  -lTMVA \
  -ltrackpid 

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

################################################
# linking tests

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_electronid

testexternals_electronid_SOURCES = testexternals.cc
testexternals_electronid_LDADD = libelectronid.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
