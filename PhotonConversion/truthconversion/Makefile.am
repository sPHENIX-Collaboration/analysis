AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

libtruthconversion_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
	-L$(HOME)/install/lib \
 `root-config --libs`

lib_LTLIBRARIES = \
	libtruthconversion.la
  
pkginclude_HEADERS = \
	TruthConversionEval.h \
	TruthConversionEvalLinkDef.h \
	Conversion.h \
	ConversionLinkDef.h\
	SVReco.h\
	SVRecoLinkDef.h\
	VtxTest.h\
	VtxTestLinkDef.h\
	VtxRegressor.h\
	VtxRegressorLinkDef.h\
	RecoConversionEval.h\
	RecoConversionEvalLinkDef.h

libtruthconversion_la_SOURCES = \
	TruthConversionEval.C \
	Conversion.cc \
  SVReco.C\
	VtxRegressor.C\
	VtxTest.C\
	RecoConversionEval.cc

libtruthconversion_la_LIBADD = \
  -lphool\
  -lg4eval\
  -lg4dst\
  -lcalotrigger\
	-lcalo_io\
  -lfun4all \
	-lg4detectors \
	-lgenfit2 \
	-lgenfit2exp \
	-lPHGenFit \
	-lg4tpc \
	-lg4intt \
	-lTMVA\
	-lg4mvtx \
	-lcalo_util

BUILT_SOURCES = \
	testexternals.C \
	gcc_test.c

noinst_PROGRAMS = \
	testexternals

testexternals_LDADD = \
	libtruthconversion.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@
%_Dict.C: %.h %LinkDef.h
		rootcint -f $@ -c $(CINTFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $^
##############################################
# please add new classes in alphabetical order
# Rule for generating CINT dictionaries from class headers.

clean-local:
	rm -f *Dict* testexternals.C
