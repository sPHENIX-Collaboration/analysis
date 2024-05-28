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
  ZdcSmdCount.h

lib_LTLIBRARIES = \
  libZdcSmdCount.la

libZdcSmdCount_la_SOURCES = \
  ZdcSmdCount.cc

libZdcSmdCount_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lg4dst \
  -lg4eval \
  -lffarawobjects \
  -lcalo_reco \
  -luspin

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libZdcSmdCount.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
