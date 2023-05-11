AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(includedir) -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include

lib_LTLIBRARIES = libFastTrackingEval.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libFastTrackingEval_la_LIBADD = \
  -lg4detectors \
  -lphg4hit \
  -lg4eval \
  -lphool

if ! MAKEROOT6
  ROOT5_DICTS = \
    FastTrackingEval_Dict.cc
endif

libFastTrackingEval_la_SOURCES = \
  $(ROOT5_DICTS) \
  FastTrackingEval.cc

pkginclude_HEADERS = \
  FastTrackingEval.h

BUILT_SOURCES = \
  testexternals.cc

testexternals_SOURCES = \
  testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libFastTrackingEval.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
