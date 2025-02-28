AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

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
  -lqautils \
  libtrash_io.la

pkginclude_HEADERS = \
  FindOutlier.h \
  FindDijets.h \
  TriggerPerformance.h \
  RandomConeAna.h \
  JetCalibration.h \
  DijetEventDisplay.h \
  DijetTreeMaker.h \
  CopyCutVertex.h \
  EliminateBackground.h \
  GL1TriggerSelect.h \
  CaloEmulatorTreeMaker.h \
  MbdEmulatorTreeMaker.h \
  CaloEmulatorTreeMakerLinkDef.h \
  TrashInfo.h \
  TrashInfov1.h

ROOTDICTS = 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = 

lib_LTLIBRARIES = \
  libemulatortreemaker.la

libemulatortreemaker_la_SOURCES = \
<<<<<<< HEAD
  CaloEmulatorTreeMaker.cc \
  EliminateBackground.cc \
  DijetEventDisplay.cc \
  RandomConeAna.cc \
  DijetTreeMaker.cc \
  JetCalibration.cc \
  TriggerPerformance.cc \
  FindOutlier.cc \
  FindDijets.cc \
  CopyCutVertex.cc \
  GL1TriggerSelect.cc \
  MbdEmulatorTreeMaker.cc
=======
  CaloEmulatorTreeMaker.cc

libemulatortreemaker_la_LIBADD = \
  -lcalotrigger_io \
  -lcalotrigger \
  -lglobalvertex \
  -lmbd_io \
  -lSubsysReco

>>>>>>> 949e24c95d8f07e039a961d9a32c4a0dbc41dc82

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
