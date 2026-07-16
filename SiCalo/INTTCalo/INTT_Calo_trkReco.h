#ifndef INTT_CALO_TRKRECO_H
#define INTT_CALO_TRKRECO_H

//#include <vector>

void INTT_Calo_trkReco_Init(void);
void INTT_Calo_trkReco_End(void);
void INTT_Calo_trkReco(void);

bool Is_Hot_Emc(int iemc);
float rz2eta(float r, float z);

void LoadTrackerData(void);

void Connect_Topo_and_Emc(void);

void CalcCircle2(float x1,float y1,float x0, float y0, float &r, float &xc, float &yc);

void CalcCircle(double x0, double y0, double x1,double y1, double x2, double y2,
		float &rs, float &xsc, float &ysc);

float Calc_Remc(float xe, float ye, float x1,float y1, float x2, float y2,
		float xc, float yc);

struct InttPair;
void FindInttPair(void);
void RemoveDuplicatedPair(InttPair &pair_i,InttPair &pair_j);
void PrintInttPair(void);

void FindCaloIntt(void);

void FindInttMvtx(void);

int FindMvtxHit(int layer, float rIntt, float xc, float yc,
		float zvtx, float r0, float r1, float z0, float z1,
		vector<int> &vMvtxHit);

void FormInttMvtxTrk(float r0, float phi0, float z0,float r1, float phi1,float z1,
		     int iMvtx0,int iMvtx1,int iMvtx2,
		     float &r_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		     float& zvtx_intt_mvtx,
		     float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m);

void FormInttMvtxTrk(float r0, float phi0, float z0,float r1, float phi1,float z1,
		     vector<int> &vMvtx0Hit, vector<int> &vMvtx1Hit,vector<int> &vMvtx2Hit,
		     int &iMvtx0, int &iMvtx1, int &iMvtx2,
		     float &r_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		     float& zvtx_intt_mvtx,
		     float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m);

void FormInttMvtxTrk1(float x0, float y0, float z0,float x1, float y1,float z1,
		      vector<int> &vMvtxHit,
		      vector<float> &vMvtx_r, vector<float> &vMvtx_phi, vector<float> &vMvtx_z,
		      int &iMvtxHit,
		      float &r_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		      float& zvtx_intt_mvtx,
		      float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m);

void FormInttMvtxTrk2(float x0, float y0, float z0,float x1, float y1,float z1,
		      vector<int> &vMvtxHit_I, vector<int> &vMvtxHit_O,
		      vector<float> &vMvtxI_r, vector<float> &vMvtxI_phi, vector<float> &vMvtxI_z,
		      vector<float> &vMvtxO_r, vector<float> &vMvtxO_phi, vector<float> &vMvtxO_z,
		      int &iMvtx_I, int &iMvtx_O,
		      float &r_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		      float& zvtx_intt_mvtx,
		      float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m);

void FormInttMvtxTrk3(float x0, float y0, float z0,float x1, float y1,float z1,
		      vector<int> &vMvtx0Hit, vector<int> &vMvtx1Hit,vector<int> &vMvtx2Hit,
		      int &iMvtx0, int &iMvtx1, int &iMvtx2,
		      float &r_intt_mvtx, float &xc_intt_mvtx, float &yc_intt_mvtx,
		      float& zvtx_intt_mvtx,
		      float &x0m, float &y0m, float &z0m, float &pt0m,float& phi0m, float &pz0m);

int SelectBestMvtxHit(vector<int> &vMvtxhit, vector<float> &vMvtx_dr,
		      vector<float> &vMvtx_dz0, vector<float> &vMvtx_dz1);

void FindCaloInttMvtx(void);

int SelectBestTopHit(vector<int>& vTop_near_trk);

// structs, vectors, variables, and constants for EMC-INTT-Mvtx tracking
//

float xBC=0;     //x of Beam Center
float yBC=0;     //y of Beam Center
float gzvtx;     //Global zvtx;

const float R_Emc = 103.5;
const float Top_E_min = 0.2; //Minimun Calo energy [GeV].

struct InttPair {
  int iINTT0;  //index of vINTT0r, vINTT0phi, vINTT0z
  int iINTT1;  //index of vINTT1r, vINTT1phi, vINTT1z
  float r;     // the radius of the circle of (INTT1,INTT0,BC)
  float xc;    // x of the center of the circle
  float yc;    // y of the center of the circle
  float z;     // z of INTT0;
  float zvtx;  // zvtx projection from INTT1 and INTT0
};

vector<InttPair>     vInttPair;

struct CaloInttMvtx {
  int iTop;       //index in vTop_r, vTop_phi, vTop_z etc (best)
  int iINTT0;     //index in vINTT0r, vINTT0phi, vINTT0z
  int iINTT1;     //index in vINTT1r, vINTT1phi, vINTT1z
  int iMvtx0;     //index in vMvtx0r, vMvtx0phi, vMvtx0z (best)
  int iMvtx1;     //index in vMvtx1r, vMvtx1phi, vMvtx1z (best)
  int iMvtx2;     //index in vMvtx2r, vMvtx2phi, vMvtx2z (best)

  int sign;       // charge sign

  float r_intt;   // The radius of the circle of (INTT1,INTT0,BC)
  float xc_intt;  // The center of the circle of (INTT1,INTT0,BC)
  float yc_intt;  //
  float z_intt;   // z of intt tracklet. z of INTT0.
  float zvtx_intt;//
  
