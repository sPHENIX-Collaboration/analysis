AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
	-I$(MY_INSTALL)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

#libBDiJetModule_la_LDFLAGS = \
#  -L$(libdir) \
#  -L$(OFFLINE_MAIN)/lib

lib_LTLIBRARIES = \
   libBDiJetModule.la

AM_CXXFLAGS = -Wall -Werror -msse2 -DRAVE -DRaveDllExport=

AM_LDFLAGS = \
	-L$(libdir) \
	-L$(MY_INSTALL)/lib \
	-L$(OFFLINE_MAIN)/lib

libBDiJetModule_la_SOURCES = \
  BDiJetModule.C \
  BDiJetModule_Dict.C
 

libBDiJetModule_la_LIBADD = \
  -lfun4all \
	-lg4detectors \
  -lg4dst \
  -lg4eval \
	-lg4hough_io \
  -lg4jets \
	-lgenfit2 \
	-lgenfit2exp \
	-lPHGenFit \
  -lphhepmc

#pkginclude_HEADERS = $(include_HEADERS)

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
  libBDiJetModule.la

testexternals_SOURCES = testexternals.C

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
