/*    This is a decent routine to fix differential non-linearity (DNL)
  and  more rapidly get at the actual resolution. We assume the following:
  Since the FTBF tracks are "nearly" horizontal, the "phi" coordinate causes 
  all of sets of triple-layers to smoothly vary across the DNL parameter.
      In this case, the residual will have a deterministic dependence on phi.
  We'll use a profile histogram from the #Residual_vsPhi result to extract a
  correction function. This will be turned into a "shift". Then we will copy
  the #Residual_vsPhi into a new histogram while applying the appropriate shifts. 
  
                                                            2018-10-30 TKH & Vlad  */

// PC = PhiCut (efficiency/track attempt before Hough)      2018-12-05 Vlad

TH2D* BH_5Residual_vsPhi_Corrected;
TH1D* ProjectionY;

// Histograming ALL Radii through an array
char name[100];
TH2D* BH_Residual_vsPhi_Corrected_[16];
TH1D* ProjectionY_[16];
 TH2D* BH_Residual_vsPhi_PC_Corrected_[16];
 TH1D* ProjectionY_PC_[16];


void DNL_Correction(int REBIN=1)
{

  BH_5Residual_vsPhi->Rebin2D(1, REBIN);

  BH_5Residual_vsPhi_Corrected = (TH2D*)BH_5Residual_vsPhi->Clone("BH_5Residual_vsPhi_Corrected");
  BH_5Residual_vsPhi_Corrected->Reset();

  for (int i=1; i<15; i++) // 0 & 15 currently aren't "new'ed" in FillBlobHist.C
    {

      BH_Residual_vsPhi_1-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_2-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_3-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_4-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_5-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_6-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_7-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_8-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_9-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_10-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_11-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_12-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_13-> Rebin2D(1, REBIN);
      BH_Residual_vsPhi_14-> Rebin2D(1, REBIN);
  
      //      BH_Residual_vsPhi_PC_[i]->Rebin2D(1, REBIN);


      // sprintf(name, "BH_Residual_vsPhi_PC_%d", i); 
      // name->Rebin2D(1, REBIN);

      //BH_Residual_vsPhi_[i] = new TH2D(Form(BH_Residual_vsPhi_%i, i));
      
      sprintf(name, "BH_Residual_vsPhi_Corrected_%d", i);
      BH_Residual_vsPhi_Corrected_[i] = (TH2D*)Form("BH_Residual_vsPhi_%d",i)->Clone(name);
      BH_Residual_vsPhi_Corrected_[i]->Reset();

      sprintf(name, "BH_Residual_vsPhi_PC_Corrected_%d", i);
      BH_Residual_vsPhi_PC_Corrected_[i] = (TH2D*)BH_Residual_vsPhi_PC_[i]->Clone(name);
      BH_Residual_vsPhi_PC_Corrected_[i]->Reset();
    }

  for (int i=1; i<BH_5Residual_vsPhi->GetNbinsX()+1; i++)
    {
      double correction = BH_5Residual_vsPhi->ProfileX()->GetBinContent(i); // This ultimately gives a y-value
      double xValue     = BH_5Residual_vsPhi->GetXaxis()->GetBinCenter(i);

      for (int j=1; j< BH_5Residual_vsPhi->GetNbinsY()+1; j++)
	{
	  int getBin     = BH_5Residual_vsPhi->GetBin(i,j);
	  double content = BH_5Residual_vsPhi->GetBinContent(getBin);
	  double yValue  = BH_5Residual_vsPhi->GetYaxis()->GetBinCenter(j);
	  BH_5Residual_vsPhi_Corrected->Fill(xValue, yValue-correction, content); // "content" in this 2D histogram is the weight
	}
    }


  for (int k=1; k<15; k++) 
    {
      for (int i=1; i<BH_Residual_vsPhi_[k]->GetNbinsX()+1; i++)
	{
	  double correction = BH_Residual_vsPhi_[k]->ProfileX()->GetBinContent(i); // This ultimately gives a y-value
	  double xValue     = BH_Residual_vsPhi_[k]->GetXaxis()->GetBinCenter(i);

	  double correction_PC = BH_Residual_vsPhi_PC_[k]->ProfileX()->GetBinContent(i);
	  double xValue_PC     = BH_Residual_vsPhi_PC_[k]->GetXaxis()->GetBinCenter(i);

	  for (int j=1; j< BH_Residual_vsPhi_[k]->GetNbinsY()+1; j++)
	    {
	      int getBin     = BH_Residual_vsPhi_[k]->GetBin(i,j);
	      double content = BH_Residual_vsPhi_[k]->GetBinContent(getBin);
	      double yValue  = BH_Residual_vsPhi_[k]->GetYaxis()->GetBinCenter(j);
	      BH_Residual_vsPhi_Corrected_[k]->Fill(xValue, yValue-correction, content); // "content" in this 2D histogram is the weight

	      int getBin_PC     = BH_Residual_vsPhi_PC_[k]->GetBin(i,j);
	      double content_PC = BH_Residual_vsPhi_PC_[k]->GetBinContent(getBin_PC);
	      double yValue_PC  = BH_Residual_vsPhi_PC_[k]->GetYaxis()->GetBinCenter(j);
	      BH_Residual_vsPhi_PC_Corrected_[k]->Fill(xValue_PC, yValue_PC-correction_PC, content_PC); // "content" in this 2D histogram is the weight
	    }
	}
    }

  ProjectionY = BH_5Residual_vsPhi_Corrected->ProjectionY(); // Sums (cause that's what histograms do) all x-values along that particular y-value
  double min = ProjectionY->GetBinCenter(1); // first bin is minimum (in Gauss tail)
  double max = ProjectionY->GetBinCenter(ProjectionY->GetNbinsX());  // last bin is maximum (in Gauss tail)

  double min[16], max[16], min_PC[16], max_PC[16];
  for (int i=1; i<15; i++) 
    {
      ProjectionY_[i] = BH_Residual_vsPhi_Corrected_[i]->ProjectionY(); // Sums (cause that's what histograms do) all x-values along that particular y-value
      min[i] = ProjectionY_[i]->GetBinCenter(1);
      max[i] = ProjectionY_[i]->GetBinCenter(ProjectionY_[i]->GetNbinsX());

      ProjectionY_PC_[i] = BH_Residual_vsPhi_PC_Corrected_[i]->ProjectionY(); 
      min_PC[i] = ProjectionY_PC_[i]->GetBinCenter(1);
      max_PC[i] = ProjectionY_PC_[i]->GetBinCenter(ProjectionY_PC_[i]->GetNbinsX());
    }
  
  TF1*   GAUSS = new TF1("GAUSS", "[0]*exp(-(x-[1])*(x-[1])/(2.0*[2]*[2]))", min, max);
  double amp   = ProjectionY->GetBinContent(ProjectionY->GetMaximumBin());
  double mean  = ProjectionY->GetMean();
  double sigma = ProjectionY->GetRMS();


  double amp[16], mean[16], sigma[16];
  double amp_PC[16], mean_PC[16], sigma_PC[16];
  for (int i=1; i<15; i++) 
    {
      sprintf(name, "GAUSS_%d", i);
      TF1* GAUSS_[i] = new TF1(name, "[0]*exp(-(x-[1])*(x-[1])/(2.0*[2]*[2]))", min[i], max[i]);
      amp[i]   = ProjectionY_[i]->GetBinContent(ProjectionY_[i]->GetMaximumBin());
      mean[i]  = ProjectionY_[i]->GetMean();
      sigma[i] = ProjectionY_[i]->GetRMS();

      sprintf(name, "GAUSS_PC_%d", i);
      TF1* GAUSS_PC_[i] = new TF1(name, "[0]*exp(-(x-[1])*(x-[1])/(2.0*[2]*[2]))", min_PC[i], max_PC[i]);
      amp_PC[i]   = ProjectionY_PC_[i]->GetBinContent(ProjectionY_PC_[i]->GetMaximumBin());
      mean_PC[i]  = ProjectionY_PC_[i]->GetMean();
      sigma_PC[i] = ProjectionY_PC_[i]->GetRMS();
    }


  for (int j=0; j<3; j++)
    {
      GAUSS->SetParameter(0,amp);
      GAUSS->SetParameter(1,mean);
      GAUSS->SetParameter(2,sigma);

      ProjectionY->Fit(GAUSS, "", "", min, max);

      amp   = ProjectionY->GetFunction("GAUSS")->GetParameter(0);
      mean  = ProjectionY->GetFunction("GAUSS")->GetParameter(1);
      sigma = ProjectionY->GetFunction("GAUSS")->GetParameter(2);

      min = mean - 2.0*sigma; 
      max = mean + 2.0*sigma; 
    }


  for (int i=1; i<15; i++) 
    {
      for (int j=0; j<3; j++)
	{
	  GAUSS_[i]->SetParameter(0,amp[i]);	  GAUSS_PC_[i]->SetParameter(0,amp_PC[i]);
	  GAUSS_[i]->SetParameter(1,mean[i]);	  GAUSS_PC_[i]->SetParameter(1,mean_PC[i]);
	  GAUSS_[i]->SetParameter(2,sigma[i]);	  GAUSS_PC_[i]->SetParameter(2,sigma_PC[i]);

	  ProjectionY_[i]->Fit(GAUSS_[i], "", "", min[i], max[i]);
	  ProjectionY_PC_[i]->Fit(GAUSS_PC_[i], "", "", min_PC[i], max_PC[i]);

	  sprintf(name, "GAUSS_%d", i);
	  amp[i]   = ProjectionY_[i]->GetFunction(name)->GetParameter(0);
	  mean[i]  = ProjectionY_[i]->GetFunction(name)->GetParameter(1);
	  sigma[i] = ProjectionY_[i]->GetFunction(name)->GetParameter(2);

	  sprintf(name, "GAUSS_PC_%d", i);
	  amp_PC[i]   = ProjectionY_PC_[i]->GetFunction(name)->GetParameter(0);
	  mean_PC[i]  = ProjectionY_PC_[i]->GetFunction(name)->GetParameter(1);
	  sigma_PC[i] = ProjectionY_PC_[i]->GetFunction(name)->GetParameter(2);

	  min[i] = mean[i] - 2.0*sigma[i]; 	  min_PC[i] = mean_PC[i] - 2.0*sigma_PC[i]; 
	  max[i] = mean[i] + 2.0*sigma[i]; 	  max_PC[i] = mean_PC[i] + 2.0*sigma_PC[i]; 
	}
    }

}
