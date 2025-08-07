AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

pkginclude_HEADERS = \
  triggercountmodule.h

ROOTDICTS = 

pcmdir = $(libdir)
nobase_dist_pcm_DATA = 

lib_LTLIBRARIES = \
  libtriggercountmodule_io.la \
  libtriggercountmodule.la

libtriggercountmodule_io_la_SOURCES = \
  $(ROOTDICTS) 

libtriggercountmodule_la_SOURCES = \
  triggercountmodule.cc

libtriggercountmodule_io_la_LIBADD = \
  -lphool -lSubsysReco -lmbd_io -ljetbase

libtriggercountmodule_la_LIBADD = \
  libtriggercountmodule_io.la \
  -lcalo_io \
  -lSubsysReco -lphg4hit -lffaobjects -lmbd_io \
  -ljetbase -lglobalvertex -lcentrality -lcalotrigger 
#-lg4jets_io
%_Dict.cc: %.h %LinkDef.h
	rootcint -f $@ @CINTDEFS@ $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

#just to get the dependency
%_Dict_rdict.pcm: %_Dict.cc ;

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals_io \
  testexternals

testexternals_io_SOURCES = testexternals.cc
testexternals_io_LDADD   = libtriggercountmodule_io.la

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libtriggercountmodule.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
