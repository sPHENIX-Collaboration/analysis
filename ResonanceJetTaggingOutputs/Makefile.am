AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = \
    libresonancejettaggingoutputs.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
	`root-config --libs`

AM_CPPFLAGS = \
  -I$(includedir) \
	-isystem$(OFFLINE_MAIN)/include \
  -isystem`root-config --incdir` \
	-DHomogeneousField

pkginclude_HEADERS = \
	BuildResonanceJetTaggingTree.h

libresonancejettaggingoutputs_la_SOURCES = \
	BuildResonanceJetTaggingTree.cc

libresonancejettaggingoutputs_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lcalo_io \
  -lfun4all \
  -lg4detectors_io \
  -lg4jets \
  -lkfparticle_sphenix_io \
  -lparticleflow_io \
  -lphg4hit \
  -lg4eval \
  -lresonancejettagging \
  `fastjet-config --libs`


################################################
# linking tests

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libresonancejettaggingoutputs.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
