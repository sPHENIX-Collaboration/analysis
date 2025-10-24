AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -lcalo_io \
  -lcalotrigger_io \ 
  -lcentrality_io \
  -lCLHEP \
  -lglobalvertex \
  -lglobalvertex_io \
  -lffarawmodules \
  -lg4dst \
  -lg4eval

pkginclude_HEADERS = \
  StreakEventsIdentifier.h

lib_LTLIBRARIES = \
  libStreakEventsIdentifier.la

libStreakEventsIdentifier_la_SOURCES = \
  StreakEventsIdentifier.cc

libStreakEventsIdentifier_la_LIBADD = \
  -lphool \
  -lmbd_io \
  -lsph_onnx \
  -lSubsysReco


BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libStreakEventsIdentifier.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
