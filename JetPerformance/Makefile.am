AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libJetPerformance_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libJetPerformance.la

include_HEADERS = \
  JetPerformance.h \
  JetPerformanceLinkDef.h
libJetPerformance_la_SOURCES = \
  JetPerformance.C \
  JetPerformance_Dict.C
libJetPerformance_la_LIBADD = \
  -lg4dst \
  -lg4eval \
  -lcemc \
  -lphool

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libJetPerformance.la

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