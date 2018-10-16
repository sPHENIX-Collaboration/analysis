#include "calculate_g1.C"

int
plot_g1_projection()
{
  TFile *fin = new TFile("output/eic_sphenix_dis_tree.root","OPEN");

  TH2F *hxQ2 = (TH2F*)fin->Get("hxQ2");
  TTree *tin = (TTree*)fin->Get("tcount");

  /* beam energies */
  float ebeam_e = 18;
  float ebeam_p = 275;

  /* Retrieve Pythia generated events luminosity information */
  /* @TODO: Get this from string in file */
  float target_lumi = 10; // in inverse femtobarn
  cout << "Pythia luminosity:  " << target_lumi << " fb^-1" << endl;

  /* collect all x-bins */
  //set<float> s_binc_x;

  /* Prepare TPaveText for plots */
  TString str_ebeam = TString::Format("%.0f GeV x %.0f GeV", ebeam_e, ebeam_p );
  TString str_lumin = TString::Format("L = %.4f fb^{-1}", target_lumi );

  TPaveText *pt_ebeam_lumi_ul = new TPaveText(1e-5,1e3,1e-3,1e4,"none");
  pt_ebeam_lumi_ul->SetFillStyle(0);
  pt_ebeam_lumi_ul->SetLineColor(0);
  pt_ebeam_lumi_ul->AddText(str_ebeam);
  pt_ebeam_lumi_ul->AddText(str_lumin);

  TPaveText *pt_ebeam_lumi_ll = new TPaveText(1e2,45,1e3,50,"none");
  pt_ebeam_lumi_ll->SetFillStyle(0);
  pt_ebeam_lumi_ll->SetLineColor(0);
  pt_ebeam_lumi_ll->AddText(str_ebeam);
  pt_ebeam_lumi_ll->AddText(str_lumin);

  /* Prepare inelasticity (y) cut lines for plots */
  TF1 *f_y095 = new TF1("f_y095", "4*x*[0]*[1]*[2]", 1e-5, 1);
  f_y095->SetParameter( 0, ebeam_e);
  f_y095->SetParameter( 1, ebeam_p);
  f_y095->SetParameter( 2, 0.95);
  TF1 *f_y001 = (TF1*)f_y095->Clone("f_y01");
  f_y001->SetParameter(2 , 0.01);

  /* make x-Q2 plot */
  TCanvas *c1 = new TCanvas();
  c1->SetRightMargin(0.12);
  hxQ2->Draw("colz");
  c1->SetLogx(1);
  c1->SetLogy(1);
  c1->SetLogz(1);

  f_y095->Draw("same");
  f_y001->Draw("same");

  TPaveText *pt_N = new TPaveText(1e-5,1e2,1e-3,3e2,"none");
  pt_N->SetFillStyle(0);
  pt_N->SetLineColor(0);
  pt_N->AddText("z = # events");

  pt_N->Draw();
  pt_ebeam_lumi_ul->Draw();
  gPad->RedrawAxis();


  /* plot g1 statistical uncertainty for different x-Q2 bins */
  TCanvas *c5 = new TCanvas();
  c5->SetRightMargin(0.12);

  TH2F* hxQ2_g1_sigma = hxQ2->Clone("x_Q2_g1_sigma");
  hxQ2_g1_sigma->GetZaxis()->SetNdivisions(505);
  hxQ2_g1_sigma->Reset();

  tcount->Draw("Q2:x >> x_Q2_g1_sigma","stdev_g1");

  /* normalize to number of entries in each bin */
  hxQ2_g1_sigma->Divide(hxQ2);

  hxQ2_g1_sigma->Draw("colz");
  c5->SetLogx(1);
  c5->SetLogy(1);

  f_y095->Draw("same");
  f_y001->Draw("same");

  TPaveText *pt_g1_stdev = new TPaveText(1e-5,1e2,1e-3,3e2,"none");
  pt_g1_stdev->SetFillStyle(0);
  pt_g1_stdev->SetLineColor(0);
  pt_g1_stdev->AddText("z = #sigma_{g_{1}}");

  pt_g1_stdev->Draw();
  pt_ebeam_lumi_ul->Draw();
  gPad->RedrawAxis();


  /* plot g1 vs Q2 for various x */
  TCanvas *c2 = new TCanvas("g1","",700,800);
  c2->SetLogx();

  TH1F* hframe_g1 = (TH1F*)hxQ2->ProjectionY()->Clone("h1_g1");
  hframe_g1->Reset();
  hframe_g1->GetXaxis()->CenterTitle();
  hframe_g1->GetYaxis()->CenterTitle();
  hframe_g1->GetXaxis()->SetTitle("Q^{2} (GeV^{2})");
  //hframe_g1->GetYaxis()->SetTitle("g_{1}(x,Q^{2}) + const(x)");
  hframe_g1->GetYaxis()->SetTitle("const(x)");
  hframe_g1->GetXaxis()->SetRangeUser(0.99,1500);
  hframe_g1->GetYaxis()->SetRangeUser(2,50);
  hframe_g1->GetYaxis()->SetNdivisions(505);

  hframe_g1->Draw("");

  /* collect all x-bins */
  set<float> s_binc_x;
  for ( int bin_x = 1; bin_x <= hxQ2->GetNbinsX(); bin_x++ )
    {
      t_x = TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) ) )
				     + ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) + hxQ2->GetXaxis()->GetBinWidth(bin_x) ) ) ) );
      s_binc_x.insert(t_x);
    }

  /* draw graphs */
  TCanvas *ctmp = new TCanvas();
  float offset = 48;

  cout << "+++++++++++++++++++++++++++++++++++++++++++" << endl;
  cout << "Plot offsets: " << endl;

  for ( set<float>::iterator itx = s_binc_x.begin();
	itx != s_binc_x.end(); itx++ )
    {
      /* print values */
      cout << "offset = " << (int)offset << " for x = " << *itx << endl;

      /* Create graph */
      ctmp->cd();

      unsigned npoints = tcount->GetEntries( TString::Format("x > 0.99*%f && x < 1.01*%f", *itx, *itx ) );
      tcount->Draw( TString::Format("%f:Q2:stdev_g1", offset),
		    TString::Format("x > 0.99*%f && x < 1.01*%f", *itx, *itx ) );

      TGraphErrors* gnew = new TGraphErrors( npoints, tcount->GetV2(), tcount->GetV1(), 0, tcount->GetV3() );
      gnew->SetMarkerColor(kRed);

      c2->cd();
      gnew->Draw("PLsame");

      offset -= 2;
    }

  pt_ebeam_lumi_ll->Draw();
  gPad->RedrawAxis();

  delete ctmp;

  return 0;
}
