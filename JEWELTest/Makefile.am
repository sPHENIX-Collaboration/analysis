AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libJEWELTest_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libJEWELTest.la

include_HEADERS = \
  JEWELTest.h \
  JEWELTestLinkDef.h
libJEWELTest_la_SOURCES = \
  JEWELTest.C \
  JEWELTest_Dict.C
libJEWELTest_la_LIBADD = \
  -lg4dst \
  -lg4eval \
  -lcemc \
  -lphool

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libJEWELTest.la

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
