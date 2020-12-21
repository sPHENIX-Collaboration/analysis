AUTOMAKE_OPTIONS = foreign

INCLUDES = -I. -I$(ROOTSYS)/include -I$(OFFLINE_MAIN)/include

bin_PROGRAMS = ccbb fakee_invmass crossterms

ccbb_SOURCES = ccbb.cc

fakee_invmass_SOURCES = fakee_invmass.cc

crossterms_SOURCES = crossterms.cc

#LDFLAGS = -L$(OFFLINE_MAIN)/lib  `root-config --libs`
LDFLAGS = -L$(OFFLINE_MAIN)/lib  @ROOTLIBS@
  

