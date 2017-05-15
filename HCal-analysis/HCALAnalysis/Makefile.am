AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include/eigen3 \
  -I$(OFFLINE_MAIN)/include \
  -I$(G4_MAIN)/include \
  -I$(G4_MAIN)/include/Geant4 \
  -I`root-config --incdir`

SUBDIRS = 

lib_LTLIBRARIES = \
  libHCALAnalysis.la 

noinst_HEADERS = \
  HCALAnalysisLinkDef.h 

include_HEADERS = \
  HCALAnalysis.h

libHCALAnalysis_la_SOURCES = \
  HCALAnalysis.C \
  HCALAnalysis_Dict.C

libHCALAnalysis_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lfun4all \
  -lg4dst \
  -lSubsysReco \
  -lphg4hit

# Rule for generating CINT dictionaries from class headers.
HCALAnalysis_Dict.C: \
  HCALAnalysis.h \
  HCALAnalysisLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict*

