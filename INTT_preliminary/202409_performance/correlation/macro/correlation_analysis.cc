#include <sPhenixStyle.C>
#include "DrawHitNumCorrelation.hh"

void HowToUse()
{
  cerr << "+" << string( 100, '-') << "+" << endl;
  cerr << "| Usage: " << "root  \'correlation_analysis.cc( run, mode, is_official, is_preliminary )\'" << endl;
  cerr << "|   here," << endl;
  cerr << "| \t" << "run:  Run number to be analyzed." << endl;
  cerr << "| \t" << "      Good runs are:" << endl;
  cerr << "| \t" << "         - 50377: trigger only" << endl;
  cerr << "| \t" << "         - 49737: trigger + extended readout" << endl;
  cerr << "| \t" << "         - 49743: streaming data" << endl;
  cerr << "| \t" << "mode: Choice of what to be analyzed." << endl;
  cerr << "| \t" << "      0: raw hit (InttRawHit), no cut" << endl;
  cerr << "| \t" << "      1: raw hit (TrkrHit) but clone hit rejection and hot channel rejection are applied" << endl;
  cerr << "| \t" << "      2: cluster (TrkrCluster)" << endl;
  cerr << "| \t" << "      3: cluster (TrkrCluster) associated with INTT tracklet" << endl;
  cerr << "| \t" << "is_official: true or false" << endl;
  cerr << "| \t" << "is_preliminary: true or false" << endl;
  cerr << "+" << string( 100, '-') << "+" << endl;
  return;
}

string GetDataPath( int run, int mode, bool is_official )
{
  string data = "/sphenix/u/nukazuka/INTT/QA/raw_hit/2024/root/InttRawHitQA_run000";
  if( mode == 1 )
    data = "/sphenix/u/nukazuka/INTT/QA/trkr_hit/2024/root/InttTrkrHitQA_run000";
  else if( mode == 2 )
    data = "";
  else if( mode == 3 )
    data = "";
  
  data += to_string( run );
  if( is_official == true )
    data += "_official.root";
  else
    data += "_special.root";

  return data;
}

string GetOutputPath( int run, int mode, bool is_official, string suffix = ".pdf", bool is_preliminary = false )
{
  string output = "results/hit_num_correlation_";

  if( mode == 0 )
    output += "InttRaw_";
  else if( mode == 1 )
    output += "TrkrHit_";
  else if( mode == 2 )
    output += "TrkrCluster_";
  else if( mode == 3 )
    output += "TrkrCluster_tracklet_";
  
  output += "000" + to_string( run );
  if( is_official == false )
    output += "_special";

  if( is_preliminary == false )
    output += "_internal";
  else
    output += "_prelimiary";
  
  output += suffix;
  return output;
}

int correlation_analysis( int run = 50377, int mode = 0, bool is_official = true, bool is_preliminary = false )
{
  if( run == 9999 )
    {
      int runs[3] = { 50377, 49737, 49743 };
      for( int i=0; i<3; i++ ) // run
	for( int j=0; j<4; j++ ) // mode
	  for( int k=0; k<2; k++ )
	    correlation_analysis( runs[i],              // run
				  j,                    // mode
				  true,                 // is_official
				  (k==0 ? true : false) // internal or preliminary
				  );
    }
  
  if( run < 0 || mode < 0 || mode > 4 )
    {
      HowToUse();
      return 0;
    }

  if( mode == 2 )
    {
      cerr << "mode 2 is not ready" << endl;
      return 0;
    }
  
  SetsPhenixStyle();
  string data = GetDataPath( run, mode, is_official );
  cout << data << endl;
  
  TFile* tf = new TFile( data.c_str(), "READ" );
  if( tf == nullptr || tf->IsOpen() == false )
    {
      cerr << data << " is not found." << endl;
      return -1;
    }
  
  TCanvas* c = new TCanvas( "canvas", "title", 800, 800 );
  TH2D* hist = (TH2D*)tf->Get( "inner_outer_barrels" );
  DrawHitNumCorrelation( c, hist, mode, is_preliminary );
  
  string output = GetOutputPath( run, mode, is_official, ".pdf", is_preliminary );
  c->Print( output.c_str() );

  cout << "Output: " << output << endl;
  return 0;
}
