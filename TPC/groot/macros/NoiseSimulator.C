TH1D* BlobPosition;
TH1D* BlobSigma;
TH1D* BlobSigmaGen;
TH1D* BlobPos;
TH1D* BlobCharge;
TF1* BlobFit;
TF1* NORMGAUSS;

TH2D* ChargeQuality;
TH2D* MeanQuality;
TH2D* SigmaQuality;

TH1D* ChargeDiffBeforeNoise;
TH1D* MeanDiffBeforeNoise;
TH1D* SigmaDiffBeforeNoise;

TH1D* ChargeDiffAfterNoise;
TH1D* MeanDiffAfterNoise;
TH1D* SigmaDiffAfterNoise;

void NoiseSimulator(int Nevent = 10000, double NOISE = 9)
{
  gSystem->Load("libgroot");

  groot* Tree = groot::instance();

  double Minphi =  9999;
  double Maxphi = -9999;
  for (int i=0; i<128; i++)
    {
      if (Tree->ZigzagMap2[0][i])
	{
	  double phi = Tree->ZigzagMap2[0][i]->PCenter();
	  if (phi < Minphi) Minphi = phi;
	  if (phi > Maxphi) Maxphi = phi;
	}
    }
  double dPhi = fabs( Tree->ZigzagMap2[0][90]->PCenter() - Tree->ZigzagMap2[0][91]->PCenter() );
  cout << dPhi << endl;
  int NBINS = int( (Maxphi - Minphi)/dPhi + 1.5 );
  cout << NBINS << endl;
  double left  = Minphi - 0.5*dPhi;
  double right = Maxphi + 0.5*dPhi;
  BlobFit      = new TF1("BlobFit", "[0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", left, right);
  BlobPos      = new TH1D("BlobPos", "BlobPos", NBINS, left, right);
  BlobSigma    = new TH1D("BlobSigma", "BlobSigma", 250, 0.0, 8.0E-3);
  BlobSigmaGen = new TH1D("BlobSigmaGen", "BlobSigmaGen", 250, 0.0, 8.0E-3);
  
  BlobPosition = new TH1D("BlobPosition", "BlobPosition", NBINS*10, left, right);
  BlobCharge   = new TH1D("BlobCharge", "BlobCharge", 256, 0, 2000);

  ChargeQuality = new TH2D("ChargeQuality", "ChargeQuality", 256, 0, 2000, 256, 0, 2000);
  MeanQuality   = new TH2D("MeanQuality", "MeanQuality", NBINS*10, left, right, NBINS*10, left, right);
  SigmaQuality  = new TH2D("SigmaQuality", "SigmaQuality", 250, 0.0, 8.0E-3, 250, 0.0, 8.0E-3);

  ChargeDiffBeforeNoise = new TH1D("ChargeDiffBeforeNoise", "ChargeDiffBeforeNoise", 300, -10, 10);
  MeanDiffBeforeNoise   = new TH1D("MeanDiffBeforeNoise", "MeanDiffBeforeNoise", 300, -dPhi, dPhi);
  SigmaDiffBeforeNoise  = new TH1D("SigmaDiffBeforeNoise", "SigmaDiffBeforeNoise", 300, -1e-4, 1e-4);

  ChargeDiffAfterNoise = new TH1D("ChargeDiffAfterNoise", "ChargeDiffAfterNoise", 300, -100, 100);
  MeanDiffAfterNoise   = new TH1D("MeanDiffAfterNoise", "MeanDiffAfterNoise", 300, -3e-4, 3e-4);
  SigmaDiffAfterNoise  = new TH1D("SigmaDiffAfterNoise", "SigmaDiffAfterNoise", 300, -3e-4, 3e-4);

  double sqrt2pi = 2.506628275;
  NORMGAUSS    = new TF1("NORMGAUSS", "[0]*0.00396825/([2]*2.506628275)*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))", left, right);

  TRandom randy;

  double R5 = 467900.0;             // microns

  //  These are values at 17.5 inches...
  //double BlobSigmaMean  = 33.5e-4;  // radians
  //double BlobSigmaSigma =  4.1e-4*4.1/4.66;  // radians

  //  These are values at 14.0 inches...
  //double BlobSigmaMean  = 31.7e-4;  // radians
  //double BlobSigmaSigma =  3.5e-4*4.1/4.66;  // radians

  //  These are values at 10.0 inches...
  //double BlobSigmaMean  = 29.4e-4;  // radians
  //double BlobSigmaSigma =  3.0e-4*4.1/4.66;  // radians

  //  These are values at 4.0 inches...
  double BlobSigmaMean  = 25.1e-4;  // radians
  double BlobSigmaSigma =  2.0e-4*4.1/4.66;  // radians

  double BlobLandauMPV   = 326.8;
  double BlobLandauSigma = 135.1;

  for (int i=0; i<Nevent; i++)
    {
      double MEAN = (left + 0.1*(right-left)) + 0.8*(right-left)*randy.Rndm();
      BlobPosition->Fill(MEAN);

      double SIGMA = randy.Gaus(BlobSigmaMean, BlobSigmaSigma);
      BlobSigmaGen->Fill(SIGMA); // radians

      double CHARGE = randy.Landau(BlobLandauMPV, BlobLandauSigma);
      BlobCharge->Fill(CHARGE);

      NORMGAUSS->SetParameter(0,CHARGE);
      NORMGAUSS->SetParameter(1,MEAN);
      NORMGAUSS->SetParameter(2,SIGMA);

      for (int j=1; j<BlobPos->GetNbinsX()+1; j++)
	{
	  double x = BlobPos->GetBinCenter(j);
	  double height = NORMGAUSS->Eval(x);
	  BlobPos->SetBinContent(j,height);
	  BlobPos->SetBinError(j,9.0);
	}

      double max   = BlobPos->GetMaximum();
      double mean  = BlobPos->GetBinCenter(BlobPos->GetMaximumBin());
      double sigma = 0.003;
      if (sigma < dPhi/3.0) sigma = dPhi/3.0;
      if (sigma > 5.0*dPhi) sigma = 5.0*dPhi;
      
      BlobFit->SetParameter(0,max);
      BlobFit->SetParameter(1,mean);
      BlobFit->SetParameter(2,sigma);

      BlobFit->SetParLimits(0, max/2.0, 3.0*max);
      BlobFit->SetParLimits(1, mean-1.0*dPhi, mean+1.0*dPhi);
      BlobFit->SetParLimits(2, 0.0007, 0.0055);

      BlobPos->Fit(BlobFit,"Q0");

      double FITTEDCHARGE = BlobFit->GetParameter(0)*BlobFit->GetParameter(2)*sqrt2pi/dPhi;
      double FITTEDMEAN   = BlobFit->GetParameter(1);
      double FITTEDSIGMA  = BlobFit->GetParameter(2);

      ChargeQuality->Fill(CHARGE,FITTEDCHARGE);
      MeanQuality->Fill(MEAN,FITTEDMEAN);
      SigmaQuality->Fill(SIGMA,FITTEDSIGMA);

      ChargeDiffBeforeNoise->Fill(CHARGE-FITTEDCHARGE);
      MeanDiffBeforeNoise->Fill(MEAN-FITTEDMEAN);
      SigmaDiffBeforeNoise->Fill(SIGMA-FITTEDSIGMA);

      for (int j=1; j<BlobPos->GetNbinsX()+1; j++)
	{
	  double oldheight = BlobPos->GetBinContent(j);
	  double newheight = oldheight + randy.Gaus(0,NOISE);
	  BlobPos->SetBinContent(j,newheight);
	}

      max   = BlobPos->GetMaximum();
      mean  = BlobPos->GetBinCenter(BlobPos->GetMaximumBin());
      sigma = 0.003;
      if (sigma < dPhi/3.0) sigma = dPhi/3.0;
      if (sigma > 5.0*dPhi) sigma = 5.0*dPhi;
      
      BlobFit->SetParameter(0,max);
      BlobFit->SetParameter(1,mean);
      BlobFit->SetParameter(2,sigma);

      BlobFit->SetParLimits(0, max/2.0, 3.0*max);
      BlobFit->SetParLimits(1, mean-1.0*dPhi, mean+1.0*dPhi);
      BlobFit->SetParLimits(2, 0.0007, 0.0055);

      BlobPos->Fit(BlobFit,"Q0");

      double REFITTEDCHARGE = BlobFit->GetParameter(0)*BlobFit->GetParameter(2)*sqrt2pi;
      double REFITTEDMEAN   = BlobFit->GetParameter(1);
      double REFITTEDSIGMA  = BlobFit->GetParameter(2);

      ChargeDiffAfterNoise->Fill(CHARGE-REFITTEDCHARGE);
      MeanDiffAfterNoise->Fill(MEAN-REFITTEDMEAN);
      SigmaDiffAfterNoise->Fill(SIGMA-REFITTEDSIGMA);

      BlobSigma->Fill(REFITTEDSIGMA);

    }

}
