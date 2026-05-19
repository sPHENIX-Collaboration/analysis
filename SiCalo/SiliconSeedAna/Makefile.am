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
  SiliconCaloTrack.h \
  SiliconCaloTrack_v1.h \
  SiliconCaloTrackMap.h \
  SiliconCaloTrackMap_v1.h \
  SiliconCaloMatching.h \
  ChargeRecalc.h  \
  SiliconSeedsAna.h 

lib_LTLIBRARIES = \
  libsiliconseedsana.la

ROOTDICTS = \
  SiliconCaloTrack_Dict.cc \
  SiliconCaloTrack_v1_Dict.cc \
  SiliconCaloTrackMap_Dict.cc \
  SiliconCaloTrackMap_v1_Dict.cc

pcmdir = $(libdir)
# more elegant way to create pcm files (without listing them)
nobase_dist_pcm_DATA = $(ROOTDICTS:.cc=_rdict.pcm)


libsiliconseedsana_la_SOURCES = \
  $(ROOTDICTS) \
  SiliconCaloTrack_v1.cc \
  SiliconCaloTrackMap.cc \
  SiliconCaloTrackMap_v1.cc \
  SiliconCaloMatching.cc \
  ChargeRecalc.cc \
  SiliconSeedsAna.cc 



libsiliconseedsana_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lg4tpc \
  -lg4intt \
  -lg4mvtx \
  -lg4eval \
  -lmicromegas_io \
  -lmvtx_io \
  -lintt_io \
  -ltrack_io \
  -ltrackbase_historic_io \
  -ltrack_reco \
  -lqautils

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libsiliconseedsana.la

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;


testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
