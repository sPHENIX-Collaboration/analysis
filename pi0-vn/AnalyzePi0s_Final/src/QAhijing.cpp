#include "TFile.h"
#include "TKey.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

void QAhijing() {
    gROOT->LoadMacro("sPhenixStyle.C");
    SetsPhenixStyle();
    
    // Define input and output paths
    const char* inputFilePath = "/Users/patsfan753/Desktop/p015/MonteCarlo/QA/RootFiles/qa.root";
    const char* outputDir = "/Users/patsfan753/Desktop/p015/MonteCarlo/QA/ProductionQA/";

    // Open the ROOT file
    TFile *file = TFile::Open(inputFilePath, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // Ensure the output directory exists
    gSystem->MakeDirectory(outputDir);

    // Loop through all keys in the ROOT file
    TIter next(file->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)next())) {
        TObject *obj = key->ReadObj();
        if (obj->IsA()->InheritsFrom("TH1")) {
            TH1 *hist = (TH1*)obj;
            TCanvas *canvas = new TCanvas("canvas", "Canvas", 800, 600);

            // Set logarithmic y-axis for specified histograms
            TString histName = hist->GetName();
            if (histName == "hClusterPt" || histName == "hClusterECore" || histName == "hClusterChi" ||
                histName == "hBadPMTs" || histName == "hTotalCaloE" || histName == "hTotalMBD" || histName == "hTowE") {
                hist->Draw();
                canvas->SetLogy(1);  // Apply logarithmic scale on y-axis
            } else if (histName == "h2TotalMBDCaloE" && hist->IsA()->InheritsFrom("TH2")) {
                TH2* h2 = (TH2*)hist;
                h2->GetZaxis()->SetTitle("Counts");
                h2->SetAxisRange(0, 100000, "Z");
                h2->Draw("COLZ");
                canvas->SetRightMargin(0.15);
                canvas->SetLogz(1);  // Set logarithmic scale on z-axis
            } else if (histName == "h2TotalMBDCaloE" && hist->IsA()->InheritsFrom("TH2")) {
                TH2* h2 = (TH2*)hist;
                h2->GetZaxis()->SetTitle("Counts");
                h2->SetAxisRange(0, 100000, "Z");
                h2->Draw("COLZ");
                canvas->SetRightMargin(0.15);
                canvas->SetLogz(1);  // Set logarithmic scale on z-axis
            } else if (histName == "h2ClusterEtaPhiWeighted" && hist->IsA()->InheritsFrom("TH2")) {
                TH2* h2 = (TH2*)hist;
                h2->GetZaxis()->SetTitle("Cluster Energy [GeV]");
                h2->GetZaxis()->SetTitleOffset(1.4);
                h2->Draw("COLZ");
                canvas->SetRightMargin(0.2);
            } else if (histName == "h2TowEtaPhiWeighted" && hist->IsA()->InheritsFrom("TH2")) {
                TH2* h2 = (TH2*)hist;
                h2->GetZaxis()->SetTitle("Tower Energy [GeV]");
                h2->Draw("COLZ");
                canvas->SetRightMargin(0.15);
            } else if (hist->IsA()->InheritsFrom("TH2")) {
                ((TH2*)hist)->Draw("COLZ");
            } else if (hist->IsA()->InheritsFrom("TH3")) {
                ((TH3*)hist)->Draw("ISO");
            } else {
                hist->Draw();
            }

            // Save the canvas as a PNG file
            TString outputFileName = TString::Format("%s%s.png", outputDir, hist->GetName());
            canvas->SaveAs(outputFileName);
            delete canvas;  // Clean up to avoid memory leaks
        }
    }

    // Close the file
    file->Close();
    delete file;
}

