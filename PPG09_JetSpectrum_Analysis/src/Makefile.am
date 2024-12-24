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
  Ana_PPG09_Mod.h

lib_LTLIBRARIES = \
  libAna_PPG09_Mod.la

libAna_PPG09_Mod_la_SOURCES = \
  Ana_PPG09_Mod.cc

libAna_PPG09_Mod_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lcalotrigger \
  -ljetbase

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libAna_PPG09_Mod.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
