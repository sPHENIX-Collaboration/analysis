#ifndef INTT_CALO_ANA_H
#define INTT_CALO_ANA_H

#include <string>

string idatafile; // input data file (INTT-Calo nDST)
int NmaxEvent;      // # of events in the data file

// imode==1 : run AnalyzeEvent()
// imode==2 : Event Display
int imode = 2;   

//pointer to the TTree for the data.0
TTree *evtTree;
TTree *caloTree;  //EMCal clusters
TTree *topoTree;  //topoClusters (EMC+iHCal+oHCal)
TTree *SiClusTree;
TTree *SiClusAllTree;
TTree *trackTree;

// data in evtTree
Int_t evt;
float xgvtx;  //Global vertex X
float ygvtx;  //Global vertex Y
float zgvtx;  //Global vertex Z

// data in caloTree
vector<float> *energy=nullptr;
vector<float> *emc_phi=nullptr;
vector<float> *emc_eta=nullptr;
vector<float> *emc_r=nullptr;
vector<float> *emc_z  =nullptr;
vector<vector<int>>   *emc_tower_ieta = nullptr;
vector<vector<int>>   *emc_tower_iphi = nullptr;
vector<vector<float>> *emc_tower_e = nullptr;
vector<vector<float>> *emc_tower_x = nullptr;
vector<vector<float>> *emc_tower_y = nullptr;
vector<vector<float>> *emc_tower_z = nullptr;
vector<vector<float>> *emc_tower_r = nullptr;
vector<vector<float>> *emc_tower_eta = nullptr;
vector<vector<float>> *emc_tower_phi = nullptr;
vector<vector<float>> *emc_tower_time = nullptr;


// data in topoTree
vector<float> *top_e  =nullptr;
vector<float> *top_x  =nullptr;
vector<float> *top_y  =nullptr;
vector<float> *top_z  =nullptr;
vector<float> *top_r  =nullptr;
vector<float> *top_phi=nullptr;
vector<float> *top_eta=nullptr;
vector<float> *emc_e  =nullptr;
vector<float> *ihc_e  =nullptr;
vector<float> *ohc_e  =nullptr;

// data in SiClustTree
vector<int> *layer=nullptr;
vector<int> *trkid=nullptr;
vector<float> *si_x=nullptr;
vector<float> *si_y=nullptr;
vector<float> *si_z=nullptr;

// data in SiClustAllTree
vector<int> *a_layer=nullptr;
//vector<int> *a_trkid=nullptr;
vector<float> *a_si_x=nullptr;
vector<float> *a_si_y=nullptr;
vector<float> *a_si_z=nullptr;
vector<float> *a_si_t=nullptr;

// data in trackTree
vector<float> *eta0=nullptr;
vector<float> *phi0=nullptr;
vector<float> *pt0=nullptr;
vector<float> *vx0=nullptr;
vector<float> *vy0=nullptr;
vector<float> *vz0=nullptr;

int i_event;   // current event that is analyzed/displayed
// functions

void INTT_Calo_Ana(void);

#endif
