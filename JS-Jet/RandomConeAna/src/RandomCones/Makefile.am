AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include  \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   librandomcones_io.la \
   librandomcones.la 

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

librandomcones_io_la_LIBADD = \
  -lphool

librandomcones_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs`

librandomcones_la_LIBADD = \
  librandomcones_io.la \
  -ljetbase \
  -lfun4all \
  -lSubsysReco

pkginclude_HEADERS = \
  RandomConeReco.h \
  RandomCone.h \
  RandomConev1.h 

ROOTDICTS = \
  RandomCone_Dict.cc \
  RandomConev1_Dict.cc

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  RandomCone_Dict_rdict.pcm \
  RandomConev1_Dict_rdict.pcm

librandomcones_io_la_SOURCES = \
  $(ROOTDICTS) \
  RandomConev1.cc 

librandomcones_la_SOURCES = \
  RandomConeReco.cc 

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

################################################
# linking tests
BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_randomcones_io \
  testexternals_randomcones


testexternals_randomcones_io_SOURCES = testexternals.cc
testexternals_randomcones_io_LDADD = librandomcones_io.la

testexternals_randomcones_SOURCES = testexternals.cc
testexternals_randomcones_LDADD = librandomcones.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
  
  