#ifndef INTT_CALO_ANA_H
#define INTT_CALO_ANA_H

#include <string>

string idatafile; // input data file (INTT-Calo nDST)
int NmaxEvent;    // # of events in the data file
string rootfile;  // output ROOT file

// imode==1 : run AnalyzeEvent()
// imode==2 : Event Display
int imode = 2;   

//pointer to the TTree for the data in nDST
TTree *evtTree;   //Global event inforation
TTree *caloTree;  //EMCal clusters
TTree *topoTree;  //topoClusters (EMC+iHCal+oHCal)
TTree *SiClusTree;    //Si Cluster assiciated with SiSeed
TTree *SiClusAllTree; //All Si Clusters
TTree *trackTree;     //SiSeed tracks. Now connected to EMCal
TTree *truthTree; // Simulation only. Truth info

// data in evtTree (Event information)
Int_t evt;
float xgvtx;  //Global vertex X
float ygvtx;  //Global vertex Y
float zgvtx;  //Global vertex Z
ULong64_t  scaledtrig;  //scaled trigger

// data in caloTree (EMCal only Clusters)
vector<float> *energy=nullptr;    //energy of EMCal cluster
vector<float> *emc_phi=nullptr;   //phi of EMCal cluster center
vector<float> *emc_eta=nullptr;   //eta of EMCal cluster center
vector<float> *emc_r=nullptr;     //r of EMCal cluster center. about 100cm)
vector<float> *emc_z  =nullptr;   //z of EMCal cluster center. 
vector<float> *emc_prob = nullptr; //prob. 1 = photon/electron like; 0= hadron like

// data of EMCal towers of EMCal clusters
vector<vector<int>>   *emc_tower_ieta = nullptr;
vector<vector<int>>   *emc_tower_iphi = nullptr;
vector<vector<float>> *emc_tower_e = nullptr;  //energy in tower
vector<vector<float>> *emc_tower_x = nullptr;  //(x,y,z) of tower
vector<vector<float>> *emc_tower_y = nullptr;
vector<vector<float>> *emc_tower_z = nullptr;
vector<vector<float>> *emc_tower_r = nullptr;  //(r,phi,eta) of tower. For eta, zvtx=0 is assumed
vector<vector<float>> *emc_tower_eta = nullptr;
vector<vector<float>> *emc_tower_phi = nullptr;
vector<vector<float>> *emc_tower_time = nullptr; 


// data in topoTree (cluster of EMCal+iHCal+oHCal)
vector<float> *top_e  =nullptr;   //total energy of cluster   
vector<float> *top_x  =nullptr;   //(x,y,z) of cluster center
vector<float> *top_y  =nullptr;
vector<float> *top_z  =nullptr;
vector<float> *top_r  =nullptr;   //(r,phi,eta) of cluster center
vector<float> *top_phi=nullptr;
vector<float> *top_eta=nullptr;
vector<float> *top_emc_e  =nullptr;  //emcal part of energy
vector<float> *top_ihc_e  =nullptr;  //ihcal part of energy 
vector<float> *top_ohc_e  =nullptr;  //ohcal part of energy

// data in SiClustTree  (Cluster of Si (MVTX and INTT) associated with SiSeed
vector<int> *layer=nullptr;  //MVTX: 0,1,2   INTT0:3,4  INTT1:5,6
vector<int> *trkid=nullptr;
vector<float> *si_x=nullptr; //(x,y,z) of cluster position
vector<float> *si_y=nullptr;
vector<float> *si_z=nullptr;

// data in SiClustAllTree (Cluster of all Si clusters)
vector<int> *a_layer=nullptr;
//vector<int> *a_trkid=nullptr;
vector<float> *a_si_x=nullptr;
vector<float> *a_si_y=nullptr;
vector<float> *a_si_z=nullptr;
vector<float> *a_si_t=nullptr;

