AUTOMAKE_OPTIONS = foreign

CINTINCLUDES = -pthread -I$(includedir) -I$(OFFLINE_MAIN)/lib -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include -I$(ROOTSYS)/lib/include -I$(OPT_SPHENIX)/include

INCLUDES = $(CINTINCLUDES)

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64 \
  -lcalo_io \
  -lCLHEP 
#  -lglobalvertex \
#  -lglobalvertex_io

#ROOTDICTS = caloTreeGenCombine_Dict.cc  caloTowerExtract_Dict.cc

#pcmdir = $(libdir)

#nobase_dist_pcm_DATA = caloTreeGenCombine_Dict_rdict.pcm  caloTowerExtract_Dict_rdict.pcm caloTowerCombineExtract_Dict_rdict.pcm

pkginclude_HEADERS = \
  caloTowerEmbed.h

lib_LTLIBRARIES = \
  libCalorimeterEmbedding.la

libCalorimeterEmbedding_la_SOURCES = \
  caloTowerEmbed.cc \
  $(ROOTDICTS)

libCalorimeterEmbedding_la_LIBADD = \
  -lphool \
  -lSubsysReco \
  -lfun4all

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libCalorimeterEmbedding.la

CALORIMETEREMBEDDINGOBJS =  caloTowerEmbed.o derp.o

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@
#rootcint -f $@ -c $(DEFAULT_INCLUDES) $(CINTINCLUDES) $^ caloTreeGenCombine.cc -c 

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

clean-local:
	rm -f *Dict*  $(BUILT_SOURCES) *.pcm
