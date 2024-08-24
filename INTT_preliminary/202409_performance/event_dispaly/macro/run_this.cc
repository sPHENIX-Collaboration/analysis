#define __CINT__
#include "analysis_tracking.hh"
#include "Analysis.hh"

int run_this( int run = 41981 )
{

  Analysis* ana = new Analysis( run , true, false );
  ana->Begin();

  return 0;
}
