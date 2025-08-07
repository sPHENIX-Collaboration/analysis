#include <sPhenixStyle.C>

template < class T >
void HistSetting( T* hist )
{
  hist->GetXaxis()->CenterTitle();
  hist->GetYaxis()->CenterTitle();
  
  hist->GetYaxis()->SetTitleOffset( 1.7 )  ;
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

void DrawWords( int run, bool is_preliminary )
{

  ///////////////////////////////////////////////////////////////////////////////
  // Writting words in the canvas                                              //
  ///////////////////////////////////////////////////////////////////////////////
  TLatex* tex = new TLatex();

  double top_margin = 0.05;
  double line_height = 0.05;
  double first_margin = 0.005;  
  double pos_y = 1.0 - top_margin + first_margin;// - line_height;

  // Date
  tex->DrawLatexNDC( 0.7, pos_y,
		     string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  pos_y -= line_height - first_margin + 0.025;
  double pos_x = 0.5;
  if( is_preliminary == false )
    {
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
    }
  else
    {

      pos_x = 0.5;
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
    }

  // p+p 200 GeV
  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "Run-24 #it{p+p} 200 GeV" );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, ( string("Run ") + to_string(run)).c_str() );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "INTT Triggered mode" );
}

int GetPeakPosition( TH1D* hist )
{
  int bin = 0;
  int content = 0;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    {
    if( content < hist->GetBinContent( i ) )
      {
	content = hist->GetBinContent( i ) ;
	bin = i;
      }
    }

  return bin;
}

int GetRisingEdge( TH1D* hist )
{
  int peak_position = GetPeakPosition( hist );
  int content_max = hist->GetBinContent( peak_position );

  for( int i=peak_position; i>0; i-- )
    {
      if( hist->GetBinContent( i ) < content_max * 1e-3 )
	{
	  return i;
	}      
    }

  return 0;
}

TH1D* GetShiftedHist( TH1D* hist )
{
  int edge_position = GetRisingEdge( hist ) ;
  auto hist_rtn = (TH1D*)hist->Clone();
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    {
      int position = i + edge_position;
      if ( position > hist->GetNbinsX() )
	position -= hist->GetNbinsX();
  
      hist_rtn->SetBinContent( i , hist->GetBinContent( position ) );
     
    }
  
    return hist_rtn;
}

int trigger_timing( int run = 50377, bool is_preliminary = false )
{

  SetsPhenixStyle();

  string output = string("results/trigger_timing_run") + to_string( run );
  if( is_preliminary == true )
    output += "_preliminary.pdf";
  else
    output += "_internal.pdf";
  
  TCanvas* c = new TCanvas( output.c_str(), "title", 800, 800 );
  // c->SetTopMargin( top_margin_ );
  // c->SetRightMargin( right_margin_ );

  string data = string( "results/InttTriggerTiming_run000" ) + to_string( run ) + ".root";
  TFile* tf = new TFile( data.c_str(), "READ" );
  TH1D* hist_raw = (TH1D*)tf->Get( "bco_diff" );
  
  auto hist = GetShiftedHist( hist_raw );
  hist->SetTitle( ";Crossing relative to GTM [BCO];Counts" );
  //hist->SetLineColor( kRed );
  hist->SetFillColorAlpha( hist->GetLineColor(), 0.1 );
  HistSetting( hist );
  //hist->GetXaxis()->SetRangeUser( -1, 128 );
  hist->GetXaxis()->SetRangeUser( 0, 128 );
  hist->Draw();

  hist_raw->SetLineColor( kGray );
  //hist_raw->Draw( "same" );
  DrawWords( run, is_preliminary );
  
  c->Print( c->GetName() );
  return 0;
}
