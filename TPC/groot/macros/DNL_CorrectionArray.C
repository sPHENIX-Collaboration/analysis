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

// New pointers need to be created and assigned to the original histograms in the code
char name[100];
TH2D* BH_Residual_vsPhi_[16];
TH2D* BH_Residual_vsPhi_PC_[16];

// Histograming ALL Radii through an array
TH2D* BH_Residual_vsPhi_Corrected_[16];
TH2D* BH_Residual_vsPhi_PC_Corrected_[16];

void TripleFit(TH2*);

void DNL_CorrectionArray(int REBIN=1)
{

  // Assign the array of pointers.
  for( int i=0; i<16; i++)
    {
      sprintf(name, "BH_Residual_vsPhi_%d", i);
      _file0->GetObject(name, BH_Residual_vsPhi_[i]);
     
      sprintf(name, "BH_Residual_vsPhi_PC_%d", i);
      _file0->GetObject(name, BH_Residual_vsPhi_PC_[i]);
    }

  // Clone these into the "correct ones"
  for (int i=1; i<15; i++) // 0 & 15 currently aren't "new'ed" in FillBlobHist.C
    {
      BH_Residual_vsPhi_[i]->Rebin2D(1, REBIN);
      BH_Residual_vsPhi_PC_[i]->Rebin2D(1, REBIN);

      sprintf(name, "BH_Residual_vsPhi_Corrected_%d", i);
      BH_Residual_vsPhi_Corrected_[i] = (TH2D*)BH_Residual_vsPhi_[i]->Clone(name);
      BH_Residual_vsPhi_Corrected_[i]->Reset();

      sprintf(name, "BH_Residual_vsPhi_PC_Corrected_%d", i);
      BH_Residual_vsPhi_PC_Corrected_[i] = (TH2D*)BH_Residual_vsPhi_PC_[i]->Clone(name);
      BH_Residual_vsPhi_PC_Corrected_[i]->Reset();
    }

  // Fill the "corrected" histograms 
  for (int k=1; k<15; k++) 
    {
      cout << "Calculating corrections for layer " << k << endl;
      for (int i=1; i<BH_Residual_vsPhi_[k]->GetNbinsX()+1; i++)
	{
	  double correction    = BH_Residual_vsPhi_[k]->ProfileX()->GetBinContent(i); // This ultimately gives a y-value
	  double correction_PC = BH_Residual_vsPhi_PC_[k]->ProfileX()->GetBinContent(i);	

	  double xValue    = BH_Residual_vsPhi_[k]->GetXaxis()->GetBinCenter(i);
	  double xValue_PC = BH_Residual_vsPhi_PC_[k]->GetXaxis()->GetBinCenter(i);

	  for (int j=1; j< BH_Residual_vsPhi_[k]->GetNbinsY()+1; j++)
	    {
	      int getBin;
	      double content, yValue;

	      getBin  = BH_Residual_vsPhi_[k]->GetBin(i,j);
	      content = BH_Residual_vsPhi_[k]->GetBinContent(getBin);
	      yValue  = BH_Residual_vsPhi_[k]->GetYaxis()->GetBinCenter(j);
	      BH_Residual_vsPhi_Corrected_[k]->Fill(xValue, yValue-correction, content); // "content" in this 2D histogram is the weight

	      getBin  = BH_Residual_vsPhi_PC_[k]->GetBin(i,j);
	      content = BH_Residual_vsPhi_PC_[k]->GetBinContent(getBin);
	      yValue  = BH_Residual_vsPhi_PC_[k]->GetYaxis()->GetBinCenter(j);
	      BH_Residual_vsPhi_PC_Corrected_[k]->Fill(xValue_PC, yValue-correction_PC, content);
	    }
	}
    }

  for (int i=1; i<15; i++)
    {
      TripleFit(BH_Residual_vsPhi_Corrected_[i]);
      TripleFit(BH_Residual_vsPhi_PC_Corrected_[i]);
    }
}

TF1* GAUSS = 0;

void TripleFit(TH2* hist)
{

  if (hist->Integral()<1000) { return; }

  TH1D* ProjectionY = hist->ProjectionY(); // Sums (cause that's what histograms do) all x-values along that particular y-value
  double min = ProjectionY->GetBinCenter(1); // first bin is minimum (in Gauss tail)
  double max = ProjectionY->GetBinCenter(ProjectionY->GetNbinsX());  // last bin is maximum (in Gauss tail)

  double amp   = ProjectionY->GetBinContent(ProjectionY->GetMaximumBin());
  double mean  = ProjectionY->GetMean();
  double sigma = ProjectionY->GetRMS();

  if (!GAUSS) { GAUSS = new TF1("GAUSS", "[0]*exp(-(x-[1])*(x-[1])/(2.0*[2]*[2]))", min, max); }

  for (int j=0; j<3; j++)
    {
      GAUSS->SetParameter(0,amp);
      GAUSS->SetParameter(1,mean);
      GAUSS->SetParameter(2,sigma);

      ProjectionY->Fit(GAUSS, "Q0", "", min, max);

      amp   = ProjectionY->GetFunction("GAUSS")->GetParameter(0);
      mean  = ProjectionY->GetFunction("GAUSS")->GetParameter(1);
      sigma = ProjectionY->GetFunction("GAUSS")->GetParameter(2);

      min = mean - 2.0*sigma; 
      max = mean + 2.0*sigma; 
    }

  
  cout << "Name: " << hist->GetName();
  cout << "\t Amp:" << amp;
  cout << "\t Mean:" << mean;
  cout << "\t Sigma:" << sigma;
  cout << endl;
}

