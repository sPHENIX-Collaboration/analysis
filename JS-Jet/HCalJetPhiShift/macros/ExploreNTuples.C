#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>

const int nTrees = 2;
const string treeNames[nTrees] = {"ntp_gshower", "ntp_tower"};

double delta_phi(double phi1, double phi2){
  double dPhi = phi1 - phi2;
  while (dPhi>=2.*M_PI) { dPhi -= 2.*M_PI; }
  while (dPhi<=-2.*M_PI) { dPhi += 2.*M_PI; }
  return dPhi;
};

void ExploreNTuples() {

  TTree *t_gshower, *t_tower;
  // Variables to store tree branch values
  int t_event, g_event;
  double t_towerID, t_ieta, t_iphi, t_eta, t_phi, t_e, t_x, t_y, t_z;
  double g_gparticleID, g_gflavor, g_gnhits, g_geta, g_gphi, g_ge, g_gpt, g_gvx, g_gvy, g_gvz, g_gembed, g_gedep, g_clusterID, g_ntowers, g_x, g_y, g_phi, g_e, g_efromtruth;


  TH2D *hPhi2D = new TH2D("hPhi2D",";truth #phi;tower #phi",64,-M_PI,M_PI,64,-M_PI,M_PI);
  TH1D *hDeltaPhi = new TH1D("hDeltaPhi",";truth #phi - tower #phi",128,-2.*M_PI,2.*M_PI);
  TH2D *hDeltaPhi_pt = new TH2D("hDeltaPhi_pt",";truth #phi - tower #phi;pion p_{T}",120,-M_PI/6.,M_PI/6.,60,0.,30.);
  TH1D *hGeantPhi = new TH1D("hGeantPhi",";truth #phi",64,-M_PI,M_PI);
  TH1D *hCaloPhi = new TH1D("hCaloPhi",";tower #phi",64,-M_PI,M_PI);
  TH1D *hTowerEt = new TH1D("hTowerEt",";tower E_{T}",60,0.,30.);
  TH2D *hTowerEt_pionEt = new TH2D("hTowerEt_pionEt",";tower E_{T};pion E_{T}",60,0.,30.,60,0.,30.);

  TFile *f = new TFile("out/HCalJetPhiShift_g4hcalout_eval_3k.root","READ");
  t_gshower =(TTree*)f->Get("ntp_gshower");
  t_tower =(TTree*)f->Get("ntp_tower");

  int towEntry = 0;

  for (int iEntry=0; iEntry<t_gshower->GetEntries(); ++iEntry) {

    t_gshower->GetEntry(iEntry);
    g_event = t_gshower->GetLeaf("event")->GetValue();

    bool next_event = false;
    double maxE_tower_phi = 0;
    double max_tower_E = 0.;

    while (towEntry<t_tower->GetEntries() && !next_event) {
      t_tower->GetEntry(towEntry);
      t_event = t_tower->GetLeaf("event")->GetValue();

      if (t_event!=g_event) { next_event = true; }
      else {
//        if (t_tower->GetLeaf("e")->GetValue()>0.1) {
          g_gparticleID = t_gshower->GetLeaf("gparticleID")->GetValue();
          g_gflavor = t_gshower->GetLeaf("gflavor")->GetValue();
          g_gnhits = t_gshower->GetLeaf("gnhits")->GetValue();
          g_geta = t_gshower->GetLeaf("geta")->GetValue();
          g_gphi = t_gshower->GetLeaf("gphi")->GetValue();
          g_ge = t_gshower->GetLeaf("ge")->GetValue();
          g_gpt = t_gshower->GetLeaf("gpt")->GetValue();
          g_gvx = t_gshower->GetLeaf("gvx")->GetValue();
          g_gvy = t_gshower->GetLeaf("gvy")->GetValue();
          g_gvz = t_gshower->GetLeaf("gvz")->GetValue();
          g_gembed = t_gshower->GetLeaf("gembed")->GetValue();
          g_gedep = t_gshower->GetLeaf("gedep")->GetValue();
          g_clusterID = t_gshower->GetLeaf("clusterID")->GetValue();
          g_ntowers = t_gshower->GetLeaf("ntowers")->GetValue();
          g_x = t_gshower->GetLeaf("x")->GetValue();
          g_y = t_gshower->GetLeaf("y")->GetValue();
          g_phi = t_gshower->GetLeaf("phi")->GetValue();
          g_e = t_gshower->GetLeaf("e")->GetValue();
          g_efromtruth = t_gshower->GetLeaf("efromtruth")->GetValue();

          t_ieta = t_tower->GetLeaf("ieta")->GetValue();
          t_iphi = t_tower->GetLeaf("iphi")->GetValue();
          t_eta = t_tower->GetLeaf("eta")->GetValue();
          t_phi = t_tower->GetLeaf("phi")->GetValue();
          t_e = t_tower->GetLeaf("e")->GetValue();
          t_x = t_tower->GetLeaf("x")->GetValue();
          t_y = t_tower->GetLeaf("y")->GetValue();
          t_z = t_tower->GetLeaf("z")->GetValue();
          if (t_e>max_tower_E) {
            maxE_tower_phi = t_phi;
            max_tower_E = t_e;
          }
//        }

      }
      ++towEntry;
    }

      if (max_tower_E>0.) {
        hPhi2D->Fill(g_phi,maxE_tower_phi);
      	hDeltaPhi_pt->Fill(delta_phi(g_phi,maxE_tower_phi),g_gpt);
      	hDeltaPhi->Fill(delta_phi(g_phi,maxE_tower_phi));
      	hGeantPhi->Fill(g_phi);
      	hCaloPhi->Fill(maxE_tower_phi);
      	hTowerEt->Fill(max_tower_E);
        hTowerEt_pionEt->Fill(max_tower_E,g_ge);
    }
  }

  hPhi2D->Draw("COLZ");
  new TCanvas;
  hDeltaPhi_pt->Draw("COLZ");
  new TCanvas;
  hDeltaPhi->Draw("COLZ");
  new TCanvas;
  hGeantPhi->Draw();
  new TCanvas;
  hCaloPhi->Draw();
  new TCanvas;
  hTowerEt->Draw();
  new TCanvas;
  hTowerEt_pionEt->Draw("COLZ");
}
