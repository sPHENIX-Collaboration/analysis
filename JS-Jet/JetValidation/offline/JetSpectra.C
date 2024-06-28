#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include "utilities.h"

void JetSpectra()
{
  SetsPhenixStyle();
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();

  TChain *ct = new TChain("T");
  ct->Add("../macro/output_sim1cutSUB.root");

  vector<float> *pt = nullptr; // Declare a pointer to a vector of floats

  ct->SetBranchAddress("pt", &pt); // Assign the branch to the vector<float>*

  // Define the binning for the histogram (Replace these values with your desired bins)
  int pt_N = 8;
  float pt_min = 10.0;
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
  /*
  TPaveStats *ptstats = new TPaveStats(0.78,0.775,0.98,0.935,"brNDC");
  ptstats->SetName("stats");
  ptstats->SetBorderSize(1);
  ptstats->SetFillColor(0);
  ptstats->SetTextAlign(12);
  ptstats->SetTextFont(42);
  ptstats->SetOptStat(1111);
  ptstats->SetOptFit(0);
  ptstats->Draw();
  pt_hist->GetListOfFunctions()->Add(ptstats);
  ptstats->SetParent(pt_hist);
  */
  
  for (int bin = 0; bin <= pt_hist->GetNbinsX(); ++bin) {
    double binContent = pt_hist->GetBinContent(bin);
    double binWidth = pt_hist->GetBinWidth(bin);
    double binErrorLow = pt_hist->GetBinErrorLow(bin);
    double binErrorUp = pt_hist->GetBinErrorUp(bin);
    pt_hist->SetBinContent(bin, binContent / binWidth);
    cout << "Bin " << bin << ": Content = " << binContent << ", Width = " << binWidth << ", Low Bin Error:" << binErrorLow << ", Upper Bin Error" << binErrorUp << endl;

    for (int i = 1; i <= pt_hist->GetNbinsX(); ++i) {
      if (pt_hist->GetBinContent(i) == 0) {
        pt_hist->SetBinContent(i, 0.0); // Set a small non-zero value
      }
    }

    //    cout << "Bin " << bin << ": Content = " << binContent << ", Width = " << binWidth << endl;
  }
  for (int binj = 0; binj <= pt_hist->GetNbinsY(); ++binj) {
    double binContentj = pt_hist->GetBinContent(binj);
    double binWidthj = pt_hist->GetBinWidth(binj);
    double binErrorLowj = pt_hist->GetBinErrorLow(binj);
    double binErrorUpj = pt_hist->GetBinErrorUp(binj);

    // pt_hist->SetBinContent(bin, binContent / binWidth);
    cout << "Bin " << binj << ": Content = " << binContentj << ", Width = " << binWidthj << ", Low Bin Error:" << binErrorLowj << ", Upper Bin Error" << binErrorUpj << endl;
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
  //  pt_hist->SetMinimum(-0.01);
  pt_hist->SetAxisRange(-0.5, 0.1, "Y");
  pt_hist->Scale(1.0/pt_hist->Integral("width"));
  pt_hist->Draw("E0");

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

  drawText("Au+Au #sqrt{s} = 200 GeV Pythia+HIJING", 0.6, 0.87, 14);
  drawText("anti-k_{T} R_{jet}=0.4, |#eta_{jet}| < 0.6", 0.6, 0.84, 14);
  // drawText("R_{sj}=0.1", 0.25, 0.81, 14);
  // drawText("10 < p_{T} < 20 [GeV/c]", 0.25, 0.775, 14);
  drawText("#it{#bf{sPHENIX}} Internal", 0.6, 0.805, 14);

  canvas->Modified();
  canvas->cd();
  canvas->SetSelected(canvas);
  canvas->SaveAs("JetSpectra_sim1SUB.pdf");
  canvas->Draw();
}

