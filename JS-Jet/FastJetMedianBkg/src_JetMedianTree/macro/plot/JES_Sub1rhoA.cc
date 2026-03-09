#define Sub1rhoA_cxx
#include "Sub1rhoA.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Sub1rhoA::Loop(string str_inp)
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();


   JetIndicesMatcher jetmatcher{0.4, 30., 5.};
   
   TFile* fout = new TFile(voi_stem(str_inp,".root").c_str(),"recreate");

   array<TH1D*,10> harr_sub1, harr_rhoA;
   for (int i=0; i<10; ++i) {
     int i0 = i*10;
     int i1 = i*10+10;
     harr_sub1[i] = new TH1D(Form("JES_sub1_%i",i),
         Form("JES %i-%i%% MBD Centrality;p_{T}_{truth};#frac{p_{T}^{jet,calo}_{SUB1}-p_{T}^{truth}}{p_{T}^{truth}}",
           i0,i1), 300, -3., 3.);
     harr_rhoA[i] = new TH1D(Form("JES_rhoA_%i",i),
         Form("JES %i-%i%% MBD Centrality;p_{T}_{truth};#frac{(p_{T}^{jet,calo}-#rho^{md.bkgd}A_{jet})-p_{T}^{truth}}{p_{T}^{truth}}",
           i0,i1), 300, -3., 3.);
   }


   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      int k = ((int) cent_mdb)/10;
      if (k < 0 || k > 9) { 
        cout << " WARNING centrality in un expected bin" << endl;
        continue;
      }

      // match the sub1 jets
      jetmatcher.reset();
      jetmatcher.add_truth (*TruthJetEta, *TruthJetPhi, *TruthJetPt);
      jetmatcher.add_reco  (*sub1JetEta,  *sub1JetPhi, *sub1JetPt);
      jetmatcher.do_matching();

      for (auto pp : jetmatcher.match) {
        auto T = (*TruthJetPt)[pp.first];
        if (T<30) continue;
        auto M = (*sub1JetPt)[pp.second];
        harr_sub1[k]->Fill((M-T)/T);
      }

      // match rhoA jets
      jetmatcher.reset();
      jetmatcher.add_truth (*TruthJetEta, *TruthJetPhi, *TruthJetPt);
      jetmatcher.add_reco  (*rhoAJetEta,  *rhoAJetPhi, *rhoAJetPtLessRhoA);
      jetmatcher.do_matching();

      for (auto pp : jetmatcher.match) {
        auto T = (*TruthJetPt)[pp.first];
        if (T<30) continue;
        auto M = (*rhoAJetPtLessRhoA)[pp.second];
        harr_rhoA[k]->Fill((M-T)/T);
      }

      // if (Cut(ientry) < 0) continue;
      if (false) if (jentry>10) {
        cout << " Breaking on jentry(" << jentry << ")" << endl;
        break;
      }
   }

   for (int k=0;k<10;++k) {
     harr_sub1[k]->Write();
     harr_rhoA[k]->Write();
   }
   fout->Close();
}
