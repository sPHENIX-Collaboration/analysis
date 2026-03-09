AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4
#creating one lib (libMBDStudy) with 2 class or object on it 

lib_LTLIBRARIES = \
  libMBDStudy.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

include_HEADERS = \
  MbdLaser.h \
  MBDStudy.h

ROOTDICTS = \
  MbdLaser_Dict.C \
  MBDStudy_Dict.C

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  MbdLaser_Dict_rdict.pcm \
  MBDStudy_Dict_rdict.pcm

libMBDStudy_la_SOURCES = \
  $(ROOTDICTS) \
  MbdLaser.cc \
  MBDStudy.cc
#digital library we need 
libMBDStudy_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lg4detectors \
  -lmbd_io \
  -lglobalvertex_io \
  -lphg4hit

libMBDStudy_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

# Rule for generating CINT dictionaries from class headers.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^


#just to get the dependency
%_Dict_rdict.pcm: %_Dict.C ;


##########################################
# linking tests
BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
#creating one lib (libMBDStudy) with 2 class or object on it 
testexternals_LDADD = \
  libMBDStudy.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@


clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm

