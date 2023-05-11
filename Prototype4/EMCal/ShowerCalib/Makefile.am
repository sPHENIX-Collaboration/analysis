AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(OFFLINE_MAIN)/include \
  -I$(G4_MAIN)/include \
  -I$(G4_MAIN)/include/Geant4 \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   libProto4ShowCalib.la

AM_CXXFLAGS = -Wall -Werror -msse2

libProto4ShowCalib_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libProto4ShowCalib_la_LIBADD = \
  -lfun4all \
  -lPrototype4 \
  -lg4dst

pkginclude_HEADERS = 

#pkginclude_HEADERS = $(include_HEADERS)

libProto4ShowCalib_la_SOURCES = \
  Proto4ShowerCalib.C \
  Proto4ShowerCalib_Dict.C 

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
  libProto4ShowCalib.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
