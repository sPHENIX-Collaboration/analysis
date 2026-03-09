AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

lib_LTLIBRARIES = \
  libJetBkgdSub.la

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `fastjet-config --libs`

libJetBkgdSub_la_LIBADD = \
  -lfun4all \
  -lg4dst \
  -lg4eval \
  -lphool \
  -lSubsysReco \
  -lg4jets


libJetBkgdSub_la_SOURCES = \
  JetBkgdSub.cc

pkginclude_HEADERS = \
  JetBkgdSub.h

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libJetBkgdSub.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
