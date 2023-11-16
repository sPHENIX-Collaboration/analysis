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
void FillingMacro() {
    TStopwatch timer;
    timer.Start();

    TChain* chain = ReadData();
    float clus_E, clus_E2, clus_eta, clus_phi, clus_eta2, clus_phi2, clus_time, clus_time2;
    float clus_chi, clus_chi2, pi0_mass, pi0_pt, MBD_charge;
    const float cut_delta_R = 0.08;
    const float cut_asymmetry = 0.5;
    const float cut_clus_chi = 4;
    const float cut_clus_chi2 = 4;
    const float cut_clus_E = 1.0;

    
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

    
    // Declare an array of vectors to store unique MBD (Minimum Bias Detector) values
    // for each histogram index (0-11).
    std::vector<float> uniqueMBDValues[12];

    // Declare a set to quickly check and ensure that MBD values are unique across all histogram indices.
    std::set<float> seenMBDValues;
    
    TH1F *hPi0Mass[12];
    for (int i = 0; i < 12; ++i) {
        hPi0Mass[i] = new TH1F(Form("hPi0Mass_%d", i), "Reconstructed Diphoton;Invariant Mass [GeV];Counts", 80, 0.0, 1.0);
        hPi0Mass[i]->Sumw2();  // Enabling Sumw2 for error bars
    }
    
    // Define the histograms for Timing Analysis
    TH1F* hLeading_GoodPeak = new TH1F("hLeading_GoodPeak", "Leading Cluster Timing Distribution", 150, 0, 38);
    TH1F* hSubLeading_GoodPeak = new TH1F("hSubLeading_GoodPeak", "Subleading Cluster Timing Distribution", 150, 0, 38);
    TH1F* hLeading_BadPeak = new TH1F("hLeading_BadPeak", "Leading Cluster Timing Distribution", 150, 0, 38);
    TH1F* hSubLeading_BadPeak = new TH1F("hSubLeading_BadPeak", "Subleading Cluster Timing Distribution", 150, 0, 38);
    
    
    
    // Define the histograms
    TH1F* hPi0Pt_1 = new TH1F("hPi0Pt_1", "Diphoton p_{T} (0 <= MBD < 21395.5)", 150, 0, 10);
    TH1F* hPi0Pt_2 = new TH1F("hPi0Pt_2", "Diphoton p_{T} (21395.5 <= MBD < 53640.9)", 150, 0, 10);
    TH1F* hPi0Pt_3 = new TH1F("hPi0Pt_3", "Diphoton p_{T} (53640.9 <= MBD < 109768)", 150, 0, 10);
    TH1F* hPi0Pt_4 = new TH1F("hPi0Pt_4", "Diphoton p_{T} (109768 <= MBD < 250000)", 150, 0, 10);

    
    Long64_t nentries = chain->GetEntries();
    std::cout << "Total entries to process: " << nentries << std::endl;
    // Print information about cuts and histograms.
    std::cout << "Cuts (inclusive):\n"
              << " Delta R >= " << cut_delta_R << "\n"
              << " Asymmetry < " << cut_asymmetry << "\n"
              << " Energy Cut >= " << cut_clus_E << "\n"
              << " Cluster Chi < " << cut_clus_chi << "\n"
              << " Cluster Chi2 < " << cut_clus_chi2 << std::endl;
    
    for(Long64_t i = 0; i < nentries; i++) {
        chain->GetEntry(i);

        float asymmetry = fabs(clus_E - clus_E2) / (clus_E + clus_E2);
        float delta_eta = clus_eta - clus_eta2;
        float delta_phi = clus_phi - clus_phi2;
        delta_phi = fabs(delta_phi);
        if(delta_phi > M_PI) delta_phi = 2*M_PI-delta_phi;
        float delta_R = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);
  
        //add "|| MBD_charge > cut_MBD_charge" for MBD cut
        
        //add  for upper bound delta R cut
//        || delta_R > .15
        
        
        //Always want data that is KEPT to be----lower bound <= parameter < upper bound
        //so data that is skipped should be --- parameter >= upper bound / parameter < lower bound
        if (delta_R < cut_delta_R || asymmetry >= cut_asymmetry || clus_chi >= cut_clus_chi || clus_chi2 >= cut_clus_chi2 || clus_E < cut_clus_E || clus_E2 < cut_clus_E) {
            continue;  // Skip the current iteration if any of the conditions are not met
        }

        int index = -1;
        //Upper limit corresponding to 0.6 centrality
        if      (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 0.0 && MBD_charge < 21395.5)      index = 0;
        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 0.0 && MBD_charge < 21395.5)      index = 1;
        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 0.0 && MBD_charge < 21395.5)      index = 2;

        //upper limit corresponding to 0.4 centrality
        else if (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9)  index = 3;
        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9)  index = 4;
        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 21395.5 && MBD_charge < 53640.9)  index = 5;
        
        //upper limit corresponding to 0.2 centrality
        else if (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 53640.9 && MBD_charge < 109768)  index = 6;
        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 53640.9 && MBD_charge < 109768)  index = 7;
        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 53640.9 && MBD_charge < 109768)  index = 8;
        
        //upper limit corresponding to max MBD value
        else if (pi0_pt >= 2.0 && pi0_pt < 3.0 && MBD_charge >= 109768 && MBD_charge < 250000)  index = 9;
        else if (pi0_pt >= 3.0 && pi0_pt < 4.0 && MBD_charge >= 109768 && MBD_charge < 250000)  index = 10;
        else if (pi0_pt >= 4.0 && pi0_pt < 5.0 && MBD_charge >= 109768 && MBD_charge < 250000)  index = 11;
        // Check if 'index' has a valid value (i.e., not equal to -1).
        if (index != -1) {
            // Fill the histogram at the given index with the value of 'pi0_mass'.
            hPi0Mass[index]->Fill(pi0_mass);
            // Check if MBD_charge is unique for this histogram index
            if (seenMBDValues.find(MBD_charge) == seenMBDValues.end()) {
                // If unique, add 'MBD_charge' to the vector corresponding to the current histogram index.
                uniqueMBDValues[index].push_back(MBD_charge);
                // Also add 'MBD_charge' to the set of seen MBD values to facilitate quick lookup.
                seenMBDValues.insert(MBD_charge);
            }
        }
        if (pi0_pt >= 2.0 && pi0_pt < 3.0) {
            if (MBD_charge >= 0.0 && MBD_charge < 21395.5) {
                if (clus_E > clus_E2) {
                    hLeading_GoodPeak->Fill(clus_time);
                    hSubLeading_GoodPeak->Fill(clus_time2);
                } else {
                    hLeading_GoodPeak->Fill(clus_time2);
                    hSubLeading_GoodPeak->Fill(clus_time);
                }
            } else if (MBD_charge >= 109768 && MBD_charge < 250000) {
                if (clus_E > clus_E2) {
                     hLeading_BadPeak->Fill(clus_time);
                     hSubLeading_BadPeak->Fill(clus_time2);
                } else {
                     hLeading_BadPeak->Fill(clus_time2);
                     hSubLeading_BadPeak->Fill(clus_time);
                }
            }
        }
        
        if(pi0_mass >= 0 && pi0_mass < 1) {
            if (0 <= MBD_charge && MBD_charge < 21395.5) hPi0Pt_1->Fill(pi0_pt);
            else if (21395.5 <= MBD_charge && MBD_charge < 53640.9) hPi0Pt_2->Fill(pi0_pt);
            else if (53640.9 <= MBD_charge && MBD_charge < 109768) hPi0Pt_3->Fill(pi0_pt);
            else if (109768 <= MBD_charge && MBD_charge < 250000) hPi0Pt_4->Fill(pi0_pt);
        }
    }


    std::cout << "hLeading_GoodPeak Entries: " << hLeading_GoodPeak->GetEntries() << std::endl;
    std::cout << "hSubLeading_GoodPeak Entries: " << hSubLeading_GoodPeak->GetEntries() << std::endl;
    std::cout << "hLeading_BadPeak Entries: " << hLeading_BadPeak->GetEntries() << std::endl;
    std::cout << "hSubLeading_BadPeak Entries: " << hSubLeading_BadPeak->GetEntries() << std::endl;

    TFile outFile("/Users/patsfan753/Desktop/Pi0_Organized/HistOutput1GeVallRuns.root", "RECREATE");
    /*
     Write Histograms to root file for Invariant Mass Distributions
     */
    for (int i = 0; i < 12; ++i) {
        std::cout << "Writing histogram for index: " << i << std::endl; // Print the histogram index being written
        hPi0Mass[i]->Write();
    }
    /*
     Write histograms to root files for Timing Cuts
     */
    hLeading_GoodPeak->Write();
    hSubLeading_GoodPeak->Write();
    hLeading_BadPeak->Write();
    hSubLeading_BadPeak->Write();
    /*
     Write Histograms to root file for pT distributions over varied MBD ranges
     */
    hPi0Pt_1->Write();
    hPi0Pt_2->Write();
    hPi0Pt_3->Write();
    hPi0Pt_4->Write();
    outFile.Close();
    
    // Writing unique MBD counts to a text file
    std::ofstream outFileText("/Users/patsfan753/Desktop/Pi0_Analysis/UniqueMBDCounts.txt");
    for (int i = 0; i < 12; ++i) {
        outFileText << i << "\t" << uniqueMBDValues[i].size() << std::endl;
    }
    outFileText.close();
    
    
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
