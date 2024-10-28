#pragma once

class Analysis
{
private:
  int run_no_;
  bool mag_on_;
  bool debug_;
  
  int fphx_bco_ = -1;
  
  int sigma_ = 3;
  int nloop_ = 50;
  int page_num_limit_ = 500;
  int good_counter_ = 0;
  
  bool is_geant_ = false;
  //bool does_reverse_ = false;

  /////////////////////////////////////////////////////////////////////////
  // Variables/Objects for I/O                                           //
  /////////////////////////////////////////////////////////////////////////
  //  string data_dir_ = "/sphenix/u/nukazuka/work_now/analysis/tracking/hinakos/work/F4AInttRead/macro/results/";
  string data_dir_ = "./results/";
  string fname_input_; 
  
  string dir_output_ = "results/tracking/";
  TString fname_output_;
  TFile *f_input_;
  TFile *f_output_; 
  string output_pdf_ = "";
  string output_good_pdf_ = "";
  
  /////////////////////////////////////////////////////////////////////////
  // Histograms                                                          //
  /////////////////////////////////////////////////////////////////////////
  TH1F *h_dcaz_one_ ;
  TH1F *h_nclus_ ;
  TH2F* h_dphi_nocut_ ;
  TH1F *h_dcaz_sigma_one_ ;
  TH1D *h_xvtx_ ;
  TH1D *h_yvtx_ ;
  TH1D *h_zvtx_ ;
  //  TH2D *h_zr_[2];

  TH1F *h_dcax_ ;
  TH1F *h_dcay_ ;
  TH1F *h_dcaz_ ;
  TH1F *h_dtheta_ ;
  TH1F *h_dphi_ ;
  TH1F *h_dca2d_ ;

  /////////////////////////////////////////////////////////////////////////
  // Trees                                                               //
  /////////////////////////////////////////////////////////////////////////
  TTree *temp_tree_ ;
  TTree *clus_tree_ ;
  TTree *truth_tree_ ;
  TTree *track_tree_ ;
  TTree *hepmctree_;

  TNtuple *ntp_clus_;
  TNtuple *ntp_evt_; 

  /////////////////////////////////////////////////////////////////////////
  // Tree variables                                                      //
  /////////////////////////////////////////////////////////////////////////
  TVector3 vertex_;
  double x_vertex_;
  double y_vertex_;
  double z_vertex_;

  // variables for temp_tree_
  int evt_temp_;
  vector<double> d_phi_;
  vector<double> d_theta_;
  vector<double> dca_x_;
  vector<double> dca_y_;
  vector<double> dca_z_;
  vector<double> dca_2d_;
  double zvtx_one_;
  double zvtx_sigma_one_;

  // variables for ?
  int evt_clus_;
  vector<double> x_in_;
  vector<double> y_in_;
  vector<double> z_in_;
  vector<double> r_in_;
  vector<int> size_in_;
  vector<double> phi_in_;
  vector<double> theta_in_;
  vector<double> adc_in_;
  vector<bool>   is_associated_in_;
  vector<double> track_incoming_theta_in_;
  vector<double> x_out_;
  vector<double> y_out_;
  vector<double> z_out_;
  vector<double> r_out_;
  vector<int> size_out_;
  vector<double> phi_out_;
  vector<double> theta_out_;
  vector<double> adc_out_;
  vector<bool>   is_associated_out_;
  vector<double> track_incoming_theta_out_;  

  // variables for ntp_evt_
  float vertex_z_ = -9999;
  
  int evt_track_;
  int ntrack_ = 0;
  vector<double> slope_rz_;
  vector<double> phi_tracklets_;
  vector<double> theta_tracklets_;
  vector<double> phi_track_;
  vector<double> theta_track_;
  vector<double> z_tracklets_out_;
  vector<int> is_deleted_;
  vector<int> dca_range_out_;
  vector<int> dca2d_range_out_;
  vector<int> dcaz_range_out_;
  vector<double> pT_;
  vector<double> px_truth_;
  vector<double> py_truth_;
  vector<double> pz_truth_;
  vector<double> px_;
  vector<double> py_;
  vector<double> pz_;
  vector<double> rad_;
  vector<double> pT_truth_;
  vector<double> charge_;

  int evt_truth_;
  int ntruth_ = 0;
  int ntruth_cut_ = 0;
  vector<double> truthpx_;
  vector<double> truthpy_;
  vector<double> truthpz_;
  vector<double> truthpt_;
  vector<double> trutheta_;
  vector<double> truththeta_;
  vector<double> truthphi_;
  vector<double> truthxvtx_;
  vector<double> truthyvtx_;
  vector<double> truthzvtx_;
  vector<double> truthzout_;
  vector<int> truthpid_;
  vector<int> status_;

  // from input file
  
  bool no_mc_;

  Float_t nclus_;
  Float_t nclus2_;
  Float_t bco_full_;
  Float_t evt_;
  Float_t size_;
  Float_t adc_;
  Float_t x_;
  Float_t y_;
  Float_t z_;
  Float_t lay_;
  Float_t lad_;
  Float_t sen_;
  Float_t x_vtx_;
  Float_t y_vtx_;
  Float_t z_vtx_;

  double m_truthpx_;
  double m_truthpy_;
  double m_truthpz_;
  double m_truthpt_;
  double m_trutheta_;
  double m_truththeta_;
  double m_truthphi_;
  double m_xvtx_;
  double m_yvtx_;
  double m_zvtx_;
  
  int m_evt_;
  int m_status_;
  int m_truthpid_;
  
  /////////////////////////////////////////////////////////////////////////
  // Misc variables/objects                                              //
  /////////////////////////////////////////////////////////////////////////
  TCanvas* c_;
  int page_counter_ = 0;

  void Init();
  void InitIO();
  void InitInput();
  void InitOutput();
  void InitHist();
  void InitTree();
  void InitCanvas();
  
  void ResetVariables();
  //void Reset();
  void ResetTempTreeVariables();
  void ResetClustTreeVariables();
  void ResetTruthTreeVariables();
  void ResetTrackTreeVariables();
  void ResetHepMcTreeVariables();

  void ProcessEvent();
  void ProcessEvent_Draw( clustEvent event );
  void EndProcess();
public:
  Analysis(int run_no = 50889, int fphx_bco=-1, bool mag_on = true, bool debug = false );

  void Begin();

  void SetDebugMode( bool flag ){ debug_ = flag;};
  void SetPageNumLimit( int val ){ page_num_limit_ = val; };
};

#ifndef Analysis_cc
#include "Analysis.cc"
#endif
