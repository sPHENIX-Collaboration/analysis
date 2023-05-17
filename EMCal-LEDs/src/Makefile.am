##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

# List of shared libraries to produce
lib_LTLIBRARIES = \
  libledtowerbuilder.la 

#
libledtowerbuilder_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib64 \
  -L$(OFFLINE_MAIN)/lib

libledtowerbuilder_la_LIBADD = \
  -lcalo_reco

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

pkginclude_HEADERS = \
  LEDTowerBuilder.h 

libledtowerbuilder_la_SOURCES = \
  LEDTowerBuilder.cc

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals_ledtowerbuilder

BUILT_SOURCES  = testexternals.cc

testexternals_ledtowerbuilder_SOURCES = testexternals.cc
testexternals_ledtowerbuilder_LDADD = libledtowerbuilder.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

##############################################
# please add new classes in alphabetical order

clean-local:
	rm -f $(BUILT_SOURCES)
