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

string GetDataPath( int run, int mode, int fphx_bco )
{
  string data = "/sphenix/u/nukazuka/INTT/QA/raw_hit/2024/root/InttRawHitQA_run000";
  //data = "/sphenix/u/nukazuka/INTT/QA/trkr_hit/2024/root/InttTrkrHitQA_run000";
  if( mode == 1 )
    data = "./results/InttHitCorrelation_run000";
  else if( mode == 2 )
    data = "./results/InttClusterCorrelation_run000";
  else if( mode == 3 )
    data = "";
  
  data += to_string( run );
  if( fphx_bco != -1 )
    data += "_FPHX_BCO_" + to_string( fphx_bco );

  data += ".root";
  return data;
}

string GetOutputPath( int run, int mode, string suffix = ".pdf", bool is_preliminary = false, int fphx_bco = -1 )
{
  string output = "results/correlation_";

  if( mode == 0 )
    output += "InttRaw_";
  else if( mode == 1 )
    output += "hit_";
  else if( mode == 2 )
    output += "cluster_";
  else if( mode == 3 )
    output += "cluster_tracklet_";
  
  output += "000" + to_string( run );

  if( fphx_bco != -1 )
    output += "_FPHX_BCO_" + to_string( fphx_bco );

  if( is_preliminary == false )
    output += "_internal";
  else
    output += "_prelimiary";
  
  output += suffix;
  return output;
}

int correlation_analysis( int run = 50889, int mode = 1, bool is_preliminary = false, int fphx_bco=-1 )
{
  if( run == 9999 )
    {
      //      int runs[3] = { 50377, 49737, 49743 };
      int runs[3] = { 50377, 49737, 50889 }; // trigger, extended, streaming
      for( int i=0; i<3; i++ ) // run
	for( int j=1; j<3; j++ ) // mode (raw hit, hit w/o hot, cluster, cluster associated to tracklet
	  for( int k=0; k<2; k++ )
	    if( i != 1 )
	      correlation_analysis( runs[i],              // run
				    j,                    // mode
				    (k==0 ? true : false) // internal or preliminary
				    );

      return 0;
    }
  
  if( run < 0 || mode < 0 || mode > 4 )
    {
      HowToUse();
      return 0;
    }

  SetsPhenixStyle();
  string data = GetDataPath( run, mode, fphx_bco );
  cout << data << endl;
  
  TFile* tf = new TFile( data.c_str(), "READ" );
  if( tf == nullptr || tf->IsOpen() == false )
    {
      cerr << data << " is not found." << endl;
      return -1;
    }
  
  TCanvas* c = new TCanvas( "canvas", "title", 800, 800 );
  TH2D* hist = (TH2D*)tf->Get( "inner_outer_barrels_no_adc0" );
  DrawHitNumCorrelation( c, hist, run, mode, is_preliminary );
  
  string output = GetOutputPath( run, mode, ".pdf", is_preliminary, fphx_bco );
  c->Print( output.c_str() );

  cout << "Output: " << output << endl;
  return 0;
}
