AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(includedir) -I$(OFFLINE_MAIN)/include -I$(ROOTSYS)/include 


lib_LTLIBRARIES = libhcalUtil.la

AM_LDFLAGS = -L$(libdir) -L$(OFFLINE_MAIN)/lib


libhcalUtil_la_LIBADD = \
	  -L$(ONLINE_MAIN)/lib -lEvent  @ROOTLIBS@  -lSpectrum

noinst_HEADERS = \
	hcalUtilLinkDef.h

include_HEADERS = \
        hcalUtil.h 


libhcalUtil_la_SOURCES = \
        hcalUtil.C \
	hcalUtil_Dict.cxx 


hcalUtil_Dict.cxx : hcalUtil.h hcalUtilLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^



INCLUDES = -I$(includedir) -I$(ROOTSYS)/include -I$(OFFLINE_MAIN)/include -I$(ONLINE_MAIN)/include -I$(MYINSTALL)/include


bin_PROGRAMS = evDisplay sdisplay shist

evDisplay_SOURCES = \
  evDisplay.C

evDisplay_LDADD = -L$(ONLINE_MAIN)/lib -lEvent @ROOTLIBS@ @ROOTGLIBS@

sdisplay_SOURCES = \
  sdisplay.cc

sdisplay_LDADD = -L$(ONLINE_MAIN)/lib -lEvent @ROOTLIBS@ @ROOTGLIBS@

shist_SOURCES = \
  shist.cc

shist_LDADD = -L$(ONLINE_MAIN)/lib -lEvent @ROOTLIBS@ @ROOTGLIBS@

clean-local:
	rm -f $(BUILT_SOURCES) *Dict*

