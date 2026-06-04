#ifndef INTT_CALO_TRKRECO_H
#define INTT_CALO_TRKRECO_H

#include <vector>

void INTT_Calo_trkReco(void);

void LoadTrackerData(void);

void Compare_Topo_and_Emc(void);

void FindInttPair(void);
void PrintInttPair(void);

void FindEmcIntt(void);
void PrintEmcIntt(void);

void FindInttMvtx(void);
void PrintInttMvtx(void);

void FindCaloInttMvtx(void);
void PrintCaloInttMvtx(void);

bool Is_Hot_Emc(int iemc);
float rz2eta(float r, float z);

//
// structs, vectors, variables, and constants for EMC-INTT-Mvtx tracking
//

float xBC=0;     //x of Beam Center
float yBC=0;     //y of Beam Center
float gzvtx;   //Global zvtx;

struct InttPair {
  int iINTT0;  //index of vINTT0r, vINTT0phi, vINTT0z
  int iINTT1;  //index of vINTT1r, vINTT1phi, vINTT1z
  float r;     // the radius of the circle of (INTT1,INTT0,BC)
  float xc;    // x of the center of the circle
  float yc;    // y of the center of the circle
};

struct CaloInttMvtx {
  int iTop;       //index of vTop_r, vTop_phi, vTop_z
  int iEmc;       //index of vEmc_r, vEmc_phi, vEmc_z
  int iINTT0;     //index in vINTT0r, vINTT0phi, vINTT0z
  int iINTT1;     //index in vINTT1r, vINTT1phi, vINTT1z
  int iMvtx0;     //index in vMvtx0r, vMvtx0phi, vMvtx0z
  int iMvtx1;     //index in vMvtx1r, vMvtx1phi, vMvtx1z
  int iMvtx2;     //index in vMvtx2r, vMvtx2phi, vMvtx2z
  
  float r_intt;   // The radius of the circle of (INTT1,INTT0,BC)
  float xc_intt;  // The center of the circle of (INTT1,INTT0,BC)
  float yc_intt;  //

  float r_emc_intt;
  float xc_emc_intt;
  float yc_emc_intt;

  float r_intt_mvtx;
  float xc_intt_mvtx;
  float yc_intt_mvtx;
};

vector<InttPair>     vInttPair;
vector<CaloInttMvtx> vCaloInttMvtx;

/*
vector<float> vEmcINTT_R;    //local radius of EmcINTT tracklet orbit (circle)
vector<float> vEmcINTT_xO;   // x of the center of the tracklet orbit
vector<float> vEmcINTT_yO;   // y of the center of the tracklet orbit
vector<float> vEmcINTT_R_EI; //local radius of EmcINTT tracklet orbit (circle)
vector<float> vEmcINTT_xO_EI;// x of the center of the tracklet orbit
vector<float> vEmcINTT_yO_EI;// y of the center of the tracklet orbit
*/

vector<float> vEmc_r;
vector<float> vEmc_phi;
vector<float> vEmc_eta;
vector<float> vEmc_z;
vector<float> vEmc_e;

vector<float> vTop_r;
vector<float> vTop_phi;
vector<float> vTop_eta;
vector<float> vTop_z;
vector<float> vTop_e;
vector<float> vTop_emc_e;
vector<float> vTop_ihc_e;
vector<float> vTop_ohc_e;

vector<float> vINTT0r;
vector<float> vINTT0phi;
vector<float> vINTT0z;

vector<float> vINTT1r;
vector<float> vINTT1phi;
vector<float> vINTT1z;

vector<float> vMvtx0r;
vector<float> vMvtx0phi;
vector<float> vMvtx0z;

vector<float> vMvtx1r;
vector<float> vMvtx1phi;
vector<float> vMvtx1z;

vector<float> vMvtx2r;
vector<float> vMvtx2phi;
vector<float> vMvtx2z;

#endif
