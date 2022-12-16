#ifndef MemTimeProgression__h
#define MemTimeProgression__h

// determine how much memory has been used

#include "stdlib.h"
#include "stdio.h"
#include <string>
#include "TStopwatch.h"

int parseLine(char* line);
int getMemValue();
using namespace std;

struct MemTimeProgression : public TObject {
    //--------------------------------
    // start at step=0
    // increment with each call()
    //  -> print to cout at call_print_interval
    //  return true is printing (to allow to print out to log)
    MemTimeProgression(const int step_int, const int _n_total_calls=0);
    long long int nCalls;
    int    mem0;
    int    max_mem;
    double time0;
    TStopwatch watch;
    int  call_print_interval; 
    int  n_total_calls;
    bool call(); // print every print_interval calls; return true if print
    string set_stats();
    string set_get_stats();
    string stats; // populate message
    /* ClassDef (MemTimeProgression,1); */
};

#endif
