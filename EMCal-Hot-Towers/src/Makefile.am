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
  CaloHotTower.h \
  CaloHotTowerSim.h

lib_LTLIBRARIES = \
  libCaloHotTower.la

libCaloHotTower_la_SOURCES = \
  CaloHotTower.cc \
  CaloHotTowerSim.cc

libCaloHotTower_la_LIBADD = \
  -lcalo_io \
  -lglobalvertex_io \
	-lmbd_io \
  -lphool \
	-lzdcinfo_io \
	-lffarawobjects \
	-lffamodules \
  -lcdbobjects \
  -lcalotrigger \
  -lSubsysReco

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libCaloHotTower.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
