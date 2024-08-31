#include "Analysis.hh"

int macro_analysis( int run = 50889 )
{
  
  Analysis* ana = new Analysis( run );
  ana->SetPreliminary( false );
  ana->Loop();
  ana->Draw();

  return 0;
}
