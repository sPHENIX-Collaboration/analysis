##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

# list of shared libraries to produce
lib_LTLIBRARIES = \
  libfilterevents.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -DHomogeneousField

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

pkginclude_HEADERS = \
  FilterEvents.h 

ROOTDICTS = \
  FilterEvents_Dict.cc 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  FilterEvents_Dict_rdict.pcm 

# sources for io library
libfilterevents_la_SOURCES = \
  $(ROOTDICTS) \
  FilterEvents.cc 

libfilterevents_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lfun4all \
  -lSubsysReco \
  -lphool \
  -lg4dst 

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

################################################
# linking tests

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_filterevents

testexternals_filterevents_SOURCES = testexternals.cc
testexternals_filterevents_LDADD = libfilterevents.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
