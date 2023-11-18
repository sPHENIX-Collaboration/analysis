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

void FillHists() {
    TStopwatch timer;
    timer.Start();
    TChain* chain = ReadData();
    float clus_E, clus_E2, clus_eta, clus_phi, clus_eta2, clus_phi2, clus_time, clus_time2;
    float clus_chi, clus_chi2, pi0_mass, pi0_pt, MBD_charge;
    // Define arrays for cut values
    std::vector<float> delta_R_values = {.07,.075,.08,.085,.09,.095,.1};
    std::vector<float> asymmetry_values = {.3, .4, .5, .6, .7};
    std::vector<float> clus_chi_values = {3, 3.5, 4, 4.5, 5};      
    std::vector<float> clus_E_values = {1.0, 1.25, 1.5};    

    // Iterate over each combination of cuts
    for (float cut_delta_R : delta_R_values) {
        for (float cut_asymmetry : asymmetry_values) {
            for (float cut_clus_chi : clus_chi_values) {
                for (float cut_clus_E : clus_E_values)  {
                    // Function to format the float values for the file name
                    auto formatFloatForFilename = [](float value) -> std::string {
                        std::ostringstream ss;
                        // Increase the precision to handle more decimal places accurately
                        ss << std::fixed << std::setprecision(3) << value;
                        std::string str = ss.str();
                        size_t dotPos = str.find('.');
                        if (dotPos != std::string::npos) {
                            // Replace '.' with "point"
                            str = str.substr(0, dotPos) + "point" + str.substr(dotPos + 1);
                        }
                        // Remove trailing zeros and 'point' for whole numbers
                        if (value == static_cast<int>(value)) {
                            size_t pointPos = str.find("point");
                            if (pointPos != std::string::npos) {
                                str.erase(pointPos);
                            }
                        } else {
                            // Remove trailing zeros for decimal values
                            str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                        }
                        return str;
                    };


                    
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

                    TH1F *hPi0Mass[12];
                    for (int i = 0; i < 12; ++i) {
                        hPi0Mass[i] = new TH1F(Form("hPi0Mass_%d", i), "Reconstructed Diphoton;Invariant Mass [GeV];Counts", 80, 0.0, 1.0);
                        hPi0Mass[i]->Sumw2();  // Enabling Sumw2 for error bars
                    }

                    
                    Long64_t nentries = chain->GetEntries();
                    std::cout << "Total entries to process: " << nentries << std::endl;
                    // Print information about cuts and histograms.
                    std::cout << "Cuts (inclusive):\n"
                              << " Delta R >= " << cut_delta_R << "\n"
                              << " Asymmetry < " << cut_asymmetry << "\n"
                              << " Energy Cut >= " << cut_clus_E << "\n"
                              << " Cluster Chi < " << cut_clus_chi << "\n";
                    
                    for(Long64_t i = 0; i < nentries; i++) {
                        chain->GetEntry(i);

                        float asymmetry = fabs(clus_E - clus_E2) / (clus_E + clus_E2);
                        float delta_eta = clus_eta - clus_eta2;
                        float delta_phi = clus_phi - clus_phi2;
                        delta_phi = fabs(delta_phi);
                        if(delta_phi > M_PI) delta_phi = 2*M_PI-delta_phi;
                        float delta_R = TMath::Sqrt(delta_eta * delta_eta + delta_phi * delta_phi);
                  
                        //Always want data that is KEPT to be----lower bound <= parameter < upper boundF
                        //so data that is skipped should be --- parameter >= upper bound / parameter < lower bound
                        if (delta_R < cut_delta_R || asymmetry >= cut_asymmetry || clus_chi >= cut_clus_chi || clus_chi2 >= cut_clus_chi || clus_E < cut_clus_E || clus_E2 < cut_clus_E) {
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
                        }
                    }
                    // Constructing the file name
                    std::string filename = "/Users/patsfan753/Desktop/AnalyzePi0s_Final/histRootFiles/hPi0Mass_E"
                                           + formatFloatForFilename(cut_clus_E) + "_Asym"
                                           + formatFloatForFilename(cut_asymmetry) + "_Delr"
                                           + formatFloatForFilename(cut_delta_R) + "_Chi"
                                           + formatFloatForFilename(cut_clus_chi) + ".root";
                    std::cout << "Processing for file: " << filename << std::endl;
                    TFile outFile(filename.c_str(), "RECREATE");
                    for (int i = 0; i < 12; ++i) {
                        std::cout << "Writing histogram for index: " << i << std::endl; // Print the histogram index being written
                        hPi0Mass[i]->Write();
                        std::cout << "Finished writing histogram " << i << std::endl;
                    }
                    outFile.Close();
                    std::cout << "Completed processing for file: " << filename << std::endl;
                }
            }
        }
    }
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
