// -- utils
#include "myUtils.C"

// ====================================================================
// sPHENIX Includes
// ====================================================================
#include <sPhenixStyle.C>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TKey.h>
#include <TLegend.h>
#include <TLine.h>
#include <TPaletteAxis.h>
#include <TProfile.h>
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
class DisplayJetAna
{
 public:
  // The constructor takes the configuration
  DisplayJetAna(std::string input_file, std::string output_dir)
    : m_input_file(std::move(input_file))
    , m_output_dir(std::move(output_dir))
  {
  }

  void run()
  {
    read_hists();
    init_hists();
    compute_vn();
    draw();
  }

 private:
  // Configuration stored as members
  std::string m_input_file;
  std::string m_output_dir;
  std::map<std::string, std::unique_ptr<TH1>> m_hists;
  static constexpr std::array<int, 3> m_harmonics = {2, 3, 4};

  bool m_saveFig{true};

  // Hists Config
  int m_bins_cent{8};
  double m_cent_low{-0.5};
  double m_cent_high{79.5};

  int m_bins_SP{400};
  double m_SP_low{-1};
  double m_SP_high{1};

  int m_bins_sample{25};

  struct HistConfig
  {
      int bins{0};
      double low{0};
      double high{0};
  };

  // --- Private Helper Methods ---
  void read_hists();
  void init_hists();
  void compute_vn();

  std::unique_ptr<TProfile> make_profile(const TH2* h2, const std::string &name, const std::string &title);
  std::unique_ptr<TH2> make_TH2(const TH2* h2, const TH2* h2_weights, const std::string &name, const std::string &title, int bins, double low, double high, bool doResolution = false);
  void draw_sEPD_corr_res(TCanvas* c1, int n, int bins, double low, double high, bool doResolution, std::string &title, const std::string &tag, std::string &output);
  void draw();
};

// ====================================================================
// Implementation of the Class Methods
// ====================================================================
void DisplayJetAna::read_hists()
{
  TH1::AddDirectory(kFALSE);

  auto file = std::unique_ptr<TFile>(TFile::Open(m_input_file.c_str()));

  // Check if the file was opened successfully.
  if (!file || file->IsZombie())
  {
    std::cout << std::format("Error: Could not open file '{}'\n", m_input_file);
    return;
  }

  // Get the list of keys (objects) in the file
  TList* keyList = file->GetListOfKeys();
  if (!keyList)
  {
    std::cout << std::format("Error: Could not get list of keys\n");
    return;
  }

  // Use a TIter to loop through the keys
  TIter next(keyList);
  TKey* key;

  std::cout << std::format("Histograms found in {}\n", m_input_file);

  while ((key = static_cast<TKey*>(next())))
  {
    // Get the class of the object from its key
    TClass* cl = gROOT->GetClass(key->GetClassName());

    // Check if the class inherits from TH1 (base histogram class)
    if (cl && cl->InheritsFrom("TH1"))
    {
      std::string name = key->GetName();
      std::cout << std::format("Reading Hist: {}\n", name);
      m_hists[name] = std::unique_ptr<TH1>(static_cast<TH1*>(file->Get(name.c_str())));
    }
  }
}

void DisplayJetAna::init_hists()
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

  myUtils::setEMCalDim(m_hists["h2DummySector"].get());
  myUtils::setEMCalDim(m_hists["h2DummyIB"].get());

  // Vn
  for (auto n : m_harmonics)
  {
    std::string name_vn = std::format("h2Vn_re_{}", n);
    std::string title = std::format("Jet v_{{{0}}}; Centrality [%]; v_{{{0}}}", n);

    m_hists[name_vn] = std::make_unique<TH2F>(name_vn.c_str(), title.c_str(),
                                              m_bins_cent, m_cent_low, m_cent_high,
                                              m_bins_SP, m_SP_low, m_SP_high);

    name_vn = std::format("h2Vn_im_{}", n);
    m_hists[name_vn] = std::make_unique<TH2F>(name_vn.c_str(), title.c_str(),
                                              m_bins_cent, m_cent_low, m_cent_high,
                                              m_bins_SP, m_SP_low, m_SP_high);

  }
}

