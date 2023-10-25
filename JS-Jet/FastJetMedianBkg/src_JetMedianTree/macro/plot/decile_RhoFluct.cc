#define RhoFluct_cxx
#include "RhoFluct.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void RhoFluct::Loop(string out_name)
{
//   In a ROOT session, you can do:
//      root> .L RhoFluct.C
//      root> RhoFluct t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   TFile* fout = new TFile(voi_stem(out_name,".root").c_str(),"recreate");

   array<TH1D*,10> harr_sub1, harr_rhoA;
   for (int i=0; i<10; ++i) {
     int i0 = i*10;
     int i1 = i*10+10;
     harr_sub1[i] = new TH1D(Form("fluct_sub1_%i",i),
         Form("impact param [%4.2f-%4.2f];p_{T,jet}^{SUB1+probe}-p_{T}^{probe};N_{events}", IP_DEC[i], IP_DEC[i+1]), 400, -50, 50);
     harr_rhoA[i] = new TH1D(Form("fluct_rhoA_%i",i),
         Form("impact param [%4.2f-%4.2f];(p_{T,jet}^{calo+probe}-#rho#timesA)-p_{T}^{probe};N_{events}", IP_DEC[i], IP_DEC[i+1]), 400, -50, 50);
   }

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      int k = get_ip_decile(impactparam);
      if (k<0 || k>9) {
        cout << "bad impact parameter; should NEVER happen" << endl;
      }

      ///////////////////////////////////////
      // Do the sub1 fluctuations
      if (sub1_ismatched) {
        harr_sub1[k]->Fill(sub1Jet_delPt);
      }
      if (rhoA_ismatched) {
        harr_rhoA[k]->Fill(rhoAJet_delPt);
      }

      /* if (true) if (jentry > 1000) break; */
   }

   for (int k=0;k<10;++k) {
     harr_sub1[k]->Write();
     harr_rhoA[k]->Write();
   }
   fout->Close();
}
