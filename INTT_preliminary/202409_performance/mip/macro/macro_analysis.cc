#define __CINT__

#ifdef __linux__
#include "/home/gnukazuk/soft/MyLibrary/include/MyLibrary.hh"
// #include "/home/gnukazuk/cernbox/Analysis_new/COMPASS_DY/common_macro/include/CommonInclude.hh"

#elif __APPLE__
// #include "/Users/nukazuka/local/MyLibrary/include/MyLibrary.hh"
#include "/Users/genki/soft/MyLibrary/include/MyLibrary.hh"
// #include "/Users/nukazuka/cernbox/Analysis_new/COMPASS_DY/common_macro/include/CommonInclude.hh"
// #include "/Users/gnukazuk/cernbox/Analysis_new/COMPASS_DY/common_macro/include/CommonInclude.hh"

#endif

#include "analysis.h"

int macro_analysis( int run = 41981 )
{
  
  analysis* ana = new analysis(0, run );
  ana->Loop();

  ana->hist_ang90->Draw();

  string output = "output_analysis_" + to_string( run ) + ".pdf";
  TCanvas* c = GetCanvas( output, "double" );

  vector < TH1D* > hists;
  // hists.push_back( ana->hist_all );
  // hists.push_back( ana->hist_aso );
  //hists.push_back( ana->hist_no_aso );
  hists.push_back( ana->hist_ang90 );
  hists.push_back( ana->hist_ang45 );
  hists.push_back( ana->hist_ang35 );
  // hists.push_back( ana->hist_ang25 );

  MultiHist* mh = new MultiHist( ana->hist_all->GetName(), ana->hist_all->GetTitle(), hists );
  gStyle->SetOptFit( true );
  mh->SetStatsFormat( 111111 );
  mh->SetStatsBoxSize( 0.15, 0.25 );
  //mh->SetStatsBoxPoint( 1.0, 1.0 );
  
  vector < TF1* > functions;
  for( auto hist : mh->GetHists() )
    {
      TF1* f = new TF1( "f", "landau", 35, 600 );
      f->SetLineColor( hist->GetLineColor() );
      f->SetLineStyle( 7 );
      //hist->Fit( f );
      functions.push_back( f );
    }
  
  c->Print( ((string)c->GetName() + "[").c_str() );
  mh->Draw( "HIST" );
  for( auto& f : functions )
    {
      f->Draw( "same" );  
      
      TLine* line = new TLine();
      line->SetLineColor( f->GetLineColor() );
      line->SetLineStyle( 3 );
      line->DrawLine( f->GetParameter(1), 0, f->GetParameter(1), 1e3 );
    }

  gPad->Update();
  c->Print( c->GetName() );

  mh->SetYmin( 1 );
  mh->Draw( "HIST" );
  for( auto& f : functions )
    f->Draw( "same" );  
  gPad->SetLogy( true );
  c->Print( c->GetName() );  
  
  gPad->SetLogy( false );
  mh->NormalizeHist();
  mh->Draw( "HISTE" );

  for( auto hist : mh->GetHists() )
    {
      TF1* f = new TF1( "f", "landau", 35, 600 );
      f->SetLineColor( hist->GetLineColor() );
      f->SetLineStyle( 7 );
      hist->Fit( f );
      //functions.push_back( f );
      f->Draw( "same" );

      TLine* line = new TLine();
      line->SetLineColor( f->GetLineColor() );
      line->SetLineStyle( 3 );
      line->DrawLine( f->GetParameter(1), 0, f->GetParameter(1), 0.3 );
    }
  c->Print( c->GetName() );  

  /*
  this_cut = "!(adc_in==210 && size_in==1) && fabs(z_vertex)<23 && is_associated_in";
  //string cut = "!(adc_in==210 && size_in==1)";
  auto hist_theta = Draw( tr, "theta", "#theta distribution;#theta;Entries", 180, -90, 90,
			"track_incoming_theta_in", this_cut, "", kBlack, 0.1 );
  hist_theta->Draw();
  c->Print( c->GetName() );  

  this_cut = "!(adc_in==210 && size_in==1) && is_associated_in";
  auto hist_z = Draw( tr, "z_vertex", "z_{vtx} distribution;z_{vtx} (cm);Entries", 100, -50, 50,
		      "z_vertex", this_cut, "", kBlack, 0.1 );
  hist_z->Draw();
  c->Print( c->GetName() );  
  */
  
  ana->hist_correlation->Draw( "colz" );
  gPad->SetLogz( true );
  DrawStats( ana->hist_correlation, 0.7, 0.7, 0.9, 0.9 );
  c->Print( c->GetName() );  
  c->Print( ((string)c->GetName() + "]").c_str() );

  return 0;
}
