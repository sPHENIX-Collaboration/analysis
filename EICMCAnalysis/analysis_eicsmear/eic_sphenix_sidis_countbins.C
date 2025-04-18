int
eic_sphenix_sidis_countbins()
{
  TFile *fin = new TFile("output/eic_sphenix_dis_histo_1M.root","OPEN");
  THnSparse *hfull = (THnSparse*)fin->Get("hn_sidis_pion_plus_accept");
  THnSparse *hfull_fullaccept = (THnSparse*)fin->Get("hn_sidis_pion_plus");

  TH2F* hxQ2 = (TH2F*)hfull->Projection(1,0);
  TH2F* hxQ2_fullaccept = (TH2F*)hfull_fullaccept->Projection(1,0);

  /* beam energies */
  float ebeam_e = 20;
  float ebeam_p = 250;

  /* Pythia generated events, cross section, and luminosity */
  float pythia_ngen = 1e6;
  float pythia_xsec = 0.60785660255324614; // in microbarn
  float convert_microbarn_to_femtobarn = 1e9;
  float pythia_lumi = pythia_ngen / ( pythia_xsec * convert_microbarn_to_femtobarn );

  /* target luminosity and scaling factor */
  float target_lumi = pythia_lumi; // in inverse femtobarn
  float lumi_scaling = target_lumi / pythia_lumi;

  cout << "Pythia luminosity:  " << pythia_lumi << " fb^-1" << endl;
  cout << "Target luminosity:  " << target_lumi << " fb^-1" << endl;
  cout << "Luminosity scaling: " << lumi_scaling << endl;

  /* Minimum number of bin entries to accept bin */
  int Nmin = 1;

  /* create tree to store information */
  TTree *tcount = new TTree("tcount", "A tree with counts in kinematics bins");
  float t_pbeam_lepton = 0;
  float t_pbeam_proton = 0;
  float t_s = 0;
  float t_x = 0;
  float t_Q2 = 0;
  float t_y = 0;
  float t_z = 0;
  float t_pT = 0;
  float t_N = 0;
  float t_stdev_N = 0;
  tcount->Branch("pbeam_lepton", &t_pbeam_lepton, "pbeam_lepton/F");
  tcount->Branch("pbeam_proton", &t_pbeam_proton, "pbeam_proton/F");
  tcount->Branch("s", &t_s, "s/F");
  tcount->Branch("x", &t_x, "x/F");
  tcount->Branch("Q2", &t_Q2, "Q2/F");
  tcount->Branch("y", &t_y, "y/F");
  tcount->Branch("z", &t_z, "z/F");
  tcount->Branch("pT", &t_pT, "pT/F");
  tcount->Branch("N", &t_N, "N/F");
  tcount->Branch("stdev_N", &t_stdev_N, "stdev_N/F");

  /* copy beam parameters */
  t_pbeam_lepton = ebeam_e;
  t_pbeam_proton = ebeam_p;

  /* center of mass energy */
  t_s = 4 * ebeam_e * ebeam_p;


  /* Get all axis to loop over */
  TAxis *ax_x = hfull->GetAxis(0);
  TAxis *ax_Q2 = hfull->GetAxis(1);
  TAxis *ax_z = hfull->GetAxis(2);
  TAxis *ax_pT = hfull->GetAxis(3);

  ax_x->Print();
  ax_Q2->Print();
  ax_z->Print();
  ax_pT->Print();

  /* collect all x-bins */
  //  set<float> s_binc_x;

  /* loop over all bins */
  for ( int bin_x = 1; bin_x <= ax_x->GetNbins(); bin_x++ )
    {
      for ( int bin_Q2 = 1; bin_Q2 <= ax_Q2->GetNbins(); bin_Q2++ )
        {
          t_x = TMath::Power(10, 0.5 * ( ( TMath::Log10( ax_x->GetBinLowEdge(bin_x) ) )
                                         + ( TMath::Log10( ax_x->GetBinLowEdge(bin_x) + ax_x->GetBinWidth(bin_x) ) ) ) );

          t_Q2 = TMath::Power(10, 0.5 * ( ( TMath::Log10( ax_Q2->GetBinLowEdge(bin_Q2) ) )
                                          + ( TMath::Log10( ax_Q2->GetBinLowEdge(bin_Q2) + ax_Q2->GetBinWidth(bin_Q2) ) ) ) );

          /* Calculate inelasticity y */
          t_y = t_Q2 / ( t_x * t_s );

          /* skip kinematics bins wth y > 0.95 and y < 1e-2 */
          if ( t_y > 0.95 || t_y < 1e-2 )
            continue;

          for ( int bin_z = 1; bin_z <= ax_z->GetNbins(); bin_z++ )
            {

              t_z = ax_z->GetBinCenter( bin_z );

              for ( int bin_pT = 1; bin_pT <= ax_pT->GetNbins(); bin_pT++ )
                {

                  t_pT = ax_pT->GetBinCenter( bin_pT );

                  /* Get bin entries */
		  int binloc[4] = { bin_x, bin_Q2, bin_z, bin_pT };
                  t_N = hfull->GetBinContent( binloc ) * lumi_scaling;

                  /* skip bins with small number of entries */
                  if ( t_N < Nmin )
                    continue;

                  t_stdev_N = 1./(sqrt(t_N));

                  tcount->Fill();
                  //s_binc_x.insert(t_x);

                  /* print values */
                  std::cout.precision(2);

		  bool toscreen = false;
		  if ( toscreen )
		    {
		      cout    //     <<"lepton = " << std::fixed << t_pbeam_lepton
			//     << " x proton = " << std::fixed << t_pbeam_proton
			//     << " , sqrt(s) = " << std::fixed << sqrt( t_s )
			<< " , x = " << std::scientific << t_x
			<< " , Q2 = " << std::scientific << t_Q2
			<< " , y = " << std::fixed << t_y
			<< " , z = " << std::fixed << t_z
			<< " , pT = " << std::fixed << t_pT
			<< " , N = " << std::scientific << t_N
			<< endl;
		    }
                }
            }
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

  /* create tree to store information */
  TFile *fout = new TFile("output/eic_sphenix_sidis_tree.root", "RECREATE");

  /* Write tree to output */
  tcount->Write();
  fout->Close();

  return 0;
}
