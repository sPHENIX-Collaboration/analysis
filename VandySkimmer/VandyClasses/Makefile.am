AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  EventInfo.h \
  Tower.h \
  JetInfo.h

lib_LTLIBRARIES = \
  libVandyClasses.la

# Include the dictionary in the main library sources
libVandyClasses_la_SOURCES = \
  Tower.cc \
  JetInfo.cc \
  VandyClasses_Dict.cc

libVandyClasses_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lphool

# Rule for the ROOT Dictionary
# Note: We explicitly list the headers so rootcling knows what to parse
VandyClasses_Dict.cc: EventInfo.h Tower.h JetInfo.h VandyClassesLinkDef.h
	rootcling -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

# Ensure the .pcm file is installed so ROOT can find the dictionary at runtime
pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  VandyClasses_Dict_rdict.pcm

BUILT_SOURCES = testexternals.cc VandyClasses_Dict.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libVandyClasses.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES) *.pcm VandyClasses_Dict.*