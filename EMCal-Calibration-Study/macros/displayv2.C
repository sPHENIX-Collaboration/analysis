// ====================================================================
// My Includes
// ====================================================================
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>
#include <sPhenixStyle.C>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TCanvas.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TKey.h>
#include <TLegend.h>
#include <TLine.h>
#include <TROOT.h>

// ====================================================================
// Standard C++ Includes
// ====================================================================
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

// ====================================================================
// The Analysis Class
// ====================================================================
class Displayv2
{
 public:
  // The constructor takes the configuration
  Displayv2(std::string output_dir)
    : m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    read_hists();
    init_hists();
    read_calib();
    draw();
  }

 private:
  // Configuration stored as members
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  std::map<std::string, int> m_ctr;

  bool m_saveFig{true};
  bool m_debug{false};

  const std::string m_calib_default{"/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_calib_ADC_to_ETower_default/da/74/da74db11788df02ed879fa373c50c66c_EMCAL_ADC_to_Etower_2025_initial_v3.root"};
  const std::string m_calib_new{"output/EMCAL_ADC_to_Etower-new_newcdbtag_v008-73823.root"};
  const std::string m_fieldname{"CEMC_calib_ADC_to_ETower"};

  const std::string m_hist_default{"output/test-2025.root"};
  const std::string m_hist_new{"output/test-73823.root"};

  // --- Private Helper Methods ---
  void read_hists();
  void init_hists();
  void draw();
  void read_calib();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void Displayv2::read_hists()
{
  TH1::AddDirectory(kFALSE);

  // auto file = std::unique_ptr<TFile>(TFile::Open(m_input_file.c_str()));

  // // Check if the file was opened successfully.
  // if (!file || file->IsZombie())
  // {
  //   std::cout << std::format("Error: Could not open file '{}'\n", m_input_file);
  //   return;
  // }

  // // Get the list of keys (objects) in the file
  // TList* keyList = file->GetListOfKeys();
  // if (!keyList)
  // {
  //   std::cout << std::format("Error: Could not get list of keys\n");
  //   return;
  // }

  // // Use a TIter to loop through the keys
  // TIter next(keyList);
  // TKey* key;

  // std::cout << std::format("Histograms found in {}\n", m_input_file);

  // while ((key = static_cast<TKey*>(next())))
  // {
  //   // Get the class of the object from its key
  //   TClass* cl = gROOT->GetClass(key->GetClassName());

  //   // Check if the class inherits from TH1 (base histogram class)
  //   if (cl && cl->InheritsFrom("TH1"))
  //   {
  //       std::string name = key->GetName();
  //       std::cout << std::format("Reading Hist: {}\n", name);
  //       m_hists[name] = std::unique_ptr<TH1>(static_cast<TH1*>(file->Get(name.c_str())));
  //   }
  // }
}

void Displayv2::init_hists()
{
  // dummy hists for labeling
  m_hists["h2DummySector"] = std::make_unique<TH2F>("h2DummySector", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, 2, 0, 2);
  m_hists["h2DummyIB"] = std::make_unique<TH2F>("h2DummyIB", "", myUtils::m_nsector / 2, 0, myUtils::m_nsector / 2, myUtils::m_nib * 2 / myUtils::m_nsector, 0, myUtils::m_nib * 2 / myUtils::m_nsector);

  // setup Dummy hists
  for (int i = 0; i < myUtils::m_nsector; ++i)
  {
    int x = i % (myUtils::m_nsector / 2) + 1;
    int y = (i < (myUtils::m_nsector / 2)) ? 2 : 1;
    m_hists["h2DummySector"]->SetBinContent(x, y, i);
  }

  for (int i = 0; i < myUtils::m_nib; ++i)
  {
    int val = i % myUtils::m_nib_per_sector;
    int x = (i / myUtils::m_nib_per_sector) % (myUtils::m_nsector / 2) + 1;
    int y = (i < (myUtils::m_nib / 2)) ? myUtils::m_nib_per_sector - val : myUtils::m_nib_per_sector + val + 1;
    m_hists["h2DummyIB"]->SetBinContent(x, y, val);
  }
}

