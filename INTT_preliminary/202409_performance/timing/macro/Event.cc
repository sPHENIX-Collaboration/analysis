#define Event_cc
#include "Event.hh"

Event::Event( string line, int offset ) :
  offset_( offset )
{
  stringstream ss( line );
  ss >> event_ >> bco_gl1_ >> bco_intt_;

  string name = string("FPHX_BCO_event") + to_string( event_ );
  string title = ";INTT Local Clock [BCO];Counts";

  hist_fphx_bco_ = new TH1D( name.c_str(), title.c_str(), 120, 0, 120 );
  hist_fphx_bco_->SetLineColor( kBlack );
  hist_fphx_bco_->SetLineWidth( 3 );

  int count = 0 ;
  for( int i=0; i<120; i++ )
    {
      ss >> count;
      // cout << setw(4) << i << " "
      // 	   << count
      // 	   << endl;
      hist_fphx_bco_->SetBinContent( i+1, count );
	
    }

  int timing = this->GetGl1Timing();
  hist_fphx_bco_gl1_match_ = new TH1D( (name + "_gl1_match").c_str(), title.c_str(),
				       1, timing, timing + 1 );
  hist_fphx_bco_gl1_match_->SetLineColor( kRed );
  hist_fphx_bco_gl1_match_->SetLineWidth( 3 );
  hist_fphx_bco_gl1_match_->SetBinContent( 1, hist_fphx_bco_->GetBinContent(this->GetGl1Bin()) );
  
}

int Event::GetFatPeakNum( int ignore_smaller_than )
{

  int counter = 0;
  int peak_width = 0;
  for( int i=1; i<hist_fphx_bco_->GetNbinsX()+1; i++ )
    {
      if( hist_fphx_bco_->GetBinContent(i) <= ignore_smaller_than ) // here is nothing
	{
	  peak_width = 0;
	  continue;
	}
      
      peak_width++; // you are inside peak now
      if( hist_fphx_bco_->GetBinContent(i+1) <= ignore_smaller_than ) // end of peak
	{
	  if( peak_width > 1 )
	    counter++;
	}
      
    }

  return counter;
}

int Event::GetGl1Timing()
{
  int timing = bco_gl1_ - bco_intt_ + offset_;
  if( timing < 0 )
    timing += 120;
  else if( timing > 120 )
    timing = timing % 120;

  return timing;
}

int Event::GetGl1Bin()
{
  return this->GetGl1Timing() + 1;
}

int Event::GetHitNumMachingGl1()
{
  return hist_fphx_bco_gl1_match_->GetBinContent( 1 );
}

int Event::GetHitNumMaxEvent()
{
  int num = 0;
  for( int i=1; i<hist_fphx_bco_->GetNbinsX()+1; i++ )
    if( num < hist_fphx_bco_->GetBinContent(i) )
      num = hist_fphx_bco_->GetBinContent(i);
    
  return num;
}

int Event::GetBinMoreThan( int threshold )
{
  int counter = 0;
  for( int i=1; i<hist_fphx_bco_->GetNbinsX()+1; i++ )
    {
      if( hist_fphx_bco_->GetBinContent(i) > threshold )
	counter++;
    }

  return counter;
}

bool Event::IsGl1Correspondings()
{
  
  if( hist_fphx_bco_->GetBinContent( this->GetGl1Bin() ) == 0 )
    return false;

  return true;
}

bool Event::IsHitInAbortGap()
{
  int abort_gap_end_first = 6;
  for( int i=1; i<abort_gap_end_first+1; i++ )
    if( hist_fphx_bco_->GetBinContent(i) > 0 )
      return true;

  int fill_num = 111;
  for( int i=abort_gap_end_first + fill_num + 1; i<120+1; i++ )
    if( hist_fphx_bco_->GetBinContent(i) > 0 )
      return true;

  return false;
}

void Event::Print()
{
  cout << "Event: " << event_ << endl;
  cout << "GL1 BCO     : " << bco_gl1_ << endl;
  cout << "INTT GTM BCO: " << bco_intt_ << endl;
  cout << "GL1 timing: " << this->GetGl1Timing() << endl;

  cout << setw(3) << "" << " | ";
  for( int j=0; j<10; j++ )
    cout << setw(4) << j << " ";

  cout << endl;
  cout << string( 3 + 3 + (4+1) * 10, '-' ) << endl;
  
  for( int i=0; i<12; i++ )
    {
      cout << setw(3) << i << " | ";
      for( int j=0; j<10; j++ )
	{
	  cout << setw(4) << hist_fphx_bco_->GetBinContent( i * 10 + j ) << " ";

	}

      cout << endl;
    }
  //  cout 

  cout << string( 3 + 3 + (4+1) * 10, '=' ) << endl;

}
