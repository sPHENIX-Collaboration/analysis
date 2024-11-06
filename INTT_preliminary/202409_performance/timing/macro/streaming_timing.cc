#include "Analysis.hh"

int streaming_timing( bool is_preliminary=false )
{

  string data_no_suffix = "./results/InttStreamingTiming_run00050889";
  Analysis* ana = new Analysis( data_no_suffix, is_preliminary );
  ana->Draw();
  
  return 0;
}
