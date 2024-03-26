AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

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
  MBDStudy.h

ROOTDICTS = \
  MBDStudy_Dict.C

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  MBDStudy_Dict_rdict.pcm

libMBDStudy_la_SOURCES = \
  $(ROOTDICTS) \
  MBDStudy.cc

libMBDStudy_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lg4detectors \
  -lmbd_io \
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

