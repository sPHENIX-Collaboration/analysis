AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libPhotons_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libPhotons.la
noinst_HEADERS = \
  PhotonsLinkDef.h

include_HEADERS = \
  Photons.h \
  Forward_pi0s.h

libPhotons_la_SOURCES = \
  Photons.C \
  Forward_pi0s.C \
  Photons_Dict.C

libPhotons_la_LIBADD = \
  -lg4dst \
  -lg4eval \
  -lcalo_util \
  -lphool

BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libPhotons.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

##############################################                                                                             
# please add new classes in alphabetical order                                                                             
# Rule for generating CINT dictionaries from class headers.                                                                
Photons_Dict.C: Photons.h Forward_pi0s.h PhotonsLinkDef.h
	rootcint -f $@ -c $(CINTFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict* testexternals.C

testexternals_SOURCES = testexternals.C