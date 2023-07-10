#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>

// Fixed size dimensions of array or collections stored in the TTree if any.

// Declaration of leaf types
Int_t           event;
Int_t           nTowers;
Float_t         eta;
Float_t         phi;
Float_t         e;
Float_t         pt;
Float_t         vx;
Float_t         vy;
Float_t         vz;
vector<int>     id;
vector<float>   eta_in;
vector<float>   phi_in;
vector<float>   e_in;
vector<float>   ieta_in;
vector<float>   iphi_in;
vector<float>   eta_out;
vector<float>   phi_out;
vector<float>   e_out;
vector<float>   ieta_out;
vector<float>   iphi_out;

TTree          *fChain;   //!pointer to the analyzed TTree or TChain
Int_t          fCurrent; //!current Tree number in a TChain


const int ncal = 2;
string cal_name[] = { "iHCal", "oHCal", "EMCal" };
string cal_tag[] = { "_in", "_out", "_em" };
TString inFileName = "HCalJetPhiShift_10k.root";
//TString inFileName = "HCalJetPhiShift_negativePion_magnetOn_15k.root";
int n_pt_bins[ncal] = {80,100};
double pt_bin_lo[ncal] = {0.,0.};
double pt_bin_hi[ncal] = {4.,50.};

vector<float> *calo_eta[ncal] = {&eta_in, &eta_out};
vector<float> *calo_phi[ncal] = {&phi_in, &phi_out};
vector<float> *calo_e[ncal] = {&e_in, &e_out};
vector<float> *calo_ieta[ncal] = {&ieta_in, &ieta_out};
vector<float> *calo_iphi[ncal] = {&iphi_in, &iphi_out};

double delta_phi(double phi1, double phi2){
  double dPhi = phi1 - phi2;
  while (dPhi>=M_PI) { dPhi -= 2.*M_PI; }
  while (dPhi<=-M_PI) { dPhi += 2.*M_PI; }
  return dPhi;
};

double phi_in_range(double phi){
  while (phi>=M_PI) { phi -= 2.*M_PI; }
  while (phi<=-M_PI) { phi += 2.*M_PI; }
  return phi;
};

TH1D *hGeantPhi = new TH1D("hGeantPhi",";truth #phi",64,-M_PI,M_PI);
TH3D *hE_inner_vs_outer = new TH3D("hE_inner_vs_outer",";pion p_{T};total E deposited in iHCal;total E deposited in oHCal",60,0.,30.,100, 0., 10.,120,0.,60.);

TH3D *hE_weighted_eta_phi[ncal];
TH2D *hPhi2D[ncal], *hDeltaPhi_pt[ncal], *hDeltaPhi_eta[ncal], *hTowerEt_pionEt[ncal], *hCaloEnergy_pionPt[ncal], *hEnergy_fraction[ncal], *hDeltaPhi_E[ncal], *hDeltaPhi_iPhi[ncal];
TH1D *hDeltaPhi[ncal], *hCaloPhi[ncal], *hTowerEt[ncal];

