AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(ROOTSYS)/include

lib_LTLIBRARIES = \
   librichana.la

AM_CXXFLAGS = -Wall -Werror -msse2

librichana_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

librichana_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4eval \
  -lphhepmc

pkginclude_HEADERS = \
  RICHEvaluator.h \
  RICHParticleID.h \
  dualrich_analyzer.h

librichana_la_SOURCES = \
  RICHEvaluator.C \
  RICHEvaluator_Dict.C \
  RICHParticleID.C \
  RICHParticleID_Dict.C\
  dualrich_analyzer.cc

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^


################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = \
  testexternals.C

testexternals_LDADD = \
  librichana.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
