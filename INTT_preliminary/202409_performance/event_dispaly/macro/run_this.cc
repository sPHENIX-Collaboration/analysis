#include <sPhenixStyle.C>
#include "analysis_tracking.hh"
#include "Analysis.hh"

int run_this( int run = 50889 )
{

  SetsPhenixStyle();
  Analysis* ana = new Analysis( run , true, false );
  //ana->SetDebugMode( true );
  ana->SetPageNumLimit( 400 );
  ana->Begin();

  return 0;
}
