AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(OFFLINE_MAIN)/include \
  -I$(OPT_SPHENIX)/include \
  -I$(OPT_SPHENIX)/hdf5/include \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   libtpcmldatainterface.la

AM_CXXFLAGS = -Werror

libtpcmldatainterface_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OPT_SPHENIX)/lib\
  -L$(OPT_SPHENIX)/hdf5/lib

libtpcmldatainterface_la_LIBADD = \
  -lg4dst \
  -lfun4all \
  -lg4eval \
  -lhdf5_cpp 

pkginclude_HEADERS = \
  TPCMLDataInterface.h

if ! MAKEROOT6
  ROOT5DICTS = \
  TPCMLDataInterface_Dict.cc 
endif

libtpcmldatainterface_la_SOURCES = \
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
  libtpcmldatainterface.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
