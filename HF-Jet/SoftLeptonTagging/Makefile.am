AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   libslt.la

AM_CXXFLAGS = -Wall -Werror -msse2

libslt_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libslt_la_LIBADD = \
  -lfun4all \
  -lphg4hit \
  -lg4detectors_io \
  -lg4hough_io \
  -lg4hough \
  -lcalo_io -lcalo_util \
  -lg4jets_io \
  -lg4eval

pkginclude_HEADERS = 

#pkginclude_HEADERS = $(include_HEADERS)

libslt_la_SOURCES = \
  SoftLeptonTaggingTruth.C \
  SoftLeptonTaggingTruth_Dict.C 

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
  libslt.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* testexternals.C
