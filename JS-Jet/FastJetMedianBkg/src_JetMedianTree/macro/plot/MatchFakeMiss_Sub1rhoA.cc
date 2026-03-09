#define Sub1rhoA_cxx
#include "Sub1rhoA.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Sub1rhoA::Loop(string str_inp)
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();


   
   TFile* fout = new TFile(voi_stem(str_inp,".root").c_str(),"recreate");

   array<TH1D*,10> harr_sub1, harr_rhoA;


   array<TH1D*,10> fake_sub1, fake_rhoA, miss_sub1, miss_rhoA;
   array<TH2D*,10> match_sub1, match_rhoA;

   int   nbins_fake = 40;
   float lobin_fake = -20.;
   float hibin_fake = 80.;
   int   nbins_miss = 40;
   float lobin_miss = -20.;
   float hibin_miss = 80.;


   for (int i=0; i<10; ++i) {
     int i0 = i*10;
     int i1 = i*10+10;

     fake_sub1[i] = new TH1D(Form("fake_sub1_%i",i), Form("cent: %i-%i%% MBC Cent;N_{jets} Fake;p_{T}^{SUB1}",i0, i1), nbins_fake, lobin_fake, hibin_fake);
     fake_rhoA[i] = new TH1D(Form("fake_rhoA_%i",i), Form("cent: %i-%i%% MBC Cent;N_{jets} Fake;p_{T}-#rho#timesA",i0, i1), nbins_fake, lobin_fake, hibin_fake);

     miss_sub1[i] = new TH1D(Form("miss_sub1_%i",i), Form("cent: %i-%i%% MBC Cent;N_{jets} miss;p_{T}^{truth}",i0, i1), nbins_miss, lobin_miss, hibin_miss);
     miss_rhoA[i] = new TH1D(Form("miss_rhoA_%i",i), Form("cent: %i-%i%% MBC Cent;N_{jets} miss;p_{T}^{truth}",i0, i1), nbins_miss, lobin_miss, hibin_miss);

     match_sub1[i] = new TH2D(Form("match_sub1_%i",i), Form("cent: %i-%i%% MBC Cent;p_{T}^{SUB1};p_{T}^{truth}",i0, i1),      nbins_fake, lobin_fake, hibin_fake, nbins_miss, lobin_miss, hibin_miss);
     match_rhoA[i] = new TH2D(Form("match_rhoA_%i",i), Form("cent: %i-%i%% MBC Cent;p_{T}-#rho#timesA;p_{T}^{truth}",i0, i1), nbins_fake, lobin_fake, hibin_fake, nbins_miss, lobin_miss, hibin_miss);
   }

   JetIndicesMatcher jetmatcher{0.4, 30., 5.};

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
        if (T<30) {
            cout << " WARNING WHY HERE!!! A0" << endl;
            continue;
        }
        auto M = (*sub1JetPt)[pp.second];
        match_sub1[k] ->Fill(M,T);
      }
      for (auto F : jetmatcher.indices_fake) {
        auto M = (*sub1JetPt)[F];
        fake_sub1[k]->Fill(M);
      }
      for (auto M : jetmatcher.indices_miss) {
        auto T = (*TruthJetPt)[M];
        miss_sub1[k]->Fill(T);
      }


      jetmatcher.reset();
      jetmatcher.add_truth (*TruthJetEta, *TruthJetPhi, *TruthJetPt);
      jetmatcher.add_reco  (*rhoAJetEta,  *rhoAJetPhi, *rhoAJetPtLessRhoA);
      jetmatcher.do_matching();

      for (auto pp : jetmatcher.match) {
        auto T = (*TruthJetPt)[pp.first];
        if (T<30) {
            cout << " WARNING WHY HERE!!! A1" << endl;
            continue;
        }
        auto M = (*rhoAJetPtLessRhoA)[pp.second];
        match_rhoA[k]->Fill(M,T);
      }

      for (auto F : jetmatcher.indices_fake) {
        auto pt = (*rhoAJetPtLessRhoA)[F];
        /* if (pt<6) cout << " pt["<<F<<"]: " << pt << endl; */
        fake_rhoA[k]->Fill(pt);
      }

      for (auto M : jetmatcher.indices_miss) {
        auto pt = (*TruthJetPt)[M];
        miss_rhoA[k]->Fill(pt);
      }

      // if (Cut(ientry) < 0) continue;
      if (false) if (jentry>10) {
        cout << " Breaking on jentry(" << jentry << ")" << endl;
        break;
      }
   }

   for (int k=0;k<10;++k) {
     fake_sub1[k]->Write();
     miss_sub1[k]->Write();
     match_sub1[k]->Write();
     fake_rhoA[k]->Write();
     miss_rhoA[k]->Write();
     match_rhoA[k]->Write();
   }
   fout->Close();
}
