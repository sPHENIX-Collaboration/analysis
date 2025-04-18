AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(OFFLINE_MAIN)/geant4/include \
  -I`root-config --incdir`

libEpFinder_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libEpFinder.la

libEpFinder_la_SOURCES = \
  EpFinderEval.cc \
  EpFinderEvalDict.C \
  EpFinder.C \
  EpInfo.C 

libEpFinder_la_LIBADD = \
  -lg4detectors \
  -lphg4hit \
  -lg4eval \
  -lphool 

pkginclude_HEADERS = \
  EpFinderEval.h \
  EpFinder.h \
  EpInfo.h 	

pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  EpFinderEvalDict_rdict.pcm

install-exec-hook:
	@[ $$OSTYPE = darwin ] && ln -sf $(DESTDIR)$(libdir)/libEpFinder.dylib $(DESTDIR)$(libdir)/libEpFinder.so || true

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libEpFinder.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating CINT dictionaries from class headers.
%Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(CINTDEFS) -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%Dict_rdict.pcm: %Dict.cc ;

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm

testexternals_SOURCES = testexternals.C
