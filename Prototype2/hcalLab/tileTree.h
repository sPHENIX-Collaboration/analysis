#ifndef __HCALLABTREE_H__
#define __HCALLABTREE_H__

class hcalLabTree
{

public:

  hcalLabTree() {
    runnumber   = 0;
    eventnumber = 0;
  } 
  hcalLabTree(int rn){
    runnumber = rn;
  }

  ~hcalLabTree() {
    ;
  } 

  void clean(){
    for(int ch = 0; ch<NCH; ch++) {
      pedestal[ch] = 0.;
      fpeak[ch]    = 0.;
      ftime[ch]    = 0.;
    }
  };
  void init(int rn);
public:
  int runnumber;
  int eventnumber;
  TString             hcLTFName;
  TFile             * fhcl;
  TTree             * thcl;

  //  EK:  reject codes
  //           reject[0] - Calorimeter
  //   1      overflows in calorimeter amplitudes
  //   10     tags outside range
  //   100    HCal time is unreliable
  //   1000   
  //   10000  Total energy in event is consistent with ZERO
  int reject;

  // timing
  float hitsample;
  float hittime;

  
  int   adc     [NCH][NSAMPLES];
  float pedestal[NCH];
  float fpeak   [NCH];
  float ftime   [NCH];

  float fsum    [NCH/2];    //  uncalibrated ADC sum from two fiber ends
  float fcsum   [NCH/2];    //  calibrated ASDC sum from two fiber ends
  float asym    [NCH/2];
  float ly      [NCH];      //  light yield from fiber ends
  float fly     [NCH/2];    //  light yield per fiber
  float tly                 //  total light yield per tile per event

  float tileXCG, tileYCG;


  int summary;
};

#endif /* __HCALLABTREE_H__ */
