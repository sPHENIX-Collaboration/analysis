void StableGauss(TH1* it, double numSigmas=2)
{
  double low   = it->GetBinCenter(1);
  double high  = it->GetBinCenter(it->GetNbinsX());

  for (i=0; i<3; i++)
    {
      it->Fit("gaus", "", "", low, high);
      double mean  = it->GetFunction("gaus")->GetParameter(1);
      double sigma = it->GetFunction("gaus")->GetParameter(2);
      low  = mean - numSigmas*sigma;
      high = mean + numSigmas*sigma;
    }
}
