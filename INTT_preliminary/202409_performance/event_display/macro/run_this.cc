#include <sPhenixStyle.C>
#include "analysis_tracking.hh"
#include "Analysis.hh"

int run_this( int run = 50889, int fphx_bco=-1 )
{

  SetsPhenixStyle();
  Analysis* ana = new Analysis( run , fphx_bco, true, false );
  //ana->SetDebugMode( true );
  ana->SetPageNumLimit( 9999999 );
  ana->Begin();

  return 0;
}
