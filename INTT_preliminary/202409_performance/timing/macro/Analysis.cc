#define Analysis_cc
#include "Analysis.hh"

Analysis::Analysis( string data_no_suffix, bool is_preliminary ) :
  is_preliminary_( is_preliminary )
{
  this->Init( data_no_suffix );
}

void Analysis::Init( string data_no_suffix )
{
  // init paths
  root_path_ = data_no_suffix + ".root";
  text_path_ = data_no_suffix + ".dat";

  if( is_preliminary_ == true )
    output_ += "_preliminary.pdf";
  else
    output_ += "_internal.pdf";
  
  // Init data an outptut ROOT file
  this->InitRoot();

  // Init data from a text file
  this->InitText();

  // normalize FPHX BCO dist
  hist_fphx_bco_->Scale( 1.0 / event_num_ );

  if( is_logy_ == true )
    {
      ymin_ = 0.1;
      ymax_ = 5e5;
    }
  else
    {
      ymin_ = 0.0;
      ymax_ = 600;
    }

  return;
}

void Analysis::InitRoot()
{
  /*!
    @brief Init for data from a ROOT file
    @details A histogram of FPHX BCO distribution of all analyzed events
    offset of INTT streaming readout, which is normally 23
    are obtained.
   */
  TFile* tf = new TFile( root_path_.c_str(), "READ" );
  
  string title = ";Beam clock [BCO];Number of INTT hits";

  hist_fphx_bco_raw_ = (TH1D*)tf->Get( "fphx_bco_raw" );
  hist_fphx_bco_raw_->SetTitle( title.c_str() );
  
  hist_fphx_bco_ = (TH1D*)tf->Get( "fphx_bco" );
  this->ConfigureHist( hist_fphx_bco_ );
  //hist_fphx_bco_->SetLineColor( kGray + 1 );
  hist_fphx_bco_->SetLineColor( kBlue + 1 );
  //hist_fphx_bco_->SetLineStyle( 2 );
  
  TH1D* hist_offset = (TH1D*)tf->Get( "streaming_offset" );
  int the_position = 0;
  int largest = 0;
  for( int i=1; i<hist_offset->GetNbinsX()+1; i++ )
    {
      int content = hist_offset->GetBinContent( i );
      if( largest < content )
	{
	  largest = content;
	  the_position = i;
	}

    }

  streaming_offset_ = hist_offset->GetBinLowEdge( the_position );
}

void Analysis::InitText()
{
  /*!
    @brief Init data from a text file
    @details BCO of GL1 and INTT and #hit for each FPHX BCO are taken from the text file event by event.
   */
  ifstream ifs( text_path_.c_str() );
  if( ifs.fail() )
    {
      cerr << text_path_ << " is not found" << endl;
      return;
    }

  string line;
  while( getline( ifs, line ) )
    {
      if( line[0] == '#' )
	continue;
      
      Event* ev = new Event( line, streaming_offset_ );
      if( ev->GetInttGtmBco() == 396957258755 )
	{
	  //ev->Print();
	  // auto hist = ev->GetHist();
	  // for( int i=1; i<hist->GetNbinsX()+1; i++ )
	    // cout << setw(3) << i << " "
	    // 	 << hist->GetBinContent( i ) << " "
	    // 	 << ev->GetGl1Timing() << " "
	    // 	 << ev->GetGl1Bin()
	    // 	 << endl;
	}
      events_.push_back( ev );
    }

  ifs.close();

  event_num_ = events_[ events_.size()-1 ]->GetEventNum();
  return;
}

////////////////////////////////////////////////////////
// private functions
////////////////////////////////////////////////////////

string Analysis::GetDate()
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

void Analysis::DrawWords( Event* event )
{

  ///////////////////////////////////////////////////////////////////////////////
  // Writting words in the canvas                                              //
  ///////////////////////////////////////////////////////////////////////////////
  TLatex* tex = new TLatex();

  double line_height = 0.05;
  double first_margin = 0.005;  
  double pos_y = 1.0 - top_margin_ + first_margin;// - line_height;

  // Date
  tex->DrawLatexNDC( 0.7, pos_y,
		     string("#it{" + GetDate() + "}").c_str() );

  // sPHENIX Internal or sPHENIX Prelimnary
  pos_y -= line_height - first_margin + 0.025;
  double pos_x = 0.2;
  if( is_preliminary_ == false )
    {
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Internal" );
    }
  else
    {
      tex->DrawLatexNDC( pos_x, pos_y, "#it{#bf{sPHENIX}} Preliminary" );
    }

  // p+p 200 GeV
  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "Run-24 #it{p+p} 200 GeV, Run 50889" );

  pos_y -= line_height;
  tex->DrawLatexNDC( pos_x, pos_y, "INTT Streaming readout " );

  pos_y -= line_height;
  stringstream ss;
  ss << "BCO " << event->GetInttGtmBco();
  tex->DrawLatexNDC( pos_x, pos_y, ss.str().c_str() );

  // pos_y -= line_height;
  // if( event->IsGl1Correspondings() )
  //   tex->DrawLatexNDC( pos_x, pos_y, "GL1 match!" );
  // else
  //   tex->DrawLatexNDC( pos_x, pos_y, "GL1 not match" );

}

