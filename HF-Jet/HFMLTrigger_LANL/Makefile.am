##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
   libhfmltrigger.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(ROOTSYS)/include \
  -I/opt/sphenix/core/rapidjson/include

AM_LDFLAGS = \
  -L$(libdir) \
	-L$(ROOTSYS)/lib \
	-L$(OFFLINE_MAIN)/lib

pkginclude_HEADERS = \
  HFMLTriggerOccupancy.h \
  HFMLTriggerInterface.h \
  HFMLTriggerHepMCTrigger.h


ROOTDICTS = \
  HFMLTriggerHepMCTrigger_Dict.C \
  HFMLTriggerInterface_Dict.C \
  HFMLTriggerOccupancy_Dict.C


pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  HFMLTriggerHepMCTrigger_Dict_rdict.pcm \
  HFMLTriggerInterface_Dict_rdict.pcm \
  HFMLTriggerOccupancy_Dict_rdict.pcm


libhfmltrigger_la_SOURCES = \
  $(ROOTDICTS) \
  HFMLTriggerInterface.C \
  HFMLTriggerHepMCTrigger.C \
  HFMLTriggerOccupancy.C

libhfmltrigger_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4mvtx \
  -lg4eval \
  -lphhepmc

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.C ;



################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = testexternals.C

testexternals_SOURCES = \
	testexternals.C

testexternals_LDADD = \
  libhfmltrigger.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
