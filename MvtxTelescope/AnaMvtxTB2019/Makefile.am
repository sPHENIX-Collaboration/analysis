AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libanamvtxtestbeam2019.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include  \
  -I$(ROOTSYS)/include \
  -I$(OPT_SPHENIX)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(ROOTSYS)/lib \
  -L$(OFFLINE_MAIN)/lib

libanamvtxtestbeam2019_la_LIBADD = \
  -lCLHEP \
  -lphool \
  -lfun4all \
  -lphparameter \
	-ltrack_io \
	-ltrackbase_historic_io \
  -lmvtx \
  -lmvtxprototype2

if ! MAKEROOT6
ROOT5_DICTS = \
  AnaMvtxTestBeam2019_Dict.C
endif

pkginclude_HEADERS = \
  AnaMvtxTestBeam2019.h

libanamvtxtestbeam2019_la_SOURCES = \
	$(ROOT5_DICTS) \
  AnaMvtxTestBeam2019.cc

# Rule for generating table CINT dictionaries.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@  -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

################################################
# linking tests

BUILT_SOURCES = testexternals.C

noinst_PROGRAMS = \
  testexternals_tracker

testexternals_tracker_SOURCES = testexternals.C
testexternals_tracker_LDADD = libanamvtxtestbeam2019.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f *Dict* *.pcm
