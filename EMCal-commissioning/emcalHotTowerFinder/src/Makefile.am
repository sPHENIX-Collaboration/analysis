AUTOMAKE_OPTIONS = foreign

CINTINCLUDES = -pthread -I$(includedir) -I$(OFFLINE_MAIN)/lib -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include -I$(ROOTSYS)/lib/include -I$(OPT_SPHENIX)/include

INCLUDES = $(CINTINCLUDES)

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -lcalo_io \
  -lCLHEP \
  -lglobalvertex \
  -lglobalvertex_io \
  -lcdbobjects
pcmdir = $(libdir)

pkginclude_HEADERS = \
  towerid.h

lib_LTLIBRARIES = \
  libtowerid.la

libtowerid_la_SOURCES = \
  towerid.cc

libtowerid_la_LIBADD = \
  -lphool \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libtowerid.la

TOWERIDOBJS=towerid.o derp.o

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@
#rootcint -f $@ -c $(DEFAULT_INCLUDES) $(CINTINCLUDES) $^ towerid.cc -c 

clean-local:
	rm -f *Dict*  $(BUILT_SOURCES) *.pcm
