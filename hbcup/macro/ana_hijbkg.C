// This macro reads and analyzes the TTree created in hijbkg_upc.cc
//

#include <cmath>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>        // Include the TMath header for trigonometric functions
#include <TH1F.h>
#include <TCanvas.h>
#include <TParticle.h>
#include <TDatabasePDG.h>

void ana_hijbkg(const char* filename = "hijbkg.root")
{
  TDatabasePDG *pdg = new TDatabasePDG;

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

  TFile *savefile = new TFile("hijbkg_results.root","RECREATE");

  // Plot histograms for pt, eta, phi, mass
  TH1F *h_pt = new TH1F("h_pt", "Pt Distribution", 200, 0, 2.0 ); // Assuming pt values are within the range [0, 100]
  TH1F *h_eta = new TH1F("h_eta", "Eta Distribution", 200, -6, 6 ); // Assuming eta values are within the range [-3, 3]
  TH1F *h_phi = new TH1F("h_phi", "Phi Distribution", 100, -M_PI, M_PI ); // Assuming phi values are within the range [-pi, pi]
  TH1F *h_mass = new TH1F("h_mass", "Inv Mass", 200, 0., 6.);
  h_pt->SetLineColor(4);
  h_eta->SetLineColor(4);
  h_phi->SetLineColor(4);
  h_mass->SetLineColor(4);

  // Assuming these are electrons
  TH1F *he_pt = new TH1F("he_pt", "Pt Distribution", 200, 0, 2.0 ); // Assuming pt values are within the range [0, 100]
  TH1F *he_eta = new TH1F("he_eta", "Eta Distribution", 200, -6, 6 ); // Assuming eta values are within the range [-3, 3]
  TH1F *he_phi = new TH1F("he_phi", "Phi Distribution", 100, -M_PI, M_PI ); // Assuming phi values are within the range [-pi, pi]
  TH1F *he_mass = new TH1F("he_mass", "Inv Mass", 200, 0., 6.);

  TLorentzVector part_v4[2];
  TLorentzVector sum_v4;

  TLorentzVector epart_v4[2]; // assuming these are e+/e- pairs
  TLorentzVector esum_v4;

  // Loop over the tree  
  Int_t nEntries = tree->GetEntries();
  for (int iEntry = 0; iEntry < nEntries; iEntry++)
  {
    tree->GetEntry(iEntry);

    // Print out a message every 1000 events
    if ( iEntry%10000 == 0 ) cout << iEntry << "\t" << m_evt << endl;

    //cout << m_part[0]->GetPdgCode() << "\t" << m_part[0]->Px() << endl;

    Double_t charge[2];
    for (int ipart=0; ipart<2; ipart++)
    {
      charge[ipart] = m_part[ipart]->GetPDG()->Charge();

      if ( fabs( m_part[ipart]->GetPDG()->PdgCode() ) == 11 )
      {
        cout << m_part[ipart]->GetPDG()->PdgCode() << "\t" << charge[ipart] << endl;
      }
 
      double px = m_part[ipart]->Px();
      double py = m_part[ipart]->Py();
      double pz = m_part[ipart]->Pz();

      part_v4[ipart].SetPxPyPzE( px, py, pz, m_part[ipart]->Energy() );

      // now assume this is an electron
      const double m_e = 0.511e-3;    // e- mass in GeV
      double e_energy = sqrt( px*px + py*py + pz*pz + m_e*m_e );
      epart_v4[ipart].SetPxPyPzE( px, py, pz, e_energy );


      // check for bad eta
      if ( fabs(part_v4[ipart].Eta()) > 1.1 )
      {
        cout << "ERROR " << iEntry << m_part[ipart]->GetPdgCode() << endl;
      }
    }

    // skip if they are like sign pairs
    if ( charge[0]*charge[1] > 0 )
    {
      continue;
    }
 
    sum_v4 = part_v4[0] + part_v4[1];
    esum_v4 = epart_v4[0] + epart_v4[1];

    // Fill the histograms with the data from the TTree
    h_pt->Fill( sum_v4.Pt()  );
    h_eta->Fill( sum_v4.Eta() );
    h_phi->Fill( sum_v4.Phi() );
    h_mass->Fill( sum_v4.M() );

    // electron PID assumption
    he_pt->Fill( esum_v4.Pt()  );
    he_eta->Fill( esum_v4.Eta() );
    he_phi->Fill( esum_v4.Phi() );
    he_mass->Fill( esum_v4.M() );
  }     

  // Draw and save the histograms
  TCanvas *ac = new TCanvas("ac","pt",550,425);
  h_pt->Draw();
  he_pt->Draw("same");
  gPad->SetLogy(1);
  ac->SaveAs("h_pt.png");

  TCanvas *dc = new TCanvas("dc","eta",550,425);
  h_eta->Draw();
  he_eta->Draw("same");
  dc->SaveAs("h_eta.png");

  TCanvas *bc = new TCanvas("bc","phi",550,425);
  h_phi->Draw();
  he_phi->Draw("same");
  bc->SaveAs("h_phi.png");

  TCanvas *ec = new TCanvas("ec","mass",550,425);
  h_mass->Draw();
  he_mass->Draw("same");
  gPad->SetLogy(1);
  ec->SaveAs("h_mass.png");

  savefile->Write();
}
