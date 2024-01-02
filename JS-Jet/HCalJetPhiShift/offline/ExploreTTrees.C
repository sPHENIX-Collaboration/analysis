#include <iostream>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
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
//TString inFileName = "HCalJetPhiShift_negativePion_magnetOn_15k.root";
int n_pt_bins[ncal] = {80,100,100};
double pt_bin_lo[ncal] = {0.,0.,0.};
double pt_bin_hi[ncal] = {4.,50.,20.};
double eta_max[ncal] = {1.1,1.1,1.152};
int n_eta_bins[ncal] = {24,24,96};
int n_phi_bins[ncal] = {64,64,256};

int *nTowers[ncal] = {&nTow_in, &nTow_out, &nTow_emc};
vector<float> *calo_eta[ncal] = {&eta_in, &eta_out, &eta_emc};
vector<float> *calo_phi[ncal] = {&phi_in, &phi_out, &phi_emc};
vector<float> *calo_e[ncal] = {&e_in, &e_out, &e_emc};
vector<int> *calo_ieta[ncal] = {&ieta_in, &ieta_out, &ieta_emc};
vector<int> *calo_iphi[ncal] = {&iphi_in, &iphi_out, &iphi_emc};

double delta_phi(double phi1, double phi2){
  double dPhi = phi1 - phi2;
  while (dPhi>=M_PI) { dPhi -= 2.*M_PI; }
  while (dPhi<=-M_PI) { dPhi += 2.*M_PI; }
  return dPhi;
};

double dR(double phi1, double phi2, double eta1, double eta2){
  double dphi_sqrd = delta_phi( phi1, phi2)*delta_phi( phi1, phi2);
  double deta_sqrd = (eta1-eta2)*(eta1-eta2);
  return sqrt(dphi_sqrd+deta_sqrd);
};

double phi_in_range(double phi){
  while (phi>=M_PI) { phi -= 2.*M_PI; }
  while (phi<=-M_PI) { phi += 2.*M_PI; }
  return phi;
};

bool tower_in_3x3(float calo_eta, float calo_phi, float eta, float phi){
  double min_eta = eta - 0.024 - 0.012;
  double max_eta = eta + 0.024 + 0.012;
  if ( calo_eta<min_eta || calo_eta>max_eta ) { return false; }
  double min_phi = phi - (M_PI/128.) - (M_PI/256.);
  double max_phi = phi + (M_PI/128.) + (M_PI/256.);
  if ( calo_phi<min_phi || calo_phi>max_phi ) { return false; }
  if ( calo_eta>=min_eta && calo_eta<=max_eta && calo_phi>=min_phi && calo_phi<=max_phi ) { return true; }
  return false;
};

TH1D *hGeantPhi = new TH1D("hGeantPhi",";truth #phi",64,-M_PI,M_PI);
TH2D *hEMCal3x3 = new TH2D("hEMCal3x3","pion p_{T};E_{EMCal}^{3x3} about max-E tower [GeV]",30,0.,30.,120,0.,60.);
TH2D *hEMCal3x3_FINE = new TH2D("hEMCal3x3_FINE","pion p_{T};E_{EMCal}^{3x3} about max-E tower [GeV]",30,0.,30.,200.,0.,2.);
TH3D *hE_inner_vs_outer = new TH3D("hE_inner_vs_outer",";pion p_{T};total E deposited in iHCal;total E deposited in oHCal",60,0.,30.,100, 0., 10.,120,0.,60.);
TH3D *hDeltaPhi_fraction_HcalOverAll = new TH3D("hDeltaPhi_fraction_HcalOverAll",";pion p_{T};E_{HCals}/E_{all calos};oHCal #Delta#phi",60,0.,30.,100,0.,1.,160,-0.4,0.4);
TH3D *hDeltaPhi_fraction_oHcalOverHcals = new TH3D("hDeltaPhi_fraction_oHcalOverHcals",";pion p_{T};E_{oHCal}/E_{HCals};oHCal #Delta#phi",60,0.,30.,100,0.,1.,160,-0.4,0.4);