// data in trackTree (extended and connected to EMC)
// since there are now 48 entries, I added vtrk_ at 
// the beginning to avoid the name conflict
vector<float> *vtrk_x0   = nullptr;  //x of silicon seed at starting point
vector<float> *vtrk_y0   = nullptr;  //y of silicon seed at starting point
vector<float> *vtrk_z0   = nullptr;  //z of silicon seed at starting point
vector<float> *vtrk_px0  = nullptr;  //px of silicon seed at (x0,y0,z0)
vector<float> *vtrk_py0  = nullptr;  //py of silicon seed at (x0,y0,z0)
vector<float> *vtrk_pz0  = nullptr;  //pz of silicon seed at (x0,y0,z0)
vector<float> *vtrk_eta0 = nullptr;  //eta of silicon seed at (x0,y0,z0)
vector<float> *vtrk_phi0 = nullptr;  //phi of silicon seed at (x0,y0,z0)
vector<float> *vtrk_pt0  = nullptr;  //pt of silicon seed at (x0,y0,z0)
vector<float> *vtrk_chi2ndf = nullptr; //chi2/NDF
vector<float> *vtrk_dxy  = nullptr;  // 2D DCA
vector<float> *vtrk_dz   = nullptr;  // z0 - zgvtx (or zgvtx - z0)
vector<int>   *vtrk_charge = nullptr;// charge of silicon seed
vector<int>   *vtrk_nmaps = nullptr; // # of associated Mvtx clusters
vector<int>   *vtrk_nintt = nullptr; // # of associated INTT clusters
vector<int>   *vtrk_innerintt = nullptr; // # of associated INTT0 clusters
vector<int>   *vtrk_outerintt = nullptr; // # of associated INTT1 clusters
vector<float> *vtrk_x_proj_emc = nullptr;// x of silicon seed projected at Emc
vector<float> *vtrk_y_proj_emc = nullptr;// y of silicon seed projected at Emc
vector<float> *vtrk_z_proj_emc = nullptr;// z of silicon seed projected at Emc
vector<float> *vtrk_px_proj_emc = nullptr;// px of silicon seed at Emc
vector<float> *vtrk_py_proj_emc = nullptr;// py of silicon seed at Emc
vector<float> *vtrk_pz_proj_emc = nullptr;// pz of silicon seed at Emc
vector<float> *vtrk_eta_proj_emc = nullptr;//eta of silicon seed projected at Emc
vector<float> *vtrk_phi_proj_emc = nullptr;//phi of silicon seed projected at Emc
vector<float> *vtrk_pt_proj_emc  = nullptr;//pt of silicon seed projected at Emc
vector<int>   *vtrk_track_id_truth = nullptr;//MC truth track ID (ID of PHG4Partilce)
vector<float> *vtrk_sicalo_pt  = nullptr;  // improved pt of Si-Emc track
vector<float> *vtrk_sicalo_phi = nullptr;  // same as phi0
vector<float> *vtrk_sicalo_eta = nullptr;  // same as eta0
vector<float> *vtrk_sicalo_dphi = nullptr; // phi difference between Emc and Seed projection
vector<float> *vtrk_sicalo_dz   = nullptr; // z difference between Emc and Seed projection
vector<float> *vtrk_sicalo_emc_x = nullptr;// x of associated Emc cluster
vector<float> *vtrk_sicalo_emc_y = nullptr;// y of associated Emc cluster
vector<float> *vtrk_sicalo_emc_z = nullptr;// z of associated Emc cluster
vector<float> *vtrk_sicalo_emc_r = nullptr;// r of associated Emc cluster
vector<float> *vtrk_sicalo_emc_phi = nullptr;// phi of associated Emc cluster
vector<float> *vtrk_sicalo_emc_energy = nullptr;// energy of associated Emc cluster
vector<float> *vtrk_sicalo_emc_chi2 = nullptr;  // chi2 of associated Emc cluster
vector<float> *vtrk_sicalo_emc_prob = nullptr;  // prob of associated Emc cluster

// SIM truth info
// truth info of particle in r<5.0 cm
vector<int> *truth_pid = nullptr;
vector<int> *truth_id  = nullptr;
vector<float> *truth_px = nullptr;   //(px,py,pz) of particle at the origin
vector<float> *truth_py = nullptr;
vector<float> *truth_pz = nullptr;
vector<float> *truth_x = nullptr;    //(x,y,z) of particle at the origin
vector<float> *truth_y = nullptr;
vector<float> *truth_z = nullptr;
vector<float> *truth_e = nullptr;    //energy of particle a the origin
vector<float> *truth_pt = nullptr;   //(pt,eta,phi)
vector<float> *truth_eta = nullptr;
vector<float> *truth_phi = nullptr;
vector<float> *truth_primary_id = nullptr;
vector<float> *truth_parent_id = nullptr;
vector<float> *truth_vtxid = nullptr;  //vertex position
vector<float> *truth_vtx_x = nullptr;
vector<float> *truth_vtx_y = nullptr;
vector<float> *truth_vtx_z = nullptr;



int i_event;   // current event that is analyzed/displayed
// functions

void INTT_Calo_Ana(string idatafile,int imode,
		   string rootfile,
		   float xBC, float yBC,
		   int istart, int nevent);

#endif
