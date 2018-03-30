int
plot_sidis_proj_uncertainty()
{
  /* Get tree with projected pseudodata and uncertainties */
  TFile *fin = new TFile("output/eic_sphenix_sidis_piplus_tree.root","OPEN");
  TTree *tcount = (TTree*)fin->Get("tcount");

  /* select cuts for plots */
  const int nplots = 3;
  float v_z_min  [nplots] = { 0.30, 0.70, 0.50 };
  float v_z_max  [nplots] = { 0.35, 0.75, 0.55 };
  float v_pT_min [nplots] = { 0.4,  0.8,  0.0  };
  float v_pT_max [nplots] = { 0.6,  1.0,  0.2  };

  /* Strings for figure description */
  float ebeam_e = 20;
  float ebeam_p = 250;
  float target_lumi = 0.0016;
  TString str_ebeam = TString::Format("%.0f GeV x %.0f GeV", ebeam_e, ebeam_p );
  TString str_lumin = TString::Format("L = %.4f fb^{-1}", target_lumi );

  /* Frame histogram */
  TH1F* hframe_g1 = new TH1F("hframe_g1","",100,1e-5,1);
  hframe_g1->Reset();
  hframe_g1->GetXaxis()->CenterTitle();
  hframe_g1->GetYaxis()->CenterTitle();
  hframe_g1->GetXaxis()->SetTitle("x");
  hframe_g1->GetYaxis()->SetTitle("Q^{2} (GeV^{2})");
  hframe_g1->GetYaxis()->SetRangeUser(0, 300);
  //hframe_g1->GetYaxis()->SetRangeUser(1e-1, 1e3);
  hframe_g1->GetYaxis()->SetNdivisions(505);

  /* loop over cuts for plots */
  for ( int i = 0; i < nplots; i++ )
    {
      TCut select_z( TString::Format("%0.2f < z && z < %0.2f", v_z_min[i], v_z_max[i] ) );
      TCut select_pT( TString::Format("%0.2f < pT && pT < %0.2f", v_pT_min[i], v_pT_max[i] ) );

      /* Prepare TPaveText for plots */
      //TPaveText *pt_select_ul = new TPaveText(1e-5,1e2,1e-3,1e3,"none");
      TPaveText *pt_select_ul = new TPaveText(1e-5,200,1e-3,300,"none");
      pt_select_ul->SetFillStyle(0);
      pt_select_ul->SetLineColor(0);
      pt_select_ul->AddText(str_ebeam);
      pt_select_ul->AddText(str_lumin);
      pt_select_ul->AddText( select_z.GetTitle() );
      pt_select_ul->AddText( select_pT.GetTitle() );

      /* plot Q2 vs x for various combinations of z and pT */
      TCanvas *c2 = new TCanvas();
      c2->SetLogx();
      //      c2->SetLogy();

      hframe_g1->Draw();
      pt_select_ul->Draw();

      /* draw graphs */
      TCanvas *ctmp = new TCanvas();
      ctmp->cd();

      unsigned npoints = tcount->GetEntries( select_z && select_pT );
      tcount->Draw( TString::Format("Q2:x:stdev_N*10"),
		    select_z && select_pT );

      TGraphErrors* gnew = new TGraphErrors( npoints, tcount->GetV2(), tcount->GetV1(), 0, tcount->GetV3() );
      gnew->SetMarkerColor(kRed);

      c2->cd();
      gnew->Draw("PSame");

      gPad->RedrawAxis();

      c2->Print( TString::Format("plots/sidis_proj_uncertainty_c%d.eps", i ) );

      delete ctmp;
    }

  return 0;
}
