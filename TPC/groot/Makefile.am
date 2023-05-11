AUTOMAKE_OPTIONS = foreign

INCLUDES = -I$(includedir) -I$(ONLINE_MAIN)/include  -I$(ONLINE_MAIN)/include/Event  -I`root-config --incdir`

lib_LTLIBRARIES = \
  libgroot.la 

noinst_HEADERS = \
  grootLinkDef.h 

include_HEADERS = \
  groot.h \
  Quiver.h \
  ATrace.h \
  AZigzag.h \
  ABlob.h \
  ATrack.h \
  OutputVisualsTPC.h \
  MyFavoriteMartin.h \
  FillRawHist.h \
  FillCalHist.h \
  FillBlobHist.h \
  FillHoughHist.h \
  FindBlobs.h \
  prdfoStream.h  

libgroot_la_SOURCES = \
  groot.C \
  Quiver.C \
  ATrace.C \
  AZigzag.C \
  ABlob.C \
  ATrack.C \
  OutputVisualsTPC.C \
  MyFavoriteMartin.C \
  FillRawHist.C \
  FillCalHist.C \
  FillBlobHist.C \
  FillHoughHist.C \
  FindBlobs.C \
  prdfoStream.cc \
  groot_Dict.C 

libgroot_la_LDFLAGS =  -L$(ONLINE_MAIN)/lib -L$(OFFLINE_MAIN)/lib -L$(ROOTSYS)/lib -lpmonitor -fPIC

libgroot_la_LIBADD = \
  -L$(libdir) \
  -L$(ONLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib \
  -L$(ROOTSYS)/lib \
 -lGpad -lHist -lGraf -lGraf3d -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lRIO -lNet -lThread -lCore -lCint -lm -ldl -lSpectrum 

# Rule for generating CINT dictionaries from class headers.
groot_Dict.C: \
  groot.h \
  Quiver.h \
  ATrace.h\
  AZigzag.h \
  ABlob.h \
  ATrack.h \
  OutputVisualsTPC.h \
  MyFavoriteMartin.h \
  FillRawHist.h \
  FillCalHist.h \
  FillBlobHist.h \
  FillHoughHist.h \
  FindBlobs.h \
  grootLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict*
