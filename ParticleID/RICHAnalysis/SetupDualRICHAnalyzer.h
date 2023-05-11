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

#include <g4hough/SvtxTrack.h>

class eic_dual_rich;

using namespace std;


// SetupDualRICHAnalyzer class //

class SetupDualRICHAnalyzer {

 private:

 public:

  SetupDualRICHAnalyzer();

  double calculate_emission_angle( double m_emi[3], double momv[3], PHG4Hit * hit ); // Calculate emission angle from IRT

  bool get_true_momentum( PHG4TruthInfoContainer * truthinfo, SvtxTrack * track, double arr_mom[3] ); // Get truth momentum of track
  bool get_emission_momentum( PHG4TruthInfoContainer * truthinfo, PHG4HitContainer * richhits, SvtxTrack * track, double arr_mom[3] ); // Get momentum from emission points

  /* analyzer object */
  eic_dual_rich *_analyzer;

};

#endif