TH1D* Analysis::GetShiftedHist( TH1D* hist_arg )
{
  TH1D* hist = (TH1D*)hist_arg->Clone();

  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    {
      auto content = hist_arg->GetBinContent( i + shift_value_ );
      // cout << hist->GetNbinsX() << "\t"
      // 	   << setw(3) << i << "\t"
      // 	   << setw(3) << i + shift_value_ << "\t"
      // 	   <<  (hist->GetNbinsX()+1 - shift_value_ ) << "\t"
      // 	   << i + shift_value_ - hist->GetNbinsX() << "\t"
      // 	   << (
      // 	       (i + shift_value_ ) > hist->GetNbinsX() ?
      // 	       i + shift_value_ - hist->GetNbinsX() : 
      // 	       i + shift_value_ 
      // 		)
      // 	   << endl;
      
      if( (i + shift_value_ ) > hist->GetNbinsX() )
	content = hist_arg->GetBinContent( i + shift_value_ - hist->GetNbinsX() );

      hist->SetBinContent( i, content );
    }
  
  return hist;
}

void Analysis::DrawGl1Timing( Event* event )
{
  double timing = event->GetGl1Timing() + 0.5; // absolute value but not bin index

  TLine* line = new TLine();
  line->SetLineColor( kAzure + 1 );
  line->SetLineWidth( 2 );
  line->SetLineStyle( 7 );
  line->DrawLine( timing, ymin_, timing, ymax_ );
}

////////////////////////////////////////////////////////
// public functions
////////////////////////////////////////////////////////
void Analysis::Draw()
{

  SetsPhenixStyle();
  
  TCanvas* c = new TCanvas( output_.c_str(), "title", 800, 800 );
  // c->SetTopMargin( top_margin_ );
  // c->SetRightMargin( right_margin_ );
  c->Print( ((string)c->GetName() + "[").c_str() );

  ///////////////////////////////////////////////////////////////////////////////
  // Drawing hists                                                             //
  ///////////////////////////////////////////////////////////////////////////////
  int counter = 0;
  for( auto& event : events_ )
    {
      counter++;

      // bool condition = event->GetBinMoreThan( 50 ) > 8;
      // bool condition = event->GetBinMoreThan( 10 ) > 12;      
      // bool condition = event->GetBinMoreThan( 0 ) > 30; // standard cut

      // dedicated cut for better plots
      // bool condition = event->GetBinMoreThan( 0 ) > 30 // for low background
      // 	//|| event->GetBinMoreThan( 50 ) < 3  // for some high peaks
      // 	|| (event->IsGl1Correspondings() == false)  // for GL1 maching event
      // 	|| (event->IsHitInAbortGap() == true )
      // 	;

      int ignore_smaller_than = 7;
      bool condition = true
	// && event->GetBinMoreThan( 10 ) < 30 // for low background
	// || event->GetBinMoreThan( 50 ) < 3  // for some high peaks
	&& ( event->IsGl1Correspondings() == true )     // for GL1 maching event
	//&& ( event->IsHitInAbortGap() == false )        // no hit in abort gap
	&& ( event->GetHitNumMachingGl1() > 30 )        // high GL1 peak 
	&& ( event->GetFatPeakNum( ignore_smaller_than ) < 4 )            // few fat peaks
	&& ( event->GetHitNumMaxEvent() / event->GetHitNumMachingGl1() < 3 ) // suppression of the highest peak
	;

      condition = true
	// && event->GetBinMoreThan( 10 ) < 30 // for low background
	// || event->GetBinMoreThan( 50 ) < 3  // for some high peaks
	&& ( event->IsGl1Correspondings() == true )     // for GL1 maching event
	//&& ( event->IsHitInAbortGap() == false )        // no hit in abort gap
	&& ( event->GetHitNumMachingGl1() > 30 )        // high GL1 peak 
	&& ( event->GetFatPeakNum( ignore_smaller_than ) > 10  )            // few fat peaks
	//	&& ( event->GetHitNumMaxEvent() / event->GetHitNumMachingGl1() < 3 ) // suppression of the highest peak
	;
      
	if( event->GetInttGtmBco() == 396912729035 )
	//if( event->GetInttGtmBco() == 396903901595 )
	{
	  cout << "The event!!!" << endl;
	  condition = true;
	}
      else
	continue;

	
      if( !condition )
       	continue;

      //cout << event->GetFatPeakNum( ignore_smaller_than ) << endl;
      // if( event->GetInttGtmBco() != 396957258755 )
      //  	continue;

      // cout << event->GetInttGtmBco() << "\t"
      // 	   << event->GetHitNumMachingGl1() << "\t"
      // 	   << event->GetHitNumMaxEvent() << "\t"
      // 	   << event->IsGl1Correspondings() << "\t"
      // 	   << event->GetHist()->GetBinContent( event->GetGl1Bin() )
      // 	   << endl;
      
      hist_fphx_bco_->GetYaxis()->SetRangeUser( ymin_, ymax_ );
      hist_fphx_bco_->Draw( "HISTE" );
      gPad->SetLogy( is_logy_ );

      event->GetHist()->Draw( "same" );
      event->GetHistGl1Match()->Draw( "same" );

      TLegend* leg = new TLegend( 0.2, 0.55, 0.35, 0.7);
      leg->AddEntry( hist_fphx_bco_, "#INTT hits (100k strobes average)", "l" );
      leg->AddEntry( event->GetHist(), "#INTT hits (Single strobe)", "l" );
      leg->AddEntry( event->GetHistGl1Match(), "#INTT hits (Matching trigger timing)", "l" );
      leg->SetTextSize( 0.04 );
      
      this->DrawWords( event );
      // this->DrawGl1Timing( event );
      leg->Draw();
      c->Print( c->GetName() );

      if( counter > process_num_ )
	break;

    }

  counter = 0;
  int counter_gl1_match = 0; 
  for( auto& event : events_ )
    {
      counter++;
      counter_gl1_match += ( event->IsGl1Correspondings() ? 1 : 0 );
    }
      
  cout << "GL1 matching ratio: "
       << counter_gl1_match << " / " << counter << " = "
       << double(counter_gl1_match) / counter
       << endl;
    
  c->Print( ((string)c->GetName() + "]").c_str() );
}
