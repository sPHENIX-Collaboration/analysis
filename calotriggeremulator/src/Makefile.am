AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -isystem`root-config --incdir`

lib_LTLIBRARIES = \
  libtrash_io.la \
  libemulatortreemaker.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib


libtrash_io_la_LIBADD = \
  -lphool

libemulatortreemaker_la_LIBADD = \
  -lcalotrigger_io \
  -lcalotrigger \
  -lglobalvertex \
  -lmbd_io \
  -lSubsysReco \
  -ljetbackground\
  -lg4jets \
  -ljetbase \
  libtrash_io.la

pkginclude_HEADERS = \
  FindOutlier.h \
  FindDijets.h \
  CopyCutVertex.h \
  EliminateBackground.h \
  GL1TriggerSelect.h \
  CaloEmulatorTreeMaker.h \
  CaloEmulatorTreeMakerLinkDef.h \
  TrashInfo.h \
  TrashInfov1.h

ROOTDICTS = \
  TrashInfo_Dict.cc \
  TrashInfov1_Dict.cc

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  TrashInfo_Dict_rdict.pcm \
  TrashInfov1_Dict_rdict.pcm

libtrash_io_la_SOURCES = \
  $(ROOTDICTS) \
  TrashInfov1.cc

libemulatortreemaker_la_SOURCES = \
  CaloEmulatorTreeMaker.cc \
  EliminateBackground.cc \
  FindOutlier.cc \
  FindDijets.cc \
  CopyCutVertex.cc \
  GL1TriggerSelect.cc

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_emulatortreemaker \
  testexternals_trash_io


testexternals_trash_io_SOURCES = testexternals.cc
testexternals_trash_io_LDADD   = libtrash_io.la

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
