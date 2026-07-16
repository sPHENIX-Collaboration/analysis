AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -isystem$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include \
  -isystem$(OPT_SPHENIX)/include  

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs` \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  triggerSelector.h \
  trackEEC.h

lib_LTLIBRARIES = \
  libTrackEEC.la 

libTrackEEC_la_SOURCES = \
  triggerSelector.cc \
  trackEEC.cc

libTrackEEC_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lphool \
  -lSubsysReco \
  -lfun4all \
  -lcalotrigger_io \
  -lcalotrigger


BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libTrackEEC.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
