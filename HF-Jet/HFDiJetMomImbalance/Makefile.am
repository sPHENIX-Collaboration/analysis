AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(MY_INSTALL)/include \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libBDiJetModule_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libBDiJetModule.la

include_HEADERS = \
  BDiJetModule.h \
  BDiJetModuleLinkDef.h

libBDiJetModule_la_SOURCES = \
  BDiJetModule.C \
  BDiJetModule_Dict.C

libBDiJetModule_la_LIBADD = \
  -lg4dst \
  -lg4eval \
  -lcemc \
  -lphool

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libBDiJetModule.la

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
