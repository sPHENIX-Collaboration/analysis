#ifndef __SETUPDUALRICHANALYZER_H_
#define __SETUPDUALRICHANALYZER_H_

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <cmath>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4hough/SvtxTrack_FastSim.h>

class eic_dual_rich;

using namespace std;


// SetupDualRICHAnalyzer class //

class SetupDualRICHAnalyzer {

 private:

 public:

  SetupDualRICHAnalyzer();

  double calculate_emission_angle( double m_emi[3], double momv[3], PHG4Hit * hit ); // Calculate emission angle from IRT

  bool get_position_from_track_state( SvtxTrack_FastSim * track, string statename, double arr_pos[3] ); // Get mean track position
  bool get_momentum_from_track_state( SvtxTrack_FastSim * track, string statename, double arr_mom[3] ); // Get momentum of track
  bool get_true_momentum( PHG4TruthInfoContainer * truthinfo, SvtxTrack_FastSim * track, double arr_mom[3] ); // Get truth momentum of track
  bool get_emission_momentum( PHG4TruthInfoContainer * truthinfo, PHG4HitContainer * richhits, SvtxTrack_FastSim * track, double arr_mom[3] ); // Get momentum from emission points

  /* analyzer object */
  eic_dual_rich *_analyzer;

};

#endif
