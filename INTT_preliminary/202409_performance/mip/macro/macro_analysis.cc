#include "Analysis.hh"

int macro_analysis( int run = 50889 )
{
  
  Analysis* ana = new Analysis( run );
  // ana->SetAdc7Correction( false );
  // ana->SetAdc14Correction( false );

  ana->Loop();

  ana->SetPreliminary( false );
  ana->Draw();

  ana->SetPreliminary( true );
  ana->Draw();

  //ana->Print();
  return 0;
}
