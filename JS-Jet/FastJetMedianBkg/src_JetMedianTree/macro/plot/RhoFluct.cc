root -l hadd_RhoFluct.root <<EOF


  TH1D* hparamt = new TH1D("hparamt", ";impactparam;N_{events}", 10000, 0.,22);
  double x[11], p[11];
  for (int i=0;i<11;++i) {
    x[i] = .1*i;
  }
  T->Draw("impactparamt>>hparamt");
  hparamt->GetQuantiles(11,x,p);
  for (int i=0;i<11;++i) {
    cout << " i("<<i<<") x("<<x[i]<<") q("<<q[i]<<")"<<endl;
  }

EOF
#define RhoFluct_cxx
#include "RhoFluct.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void RhoFluct::Loop()
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

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}
