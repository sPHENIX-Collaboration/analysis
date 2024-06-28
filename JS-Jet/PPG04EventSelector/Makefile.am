AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include  \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
   libPPG04EventSelector.la 

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libPPG04EventSelector_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs`

libPPG04EventSelector_la_LIBADD = \
  -lphool \
  -lfun4all \
  -ljetbase \
  -lg4dst \
  -lg4eval \
  -lg4jets \
  -lSubsysReco

pkginclude_HEADERS = \
  PPG04EventSelector.h

libPPG04EventSelector_la_SOURCES = \
  PPG04EventSelector.cc


################################################
# linking tests
BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_PPG04EventSelector

testexternals_PPG04EventSelector_SOURCES = testexternals.cc
testexternals_PPG04EventSelector_LDADD = libPPG04EventSelector.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
  
  
