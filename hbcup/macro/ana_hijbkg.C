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
  TH1F *h_pt = new TH1F("h_pt", "Pt Distribution", 200, 0, 2.0 );
  TH1F *h_eta = new TH1F("h_eta", "Eta Distribution", 200, -6, 6 );
  TH1F *h_rap = new TH1F("h_rap", "Rapidity Distribution", 200, -4, 4 );
  TH1F *h_phi = new TH1F("h_phi", "Phi Distribution", 100, -M_PI, M_PI );
  TH1F *h_dphi = new TH1F("h_dphi", "dPhi Distribution", 100, -M_PI, M_PI );
  TH1F *h_mass = new TH1F("h_mass", "Inv Mass", 200, 0., 6.);
  h_pt->SetLineColor(4);
  h_eta->SetLineColor(4);
  h_phi->SetLineColor(4);
  h_mass->SetLineColor(4);

  // Assuming all tracks are electrons
  TH1F *he_pt = new TH1F("he_pt", "Pt, assuming e+/e-", 200, 0, 2.0 );
  TH1F *he_eta = new TH1F("he_eta", "Eta, assuming e+/e- ", 200, -6, 6 );
  TH1F *he_rap = new TH1F("he_rap", "Rapidity, assuming e+/e-", 200, -4, 4 );
  TH1F *he_phi = new TH1F("he_phi", "Phi, assuming e+/e- ", 100, -M_PI, M_PI );
  TH1F *he_dphi = new TH1F("he_dphi", "dPhi, assuming e+/e- ", 100, -M_PI, M_PI );
  TH1F *he_mass = new TH1F("he_mass", "Mass, assuming e+/e-", 200, 0., 6.);

  // With cut on J/Psi mass
  TH1F *hej_pt = new TH1F("hej_pt", "Pt, J/Psi mass cut", 200, 0, 2.0 );
  TH1F *hej_eta = new TH1F("hej_eta", "Eta, J/Psi mass cut ", 200, -6, 6 );
  TH1F *hej_rap = new TH1F("hej_rap", "Rapidity, J/Psi mass cut", 200, -4, 4 );
  TH1F *hej_dphi = new TH1F("hej_dphi", "dPhi, J/Psi mass cut", 100, -M_PI, M_PI );
  hej_pt->SetLineColor(2);
  hej_eta->SetLineColor(2);
  hej_rap->SetLineColor(2);
  hej_dphi->SetLineColor(2);
  hej_pt->SetXTitle("#p_T (GeV/c)");

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
    h_rap->Fill( sum_v4.Rapidity() );
    h_dphi->Fill( part_v4[0].DeltaPhi( part_v4[1] ) );

    // electron PID assumption
    he_pt->Fill( esum_v4.Pt()  );
    he_eta->Fill( esum_v4.Eta() );
    he_phi->Fill( esum_v4.Phi() );
    he_mass->Fill( esum_v4.M() );
    he_rap->Fill( esum_v4.Rapidity() );
    he_dphi->Fill( epart_v4[0].DeltaPhi( epart_v4[1] ) );

    // in J/Psi mass window
    if ( fabs( esum_v4.M() - 3.1 ) < 0.3  ) 
    {
      hej_pt->Fill( esum_v4.Pt()  );
      hej_eta->Fill( esum_v4.Eta() );
      hej_rap->Fill( esum_v4.Rapidity() );
      hej_dphi->Fill( epart_v4[0].DeltaPhi( epart_v4[1] ) );
    }
  }     

  // Draw and save the histograms
  TCanvas *ac = new TCanvas("ac","pt",550,425);
  h_pt->SetMinimum(0.5);
  h_pt->Draw();
  he_pt->Draw("same");
  hej_pt->Draw("same");
  gPad->SetLogy(1);
  ac->SaveAs("h_pt.png");

  TCanvas *dc = new TCanvas("dc","eta",550,425);
  h_eta->SetMinimum(0.5);
  h_eta->Draw();
  he_eta->Draw("same");
  hej_eta->Draw("same");
  dc->SaveAs("h_eta.png");

  TCanvas *fc = new TCanvas("fc","rapidity",550,425);
  h_rap->SetMinimum(0.5);
  h_rap->Draw();
  he_rap->Draw("same");
  hej_rap->Draw("same");
  dc->SaveAs("h_eta.png");

  TCanvas *bc = new TCanvas("bc","phi",550,425);
  h_phi->SetMinimum(0.5);
  h_phi->Draw();
  he_phi->Draw("same");
  bc->SaveAs("h_phi.png");

  TCanvas *gc = new TCanvas("gc","dphi",550,425);
  h_dphi->SetMinimum(0.5);
  h_dphi->Draw();
  he_dphi->Draw("same");
  hej_dphi->Draw("same");
  dc->SaveAs("h_eta.png");

  TCanvas *ec = new TCanvas("ec","mass",550,425);
  h_mass->SetMinimum(0.5);
  h_mass->Draw();
  he_mass->Draw("same");
  gPad->SetLogy(1);
  ec->SaveAs("h_mass.png");

  savefile->Write();
}
