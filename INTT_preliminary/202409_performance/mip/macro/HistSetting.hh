#pragma once

template < class T >
void HistSetting( T* hist)
{
  hist->GetXaxis()->SetLabelSize( 0.05 );
  hist->GetXaxis()->SetTitleSize( 0.05 );
  hist->GetXaxis()->CenterTitle();
  
  hist->GetYaxis()->SetLabelSize( 0.05 );
  hist->GetYaxis()->SetTitleSize( 0.05 );
  hist->GetYaxis()->CenterTitle();
  
}
