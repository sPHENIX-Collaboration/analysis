AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libtrksinjetqa.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

pkginclude_HEADERS = \
  TrksInJetQA.h \
  TrksInJetQAHist.h \
  TrksInJetQATypes.h \
  TrksInJetQAConfig.h \
  TrksInJetQABaseFiller.h \
  TrksInJetQAInJetFiller.h \
  TrksInJetQAInclusiveFiller.h \
  TrksInJetQABaseManager.h \
  TrksInJetQAHitManager.h \
  TrksInJetQAClustManager.h \
  TrksInJetQATrkManager.h \
  TrksInJetQAJetManager.h

if ! MAKEROOT6
  ROOT5_DICTS = \
    TrksInJetQA_Dict.cc
endif

libtrksinjetqa_la_SOURCES = \
  $(ROOT5_DICTS) \
  TrksInJetQA.cc \
  TrksInJetQABaseFiller.cc \
  TrksInJetQAInJetFiller.cc \
  TrksInJetQAInclusiveFiller.cc \
  TrksInJetQABaseManager.cc \
  TrksInJetQAHitManager.cc \
  TrksInJetQAClustManager.cc \
  TrksInJetQATrkManager.cc \
  TrksInJetQAJetManager.cc

libtrksinjetqa_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lphg4hit \
  -lg4dst \
  -lg4eval \
  -lqautils \
  `fastjet-config --libs`


################################################
# linking tests

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.C
testexternals_LDADD = libtrksinjetqa.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating table CINT dictionaries.
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm
