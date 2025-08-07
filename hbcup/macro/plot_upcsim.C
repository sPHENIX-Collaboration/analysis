//
// Make mass, pt etc plots for UPC from the simUPC DSTs
//

void plot_upcsim()
{
  const int rebin = 2;
  const int NFILES = 9;
  vector<std::string> fnames {
    "anaupc_cohjpsi.root",
    "anaupc_incohjpsi.root",
    "anaupc_cohjpsi_mumu.root",
    "anaupc_incohjpsi_mumu.root",
    "anaupc_qedhiw.root",
    "anaupc_qedhiw2.root",
    "anaupc_qedhiw_mumu.root",
    "anaupc_qedhiw2_mumu.root",
    "sHijing/anaupc_hijing.root"
    //"sHijing/anaupc_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000019-000000.root"
  };

  int hijfile = NFILES - 1; // hijing file index, always last

  //const double integ_lumi = 4.5e9;   // in inverse barns, pre-sPHENIX
  const double integ_lumi = 5.75e9;    // in inverse barns, 2025 AuAu
  const double stream_fraction = 1.0;  // fraction of crossings we stream
  const double upcsigma[] = { 27.8e-6, 29.5e-6, 27.7e-6, 29.5e-6, 6.374e-3, 17.166e-6, 6.238e-3, 17.190e-6, 7.2*(3.12e3/2e5) };  // in barns
  //const double upcsigma[] = { 27.8e-6, 29.5e-6, 27.7e-6, 29.5e-6, 6.374e-3, 17.166e-6, 6.238e-3, 17.190e-6, 7.2 };  // in barns
  //double nevents[] = { 1e5, 1e5, 1e6, 0, 0, 0, 0, 0 };  // not used.. we get nevents from datafiles
  double nevents[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // we get nevents from the datafiles

  TFile *tfile[NFILES];
  TH1 *h_trig[NFILES];

  TH1 *h_mass[NFILES];
  TH1 *h_pt[NFILES];
  TH1 *h_y[NFILES];

  TH1 *h_mass_ls[NFILES];
  TH1 *h_pt_ls[NFILES];
  TH1 *h_y_ls[NFILES];

  double scale[NFILES];   // scaling factor to normalize to same integrated luminosity

  for (int ifile=0; ifile<NFILES; ifile++)
  {
    tfile[ifile] = new TFile(fnames[ifile].c_str(),"READ");

    h_mass[ifile] = (TH1*)tfile[ifile]->Get("h_mass");
    h_y[ifile] = (TH1*)tfile[ifile]->Get("h_y");
    h_pt[ifile] = (TH1*)tfile[ifile]->Get("h_pt");

    h_mass_ls[ifile] = (TH1*)tfile[ifile]->Get("h_mass_ls");
    h_y_ls[ifile] = (TH1*)tfile[ifile]->Get("h_y_ls");
    h_pt_ls[ifile] = (TH1*)tfile[ifile]->Get("h_pt_ls");

    h_mass[ifile]->Sumw2();
    h_y[ifile]->Sumw2();
    h_pt[ifile]->Sumw2();

    h_mass[ifile]->SetXTitle("mass (GeV/c^2)");
    h_y[ifile]->SetXTitle("y");
    h_pt[ifile]->SetXTitle("p_{T} (GeV/c)");

    if ( h_mass_ls[ifile] != 0 )
    {
      h_mass_ls[ifile]->Sumw2();
      h_y_ls[ifile]->Sumw2();
      h_pt_ls[ifile]->Sumw2();
    }

    if ( rebin>1 )
    {
      h_mass[ifile]->Rebin( rebin );

      if ( h_mass_ls[ifile] != 0 )
      {
        h_mass_ls[ifile]->Rebin( rebin );
      }
    }

    h_trig[ifile] = (TH1*)tfile[ifile]->Get("h_trig");
    nevents[ifile] = h_trig[ifile]->GetBinContent(1);

    // Scaling factor for integrated luminosity expected in sPHENIX Run
    scale[ifile] = upcsigma[ifile]*integ_lumi/nevents[ifile];

    cout << fnames[ifile] << "\t" << nevents[ifile] << "\t" << scale[ifile] << endl; 
  }


  for (int ifile=0; ifile<NFILES; ifile++)
  {
    h_mass[ifile]->Scale( scale[ifile] );
    h_y[ifile]->Scale( scale[ifile] );
    h_pt[ifile]->Scale( scale[ifile] );

    if ( h_mass_ls[ifile] != 0 )
    {
      h_mass_ls[ifile]->Scale( scale[ifile] );
      h_y_ls[ifile]->Scale( scale[ifile] );
      h_pt_ls[ifile]->Scale( scale[ifile] );
    }
  }

  //=== Make Sums
  TH1 *h_tot_mass = (TH1*)h_mass[0]->Clone("h_tot_mass");           // all mass pairs, opp-sign
  h_tot_mass->SetXTitle("mass [GeV]");
  TH1 *h_tot_mass_ls = (TH1*)h_mass_ls[0]->Clone("h_tot_mass_ls");  // all mass pairs, like-sign
  for (int ifile=1; ifile<NFILES; ifile++)
  {
    h_tot_mass->Add( h_mass[ifile] );

    if ( h_mass_ls[ifile] != 0 )
    {
      h_tot_mass_ls->Add( h_mass_ls[ifile] );
    }
  }

  // J/Psi sums
  TH1 *h_jpsi_mass = (TH1*)h_mass[0]->Clone("h_jpsi_mass");           // all mass pairs, opp-sign
  h_jpsi_mass->SetXTitle("mass [GeV]");
  h_jpsi_mass->SetTitle("J/Psi");
  TH1 *h_jpsi_mass_ls = (TH1*)h_mass_ls[0]->Clone("h_jpsi_mass_ls");  // all mass pairs, like-sign
  h_jpsi_mass_ls->SetLineColor(4);
  for (int ifile=1; ifile<=3; ifile++)
  {
    h_jpsi_mass->Add( h_mass[ifile] );

    if ( h_mass_ls[ifile] != 0 )
    {
      h_jpsi_mass_ls->Add( h_mass_ls[ifile] );
    }
  }

  // QED sums
  TH1 *h_qed_mass = (TH1*)h_mass[4]->Clone("h_qed_mass");           // all mass pairs, opp-sign
  h_qed_mass->SetXTitle("mass [GeV]");
  h_qed_mass->SetTitle("QED");
  TH1 *h_qed_mass_ls = (TH1*)h_mass_ls[4]->Clone("h_qed_mass_ls");  // all mass pairs, like-sign
  h_qed_mass_ls->SetLineColor(4);
  for (int ifile=5; ifile<=7; ifile++)
  {
    h_qed_mass->Add( h_mass[ifile] );

    if ( h_mass_ls[ifile] != 0 )
    {
      h_qed_mass_ls->Add( h_mass_ls[ifile] );
    }
  }

  // Hijing (no sum needed)

  //=== Mass Plots
  int ncv = 0;
  TCanvas *ac[100];

  //== Total
  ac[ncv] = new TCanvas("cmass","mass",800,600);
  h_tot_mass->Draw("hist");
  h_tot_mass_ls->Draw("histsame");
  gPad->SetLogy(1);
  ncv++;

  //== J/Psi
  ac[ncv] = new TCanvas("cmassjpsi","mass jpsi",800,600);
  h_jpsi_mass->Draw("hist");
  h_jpsi_mass_ls->Draw("histsame");
  //gPad->SetLogy(1);
  ncv++;

  //== J/Psi (e+e- vs mu+mu-)
  ac[ncv] = new TCanvas("cmassjpsi_e_vs_mu","mass jpsi e vs mu",800,600);
  h_mass[2]->Draw("hist");
  for (int ifile=0; ifile<=3; ifile++)
  {
    h_mass[ifile]->SetLineColor(ifile+1);
    h_mass[ifile]->Draw("histsame");
  }
  TLegend *leg = new TLegend(0.7,0.7,0.9,0.9);
  //leg->SetHeader("The Legend Title","C"); // option "C" allows to center the header
  leg->AddEntry(h_mass[0],"Coh J/Psi#rightarrowe+e-","l");
  leg->AddEntry(h_mass[1],"Incoh J/Psi->e+e-","l");
  leg->AddEntry(h_mass[2],"Coh J/Psi->mu+mu-","l");
  leg->AddEntry(h_mass[3],"Incoh J/Psi->mu+mu-","l");
  leg->Draw();
  //gPad->SetLogy(1);
  ncv++;

  //== QED
  ac[ncv] = new TCanvas("cmassqed","mass qed",800,600);
  h_qed_mass->Draw("hist");
  h_qed_mass_ls->Draw("histsame");
  gPad->SetLogy(1);
  ncv++;

  /*
  for (int ifile=NFILES-1; ifile>=0; ifile--)
  {
    h_mass[ifile]->SetLineColor(ifile+2);
    h_mass[ifile]->Draw("histsame");

    if ( h_mass_ls[ifile] != 0 )
    {
      h_mass_ls[ifile]->SetLineColor(ifile+70);
      h_mass_ls[ifile]->Draw("histsame");
    }
  }
  ncv++;
  */

  ac[ncv] = new TCanvas("cmass_ls","mass, like vs unlike",800,600);
  h_tot_mass->Draw("hist");
  h_tot_mass_ls->SetLineColor(4);
  h_tot_mass_ls->Draw("histsame");
  ncv++;

  ac[ncv] = new TCanvas("cmass_hijing_ls","mass from hijing events, like vs unlike",800,600);
  h_mass[hijfile]->Draw("hist");
  h_mass_ls[hijfile]->SetLineColor(4);
  h_mass_ls[hijfile]->Draw("histsame");
  ncv++;

  //=== pt
  TH1 *h_tot_pt = (TH1*)h_pt[0]->Clone("h_tot_pt");
  h_tot_pt->SetXTitle("p_{T} [GeV/c]");
  for (int ifile=1; ifile<NFILES; ifile++)
  {
    h_tot_pt->Add( h_pt[ifile] );
  }

  ac[ncv] = new TCanvas("cpt","pt",800,600);
  h_tot_pt->Draw("hist");
  for (int ifile=NFILES-1; ifile>=0; ifile--)
  {
    h_pt[ifile]->SetLineColor(ifile+2);
    h_pt[ifile]->Draw("histsame");
  }
  ncv++;

  //=== y
  TH1 *h_tot_y = (TH1*)h_y[0]->Clone("h_tot_y");
  h_tot_y->SetXTitle("y");
  for (int ifile=1; ifile<NFILES; ifile++)
  {
    h_tot_y->Add( h_y[ifile] );
  }

  ac[ncv] = new TCanvas("crap","rapidity",800,600);
  h_tot_y->Draw("hist");
  for (int ifile=NFILES-1; ifile>=0; ifile--)
  {
    h_y[ifile]->SetLineColor(ifile+2);
    h_y[ifile]->Draw("histsame");
  }
  ncv++;

}




