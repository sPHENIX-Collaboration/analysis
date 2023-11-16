#include <iostream>
#include <vector>
#include <TChain.h>
#include <TH1F.h>
#include <TMath.h>
#include <TFile.h>
#include <TStopwatch.h>
#include <map>
#include <vector>

TChain* ReadData();
void ComboFill() {
    TStopwatch timer;
    timer.Start();

    TChain* chain = ReadData();
    float clus_E, clus_E2, clus_eta, clus_phi, clus_eta2, clus_phi2, clus_time, clus_time2;
    float clus_chi, clus_chi2, pi0_mass, pi0_pt, MBD_charge;
    // Define the new vectors for cut values
    std::vector<float> energyCuts = {1, 1.2, 1.4, 1.6, 1.8, 2.0};
    

    std::vector<float> asymmetryCuts = {.4, .5, .6, .7, .8};

    std::vector<float> deltaRCuts = {.05, .06, .07, .08, .09, .1};
    std::vector<float> chiCuts = {2, 3, 4};
    
    
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
    chain->SetBranchAddress("clus_time", &clus_time);
    chain->SetBranchAddress("clus_time2", &clus_time2);
    
    // Open a single ROOT file to store all histograms
    std::string outFileName = "/Users/patsfan753/Desktop/Pi0_Organized/RootHistOutput/HistOutputCutCombos.root";
    TFile outFile(outFileName.c_str(), "RECREATE");
    
    // Nested loops for each cut type
    for (float cut_clus_E : energyCuts) {
        for (float cut_asymmetry : asymmetryCuts) {
            for (float cut_delta_R : deltaRCuts) {
                for (float cut_clus_chi : chiCuts) {
                    float cut_clus_chi2 = cut_clus_chi; // Assuming cut_clus_chi2 is the same as cut_clus_chi

                    TH1F *hPi0Mass[12];
                    for (int i = 0; i < 12; ++i) {
                        std::string histName = Form("hPi0Mass_E%.1f_Asym%.1f_DelR%.2f_Chi%.1f_%d",
                                                    cut_clus_E, cut_asymmetry, cut_delta_R, cut_clus_chi, i);
                        hPi0Mass[i] = new TH1F(histName.c_str(), "Reconstructed Diphoton;Invariant Mass [GeV];Counts", 80, 0.0, 1.0);
                        hPi0Mass[i]->Sumw2();
                    }
                    Long64_t nentries = chain->GetEntries();
                    std::cout << "Total entries to process: " << nentries << std::endl;

                    
                    for(Long64_t i = 0; i < nentries; i++) {
                        chain->GetEntry(i);

                        float asymmetry = fabs(clus_E - clus_E2) / (clus_E + clus_E2);
                        
                        float delta_eta = clus_eta - clus_eta2;
                        float delta_phi = clus_phi - clus_phi2;
                        if(delta_phi > M_PI) delta_phi = 2*M_PI-delta_phi;
                        float delta_R = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);
                        if (delta_R < cut_delta_R || asymmetry >= cut_asymmetry || clus_chi >= cut_clus_chi || clus_chi2 >= cut_clus_chi2 || clus_E < cut_clus_E || clus_E2 < cut_clus_E) {
                            continue;  // Skip the current iteration if any of the conditions are not met
                        }
                        int index = -1;
                        //Upper limit corresponding to Peripheral
                        if      (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 0.0 && MBD_charge < 21395.5)      index = 0;
                        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 0.0 && MBD_charge < 21395.5)      index = 1;
                        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 0.0 && MBD_charge < 21395.5)      index = 2;

                        //upper limit corresponding LowMid
                        else if (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9)  index = 3;
                        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9)  index = 4;
                        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9)  index = 5;
                        
                        //upper limit corresponding HighMid
                        else if (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 53640.9 && MBD_charge < 109768)  index = 6;
                        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 53640.9 && MBD_charge < 109768)  index = 7;
                        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 53640.9 && MBD_charge < 109768)  index = 8;
                        
                        //upper limit corresponding to Central
                        else if (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 109768 && MBD_charge < 250000)  index = 9;
                        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 109768 && MBD_charge < 250000)  index = 10;
                        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 109768 && MBD_charge < 250000)  index = 11;
                        // Check if 'index' has a valid value (i.e., not equal to -1).
                        if (index != -1) {
                            // Fill the histogram at the given index with the value of 'pi0_mass'.
                            hPi0Mass[index]->Fill(pi0_mass);
                        }
                    }
                    for (int i = 0; i < 12; ++i) {
                        hPi0Mass[i]->Write();
                        delete hPi0Mass[i]; // Free memory
                    }
                }
            }
        }
    }
    // Close the file after writing all histograms
    outFile.Close();
    // Stop the timer and print out the total runtime.
    timer.Stop();
    std::cout << "Total real time: " << timer.RealTime() << " seconds\n";
    std::cout << "Total CPU time: " << timer.CpuTime() << " seconds\n";
}
TChain* ReadData() {
    std::cout << "\033[1;34mLoading data...\033[0m" << std::endl;
    TChain *chain = new TChain("T");
    
    /*
     BAD QA RUNS INCLUDE: "21598", "21599", "21615",
     */

    const std::vector<std::string> runs = { "21518", "21520", "21598", "21599", "21615", "21796", "21813", "21889", "21891", "22949", "22950", "22951", "22979", "22982" };
    std::string baseDir = "/Users/patsfan753/Desktop/Pi0_Organized/ntp_rootFiles_11_6/";
    for (const auto& run : runs) {
        std::cout << "Adding run: " << run << std::endl; // Print the run being loaded
        chain->Add((baseDir + run + "/ntp.root").c_str());
    }
    std::cout << "\033[1;32mData loaded.\033[0m" << std::endl;
    return chain;
}

