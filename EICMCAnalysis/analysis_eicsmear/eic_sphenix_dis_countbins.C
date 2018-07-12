#include "calculate_g1.C"

int
eic_sphenix_dis_countbins()
{
  TFile *fin = new TFile("output/eic_sphenix_dis_histo_1M.root","OPEN");
  THnSparse *hfull = (THnSparse*)fin->Get("hn_dis_event_accept");
  THnSparse *hfull_fullaccept = (THnSparse*)fin->Get("hn_dis_event");

  TH2F* hxQ2 = (TH2F*)hfull->Projection(1,0);
  TH2F* hxQ2_fullaccept = (TH2F*)hfull_fullaccept->Projection(1,0);

  /* beam energies */
  float ebeam_e = 20;
  float ebeam_p = 250;

  /* Retrieve Pythia generated events luminosity information */
  /* @TODO: Get this from string in file */
  float target_lumi = -1; // in inverse femtobarn
  cout << "Pythia luminosity:  " << target_lumi << " fb^-1" << endl;

  /* create tree to store information */
  TTree *tcount = new TTree("tcount", "A tree with counts in kinematics bins");
  float t_pbeam_lepton = 0;
  float t_pbeam_proton = 0;
  float t_s = 0;
  float t_x = 0;
  float t_Q2 = 0;
  float t_y = 0;
  float t_N = 0;
  float t_stdev_N = 0;
  float t_stdev_g1 = 0;
  tcount->Branch("pbeam_lepton", &t_pbeam_lepton, "pbeam_lepton/F");
  tcount->Branch("pbeam_proton", &t_pbeam_proton, "pbeam_proton/F");
  tcount->Branch("s", &t_s, "s/F");
  tcount->Branch("x", &t_x, "x/F");
  tcount->Branch("Q2", &t_Q2, "Q2/F");
  tcount->Branch("y", &t_y, "y/F");
  tcount->Branch("N", &t_N, "N/F");
  tcount->Branch("stdev_N", &t_stdev_N, "stdev_N/F");
  tcount->Branch("stdev_g1", &t_stdev_g1, "stdev_g1/F");

  /* copy beam parameters */
  t_pbeam_lepton = ebeam_e;
  t_pbeam_proton = ebeam_p;

  /* center of mass energy */
  t_s = 4 * ebeam_e * ebeam_p;

  /* collect all x-bins */
  set<float> s_binc_x;

  /* loop over all bins */
  for ( int bin_x = 1; bin_x <= hxQ2->GetNbinsX(); bin_x++ )
    {
      for ( int bin_y = 1; bin_y <= hxQ2->GetNbinsY(); bin_y++ )
	{
	  t_x = TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) ) )
					 + ( TMath::Log10( hxQ2->GetXaxis()->GetBinLowEdge(bin_x) + hxQ2->GetXaxis()->GetBinWidth(bin_x) ) ) ) );

	  t_Q2 = TMath::Power(10, 0.5 * ( ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_y) ) )
					  + ( TMath::Log10( hxQ2->GetYaxis()->GetBinLowEdge(bin_y) + hxQ2->GetYaxis()->GetBinWidth(bin_y) ) ) ) );

	  t_y = t_Q2 / ( t_x * t_s );

	  t_N = hxQ2->GetBinContent( bin_x, bin_y );

	  /* skip kinematics bins wth y > 0.95 and y < 1e-2 */
	  if ( t_y > 0.95 || t_y < 1e-2 )
	    continue;

	  /* skip bins with no entries */
	  if ( t_N < 1 )
	    continue;

	  t_stdev_N = 1./(sqrt(t_N));
	  t_stdev_g1 = get_g1_sigma( t_x, t_Q2, t_y, t_N, 0.000511 );

	  tcount->Fill();
	  s_binc_x.insert(t_x);

	  /* print values */
	  std::cout.precision(2);

	  cout << "lepton = " << std::fixed << t_pbeam_lepton
	       << " x proton = " << std::fixed << t_pbeam_proton
	       << " , sqrt(s) = " << std::fixed << sqrt( t_s )
	       << " , x = " << std::scientific << t_x
	       << " , Q2 = " << std::scientific << t_Q2
	       << " , y = " << std::fixed << t_y
	       << " , N = " << std::scientific << t_N
	       << endl;
	}
    }

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

  pt_ebeam_lumi_ul->Draw();
  gPad->RedrawAxis();

  /* make x-Q2 plot for 'perfect' acceptance */
  TCanvas *c4 = new TCanvas();
  c4->SetRightMargin(0.12);
  hxQ2_fullaccept->Draw("colz");
  c4->SetLogx(1);
  c4->SetLogy(1);
  c4->SetLogz(1);

  f_y095->Draw("same");
  f_y001->Draw("same");

  pt_ebeam_lumi_ul->Draw();
  gPad->RedrawAxis();

  /* make x-Q2 acceptance fraction pot */
  TCanvas *c3 = new TCanvas();
  c3->SetRightMargin(0.12);
  TH2F* hxQ2_acceptance_ratio = hxQ2->Clone("x_Q2_acceptance_ratio");
  hxQ2_acceptance_ratio->GetZaxis()->SetNdivisions(505);
  hxQ2_acceptance_ratio->Divide(hxQ2_fullaccept);
  hxQ2_acceptance_ratio->Draw("colz");
  c3->SetLogx(1);
  c3->SetLogy(1);

  f_y095->Draw("same");
  f_y001->Draw("same");

  pt_ebeam_lumi_ul->Draw();
  gPad->RedrawAxis();

  /* plot g1 vs Q2 for various x */
  TCanvas *c2 = new TCanvas("g1","",700,800);
  c2->SetLogx();

  TH1F* hframe_g1 = (TH1F*)hfull->Projection(1)->Clone("h1_g1");
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

  /* create tree to store information */
  TFile *fout = new TFile("output/eic_sphenix_dis_tree.root", "RECREATE");

  /* Write tree to output */
  tcount->Write();
  fout->Close();

  return 0;
}
