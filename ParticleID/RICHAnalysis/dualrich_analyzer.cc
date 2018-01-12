/////////Code developer Alessio Del Dotto/////////
////////Compilation and usage in ROOT/////////
//.L dualrich_analyzer.cpp++
//eic_dual_rich *a = new eic_dual_rich()
//a->some method of the class

#include "dualrich_analyzer.h"

using namespace std;


void eic_dual_rich::set_mirror(double center_posx, double center_posy, double center_posz, double radius){

  cx = center_posx;
  cy = center_posy;
  cz = center_posz;
  R_mirror = radius;

}

void eic_dual_rich::set_radiator_one(double mean_refraction_index1){

  refidx1 = mean_refraction_index1;

}

void eic_dual_rich::set_radiator_two(double mean_refraction_index2){

  refidx1 = mean_refraction_index2;

}

double eic_dual_rich::ind_ray(double Ex, double Ey, double Ez, double Dx, double Dy, double Dz, double vx, double vy, double vz, int select_radiator){

  //int sel_radiator = 2;

  //if(select_radiator == 1) sel_radiator = 1;

  double cex,cey,cez;
  double cdx,cdy,cdz;

  int i;//,iwhere;

  double th,a,d;
  double x,dx;

  double y,y1;

  double eps = 0.00000000001;
  double R = 0;  

  R = R_mirror;

  double esx, esy, esz, es;
  double ref_frac, theta1, theta2;

  double theta_c = 0.;

  cex = -cx+Ex;
  cey = -cy+Ey;
  cez = -cz+Ez;

  cdx = -cx+Dx;
  cdy = -cy+Dy;
  cdz = -cz+Dz;

  //a = TMath::Sqrt(cex*cex+cey*cey+cez*cez);
  //d = TMath::Sqrt(cdx*cdx+cdy*cdy+cdz*cdz);
  //th = TMath::ACos((cdx*cex+cdy*cey+cdz*cez)/a/d);

  a = sqrt(cex*cex+cey*cey+cez*cez);
  d = sqrt(cdx*cdx+cdy*cdy+cdz*cdz);
  th = acos((cdx*cex+cdy*cey+cdz*cez)/a/d);

  i = 0;
  x = th/2.;  
  y = R*(a*sin(x)-d*sin(th-x))+a*d*sin(th-2*x);
  y1 = R*(a*cos(x)+d*cos(th-x))-2*a*d*cos(th-2*x);
  dx = -1*y/y1;

  while(abs(dx)>eps && i<100){

    x+=dx;
    y = R*(a*sin(x)-d*sin(th-x))+a*d*sin(th-2*x);
    y1 = R*(a*cos(x)+d*cos(th-x))-2*a*d*cos(th-2*x);
    dx = -1*y/y1;
    i++;

  }

  if(i>=100) cout<<"Not convergent"<<endl;

  if(i<100){
    sx = cx + (R*cos(x)/a-R*sin(x)/tan(th)/a)*cex + (R*sin(x)/d/sin(th))*cdx;
    sy = cy + (R*cos(x)/a-R*sin(x)/tan(th)/a)*cey + (R*sin(x)/d/sin(th))*cdy;
    sz = cz + (R*cos(x)/a-R*sin(x)/tan(th)/a)*cez + (R*sin(x)/d/sin(th))*cdz;
  }

  esx = sx - Ex;
  esy = sy - Ey;
  esz = sz - Ez;

  es = sqrt(esx*esx+esy*esy+esz*esz);

  esx = esx/es;
  esy = esy/es;
  esz = esz/es;

  if(select_radiator == 1){
    ref_frac = refidx2/refidx1;
  
    theta2 = acos(esz);
    theta1 = asin(sin(theta2)*ref_frac);
    
    esx = esx*sin(theta1)/sin(theta2);
    esy = esy*sin(theta1)/sin(theta2);
    esz = cos(theta1);
  }
  theta_c = acos((esx*vx+esy*vy+esz*vz));

  return theta_c;

}

void eic_dual_rich::fill_cherenkov_array(double angle){

  ch_vector.push_back (angle);

}

void eic_dual_rich::cut_cherenkov_array(double theta_min, double theta_max){

  vector<double> cut_vector;

  for (unsigned int i=0; i<ch_vector.size(); i++){

    if(theta_min<theta_max && ch_vector.at(i)>=theta_min && ch_vector.at(i)<=theta_max) cut_vector.push_back (ch_vector.at(i));
    else if (theta_min>=theta_max) cout<<"Applied cut wrong: theta_min>theta_max"<<endl;

  }

  ch_vector.clear();

  for (unsigned int i=0; i<cut_vector.size(); i++){

    ch_vector.push_back(cut_vector.at(i));

  }

}

double eic_dual_rich::mean_cherenkov_angle(){

  double sum = 0.;

  for (unsigned int i=0; i<ch_vector.size(); i++){

    sum += ch_vector.at(i);

  }

  sum = sum/ch_vector.size();

  return sum;

}

double eic_dual_rich::SD_cherenkov_angle(){

  double sum = 0.;

  for (unsigned int i=0; i<ch_vector.size(); i++){

    sum += (ch_vector.at(i)-mean_cherenkov_angle())*(ch_vector.at(i)-mean_cherenkov_angle());

  }

  sum = sum/ch_vector.size();

  return sqrt(sum);

}

void eic_dual_rich::clear_cherenkov_array(){

  ch_vector.clear();

}