void ExploreTTrees() {
  
  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
    hE_weighted_eta_phi[i_cal] = new TH3D(Form("hE_weighted_eta_phi_%s",cal_name[i_cal].c_str()),";pion p_{T};calo #eta;calo #phi",60,0.,30.,22,-1.,1.,64,-M_PI,M_PI);
    hPhi2D[i_cal] = new TH2D(Form("hPhi2D_%s",cal_name[i_cal].c_str()),";truth #phi;tower #phi",64,-M_PI,M_PI,64,-M_PI,M_PI);
    hDeltaPhi_pt[i_cal] = new TH2D(Form("hDeltaPhi_pt_%s",cal_name[i_cal].c_str()),";pion p_{T};tower #phi - truth #phi",120,0.,60.,320,-4.,4.);
    hDeltaPhi_E[i_cal] = new TH2D(Form("hDeltaPhi_E_%s",cal_name[i_cal].c_str()),";tower #phi - truth #phi;tower E_{T}",320,-4.,4.,n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal]);
    hDeltaPhi_iPhi[i_cal] = new TH2D(Form("hDeltaPhi_iPhi_%s",cal_name[i_cal].c_str()),";tower #phi - truth #phi;tower iphi",320,-4.,4.,24,0.,24.);
    hDeltaPhi_eta[i_cal] = new TH2D(Form("hDeltaPhi_eta_%s",cal_name[i_cal].c_str()),";pion #eta;tower #phi - truth #phi",60,-1.,1.,320,-4.,4.);
    hTowerEt_pionEt[i_cal] = new TH2D(Form("hTowerEt_pionEt_%s",cal_name[i_cal].c_str()),";tower E_{T};pion E_{T}",n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal],120,0.,60.);
    hCaloEnergy_pionPt[i_cal] = new TH2D(Form("hCaloEnergy_pionPt_%s",cal_name[i_cal].c_str()),";pion p_{T};total E deposited in calo",60,0.,30.,n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal]);
    hEnergy_fraction[i_cal] = new TH2D(Form("hEnergy_fraction_%s",cal_name[i_cal].c_str()),";pion E;fraction of pion E in calo",120,0.,60.,100,0.,1.);
    hDeltaPhi[i_cal] = new TH1D(Form("hDeltaPhi_%s",cal_name[i_cal].c_str()),";tower #phi - truth #phi",128,-2.*M_PI,2.*M_PI);
    hCaloPhi[i_cal] = new TH1D(Form("hCaloPhi_%s",cal_name[i_cal].c_str()),";tower #phi",64,-M_PI,M_PI);
    hTowerEt[i_cal] = new TH1D(Form("hTowerEt_%s",cal_name[i_cal].c_str()),";tower E_{T}",n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal]);
  }
  
  TFile *f = new TFile(inFileName,"READ");
  TTree *fChain = (TTree*)f->Get("T");
  
  fChain->SetBranchAddress("event", &event);
  fChain->SetBranchAddress("nTowers", &nTowers);
  fChain->SetBranchAddress("eta", &eta);
  fChain->SetBranchAddress("phi", &phi);
  fChain->SetBranchAddress("e", &e);
  fChain->SetBranchAddress("pt", &pt);
  fChain->SetBranchAddress("vx", &vx);
  fChain->SetBranchAddress("vy", &vy);
  fChain->SetBranchAddress("vz", &vz);
  
  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
    fChain->SetBranchAddress(Form("eta%s",cal_tag[i_cal].c_str()), &calo_eta[i_cal]);
    fChain->SetBranchAddress(Form("phi%s",cal_tag[i_cal].c_str()), &calo_phi[i_cal]);
    fChain->SetBranchAddress(Form("e%s",cal_tag[i_cal].c_str()), &calo_e[i_cal]);
    fChain->SetBranchAddress(Form("ieta%s",cal_tag[i_cal].c_str()), &calo_ieta[i_cal]);
    fChain->SetBranchAddress(Form("iphi%s",cal_tag[i_cal].c_str()), &calo_iphi[i_cal]);
  }
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {  // LOOP OVER EVENTS
    Long64_t ientry = fChain->GetTreeNumber();
    if (ientry < 0) break;
    
    fChain->GetEntry(jentry);
    
    hGeantPhi->Fill(phi_in_range(phi));
    
    float total_E[ncal] = {0.,0.};
    
    for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
      float max_tow_e = 0.;
      float max_tow_eta;
      float max_tow_phi;
      int   max_tow_iphi;
      
      
      for (int itow=0; itow<calo_e[i_cal]->size(); ++itow) {  // LOOP OVER CALORIMETER TOWERS
        total_E[i_cal] += calo_e[i_cal]->at(itow);
        if (calo_e[i_cal]->at(itow)>max_tow_e){
          max_tow_e = calo_e[i_cal]->at(itow);
          max_tow_eta = calo_eta[i_cal]->at(itow);
          max_tow_phi = phi_in_range(calo_phi[i_cal]->at(itow));
          max_tow_iphi = calo_iphi[i_cal]->at(itow);
        }
        hCaloEnergy_pionPt[i_cal]->Fill(pt,calo_e[i_cal]->at(itow));
        hE_weighted_eta_phi[i_cal]->Fill(pt,calo_eta[i_cal]->at(itow),calo_phi[i_cal]->at(itow),calo_e[i_cal]->at(itow));
      }  // end tower loop
      
      double e_fraction = (double) total_E[i_cal]/e;
      hEnergy_fraction[i_cal]->Fill(e,e_fraction);
      hPhi2D[i_cal]->Fill(phi_in_range(phi),max_tow_phi);
      hDeltaPhi[i_cal]->Fill(delta_phi(max_tow_phi,phi));
      hDeltaPhi_pt[i_cal]->Fill(pt,delta_phi(max_tow_phi,phi));
      hDeltaPhi_E[i_cal]->Fill(delta_phi(max_tow_phi,phi),max_tow_e);
      hDeltaPhi_iPhi[i_cal]->Fill(delta_phi(max_tow_phi,phi),max_tow_iphi);
      hDeltaPhi_eta[i_cal]->Fill(eta,delta_phi(max_tow_phi,phi));
      hCaloPhi[i_cal]->Fill(phi_in_range(max_tow_phi));
      hTowerEt[i_cal]->Fill(max_tow_e);
      hTowerEt_pionEt[i_cal]->Fill(max_tow_e,e);
      // if (Cut(ientry) < 0) continue;
    }  // end calo loop
    hE_inner_vs_outer->Fill(pt, total_E[0], total_E[1]);
  }  // end event loop
  
  //  hPhi2D->Draw("COLZ");
  //  new TCanvas;
  //  hDeltaPhi->Draw("COLZ");
  //  new TCanvas;
  //  hDeltaPhi_pt->Draw("COLZ");
  //  new TCanvas;
  //  hGeantPhi->Draw();
  //  new TCanvas;
  //  hCaloPhi->Draw();
  //  new TCanvas;
  //  hTowerEt->Draw();
  //  new TCanvas;
  //  hTowerEt_pionEt->Draw("COLZ");
  //  new TCanvas;
  //  hDeltaPhi_eta->Draw("COLZ");
  
  string outputroot = (string) inFileName;
  string remove_this = ".root";
  size_t pos = outputroot.find(remove_this);
  TString outFileName = outputroot + "_histos.root";
  
  TFile *outFile = new TFile(outFileName,"RECREATE");
  hGeantPhi->Write();
  hE_inner_vs_outer->Write();
  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
    hE_weighted_eta_phi[i_cal]->Write();
    hPhi2D[i_cal]->Write();
    hDeltaPhi_pt[i_cal]->Write();
    hDeltaPhi_eta[i_cal]->Write();
    hTowerEt_pionEt[i_cal]->Write();
    hCaloEnergy_pionPt[i_cal]->Write();
    hEnergy_fraction[i_cal]->Write();
    hDeltaPhi_E[i_cal]->Write();
    hDeltaPhi_iPhi[i_cal]->Write();
    hDeltaPhi[i_cal]->Write();
    hCaloPhi[i_cal]->Write();
    hTowerEt[i_cal]->Write();
  }
  
}
