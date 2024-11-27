#define MipHist_cc
#include "MipHist.hh"


////////////////////////////////////////////////////////////////////////
// constructor                                                        //
////////////////////////////////////////////////////////////////////////
MipHist::MipHist( string name, string title ) :
  name_( name ),
  title_( title )
{
  this->Init();
}

////////////////////////////////////////////////////////////////////////
// private functions                                                  //
////////////////////////////////////////////////////////////////////////
void MipHist::Init()
{
  
  string name_raw = name_ + "_raw";
  hist_raw_ = new TH1D( name_raw.c_str(), title_.c_str(), bin_num_, xmin_, xmax_) ; // , "adc_in", cut, "",
  hist_raw_->SetLineWidth( 2 );
  HistSetting( hist_raw_ );

  string name_shc = name_ + "_shc";
  hist_shc_adc7_ = new TH1D( name_shc.c_str(), title_.c_str(), bin_num_, xmin_, xmax_) ; // , "adc_in", cut, "",
  hist_shc_adc7_->SetLineWidth( 2 );
  HistSetting( hist_shc_adc7_ );

  string name_dhc = name_ + "_dhc";
  hist_dhc_adc14_ = new TH1D( name_dhc.c_str(), title_.c_str(), bin_num_, xmin_, xmax_) ; // , "adc_in", cut, "",
  hist_dhc_adc14_->SetLineWidth( 2 );
  HistSetting( hist_dhc_adc14_ );

  f_ = new TF1( "f", "landau", xmin_, xmax_ );
  f_->SetLineStyle( 2 );
  
}

////////////////////////////////////////////////////////////////////////
// private functions                                                  //
////////////////////////////////////////////////////////////////////////
void MipHist::Fill( TH1D* hist, int val )
{
  hist->Fill( val );
}

void MipHist::ModifyAdc( TH1D* hist_ref, double power_factor )
{
  cout << "MipHist::ModifyAdc, " << hist_ref->GetName() << endl;
  
  for( int i=1; i<hist_ref->GetNbinsX() + 1; i++ )
    {
      int content_raw = hist_raw_->GetBinContent( i );

      int content = hist_ref->GetBinContent(i);
      if( content == 0 )
	{
	  continue;
	}
      
      int content_diff = content_raw - content;
      double content_modified = pow( modification_factor_, power_factor ) * content + content_diff;
      hist_->SetBinContent( i, content_modified ); // update if needed
      
      cout << i << " "
	   << hist_->GetBinLowEdge( i ) << " "
	   << content_raw << "\t"
	   << content << "\t"
	   << content_diff << "\t"
	   << content_modified
	   << endl;
    }

  // cout << "ModifyAdc" << endl;
  // cout << "Index of ADC: " << index_adc << endl;
  // cout << "#ADC: " << num_cluster_adc << endl;
  // cout << "#ADC (single-hit cluster): " << num_single_hit_cluster_adc << endl;
  // cout << "#ADC (multiple-hit cluster): " << num_multiple_hit_cluster_adc << endl;
  // cout << "Modified #ADC (single-hit cluster): " << num_modified_single_hit_cluster_adc << endl;
  // cout << "Moddified #ADC: " << num_modified_cluster_adc << endl;

}

////////////////////////////////////////////////////////////////////////
// public functions                                                   //
////////////////////////////////////////////////////////////////////////
void MipHist::DrawLine( TF1* f )
{
  TLine* line = new TLine();
  line->SetLineColor( f->GetLineColor() );
  line->SetLineStyle( f->GetLineStyle() );

  double mpv = f->GetParameter(1);
  line->DrawLine( mpv, 0, mpv, 1.3 );

}

void MipHist::Fill( int val )
{
  this->Fill( hist_raw_, val );
}

void MipHist::FillAll( int val, bool is_single, bool is_double )
{
  this->Fill( hist_raw_, val );

  if( is_single )
    this->Fill( hist_shc_adc7_, val );
  else if( is_double )
    this->Fill( hist_dhc_adc14_, val );
}

TF1* MipHist::GetNormalizedFunction( double top_val )
{
  TF1* f = (TF1*)f_->Clone();
  f->SetParLimits( 0, 0, 250 );
  f->SetParLimits( 1, 70, 150 );
  f->SetParLimits( 2, 0, 100 );
  auto hist = this->GetNormalizedHist( top_val );
  hist->Fit( f, "0", "", 35, 600 );

  return f;
}

TH1D* MipHist::GetNormalizedHist( double top_val )
{
  auto hist = (TH1D*)hist_->Clone();
  if( true )
    {
      hist->Scale( 1.0 / hist->GetEntries() );
    }
  else if( false )
    {
      double max_content = 0.0;
      for( int i=1; i<hist->GetNbinsX()+1; i++ )
	if( max_content < hist->GetBinContent(i) )
	  max_content = hist->GetBinContent(i) ;
      
      hist->Scale( top_val / max_content );
      //      hist->Scale( 1. / max_content );
    }
  else if( false )
    {
      hist->Scale( 1. / hist->GetBinContent( 2 ) );
    }
  return hist;
}

void MipHist::ModifyAdc()
{
  // init
  hist_ = (TH1D*)hist_raw_->Clone();
  hist_->SetName( name_.c_str() );
  hist_->SetFillColorAlpha( color_, alpha_ );
  hist_->SetLineColorAlpha( color_, 1 );

  if( does_adc7_correction_ == true )
    this->ModifyAdc( hist_shc_adc7_, 1.0 );
  
  if( does_adc14_correction_ == true )
    this->ModifyAdc( hist_dhc_adc14_, 2.0 );

  hist_->Fit( f_, "0" );
}

void MipHist::SetColorAlpha( int color, double alpha, bool does_all )
{
  color_ = color;
  alpha_ = alpha;
  
  hist_raw_->SetFillColorAlpha( color_, alpha_ );
  hist_raw_->SetLineColorAlpha( color_, 1 );

  hist_shc_adc7_->SetFillColorAlpha( color_, alpha_ );
  hist_shc_adc7_->SetLineColorAlpha( color_, 1 );

  hist_dhc_adc14_->SetFillColorAlpha( color_, alpha_ );
  hist_dhc_adc14_->SetLineColorAlpha( color_, 1 );

  if( does_all )
    {
      hist_->SetFillColorAlpha( color_, alpha_ );
      hist_->SetLineColorAlpha( color_, 1 );
    }
  
  f_->SetLineColor( color_ );
}

void MipHist::Print()
{

  cout << string( 50, '-' ) << endl;
  cout << tag_ << endl;
  cout << setw(3) << "bin" << " "
       << setw(4) << "DAC" << "  "
       << setw(8) << "Raw" << "  "
       << setw(8) << "Single7" << "  "
       << setw(8) << "Double14" << "  "
       << setw(8) << "Modified"
       << endl;

  // from underflow bin to overflow bin
  for( int i=0; i<hist_->GetNbinsX() + 2; i++ )
    {
      cout << setw(3) << i << " "
	   << setw(4) << hist_->GetBinLowEdge( i ) << "  "
	   << setw(8) << hist_raw_->GetBinContent(i) << "  "
	   << setw(8) << hist_shc_adc7_->GetBinContent(i) << "  "
	   << setw(8) << hist_dhc_adc14_->GetBinContent(i) << "  "
	   << setw(8) << hist_->GetBinContent(i) << "  "
	   << endl;
    }
  cout << endl;

}
