
AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

lib_LTLIBRARIES = libHFJetTruthGeneration.la 

pkginclude_HEADERS = \
	HFJetDefs.h \
	HFJetTruthTrigger.h

libHFJetTruthGeneration_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs` \
  `fastjet-config --libs`

libHFJetTruthGeneration_la_LIBADD =  \
  -lfun4all \
  -lg4dst \
  -lg4eval \
  -lphhepmc

if ! MAKEROOT6
ROOT5_DICTS = \
  HFJetTruthTrigger_Dict.cc 
endif

libHFJetTruthGeneration_la_SOURCES = \
  $(ROOT5_DICTS) \
  HFJetTruthTrigger.cc 

BUILT_SOURCES = \
  testexternals.cc

testexternals_SOURCES = \
  testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libHFJetTruthGeneration.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^


clean-local:
	rm -f *Dict* $(BUILT_SOURCES)
	