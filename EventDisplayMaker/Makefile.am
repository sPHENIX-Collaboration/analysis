AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

AM_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  -L$(OFFLINE_MAIN)/lib64

pkginclude_HEADERS = \
  event_display_maker.h \
  displayEvents.h

lib_LTLIBRARIES = \
  libhf_trigger_data_to_json.la

libhf_trigger_data_to_json_la_SOURCES = \
  event_display_maker.cc \
  displayEvents.cc

libhf_trigger_data_to_json_la_LIBADD = \
  -lKFParticle \
  -lSubsysReco \
  -lphool \
  -lcalo_io \
  -lcalotrigger \
  -lkfparticle_sphenix_io \
  -ltrackbase_historic

BUILT_SOURCES = testexternals.cc

noinst_PROGRAMS = \
  testexternals

testexternals_SOURCES = testexternals.cc
testexternals_LDADD   = libhf_trigger_data_to_json.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

clean-local:
	rm -f $(BUILT_SOURCES)
