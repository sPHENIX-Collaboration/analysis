##
# sPHENIX macros
#
# @file
# @version 0.1

ROOT_CPPFLAGS = `root-config --cflags`
LIBFLAGS = -L$(MYINSTALL)/lib `root-config --libs`

all: bin/QA

bin/QA: macro/QA.C
	mkdir -p bin
	g++ -o $@ $< $(ROOT_CPPFLAGS) $(LIBFLAGS) -O3

clean:
	rm bin/QA