void Displayv2::read_calib()
{
  std::unique_ptr<CDBTTree> cdbttree_default = std::make_unique<CDBTTree>(m_calib_default);
  std::unique_ptr<CDBTTree> cdbttree_new = std::make_unique<CDBTTree>(m_calib_new);

  unsigned int ntowers = static_cast<unsigned int>(myUtils::m_nphi * myUtils::m_neta);

  std::string title;
  title = "EMCal Calibration [MeV/ADC]; Tower Index #phi; Tower Index #eta";
  m_hists["h2Calib_default"] = std::make_unique<TH2F>("h2Calib_default", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);
  m_hists["h2Calib_new"] = std::make_unique<TH2F>("h2Calib_new", title.c_str(), myUtils::m_nphi, 0, myUtils::m_nphi, myUtils::m_neta, 0, myUtils::m_neta);

  title = "EMCal Calibration [MeV/ADC]; Default; New";
  m_hists["hCalib_corr"] = std::make_unique<TH2F>("hCalib_corr", title.c_str(), 100, 0, 10, 100, 0, 10);

  title = "EMCal Calibration; Calibration [MeV/ADC]; Counts";
  m_hists["hCalib_default"] = std::make_unique<TH1F>("hCalib_default", title.c_str(), 100, 0, 10);
  m_hists["hCalib_new"] = std::make_unique<TH1F>("hCalib_new", title.c_str(), 100, 0, 10);

  m_hists["hCalib_default"]->Sumw2();
  m_hists["hCalib_new"]->Sumw2();

  for (unsigned int channel = 0; channel < ntowers; ++channel)
  {
    unsigned int key = TowerInfoDefs::encode_emcal(channel);
    int phi = static_cast<int>(TowerInfoDefs::getCaloTowerPhiBin(key));
    int eta = static_cast<int>(TowerInfoDefs::getCaloTowerEtaBin(key));

    double calib_default = cdbttree_default->GetFloatValue(static_cast<int>(key), m_fieldname) * 1e3;
    double calib_new = cdbttree_new->GetFloatValue(static_cast<int>(key), m_fieldname) * 1e3;

    dynamic_cast<TH2 *>(m_hists["h2Calib_default"].get())->SetBinContent(phi + 1, eta + 1, calib_default);
    dynamic_cast<TH2 *>(m_hists["h2Calib_new"].get())->SetBinContent(phi + 1, eta + 1, calib_new);
    dynamic_cast<TH2 *>(m_hists["hCalib_corr"].get())->Fill(calib_default, calib_new);

    m_hists["hCalib_default"]->Fill(calib_default);
    m_hists["hCalib_new"]->Fill(calib_new);

    if(m_debug && m_ctr["debug"]++ < 10)
    {
        std::cout << std::format("Calib Default: {:.2f}, New: {:.2f}\n", calib_default, calib_new);
    }
  }
}

