#ifndef analysis_h
#define analysis_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class analysis
{
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           evt_clus;
   vector<double>  *x_in;
   vector<double>  *y_in;
   vector<double>  *z_in;
   vector<double>  *r_in;
   vector<int>     *size_in;
   vector<double>  *phi_in;
   vector<double>  *theta_in;
   vector<double>  *adc_in;
   vector<bool>    *is_associated_in;
   vector<double>  *track_incoming_theta_in;
   vector<double>  *x_out;
   vector<double>  *y_out;
   vector<double>  *z_out;
   vector<double>  *r_out;
   vector<int>     *size_out;
   vector<double>  *phi_out;
   vector<double>  *theta_out;
   vector<double>  *adc_out;
   vector<bool>    *is_associated_out;
   vector<double>  *track_incoming_theta_out;
   Double_t        z_vertex;

   // List of branches
   TBranch        *b_evt_clus;   //!
   TBranch        *b_x_in;   //!
   TBranch        *b_y_in;   //!
   TBranch        *b_z_in;   //!
   TBranch        *b_r_in;   //!
   TBranch        *b_size_in;   //!
   TBranch        *b_phi_in;   //!
   TBranch        *b_theta_in;   //!
   TBranch        *b_adc_in;   //!
   TBranch        *b_is_associated_in;   //!
   TBranch        *b_track_incoming_theta_in;   //!
   TBranch        *b_x_out;   //!
   TBranch        *b_y_out;   //!
   TBranch        *b_z_out;   //!
   TBranch        *b_r_out;   //!
   TBranch        *b_size_out;   //!
   TBranch        *b_phi_out;   //!
   TBranch        *b_theta_out;   //!
   TBranch        *b_adc_out;   //!
   TBranch        *b_is_associated_out;   //!
   TBranch        *b_track_incoming_theta_out;   //!
   TBranch        *b_z_vertex;   //!

  //TH1D* hist;
  TH1D* hist_all;
  TH1D* hist_aso;
  TH1D* hist_no_aso;
  TH1D* hist_ang90;
  TH1D* hist_ang45;
  TH1D* hist_ang35;
  TH1D* hist_ang25;

  TH2D* hist_correlation;
  
  analysis(TTree *tree=0, int run = 41981 );
   virtual ~analysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifndef analysis_cxx
#include "analysis.C"
#endif // #ifdef analysis_cxx
