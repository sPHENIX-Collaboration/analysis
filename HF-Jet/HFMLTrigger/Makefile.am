AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir` \
  -I/opt/sphenix/core/rapidjson/include

lib_LTLIBRARIES = \
   libHFMLTrigger.la

AM_CXXFLAGS = -Wall -Werror -msse2

libHFMLTrigger_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L/opt/sphenix/core/rapidjson/lib

libHFMLTrigger_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4mvtx \
  -lg4eval \
  -lphhepmc

pkginclude_HEADERS = \
	HFMLTriggerInterface.h\
	HFMLTriggerHepMCTrigger.h
#pkginclude_HEADERS = $(include_HEADERS)

libHFMLTrigger_la_SOURCES = \
  HFMLTriggerHepMCTrigger.C \
  HFMLTriggerHepMCTrigger_Dict.C 
  
OldSOURCES = \
  HFMLTriggerOccupancy.C \
  HFMLTriggerOccupancy_Dict.C \
  HFMLTriggerInterface.C \
  HFMLTriggerInterface_Dict.C \
  HFMLTriggerHepMCTrigger.C \
  HFMLTriggerHepMCTrigger_Dict.C 

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

%_Dict.cpp: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = \
  testexternals.C

testexternals_LDADD = \
  libHFMLTrigger.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
