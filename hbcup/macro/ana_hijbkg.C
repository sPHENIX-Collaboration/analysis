// This macro reads and analyzes the TTree created in hijbkg_upc.cc
//

#include <cmath>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>        // Include the TMath header for trigonometric functions
#include <TH1F.h>
#include <TCanvas.h>
#include <TParticle.h>

void ana_hijbkg(const char* filename = "hijbkg.root")
{
  TFile *tfile= new TFile(filename,"READ");
  TTree* tree = (TTree*)tfile->Get("T"); // the TTree in the HIJING file

  vector<float> *m_pt = 0;
  Int_t m_evt = 0;
  Float_t m_b = 0;
  Float_t m_cent = 0;
  TParticle *m_part[2] = {0};

  //set the branches of the tree
  tree->SetBranchAddress("evt", &m_evt);
  tree->SetBranchAddress("b", &m_b);
  tree->SetBranchAddress("part0", &m_part[0]);
  tree->SetBranchAddress("part1", &m_part[1]);

  // Plot histograms for pt, eta, phi, mass
  TH1F *h_pt = new TH1F("h_pt", "Pt Distribution", 160, 0, 8.0 ); // Assuming pt values are within the range [0, 100]
  TH1F *h_eta = new TH1F("h_eta", "Eta Distribution", 100, -5, 5 ); // Assuming eta values are within the range [-3, 3]
  TH1F *h_phi = new TH1F("h_phi", "Phi Distribution", 100, -M_PI, M_PI ); // Assuming phi values are within the range [-pi, pi]
  TH1F *h_mass = new TH1F("h_mass", "Inv Mass", 600, 0., 6.);

  TLorentzVector part_v4[2];
  TLorentzVector sum_v4;

  // Loop over the tree  
  Int_t nEntries = tree->GetEntries();
  for (int iEntry = 0; iEntry < nEntries; iEntry++)
  {
    tree->GetEntry(iEntry);

    // Print out a message every 1000 events
    if ( iEntry%1000 == 0 ) cout << iEntry << "\t" << m_evt << endl;

    //cout << m_part[0]->GetPdgCode() << "\t" << m_part[0]->Px() << endl;

    for (int ipart=0; ipart<2; ipart++)
    {
      part_v4[ipart].SetPxPyPzE( m_part[ipart]->Px(), m_part[ipart]->Py(), m_part[ipart]->Pz(), m_part[ipart]->Energy() );
    }

    sum_v4 = part_v4[0] + part_v4[1];

    // Fill the histograms with the data from the TTree
    h_pt->Fill( sum_v4.Pt()  );
    h_eta->Fill( sum_v4.Eta() );
    h_phi->Fill( sum_v4.Phi() );
    h_mass->Fill( sum_v4.M() );

  }     

  // Draw and save the histograms
  TCanvas *ac = new TCanvas("ac","pt",550,425);
  h_pt->Draw();
  gPad->SetLogy(1);
  ac->SaveAs("h_pt.png");

  TCanvas *dc = new TCanvas("dc","eta",550,425);
  h_eta->Draw();
  dc->SaveAs("h_eta.png");

  TCanvas *bc = new TCanvas("bc","phi",550,425);
  h_phi->Draw();
  bc->SaveAs("h_phi.png");

  TCanvas *ec = new TCanvas("ec","mass",550,425);
  h_mass->Draw();
  gPad->SetLogy(1);
  ec->SaveAs("h_mass.png");

}
