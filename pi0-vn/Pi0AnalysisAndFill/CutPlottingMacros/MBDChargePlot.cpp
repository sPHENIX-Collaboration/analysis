#include <iostream>
#include <vector>
#include <TChain.h>
#include <TH2F.h>
#include <TMath.h>
#include <TFile.h>

TChain* ReadData();

void MBDChargePlot() {
    // 1. Start the timer
    auto start = std::chrono::high_resolution_clock::now();

    // 2. Initialize and set branches
    TChain* chain = ReadData();
    float clus_E, clus_E2, clus_eta, clus_phi, clus_eta2, clus_phi2;
    float clus_chi, clus_chi2, pi0_mass, pi0_pt, MBD_charge;
    // Variables to store the min and max values for pT, Mass, and MBD
    float min_pt = FLT_MAX, max_pt = -FLT_MAX;
    float min_mass = FLT_MAX, max_mass = -FLT_MAX;
    float min_MBD = FLT_MAX, max_MBD = -FLT_MAX;
    
    chain->SetBranchAddress("clus_E", &clus_E);
    chain->SetBranchAddress("clus_E2", &clus_E2);
    chain->SetBranchAddress("clus_eta", &clus_eta);
    chain->SetBranchAddress("clus_phi", &clus_phi);
    chain->SetBranchAddress("clus_eta2", &clus_eta2);
    chain->SetBranchAddress("clus_phi2", &clus_phi2);
    chain->SetBranchAddress("clus_chi", &clus_chi);
    chain->SetBranchAddress("clus_chi2", &clus_chi2);
    chain->SetBranchAddress("pi0_mass", &pi0_mass);
    chain->SetBranchAddress("pi0_pt", &pi0_pt);
    chain->SetBranchAddress("totalMBD", &MBD_charge);

    // Define the 3D histogram
    TH3D *h3MassPtMBD = new TH3D("h3MassPtMBD", "Invariant Mass vs. p_{T} vs. MBD Charge; p_{T}; Invariant Mass; MBD Charge",
                                  50, 2, 5,   // pT bins
                                  50, 0, 2,   // Mass bins
                                  50, 0, 2000);  // MBD


    // Adjust the label offset for each axis
    h3MassPtMBD->GetXaxis()->SetLabelOffset(0.03); // default is usually 0.005
    h3MassPtMBD->GetYaxis()->SetLabelOffset(0.01);
    h3MassPtMBD->GetZaxis()->SetLabelOffset(0.01);

    // Adjust the label size for each axis
    h3MassPtMBD->GetXaxis()->SetLabelSize(0.03);  // default is usually 0.02
    h3MassPtMBD->GetYaxis()->SetLabelSize(0.03);
    h3MassPtMBD->GetZaxis()->SetLabelSize(0.03);
    
    // Adjust the title offset for clarity
    h3MassPtMBD->GetXaxis()->SetTitleOffset(1.5);  // default is usually 1.0
    h3MassPtMBD->GetYaxis()->SetTitleOffset(2.0);
    h3MassPtMBD->GetZaxis()->SetTitleOffset(1.5);

    
    // 4. Loop over entries and apply conditions
    Long64_t nentries = chain->GetEntries();
    for(Long64_t i = 0; i < nentries; i++) {
        chain->GetEntry(i);

        float asymmetry = fabs(clus_E - clus_E2) / (clus_E + clus_E2);
        float delta_eta = clus_eta - clus_eta2;
        float delta_phi = clus_phi - clus_phi2;
        float delta_R = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

        // Filtering conditions
        if (delta_R < 0.06 || asymmetry > 0.5 || clus_chi > 4 || clus_chi2 > 4) {
            continue;  // Skip the current iteration if any of the conditions are not met
        }

        h3MassPtMBD->Fill(pi0_pt, pi0_mass, MBD_charge);
        // Update the min and max values
        if (pi0_pt < min_pt) min_pt = pi0_pt;
        if (pi0_pt > max_pt) max_pt = pi0_pt;
        if (pi0_mass < min_mass) min_mass = pi0_mass;
        if (pi0_mass > max_mass) max_mass = pi0_mass;
        if (MBD_charge < min_MBD) min_MBD = MBD_charge;
        if (MBD_charge > max_MBD) max_MBD = MBD_charge;
    }
    // Display the calculated ranges after the loop
    std::cout << "pT Range: [" << min_pt << ", " << max_pt << "]" << std::endl;
    std::cout << "Mass Range: [" << min_mass << ", " << max_mass << "]" << std::endl;
    std::cout << "MBD Range: [" << min_MBD << ", " << max_MBD << "]" << std::endl;
    // Set the color palette
    gStyle->SetPalette(kRainBow);
    // Display pt vs mass histogram
    TCanvas *canvas = new TCanvas("canvas", "Pi0 Mass vs. pT vs MBD Charge", 900, 600);
    h3MassPtMBD->Draw("COLZ");
    // Modify the viewing angle for clarity
    canvas->SetTheta(30);
    canvas->SetPhi(-30);
    

    h3MassPtMBD->SetStats(1);
    // Draw the 3D histogram
    h3MassPtMBD->Draw("BOX2");  // Use BOX2 for enhanced visualization

    canvas->Modified();
    canvas->Update();
    canvas->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/CutsAnalysisPlots/3Dcorrelations/MassVsPtVsMBD.png");

    /*
     Project for pT range 2 to 3
    */
//    int binLow2to3 = h3MassPtMBD->GetXaxis()->FindBin(2.0);
//    int binHigh2to3 = h3MassPtMBD->GetXaxis()->FindBin(3.0) - 1;

    TH2D *h2Projection2to3 = (TH2D*)h3MassPtMBD->Project3D("yx")->Clone("Projection2to3");
    h2Projection2to3->GetXaxis()->SetRange(2, 3);
    h2Projection2to3->SetTitle(("pT vs Mass for 2 < p_{T} < 3, z axis showing MBD"));
    h2Projection2to3->GetXaxis()->SetTitle("pT");
    h2Projection2to3->GetYaxis()->SetTitle("pi0 Mass");
    h2Projection2to3->SetStats(0);
//
//    h2Projection2to3->RebinX(2);
//    h2Projection2to3->RebinY(2);


    TCanvas* canvas2to3 = new TCanvas("canvas2to3", "Mass vs MBD pt between 2 and 3", 900, 600);
    canvas2to3->SetRightMargin(0.15); // Adjust the right margin for z-axis
    canvas2to3->SetLogz();
    h2Projection2to3->GetXaxis()->SetTitleOffset(1.2); // Shift the x-axis title up
    h2Projection2to3->Draw("COLZ");
    
    canvas2to3->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/CutsAnalysisPlots/2Dcorrelations/MassVsMBD_2to3.png");

    /*
     Project for pT range 3 to 4
    */
    int binLow3to4 = h3MassPtMBD->GetXaxis()->FindBin(2.0);
    int binHigh3to4 = h3MassPtMBD->GetXaxis()->FindBin(10.0) - 1;

    TH2D *h2Projection2to10 = (TH2D*)h3MassPtMBD->Project3D("yx")->Clone("Projection2to10");
    h2Projection2to10->GetXaxis()->SetRange(binLow3to4, binHigh3to4);
    h2Projection2to10->SetTitle(("pT vs Mass for 3 < p_{T} < 4, z axis showing MBD"));
    h2Projection2to10->GetXaxis()->SetTitle("pT");
    h2Projection2to10->GetYaxis()->SetTitle("pi0 Mass");
    h2Projection2to10->SetStats(0);

    TCanvas* canvas3to4 = new TCanvas("canvas3to4", "Mass vs MBD pt between 3 and 4", 900, 600);
    canvas3to4->SetRightMargin(0.15); // Adjust the right margin for z-axis
    h2Projection2to10->GetXaxis()->SetTitleOffset(1.2); // Shift the x-axis title up
    h2Projection2to10->Draw("COLZ");
    canvas3to4->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/CutsAnalysisPlots/2Dcorrelations/MassVsMBD_3to4.png");

    /*
     Project for pT range 4 to 5
    */
    int binLow4to5 = h3MassPtMBD->GetXaxis()->FindBin(4.0);
    int binHigh4to5 = h3MassPtMBD->GetXaxis()->FindBin(5.0) - 1;

    TH2D *h2Projection4to5 = (TH2D*)h3MassPtMBD->Project3D("xy")->Clone("Projection4to5");
    h2Projection4to5->GetXaxis()->SetRange(binLow4to5, binHigh4to5);
    h2Projection4to5->SetTitle(("pT vs pi0 Mass for 4 < p_{T} < 5 (Z axis MBD)"));
    h2Projection4to5->GetXaxis()->SetTitle("pT");
    h2Projection4to5->GetYaxis()->SetTitle("pi0 Mass");
    h2Projection4to5->SetStats(0);

    TCanvas* canvas4to5 = new TCanvas("canvas4to5", "Mass vs MBD pt between 4 and 5", 900, 600);
    canvas4to5->SetRightMargin(0.15); // Adjust the right margin for z-axis
    h2Projection4to5->GetXaxis()->SetTitleOffset(1.2); // Shift the x-axis title up
    h2Projection4to5->Draw("COLZ");
    canvas4to5->SaveAs("/Users/patsfan753/Desktop/Pi0_Analysis/CutsAnalysisPlots/2Dcorrelations/MassVsMBD_4to5.png");
    // Extract mean, median, and mode values
       double meanX = h3MassPtMBD->GetMean(1);
       double meanY = h3MassPtMBD->GetMean(2);
       double meanZ = h3MassPtMBD->GetMean(3);
    
       int binX_mode = h3MassPtMBD->ProjectionX()->GetMaximumBin();
       int binY_mode = h3MassPtMBD->ProjectionY()->GetMaximumBin();
       int binZ_mode = h3MassPtMBD->ProjectionZ()->GetMaximumBin();

       double modeX = h3MassPtMBD->GetXaxis()->GetBinCenter(binX_mode);
       double modeY = h3MassPtMBD->GetYaxis()->GetBinCenter(binY_mode);
       double modeZ = h3MassPtMBD->GetZaxis()->GetBinCenter(binZ_mode);

       // Output the results
       std::cout << "Means:\n";
       std::cout << "pT (X-axis): " << meanX << "\n";
       std::cout << "Invariant Mass (Y-axis): " << meanY << "\n";
       std::cout << "MBD Charge (Z-axis): " << meanZ << "\n";


       std::cout << "\nModes:\n";
       std::cout << "pT (X-axis): " << modeX << "\n";
       std::cout << "Invariant Mass (Y-axis): " << modeY << "\n";
       std::cout << "MBD Charge (Z-axis): " << modeZ << "\n";
    
    
    // Clean up and stop the timer
    delete h3MassPtMBD;
    delete canvas;

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
}


TChain* ReadData() {
    std::cout << "\033[1;34mLoading data...\033[0m" << std::endl;
    TChain *chain = new TChain("T");
    
    // To include other runs, uncomment the desired run numbers from the list below:
    // "22951", "21520", "21598", "21599", "21615", "21796", "21813", "21889", "21891",
    // "22949", "22950", "22979", "22982"
    const std::vector<std::string> runs = { "21518", "21889", "21891", "22949", "22950", "22951", "22979", "22982" };
    std::string baseDir = "/Users/patsfan753/Desktop/Pi0_Analysis/ntp_rootFiles_10_20/";
    for (const auto& run : runs) {
        chain->Add((baseDir + run + "/ntp.root").c_str());
    }
    std::cout << "\033[1;32mData loaded.\033[0m" << std::endl;
    return chain;
}


