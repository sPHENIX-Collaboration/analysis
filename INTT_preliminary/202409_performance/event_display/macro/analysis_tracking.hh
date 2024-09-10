#pragma once

#include "include_tracking/least_square2.cc"
//#include "track_pT.hh"
#include "track.hh"
#include "truth.hh"

#include "cluster.hh"
#include "clustEvent.hh"
int n_dotracking = 0;

TCanvas *c;
TH2F *h_dphi_nocut;

#include "DoTracking.hh"

template < class D >
void erase(vector < D > &vec)
{
  vec.erase(vec.begin(), vec.end());
}

template < class D >
void reverse(vector < D > &vec)
{
  reverse(vec.begin(), vec.end());
}

