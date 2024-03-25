#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TAxis.h>
#include <TChain.h>

// Fixed size dimensions of array or collections stored in the TTree if any.

// Declaration of leaf types
Int_t           event;
Float_t         eta;
Float_t         phi;
Float_t         e;
Float_t         pt;
Float_t         vx;
Float_t         vy;
Float_t         vz;
vector<int>     id;

int nTow_in;
vector<float>   eta_in;
vector<float>   phi_in;
vector<float>   e_in;
vector<int>   ieta_in;
vector<int>   iphi_in;
int nTow_out;
vector<float>   eta_out;
vector<float>   phi_out;
vector<float>   e_out;
vector<int>   ieta_out;
vector<int>   iphi_out;
int nTow_emc;
vector<float>   eta_emc;
vector<float>   phi_emc;
vector<float>   e_emc;
vector<int>   ieta_emc;
vector<int>   iphi_emc;

TTree          *fChain;   //!pointer to the analyzed TTree or TChain
Int_t          fCurrent; //!current Tree number in a TChain


const int ncal = 3;
string cal_name[] = { "iHCal", "oHCal", "EMCal" };
string cal_tag[] = { "_in", "_out", "_emc" };
TString inFileName = "HCalJetPhiShift_10k.root";
int n_pt_bins[ncal] = {80,100,100};
double pt_bin_lo[ncal] = {0.,0.,0.};
double pt_bin_hi[ncal] = {4.,50.,20.};
double eta_max[ncal] = {1.1,1.1,1.152};
int n_eta_bins[ncal] = {24,24,96};
int n_phi_bins[ncal] = {64,64,256};

int *nTowers[ncal] = {&nTow_in, &nTow_out, &nTow_emc};
vector<float> *calo_eta[ncal] = {&eta_in, &eta_out, &eta_emc};
vector<float> *calo_phi[ncal] = {&phi_in, &phi_out, &phi_emc};
vector<float> *calo_e[ncal] = {&phi_in, &phi_out, &e_emc};
vector<int> *calo_ieta[ncal] = {&ieta_in, &ieta_out, &ieta_emc};
vector<int> *calo_iphi[ncal] = {&iphi_in, &iphi_out, &iphi_emc};

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

bool tower_in_square(float calo_eta, float calo_phi, float eta, float phi, int N){
  double min_eta = eta - 0.024*N - 0.012;
  double max_eta = eta + 0.024*N + 0.012;
  if ( calo_eta<min_eta || calo_eta>max_eta ) { return false; }
  double min_phi = phi - (M_PI/128.)*N - (M_PI/256.);
  double max_phi = phi + (M_PI/128.)*N + (M_PI/256.);
  if ( calo_phi<min_phi || calo_phi>max_phi ) { return false; }
  if ( calo_eta>=min_eta && calo_eta<=max_eta && calo_phi>=min_phi && calo_phi<=max_phi ) { return true; }
  return false;
};

bool tower_in_ring(float calo_eta, float calo_phi, float eta, float phi, int N){
  double n_rings = (double) N;
  double eta_lo = eta - 0.024*n_rings;
  double eta_hi = eta + 0.024*n_rings;
  double phi_lo = phi_in_range(phi - (M_PI/128.)*n_rings);// - (M_PI/256.);
  double phi_hi = phi_in_range(phi + (M_PI/128.)*n_rings);// + (M_PI/256.);
//  if (fabs(delta_phi(calo_phi,phi_lo))>(M_PI/256.) || fabs(delta_phi(calo_phi,phi_hi))>(M_PI/256.)) { return false; }
//  if ( fabs(eta_lo-calo_eta)>0.012 || fabs(eta_hi-calo_eta)>0.012 ) { return false; }
  if (fabs(delta_phi(phi_in_range(calo_phi),phi_in_range(phi_lo)))<(M_PI/256.) || fabs(delta_phi(phi_in_range(calo_phi),phi_in_range(phi_hi)))<(M_PI/256.)) {
    if ( calo_eta>eta_lo-0.012 && calo_eta<eta_hi+0.012 ) { return true; }
  }
  if ( fabs(eta_lo-calo_eta)<=0.012 || fabs(eta_hi-calo_eta)<=0.012 ) {
    if ( calo_phi>phi_lo-(M_PI/256.) && calo_phi<phi_hi+(M_PI/256.) ) { return true; }
  }
  return false;
};

const int i_cal = 2; //  THIS IS AN EMCAL STUDY
//const int ref_cal = -1; //  STUDY WRT PION
//const int ref_cal = 0; //  STUDY WRT IHCAL
//const int ref_cal = 1; //  STUDY WRT OHCAL
const int ref_cal = 2; //  STUDY WRT EMCAL

