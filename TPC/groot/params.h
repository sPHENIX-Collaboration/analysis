#ifndef __params_H__
#define __params_H__

#include <vector>
#include <string>

#define Nr    16
#define Nphi 128

#define Nhybrid 14 //we killed one of the sectors as it was sparking to the GEM due to ground issues. 
#define CHhybrid 128
#define Nsrs Nhybrid*CHhybrid

// Data type to define the zigzag shape...
  struct CheveronPad_t 
  {
    std::string fName;
    double fPolyWidth;
    int fPolyLayer;
    std::vector<double> fX;
    std::vector<double> fY;
    std::vector<double> fVia;
    std::vector<double> fWire;
    std::vector<double> r;
    std::vector<double> phi;
    std::vector<int> ID;
  };

#endif /* __params_H__ */
