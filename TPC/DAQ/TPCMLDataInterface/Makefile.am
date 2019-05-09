AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   libg4eval.la

AM_CXXFLAGS = -Werror

libg4eval_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libg4eval_la_LIBADD = \
  -lg4eval \
  -lg4dst \
  -lhdf5_hl_cpp

pkginclude_HEADERS = \
  TPCMLDataInterface.h

if ! MAKEROOT6
  ROOT5DICTS = \
  TPCMLDataInterface_Dict.cc 
endif

libg4eval_la_SOURCES = \
  $(ROOT5DICTS) \
  TPCMLDataInterface.cc

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = \
  testexternals.cc

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libg4eval.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
