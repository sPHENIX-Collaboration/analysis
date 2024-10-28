#pragma once

/*!
  @brief A event class to make the timing plot of INTT streaming readout data
  @details The format of 1st argument of the constrctor is
  #event    GL1_BCO    INTT_GTM_BCO    FPHX0 ... FPHX119
 */

class Event
{
private:
  int event_ = 0;
  uint64_t bco_gl1_ = 0;
  uint64_t bco_intt_ = 0;
  int offset_ = 0;
  
  TH1D* hist_fphx_bco_;
  TH1D* hist_fphx_bco_gl1_match_;
public:
  Event( string line, int offset );

  TH1D* GetHist(){ return hist_fphx_bco_;};
  TH1D* GetHistGl1Match(){ return hist_fphx_bco_gl1_match_;};

  int GetEventNum(){ return event_;};
  uint64_t GetGl1Bco(){ return bco_gl1_;};
  uint64_t GetInttGtmBco(){ return bco_intt_;};

  int GetFatPeakNum( int ignore_smaller_than = 0 );
  int GetHitNumMachingGl1();
  int GetHitNumMaxEvent();
  int GetGl1Timing();
  int GetGl1Bin();
  int GetBinMoreThan( int threshold = 0 );
  
  bool IsGl1Correspondings();
  bool IsHitInAbortGap();
  void Print();
};

#ifndef Event_cc
#include "Event.cc"
#endif