TH3D *hE_weighted_eta_phi[ncal], *h_eta_phi[ncal];
TH2D *hPhi2D[ncal], *hDeltaPhi_pt[ncal], *hDeltaPhi_eta[ncal], *hTowerEt_pionEt[ncal], *hCaloEnergy_pionPt[ncal], *hEnergy_fraction[ncal], *hDeltaPhi_E[ncal], *hDeltaPhi_iPhi[ncal], *hDeltaPhi_fraction[ncal];

TH1D *hDeltaPhi[ncal], *hCaloPhi[ncal], *hTowerEt[ncal];

void ExploreTTrees() {
  
  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
    hE_weighted_eta_phi[i_cal] = new TH3D(Form("hE_weighted_eta_phi_%s",cal_name[i_cal].c_str()),";pion p_{T};calo #eta;calo #phi",60,0.,30.,n_eta_bins[i_cal],-eta_max[i_cal],eta_max[i_cal],n_phi_bins[i_cal],-M_PI,M_PI);
    h_eta_phi[i_cal] = new TH3D(Form("h_eta_phi_%s",cal_name[i_cal].c_str()),";pion p_{T};calo #eta;calo #phi",60,0.,30.,n_eta_bins[i_cal],-eta_max[i_cal],eta_max[i_cal],n_phi_bins[i_cal],-M_PI,M_PI);
    hPhi2D[i_cal] = new TH2D(Form("hPhi2D_%s",cal_name[i_cal].c_str()),";truth #phi;tower #phi",n_phi_bins[i_cal],-M_PI,M_PI,n_phi_bins[i_cal],-M_PI,M_PI);
    hDeltaPhi_pt[i_cal] = new TH2D(Form("hDeltaPhi_pt_%s",cal_name[i_cal].c_str()),";pion p_{T};tower #phi - truth #phi",120,0.,60.,320,-4.,4.);
    hDeltaPhi_E[i_cal] = new TH2D(Form("hDeltaPhi_E_%s",cal_name[i_cal].c_str()),";tower #phi - truth #phi;tower E_{T}",320,-4.,4.,n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal]);
    hDeltaPhi_fraction[i_cal] = new TH2D(Form("hDeltaPhi_fraction_%s",cal_name[i_cal].c_str()),";tower #phi - truth #phi;fraction of total calo E in layer",320,-4.,4.,100,0,1.);
    hDeltaPhi_iPhi[i_cal] = new TH2D(Form("hDeltaPhi_iPhi_%s",cal_name[i_cal].c_str()),";tower #phi - truth #phi;tower iphi",320,-4.,4.,256,0.,256.);
    hDeltaPhi_eta[i_cal] = new TH2D(Form("hDeltaPhi_eta_%s",cal_name[i_cal].c_str()),";pion #eta;tower #phi - truth #phi",n_phi_bins[i_cal],-eta_max[i_cal],eta_max[i_cal],320,-4.,4.);
    hTowerEt_pionEt[i_cal] = new TH2D(Form("hTowerEt_pionEt_%s",cal_name[i_cal].c_str()),";tower E_{T};pion E_{T}",n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal],120,0.,60.);
    hCaloEnergy_pionPt[i_cal] = new TH2D(Form("hCaloEnergy_pionPt_%s",cal_name[i_cal].c_str()),";pion p_{T};total E deposited in calo",60,0.,30.,n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal]);
    hEnergy_fraction[i_cal] = new TH2D(Form("hEnergy_fraction_%s",cal_name[i_cal].c_str()),";pion E;fraction of pion E in calo",120,0.,60.,100,0.,10.);
    hDeltaPhi[i_cal] = new TH1D(Form("hDeltaPhi_%s",cal_name[i_cal].c_str()),";tower #phi - truth #phi",128,-2.*M_PI,2.*M_PI);
    hCaloPhi[i_cal] = new TH1D(Form("hCaloPhi_%s",cal_name[i_cal].c_str()),";tower #phi",n_phi_bins[i_cal],-M_PI,M_PI);
    hTowerEt[i_cal] = new TH1D(Form("hTowerEt_%s",cal_name[i_cal].c_str()),";tower E_{T}",n_pt_bins[i_cal], pt_bin_lo[i_cal], pt_bin_hi[i_cal]);
  }
  
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
  
  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
    fChain->SetBranchAddress(Form("nTow%s",cal_tag[i_cal].c_str()), &nTowers[i_cal]);
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
    
    float total_E[ncal] = {0.,0.,0.};
    float max_out_phi;
    //    float max_tow_phi[ncal];// = {0.,0.,0.};
    
    float closestOuter_tow_dR = 9999.;
    float closestOuter_tow_eta;
    float closestOuter_tow_phi;
    
    float energyIn3x3 = 0;
    
    for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
      float max_tow_e = 0.;
      float max_tow_eta;
      int   max_tow_iphi;
      float max_tow_phi;

      for (int itow=0; itow<calo_e[i_cal]->size(); ++itow) {  // LOOP OVER CALORIMETER TOWERS
        
        if (i_cal==2 && tower_in_3x3(calo_eta[i_cal]->at(itow), calo_phi[i_cal]->at(itow), eta, phi) ){
          energyIn3x3 += calo_e[i_cal]->at(itow);
        }
        
//        if (calo_e[i_cal]->at(itow)<=0.) continue;
//        if (i_cal==2 && calo_e[i_cal]->at(itow)<0.08) { continue; }
        
        total_E[i_cal] += calo_e[i_cal]->at(itow);
        if (calo_e[i_cal]->at(itow)>max_tow_e){
          max_tow_e = calo_e[i_cal]->at(itow);
          max_tow_eta = calo_eta[i_cal]->at(itow);
          max_tow_phi = calo_phi[i_cal]->at(itow);
          max_tow_iphi = calo_iphi[i_cal]->at(itow);
        }
        if( i_cal==1 ){  // MATCH PION TO CLOSEST OHCAL TOWER
          float delta_R = dR(phi, calo_phi[i_cal]->at(itow), eta, calo_eta[i_cal]->at(itow));
          if (delta_R<closestOuter_tow_dR){
            closestOuter_tow_dR = delta_R;
            closestOuter_tow_eta = calo_eta[i_cal]->at(itow);
            closestOuter_tow_phi = calo_phi[i_cal]->at(itow);
          }
        }
        hCaloEnergy_pionPt[i_cal]->Fill(pt,calo_e[i_cal]->at(itow));
        h_eta_phi[i_cal]->Fill(pt,calo_eta[i_cal]->at(itow),phi_in_range(calo_phi[i_cal]->at(itow)));
        hE_weighted_eta_phi[i_cal]->Fill(pt,calo_eta[i_cal]->at(itow),phi_in_range(calo_phi[i_cal]->at(itow)),calo_e[i_cal]->at(itow));
      }  // end tower loop
      
      if (i_cal==2) {
        hEMCal3x3->Fill(pt,energyIn3x3);
        hEMCal3x3_FINE->Fill(pt,energyIn3x3);
      }

