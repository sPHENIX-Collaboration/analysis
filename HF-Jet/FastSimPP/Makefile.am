AUTOMAKE_OPTIONS = foreign

AM_CXXFLAGS = -DG4INTY_USE_XT -DG4VIS_USE_OPENGL -DG4UI_USE_TCSH -DG4UI_USE_XM -DG4VIS_USE_OPENGLXM -DG4VIS_USE_RAYTRACERX -DG4VIS_USE_OPENGLX

# set in configure.in to check if gcc version >= 4.8
#if GCC_GE_48
#  AM_CXXFLAGS += -std=c++11
#endif

# List of shared libraries to produce
lib_LTLIBRARIES = \
  libhffastsim.la

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(XERCESCROOT)/include \
  -isystem $(OFFLINE_MAIN)/include \
  -isystem $(ROOTSYS)/include \
  -isystem ${G4_MAIN}/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(OPT_SPHENIX)/include

# rootcint barfs on isystem
RINCLUDES = \
  -I$(includedir) \
  -I$(XERCESCROOT)/include \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include \
  -I${G4_MAIN}/include \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(OPT_SPHENIX)/include


libhffastsim_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --evelibs` \
  `geant4-config --libs`

libhffastsim_la_LIBADD = \
  -lboost_filesystem \
  -lg4decayer \
  -lgsl \
  -lgslcblas \
  -lHepMC \
  -lphfield \
  -lphgeom \
  -lphg4gdml \
  -lphhepmc


# for root6 we need pcm and dictionaries but only for
# i/o classes. For root5 we need only dictionaries but
# those for i/o and classes available on the cmd line
# MAKEROOT6 is set in the configure.ac, true for root6
if MAKEROOT6
# this is a tweak to install files in $(libdir), automake refuses
# to install other files in libdir, this construct gets around this
pcmdir = $(libdir)
nobase_dist_pcm_DATA = \
  HFFastSim_Dict_rdict.pcm
else
  ROOT5TBDICTS = \
  HFFastSim_Dict.cc
endif

libhffastsim_la_SOURCES = \
  $(ROOT5TBDICTS) \
  HFFastSim_Dict.cc \
  HFFastSim.cc 

##############################################
# please add new classes in alphabetical order
pkginclude_HEADERS = \
  HFFastSim.h 

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals_hffastsim

BUILT_SOURCES = testexternals.cc
testexternals_hffastsim_SOURCES = testexternals.cc
testexternals_hffastsim_LDADD = libhffastsim.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ -c $(DEFAULT_INCLUDES) $(RINCLUDES) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

clean-local:
	rm -f *Dict* $(BUILT_SOURCES) *.pcm 
