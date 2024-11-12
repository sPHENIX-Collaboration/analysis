AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

pkginclude_HEADERS = \
  CaloEmulatorTreeMaker.h \
  CaloEmulatorTreeMakerLinkDef.h 

ROOTDICTS = 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = 

lib_LTLIBRARIES = \
  libemulatortreemaker.la

libemulatortreemaker_la_SOURCES = \
  CaloEmulatorTreeMaker.cc

libemulatortreemaker_la_LIBADD = \
  -lcalotrigger_io \
  -lcalotrigger \
  -lglobalvertex \
  -lmbd_io \
  -lSubsysReco


%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_emulatortreemaker

testexternals_emulatortreemaker_SOURCES = testexternals.cc
testexternals_emulatortreemaker_LDADD   = libemulatortreemaker.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
