AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(includedir) -I$(ROOTSYS)/include -I$(OFFLINE_MAIN)/include -I$(ONLINE_MAIN)/include -I$(MYINSTALL)/include


lib_LTLIBRARIES = libhcal.la


include_HEADERS = \
        hcal.h \
	hcalLinkDef.h

libhcal_la_SOURCES = \
        hcal.C \
	hcal_Dict.cxx

libhcal_la_LIBADD = \
	  -L$(libdir) \
	  -L$(ONLINE_MAIN)/lib  -lNoRootEvent @ROOTLIBS@ @ROOTGLIBS@

hcal_Dict.cxx : hcal.h hcalLinkDef.h
	rootcint -f $@ -c $(DEFS) $(INCLUDES) $^

clean-local:
	rm -f $(BUILT_SOURCES) *Dict*

