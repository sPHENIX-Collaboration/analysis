AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem`root-config --incdir`

#  -I/sphenix/tg/tg01/commissioning/INTT/work/genki/repos/INTT/general_codes/hachiya/INTTVtx/install/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

lib_LTLIBRARIES = \
    libinttread.la

ROOTDICTS = \
  InttEvent_Dict.cc

pcmdir = $(libdir)

nobase_dist_pcm_DATA = \
  InttEvent_Dict_rdict.pcm

pkginclude_HEADERS = \
  InttRawData.h \
  InttDummyData.h \
  InttOfflineDummyData.h \
  InttAna.h \
  InttHotMap.h \
  InttEvent.h

libinttread_la_SOURCES = \
  $(ROOTDICTS) \
  InttRawData.cc  \
  InttDummyData.cc  \
  InttOfflineDummyData.cc  \
  InttAna.cc \
  InttHotMap.cc \
  InttEvent.cc 

libinttread_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lg4dst \
  -lg4eval \
  -lintt 
#  -lInttXYVertexFinder 

################################################
# linking tests

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libinttread.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

%_Dict.cc: %.h %LinkDef.h
	rootcling -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
