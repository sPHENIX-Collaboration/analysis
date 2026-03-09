// plot the total (signal plus bkg) that we expect, where signal = UPC (starlight) and background = Au+Au collisions (Hijing)


void plot_total_plusbkg()
{
  gStyle->SetOptStat(0);

  TFile *upcfile = new TFile("AcceptanceMRP.root","READ");
  TH1 *h_InvMass_smeared_sphenix_total = (TH1*)upcfile->Get("h_InvMass_smeared_sphenix_total");
  TH1 *h_pt_sphenix_total = (TH1*)upcfile->Get("h_pt_sphenix_total");
  TH1 *h_rapt_sphenix_total = (TH1*)upcfile->Get("h_rapt_sphenix_total");

  TFile *hijbkgfile = new TFile("hijbkg_results.root","READ");
  TH1 *he_mass = (TH1*)hijbkgfile->Get("he_mass");
  TH1 *he_eta = (TH1*)hijbkgfile->Get("he_eta");      // should be rapidity
  TH1 *he_pt = (TH1*)hijbkgfile->Get("he_pt");

  // Scale to integ lumi = 4.7/nb
  double nevt_hij = 3333315. + 3716246. + 3313128.;   // number of hijing events simulated
  double sigma_auau = 7.2;      // au+au cross-section [barns]
  double integ_lumi = 4.7e9;   // target integrated luminosity
  double lumi_scale = (sigma_auau*integ_lumi)/nevt_hij;
  cout << "lumi scaled " << lumi_scale << endl;
  he_mass->Sumw2();
  he_mass->Scale( lumi_scale );

  float markersize = 0.7;
  
  // Kludge - take out the first 0.6 GeV of mass
  int minbin = he_mass->FindBin( 0.6 );
  for ( int ibin=1; ibin<=minbin; ibin++ )
  {
    he_mass->SetBinContent( ibin, 0. );
  }

  // Add the UPC total to the background
  TH1 *h_InvMass_smeared_sphenix_totalbkg = (TH1*)h_InvMass_smeared_sphenix_total->Clone("h_InvMass_smeared_sphenix_totalbkg");
  h_InvMass_smeared_sphenix_totalbkg->Add( he_mass );

  TCanvas *c_mass_total = new TCanvas("c_mass_total","Invariant Mass",550,425);
  h_InvMass_smeared_sphenix_totalbkg->SetLineColor(kBlack);
  h_InvMass_smeared_sphenix_totalbkg->SetMarkerColor(kBlack);
  h_InvMass_smeared_sphenix_totalbkg->SetMarkerStyle(20);
  h_InvMass_smeared_sphenix_totalbkg->SetMarkerSize( markersize );
  h_InvMass_smeared_sphenix_totalbkg->Draw("ehist");

  he_mass->SetLineColor(kRed);
  he_mass->SetMarkerColor(kRed);
  he_mass->SetMarkerSize( markersize );
  he_mass->SetMarkerStyle( 20 );
  he_mass->Draw( "ecsame" );

  h_InvMass_smeared_sphenix_total->SetLineColor(kBlue);
  h_InvMass_smeared_sphenix_total->SetMarkerColor(kBlue);
  h_InvMass_smeared_sphenix_total->SetMarkerStyle( 20 );
  h_InvMass_smeared_sphenix_total->SetMarkerSize( markersize );
  h_InvMass_smeared_sphenix_total->SetFillStyle( 1001 );
  h_InvMass_smeared_sphenix_total->SetFillColor( 7 );
  h_InvMass_smeared_sphenix_total->Draw("ehistsame");

  h_InvMass_smeared_sphenix_totalbkg->Draw("ehistsame");
  //gPad->SetLogy( 1 );

  // Calculate the total counts in the j/psi mass windows
  int minjbin = he_mass->FindBin( 3.2 );
  int maxjbin = he_mass->FindBin( 3.31 );
  double upcj = 0.;     // num upc j/psi
  double upcjerr = 0.;  // err on upc j/psi counts
  double hijj = 0.;     // num hij with 2 tracks in j/psi mass window
  double hijjerr = 0.;  // err on hij counts

  for (int ibin=minjbin; ibin<=maxjbin; ibin++)
  {
    upcj += h_InvMass_smeared_sphenix_total->GetBinContent( ibin );
    upcjerr += h_InvMass_smeared_sphenix_total->GetBinError( ibin )*he_mass->GetBinError( ibin );

    hijj += he_mass->GetBinContent( ibin );
    hijjerr += he_mass->GetBinError( ibin )*he_mass->GetBinError( ibin );
  }

  upcjerr = sqrt(upcjerr);
  hijjerr = sqrt(hijjerr);
  cout << "upcj\tupcjerr\thijj\thijjerr" << endl;
  cout << upcj << "\t" << upcjerr << "\t" << hijj << "\t" << hijjerr << endl;
  cout << "S/N = " << upcj/hijj << endl;
}

