#define HitNumCorrelation_cc
#include "DrawHitNumCorrelation.hh"

void ConfigureHist( TH2D* hist, int mode )
{
  hist->GetXaxis()->CenterTitle();
  hist->GetYaxis()->CenterTitle();

  if( mode == 0 ) // raw hit
    {
      hist->GetXaxis()->SetLabelSize( 0.04 );
      hist->GetXaxis()->SetRangeUser( 0, 500 );

      hist->GetYaxis()->SetLabelSize( 0.04 );
      hist->GetYaxis()->SetRangeUser( 0, 500 );

    }
  else if( mode == 1 ) // Trkr hit
    {
      hist->GetXaxis()->SetRangeUser( 0, 300 );
      hist->GetYaxis()->SetRangeUser( 0, 300 );
    }
  else if( mode == 2 ) // Trkr cluster
    {
      hist->GetXaxis()->SetRangeUser( 0, 120 );
      hist->GetYaxis()->SetRangeUser( 0, 120 );      
    }
}

string GetDate()
{
  return "9/13/2024";
  TDatime dt;
  int year	= dt.GetYear();
  int month	= dt.GetMonth();
  int day	= dt.GetDay();

  // format: mm/dd/yyyy
  std::stringstream ss;
  ss << month << "/" << day << "/" << year;

  return ss.str();
}

void DrawHitNumCorrelation( TCanvas* c, TH2D* hist, int run, int mode, bool is_preliminary )
{
  ///////////////////////////////////////////////////////////////////////////////
  // Canvas configuration                                                     //
  ///////////////////////////////////////////////////////////////////////////////
  //  auto pad = c->cd(1);
  // pad->SetPadTopMargin(0.1);
  // pad->SetPadRightMargin(0.1);
  double top_margin = 0.1;
  double right_margin = top_margin;
  right_margin = 0.15;

  c->SetTopMargin( top_margin );
  c->SetRightMargin( right_margin );
  // c->SetPadBottomMargin(0.16);
  // c->SetPadLeftMargin(0.16);

  ///////////////////////////////////////////////////////////////////////////////
  // Hist configuration and draw                                              //
  ///////////////////////////////////////////////////////////////////////////////
  string title = ";"; // hist title
  string target = "hits";
  if( mode > 1 )
    target = "clusters";
    
  title = title + "Number of " + target + " at inner barrel;" // x-axis
    + "Number of " + target + " at outer barrel;" // y-axis
    + ""; // z axis

  hist->SetTitle( title.c_str() );

  // if( mode == 1 )
  //   hist->Rebin2D(2, 2);

  hist->Draw( "colz" );
  cout << "hit num: " << hist->GetEntries() << endl;
  ConfigureHist( hist, mode );
  gPad->SetLogz( true );

  double palette_width = 0.05;
  DrawPaletteAxis( hist,
		   1.0 - (right_margin - 0.01 ), 0.16,
		   1.0 - (right_margin - 0.01 - palette_width ), 1.0 - top_margin,
		   1);

  ///////////////////////////////////////////////////////////////////////////////
  // Writting words in the canvas                                              //
  ///////////////////////////////////////////////////////////////////////////////
  TLatex* tex = new TLatex();
  double line_height = 0.05;
  double first_margin = 0.012;  
  double pos_y = 1.0 - top_margin + first_margin;// - line_height;

  // Date
  tex->DrawLatexNDC( 0.65, pos_y,
		     string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  //  pos_y -= line_height - first_margin + 0.025;
  double pos_x = 0.18;
  if( is_preliminary == false )
    {
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
    }
  else
    {

      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
    }

  // p+p 200 GeV
  pos_y -= line_height - first_margin + 0.03;
  //  pos_y -= line_height;
  //  tex->DrawLatexNDC( pos_x, pos_y, ("Run-24 #it{p+p} 200 GeV, Run " + to_string(run) ).c_str());
  tex->DrawLatexNDC( pos_x, pos_y, ("Run-24 #it{p+p} 200 GeV") );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, ("Run " + to_string(run) ).c_str());

  pos_y -= line_height;
  if( run == 50889 )
    {
      tex->DrawLatexNDC( pos_x, pos_y, "INTT Streaming readout" );
    }
  else
    {
      tex->DrawLatexNDC( pos_x, pos_y, "INTT Triggered mode" );

    }
  
  return;
}

