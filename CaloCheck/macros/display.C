// sPHENIX includes
#include <sPhenixStyle.C>
#include <calobase/TowerInfoDefs.h>

// -- root includes
#include <TCanvas.h>
#include <TH2F.h>
#include <TPaletteAxis.h>

#include "src/geometry_constants.h"

// c++ includes
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <format>
#include <map>
#include <filesystem>
#include <vector>
#include <sstream>

R__LOAD_LIBRARY(libcalo_io.so)

void setEMCalDim(TH1 *hist)
{
  hist->GetXaxis()->SetLimits(0, 256);
  hist->GetXaxis()->SetNdivisions(32, false);
  hist->GetXaxis()->SetLabelSize(0.04F);
  hist->GetXaxis()->SetTickSize(0.01F);
  hist->GetYaxis()->SetTickSize(0.01F);
  hist->GetYaxis()->SetLabelSize(0.04F);
  hist->GetYaxis()->SetLimits(0, 96);
  hist->GetYaxis()->SetNdivisions(12, false);
  hist->GetYaxis()->SetTitleOffset(0.5);
  hist->GetXaxis()->SetTitleOffset(1);
}

void setEMCalIBDims(TH1 *hist, int xlow, int ylow)
{
  hist->GetXaxis()->SetLimits(xlow, xlow+8);
  hist->GetXaxis()->SetNdivisions(8, false);
  hist->GetXaxis()->SetLabelSize(0.04F);
  hist->GetXaxis()->SetTickSize(0.01F);
  hist->GetYaxis()->SetTickSize(0.01F);
  hist->GetYaxis()->SetLabelSize(0.04F);
  hist->GetYaxis()->SetLimits(ylow, ylow+8);
  hist->GetYaxis()->SetNdivisions(8, false);
  hist->GetYaxis()->SetTitleOffset(0.5);
  hist->GetXaxis()->SetTitleOffset(1);
}

void create_default_emcal(TH1* h2Sector, TH1* h2IB)
{
  for (unsigned int phi = 0; phi < CaloGeometry::CEMC_PHI_BINS; ++phi)
  {
    for (unsigned int eta = 0; eta < CaloGeometry::CEMC_ETA_BINS; ++eta)
    {
      unsigned int key = TowerInfoDefs::encode_emcal(eta, phi);
      unsigned int towerIndex = TowerInfoDefs::decode_emcal(key);

      std::pair<int, int> sector_ib = TowerInfoDefs::getEMCalSectorIB(towerIndex);
      int sector = sector_ib.first;
      int ib = sector_ib.second;

      dynamic_cast<TH2 *>(h2Sector)->Fill(phi, eta, sector);
      dynamic_cast<TH2 *>(h2IB)->Fill(phi, eta, ib);
    }
  }

  dynamic_cast<TH2 *>(h2Sector)->Rebin2D(8, 48);
  dynamic_cast<TH2 *>(h2Sector)->Scale(1. / CaloGeometry::CEMC_NCHANNEL_PER_SECTOR);

  dynamic_cast<TH2 *>(h2IB)->Rebin2D(8, 8);
  dynamic_cast<TH2 *>(h2IB)->Scale(1. / CaloGeometry::CEMC_NCHANNEL_PER_IB);

  setEMCalDim(h2Sector);
  setEMCalDim(h2IB);
}

void init(std::map<std::string, std::unique_ptr<TH1>> &hists)
{
  // init hists
  hists["h2Sector"] = std::make_unique<TH2F>("h2Sector", "; Tower Index #phi; Tower Index #eta", CaloGeometry::CEMC_PHI_BINS, 0, CaloGeometry::CEMC_PHI_BINS, CaloGeometry::CEMC_ETA_BINS, 0, CaloGeometry::CEMC_ETA_BINS);
  hists["h2IB"] = std::make_unique<TH2F>("h2IB", "; Tower Index #phi; Tower Index #eta", CaloGeometry::CEMC_PHI_BINS, 0, CaloGeometry::CEMC_PHI_BINS, CaloGeometry::CEMC_ETA_BINS, 0, CaloGeometry::CEMC_ETA_BINS);

  create_default_emcal(hists["h2Sector"].get(), hists["h2IB"].get());
}

std::vector<std::string> split(const std::string &s, const char delimiter)
{
    std::vector<std::string> result;

    std::stringstream ss(s);
    std::string temp;

    while(getline(ss,temp,delimiter)) {
        if(!temp.empty()) {
            result.push_back(temp);
        }
    }

    return result;
}

