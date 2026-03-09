AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include  \
  -isystem`root-config --incdir` \
  -isystem$(OPT_SPHENIX)/include

lib_LTLIBRARIES = \
   libpileuprejection.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libpileuprejection_la_LIBADD = \
  -lmbd_io \
  -lffamodules \
  -lSubsysReco \
  -lphool 

pkginclude_HEADERS = \
  PileupRejector.h

libpileuprejection_la_SOURCES = \
  PileupRejector.cc

################################################
# linking tests
BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_pileuprejection

testexternals_pileuprejection_SOURCES = testexternals.cc
testexternals_pileuprejection_LDADD = libpileuprejection.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
