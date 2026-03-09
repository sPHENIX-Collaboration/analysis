//
//
//
//

void AcceptanceMRP()
{
  gStyle->SetOptStat(0);

  // Open up the 3 files with the saved histograms
  // and get the histograms
  const int NMODES = 3;
  const char* mode_dir[NMODES] = { "PROD1", "PROD2", "PROD4" };
  TFile *tfile[NMODES];
  TString name;

  TH1 *h_rapt[NMODES];
  TH1 *h_rapt_sphenix[NMODES];
  TH1 *h_rapt_total;               // all 3 modes summed
  TH1 *h_rapt_sphenix_total;

  TH1 *h_InvMass[NMODES];
  TH1 *h_InvMass_sphenix[NMODES];
  TH1 *h_InvMass_total;               // all 3 modes summed
  TH1 *h_InvMass_sphenix_total;

  TH1 *h_InvMass_smeared[NMODES];
  TH1 *h_InvMass_smeared_sphenix[NMODES];
  TH1 *h_InvMass_smeared_total;               // all 3 modes summed
  TH1 *h_InvMass_smeared_sphenix_total;

  TH1 *h_pt[NMODES];
  TH1 *h_pt_sphenix[NMODES];
  TH1 *h_pt_total;               // all 3 modes summed
  TH1 *h_pt_sphenix_total;


  // Loop over the prod_modes
  for (int imode=0; imode<NMODES; imode++)
  {
    // Open up the TFile
    name = mode_dir[imode]; name += "/upc_starlight.root";
    tfile[imode] = new TFile(name,"READ");

    // Now get the histograms rapidity
    h_rapt[imode] = (TH1*)tfile[imode]->Get("h_rapt");
    h_rapt_sphenix[imode] = (TH1*)tfile[imode]->Get("h_rapt_sphenix");

    // Now get the histograms Inv Mass
    h_InvMass[imode] = (TH1*)tfile[imode]->Get("h_InvMass");
    h_InvMass_sphenix[imode] = (TH1*)tfile[imode]->Get("h_InvMass_sphenix");

    // Now get the histograms Inv Mass with momentum resolution smearing
    h_InvMass_smeared[imode] = (TH1*)tfile[imode]->Get("h_InvMass_smeared");
    h_InvMass_smeared_sphenix[imode] = (TH1*)tfile[imode]->Get("h_InvMass_smeared_sphenix");

    // Now get the histograms transverse momentum
    h_pt[imode] = (TH1*)tfile[imode]->Get("h_pt");
    h_pt_sphenix[imode] = (TH1*)tfile[imode]->Get("h_pt_sphenix");

  }

  TFile *savefile = new TFile("AcceptanceMRP.root","RECREATE");

  // Create the total rapidity (sum of all 3 modes)
  h_rapt_total = (TH1*)h_rapt[1]->Clone("h_rapt_total");
  h_rapt_total->Add( h_rapt[0] );
  h_rapt_total->Add( h_rapt[2] );

  h_rapt_sphenix_total = (TH1*)h_rapt_sphenix[0]->Clone("h_rapt_sphenix_total");
  h_rapt_sphenix_total->Add( h_rapt_sphenix[1] );
  h_rapt_sphenix_total->Add( h_rapt_sphenix[2] );

  float markersize = 0.7;

  // Plot the rapidity acceptance from all 3 modes combined
  TCanvas *c_rap = new TCanvas("c_rap","Rapidity",550,425);
  //h_rapt_total->Draw("ehist");
  h_rapt_sphenix_total->SetLineColor(kBlack);
  h_rapt_sphenix_total->SetMarkerColor(kBlack);
  h_rapt_sphenix_total->SetMarkerSize( markersize );
  h_rapt_sphenix_total->Draw("ehist");
  h_rapt_sphenix[0]->SetLineColor(kBlue);
  h_rapt_sphenix[0]->SetMarkerColor(kBlue);
  h_rapt_sphenix[0]->SetMarkerSize( markersize );
  h_rapt_sphenix[0]->Draw("ehistsame");
  h_rapt_sphenix[1]->SetLineColor(kRed);
  h_rapt_sphenix[1]->SetMarkerColor(kRed);
  h_rapt_sphenix[1]->SetMarkerSize( markersize );
  h_rapt_sphenix[1]->Draw("ehistsame");
  h_rapt_sphenix[2]->SetLineColor(kGreen);
  h_rapt_sphenix[2]->SetMarkerColor(kGreen);
  h_rapt_sphenix[2]->SetMarkerSize( markersize );
  h_rapt_sphenix[2]->Draw("ehistsame");
  gPad->SetLogy(1);

  //Create the total invariant mass (sum of all 3 modes)
  h_InvMass_total = (TH1*)h_InvMass[0]->Clone("h_InvMass_total");
  h_InvMass_total->Add( h_InvMass[1] );
  h_InvMass_total->Add( h_InvMass[2] );

  h_InvMass_sphenix_total = (TH1*)h_InvMass[0]->Clone("h_InvMass_sphenix_total");
  h_InvMass_sphenix_total->Add( h_InvMass_sphenix[1] );
  h_InvMass_sphenix_total->Add( h_InvMass_sphenix[2] );

  // Plot the mass acceptance from all 3 modes combined
  TCanvas *d_rap = new TCanvas("d_InvMass","Invariant Mass",550,425);
  h_InvMass_sphenix_total->SetLineColor(kBlack);
  h_InvMass_sphenix_total->SetMarkerColor(kBlack);
  h_InvMass_sphenix_total->SetMarkerSize( markersize );
  h_InvMass_sphenix_total->Draw("ehist");
  h_InvMass_sphenix[0]->SetLineColor(kBlue);
  h_InvMass_sphenix[0]->SetMarkerColor(kBlue);
  h_InvMass_sphenix[0]->SetMarkerSize( markersize );
  h_InvMass_sphenix[0]->Draw("ehistsame");
  h_InvMass_sphenix[1]->SetLineColor(kRed);
  h_InvMass_sphenix[1]->SetMarkerColor(kRed);
  h_InvMass_sphenix[1]->SetMarkerSize( markersize );
  h_InvMass_sphenix[1]->Draw("ehistsame");
  h_InvMass_sphenix[2]->SetLineColor(kGreen);
  h_InvMass_sphenix[2]->SetMarkerColor(kGreen);
  h_InvMass_sphenix[2]->SetMarkerSize( markersize );
  h_InvMass_sphenix[2]->Draw("ehistsame");
  gPad->SetLogy(1);

  //Create the total smeared invariant mass (sum of all 3 modes)
  h_InvMass_smeared_total = (TH1*)h_InvMass_smeared[0]->Clone("h_InvMass_smeared_total");
  h_InvMass_smeared_total->Add( h_InvMass_smeared[1] );
  h_InvMass_smeared_total->Add( h_InvMass_smeared[2] );

  h_InvMass_smeared_sphenix_total = (TH1*)h_InvMass_smeared_sphenix[0]->Clone("h_InvMass_smeared_sphenix_total");
  h_InvMass_smeared_sphenix_total->Add( h_InvMass_smeared_sphenix[1] );
  h_InvMass_smeared_sphenix_total->Add( h_InvMass_smeared_sphenix[2] );

  // Plot the mass acceptance from all 3 modes combined
  TCanvas *d_rap_smeared = new TCanvas("d_InvMass_smeared","Invariant Mass",550,425);
  h_InvMass_smeared_sphenix_total->SetLineColor(kBlack);
  h_InvMass_smeared_sphenix_total->SetMarkerColor(kBlack);
  h_InvMass_smeared_sphenix_total->SetMarkerSize( markersize );
  h_InvMass_smeared_sphenix_total->Draw("ehist");
  h_InvMass_smeared_sphenix[0]->SetLineColor(kBlue);
  h_InvMass_smeared_sphenix[0]->SetMarkerColor(kBlue);
  h_InvMass_smeared_sphenix[0]->SetMarkerSize( markersize );
  h_InvMass_smeared_sphenix[0]->Draw("ehistsame");
  h_InvMass_smeared_sphenix[1]->SetLineColor(kRed);
  h_InvMass_smeared_sphenix[1]->SetMarkerColor(kRed);
  h_InvMass_smeared_sphenix[1]->SetMarkerSize( markersize );
  h_InvMass_smeared_sphenix[1]->Draw("ehistsame");
  h_InvMass_smeared_sphenix[2]->SetLineColor(kGreen);
  h_InvMass_smeared_sphenix[2]->SetMarkerColor(kGreen);
  h_InvMass_smeared_sphenix[2]->SetMarkerSize( markersize );
  h_InvMass_smeared_sphenix[2]->Draw("ehistsame");
  gPad->SetLogy(1);

  double tot_qed = h_InvMass_smeared_sphenix[0]->Integral();
  double tot_coh = h_InvMass_smeared_sphenix[1]->Integral();
  double tot_incoh = h_InvMass_smeared_sphenix[2]->Integral();
  cout << "Events in sPHENIX" << endl;
  cout << "QED: " << tot_qed << endl;
  cout << "Coh J/Psi: " << tot_coh << endl;
  cout << "Incoh J/Psi: " << tot_incoh << endl;

  //Create the total transverse momentum histogram (sum of all 3 modes)
  h_pt_total = (TH1*)h_pt[0]->Clone("h_pt_total");  
  h_pt_total->Add( h_pt[1] );
  h_pt_total->Add( h_pt[2] );


  h_pt_sphenix_total = (TH1*)h_pt_sphenix[0]->Clone("h_pt_sphenix_total");
  h_pt_sphenix_total->Add( h_pt_sphenix[1] );
  h_pt_sphenix_total->Add( h_pt_sphenix[2] );

  //Plot the transverse momentum acceptance from all 3 modes combined
  TCanvas *e_pt = new TCanvas("e_pt","Transverse Momentum",550,425);
  h_pt_sphenix_total->SetLineColor(kBlack);
  h_pt_sphenix_total->SetMarkerColor(kBlack);
  h_pt_sphenix_total->SetMarkerSize( markersize );
  h_pt_sphenix_total->SetXTitle( "p_{T} (GeV/c)" );
  h_pt_sphenix_total->Draw("ehist");
  h_pt_sphenix[0]->SetLineColor(kBlue);
  h_pt_sphenix[0]->SetMarkerColor(kBlue);
  h_pt_sphenix[0]->SetMarkerSize( markersize );
  h_pt_sphenix[0]->Draw("ehistsame");
  h_pt_sphenix[1]->SetLineColor(kRed);
  h_pt_sphenix[1]->SetMarkerColor(kRed);
  h_pt_sphenix[1]->SetMarkerSize( markersize );
  h_pt_sphenix[1]->Draw("ehistsame");
  h_pt_sphenix[2]->SetLineColor(kGreen);
  h_pt_sphenix[2]->SetMarkerColor(kGreen);
  h_pt_sphenix[2]->SetMarkerSize( markersize );
  h_pt_sphenix[2]->Draw("ehistsame");
  gPad->SetLogy(1);

  savefile->Write();
}

