#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

#include <TFile.h>
#include <TKey.h>
#include <TH1.h>
#include <TDirectory.h>
#include <TCanvas.h>
#include <iostream>
#include <sstream>
#include <TSystem.h>

void DiphotonQA() {
    gROOT->SetBatch(kTRUE);
    gSystem->Load("sPhenixStyle.C");
    SetsPhenixStyle();

    // Prompt for user input at the start of the function
    std::string userPath;
    std::cout << "Enter the path for the data files: ";
    std::getline(std::cin, userPath);

    // Construct the full paths using the userPath
    std::string rootFilePath = userPath + "/p015/test.root";
    std::string outputBasePath = userPath + "/p015/QA/Diphoton/";

    TFile* file = TFile::Open(rootFilePath.c_str());
    if (!file || file->IsZombie()) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    TDirectory* qaDir = file->GetDirectory("QA");
    if (!qaDir) {
        std::cerr << "QA directory not found." << std::endl;
        file->Close();
        delete file;
        return;
    }

    const char* folderNames[] = {"h2DeltaRVsMass", "h2AsymVsMass", "h2DeltaRVsAsym", "h2Pi0EtaPhi"};
    const int numFolders = sizeof(folderNames) / sizeof(folderNames[0]);
    
    // Define centrality and pT range labels
    const char* centralityLabels[] = {"40-60% Centrality", "20-40% Centrality", "0-20% Centrality"};
    const char* ptRangeLabels[] = {
        "2.0 #leq p_{T} < 2.5 GeV",
        "2.5 #leq p_{T} < 3.0 GeV",
        "3.0 #leq p_{T} < 3.5 GeV",
        "3.5 #leq p_{T} < 4.0 GeV",
        "4.0 #leq p_{T} < 4.5 GeV",
        "4.5 #leq p_{T} < 5.0 GeV"
    };

    for (int i = 0; i < numFolders; ++i) {
        std::cout << "Processing folder: " << folderNames[i] << std::endl;
        TDirectory* currentFolder = qaDir->GetDirectory(folderNames[i]);
        if (!currentFolder) {
            continue; // Skip if directory not found
        }

        std::string outputPath = std::string(outputBasePath) + folderNames[i];
        std::cout << "Creating output directory: " << outputPath << std::endl;
        gSystem->Exec(("mkdir -p '" + outputPath + "'").c_str());

        for (int histIndex = 0; histIndex < 18; ++histIndex) {
            int centralityIndex = histIndex / 6;
             int ptRangeIndex = histIndex % 6;
            
            std::string histName = std::string(folderNames[i]) + "_" + std::to_string(histIndex);
            TH1* hist = dynamic_cast<TH1*>(currentFolder->Get(histName.c_str()));
            if (!hist) {
                continue; // Skip if histogram not found
            }
            hist->GetZaxis()->SetTitle("Counts"); // Set the Z-axis title
            hist->GetZaxis()->SetTitleOffset(1.5);
            hist->GetYaxis()->SetRangeUser(0, 1);
            std::string newTitle = hist->GetTitle();
            newTitle += " - ";
            newTitle += centralityLabels[centralityIndex];
            newTitle += " - ";
            newTitle += ptRangeLabels[ptRangeIndex];
            hist->SetTitle(newTitle.c_str());
            
            TCanvas* c = new TCanvas();
            c->SetRightMargin(0.2);
            std::string imageName = outputPath + "/" + histName + ".png";
            hist->Draw("colz1");
            TLegend *leg = new TLegend(0.16,.82,0.26,.92);
            leg->SetFillStyle(0);
            TLatex latex;
            latex.SetTextSize(0.044);
            latex.SetTextAlign(11); // Align left, bottom
            std::string combinedLabel = std::string(centralityLabels[centralityIndex]) + ", " + std::string(ptRangeLabels[ptRangeIndex]);
            latex.DrawLatexNDC(0.15, 0.065, combinedLabel.c_str());

        

            c->SaveAs(imageName.c_str());
            delete c; // Ensure the canvas is properly deleted

            // Check if file was saved
            if (gSystem->AccessPathName(imageName.c_str()) != 0) {
                std::cerr << "Failed to save: " << imageName << std::endl;
            } else {
                std::cout << "Saved: " << imageName << std::endl;
            }
        }
    }

    file->Close();
    std::cout << "Finished processing." << std::endl;
    delete file;
}