//      cout<<cal_tag[i_cal]<<"\t"<<max_tow_eta<<"\t"<<eta<<"\t"<<delta_phi(max_tow_phi,phi)<<endl;
      
      if (i_cal==1) {max_out_phi=max_tow_phi;}
      
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
    
    for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
      hDeltaPhi_fraction[i_cal]->Fill(delta_phi(max_out_phi,phi),total_E[i_cal]/(total_E[0]+total_E[1]+total_E[2]));
    }
    
    hDeltaPhi_fraction_HcalOverAll->Fill(pt, (total_E[0]+total_E[1])/(total_E[0]+total_E[1]+total_E[2]), delta_phi(closestOuter_tow_phi,phi));
    hDeltaPhi_fraction_oHcalOverHcals->Fill(pt, total_E[1]/(total_E[0]+total_E[1]), delta_phi(closestOuter_tow_phi,phi));
    
    hE_inner_vs_outer->Fill(pt, total_E[0], total_E[1]);
  }  // end event loop
  
//  new TCanvas;
//  hGeantPhi->Draw();
//  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
//    hPhi2D[i_cal]->Draw("COLZ");
//    new TCanvas;
//    hDeltaPhi[i_cal]->Draw("COLZ");
//    new TCanvas;
//    hDeltaPhi_pt[i_cal]->Draw("COLZ");
//    new TCanvas;
//    hCaloPhi[i_cal]->Draw();
//    new TCanvas;
//    hTowerEt[i_cal]->Draw();
//    new TCanvas;
//    hTowerEt_pionEt[i_cal]->Draw("COLZ");
//    new TCanvas;
//    hDeltaPhi_eta[i_cal]->Draw("COLZ");
//  }

  
  string outputroot = (string) inFileName;
  string remove_this = ".root";
  size_t pos = outputroot.find(remove_this);
  if (pos != string::npos) { outputroot.erase(pos, remove_this.length()); }
  TString outFileName = outputroot + "_histos.root";
  
  TFile *outFile = new TFile(outFileName,"RECREATE");
  hEMCal3x3->Write();
  hEMCal3x3_FINE->Write();
  hGeantPhi->Write();
  hE_inner_vs_outer->Write();
  hDeltaPhi_fraction_HcalOverAll->Write();
  hDeltaPhi_fraction_oHcalOverHcals->Write();
  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
    hE_weighted_eta_phi[i_cal]->Write();
    h_eta_phi[i_cal]->Write();
    hPhi2D[i_cal]->Write();
    hDeltaPhi_pt[i_cal]->Write();
    hDeltaPhi_eta[i_cal]->Write();
    hTowerEt_pionEt[i_cal]->Write();
    hCaloEnergy_pionPt[i_cal]->Write();
    hEnergy_fraction[i_cal]->Write();
    hDeltaPhi_fraction[i_cal]->Write();
    hDeltaPhi_E[i_cal]->Write();
    hDeltaPhi_iPhi[i_cal]->Write();
    hDeltaPhi[i_cal]->Write();
    hCaloPhi[i_cal]->Write();
    hTowerEt[i_cal]->Write();
  }
  
  for (int i_cal=0; i_cal<ncal; ++i_cal) {  // LOOP OVER CALORIMETER LAYERS
    new TCanvas;
    hDeltaPhi_fraction[i_cal]->Draw("COLZ");
  }
  
  vector<TH3D *> histos = {hDeltaPhi_fraction_HcalOverAll, hDeltaPhi_fraction_oHcalOverHcals};
  
  TCanvas * can = new TCanvas( "can" , "" ,700 ,500 );
  can->SetLogz();
  const char us[] = "_";
  
  auto fa1 = new TF1("fa1","0.",0,1);
  
  for (int i=0; i<histos.size(); ++i ) {
    TH2D *hTemp = (TH2D*)histos[i]->Project3D("ZY");
    hTemp->SetLineColor(kRed);
    hTemp->SetMarkerColor(kRed);
    hTemp->Draw("COLZ");
    hTemp->ProfileX()->Draw("SAME");
    can->SaveAs(Form("plots/ExploreTTrees/%s.pdf",histos[i]->GetName()),"PDF");
    hTemp->ProfileX()->Draw("");
    fa1->Draw("SAME");
    can->SaveAs(Form("plots/ExploreTTrees/%s_pfx.pdf",histos[i]->GetName()),"PDF");
    for (int j=0; j<3; ++j) {
      double ptlo = (double)10.*j;
      double pthi = (double)10.*(j+1);
      histos[i]->GetXaxis()->SetRangeUser(ptlo,pthi);
      hTemp = (TH2D*)histos[i]->Project3D("ZY");
      hTemp->SetLineColor(kRed);
      hTemp->SetMarkerColor(kRed);
      hTemp->Draw("COLZ");
      hTemp->ProfileX()->Draw("SAME");
      can->SaveAs(Form("plots/ExploreTTrees/%s%s%i%s%i.pdf",histos[i]->GetName(),us,(int)ptlo,us,(int)pthi),"PDF");
      hTemp->ProfileX()->Draw("");
      fa1->Draw("SAME");
      can->SaveAs(Form("plots/ExploreTTrees/%s%s%i%s%i_pfx.pdf",histos[i]->GetName(),us,(int)ptlo,us,(int)pthi),"PDF");
    }
    for (int j=1; j<10; ++j) {
      double ptlo = (double)1.*j;
      double pthi = (double)1.*(j+1);
      histos[i]->GetXaxis()->SetRangeUser(ptlo,pthi);
      hTemp = (TH2D*)histos[i]->Project3D("ZY");
      hTemp->SetLineColor(kRed);
      hTemp->SetMarkerColor(kRed);
      hTemp->Draw("COLZ");
      hTemp->ProfileX()->Draw("SAME");
      can->SaveAs(Form("plots/ExploreTTrees/%s%s%i%s%i.pdf",histos[i]->GetName(),us,(int)ptlo,us,(int)pthi),"PDF");
      hTemp->ProfileX()->Draw("");
      fa1->Draw("SAME");
      can->SaveAs(Form("plots/ExploreTTrees/%s%s%i%s%i_pfx.pdf",histos[i]->GetName(),us,(int)ptlo,us,(int)pthi),"PDF");
    }
    histos[i]->GetXaxis()->SetRangeUser(0.,30.);
  }
  