void DisplayJetAna::compute_vn()
{
  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    int n = m_harmonics[n_idx];

    auto* prof_re = dynamic_cast<TH2*>(m_hists[std::format("h2SP_re_prof_{}", n)].get());
    auto* prof_im = dynamic_cast<TH2*>(m_hists[std::format("h2SP_im_prof_{}", n)].get());
    auto* prof_res = dynamic_cast<TH2*>(m_hists[std::format("h2SP_res_prof_{}", n)].get());

    auto* h2Vn_re = dynamic_cast<TH2*>(m_hists[std::format("h2Vn_re_{}", n)].get());
    auto* h2Vn_im = dynamic_cast<TH2*>(m_hists[std::format("h2Vn_im_{}", n)].get());

    auto* h2Jet = dynamic_cast<TH2*>(m_hists["h2Jet"].get());

    for (int cent_bin = 0; cent_bin < static_cast<int>(m_bins_cent); ++cent_bin)
    {
      double cent = m_hists["hCentrality"]->GetBinCenter(cent_bin + 1);

      for (int sample_bin = 0; sample_bin < m_bins_sample; ++sample_bin)
      {
        double SP_re = prof_re->GetBinContent(cent_bin + 1, sample_bin + 1);
        double SP_im = prof_im->GetBinContent(cent_bin + 1, sample_bin + 1);
        double SP_res = prof_res->GetBinContent(cent_bin + 1, sample_bin + 1);

        int nJets = static_cast<int>(h2Jet->GetBinContent(cent_bin + 1, sample_bin + 1));

        if (SP_res > 0)
        {
          double vn_re = SP_re / std::sqrt(SP_res);
          double vn_im = SP_im / std::sqrt(SP_res);

          h2Vn_re->Fill(cent, vn_re, nJets);
          h2Vn_im->Fill(cent, vn_im, nJets);
        }
      }
    }
  }
}

std::unique_ptr<TProfile> DisplayJetAna::make_profile(const TH2* h2, const std::string &name, const std::string &title)
{
  int bins_x = h2->GetNbinsX();
  int bins_y = h2->GetNbinsY();
  double x_low = h2->GetXaxis()->GetXmin();
  double x_high = h2->GetXaxis()->GetXmax();

  auto hprof = std::make_unique<TProfile>(name.c_str(), title.c_str(), bins_x, x_low, x_high, "s");

  for(int bin_x = 1; bin_x <= bins_x; ++bin_x)
  {
    for(int bin_y = 1; bin_y <= bins_y; ++bin_y)
    {
        double val = h2->GetBinContent(bin_x, bin_y);
        double x = hprof->GetBinCenter(bin_x);
        hprof->Fill(x, val);
    }
  }

  return hprof;
}

std::unique_ptr<TH2> DisplayJetAna::make_TH2(const TH2* h2, const TH2* h2_weights, const std::string &name, const std::string &title, int bins, double low, double high, bool doResolution)
{
  int bins_x = h2->GetNbinsX();
  int bins_y = h2->GetNbinsY();
  double x_low = h2->GetXaxis()->GetXmin();
  double x_high = h2->GetXaxis()->GetXmax();

  auto h2_new = std::make_unique<TH2F>(name.c_str(), title.c_str(), bins_x, x_low, x_high, bins, low, high);

  for (int bin_x = 1; bin_x <= bins_x; ++bin_x)
  {
    for (int bin_y = 1; bin_y <= bins_y; ++bin_y)
    {
      double val = h2->GetBinContent(bin_x, bin_y);
      double weight = h2_weights->GetBinContent(bin_x, bin_y);
      double x = h2_new->GetXaxis()->GetBinCenter(bin_x);
      if ((doResolution && val > 0) || !doResolution)
      {
        val = (doResolution) ? std::sqrt(val) : val;
        h2_new->Fill(x, val, weight);
      }
    }
  }

  return h2_new;
}

