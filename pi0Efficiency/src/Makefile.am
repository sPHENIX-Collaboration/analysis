AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -lHepMC \
  -lCLHEP \
  -lg4vertex_io \
  -lcalo_io \
  -lg4dst \
  -lg4eval

pkginclude_HEADERS = \
  pi0Efficiency.h

lib_LTLIBRARIES = \
  libpi0Efficiency.la

libpi0Efficiency_la_SOURCES = \
  pi0Efficiency.cc

libpi0Efficiency_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lfun4all \
  -lHepMC

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libpi0Efficiency.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
