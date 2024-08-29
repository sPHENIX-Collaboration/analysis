#pragma once

#include "DrawPaletteAxis.hh"

//! General configuration of TH2D
void ConfigureHist( TH2D* hist, int mode );

//! Date of execution is returned (MM/DD/YYYY)
string GetDate();

//! Main function
void DrawHitNumCorrelation( TCanvas* c, TH2D* hist, int mode, bool is_preliminary = false );

#ifndef HitNumCorrelation_cc
#include "DrawHitNumCorrelation.cc"
#endif
