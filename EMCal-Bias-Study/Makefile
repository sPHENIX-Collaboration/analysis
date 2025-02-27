##
# EMCal Bias Study
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
ROOT_LIBFLAGS = `root-config --libs`
sPHENIX_CPPFLAGS = -I$(OFFLINE_MAIN)/include -I$(OFFLINE_MAIN)/rootmacros
sPHENIX_LIBFLAGS = -L$(OFFLINE_MAIN)/lib -lcalo_io
CPPFLAGS = -I$(MYINSTALL)/include
LIBFLAGS = -L$(MYINSTALL)/lib

all: bin/read-Bias

bin/read-Bias: macros/read-Bias.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(ROOT_LIBFLAGS) $(sPHENIX_CPPFLAGS) $(sPHENIX_LIBFLAGS)

clean:
	rm -f bin/read-Bias

# end
