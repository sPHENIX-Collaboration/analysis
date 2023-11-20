#include <TFile.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <vector>
#include <string>

void CaloE_MBD_correlation() {
  std::vector<std::string> runNumbers = {"21518", "21520", "21598", "21599", "21615", "21796", "21813", "21889", "21891", "22949", "22950", "22951", "22979", "22982"};

  for (const auto& run : runNumbers) {
    std::string filePath = "/Users/patsfan753/Desktop/QA_EMCal/UpdatedROOT_10_7/" + run + "/qa.root";
    TFile *file = TFile::Open(filePath.c_str());
    if (!file || file->IsZombie()) continue;
    TH2F *hist = (TH2F *)file->Get("h2TotalMBDCaloE");
    if (!hist) continue;

    TCanvas *c1 = new TCanvas("c1", "CaloE vs MBD", 8000, 6000);
    c1->SetFrameLineWidth(3);
    c1->SetHighLightColor(0);
    gStyle->SetPalette(kRainBow);
    gStyle->SetOptStat(0);
    hist->GetXaxis()->SetTitle("Total EMCal Energy [Arb]");
    hist->GetYaxis()->SetTitle("Total MBD Charge [Arb]");
    hist->SetTitle(Form("CaloE vs MBD (Run: %s)", run.c_str()));
    hist->Draw("COLZ");
    c1->SetLogz();
    hist->GetZaxis()->SetLabelColor(1);
    hist->GetZaxis()->SetTitle("Density");
    hist->GetZaxis()->SetTitleOffset(1.3);

    TPaveText *pt = new TPaveText(0.15,0.85,0.35,0.85,"brNDC");
    pt->AddText("sPHENIX EMCal QA");
    pt->SetFillColor(0);
    pt->SetBorderSize(0);
    pt->SetTextFont(62);
    pt->SetTextSize(0.04);
    pt->Draw();

    std::string outFilePath = "/Users/patsfan753/Desktop/QA_EMCal/MBD_TotalCaloE_OutputCorrelation/CaloE_MBD_Correlation_Elevated_" + run + ".png";
    c1->SaveAs(outFilePath.c_str(), "PNG");

    delete pt;
    delete c1;
    file->Close();
    delete file;
  }
}