void EMCalTowerRingStudy() {
  
  TH3D *hEsum_per_nRings = new TH3D("hEsum_per_nRings",";pion p_{T} [GeV/c];E sum in tower square [GeV];n_{rings}",30,0.,30.,100,0.,100.,30,0.,30.);
  TH3D *hEsum_in_ringN = new TH3D("hEsum_in_ringN",";pion p_{T} [GeV/c];E sum in tower ring [GeV];n_{rings}",30,0.,30.,200,0.,40.,30,0.,30.);
  TH3D *hEMCalE_over_pionE = new TH3D("hEMCalE_over_pionE",";pion p_{T} [GeV/c];E_{EMCal}/E_{pion};n_{rings}",30,0.,30.,115,0.,1.15,30,0.,30.);

  TH3D *hTotalCaloSum_per_nRings = new TH3D("hTotalCaloSum_per_nRings",";pion p_{T} [GeV/c];E sum in all calos [GeV];",30,0.,30.,200,0.,100.,30,0.,30.);
  TH3D *hTotalCaloEfrac_per_nRings = new TH3D("hTotalCaloEfrac_per_nRings",";pion p_{T} [GeV/c];total calo E / pion E;",30,0.,30.,70,0.,7.,30,0.,30.);
//  hTotalCaloSum_per_nRings->GetXaxis()->SetNdivisions(29);
  for (int i=0; i<30; ++i){ hTotalCaloSum_per_nRings->GetXaxis()->SetBinLabel(i+1,Form("%ix%i",2*i+1,2*i+1)); }
  hTotalCaloSum_per_nRings->GetXaxis()->SetAlphanumeric();

  TFile *f = new TFile(inFileName,"READ");
  TTree *fChain = (TTree*)f->Get("T");
  
  fChain->SetBranchAddress("event", &event);
  fChain->SetBranchAddress("eta", &eta);
  fChain->SetBranchAddress("phi", &phi);
  fChain->SetBranchAddress("e", &e);
  fChain->SetBranchAddress("pt", &pt);
  fChain->SetBranchAddress("vx", &vx);
  fChain->SetBranchAddress("vy", &vy);
  fChain->SetBranchAddress("vz", &vz);
  
  for (int ical=0; ical<ncal; ++ical) {  // LOOP OVER CALORIMETER LAYERS
    fChain->SetBranchAddress(Form("nTow%s",cal_tag[ical].c_str()), &nTowers[ical]);
    fChain->SetBranchAddress(Form("eta%s",cal_tag[ical].c_str()), &calo_eta[ical]);
    fChain->SetBranchAddress(Form("phi%s",cal_tag[ical].c_str()), &calo_phi[ical]);
    fChain->SetBranchAddress(Form("e%s",cal_tag[ical].c_str()), &calo_e[ical]);
    fChain->SetBranchAddress(Form("ieta%s",cal_tag[ical].c_str()), &calo_ieta[ical]);
    fChain->SetBranchAddress(Form("iphi%s",cal_tag[ical].c_str()), &calo_iphi[ical]);
  }
  
  
  Long64_t nentries = fChain->GetEntriesFast();
  
  for (Long64_t jentry=0; jentry<nentries;jentry++) {  // LOOP OVER EVENTS
    Long64_t ientry = fChain->GetTreeNumber();
    if (ientry < 0) break;
    
    fChain->GetEntry(jentry);
    
    double total_calo_E[ncal] = {0.,0.,0.};
    
    float ref_e = 0.;
    float ref_eta;
    float ref_phi;
    int   ref_ieta;
    int   ref_iphi;
    
    if (ref_cal==-1) {
      ref_e = e;
      ref_eta = eta;
      ref_phi = phi;
//      ref_phi = phi_in_range(phi+M_PI);
    }
    else if (ref_cal>=0 && ref_cal<=2){
      for (int itow=0; itow<calo_e[ref_cal]->size(); ++itow) {  // LOOP OVER REFERENCE CAL TOWERS
        if (calo_e[ref_cal]->at(itow)>ref_e) {
          ref_e = calo_e[ref_cal]->at(itow);
          ref_eta = calo_eta[ref_cal]->at(itow);
          ref_phi = calo_phi[ref_cal]->at(itow);
//          ref_phi = phi_in_range( calo_phi[ref_cal]->at(itow) + M_PI );
        }
      }
    }
    else { cerr<<"ope"<<endl; }
    
    for (int index=0; index<2; ++index) {  // LOOP OVER HCALS
      for (int itow=0; itow<calo_e[index]->size(); ++itow) {  // LOOP OVER CAL TOWERS
        total_calo_E[index] += calo_e[index]->at(itow);
      }
    }
    
    int n_towers_in_square = 0;
    double square_e_sum = 0.;
    for (int n_rings = 0; n_rings<30; ++n_rings) {
      
      double ring_e_sum = 0.;
      int n_towers_in_ring = 0;
      for (int itow=0; itow<calo_e[i_cal]->size(); ++itow) {  // LOOP OVER EMCAL TOWERS
//        if (i_cal==2 && calo_e[i_cal]->at(itow)<0.1) { continue; }
        if ( tower_in_ring(calo_eta[i_cal]->at(itow),calo_phi[i_cal]->at(itow),ref_eta,ref_phi,n_rings) ) {
          total_calo_E[2] += calo_e[i_cal]->at(itow);
          ring_e_sum += calo_e[i_cal]->at(itow);
          square_e_sum += calo_e[i_cal]->at(itow);
          n_towers_in_square += 1;
          n_towers_in_ring += 1;
        }
      }  // end tower loop
      hEsum_per_nRings->Fill(pt,square_e_sum,n_rings);
      hEsum_in_ringN->Fill(pt,ring_e_sum,n_rings);
      hEMCalE_over_pionE->Fill(pt,ring_e_sum/e,n_rings);
      
      hTotalCaloSum_per_nRings->Fill(e,square_e_sum+total_calo_E[0]+total_calo_E[1],n_rings);
      hTotalCaloEfrac_per_nRings->Fill(e,(square_e_sum+total_calo_E[0]+total_calo_E[1])/e,n_rings);
    }
    
    // if (Cut(ientry) < 0) continue;
    
  }  // end event loop
  
  
  
  string outputroot = (string) inFileName;
  string remove_this = ".root";
  size_t pos = outputroot.find(remove_this);
  if (pos != string::npos) { outputroot.erase(pos, remove_this.length()); }
  if (ref_cal>=0 && ref_cal<=2){
    outputroot += cal_tag[ref_cal] + "Ref";
  }
  TString outFileName = outputroot + "_EMCalRings.root";
  
  TFile *outFile = new TFile(outFileName,"RECREATE");
  hEsum_per_nRings->Write();
  hEsum_in_ringN->Write();
  hTotalCaloSum_per_nRings->Write();
  hTotalCaloEfrac_per_nRings->Write();
  hEMCalE_over_pionE->Write();
  //  for (int ical=0; ical<ncal; ++ical) {  // LOOP OVER CALORIMETER LAYERS
  //  }

  vector<TH3D *> histos = {hEsum_per_nRings, hEsum_in_ringN, hTotalCaloSum_per_nRings, hTotalCaloEfrac_per_nRings, hEMCalE_over_pionE};
  auto fa1 = new TF1("fa1","0.",0,1);

  TCanvas * can = new TCanvas( "can" , "" ,700 ,500 );
  can->SetLogz();
  const char us[] = "_";
  
  for (int i=0; i<histos.size(); ++i ) {
    histos[i]->Project3D("YZ")->Draw("COLZ");
    can->SaveAs(Form("plots/EMCalTowerRingStudy/%s.pdf",histos[i]->GetName()),"PDF");
    for (int j=0; j<3; ++j) {
      double ptlo = (double)10.*j;
      double pthi = (double)10.*(j+1);
      histos[i]->GetXaxis()->SetRangeUser(ptlo,pthi);
      histos[i]->Project3D("YZ")->Draw("COLZ");
      can->SaveAs(Form("plots/EMCalTowerRingStudy/%s%s%i%s%i.pdf",histos[i]->GetName(),us,(int)ptlo,us,(int)pthi),"PDF");
    }
    if (i==4){
      histos[i]->GetXaxis()->SetRangeUser(1.,2.);
      TH2D *hDenom2D = (TH2D*)histos[i]->Project3D("YZ");
      TH1D *hDenom = (TH1D*)hDenom2D->ProfileX();
      hDenom->SetName("hDenom");

      for (int j=1; j<10; ++j) {
        double ptlo = (double)1.*j;
        double pthi = (double)1.*(j+1);
        histos[i]->GetXaxis()->SetRangeUser(ptlo,pthi);
        histos[i]->Project3D("YZ")->Draw("COLZ");
        TH2D *hTemp = (TH2D*)histos[i]->Project3D("YZ");
        hTemp->SetLineColor(kRed);
        hTemp->SetMarkerColor(kRed);
        TH1D *hNum = (TH1D*)hTemp->ProfileX();
        hNum->Draw("SAME");
        can->SaveAs(Form("plots/EMCalTowerRingStudy/%s_FINE_%s%i%s%i.pdf",histos[i]->GetName(),us,(int)ptlo,us,(int)pthi),"PDF");
        hNum->Divide(hDenom);
        hNum->GetYaxis()->SetRangeUser(0.1,2.2);
        hNum->Draw();
        fa1->Draw("SAME");
        can->SaveAs(Form("plots/EMCalTowerRingStudy/%s_FINE_%s%i%s%i__ratioTo1_2.pdf",histos[i]->GetName(),us,(int)ptlo,us,(int)pthi),"PDF");
        hNum->Reset();
      }
    }
    histos[i]->GetXaxis()->SetRangeUser(0.,30.);
  }

//  hEsum_per_nRings->Project3D("YZ")->Draw("COLZ");
//  can->SaveAs("plots/EMCalTowerRingStudy/hEsum_per_nRings.pdf","PDF");
//  hEsum_in_ringN->Project3D("YZ")->Draw("COLZ");
//  can->SaveAs("plots/EMCalTowerRingStudy/hEsum_in_ringN.pdf","PDF");
//  hTotalCaloSum_per_nRings->Project3D("YZ")->Draw("COLZ");
//  can->SaveAs("plots/EMCalTowerRingStudy/hTotalCaloSum_per_nRings.pdf","PDF");
//  hTotalCaloEfrac_per_nRings->Project3D("YZ")->Draw("COLZ");
//  can->SaveAs("plots/EMCalTowerRingStudy/hTotalCaloEfrac_per_nRings.pdf","PDF");
}
