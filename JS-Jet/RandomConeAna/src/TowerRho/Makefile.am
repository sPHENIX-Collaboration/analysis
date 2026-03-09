AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include  \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   libtowerrho_io.la \
   libtowerrho.la 

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libtowerrho_io_la_LIBADD = \
  -lphool

libtowerrho_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs`

libtowerrho_la_LIBADD = \
  libtowerrho_io.la \
  -ljetbase \
  -lfun4all \
  -lSubsysReco

pkginclude_HEADERS = \
  DetermineTowerRho.h \
  TowerRho.h \
  TowerRhov1.h 

ROOTDICTS = \
  TowerRho_Dict.cc \
  TowerRhov1_Dict.cc

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  TowerRho_Dict_rdict.pcm \
  TowerRhov1_Dict_rdict.pcm

libtowerrho_io_la_SOURCES = \
  $(ROOTDICTS) \
  TowerRhov1.cc 

libtowerrho_la_SOURCES = \
  DetermineTowerRho.cc 

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

################################################
# linking tests
BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_towerrho_io \
  testexternals_towerrho


testexternals_towerrho_io_SOURCES = testexternals.cc
testexternals_towerrho_io_LDADD = libtowerrho_io.la

testexternals_towerrho_SOURCES = testexternals.cc
testexternals_towerrho_LDADD = libtowerrho.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
  
  