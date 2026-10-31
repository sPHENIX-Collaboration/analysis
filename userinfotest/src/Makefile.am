AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include


AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -L$(ROOTSYS)/lib

lib_LTLIBRARIES = \
  libuserinfotest.la

pkginclude_HEADERS = \
  userinfotest.h

ROOTDICTS = 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = 

libuserinfotest_la_SOURCES = \
  userinfotest.cc

libuserinfotest_la_LIBADD = \
  -ltreeuserinfo \
  -lSubsysReco

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_userinfotest

testexternals_userinfotest_SOURCES = testexternals.cc
testexternals_userinfotest_LDADD   = libuserinfotest.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
