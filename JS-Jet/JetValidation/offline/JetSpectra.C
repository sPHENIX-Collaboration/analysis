#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

void JetSpectra()
{
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  TChain *ct = new TChain("T");
  ct->Add("../macro/output.root");

  vector<float> *pt = nullptr; // Declare a pointer to a vector of floats

  ct->SetBranchAddress("pt", &pt); // Assign the branch to the vector<float>*

  // Define the binning for the histogram (Replace these values with your desired bins)
  int pt_N = 8;
  float pt_min = 15.0;
  float pt_max = 50.0;

  TH1F *pt_hist = new TH1F("h_MC_Reso", "", pt_N, pt_min, pt_max);

  Long64_t nEntries = ct->GetEntries();

  for (Long64_t iEntry = 0; iEntry < nEntries; ++iEntry) {
    ct->GetEntry(iEntry);

    // Loop over the vector<float> and fill the histogram
    for (size_t i = 0; i < pt->size(); ++i) {
      pt_hist->Fill((*pt)[i]); // Fill the histogram with each element of the vector
    }
  }

  // TCanvas *canvas = new TCanvas("canvas", "Histogram Canvas", 800, 600);
  TCanvas *canvas = new TCanvas("canvas", "canvas",10,54,700,500);
  canvas->Range(41,-4.051882,51,0.1891844);
  canvas->SetFillColor(0);
  canvas->SetBorderMode(0);
  canvas->SetBorderSize(2);
  canvas->SetLogy();
  canvas->SetFrameBorderMode(0);
  canvas->SetFrameBorderMode(0);

  TPaveStats *ptstats = new TPaveStats(0.78,0.775,0.98,0.935,"brNDC");
  ptstats->SetName("stats");
  ptstats->SetBorderSize(1);
  ptstats->SetFillColor(0);
  ptstats->SetTextAlign(12);
  ptstats->SetTextFont(42);
  TText *ptstats_LaTex = ptstats->AddText("pt_hist");
  ptstats_LaTex->SetTextSize(0.0368);
  ptstats_LaTex = ptstats->AddText("Entries = 23023  ");
  ptstats_LaTex = ptstats->AddText("Mean  =  44.05");
  ptstats_LaTex = ptstats->AddText("s_{NN}=200 GeV");
  ptstats_LaTex = ptstats->AddText("-0.3 < #eta < 0.7");
  ptstats->SetOptStat(1111);
  ptstats->SetOptFit(0);
  ptstats->Draw();
  pt_hist->GetListOfFunctions()->Add(ptstats);
  ptstats->SetParent(pt_hist);

  // Normalize the x-axis by dividig each bin content by its width                                                                                                          
  for (int biny = 1; biny <= pt_hist->GetNbinsY(); ++biny) {
    double binContenty = pt_hist->GetBinContent(biny);
    double binWidthy = pt_hist->GetBinWidth(biny);
    pt_hist->SetBinContent(biny, binContenty / binWidthy);
  }
  // Normalize the x-axis by dividing each bin content by its width                                                                                                          
  for (int bin = 1; bin <= pt_hist->GetNbinsX(); ++bin) {
    double binContent = pt_hist->GetBinContent(bin);
    double binWidth = pt_hist->GetBinWidth(bin);
    pt_hist->SetBinContent(bin, binContent / binWidth);
  }


  Int_t ci;      // for color index setting                                                                                                                                  
  TColor *color; // for color definition with alpha                                                                                                                          
  ci = TColor::GetColor("#000099");
  pt_hist->SetLineColor(ci);
  pt_hist->GetXaxis()->SetTitle("p_{T}");
  pt_hist->GetXaxis()->SetLabelFont(42);
  pt_hist->GetXaxis()->SetTitleOffset(1);
  pt_hist->GetXaxis()->SetTitleFont(42);
  pt_hist->GetYaxis()->SetTitle("N Jets (arbitrary units)");
  pt_hist->GetYaxis()->SetLabelFont(42);
  pt_hist->GetYaxis()->SetTitleFont(42);
  pt_hist->GetZaxis()->SetLabelFont(42);
  pt_hist->GetZaxis()->SetTitleOffset(1);
  pt_hist->GetZaxis()->SetTitleFont(42);
  pt_hist->Scale(1.0/pt_hist->Integral("width"));
  pt_hist->Draw("");

  TPaveLabel *label = new TPaveLabel(0.2, 0.5, 0.5, 0.6, "s_{NN}=200GeV");
  label->SetTextSize(0.04); // Adjust the label's text size                                                                                                                  
  label->SetTextFont(42);   // Set the font for the label text                                                                                                               
  label->SetFillColor(0);
  label->Draw();

  TPaveText *pt_textt = new TPaveText(0.4742407,0.9366491,0.5257593,0.995,"blNDC");
  pt_textt->SetName("title");
  pt_textt->SetBorderSize(0);
  pt_textt->SetFillColor(0);
  pt_textt->SetFillStyle(0);

  pt_textt->SetTextFont(42);
  TText *pt_LaTex = pt_textt->AddText("Jet Spectra R=0.4 [Simulation]");
  pt_LaTex->SetTextSize(0.04); // Adjust the size of the title text                                                                                                          
  pt_LaTex->SetTextFont(42); // Set the font for the title                                                                                                                   
  pt_textt->Draw();
  canvas->Modified();
  canvas->cd();
  canvas->SetSelected(canvas);
  canvas->SaveAs("JetSpectra.pdf");
  canvas->Draw();
}

