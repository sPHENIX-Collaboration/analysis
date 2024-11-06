#include <sPhenixStyle.C>

int correlation_analysis_each_BCO()
{

  SetsPhenixStyle();
  
  TGraph* g = new TGraph();
  g->SetTitle( ";Mean of number of clusters at inner barrel;Mean of number of clusters at outer barrel;" );
  g->SetMarkerStyle( 20 );

  TGraph* g_bco = new TGraph();
  g_bco->SetTitle( ";FPHX BCO [BCO];Mean of number of INTT clusters " );

  TGraph* g_correlation = new TGraph();
  g_correlation->SetTitle( ";FPHX BCO [BCO];Correlation factor between number of INTT clusters at inner and outer barrels" );

  for( int i=0; i<120; i++ )
    {
      //string data = string("results/InttClusterCorrelation_run00050889_FPHX_BCO_") + to_string(i) + ".root";
      string data = string("results/InttHitCorrelation_run00050889_FPHX_BCO_") + to_string(i) + ".root";
      TFile* tf = new TFile( data.c_str(), "READ" );
      if( tf->IsOpen() == false )
	break;
      
      auto hist = (TH2D*)tf->Get( "inner_outer_barrels_no_adc0" );

      g->AddPoint( hist->GetMean(1), hist->GetMean(2) );
      g_bco->AddPoint( i, hist->GetMean(1) +  hist->GetMean(2) );
      g_correlation->AddPoint( i, hist->GetCorrelationFactor() );

      cout << data << endl;
    }

  string output = "results/correlation_analysis_each_BCO.pdf";
  TCanvas* c = new TCanvas( output.c_str(), "title", 800, 800 );
  c->Print( (output + "[").c_str() );

  g->Draw( "AP" );
  c->Print( c->GetName( ) );

  g_bco->Draw( "AP" );
  c->Print( c->GetName( ) );

  g_correlation->Draw( "AP" );
  c->Print( c->GetName( ) );

  c->Print( (output + "]").c_str() );
  return 0;
}
