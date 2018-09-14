int
FitSlices( TString fname )
{

  /* Get input 3D histogram */
  TFile *fin = new TFile( fname, "OPEN");
  TH3F *hin3D = (TH3F*)fin->Get("MomMagMap");

  /* Get bin counts for each axis */
  cout << "X axis = " << hin3D->GetXaxis()->GetTitle() << endl;
  cout << "Y axis = " << hin3D->GetYaxis()->GetTitle() << endl;
  cout << "Z axis = " << hin3D->GetZaxis()->GetTitle() << endl;

  unsigned nbins_ptrue = hin3D->GetXaxis()->GetNbins();
  unsigned nbins_eta = hin3D->GetYaxis()->GetNbins();
  unsigned nbins_pdiff = hin3D->GetZaxis()->GetNbins();

  /* Fit all slices in eta-ptrue */
  hin3D->FitSlicesZ();

  /* Open ofstream for output to csv file */
  ofstream ofs("fitslices_out.csv");

  /* Loop over eta bins & slice */
  TH2D* HistMomMagMap_0 = (TH2D*)gDirectory->Get("HistMomMagMap_0");
  TH2D* HistMomMagMap_1 = (TH2D*)gDirectory->Get("HistMomMagMap_1");
  TH2D* HistMomMagMap_2 = (TH2D*)gDirectory->Get("HistMomMagMap_2");

  for ( unsigned bin_eta = 1; bin_eta <= nbins_eta; bin_eta++ )
    {

      for ( unsigned bin_ptrue = 1; bin_ptrue <= nbins_ptrue; bin_ptrue++ )
	{

	  double eta_i = HistMomMagMap_1->GetYaxis()->GetBinCenter(bin_eta);
	  double ptrue_i = HistMomMagMap_1->GetXaxis()->GetBinCenter(bin_ptrue);

	  /* Access fit parameters */
	  double norm_i = HistMomMagMap_0->GetBinContent(bin_ptrue,bin_eta);
	  double mean_i = HistMomMagMap_1->GetBinContent(bin_ptrue,bin_eta);
	  double sigm_i = HistMomMagMap_2->GetBinContent(bin_ptrue,bin_eta);

	  double mean_err_i = HistMomMagMap_1->GetBinError(bin_ptrue,bin_eta);
	  double sigm_err_i = HistMomMagMap_2->GetBinError(bin_ptrue,bin_eta);

	  cout << eta_i << " " << ptrue_i << " " << norm_i << " " << mean_i << " " << sigm_i << " " << mean_err_i << " " << sigm_err_i << endl;


	  /* For Giorgian */
	  ofs << ptrue_i << "," << eta_i << "," << sigm_i << "," << sigm_err_i << "," << mean_i << "," << mean_err_i << "," << norm_i << endl;

	}

    }

  /* Close ofstream */
  ofs.close();

  /* Loop over momentum bins & slice */

  /* Fit gaussian to 1D histogram */

  /* Store fit parameters */

  return 0;
}
