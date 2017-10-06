#ifndef __DUALRICH_ANALYZER_H_
#define __DUALRICH_ANALYZER_H_

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <cmath>

using namespace std;

//////////////////////////////////eic_dual_rich class/////////////////////////////////////////////

class eic_dual_rich {

 private:

 public:

  double cx;  
  double cy;
  double cz;
  double R_mirror;
  double refidx1;
  double refidx2;
  double sx,sy,sz;  //coordinates of the photon hit on the mirror provided by IRT

  vector<double> ch_vector; //vector of cherenkov angles

  double ind_ray(double Ex, double Ey, double Ez, double Dx, double Dy, double Dz, double vx, double vy, double vz, int select_radiator=2); //Indirect Ray Tracing

  void set_mirror(double center_posx, double center_posy, double center_posz, double radius); //set mirror parameters
  void set_radiator_one(double mean_refraction_index1); //set index of refraction, first radiator
  void set_radiator_two(double mean_refraction_index2); //set index of refraction, second radiator

  void fill_cherenkov_array(double angle); //fill ch_vector
  void cut_cherenkov_array(double theta_min, double theta_max); //cut ch_vector
  double mean_cherenkov_angle(); //mean value of ch_vector
  double SD_cherenkov_angle(); //SD of ch_vector
  void clear_cherenkov_array(); //clear ch_vector

};

#endif



