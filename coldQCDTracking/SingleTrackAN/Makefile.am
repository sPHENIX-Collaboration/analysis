AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libsingletrackan.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include \
  -isystem$(OPT_SPHENIX)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -L$(ROOTSYS)/lib \
  -L$(OPT_SPHENIX)/lib

pkginclude_HEADERS = \
  SingleTrackAN.h

libsingletrackan_la_SOURCES = \
  SingleTrackAN.cc

libsingletrackan_la_LDFLAGS = \
  -avoid-version

libsingletrackan_la_LIBADD = \
  -lphool \
  -lfun4all \
  -lffaobjects \
  -lffarawobjects \
  -luspin \
  -lSubsysReco \
  -ltrackbase_historic \
  -ltrackbase_historic_io \
  -lbcolumicount_io \
  -lbcolumicount

################################################
# linking test

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = testexternals

testexternals_SOURCES = testexternals.cc

testexternals_LDADD = \
  libsingletrackan.la \
  -lbcolumicount_io \
  -lbcolumicount \
  -ltrackbase_historic \
  -ltrackbase_historic_io

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

################################################

clean-local:
	rm -f $(BUILT_SOURCES)