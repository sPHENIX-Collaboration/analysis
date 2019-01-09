AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libtruthconversion_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
	-L$(HOME)/install/lib \
 `root-config --libs`

lib_LTLIBRARIES = \
	libtruthconversion.la
  
include_HEADERS = \
	ConversionClusterTreeMaker.h \
	ConversionClusterTreeMakerLinkDef.h \
	TruthConversionEval.h \
	TruthConversionEvalLinkDef.h \
	Conversion.h \
	ConversionLinkDef.h 

libtruthconversion_la_SOURCES = \
	ConversionClusterTreeMaker.C \
	ConversionClusterTreeMaker_Dict.C \
	TruthConversionEval.C \
	TruthConversionEval_Dict.C \
	Conversion.cc \
	Conversion_Dict.C

libtruthconversion_la_LIBADD = \
  -lg4dst \
  -lg4eval \
  -lcalotrigger \
  -lphool

BUILT_SOURCES = \
	testexternals.C

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

##############################################
# please add new classes in alphabetical order
# Rule for generating CINT dictionaries from class headers.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(CINTFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict* testexternals.C
