#include <TFile.h>
#include <TProfile2D.h>
#include <TH2D.h>
#include <iostream>

void ExtractRMS(const char* inputFile, const char* outputFile) {
    // Open the input ROOT file
    TFile* inFile = TFile::Open(inputFile, "READ");
    if (!inFile || inFile->IsZombie()) {
        std::cerr << "Error: Cannot open input file " << inputFile << std::endl;
        return;
    }

    // Retrieve the TProfile2D histograms
    TProfile2D* h_2D_emcal = dynamic_cast<TProfile2D*>(inFile->Get("h_2D_emcal"));
    TProfile2D* h_2D_ihcal = dynamic_cast<TProfile2D*>(inFile->Get("h_2D_ihcal"));
    TProfile2D* h_2D_ohcal = dynamic_cast<TProfile2D*>(inFile->Get("h_2D_ohcal"));

    if (!h_2D_emcal || !h_2D_ihcal || !h_2D_ohcal) {
        std::cerr << "Error: One or more TProfile2D histograms not found in the input file." << std::endl;
        inFile->Close();
        return;
    }

    std::cout << "Create new TH2D histograms to store the RMS (bin errors)" << std::endl;
    TH2D* h_2D_emcal_rms = new TH2D("h_2D_emcal_rms", "RMS of h_2D_emcal;X;Y", 
                                   h_2D_emcal->GetNbinsX(), h_2D_emcal->GetXaxis()->GetXbins()->GetArray(),
                                   h_2D_emcal->GetNbinsY(), h_2D_emcal->GetYaxis()->GetXbins()->GetArray());

    TH2D* h_2D_ihcal_rms = new TH2D("h_2D_ihcal_rms", "RMS of h_2D_ihcal;X;Y", 
                                   h_2D_ihcal->GetNbinsX(), h_2D_ihcal->GetXaxis()->GetXbins()->GetArray(),
                                   h_2D_ihcal->GetNbinsY(), h_2D_ihcal->GetYaxis()->GetXbins()->GetArray());

    TH2D* h_2D_ohcal_rms = new TH2D("h_2D_ohcal_rms", "RMS of h_2D_ohcal;X;Y", 
                                   h_2D_ohcal->GetNbinsX(), h_2D_ohcal->GetXaxis()->GetXbins()->GetArray(),
                                   h_2D_ohcal->GetNbinsY(), h_2D_ohcal->GetYaxis()->GetXbins()->GetArray());

    TH1F* h_emcal_rms = new TH1F("h_emcal_rms","",1000,0,0.1);
    TH1F* h_ihcal_rms = new TH1F("h_ihcal_rms","",1000,0,0.01);
    TH1F* h_ohcal_rms = new TH1F("h_ohcal_rms","",1000,0,0.05);

    std::cout << "Fill the TH2D histograms with bin errors from the TProfile2D histograms" << std::endl;
    for (int x = 1; x <= h_2D_emcal->GetNbinsX(); ++x) {
        for (int y = 1; y <= h_2D_emcal->GetNbinsY(); ++y) {
            h_2D_emcal_rms->SetBinContent(x, y, h_2D_emcal->GetBinError(x, y));
            h_emcal_rms->Fill(h_2D_emcal->GetBinError(x, y));
            if (x < 25 && y < 65) {
                h_2D_ihcal_rms->SetBinContent(x, y, h_2D_ihcal->GetBinError(x, y));
                h_2D_ohcal_rms->SetBinContent(x, y, h_2D_ohcal->GetBinError(x, y));
                h_ihcal_rms->Fill(h_2D_ihcal->GetBinError(x, y));
                h_ohcal_rms->Fill(h_2D_ohcal->GetBinError(x, y));
            }
        }
    }

    std::cout << "Open the output ROOT file" << std::endl;
    TFile* outFile = TFile::Open(outputFile, "RECREATE");
    if (!outFile || outFile->IsZombie()) {
        std::cerr << "Error: Cannot open output file " << outputFile << std::endl;
        inFile->Close();
        return;
    }

    std::cout << "Write the new histograms to the output file" << std::endl;
    h_2D_emcal_rms->Write();
    h_2D_ihcal_rms->Write();
    h_2D_ohcal_rms->Write();
    h_emcal_rms->Write();
    h_ihcal_rms->Write();
    h_ohcal_rms->Write();

    // Clean up
    outFile->Close();
    inFile->Close();

    std::cout << "Histograms successfully created and saved to " << outputFile << std::endl;
}
