AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libTreeMaker_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libTreeMaker.la

include_HEADERS = \
  TreeMaker.h \
  TreeMakerLinkDef.h

libTreeMaker_la_SOURCES = \
  TreeMaker.C \
  TreeMakerInitializations.C \
  TreeMakerGetJets.C \
  TreeMakerGetClusters.C \
  TreeMakerCreateNode.C \
  TreeMakerCopyAndMakeJets.C \
  TreeMakerCopyAndMakeClusters.C \
  TreeMakerUseFastJet.C \
  TreeMaker_Dict.C

libTreeMaker_la_LIBADD = \
  -lg4dst \
  -lg4eval \
  -lcalotrigger \
  -ljetbackground \
  -lphool

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libTreeMaker.la

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
