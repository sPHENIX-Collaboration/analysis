//
// Make mass, pt etc plots for UPC from the simUPC DSTs
//

void plot_upcsim()
{
  const int rebin = 8;
  const int NFILES = 6;
  vector<std::string> fnames {
    "anaupc_cohjpsi.root",
    "anaupc_incohjpsi.root",
    "anaupc_qedhiw.root",
    "anaupc_cohjpsi_mumu.root",
    "anaupc_incohjpsi_mumu.root",
    "sHijing/anaupc_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000019.root"
  };

  int hijfile = 5;

  //const double integ_lumi = 4.5e9;   // in inverse barns, pre-sPHENIX
  const double integ_lumi = 5.75e9;   // in inverse barns, 2025 AuAu
  const double upcsigma[] = { 27.8e-6, 29.5e-6, 6.8538e-3, 27.7e-6, 29.5e-6, 7.2 };  // in barns
  double nevents[] = { 1e5, 1e5, 1e6, 0, 0, 0 };  // not used.. we get nevents from datafiles

  TFile *tfile[NFILES];
  TH1 *h_trig[NFILES];

  TH1 *h_mass[NFILES];
  TH1 *h_pt[NFILES];
  TH1 *h_y[NFILES];

  TH1 *h_mass_ls[NFILES];
  TH1 *h_pt_ls[NFILES];
  TH1 *h_y_ls[NFILES];

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
    cout << fnames[ifile] << "\t" << nevents[ifile] << endl; 
  }


  // Scaling factor for integrated luminosity expected in sPHENIX Run
  double scale[NFILES];
  for (int ifile=0; ifile<NFILES; ifile++)
  {
    scale[ifile] = upcsigma[ifile]*integ_lumi/nevents[ifile];
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

  //=== Mass
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

  int ncv = 0;
  TCanvas *ac[100];
  ac[ncv] = new TCanvas("cmass","mass",800,600);
  h_tot_mass->Draw("hist");
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




