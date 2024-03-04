AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
  libRandomConeTree.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs`

libRandomConeTree_la_LIBADD = \
  -lfun4all \
  -lphool \
  -lSubsysReco \
  -lg4dst \
  -lg4eval \
  -lg4jets \
  -lrandomcones \
  -ltowerrho 
  
libRandomConeTree_la_SOURCES = \
  RandomConeTree.cc

pkginclude_HEADERS = \
  RandomConeTree.h

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libRandomConeTree.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
