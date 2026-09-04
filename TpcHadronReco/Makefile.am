AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

# Keep a stable lower-case install path for analysis macros:
#   #include <tpchadronreco/TpcV0CandidateTree.h>
pkgincludedir = $(includedir)/tpchadronreco
pkginclude_HEADERS = \
  TpcV0CandidateTree.h \
  TpcTrackFit.h \
  TpcTrackHelixFitter.h \
  TpcTrackKalmanFitter.h

lib_LTLIBRARIES = libTpcHadronReco.la

libTpcHadronReco_la_SOURCES = \
  TpcV0CandidateTree.cc \
  TpcTrackHelixFitter.cc \
  TpcTrackKalmanFitter.cc

# These are the same sPHENIX dependencies used by the original
# TrackingDiagnostics package. They can be reduced later once the module is
# placed in its final repository and built in the target environment.
libTpcHadronReco_la_LIBADD = \
  -lffaobjects \
  -lphool \
  -lfun4all \
  -lg4eval \
  -lg4eval_io \
  -lphg4hit \
  -lphfield \
  -lmbd_io \
  -lg4detectors_io \
  -lmvtx \
  -lmvtx_io \
  -lintt \
  -lintt_io \
  -lmicromegas_io \
  -lSubsysReco \
  -ltpc_io \
  -ltrack_io \
  -ltrackeralign \
  -ltrackbase_historic_io \
  -ltpctrackreco

BUILT_SOURCES = testexternals.cc
noinst_PROGRAMS = testexternals
testexternals_SOURCES = testexternals.cc
testexternals_LDADD = libTpcHadronReco.la

testexternals.cc:
	echo "// generated build/link test" > $@
	echo "int main() { return 0; }" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
