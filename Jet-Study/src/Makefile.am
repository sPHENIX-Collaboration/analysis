AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  JetValidation.h \
  JetValidationv2.h \
  JetUtils.h \
  EventCheck.h \
  EventValidation.h

lib_LTLIBRARIES = \
  libJetValidation.la

libJetValidation_la_SOURCES = \
  JetValidation.cc \
  JetValidationv2.cc \
  JetUtils.cc \
  EventCheck.cc \
  EventValidation.cc

libJetValidation_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lfun4all \
  -lcalotrigger \
  -lg4dst

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libJetValidation.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