  float r_emc_intt;  //The radius of the circle of (EMC-INTT1-INTT0)
  float xc_emc_intt; //The center of the circle
  float yc_emc_intt;
  float z_emc_intt;
  float zvtx_emc_intt;// zvtxt projection of the EMC-INTT tracklet

  float r_intt_mvtx;
  float xc_intt_mvtx;
  float yc_intt_mvtx;
  float zvtx_intt_mvtx;

  // Emc cluser center
  float xemc;
  float yemc;
  float zemc;
  
  // energy of EMC/HCAL cluster pointed to by this track
  float total_e;   //total_e=emcal_e+ihcal_e+ohcal_e
  float emcal_e;   //emcal part of topo cluster energy
  float ihcal_e;   //ihcal part of topo cluster energy
  float ohcal_e;   //ohcal part of topo cluster energy
  float emc_e;     //energy of emcal cluster associated with topocluster

  // eID parameter
  float BDT_e;     // BDT socre for electron ID
  
  // EMC-INTT based track information.
  float x0e;    // (x0,y0,z0) of the track starting point
  float y0e;
  float z0e;
  float pt0e;   // (pt0, pt0, pz0) at (x0,y0,z0);
  float phi0e;
  float pz0e;

  // Mvtx-INTT based track information.
  float x0m;    // (x0,y0,z0) of the track starting point
  float y0m;
  float z0m;
  float pt0m;   // (pt0, pt0, pz0) at (x0,y0,z0);
  float phi0m;
  float pz0m;
};

vector<CaloInttMvtx> vCaloInttMvtx;
CaloInttMvtx a_CaloInttMvtx;


// Emc clusters
vector<float> vEmc_r;
vector<float> vEmc_phi;
vector<float> vEmc_eta;
vector<float> vEmc_z;
vector<float> vEmc_e;

// TopoClusters
vector<float> vTop_r;
vector<float> vTop_phi;
vector<float> vTop_eta;
vector<float> vTop_z;
vector<float> vTop_e;
vector<float> vTop_emc_e;
vector<float> vTop_ihc_e;
vector<float> vTop_ohc_e;

// Emc Cluster info associated with TopCluster
vector<float> vTop_emc_r;
vector<float> vTop_emc_phi;
vector<float> vTop_emc_eta;
vector<float> vTop_emc_z;
vector<float> vTop_emc_emc_e;

// INTT0 and INTT1 clusters
vector<float> vINTT0r;
vector<float> vINTT0phi;
vector<float> vINTT0z;
vector<float> vINTT0t;

vector<float> vINTT1r;
vector<float> vINTT1phi;
vector<float> vINTT1z;
vector<float> vINTT1t;

// Mvtx0, Mvtx1, Mvtx2 clusters
vector<float> vMvtx0r;
vector<float> vMvtx0phi;
vector<float> vMvtx0z;
vector<float> vMvtx0t;

vector<float> vMvtx1r;
vector<float> vMvtx1phi;
vector<float> vMvtx1z;
vector<float> vMvtx1t;

vector<float> vMvtx2r;
vector<float> vMvtx2phi;
vector<float> vMvtx2z;
vector<float> vMvtx2t;

//
// Histograms to monitor/diagonize the program
//
//Histograms
TH1F *hzgvtx;
TH1F *hINTTdphi;
TH1F *hINTTdZvtx;
TH1F *hINTTdz0;
TH1F *hINTTdz1;
TH2F *hINTTdz0dz1;
TH2F *hINTTdz0dz1r;
TH1F *hINTTdz_r;
TH1F *hInttPair_r;
TH1F *hEmcIntt_dr;
TH1F *hEmcIntt_dr_Zcut;
TH1F *hEmcIntt_dr_Zvtx2;
TH1F *hEmcIntt_dr_Zvtx4;
TH1F *hEmcIntt_dr_Zvtx5;
TH1F *hdZe1;
TH1F *hdZe0;
TH1F *hTopEmc_dphi;
TH1F *hTopEmc_deta;
TH1F *hTopEmc_dphi_1;
TH1F *hTopEmc_dphi_2;
TH1F *hTopEmc_dphi_3;
TH1F *hTopEmc_deta_1;
TH1F *hTopEmc_deta_2;
TH1F *hTopEmc_deta_3;

TH1F *hZvtxTrk;
TH1F *hZvtxTrk2;
TH1F *hZvtxTrk3;
TH1F *hZvtxTrk4;
TH1F *hdZvtxTrk;
TH1F *hdZvtxTrk2;
TH1F *hdZvtxTrk3;
TH1F *hdZvtxTrk4;
TH1F *hTopE;
TH1F *hp_Trk;
TH1F *hptTrk;
TH1F *hTopE_p;
TH1F *hEmcE_p;
TH1F *hTopE_p_Zvtx;
TH1F *hEmcE_p_Zvtx;

TH1F *hrIntt_rsEmc;
TH1F *hrIntt_rsEmc2;
TH1F *hrIntt_rsEmc3;

TH1F *hMvtx0_dr;
TH1F *hMvtx1_dr;
TH1F *hMvtx2_dr;
TH1F *hMvtx0_dz0;
TH1F *hMvtx0_dz1;
TH1F *hMvtx1_dz0;
TH1F *hMvtx1_dz1;
TH1F *hMvtx2_dz0;
TH1F *hMvtx2_dz1;
TH1F *hMvtx0_dr2;
TH1F *hMvtx1_dr2;
TH1F *hMvtx2_dr2;

TH1F *hdphi_ij;
TH1F *hdzvtx_ij;
TH1F *hdphi_ij2;
#endif
