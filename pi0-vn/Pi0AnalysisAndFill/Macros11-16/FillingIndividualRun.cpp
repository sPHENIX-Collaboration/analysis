#include <vector>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TMath.h>
#include <iostream>
#include <chrono>

void FillingIndividualRun() {
    auto start = std::chrono::high_resolution_clock::now(); // Start the timer
    // Define the base directory and run numbers
    std::string baseDir = "/Users/patsfan753/Desktop/Pi0_Analysis/ntp_rootFiles_11_6/";
    std::vector<std::string> runs = { "21518", "21520", "21598", "21599", "21615", "21796", "21813", "21889", "21891", "22949", "22950", "22951", "22979", "22982"};

    // Create an output file
    TFile* outFile = new TFile("/Users/patsfan753/Desktop/Pi0_Analysis/IndividualInvarSpect_output.root", "RECREATE");

    // Loop over each run
    for (const auto& run : runs) {
        std::cout << "Processing Run: " << run << std::endl;
        // Form the full path to the ntp.root file for the current run
        std::string filePath = baseDir + run + "/ntp.root";
        TFile* file = TFile::Open(filePath.c_str(), "READ");
        
        // Continue to the next run if the file does not exist or is not open
        if (!file || !file->IsOpen()) {
            std::cerr << "Error opening file: " << filePath << std::endl;
            continue;
        }

        // Get the TTree from the file
        TTree* tree = dynamic_cast<TTree*>(file->Get("T"));
        if (!tree) {
            std::cerr << "TTree not found in file: " << filePath << std::endl;
            file->Close();
            continue;
        }
        std::cout << "Creating histogram for Run: " << run << std::endl;
        // Variables to hold the data from the tree
        float clus_E, clus_E2, clus_eta, clus_phi, clus_eta2, clus_phi2, clus_chi, clus_chi2, pi0_mass, pi0_pt, MBD_charge;

        // Set the branch addresses
        tree->SetBranchAddress("clus_E", &clus_E);
        tree->SetBranchAddress("clus_E2", &clus_E2);
        tree->SetBranchAddress("clus_eta", &clus_eta);
        tree->SetBranchAddress("clus_phi", &clus_phi);
        tree->SetBranchAddress("clus_eta2", &clus_eta2);
        tree->SetBranchAddress("clus_phi2", &clus_phi2);
        tree->SetBranchAddress("clus_chi", &clus_chi);
        tree->SetBranchAddress("clus_chi2", &clus_chi2);
        tree->SetBranchAddress("pi0_mass", &pi0_mass);
        tree->SetBranchAddress("pi0_pt", &pi0_pt);
        tree->SetBranchAddress("totalMBD", &MBD_charge);

        // Create a histogram for this run
        TH1F* h = new TH1F(run.c_str(), Form("Reconstructed Diphoton, Run %s;Invariant Mass [GeV];Counts", run.c_str()), 100, 0, 1);

        // Constants for the cuts
        const float cut_delta_R = 0.08;
        const float cut_asymmetry = 0.5;
        const float cut_clus_chi = 4;
        const float cut_clus_chi2 = 4;

        // Loop over the entries in the tree
        Long64_t nEntries = tree->GetEntries();
        for (Long64_t i = 0; i < nEntries; ++i) {
            tree->GetEntry(i);

            // Calculate asymmetry and delta R
            float asymmetry = fabs(clus_E - clus_E2) / (clus_E + clus_E2);
            float delta_eta = clus_eta - clus_eta2;
            float delta_phi = clus_phi - clus_phi2;
            delta_phi = fabs(delta_phi);
            if (delta_phi > M_PI) delta_phi = 2*M_PI - delta_phi;
            float delta_R = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);

            if (delta_R < cut_delta_R || asymmetry >= cut_asymmetry || clus_chi >= cut_clus_chi || clus_chi2 >= cut_clus_chi2 || clus_E < 1.0 || clus_E2 < 1.0) {
                continue;  // Skip the current iteration if any of the conditions are not met
            }

            // Fill the histogram only if the pt and MBD charge are within the specified range
            if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9) {
                h->Fill(pi0_mass);
            }

        }
        std::cout << "Run " << run << " - Histogram Filled and Written to File" << std::endl;
        // Write the histogram to the output file
        outFile->cd();
        h->Write();
        file->Close();
//        delete h;
    }

    // Close the output file
    outFile->Close();
    delete outFile;
    auto finish = std::chrono::high_resolution_clock::now(); // End the timer
    std::chrono::duration<double> elapsed = finish - start; // Calculate elapsed time
    std::cout << "Total time taken: " << elapsed.count() << " s\n";
}