void display(const std::string &outputDir="plots", bool save_fig = true)
{

  std::string outputPdf = outputDir + "/pdf";
  std::string outputImg = outputDir + "/images";

  // create output dir
  std::filesystem::create_directories(outputPdf);
  std::filesystem::create_directories(outputImg);

  // set sPHENIX plotting style
  SetsPhenixStyle();

  TH1::AddDirectory(kFALSE);

  std::map<std::string, std::unique_ptr<TH1>> hists;
  init(hists);

  std::map<std::string, std::unique_ptr<TFile>> tfiles;
  std::map<std::string, std::string> tfiles_path;

  std::string input_default = "data/test-default-47289.root";
  std::string input_new = "data/test-47289.root";

  tfiles_path["default"] = input_default;
  tfiles_path["new"] = input_new;

  tfiles["default"] = std::unique_ptr<TFile>(TFile::Open(input_default.c_str()));
  tfiles["new"] = std::unique_ptr<TFile>(TFile::Open(input_new.c_str()));

  for (const auto &[name, tfile] : tfiles)
  {
    if (!tfile || tfile->IsZombie())
    {
      std::cout << std::format("Error: Could not open ROOT file: {}", tfiles_path[name]) << std::endl;
      return;
    }

    TList* keys = tfile->GetListOfKeys();

    for(const auto &key : *keys)
    {
      std::string thist_name = key->GetName();
      std::string hist_name = std::format("{}_{}", thist_name, name);

      std::cout << std::format("Name: {}", thist_name) << std::endl;
      hists[hist_name] = std::unique_ptr<TH2>(static_cast<TH2*>(tfile->Get(thist_name.c_str())->Clone(hist_name.c_str())));
      setEMCalDim(hists[hist_name].get());
    }
  }

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.06F);
  c1->SetRightMargin(.09F);
  c1->SetTopMargin(.09F);
  c1->SetBottomMargin(.12F);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08F);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08F);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  gPad->SetGrid();

  std::map<std::string, std::pair<int, int>> hist_axis_lims;

  hist_axis_lims["47289_3460418"] = std::make_pair(56, 16);
  hist_axis_lims["47289_3473947"] = std::make_pair(232, 40);

  for (const auto &[name, hist] : hists)
  {
    if(name == "h2Sector" || name == "h2IB")
    {
      continue;
    }

    std::vector<std::string> tokens = split(name, '_');
    std::string run = tokens[2];
    std::string event = tokens[3];

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06F);
    c1->SetRightMargin(.09F);
    c1->SetTopMargin(.09F);
    c1->SetBottomMargin(.12F);

    hist->Draw("COLZ1");

    if(name.starts_with("h2_isBadChi2"))
    {
      hist->SetMaximum(5);
    }

    hists["h2Sector"]->Draw("TEXT MIN0 same");
    hists["h2IB"]->Draw("TEXT MIN0 same");

    hist->GetXaxis()->SetTitleSize(0.05F);
    hist->GetYaxis()->SetTitleSize(0.05F);

    if(save_fig)
    {
      c1->Print(std::format("{}/{}.png",outputImg, name).c_str());
    }
    c1->Print(std::format("{}/{}.pdf",outputPdf, name).c_str());

    c1->SetCanvasSize(1300, 1000);
    c1->SetLeftMargin(.09F);
    c1->SetRightMargin(.13F);
    c1->SetTopMargin(.09F);
    c1->SetBottomMargin(.12F);

    int xlow = hist_axis_lims[std::format("{}_{}", run, event)].first;
    int ylow = hist_axis_lims[std::format("{}_{}", run, event)].second;

    hist->Draw("COLZ1");

    hist->GetXaxis()->SetRangeUser(xlow, xlow+8);
    hist->GetYaxis()->SetRangeUser(ylow, ylow+8);
    hist->GetYaxis()->SetTitleOffset(0.8F);

    hist->GetXaxis()->SetNdivisions(8, false);
    hist->GetYaxis()->SetNdivisions(8, false);

    hist->SetMinimum(0);

    TPaletteAxis *palette = dynamic_cast<TPaletteAxis *>(hist->GetListOfFunctions()->FindObject("palette"));
    if (palette)
    {
        float x1 = 0.88;
        float x1_shift = 0.05;
        float y1 = 0.12;
        float y1_shift = 0.8;
        palette->SetX1NDC(x1); // Set the left edge of the palette in NDC coordinates
        palette->SetY1NDC(y1); // Set the bottom edge
        palette->SetX2NDC(x1+x1_shift); // Set the right edge
        palette->SetY2NDC(y1+y1_shift); // Set the top edge
    }
    gPad->Update(); // Update the canvas to reflect the changes

    if(save_fig)
    {
      c1->Print(std::format("{}/{}-zoom.png",outputImg, name).c_str());
    }
    c1->Print(std::format("{}/{}-zoom.pdf",outputPdf, name).c_str());
  }
}
