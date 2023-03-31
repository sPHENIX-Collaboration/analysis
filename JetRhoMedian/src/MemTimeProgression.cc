#include "MemTimeProgression.h"

#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <iostream>
#include <iomanip>
#include "TString.h"

/* ClassImp(MemTimeProgression) */

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
};

int getMemValue(){ //Note: this value is in KB!
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmSize:", 7) == 0){
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return result;
};

MemTimeProgression::MemTimeProgression(const int print_int, const int _n_total_calls) :
    nCalls   {0},
    mem0     {0},
    max_mem  {0},
    time0    {0.},
    watch    {},
    call_print_interval {print_int},
    n_total_calls { _n_total_calls } 
{
    watch.Start();
    call();
};

string MemTimeProgression::set_stats() {
    int    mem1  = getMemValue();
    if (mem1 > max_mem) max_mem = mem1;
    double time1 = watch.RealTime();
    watch.Continue();

    const char* pm_mem = (mem1>mem0) ? "+" : "-";
    if (n_total_calls == 0) {
      stats = Form(" Finished %8lli calls | Time: %5.0f sec (+ %4.0f) | "
                 "Mem: %6.2f MB (%s%6.2f)",
                  nCalls, time1, time1-time0, mem1/1000., pm_mem, (mem1-mem0)/1000.);
    } else {
      const double calls_left = n_total_calls-nCalls;
      const double rate = nCalls / time1;
      int sec_left = calls_left / rate;
      int min_left = sec_left / 60;
      const int hr_left  = min_left / 60;
      sec_left = sec_left % 60;
      min_left = min_left % 60;

      ostringstream time;
      time << std::setfill('0') << std::setw(2) << hr_left  << ":"
           << std::setfill('0') << std::setw(2) << min_left << ":"
           << std::setfill('0') << std::setw(2) << sec_left;
      

      stats = Form(" Finished %8lli calls | Time: %5.0f sec (+ %4.0f) | "
                 "Mem: %6.2f MB (%s%6.2f)  | ETA %s",
                  nCalls, time1, time1-time0, mem1/1000., pm_mem, (mem1-mem0)/1000.,
                  time.str().c_str());
    }

    time0=time1;
    mem0=mem1;
    return stats;
};

bool MemTimeProgression::call(){
    ++nCalls;
    if (nCalls % call_print_interval == 0) {
        set_stats();
        cout << stats << endl;
        return true;
    } else {
        return false;
    }
};

string MemTimeProgression::set_get_stats() {
    cout << set_stats() << endl;
    return stats;
};
