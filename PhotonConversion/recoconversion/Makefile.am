AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

AM_CXXFLAGS = -DRAVE -DRaveDLLExport=

librecoconversion_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  librecoconversion.la

include_HEADERS = \
  ConvertedPhotonReconstructor.h \
  ConvertedPhotonReconstructorLinkDef.h \
  RecoConversionEval.h \
  RecoConversionEvalLinkDef.h \
	RaveVertexingAux.h \
	RaveVertexingAuxLinkDef.h

librecoconversion_la_SOURCES = \
  ConvertedPhotonReconstructor.cc \
  ConvertedPhotonReconstructor_Dict.C \
  RecoConversionEval.cc \
  RecoConversionEval_Dict.C \
	RaveVertexingAux.C \
	RaveVertexingAux_Dict.C

librecoconversion_la_LIBADD = \
	-lphool\
	-lgenfit2 \
	-lgenfit2exp \
	-lPHGenFit \
	-lphgeom \
	-lg4eval
BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  librecoconversion.la

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

testexternals_SOURCES = testexternals.C