//  gStyle->SetPalette(kCMYK);

  hEMCal3x3->Draw("COLZ");
  can->SaveAs("plots/ExploreTTrees/hEMCal3x3.pdf","PDF");

  hEMCal3x3_FINE->Draw("COLZ");
  can->SaveAs("plots/ExploreTTrees/hEMCal3x3_FINE.pdf","PDF");

  
  TF1 *f1 = new TF1("f1","gaus",0.,0.6);
  
  float MIP_value[29];//, MIP_sigma[29];
  
  TH1D *hEMCal3x3_FINE_py[30];
  new TCanvas;
  for (int i=1; i<30; ++i) {
    int ptbin = i+1;
    int ptlo = i;
    int pthi = i+1;
    hEMCal3x3_FINE_py[i-1] = (TH1D*)hEMCal3x3_FINE->ProjectionY(Form("hEMCal3x3_FINE_py__%i_%iGeV",ptlo,pthi),ptbin,ptbin);
//    hEMCal3x3_FINE_py[i-1]->Scale(1./hEMCal3x3_FINE_py[i-1]->Integral());
    hEMCal3x3_FINE_py[i-1]->SetTitle(Form("%i-%i GeV/c pion",ptlo,pthi));
    hEMCal3x3_FINE_py[i-1]->SetStats(0);
    hEMCal3x3_FINE_py[i-1]->GetYaxis()->SetRangeUser(0.0,80.0);
    hEMCal3x3_FINE_py[i-1]->Draw("SAMEPLCPMC");
    hEMCal3x3_FINE_py[i-1]->Fit(f1,"","",0.,0.6);
    MIP_value[i-1] = f1->GetMaximumX();
  }
  
  float mean_value = 0;
  for (int i=0; i<29; ++i) {
    cout<<i+1<<"-"<<i+2<<"GeV/c \t"<<MIP_value[i]<<endl;
    mean_value += MIP_value[i];
  }
  mean_value/=29.;
  cout<<endl<<mean_value<<endl;
  
  TLegend *leg0;
  
  leg0 = new TLegend(0.7, 0.3, 0.98, 0.98,NULL,"brNDC");    // LEGEND 0
  leg0->SetBorderSize(0);  leg0->SetLineColor(1);  leg0->SetLineStyle(1);
  leg0->SetLineWidth(1);
//  leg0->SetFillColorAlpha(0,0.0);
//  leg0->SetFillStyle(1001);
  leg0->SetNColumns(2);
  leg0->SetHeader("p_{T}^{pion}\t\t\tGaussian Mean","");
//    leg0->AddEntry((TObject*)0,"EA_{Low} \t", "");
//    leg0->AddEntry((TObject*)0,"\tEA_{High}", "");
  
  for (int i=0; i<29; ++i) {
    string title = "";
    leg0->AddEntry(hEMCal3x3_FINE_py[i],Form("%i-%i GeV/c",i+1,i+2),"lpf");
    leg0->AddEntry("",Form("%f",MIP_value[i]),"");
  }
  leg0->Draw("SAME");
  
}

