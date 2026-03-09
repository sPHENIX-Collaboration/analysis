AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  ThirdJetSpectra.h

lib_LTLIBRARIES = \
  libThirdJetSpectra.la

libThirdJetSpectra_la_SOURCES = \
  ThirdJetSpectra.cc

libThirdJetSpectra_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lfun4all \
  -lphhepmc \
  -lfastjet \
  -lPHPythia8 \
  -lg4jets 

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libThirdJetSpectra.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