void DisplayJetAna::draw_sEPD_corr_res(TCanvas* c1, int n, int bins, double low, double high, bool doResolution, std::string &title, const std::string &tag, std::string &output)
{
  auto* hCentrality = m_hists["hCentrality"].get();
  std::unique_ptr<TLine> line = std::make_unique<TLine>(hCentrality->GetXaxis()->GetXmin(), 0, hCentrality->GetXaxis()->GetXmax(), 0);

  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);

  auto* h2Event = dynamic_cast<TH2*>(m_hists["h2Event"].get());

  std::string hist_name = std::format("h2SP_res_prof_{}", n);
  auto* h2SP_res_prof = dynamic_cast<TH2*>(m_hists[hist_name].get());

  std::string hist2D_name = std::format("h2SP_res_{}", n);
  std::string hist_prof_name = std::format("hSP_res_prof_{}", n);

  auto h2SP_res = make_TH2(h2SP_res_prof, h2Event, hist2D_name, title, bins, low, high, doResolution);
  auto* hSP_res_prof = h2SP_res->ProfileX(hist_prof_name.c_str(), 1, -1, "s");

  h2SP_res->Draw("COLZ1");
  hSP_res_prof->Draw("same p e X0");
  if (!doResolution)
  {
    line->Draw("same");
  }

  h2SP_res->GetZaxis()->SetMaxDigits(3);
  h2SP_res->GetYaxis()->SetTitleOffset(1.5f);
  h2SP_res->GetXaxis()->SetTitleOffset(1.f);

  h2SP_res->SetMinimum(0);
  h2SP_res->SetMaximum(2e6);

  hSP_res_prof->SetMarkerColor(kRed);
  hSP_res_prof->SetLineColor(kRed);
  hSP_res_prof->SetMarkerStyle(kFullDotLarge);
  hSP_res_prof->SetLineWidth(3);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}-{}.png", m_output_dir, hist2D_name, tag).c_str());
}