void Displayv2::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.04f);
  c1->SetRightMargin(.08f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08f);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08f);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  gPad->SetGrid();

  for (const auto &[name, hist] : m_hists)
  {
    if (name.starts_with("h2"))
    {
      myUtils::setEMCalDim(hist.get());
    }
  }

  std::string output;

  // create output directory
  std::filesystem::create_directories(m_output_dir);
  std::filesystem::create_directories(m_output_dir + "/images");

  output = std::format("{}/plots-Calib.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  for (const auto &[name, hist] : m_hists)
  {
    if (name.starts_with("h2Calib"))
    {
      hist->Draw("COLZ");
      m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
      m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

      hist->SetMaximum(5);

      c1->Print(output.c_str(), "pdf portrait");
      c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
    }
  }

  c1->Print((output + "]").c_str(), "pdf portrait");

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.12f);
  c1->SetRightMargin(.03f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  gPad->SetGrid(0,0);
  gPad->SetLogy();

  output = std::format("{}/plots-Calib-Comp.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  m_hists["hCalib_default"]->Draw("HIST");
  m_hists["hCalib_new"]->Draw("HIST same");

  m_hists["hCalib_default"]->SetLineColor(kBlue);
  m_hists["hCalib_new"]->SetLineColor(kRed);

  m_hists["hCalib_default"]->SetLineWidth(3);
  m_hists["hCalib_new"]->SetLineWidth(3);

  double xshift = 0.5;
  double yshift = 0;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(m_hists["hCalib_default"].get(), "Default", "l");
  leg->AddEntry(m_hists["hCalib_new"].get(), "New", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  c1->Print(std::format("{}/images/{}.png", m_output_dir, "hCalib-overlay").c_str());

  c1->Print((output + "]").c_str(), "pdf portrait");

}

// void Displayv2::plot_overall_psi(TCanvas* c1, int order, const std::string& side, const std::string& output)
// {
//   std::string name = std::format("h3_sEPD_Psi_{}", order);
//   std::string direction = (side == "x") ? "South" : "North";
//   std::string title = std::format("sEPD {} #Psi_{{{}}}: |z| < 10 cm and MB", direction, order);

//   auto h = dynamic_cast<TH3*>(m_hists[name].get())->Project3D(side.c_str());
//   auto h_corr = dynamic_cast<TH3 *>(m_hists[std::format("{}_corr", name)].get())->Project3D(side.c_str());
//   auto h_corr2 = dynamic_cast<TH3 *>(m_hists[std::format("{}_corr2", name)].get())->Project3D(side.c_str());

//   h->Draw();
//   h_corr->Draw("same");
//   h_corr2->Draw("same");

//   h->SetLineColor(kRed);
//   h_corr->SetLineColor(kBlue);
//   h_corr2->SetLineColor(kGreen+3);

//   h->SetTitle(title.c_str());
//   h->GetYaxis()->SetMaxDigits(3);
//   h->GetYaxis()->SetTitle("Events");
//   h->GetYaxis()->SetTitleOffset(1.1f);
//   h->GetXaxis()->SetTitleOffset(1.f);
//   h->GetYaxis()->SetRangeUser(0, h->GetMaximum()*1.1);

//   double xshift = 0.25;
//   double yshift = 0;

//   std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
//   leg->SetFillStyle(0);
//   leg->SetTextSize(0.06f);
//   leg->AddEntry(h, "Default", "l");
//   leg->AddEntry(h_corr, "1^{st} Order Corrected", "l");
//   leg->AddEntry(h_corr2, "2^{nd} Order Corrected", "l");
//   leg->Draw("same");

//   c1->Print(output.c_str(), "pdf portrait");
//   if(m_saveFig) c1->Print(std::format("{}/images/{}-{}-overlay.png", m_output_dir, name, direction).c_str());
// }

// void Displayv2::plot_psi(TCanvas* c1, const std::string& name, int order, const std::string& side, short int color,  const std::string& output)
// {
//     std::string direction = (side == "x") ? "South" : "North";
//     std::string title = std::format("sEPD {} #Psi_{{{}}}: |z| < 10 cm and MB", direction, order);

//     auto h = dynamic_cast<TH3*>(m_hists[name].get());

//     for (int cent_bin = 1; cent_bin <= 10; ++cent_bin)
//     {
//       h->GetZaxis()->SetRange(cent_bin, cent_bin + 1);
//       std::string hist_name = std::format("{}", cent_bin);
//       auto hx = dynamic_cast<TH1*>(h->Project3D(side.c_str())->Clone(hist_name.c_str()));
//       if (cent_bin == 1)
//       {
//         hx->Draw();
//         hx->SetTitle(title.c_str());
//         hx->GetYaxis()->SetTitle("Events");
//         hx->GetYaxis()->SetTitleOffset(1.f);
//         hx->GetXaxis()->SetTitleOffset(1.f);
//         hx->GetYaxis()->SetMaxDigits(3);
//         hx->GetYaxis()->SetRangeUser(0, hx->GetMaximum()*1.2);
//       }
//       else
//       {
//         hx->Draw("same");
//       }

//       float alpha = static_cast<float>(1.1 - 0.1 * cent_bin);
//       hx->SetLineColorAlpha(color, alpha);
//     }

//   c1->Print(output.c_str(), "pdf portrait");
//   if(m_saveFig) c1->Print(std::format("{}/images/{}-{}.png", m_output_dir, name, side).c_str());
// }

// void Displayv2::plot_val1(TCanvas* c1, const std::string& name, const std::string& name_corr, const std::string& output)
// {
//   c1->SetLeftMargin(.08f);
//   c1->SetRightMargin(.03f);

//   auto h = m_hists[name].get();
//   auto h2 = m_hists[name_corr].get();

//   h->Draw("p e X0");
//   h2->Draw("same p e X0");

//   h->SetLineColor(kRed);
//   h2->SetLineColor(kBlue);

//   h->SetMarkerStyle(kFullDotLarge);
//   h2->SetMarkerStyle(kFullDotLarge);

//   h->SetMarkerColor(kRed);
//   h2->SetMarkerColor(kBlue);

//   if (h->GetBinContent(10) < 0)
//   {
//     h->GetYaxis()->SetRangeUser(h->GetMinimum() * 1.1, std::fabs(h->GetMinimum() * 0.35));
//   }
//   else{
//     h->GetYaxis()->SetRangeUser(-h->GetMaximum() * 0.15, std::fabs(h->GetMaximum() * 1.1));
//   }
//   h->GetYaxis()->SetMaxDigits(1);

//   h->GetYaxis()->SetTitleOffset(1.f);
//   h->GetXaxis()->SetTitleOffset(1.f);

//   double xshift = -0.1;
//   double yshift = 0.05;

//   std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
//   leg->SetFillStyle(0);
//   leg->SetTextSize(0.06f);
//   leg->AddEntry(h, "Default", "pe");
//   leg->AddEntry(h2, "Corrected", "pe");
//   leg->Draw("same");

//   std::unique_ptr<TLine> line = std::make_unique<TLine>(0, 0, 100, 0);

//   line->SetLineColor(kBlack);
//   line->SetLineStyle(kDashed);
//   line->Draw("same");

//   c1->Print(output.c_str(), "pdf portrait");
//   if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, name).c_str());
// }

// void Displayv2::plot_val2(TCanvas* c1, const std::string& side, int order, const std::string& output)
// {
//   std::string namex = std::format("h_sEPD_Q_{}_xx_{}_avg", side, order);
//   std::string namey = std::format("h_sEPD_Q_{}_yy_{}_avg", side, order);
//   std::string namex_corr = std::format("h_sEPD_Q_{}_xx_{}_corr_avg", side, order);
//   std::string namey_corr = std::format("h_sEPD_Q_{}_yy_{}_corr_avg", side, order);

//   auto hx = m_hists[namex].get();
//   auto hy = m_hists[namey].get();
//   auto hx_corr = m_hists[namex_corr].get();
//   auto hy_corr = m_hists[namey_corr].get();

//   hx->Divide(hy);
//   hx_corr->Divide(hy_corr);

//   hx->Draw("HIST P");
//   hx_corr->Draw("same HIST P");

//   hx->SetLineColor(kRed);
//   hx_corr->SetLineColor(kBlue);

//   hx->SetMarkerStyle(kFullDotLarge);
//   hx_corr->SetMarkerStyle(kFullDotLarge);

//   hx->SetMarkerColor(kRed);
//   hx_corr->SetMarkerColor(kBlue);

//   std::string ytitle = std::format("<Q_{{{},x}}^{{2}}> / <Q_{{{},y}}^{{2}}>", order, order);
//   hx->GetYaxis()->SetTitle(ytitle.c_str());
//   hx->GetYaxis()->SetRangeUser(0, 1.4);

//   hx->GetYaxis()->SetMaxDigits(1);

//   hx->GetYaxis()->SetTitleOffset(1.f);
//   hx->GetXaxis()->SetTitleOffset(1.f);

//   double xshift = -0.1;
//   double yshift = -0.5;

//   std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
//   leg->SetFillStyle(0);
//   leg->SetTextSize(0.06f);
//   leg->AddEntry(hx, "Default", "p");
//   leg->AddEntry(hx_corr, "Corrected", "p");
//   leg->Draw("same");

//   std::unique_ptr<TLine> line = std::make_unique<TLine>(0, 1, 100, 1);

//   line->SetLineColor(kBlack);
//   line->SetLineStyle(kDashed);
//   line->Draw("same");

//   c1->Print(output.c_str(), "pdf portrait");
//   if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, namex).c_str());
// }

//   // set sPHENIX plotting style
//   SetsPhenixStyle();

//   std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
//   c1->SetTickx();
//   c1->SetTicky();

//   c1->SetCanvasSize(1200, 1000);
//   c1->SetLeftMargin(.12f);
//   c1->SetRightMargin(.02f);
//   c1->SetTopMargin(.11f);
//   c1->SetBottomMargin(.09f);

//   gStyle->SetOptTitle(1);
//   gStyle->SetTitleStyle(0);
//   gStyle->SetTitleFontSize(0.08f);
//   gStyle->SetTitleW(1);
//   gStyle->SetTitleH(0.08f);
//   gStyle->SetTitleFillColor(0);
//   gStyle->SetTitleBorderSize(0);

//   std::string output;

//   // create output directory
//   std::filesystem::create_directories(m_output_dir);
//   std::filesystem::create_directories(m_output_dir + "/images");
//   output = std::format("{}/plots.pdf", m_output_dir);

//   c1->Print((output + "[").c_str(), "pdf portrait");

//   // Overall Summary Plots
//   plot_overall_psi(c1.get(), 2, "x", output);
//   plot_overall_psi(c1.get(), 2, "y", output);
//   plot_overall_psi(c1.get(), 3, "x", output);
//   plot_overall_psi(c1.get(), 3, "y", output);

//   // Individual
//   plot_psi(c1.get(), "h3_sEPD_Psi_2", 2, "x", kRed, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_2_corr", 2, "x", kBlue, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_2_corr2", 2, "x", kGreen+3, output);

//   plot_psi(c1.get(), "h3_sEPD_Psi_2", 2, "y", kRed, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_2_corr", 2, "y", kBlue, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_2_corr2", 2, "y", kGreen+3, output);

//   plot_psi(c1.get(), "h3_sEPD_Psi_3", 3, "x", kRed, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_3_corr", 3, "x", kBlue, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_3_corr2", 3, "x", kGreen+3, output);

//   plot_psi(c1.get(), "h3_sEPD_Psi_3", 3, "y", kRed, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_3_corr", 3, "y", kBlue, output);
//   plot_psi(c1.get(), "h3_sEPD_Psi_3_corr2", 3, "y", kGreen+3, output);

//   // Val 1
//   plot_val1(c1.get(), "h_sEPD_Q_S_x_2_avg", "h_sEPD_Q_S_x_2_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_S_y_2_avg", "h_sEPD_Q_S_y_2_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_N_x_2_avg", "h_sEPD_Q_N_x_2_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_N_y_2_avg", "h_sEPD_Q_N_y_2_corr_avg", output);

//   plot_val1(c1.get(), "h_sEPD_Q_S_x_3_avg", "h_sEPD_Q_S_x_3_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_S_y_3_avg", "h_sEPD_Q_S_y_3_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_N_x_3_avg", "h_sEPD_Q_N_x_3_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_N_y_3_avg", "h_sEPD_Q_N_y_3_corr_avg", output);

//   plot_val1(c1.get(), "h_sEPD_Q_S_xy_2_avg", "h_sEPD_Q_S_xy_2_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_N_xy_2_avg", "h_sEPD_Q_N_xy_2_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_S_xy_3_avg", "h_sEPD_Q_S_xy_3_corr_avg", output);
//   plot_val1(c1.get(), "h_sEPD_Q_N_xy_3_avg", "h_sEPD_Q_N_xy_3_corr_avg", output);

//   // 2nd Order Validation
//   plot_val2(c1.get(), "S", 2, output);
//   plot_val2(c1.get(), "N", 2, output);

//   plot_val2(c1.get(), "S", 3, output);
//   plot_val2(c1.get(), "N", 3, output);

//   c1->Print((output + "]").c_str(), "pdf portrait");
// }

int main(int argc, const char *const argv[])
{
  gROOT->SetBatch(true);

  if (argc > 2)
  {
    std::cout << "Usage: " << argv[0] << " [output_directory]" << std::endl;
    return 1;
  }

  std::string output_dir = (argc >= 2) ? argv[1] : ".";

  try
  {
    Displayv2 analysis(output_dir);
    analysis.run();
  }
  catch (const std::exception &e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
