AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  HerwigProductionQAModule.h

lib_LTLIBRARIES = \
  libHerwigProductionQAModule.la

libHerwigProductionQAModule_la_SOURCES = \
  HerwigProductionQAModule.cc

libHerwigProductionQAModule_la_LIBADD = \
  -lphool \
  -lSubsysReco\
  -lfun4all \
  -lg4dst \
  -lphhepmc \
  -lg4jets \
  -lfastjet 

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libHerwigProductionQAModule.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