void DisplayJetAna::draw()
{
  // set sPHENIX plotting style
  SetsPhenixStyle();

  std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>("c1");
  c1->SetTickx();
  c1->SetTicky();

  c1->SetCanvasSize(2900, 1000);
  c1->SetLeftMargin(.06f);
  c1->SetRightMargin(.02f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  gStyle->SetOptTitle(1);
  gStyle->SetTitleStyle(0);
  gStyle->SetTitleFontSize(0.08f);
  gStyle->SetTitleW(1);
  gStyle->SetTitleH(0.08f);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleBorderSize(0);

  std::string output;

  // create output directory
  std::filesystem::create_directories(m_output_dir);
  std::filesystem::create_directories(m_output_dir + "/images");
  output = std::format("{}/plots-phi-eta.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  // ---------------------------------------------
  gPad->SetGrid();

  auto h2Dead = dynamic_cast<TH2*>(m_hists["h2EMCalBadTowersDead"].get());
  auto h2Deadv2 = dynamic_cast<TH2*>(m_hists["h2EMCalBadTowersDeadv2"].get());

  myUtils::setEMCalDim(h2Dead);
  myUtils::setEMCalDim(h2Deadv2);

  h2Dead->Draw("COL");
  m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
  m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2EMCalBadTowersDead").c_str());

  h2Deadv2->Draw("COL");
  m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
  m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2EMCalBadTowersDeadv2").c_str());

  // ---------------------------------------------

  gPad->SetGrid(0, 0);
  c1->SetLeftMargin(.03f);
  c1->SetRightMargin(.07f);

  auto h2JetPhiEta = dynamic_cast<TH2*>(dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPt"].get())->Project3D("yx"));
  auto h2JetPhiEtav2 = dynamic_cast<TH2*>(dynamic_cast<TH3*>(m_hists["h3JetPhiEtaPtv2"].get())->Project3D("yx"));

  h2JetPhiEta->Draw("COLZ1");
  gPad->Update();

  TPaletteAxis* palette = dynamic_cast<TPaletteAxis*>(h2JetPhiEta->GetListOfFunctions()->FindObject("palette"));
  palette->SetX2NDC(.97f);

  h2JetPhiEta->SetMaximum(5e3);
  h2JetPhiEta->GetYaxis()->SetTitleOffset(0.4f);
  h2JetPhiEta->GetZaxis()->SetMaxDigits(2);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2JetPhiEta").c_str());

  h2JetPhiEtav2->Draw("COLZ1");
  gPad->Update();

  palette = dynamic_cast<TPaletteAxis*>(h2JetPhiEtav2->GetListOfFunctions()->FindObject("palette"));
  palette->SetX2NDC(.97f);

  h2JetPhiEtav2->SetMaximum(5e3);
  h2JetPhiEtav2->GetYaxis()->SetTitleOffset(0.4f);
  h2JetPhiEtav2->GetZaxis()->SetMaxDigits(2);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2JetPhiEtav2").c_str());

  c1->Print((output + "]").c_str(), "pdf portrait");

  // -------------------------------------------

  c1->SetCanvasSize(1200, 1000);
  c1->SetLeftMargin(.12f);
  c1->SetRightMargin(.02f);
  c1->SetTopMargin(.11f);
  c1->SetBottomMargin(.09f);

  output = std::format("{}/plots.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  auto hJetPhi = h2JetPhiEta->ProjectionX();
  auto hJetPhiv2 = h2JetPhiEtav2->ProjectionX();

  hJetPhi->Draw();
  hJetPhi->SetLineColor(kBlue);
  hJetPhi->SetLineWidth(3);
  hJetPhi->GetYaxis()->SetRangeUser(0, 5.5e4);
  hJetPhi->GetYaxis()->SetMaxDigits(3);
  hJetPhi->GetYaxis()->SetTitle("Jet Yield / (2#pi / 64)");
  hJetPhi->GetYaxis()->SetTitleOffset(1.f);

  hJetPhiv2->Draw("same");
  hJetPhiv2->SetLineColor(kRed);
  hJetPhiv2->SetLineWidth(3);

  double xshift = 0.17;
  double yshift = 0.05;

  std::unique_ptr<TLegend> leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hJetPhi, "Default", "l");
  leg->AddEntry(hJetPhiv2, "Dead IB Mask", "l");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJetPhi-overlay").c_str());

  // -------------------------------------------

  auto* hCentrality = m_hists["hCentrality"].get();

  hCentrality->Draw();
  hCentrality->SetLineColor(kBlue);
  hCentrality->SetLineWidth(3);
  hCentrality->GetYaxis()->SetRangeUser(0, hCentrality->GetMaximum()*1.1);
  hCentrality->GetYaxis()->SetMaxDigits(3);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hCentrality").c_str());

  // -------------------------------------------

  auto* h2Event = dynamic_cast<TH2*>(m_hists["h2Event"].get());
  std::string title = "Centrality: |z| < 10 cm and MB; Centrality [%]; Average Events / 25 Samples";

  auto hEvent = make_profile(h2Event, "hEvent", title);

  hEvent->Draw("HIST");
  hEvent->Draw("p e X0 same");

  hEvent->GetYaxis()->SetMaxDigits(3);
  hEvent->GetYaxis()->SetTitleOffset(1.f);
  hEvent->GetXaxis()->SetTitleOffset(0.85f);
  hEvent->GetYaxis()->SetRangeUser(0, hEvent->GetMaximum()*1.1);
  hEvent->SetLineWidth(3);
  hEvent->SetLineColor(kBlue);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hEvent").c_str());

  // -------------------------------------------

  gPad->SetLogy();
  auto* h2Jet = dynamic_cast<TH2*>(m_hists["h2Jet"].get());
  title = "Jets: |z| < 10 cm and MB; Centrality [%]; Average Jets / 25 Samples";

  auto hJet = make_profile(h2Jet, "hJet", title);

  hJet->Draw("HIST");
  hJet->Draw("p e X0 same");

  hJet->GetYaxis()->SetMaxDigits(3);
  hJet->GetYaxis()->SetTitleOffset(1.f);
  hJet->GetXaxis()->SetTitleOffset(0.85f);

  double exponent = std::ceil(std::log10(hJet->GetMaximum()));
  double upper_bound = std::pow(10.0, exponent);

  hJet->GetYaxis()->SetRangeUser(5e-1, upper_bound);
  hJet->SetLineWidth(3);
  hJet->SetLineColor(kBlue);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "hJet").c_str());

  c1->Print((output + "]").c_str(), "pdf portrait");

  // -------------------------------------------

  c1->SetCanvasSize(1400, 1000);
  c1->SetLeftMargin(.16f);
  c1->SetRightMargin(.13f);
  c1->SetTopMargin(.08f);
  c1->SetBottomMargin(.12f);

  output = std::format("{}/plots-2D.pdf", m_output_dir);

  c1->Print((output + "[").c_str(), "pdf portrait");

  // -------------------------------------------
  // Reference Flow
  // -------------------------------------------

  gPad->SetLogy(0);

  auto* h2SP_res_2 = m_hists["h2SP_res_prof_2"].get();

  h2SP_res_2->Draw("COLZ1");
  title = std::format("sEPD: Re(#LT Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}#GT)", 2);
  h2SP_res_2->SetTitle(title.c_str());
  h2SP_res_2->GetYaxis()->SetLabelSize(0.06f);
  h2SP_res_2->GetXaxis()->SetLabelSize(0.06f);
  h2SP_res_2->GetYaxis()->SetTitleSize(0.06f);
  h2SP_res_2->GetXaxis()->SetTitleSize(0.06f);
  h2SP_res_2->GetXaxis()->SetTitleOffset(0.9f);
  h2SP_res_2->GetYaxis()->SetTitleOffset(0.9f);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h2SP_res_prof_2").c_str());

  // -------------------------------------------

  gPad->SetLogy();
  c1->SetBottomMargin(.13f);

  auto* h3SP_res_2 = dynamic_cast<TH3*>(m_hists["h3SP_res_2"].get());

  h3SP_res_2->GetXaxis()->SetRange(3,3);
  h3SP_res_2->GetYaxis()->SetRange(16,16);

  auto* h3SP_res_example = h3SP_res_2->Project3D("z");

  h3SP_res_example->Draw();
  h3SP_res_example->SetLineColor(kBlue);
  h3SP_res_example->SetLineWidth(3);

  h3SP_res_example->SetTitle("Sample: 15, Centrality: 20-30%");
  h3SP_res_example->GetYaxis()->SetTitle("Events");

  title = std::format("Re(Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}})", 2);
  h3SP_res_example->GetXaxis()->SetTitle(title.c_str());
  h3SP_res_example->GetXaxis()->SetRangeUser(-0.015,0.02);

  h3SP_res_example->GetYaxis()->SetLabelSize(0.06f);
  h3SP_res_example->GetXaxis()->SetLabelSize(0.05f);
  h3SP_res_example->GetYaxis()->SetTitleSize(0.06f);
  h3SP_res_example->GetXaxis()->SetTitleSize(0.06f);
  h3SP_res_example->GetXaxis()->SetTitleOffset(0.9f);
  h3SP_res_example->GetYaxis()->SetTitleOffset(1.f);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, "h3SP_res_example").c_str());

  gPad->SetLogy(0);
  c1->SetBottomMargin(.12f);

  // -------------------------------------------

  std::vector<HistConfig> histInfo = {{40, 0, 4e-4},
                                      {40, -8e-5, 8e-5},
                                      {40, -8e-5, 8e-5}
                                     }; 
  for (size_t n_idx = 0; n_idx < m_harmonics.size(); ++n_idx)
  {
    int n = m_harmonics[n_idx];
    // North-South Correlation
    title = std::format("sEPD N-S Correlation; Centrality [%]; Re(#LT Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}#GT)", n);
    draw_sEPD_corr_res(c1.get(), n, histInfo[n_idx].bins, histInfo[n_idx].low, histInfo[n_idx].high, false, title, "Correlation", output);

    // Reference Flow
    title = std::format("Reference Flow; Centrality [%]; #sqrt{{Re(#LT Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}#GT) }}", n);
    draw_sEPD_corr_res(c1.get(), n, 40, 0, 0.02, true, title, "Correlation", output);
  }

  // -------------------------------------------
  // Event Plane Resolution
  // -------------------------------------------

  // -------------------------------------------
  //  Presentation by Rosi at WWND2022
  //  https://indico.cern.ch/event/1039540/contributions/4737615/attachments/2401701/4107451/RReedWWND2022v1.pdf
  //  Data points from Slide 24
  // -------------------------------------------

  // sPHENIX MC
  std::vector<double> evt_res_sPHENIX_MC = {0.34, 0.5, 0.57, 0.55, 0.5, 0.42, 0.32, 0.22};

  // STAR EPD
  // Centrality binning unevent thus requiring x,y points
  std::vector<std::pair<double, double>> evt_res_star = {std::make_pair(2.5, 0.29),
                                                         std::make_pair(7.5, 0.45),
                                                         std::make_pair(15, 0.52),
                                                         std::make_pair(25, 0.56),
                                                         std::make_pair(35, 0.53),
                                                         std::make_pair(45, 0.44),
                                                         std::make_pair(55, 0.33),
                                                         std::make_pair(65, 0.2),
                                                         std::make_pair(75, 0.06)
                                                        };

  std::unique_ptr<TGraph> graph_evt_res_star = std::make_unique<TGraph>(evt_res_star.size());

  for (size_t point = 0; point < evt_res_star.size(); ++point)
  {
    double x = evt_res_star[point].first;
    double y = evt_res_star[point].second;
    graph_evt_res_star->SetPoint(static_cast<int>(point), x, y);
  }

  std::string hist_name = "evt_res_sPHENIX_MC";
  m_hists[hist_name] = std::make_unique<TH1F>(hist_name.c_str(),"", m_bins_cent, m_cent_low, m_cent_high);
  auto* hSP_res_sPHENIX_MC = m_hists[hist_name].get();

  hist_name = std::format("hSP_evt_res_prof_{}", 2);
  auto* hSP_evt_res_prof = dynamic_cast<TProfile*>(m_hists[hist_name].get());

  title = std::format("Event Plane Resolution; Centrality [%]; #sqrt{{#LTRe(Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}) / (|Q^{{S}}_{{{0}}}||Q^{{N}}_{{{0}}}|)#GT}}", 2);
  hist_name = std::format("hSP_evt_res_sqrt_prof_{}", 2);
  m_hists[hist_name] = std::make_unique<TH1F>(hist_name.c_str(), title.c_str(), m_bins_cent, m_cent_low, m_cent_high);
  auto* hSP_sqrt = m_hists[hist_name].get();

  title = std::format("Event Plane Resolution; Centrality [%]; #sqrt{{2#LTRe(Q^{{S}}_{{{0}}} Q^{{N*}}_{{{0}}}) / (|Q^{{S}}_{{{0}}}||Q^{{N}}_{{{0}}}|)#GT}}", 2);
  auto* hSP_sqrtv2 = dynamic_cast<TH1*>(hSP_sqrt->Clone("hSP_sqrtv2"));
  hSP_sqrtv2->SetTitle(title.c_str());

  for (int bin_cent = 1; bin_cent <= m_bins_cent; ++bin_cent)
  {
    double val = hSP_evt_res_prof->GetBinContent(bin_cent);
    double err = hSP_evt_res_prof->GetBinError(bin_cent);
    hSP_res_sPHENIX_MC->SetBinContent(bin_cent, evt_res_sPHENIX_MC[static_cast<size_t>(bin_cent-1)]);
    if (val > 0)
    {
      val = std::sqrt(val);
      err /= 2 * val;

      double valv2 = std::sqrt(2) * val;
      double errv2 = std::sqrt(2) * err;

      hSP_sqrt->SetBinContent(bin_cent, val);
      hSP_sqrt->SetBinError(bin_cent, err);

      hSP_sqrtv2->SetBinContent(bin_cent, valv2);
      hSP_sqrtv2->SetBinError(bin_cent, errv2);
    }
  }

  hSP_sqrt->Draw("hist l p");
  hSP_sqrt->SetMarkerColor(kBlue);
  hSP_sqrt->SetLineColor(kBlue);
  hSP_sqrt->SetMarkerStyle(kFullDotLarge);
  hSP_sqrt->SetLineWidth(3);
  hSP_sqrt->GetYaxis()->SetRangeUser(0, 0.6);
  hSP_sqrt->GetYaxis()->SetTitleOffset(1.1f);
  hSP_sqrt->GetXaxis()->SetTitleOffset(1.f);

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, hist_name).c_str());

  hSP_sqrtv2->Draw("p e X0");
  hSP_sqrtv2->SetMarkerColor(kBlue);
  hSP_sqrtv2->SetLineColor(kBlue);
  hSP_sqrtv2->SetMarkerStyle(kFullDotLarge);
  hSP_sqrtv2->SetLineWidth(3);
  hSP_sqrtv2->GetYaxis()->SetRangeUser(0, 0.6);
  hSP_sqrtv2->GetYaxis()->SetTitleOffset(1.1f);
  hSP_sqrtv2->GetXaxis()->SetTitleOffset(1.f);

  hSP_sqrtv2->Draw("same hist l p");

  hSP_res_sPHENIX_MC->Draw("same l p");
  hSP_res_sPHENIX_MC->SetMarkerColor(kRed);
  hSP_res_sPHENIX_MC->SetLineColor(kRed);
  hSP_res_sPHENIX_MC->SetMarkerStyle(kFullDotLarge);
  hSP_res_sPHENIX_MC->SetLineWidth(3);

  graph_evt_res_star->Draw("same l p");
  graph_evt_res_star->SetMarkerColor(kRed);
  graph_evt_res_star->SetLineColor(kRed);
  graph_evt_res_star->SetMarkerStyle(kOpenCircle);
  graph_evt_res_star->SetLineWidth(2);

  xshift = -0.02;
  yshift = -0.5;

  leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .85 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.06f);
  leg->AddEntry(hSP_sqrtv2, "Run 3 Au+Au: 68144", "lp");
  leg->AddEntry(hSP_res_sPHENIX_MC, "Rosi WWND2022", "lp");
  leg->AddEntry(graph_evt_res_star.get(), "STAR EPD", "lp");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/{}-v2.png", m_output_dir, hist_name).c_str());

  // -------------------------------------------
  // SP: Compare regular with anti
  // -------------------------------------------

  auto* hSP_re_prof = m_hists["hSP_re_prof_2"].get();
  auto* hSP_re_anti_prof = m_hists["hSP_re_anti_prof_2"].get();

  hSP_re_prof->Draw("p e");
  hSP_re_prof->SetLineColor(kBlue);
  hSP_re_prof->SetMarkerColor(kBlue);
  hSP_re_prof->SetMarkerStyle(kFullDotLarge);
  hSP_re_prof->SetLineWidth(3);

  hSP_re_prof->GetYaxis()->SetLabelSize(0.05f);
  hSP_re_prof->GetYaxis()->SetTitleSize(0.05f);
  hSP_re_prof->GetXaxis()->SetLabelSize(0.06f);
  hSP_re_prof->GetXaxis()->SetTitleSize(0.06f);

  hSP_re_prof->GetXaxis()->SetTitleOffset(0.9f);
  hSP_re_prof->GetYaxis()->SetTitleOffset(1.5f);

  hSP_re_prof->GetYaxis()->SetRangeUser(0, 1e-2);

  hSP_re_anti_prof->Draw("same p e");
  hSP_re_anti_prof->SetLineColor(kRed);
  hSP_re_anti_prof->SetMarkerColor(kRed);
  hSP_re_anti_prof->SetMarkerStyle(kFullDotLarge);
  hSP_re_anti_prof->SetLineWidth(3);

  xshift = -0.02;
  yshift = -0.5;

  leg = std::make_unique<TLegend>(0.2 + xshift, .65 + yshift, 0.54 + xshift, .75 + yshift);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.05f);
  leg->AddEntry(hSP_re_prof, "Opposite-Side Correlation", "lpe");
  leg->AddEntry(hSP_re_anti_prof, "Same-Side Corrleation", "lpe");
  leg->Draw("same");

  c1->Print(output.c_str(), "pdf portrait");
  if (m_saveFig) c1->Print(std::format("{}/images/hSP_re_prof-overlay.png", m_output_dir).c_str());

  // -------------------------------------------
  // SP
  // -------------------------------------------

  gPad->SetLogz();

  std::unique_ptr<TLine> line = std::make_unique<TLine>(hCentrality->GetXaxis()->GetXmin(), 0, hCentrality->GetXaxis()->GetXmax(), 0);

  line->SetLineColor(kBlack);
  line->SetLineStyle(kDashed);

  std::vector<std::string> types = {"re", "im"};

  for(auto type : types)
  {
    for(auto n : m_harmonics)
    {
        hist_name = std::format("h2SP_{}_prof_{}", type, n);
        auto* h2SP_prof = dynamic_cast<TH2*>(m_hists[hist_name].get());
        std::string type_cap = (type == "re") ? "Re" : "Im";
        title = std::format("Scalar Product; Centrality [%]; {0}(#LT q_{{{1}}} Q^{{S|N*}}_{{{1}}}#GT)", type_cap, n);

        std::string hist2D_name = std::format("h2SP_{}_{}", type, n);
        std::string hist_prof_name = std::format("hSP_prof_{}", n);

        auto h2SP = make_TH2(h2SP_prof, h2Jet, hist2D_name, title, 75, -0.015, 0.015);
        auto* hSP_prof = h2SP->ProfileX(hist_prof_name.c_str(), 1, -1, "s");

        h2SP->Draw("COLZ1");
        hSP_prof->Draw("same p e X0");
        line->Draw("same");

        h2SP->GetZaxis()->SetMaxDigits(3);
        h2SP->GetYaxis()->SetTitleOffset(1.5f);
        h2SP->GetXaxis()->SetTitleOffset(1.f);

        // h2SP->SetMinimum(0);
        h2SP->SetMaximum(1e6);

        hSP_prof->SetMarkerColor(kRed);
        hSP_prof->SetLineColor(kRed);
        hSP_prof->SetMarkerStyle(kFullDotLarge);
        hSP_prof->SetLineWidth(3);

        c1->Print(output.c_str(), "pdf portrait");
        if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, hist2D_name).c_str());
    }
  }

  // -------------------------------------------
  // Vn
  // -------------------------------------------

  for (auto type : types)
  {
    for (auto n : m_harmonics)
    {
      hist_name = std::format("h2Vn_{}_{}", type, n);
      std::string hist_prof_name = std::format("{}_prof", hist_name);

      auto* h2Vn = dynamic_cast<TH2*>(m_hists[hist_name].get());
      std::string type_cap = (type == "re") ? "Re" : "Im";
      title = std::format("Jet v_{{{0}}}; Centrality [%]; {1}(v_{{{0}}})", n, type_cap);

      auto* h2Vn_prof = h2Vn->ProfileX(hist_prof_name.c_str(), 1, -1, "s");

      h2Vn->Draw("COLZ1");
      h2Vn_prof->Draw("same p e X0");
      line->Draw("same");

      h2Vn->SetTitle(title.c_str());
      h2Vn->GetZaxis()->SetMaxDigits(3);
      h2Vn->GetYaxis()->SetTitleOffset(1.f);
      h2Vn->GetXaxis()->SetTitleOffset(0.85f);
      h2Vn->GetYaxis()->SetLabelSize(0.06f);
      h2Vn->GetXaxis()->SetLabelSize(0.06f);
      h2Vn->GetYaxis()->SetTitleSize(0.06f);
      h2Vn->GetXaxis()->SetTitleSize(0.06f);

      h2Vn->SetMaximum(1e6);

      h2Vn_prof->SetMarkerColor(kRed);
      h2Vn_prof->SetLineColor(kRed);
      h2Vn_prof->SetMarkerStyle(kFullDotLarge);
      h2Vn_prof->SetLineWidth(3);

      c1->Print(output.c_str(), "pdf portrait");
      if (m_saveFig) c1->Print(std::format("{}/images/{}.png", m_output_dir, hist_name).c_str());
    }
  }

  c1->Print((output + "]").c_str(), "pdf portrait");
}

int main(int argc, const char* const argv[])
{
  gROOT->SetBatch(true);

  if (argc < 2 || argc > 3)
  {
    std::cout << "Usage: " << argv[0] << " <input_file> [output_directory]" << std::endl;
    return 1;
  }

  const std::string input_file = argv[1];
  std::string output_dir = (argc >= 3) ? argv[2] : ".";

  try
  {
    DisplayJetAna analysis(input_file, output_dir);
    analysis.run();
  }
  catch (const std::exception& e)
  {
    std::cout << std::format("An exception occurred: {}", e.what()) << std::endl;
    return 1;
  }

  std::cout << "Analysis complete." << std::endl;
  return 0;
}
