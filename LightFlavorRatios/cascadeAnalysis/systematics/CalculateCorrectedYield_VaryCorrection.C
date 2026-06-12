void CalculateCorrectedYield_VaryCorrection(std::string inputFile)
{
  enum CorrIdx 
  {
    kPt  = 0,
    kPhi = 1,
    kEta = 2,
    kRap = 3,
    kPt_min  = 4,
    kPhi_min = 5,
    kEta_min = 6,
    kRap_min = 7,
    kPt_max  = 8,
    kPhi_max = 9,
    kEta_max = 10,
    kRap_max = 11,
    kNCorrTypes
  };

  std::vector<TH1F*> h_geoAccEff_KS0(kNCorrTypes, nullptr);
  std::vector<TH1F*> h_geoAccEff_Xi(kNCorrTypes,  nullptr);
  std::vector<TH1F*> h_cutEff_KS0(kNCorrTypes, nullptr);
  std::vector<TH1F*> h_cutEff_Xi(kNCorrTypes,  nullptr);
  std::vector<TH1F*> h_trackAcc_KS0(kNCorrTypes, nullptr);
  std::vector<TH1F*> h_trackAcc_Xi(kNCorrTypes,  nullptr);

  const std::vector<std::string> corrNames = {"pT", "phi", "eta", "rap"};

  TFile* correctionFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_evaluator/geometricAcceptanceEfficiencyCorrections.root", "READ");
  for (int i = 0; i < 4; i++)
  {
    TH1F* h_KS0 = (TH1F*)correctionFile->Get(("h_geoAccEff_" + corrNames[i] + "_KS0").c_str());
    h_geoAccEff_KS0[i] = (TH1F*)h_KS0->Clone(("h_geoAccEff_" + corrNames[i] + "_KS0_copy").c_str());
    h_geoAccEff_KS0[i]->SetDirectory(0);
 
    TH1F* h_Xi = (TH1F*)correctionFile->Get(("h_geoAccEff_" + corrNames[i] + "_Xi").c_str());
    h_geoAccEff_Xi[i] = (TH1F*)h_Xi->Clone(("h_geoAccEff_" + corrNames[i] + "_Xi_copy").c_str());
    h_geoAccEff_Xi[i]->SetDirectory(0);
  }  
  correctionFile->Close();
  
  correctionFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/geometric_acceptance/analysis/plots_evaluator/geometricAcceptanceEfficiencyCorrections_pTref0p3.root", "READ");
  for (int i = 0; i < 4; i++)
  {
    TH1F* h_KS0 = (TH1F*)correctionFile->Get(("h_geoAccEff_" + corrNames[i] + "_KS0").c_str());
    h_geoAccEff_KS0[i+4] = (TH1F*)h_KS0->Clone(("h_geoAccEff_" + corrNames[i] + "_KS0_min").c_str());
    h_geoAccEff_KS0[i+4]->SetDirectory(0);
 
    TH1F* h_Xi = (TH1F*)correctionFile->Get(("h_geoAccEff_" + corrNames[i] + "_Xi").c_str());
    h_geoAccEff_Xi[i+4] = (TH1F*)h_Xi->Clone(("h_geoAccEff_" + corrNames[i] + "_Xi_min").c_str());
    h_geoAccEff_Xi[i+4]->SetDirectory(0);
    
    h_geoAccEff_KS0[i+8] = (TH1F*)h_KS0->Clone(("h_geoAccEff_" + corrNames[i] + "_KS0_max").c_str());
    h_geoAccEff_KS0[i+8]->SetDirectory(0);
 
    h_geoAccEff_Xi[i+8] = (TH1F*)h_Xi->Clone(("h_geoAccEff_" + corrNames[i] + "_Xi_max").c_str());
    h_geoAccEff_Xi[i+8]->SetDirectory(0);
  }  
  correctionFile->Close();
  
  correctionFile = TFile::Open("/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/pTComp/cutEfficiencyCorrections.root", "READ");
  for (int i = 0; i < 4; i++)
  {
    TH1F* h_KS0 = (TH1F*)correctionFile->Get(("h_cutEff_" + corrNames[i] + "_KS0").c_str());
    h_cutEff_KS0[i] = (TH1F*)h_KS0->Clone(("h_cutEff_" + corrNames[i] + "_KS0_copy").c_str());
    h_cutEff_KS0[i]->SetDirectory(0);
 
    TH1F* h_Xi = (TH1F*)correctionFile->Get(("h_cutEff_" + corrNames[i] + "_Xi").c_str());
    h_cutEff_Xi[i] = (TH1F*)h_Xi->Clone(("h_cutEff_" + corrNames[i] + "_Xi_copy").c_str());
    h_cutEff_Xi[i]->SetDirectory(0);
   
    h_KS0 = (TH1F*)correctionFile->Get(("h_cutEff_" + corrNames[i] + "_KS0_max").c_str()); 
    h_cutEff_KS0[i+8] = (TH1F*)h_KS0->Clone(("h_cutEff_" + corrNames[i] + "_KS0_max_copy").c_str());
    h_cutEff_KS0[i+8]->SetDirectory(0);
 
    h_Xi = (TH1F*)correctionFile->Get(("h_cutEff_" + corrNames[i] + "_Xi_max").c_str());
    h_cutEff_Xi[i+8] = (TH1F*)h_Xi->Clone(("h_cutEff_" + corrNames[i] + "_Xi_max_copy").c_str());
    h_cutEff_Xi[i+8]->SetDirectory(0);
  }  
  correctionFile->Close();

  correctionFile = TFile::Open("/sphenix/user/rosstom/analysis/LightFlavorRatios/bachelor_pi_efficiency/analysis/trackingAcceptanceEfficiencyCorrections_withSystematics.root", "READ");
  std::string tailMod = "";
  for (int i = 0; i < kNCorrTypes; i++)
  {
    if (i == 4)
    {
      tailMod = "_high";
    }
    else if (i == 8)
    {
      tailMod = "_low";
    }
    TH1F* h_KS0 = (TH1F*)correctionFile->Get(("h_trackAcc_" + corrNames[i%4] + "_KS0" + tailMod).c_str());
    h_trackAcc_KS0[i] = (TH1F*)h_KS0->Clone(("h_trackAcc_" + corrNames[i%4] + "_KS0" + tailMod + "_copy").c_str());
    h_trackAcc_KS0[i]->SetDirectory(0);
 
    TH1F* h_Xi = (TH1F*)correctionFile->Get(("h_trackAcc_" + corrNames[i%4] + "_Xi" + tailMod).c_str());
    h_trackAcc_Xi[i] = (TH1F*)h_Xi->Clone(("h_trackAcc_" + corrNames[i%4] + "_Xi" + tailMod + "_copy").c_str());
    h_trackAcc_Xi[i]->SetDirectory(0);
  }  
  correctionFile->Close();
  
  double KS02pipi_BR_PDG = 0.6920;
  double KS02pipi_BR_uncert_PDG = 0.0005;
  double Xi2Lambdapi_BR_PDG = 0.99887;
  double Xi2Lambdapi_BR_uncert_PDG = 0.00035;
  double Lambda2ppi_BR_PDG = 0.641;
  double Lambda2ppi_BR_uncert_PDG = 0.005;

  TFile* inputRawYield = new TFile(inputFile.c_str());
  const std::vector<std::string> variants  = {"", "_max", "_min"};
  //const std::vector<std::string> variants  = {"", "_max"};
  const std::vector<std::string> particles = {"KS0", "Xi"};
  const std::vector<std::string> diffNames = {"pT", "phi", "eta", "rap"};

  std::map<std::string, TH1F*> corrMap_geoAccEff = 
  {
    {"KS0_pT",  h_geoAccEff_KS0[kPt]},
    {"KS0_phi", h_geoAccEff_KS0[kPhi]},
    {"KS0_eta", h_geoAccEff_KS0[kEta]},
    {"KS0_rap", h_geoAccEff_KS0[kRap]},
    {"Xi_pT",   h_geoAccEff_Xi[kPt]},
    {"Xi_phi",  h_geoAccEff_Xi[kPhi]},
    {"Xi_eta",  h_geoAccEff_Xi[kEta]},
    {"Xi_rap",  h_geoAccEff_Xi[kRap]},
    {"KS0_pT_min",  h_geoAccEff_KS0[kPt_min]},
    {"KS0_phi_min", h_geoAccEff_KS0[kPhi_min]},
    {"KS0_eta_min", h_geoAccEff_KS0[kEta_min]},
    {"KS0_rap_min", h_geoAccEff_KS0[kRap_min]},
    {"Xi_pT_min",   h_geoAccEff_Xi[kPt_min]},
    {"Xi_phi_min",  h_geoAccEff_Xi[kPhi_min]},
    {"Xi_eta_min",  h_geoAccEff_Xi[kEta_min]},
    {"Xi_rap_min",  h_geoAccEff_Xi[kRap_min]},
    {"KS0_pT_max",  h_geoAccEff_KS0[kPt_max]},
    {"KS0_phi_max", h_geoAccEff_KS0[kPhi_max]},
    {"KS0_eta_max", h_geoAccEff_KS0[kEta_max]},
    {"KS0_rap_max", h_geoAccEff_KS0[kRap_max]},
    {"Xi_pT_max",   h_geoAccEff_Xi[kPt_max]},
    {"Xi_phi_max",  h_geoAccEff_Xi[kPhi_max]},
    {"Xi_eta_max",  h_geoAccEff_Xi[kEta_max]},
    {"Xi_rap_max",  h_geoAccEff_Xi[kRap_max]},
  };

  std::map<std::string, TH1F*> corrMap_cutEff = 
  {
    {"KS0_pT",  h_cutEff_KS0[kPt]},
    {"KS0_phi", h_cutEff_KS0[kPhi]},
    {"KS0_eta", h_cutEff_KS0[kEta]},
    {"KS0_rap", h_cutEff_KS0[kRap]},
    {"Xi_pT",   h_cutEff_Xi[kPt]},
    {"Xi_phi",  h_cutEff_Xi[kPhi]},
    {"Xi_eta",  h_cutEff_Xi[kEta]},
    {"Xi_rap",  h_cutEff_Xi[kRap]},
    {"KS0_pT_max",  h_cutEff_KS0[kPt_max]},
    {"KS0_phi_max", h_cutEff_KS0[kPhi_max]},
    {"KS0_eta_max", h_cutEff_KS0[kEta_max]},
    {"KS0_rap_max", h_cutEff_KS0[kRap_max]},
    {"Xi_pT_max",   h_cutEff_Xi[kPt_max]},
    {"Xi_phi_max",  h_cutEff_Xi[kPhi_max]},
    {"Xi_eta_max",  h_cutEff_Xi[kEta_max]},
    {"Xi_rap_max",  h_cutEff_Xi[kRap_max]},
  };

  std::map<std::string, TH1F*> corrMap_trackAcc = 
  {
    {"KS0_pT",  h_trackAcc_KS0[kPt]},
    {"KS0_phi", h_trackAcc_KS0[kPhi]},
    {"KS0_eta", h_trackAcc_KS0[kEta]},
    {"KS0_rap", h_trackAcc_KS0[kRap]},
    {"Xi_pT",   h_trackAcc_Xi[kPt]},
    {"Xi_phi",  h_trackAcc_Xi[kPhi]},
    {"Xi_eta",  h_trackAcc_Xi[kEta]},
    {"Xi_rap",  h_trackAcc_Xi[kRap]},
    {"KS0_pT_min",  h_trackAcc_KS0[kPt_min]},
    {"KS0_phi_min", h_trackAcc_KS0[kPhi_min]},
    {"KS0_eta_min", h_trackAcc_KS0[kEta_min]},
    {"KS0_rap_min", h_trackAcc_KS0[kRap_min]},
    {"Xi_pT_min",   h_trackAcc_Xi[kPt_min]},
    {"Xi_phi_min",  h_trackAcc_Xi[kPhi_min]},
    {"Xi_eta_min",  h_trackAcc_Xi[kEta_min]},
    {"Xi_rap_min",  h_trackAcc_Xi[kRap_min]},
    {"KS0_pT_max",  h_trackAcc_KS0[kPt_max]},
    {"KS0_phi_max", h_trackAcc_KS0[kPhi_max]},
    {"KS0_eta_max", h_trackAcc_KS0[kEta_max]},
    {"KS0_rap_max", h_trackAcc_KS0[kRap_max]},
    {"Xi_pT_max",   h_trackAcc_Xi[kPt_max]},
    {"Xi_phi_max",  h_trackAcc_Xi[kPhi_max]},
    {"Xi_eta_max",  h_trackAcc_Xi[kEta_max]},
    {"Xi_rap_max",  h_trackAcc_Xi[kRap_max]},
  };

  TFile* outputFile = TFile::Open("cutVarianceSystematics.root", "RECREATE");

  for (const auto& var: variants)
  {
    for (const auto& par: particles)
    {
      for (const auto& dif: diffNames)
      {
        std::string histName = "h_" + dif + "_base_" + par + var;
        std::cout << "Correcting histogram : " << histName << std::endl;
        TH1F* h = (TH1F*)inputRawYield->Get(histName.c_str());
        h->SetDirectory(0);
        int nBins = h->GetNbinsX();
        
        for (int b = 1; b <= nBins; b++)
        {
          double yield = h->GetBinContent(b);
          double statErr = h->GetBinError(b);
        
          double geoAccEff = corrMap_geoAccEff.at(par + "_" + dif)->GetBinContent(b);  
          double cutEff = corrMap_cutEff.at(par + "_" + dif)->GetBinContent(b);  
          double trackAcc = corrMap_trackAcc.at(par + "_" + dif)->GetBinContent(b);  

          double totalCorr;
          if (par == "KS0")
          {
            //totalCorr  = geoAccEff * cutEff * trackAcc * KS02pipi_BR_PDG;
            totalCorr  = cutEff * KS02pipi_BR_PDG;
          }
          else
          {
            //totalCorr  = geoAccEff * cutEff * trackAcc * Xi2Lambdapi_BR_PDG * Lambda2ppi_BR_PDG;
            totalCorr  = cutEff * Xi2Lambdapi_BR_PDG * Lambda2ppi_BR_PDG;
          }
          double corrYield  = yield / totalCorr;
          double corrErr    = statErr / totalCorr;
          h->SetBinContent(b, corrYield);
          h->SetBinError(b, corrErr);
        }

        //h->Divide(corrMap_geoAccEff.at(par + "_" + dif));
        //h->Divide(corrMap_cutEff.at(par + "_" + dif + var));
        //h->Divide(corrMap_trackAcc.at(par + "_" + dif));
        //if (par == "KS0")
        //{
        //  h->Scale(1.0/(KS02pipi_BR_PDG));
        //}
        //else
        //{
        //  h->Scale((1.0/(Xi2Lambdapi_BR_PDG*Lambda2ppi_BR_PDG)));
        //}
        //h->SetName((histName + var).c_str());
        h->SetName((histName).c_str());
        outputFile->cd();
        h->Write();
      }
    }
  }
}
