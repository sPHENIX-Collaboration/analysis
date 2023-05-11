AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libSvtxEvaluatorHaiwang_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libSvtxEvaluatorHaiwang.la

libSvtxEvaluatorHaiwang_la_SOURCES = \
  SvtxEvaluatorHaiwang.cc \
  SvtxEvaluatorHaiwangDict.C

libSvtxEvaluatorHaiwang_la_LIBADD = \
  -lg4detectors \
  -lphg4hit \
  -lg4eval \
  -lphool

install-exec-hook:
	@[ $$OSTYPE = darwin ] && ln -sf $(DESTDIR)$(libdir)/libSvtxEvaluatorHaiwang.dylib $(DESTDIR)$(libdir)/libSvtxEvaluatorHaiwang.so || true

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libSvtxEvaluatorHaiwang.la

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
