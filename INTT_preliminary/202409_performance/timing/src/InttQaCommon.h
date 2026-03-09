#pragma once

#include <utility>
using namespace std;
namespace InttQa
{
  /*!
    @brief A common header file for INTT QA
   */
  // common variables
  const int kFelix_num = 8; //! the number of our FELIX server
  const int kFee_num   = 14;  //! the number of half-ladders in a single FELIX server
  const int kChip_num  = 26; //! the number of chip in a half-ladder
  const int kChan_num  = 128; //! the number of channel in a single chip
  const int kFirst_pid = 3001; //! the first pid (packet ID), which means intt0
  const int kBco_max   = 128;

  const int kColors[10] = {
    kBlack,    kRed,       kBlue, 
    kGreen+2,  kMagenta+1, kYellow+1, 
    kCyan+1,   kOrange+1,  kBlue+9, 
    kGray + 2
  }; //! A list of nice colors

  // functions depending on variables above

  //! It returns int, which can be used as color in ROOT, for num-th graph/histograms. It checks if given parameter is in the range or not.
  template < class aaa >
  int GetColor( aaa num )
  {
    assert( 0 <= num && num < 10 );
    return InttQa::kColors[ num ];
  }

  //! Some configuration is done for a better-looking histogram
  template < class TH >
  void HistConfig( TH* hist, int index=0 )
  {
    hist->SetLineColor( InttQa::GetColor(index) );
    hist->SetFillColorAlpha( hist->GetLineColor(), 0.1 );
  }

} // end of namespace InttQa
