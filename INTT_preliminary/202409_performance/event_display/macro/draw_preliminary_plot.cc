#include <sPhenixStyle.C>


int draw_preliminary_plot( int run_num = 50889 )
{

  SetsPhenixStyle();
  string data_dir = "results/tracking/";
  string data = data_dir + "tracking_run" + to_string( run_num ) + ".root";

  TFile* tf = new TFile( data.c_str(), "READ" );
  tf->ls();

  auto list = tf->GetListOfKeys();
  for( int i=0; i<list->GetEntries(); i++ )
    {
      string name = list->At( i )->GetName();
      if( name.find( "canvas" ) == string::npos )
	continue;

      TCanvas* c = (TCanvas*)tf->Get( name.c_str() );
      c->Draw();

      string num_str = name.substr( name.find_last_of( "_" ), name.size() );
      string output = data_dir + "event_display_run" + to_string( run_num ) + num_str + ".pdf";

      cout << output << endl;
      c->Print( output.c_str() );

      c->Print();
      c->ls();
      break;
    }

  
  tf->Close();
  return 0;
}
