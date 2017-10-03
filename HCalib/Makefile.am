AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I$(G4_MAIN)/include \
  -I$(G4_MAIN)/include/Geant4 \
  -I`root-config --incdir`

SUBDIRS = 

lib_LTLIBRARIES = \
  libHCalib.la 

noinst_HEADERS = \
  HCalibLinkDef.h 

include_HEADERS = \
  HCalib.h

libHCalib_la_SOURCES = \
  HCalib.C \
  HCalib_Dict.C

libHCalib_la_LIBADD = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -lfun4all \
  -lg4dst \
  -lSubsysReco \
  -lphg4hit

# Rule for generating CINT dictionaries from class headers.
HCalib_Dict.C: \
  HCalib.h \
  HCalibLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict*

