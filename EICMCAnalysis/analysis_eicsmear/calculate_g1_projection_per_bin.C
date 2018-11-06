#include "calculate_g1.C"

int
calculate_g1_projection_per_bin(TString fin_name)
{
  TFile *fin = new TFile(fin_name,"OPEN");
  THnSparse *hfull = (THnSparse*)fin->Get("hn_dis_electron_accept");

  TH2F* hxQ2 = (TH2F*)hfull->Projection(1,0);
  hxQ2->SetName("hxQ2");

  /* beam energies */
  float ebeam_e = 18;
  float ebeam_p = 275;

  /* Retrieve Pythia generated events luminosity information */
  /* @TODO: Get this from string in file */
  TObjString* gen_crossSection_s = (TObjString*)fin->Get("crossSection");
  TObjString* gen_nEvents_s = (TObjString*)fin->Get("nEvents");
  TObjString* gen_lumi_ifb_s = (TObjString*)fin->Get("luminosity");

  float gen_crossSection_mb = (TString((gen_crossSection_s)->GetName())).Atof(); // microbarn (10^-6) from pythiaeRHIC
  float gen_nEvents = (TString((gen_nEvents_s)->GetName())).Atof();
  float gen_lumi_ifb = (TString((gen_lumi_ifb_s)->GetName())).Atof();

  float target_lumi_ifb = 10.;

  cout << "Pythia cross section (microbarn):  " << gen_crossSection_mb << " mb" << endl;
  //cout << "Pythia cross section (femtobarn):  " << gen_crossSection_fb << " fb" << endl;
  cout << "Pythia generated (scaled) luminosity:  " << gen_lumi_ifb << " fb^-1" << endl;
  cout << "Target luminosity:  " << target_lumi_ifb << " fb^-1" << endl;

  /* create tree to store information */
  TTree *tcount = new TTree("tcount", "A tree with counts in kinematics bins");
  float t_pbeam_lepton = 0;
  float t_pbeam_proton = 0;
  float t_lumi_ifb = 0;
  float t_s = 0;
  float t_x = 0;
  float t_Q2 = 0;
  float t_y = 0;
  float t_N = 0;
  float t_stdev_N = 0;
  float t_stdev_g1 = 0;
  float t_stdev_A1 = 0;
  tcount->Branch("pbeam_lepton", &t_pbeam_lepton, "pbeam_lepton/F");
  tcount->Branch("pbeam_proton", &t_pbeam_proton, "pbeam_proton/F");
  tcount->Branch("luminosity", &t_lumi_ifb, "luminosity/F"); // in inverse femtobarn
  tcount->Branch("s", &t_s, "s/F");
  tcount->Branch("x", &t_x, "x/F");
  tcount->Branch("Q2", &t_Q2, "Q2/F");
  tcount->Branch("y", &t_y, "y/F");
  tcount->Branch("N", &t_N, "N/F");
  tcount->Branch("stdev_N", &t_stdev_N, "stdev_N/F");
  tcount->Branch("stdev_g1", &t_stdev_g1, "stdev_g1/F");
  tcount->Branch("stdev_A1", &t_stdev_A1, "stdev_A1/F");

  /* copy beam parameters */
  t_pbeam_lepton = ebeam_e;
  t_pbeam_proton = ebeam_p;
  t_lumi_ifb     = gen_lumi_ifb;

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

	  t_N = (float)hxQ2->GetBinContent( bin_x, bin_y ) * (target_lumi_ifb / gen_lumi_ifb);

	  /* skip bins with no entries */
	  if ( t_N < 1 )
	    continue;

	  t_stdev_N = 1./(sqrt(t_N));
	  t_stdev_g1 = get_g1_sigma( t_x, t_Q2, t_y, t_N, 0.000511 );
	  t_stdev_A1 = get_A1_sigma( t_x, t_Q2, t_y, t_N, 0.000511 );

	  tcount->Fill();
	  s_binc_x.insert(t_x);

	  /* print values */
	  std::cout.precision(3);

	  cout << "lepton = " << std::fixed << t_pbeam_lepton
	       << " x proton = " << std::fixed << t_pbeam_proton
	       << " , L = " << std::fixed << t_lumi_ifb << " fb^-1"
	       << " , sqrt(s) = " << std::fixed << sqrt( t_s ) << " GeV"
	       << " , x = " << std::scientific << t_x
	       << " , Q2 = " << std::scientific << t_Q2
	       << " , y = " << std::fixed << t_y
	       << " , N = " << std::scientific << t_N
	       << " , g1 = " << std::fixed << -1
	       << " , g1_stdev = " << std::fixed << t_stdev_g1
	       << " , A1_stdev = " << std::fixed << t_stdev_A1
	       << endl;
	}
    }

  /* create tree to store information */
  TFile *fout = new TFile("output/eic_sphenix_dis_tree.root", "RECREATE");
  fout->cd();

  /* save input histogram in output */
  hxQ2->Write();

  /* Write tree to output */
  tcount->Write();

  /* copy string */
  gen_lumi_ifb_s->Write("luminosity");

  /* close file */
  fout->Close();

  return 0;
}
