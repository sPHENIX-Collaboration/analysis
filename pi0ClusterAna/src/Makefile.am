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
  pi0ClusterAna.h

lib_LTLIBRARIES = \
  libpi0ClusterAna.la

libpi0ClusterAna_la_SOURCES = \
  pi0ClusterAna.cc

libpi0ClusterAna_la_LIBADD = \
  -lphool \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libpi0ClusterAna.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
