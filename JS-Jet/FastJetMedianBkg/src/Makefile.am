AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem`root-config --incdir`

lib_LTLIBRARIES = \
   libfastjetmedianbkg.la \
   libjetplusbackground.la
#   libtracksandcalojets.la

libfastjetmedianbkg_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs`

libfastjetmedianbkg_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4eval

libfastjetmedianbkg_la_SOURCES = \
  CaloJetRhoEst.cc \
    MemTimeProgression.cc

pkginclude_HEADERS = \
  CaloJetRhoEst.h \
  JetPlusBackground.h \
    MemTimeProgression.h

#  TracksAndCaloJets.h

libjetplusbackground_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs`

libjetplusbackground_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4eval

libjetplusbackground_la_SOURCES = \
  JetPlusBackground.cc \
    MemTimeProgression.cc
 
#libtracksandcalojets_la_LDFLAGS = \
#  -L$(libdir) \
#  -L$(OFFLINE_MAIN)/lib \
#  `fastjet-config --libs`
#
#libtracksandcalojets_la_LIBADD = \
#  -lfun4all \
#  -lg4dst \
#  -lg4eval
#
#libtracksandcalojets_la_SOURCES = \
#  TracksAndCaloJets.cc \
#    MemTimeProgression.cc
    
################################################
# linking tests

noinst_PROGRAMS = testexternals

BUILT_SOURCES = \
  testexternals.cc

testexternals_SOURCES = \
  testexternals.cc

testexternals_LDADD = \
  libfastjetmedianbkg.la \
  libjetplusbackground.la

#  libjetplusbackground.la
#  libtracksandcalojets.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
