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
  WaveForm.h \
  WaveFormNtuple.h \
  WaveFormPacket.h

lib_LTLIBRARIES = \
  libWaveForm.la

libWaveForm_la_SOURCES = \
  WaveForm.cc \
  WaveFormNtuple.cc \
  WaveFormPacket.cc

libWaveForm_la_LIBADD = \
  -lffarawobjects \
  -lcalo_io \
  -lphool \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libWaveForm.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
