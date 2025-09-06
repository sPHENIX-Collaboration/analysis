// -- sPHENIX includes
#include <calobase/TowerInfoDefs.h>

// for CDBTTree
#include <cdbobjects/CDBTTree.h>

#include <CDBUtils.C>

// -- sPHENIX Style
#include <sPhenixStyle.C>

// -- utils
#include "myUtils.C"

// -- root includes
#include <TCanvas.h>
#include <TH2.h>

// -- c++ includes
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <string>

void displayHotMap(unsigned int runnumber, const std::string &outputDir = ".", const std::string &dbtag = "newcdbtag", const std::string &overWriteURL = "", const std::string &overWriteFracChi2URL = "")
{
  std::cout << "##############################################" << std::endl;
  std::cout << "Processing" << std::endl;
  std::cout << std::format("Run: {}\n", runnumber);
  std::cout << std::format("Output Directory: {}\n", outputDir);
  std::cout << std::format("dbtag: {}\n", dbtag);
  std::cout << std::format("Overwrite URL Bad Towers: {}\n", overWriteURL);
  std::cout << std::format("Overwrite URL Frac Chi2: {}\n", overWriteFracChi2URL);
  std::cout << "##############################################" << std::endl;

  setGlobalTag(dbtag);
  std::string emcal_hotmap = (overWriteURL.empty()) ? getCalibration("CEMC_BadTowerMap", runnumber) : overWriteURL;
  std::string emcal_chi2 = (overWriteFracChi2URL.empty()) ? getCalibration("CEMC_hotTowers_fracBadChi2", runnumber) : overWriteFracChi2URL;

  if (emcal_hotmap.starts_with("DataBaseException"))
  {
    std::cout << std::format("Error! No hot map exists for Run: {}", runnumber) << std::endl;
    return;
  }

  bool doHotChi2 = !emcal_chi2.starts_with("DataBaseException");

  std::cout << std::format("EMCal Bad Tower Map: {}", emcal_hotmap) << std::endl;
  std::cout << std::format("EMCal Frac Bad Chi2: {}", emcal_chi2) << std::endl;

  std::unique_ptr<CDBTTree> cdbttree_hotMap = std::make_unique<CDBTTree>(emcal_hotmap);

  std::unique_ptr<CDBTTree> cdbttree_chi2 = (doHotChi2) ? std::make_unique<CDBTTree>(emcal_chi2) : nullptr;

  const std::string fieldname = "status";
  const std::string fieldname_chi2 = "fraction";
  const float fraction_badChi2_threshold = 0.01f;

  // set sPHENIX plotting style
  SetsPhenixStyle();

  // create output directories
  std::filesystem::create_directories(outputDir);

  std::map<std::string, std::unique_ptr<TH1>> hists;

  std::string title;

  title = std::format("EMCal isBad: Run {} ; Tower Index #phi; Tower Index #eta", runnumber);
  hists["h2EMCalBadTowers"] = std::make_unique<TH2F>("h2EMCalBadTowers", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  title = std::format("EMCal Dead: Run {} ; Tower Index #phi; Tower Index #eta", runnumber);
  hists["h2EMCalBadTowersDead"] = std::make_unique<TH2F>("h2EMCalBadTowersDead", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  title = std::format("EMCal Hot: Run {} ; Tower Index #phi; Tower Index #eta", runnumber);
  hists["h2EMCalBadTowersHot"] = std::make_unique<TH2F>("h2EMCalBadTowersHot", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  title = std::format("EMCal Cold: Run {} ; Tower Index #phi; Tower Index #eta", runnumber);
  hists["h2EMCalBadTowersCold"] = std::make_unique<TH2F>("h2EMCalBadTowersCold", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  title = std::format("EMCal Fraction badChi2: Run {}; Tower Index #phi; Tower Index #eta", runnumber);
  hists["h2EMCalBadTowersFracBadChi2"] = std::make_unique<TH2F>("h2EMCalBadTowersFracBadChi2", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  // dummy hists for labeling
  hists["h2DummySector"] = std::make_unique<TH2F>("h2DummySector", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, 2, 0, 2);
  hists["h2DummyIB"] = std::make_unique<TH2F>("h2DummyIB", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, myUtils::m_nib * 2 / myUtils::m_nsector, 0, myUtils::m_nib * 2 / myUtils::m_nsector);

  for (const auto &[name, hist] : hists)
  {
    myUtils::setEMCalDim(hist.get());
  }

  // setup Dummy hists
  for (int i = 0; i < myUtils::m_nsector; ++i)
  {
    int x = i % (myUtils::m_nsector / 2) + 1;
    int y = (i < (myUtils::m_nsector / 2)) ? 2 : 1;
    hists["h2DummySector"]->SetBinContent(x, y, i);
  }

  for (int i = 0; i < myUtils::m_nib; ++i)
  {
    int val = i % myUtils::m_nib_per_sector;
    int x = (i / myUtils::m_nib_per_sector) % (myUtils::m_nsector / 2) + 1;
    int y = (i < (myUtils::m_nib / 2)) ? myUtils::m_nib_per_sector - val : myUtils::m_nib_per_sector + val + 1;
    hists["h2DummyIB"]->SetBinContent(x, y, val);
  }

  // bad tower map hist
  unsigned int nTowers = static_cast<unsigned int>(myUtils::m_nphi * myUtils::m_neta);

  // status counters
  std::map<std::string, int> ctr;

  for (unsigned int channel = 0; channel < nTowers; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_emcal(channel);
    unsigned int phi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int eta = TowerInfoDefs::getCaloTowerEtaBin(key);

    int hotMap_val = cdbttree_hotMap->GetIntValue(static_cast<int>(key), fieldname);
    float fraction_badChi2 = (doHotChi2) ? cdbttree_chi2->GetFloatValue(static_cast<int>(key), fieldname_chi2) : 0;

    // nonzero means tower is bad
    if (hotMap_val != 0)
    {
      hists["h2EMCalBadTowers"]->Fill(phi, eta);
      ++ctr["bad"];
      if(hotMap_val == 1)
      {
        hists["h2EMCalBadTowersDead"]->Fill(phi, eta);
        ++ctr["dead"];
      }
      if(hotMap_val == 2)
      {
        hists["h2EMCalBadTowersHot"]->Fill(phi, eta);
        ++ctr["hot"];
      }
      if(hotMap_val == 3)
      {
        hists["h2EMCalBadTowersCold"]->Fill(phi, eta);
        ++ctr["cold"];
      }
    }

    if(fraction_badChi2 > fraction_badChi2_threshold)
    {
      hists["h2EMCalBadTowersFracBadChi2"]->Fill(phi, eta);
      ++ctr["fracBadChi2"];
    }
  }

  std::cout << std::format("Bad: {}, Dead: {}, Hot: {}, Cold: {}, Frac Bad Chi2: {}\n", ctr["bad"], ctr["dead"], ctr["hot"], ctr["cold"], ctr["fracBadChi2"]);

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.06f);
  c1->SetRightMargin(.02f);
  c1->SetTopMargin(.1f);
  c1->SetBottomMargin(.12f);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08f);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08f);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  gPad->SetGrid();

  c1->Print(std::format("{}/h2EMCalBadTowers-{}.pdf[", outputDir, runnumber).c_str(), "pdf portrait");

  for (const auto &[name, hist] : hists)
  {
    if(name.starts_with("h2EMCalBadTowers"))
    {
      hist->Draw("COL");
      hists["h2DummySector"]->Draw("TEXT MIN0 same");
      hists["h2DummyIB"]->Draw("TEXT MIN0 same");

      hist->SetMaximum(5);

      c1->Print(std::format("{}/h2EMCalBadTowers-{}.pdf", outputDir, runnumber).c_str(), "pdf portrait");
      c1->Print(std::format("{}/{}-{}.png", outputDir, name, runnumber).c_str());
    }
  }

  c1->Print(std::format("{}/h2EMCalBadTowers-{}.pdf]", outputDir, runnumber).c_str(), "pdf portrait");
}

#ifndef __CINT__
int main(int argc, const char *const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2 || args.size() > 6)
  {
    std::cerr << "usage: " << args[0] << " <runnumber> [outputDir] [dbtag] [overWriteURL] [overWriteFracChi2URL]" << std::endl;
    std::cerr << "  runnumber: Run" << std::endl;
    std::cerr << "  outputDir: Output Plot Directory" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    std::cerr << "  overWriteURL: (optional) Direct URL to EMCal Bad Tower Map (default: none)" << std::endl;
    std::cerr << "  overWriteFracChi2URL: (optional) Direct URL to EMCal Frac Chi2 (default: none)" << std::endl;
    return 1;  // Indicate error
  }

  unsigned int runnumber = static_cast<unsigned int>(std::stoul(args[1]));
  std::string outputDir = ".";
  std::string dbtag = "newcdbtag";
  std::string overWriteURL = "";
  std::string overWriteFracChi2URL = "";

  if (args.size() >= 3)
  {
    outputDir = args[2];
  }
  if (args.size() >= 4)
  {
    dbtag = args[3];
  }
  if (args.size() >= 5)
  {
    overWriteURL = args[4];
  }
  if (args.size() >= 6)
  {
    overWriteFracChi2URL = args[5];
  }

  displayHotMap(runnumber, outputDir, dbtag, overWriteURL, overWriteFracChi2URL);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
#endif
