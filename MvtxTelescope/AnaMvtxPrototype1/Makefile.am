AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
	-I$(MY_INSTALL)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

#libAnaMvtxPrototype1_la_LDFLAGS = \
#  -L$(libdir) \
#  -L$(OFFLINE_MAIN)/lib

lib_LTLIBRARIES = \
   libAnaMvtxPrototype1.la

AM_CXXFLAGS = -Wall -Werror -msse2 -DRAVE -DRaveDllExport=

AM_LDFLAGS = \
	-L$(libdir) \
	-L$(MY_INSTALL)/lib \
	-L$(OFFLINE_MAIN)/lib

libAnaMvtxPrototype1_la_SOURCES = \
  AnaMvtxPrototype1.C \
  AnaMvtxPrototype1_Dict.C
 

libAnaMvtxPrototype1_la_LIBADD = \
  -lfun4all \
  -ltrack_io \
  -ltrack_util \
  -lMvtxPrototype1

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
  libAnaMvtxPrototype1.la

testexternals_SOURCES = testexternals.C

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
