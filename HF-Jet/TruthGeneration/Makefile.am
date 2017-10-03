AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   libHFJetTruthGeneration.la

AM_CXXFLAGS = -Wall -Werror -msse2

libHFJetTruthGeneration_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libHFJetTruthGeneration_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4eval \
  -lphhepmc

pkginclude_HEADERS = \
	HFJetDefs.h

#pkginclude_HEADERS = $(include_HEADERS)

libHFJetTruthGeneration_la_SOURCES = \
  HFJetTruthTrigger.C \
  HFJetTruthTrigger_Dict.C 

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
  libHFJetTruthGeneration.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
