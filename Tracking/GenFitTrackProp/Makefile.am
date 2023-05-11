AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libGenFitTrackProp_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libGenFitTrackProp.la

libGenFitTrackProp_la_SOURCES = \
  GenFitTrackProp.cc \
  GenFitTrackPropDict.C

libGenFitTrackProp_la_LIBADD = \
  -lg4detectors \
  -lphg4hit \
  -lg4eval \
  -lPHGenFit \
  -lphool

install-exec-hook:
	@[ $$OSTYPE = darwin ] && ln -sf $(DESTDIR)$(libdir)/libGenFitTrackProp.dylib $(DESTDIR)$(libdir)/libGenFitTrackProp.so || true

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libGenFitTrackProp.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

# Rule for generating CINT dictionaries from class headers.
%Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(CINTFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict*

testexternals_SOURCES = testexternals.C
