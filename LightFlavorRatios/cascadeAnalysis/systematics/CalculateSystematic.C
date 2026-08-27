void CalculateSystematic()
{
  //TFile* inputFile = TFile::Open("cutVarianceSystematics.root", "READ");
  //TFile* inputFile = TFile::Open("geoAccEffSystematics.root", "READ");
  //TFile* inputFile = TFile::Open("trackAccSystematics.root", "READ");
  TFile* inputFile = TFile::Open("fitFunctionSystematics.root", "READ");
  //TFile* inputFile = TFile::Open("cutEffSystematics.root", "READ");

  const std::vector<std::string> kinVars  = {"pT", "eta", "phi", "rap"};
  const std::vector<std::string> variants = {"base", "base_min", "base_max"};
  //const std::vector<std::string> variants = {"base", "base_max"};

  // Load all histograms and detach from file
  std::map<std::string, TH1F*> hMap;
  for (const auto& var : variants)
  {
    for (const auto& kin : kinVars)
    {
      for (const auto& par : {"KS0", "Xi"})
      {
        std::string suffix  = (var == "base") ? "" : var.substr(4); // "" / "_min" / "_max"
        std::string name    = "h_" + kin + "_base_" + par + suffix;
        TH1F* h             = (TH1F*)inputFile->Get(name.c_str());
        h->SetDirectory(0);
        hMap[var + "_" + kin + "_" + par] = h;
      }
    }
  }
  inputFile->Close();

  // Compute Xi/(2*KS0) ratios for each variant and kinematic variable
  std::map<std::string, TH1F*> ratioMap;
  for (const auto& var : variants)
  {
    for (const auto& kin : kinVars)
    {
      TH1F* hXi  = hMap.at(var + "_" + kin + "_Xi");
      TH1F* hKS0 = hMap.at(var + "_" + kin + "_KS0");

      TH1F* hRatio = (TH1F*)hXi->Clone(("ratio_" + var + "_" + kin).c_str());
      hRatio->SetDirectory(0);
      hRatio->Divide(hXi, hKS0, 1, 2); // Xi / (2*KS0) via scale factor
      ratioMap[var + "_" + kin] = hRatio;
    }
  }

  // Calculate systematic uncertainty as max bin-by-bin deviation of min/max from base
  //TFile* outputFile = TFile::Open("cutVarianceSystematicUncertainties.root", "RECREATE");
  //TFile* outputFile = TFile::Open("geoAccEffSystematicUncertainties.root", "RECREATE");
  //TFile* outputFile = TFile::Open("trackAccSystematicUncertainties.root", "RECREATE");
  //TFile* outputFile = TFile::Open("fitFunctionSystematicUncertainties.root", "RECREATE");
  //TFile* outputFile = TFile::Open("cutEffSystematicUncertainties.root", "RECREATE");

  for (const auto& kin : kinVars)
  {
    TH1F* hBase = ratioMap.at("base_"     + kin);
    TH1F* hMin  = ratioMap.at("base_min_" + kin);
    TH1F* hMax  = ratioMap.at("base_max_" + kin);

    TH1F* hSyst = (TH1F*)hBase->Clone(("h_syst_" + kin).c_str());
    hSyst->SetDirectory(0);
    hSyst->Reset();

    for (int b = 1; b <= hBase->GetNbinsX(); b++)
    {
      double base    = hBase->GetBinContent(b);
      double baseErr = hBase->GetBinError(b);
      double diffMin = std::abs(hMin->GetBinContent(b) - base);
      double diffMax = std::abs(hMax->GetBinContent(b) - base);
      double diffErr = diffMax > diffMin ? hMax->GetBinError(b) : hMin->GetBinError(b);
      //double diffErr = hMax->GetBinError(b);
      double diff = std::max(diffMin, diffMax);
      //double diff = diffMax;
      //hSyst->SetBinContent(b, std::max(diffMin, diffMax));
      double barlowSys = diff;
      //if (diff*diff > (baseErr*baseErr + diffErr*diffErr))
      //{
      //  barlowSys = std::sqrt(diff*diff - (baseErr*baseErr + diffErr*diffErr));
      //}
      //else
      //{
      //  barlowSys = 0.0;
      //}
      //std::cout << "diff: " << diff << std::endl;
      //std::cout << "baseErr: " << baseErr << std::endl;
      //std::cout << "diffErr: " << diffErr << std::endl;
      //std::cout << "barlowSys: " << barlowSys << std::endl;
      hSyst->SetBinContent(b, barlowSys);
      std::cout << "Kin: " << kin << ", bin: " << b << ", barlowSys: " << 100*barlowSys/base << std::endl;
    }

    //outputFile->cd();
    //hBase->Write(("h_ratio_base_" + kin).c_str());
    //hSyst->Write(("h_syst_"       + kin).c_str());
  }

  //outputFile->Close();
  //std::cout << "Systematics saved to cutVarianceSystematicUncertainties.root" << std::endl;
  //std::cout << "Systematics saved to geoAccEffSystematicUncertainties.root" << std::endl;
  //std::cout << "Systematics saved to trackAccSystematicUncertainties.root" << std::endl;
  //std::cout << "Systematics saved to fitFunctionSystematicUncertainties.root" << std::endl;
  //std::cout << "Systematics saved to cutEffSystematicUncertainties.root" << std::endl;
}
