#pragma once

template < class T >
void HistSetting1D( T* hist )
{

  double content = 0;
  for( int i=1; i<hist->GetNbinsX()+1; i++ )
    if( content < hist->GetBinContent( i ) )
      content =  hist->GetBinContent( i ) ;

  double label_size = 0.05;
  double title_size = 0.05;
  int digits = TMath::Log10( content ) ;
  if( digits > 2 )
    {
      label_size = 0.04;
      title_size = 0.04;
    }
  HistSetting( hist, label_size, title_size );  
}

template < class T >
void HistSetting( T* hist, double label_size=0.05, double title_size=0.05 )
{
  
  hist->GetXaxis()->SetLabelSize( label_size );
  hist->GetXaxis()->SetTitleSize( title_size );
  hist->GetXaxis()->CenterTitle();
  
  hist->GetYaxis()->SetLabelSize( label_size );
  hist->GetYaxis()->SetTitleSize( title_size );
  //hist->GetYaxis()->SetTitleOffset( 1. );
  hist->GetYaxis()->CenterTitle();
  
  hist->GetZaxis()->SetLabelSize( label_size );
  hist->GetZaxis()->SetTitleSize( title_size );
  hist->GetZaxis()->CenterTitle();  
}
