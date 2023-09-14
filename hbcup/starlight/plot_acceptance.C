// 
// Make combined acceptance plots of all 3 UPC Modes
//
//

void plot_acceptance()
{
  // Open up the 3 files with the saved histograms
  // and get the histograms
  const int NMODES = 3;
  const char* mode_dir[NMODES] = { "PROD1", "PROD2", "PROD4" };
  TFile *tfile[NMODES];
  TString name;

  TH1 *h_rap[NMODES];
  TH1 *h_rap_sphenix[NMODES];
  TH1 *h_rap_total;               // all 3 modes summed
  TH1 *h_rap_sphenix_total;

  // Loop over the prod_modes
  for (int imode=0; imode<NMODES; imode++)
  {
    // Open up the TFile
    name = mode_dir[imode]; name += "/upc_starlight.root";
    tfile[imode] = new TFile(name,"READ");

    // Now get the histograms
    h_rap[imode] = (TH1*)tfile[imode]->Get("h_rap");
    h_rap_sphenix[imode] = (TH1*)tfile[imode]->Get("h_rap_sphenix");
  }

  // Create the total histogram (sum of all 3 modes)
  h_rap_total = (TH1*)h_rap[0]->Clone("h_rap_total");
  h_rap_total->Add( h_rap[1] );
  h_rap_total->Add( h_rap[2] );

  h_rap_sphenix_total = (TH1*)h_rap[0]->Clone("h_rap_sphenix_total");
  h_rap_sphenix_total->Add( h_rap_sphenix[1] );
  h_rap_sphenix_total->Add( h_rap_sphenix[2] );

  // Plot the rapidity acceptance from all 3 modes combined
  TCanvas *c_rap = new TCanvas("c_rap","Rapidity",550,425);
  h_rap_total->Draw("ehist");
  h_rap_sphenix_total->SetLineColor(kBlue);
  h_rap_sphenix_total->SetMarkerColor(kBlue);
  h_rap_sphenix_total->Draw("ehistsame");
}

