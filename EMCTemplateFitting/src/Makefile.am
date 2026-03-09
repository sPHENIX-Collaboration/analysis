##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
  libPrototype4_io.la \
  libPrototype4.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

pkginclude_HEADERS = \
  CaloCalibration.h \
  CaloTemplateFit.h \
  CaloUnpackPRDF.h \
  EventInfoSummary.h \
  GenericUnpackPRDF.h \
  PROTOTYPE4_FEM.h \
  Prototype4DSTReader.h \
  RawTower_Prototype4.h \
  RawTower_Temperature.h \
  RunInfoUnpackPRDF.h \
  TempInfoUnpackPRDF.h

ROOTDICTS = \
  RawTower_Prototype4_Dict.cc \
  RawTower_Temperature_Dict.cc

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  RawTower_Prototype4_Dict_rdict.pcm \
  RawTower_Temperature_Dict_rdict.pcm

libPrototype4_io_la_SOURCES = \
  $(ROOTDICTS) \
  PROTOTYPE4_FEM.cc \
  RawTower_Prototype4.cc \
  RawTower_Temperature.cc

libPrototype4_la_SOURCES = \
  CaloCalibration.cc \
  CaloTemplateFit.cc \
  CaloUnpackPRDF.cc \
  EventInfoSummary.cc \
  GenericUnpackPRDF.cc \
  Prototype4DSTReader.cc \
  RunInfoUnpackPRDF.cc \
  TempInfoUnpackPRDF.cc

libPrototype4_io_la_LIBADD = \
  -lcalo_io \
  -lphool

libPrototype4_la_LIBADD = \
  libPrototype4_io.la \
  -lSubsysReco \
  -lfun4all \
  -lphparameter

# Rule for generating CINT dictionaries from class headers.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;


# link check
BUILT_SOURCES = \
  testexternals.cc

noinst_PROGRAMS = \
  testexternals_io \
  testexternals

testexternals_io_SOURCES = \
  testexternals.cc

testexternals_io_LDADD = \
  libPrototype4_io.la

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libPrototype4.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
