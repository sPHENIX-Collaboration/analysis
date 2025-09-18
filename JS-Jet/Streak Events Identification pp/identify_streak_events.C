#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TLine.h>
#include <TText.h>
#include <TVector2.h>
#include <TSystem.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <utility>
#include <string>

#include <map>
#include <iomanip>  // For std::setw, std::setprecision


// Function definitions OUTSIDE the main function
float calculateClusterTime(Float_t time_array[49], Float_t energy_array[49]) {
    float weighted_time = 0;
    float total_energy = 0;
    
    for (int i = 0; i < 49; ++i) {
        if (energy_array[i] > 0.1) {  // Minimum energy threshold
            weighted_time += time_array[i] * energy_array[i];
            total_energy += energy_array[i];
        }
    }
    
    return (total_energy > 0) ? weighted_time / total_energy : -999;
}

float calculateTimeSpread(Float_t time_array[49], Float_t energy_array[49]) {
    float avg_time = calculateClusterTime(time_array, energy_array);
    if (avg_time == -999) return -999;
    
    float weighted_variance = 0;
    float total_energy = 0;
    
    for (int i = 0; i < 49; ++i) {
        if (energy_array[i] > 0.1) {
            float time_diff = time_array[i] - avg_time;
            weighted_variance += time_diff * time_diff * energy_array[i];
            total_energy += energy_array[i];
        }
    }
    
    return (total_energy > 0) ? TMath::Sqrt(weighted_variance / total_energy) : -999;
}

void identify_streak_events(const char* filename = "/sphenix/user/shuhangli/ppg12/anatreemaker/macro_maketree/data/ana468/condorout/caloana0316.root", 
                                     int maxEvents = -1,
                                     const char* outputPrefix = "streak_analysis") {
    
    //  the png output directory
    std::string png_output_dir = "/sphenix/user/muhammadibrahim/sphenix_work_2025/streak_events/new/emcal/png_output/";
    
    // Create directory if it doesn't exist
    gSystem->mkdir(png_output_dir.c_str(), kTRUE);
    
    // Set ROOT style for better plots
    gStyle->SetOptStat(1111);
    gStyle->SetPalette(1);
    
    TFile* f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    TTree* tree = (TTree*)f->Get("slimtree");
    if (!tree) {
        std::cerr << "Tree 'slimtree' not found in file." << std::endl;
        f->Close();
        return;
    }

    const int MAX = 1000;

    // Variables
    int runnumber = 0;
    int eventnumber = 0;
    int ncluster = 0;
    int njet = 0;

    // Cluster arrays
    Float_t cluster_E[MAX], cluster_Eta[MAX], cluster_Phi[MAX], cluster_weta[MAX];
    Float_t cluster_Et[MAX], cluster_weta_cogx[MAX]; 
    Float_t cluster_time_array[MAX][49];

    // Jet arrays
    Float_t jet_Pt[MAX], jet_Phi[MAX], jet_E[MAX];

    // Cluster grid array
    Float_t cluster_e_array[MAX][49];

    // Set branch addresses
    tree->SetBranchAddress("eventnumber", &eventnumber);
    tree->SetBranchAddress("runnumber", &runnumber);
    tree->SetBranchAddress("ncluster_CLUSTERINFO_CEMC_NO_SPLIT", &ncluster);
    tree->SetBranchAddress("cluster_E_CLUSTERINFO_CEMC_NO_SPLIT", cluster_E);
    tree->SetBranchAddress("cluster_Et_CLUSTERINFO_CEMC_NO_SPLIT", cluster_Et);
    tree->SetBranchAddress("cluster_Eta_CLUSTERINFO_CEMC_NO_SPLIT", cluster_Eta);
    tree->SetBranchAddress("cluster_Phi_CLUSTERINFO_CEMC_NO_SPLIT", cluster_Phi);
    tree->SetBranchAddress("cluster_weta_CLUSTERINFO_CEMC_NO_SPLIT", cluster_weta);
    tree->SetBranchAddress("cluster_weta_cogx_CLUSTERINFO_CEMC_NO_SPLIT", cluster_weta_cogx);

    // Check for timing and energy arrays existence
    bool has_time_array = tree->GetBranch("cluster_time_array_CLUSTERINFO_CEMC_NO_SPLIT") != nullptr;
    bool has_e_array = tree->GetBranch("cluster_e_array_CLUSTERINFO_CEMC_NO_SPLIT") != nullptr;
    
    if (has_time_array) {
        tree->SetBranchAddress("cluster_time_array_CLUSTERINFO_CEMC_NO_SPLIT", cluster_time_array);
    }

    if (has_e_array) {
        tree->SetBranchAddress("cluster_e_array_CLUSTERINFO_CEMC_NO_SPLIT", cluster_e_array);
    }

    // Jet branches
    tree->SetBranchAddress("njet", &njet);
    tree->SetBranchAddress("jet_Pt", jet_Pt);
    tree->SetBranchAddress("jet_Phi", jet_Phi);
    tree->SetBranchAddress("jet_E", jet_E);

    // Standard histograms
    TH2F* h_eta_phi_all = new TH2F("h_eta_phi_all", "Eta vs Phi (All Events);#phi;#eta", 64, -3.2, 3.2, 64, -1.1, 1.1);
    TH2F* h_eta_phi_streak = new TH2F("h_eta_phi_streak", "Eta vs Phi (Streak Events);#phi;#eta", 64, -3.2, 3.2, 64, -1.1, 1.1);
    
    TH2F* h_phi_Et_all = new TH2F("h_phi_E_{T}_all", "Cluster E_{T} vs Phi (All);#phi;E_{T} [GeV]", 64, -3.2, 3.2, 100, 0, 50);
    TH2F* h_phi_Et_streak = new TH2F("h_phi_E_{T}_streak", "Cluster E_{T} vs Phi (Streak);#phi;E_{T} [GeV]", 64, -3.2, 3.2, 100, 0, 50);
    
    TH1F* h_weta_all = new TH1F("h_weta_all", "Cluster w_{#eta} (All);w_{#eta};Counts", 100, 0, 3.0);
    TH1F* h_weta_streak = new TH1F("h_weta_streak", "Cluster w_{#eta} (Streak);w_{#eta};Counts", 100, 0, 3.0);
    
    TH1F* h_weta_cogx_all = new TH1F("h_weta_cogx_all", "Cluster w_{#eta} COG_{x} (All);w_{#eta} COG_{x};Counts", 100, 0, 2.0);
    TH1F* h_weta_cogx_streak = new TH1F("h_weta_cogx_streak", "Cluster w_{#eta} COG_{x} (Streak);w_{#eta} COG_{x};Counts", 100, 0, 2.0);
    
    TH1F* h_cluster_Et_all = new TH1F("h_cluster_Et_all", "Cluster E_{T} (All);E_{T} [GeV];Counts", 100, 0, 100);
    TH1F* h_cluster_Et_streak = new TH1F("h_cluster_Et_streak", "Cluster E_{T} (Streak);E_{T} [GeV];Counts", 100, 0, 100);
    
    TH1F* h_asymmetry = new TH1F("h_asymmetry", "Hemisphere Energy Asymmetry;E_{diff};Counts", 100, -1.0, 1.0);
    TH1F* h_jet_dphi = new TH1F("h_jet_dphi", "#Delta#phi between leading jets;#Delta#phi;Counts", 64, 0, TMath::Pi());
    TH1F* h_jet_asym = new TH1F("h_jet_asym", "Jet energy hemisphere asymmetry;Asymmetry;Counts", 100, -1, 1);
    
    //TH2F* h_cluster_grid = new TH2F("h_cluster_grid", "Highest-E Cluster Tower Map;#phi (tower);#eta (tower)", 7, -3, 4, 7, -3, 4);

    // Energy bin histograms - ALL events
    TH1F* h_cluster_Et_10_20_all = new TH1F("h_cluster_Et_10_20_all", "Cluster E_{T} (10-20 GeV, All);E_{T} [GeV];Entries", 20, 10, 20);
    TH1F* h_cluster_Et_20_25_all = new TH1F("h_cluster_Et_20_25_all", "Cluster E_{T} (20-25 GeV, All);E_{T} [GeV];Entries", 10, 20, 25);
    TH1F* h_cluster_Et_25_30_all = new TH1F("h_cluster_Et_25_30_all", "Cluster E_{T} (25-30 GeV, All);E_{T} [GeV];Entries", 10, 25, 30);
    TH1F* h_cluster_Et_gt30_all = new TH1F("h_cluster_Et_gt30_all", "Cluster E_{T} (>30 GeV, All);E_{T} [GeV];Entries", 20, 30, 100);

    // Energy bin histograms - events
    TH1F* h_cluster_Et_10_20_streak = new TH1F("h_cluster_Et_10_20_streak", "Cluster E_{T} (10-20 GeV, Streak);E_{T} [GeV];Entries", 20, 10, 20);
    TH1F* h_cluster_Et_20_25_streak = new TH1F("h_cluster_Et_20_25_streak", "Cluster E_{T} (20-25 GeV, Streak);E_{T} [GeV];Entries", 10, 20, 25);
    TH1F* h_cluster_Et_25_30_streak = new TH1F("h_cluster_Et_25_30_streak", "Cluster E_{T} (25-30 GeV, Streak);E_{T} [GeV];Entries", 10, 25, 30);
    TH1F* h_cluster_Et_gt30_streak = new TH1F("h_cluster_Et_gt30_streak", "Cluster E_{T} (>30 GeV, Streak);E_{T} [GeV];Entries", 20, 30, 100);

    // 2D Jet Analysis Histograms
    TH2F* h_asym_vs_leadingjet = new TH2F("h_asym_vs_leadingjet", "Jet Asymmetry vs Leading Jet p_{T};Leading Jet p_{T} [GeV];Asymmetry", 100, 0, 100, 100, -1, 1);
    TH2F* h_jet_pt_vs_phi = new TH2F("h_jet_pt_vs_phi", "Jet p_{T} vs #phi;#phi;p_{T} [GeV]", 64, -TMath::Pi(), TMath::Pi(), 100, 0, 100);
    TH2F* h_jet_multiplicity_vs_asym = new TH2F("h_jet_multiplicity_vs_asym", "Jet Multiplicity vs Asymmetry;N_{jets};Asymmetry", 20, 0, 20, 100, -1, 1);
    TH2F* h_dphi_vs_pt_ratio = new TH2F("h_dphi_vs_pt_ratio", "#Delta#phi vs p_{T} Ratio;#Delta#phi;p_{T,lead}/p_{T,sublead}", 64, 0, TMath::Pi(), 100, 0, 10);

    // Timing histograms
    TH1F* h_cluster_time_all = new TH1F("h_cluster_time_all", "Cluster Timing (All);Time [ns];Counts", 100, -50, 50);
    TH1F* h_cluster_time_streak = new TH1F("h_cluster_time_streak", "Cluster Timing (Streak);Time [ns];Counts", 100, -50, 50);

    // Timing spread histograms
    TH1F* h_time_spread_all = new TH1F("h_time_spread_all", "Cluster Time Spread (All);Time RMS [ns];Counts", 100, 0, 20);
    TH1F* h_time_spread_streak = new TH1F("h_time_spread_streak", "Cluster Time Spread (Streak);Time RMS [ns];Counts", 100, 0, 20);

    // 2D timing analysis
    TH2F* h_time_vs_e = new TH2F("h_time_vs_E", "Cluster Time vs E;E [GeV];Time [ns]", 100, 0, 100, 100, -50, 50);
    TH2F* h_time_spread_vs_e = new TH2F("h_time_spread_vs_E", "Time Spread vs E;E [GeV];Time RMS [ns]", 100, 0, 100, 100, 0, 20);
    TH2F* h_time_vs_weta = new TH2F("h_time_vs_weta", "Cluster Time vs w_{#eta};w_{#eta};Time [ns]", 100, 0, 3.0, 100, -50, 50);

    std::vector<std::pair<int, int>> streakEvents;

    // Maps to store run-based statistics
    std::map<int, int> run_total_events;
    std::map<int, int> run_streak_events;



   Long64_t nentries = tree->GetEntries();
   Long64_t max_events;
   //Long64_t max_events = std::min(nentries, (Long64_t)maxEvents);

    if (maxEvents == -1) {
        max_events = nentries;
        std::cout << "Processing ALL " << nentries << " events..." << std::endl;
    } else {
        max_events = std::min(nentries, (Long64_t)maxEvents);
        std::cout << "Processing " << max_events << " out of " << nentries << " events..." << std::endl;
    }
    

    std::cout << "Opening file: " << filename << std::endl;
    std::cout << "TTree '" << tree->GetName() << "' loaded with " << tree->GetEntries() << " entries." << std::endl;
    std::cout << "Starting streak event analysis on first " << max_events << " events..." << std::endl;

    int streakCount = 0;
    
    for (Long64_t ievt = 0; ievt < max_events; ++ievt) {
        tree->GetEntry(ievt);
       
        // Count total events per run
        run_total_events[runnumber]++;
        
        if (ievt % 10000 == 0) {
            std::cout << "Processing event: " << ievt << " / " << max_events << std::endl;
        }
        
        bool cluster_condition_met = false;
        float e_plus = 0, e_minus = 0;
        int max_idx = -1;
        float max_Et = -1;

        // Fill energy bins for ALL events
        for (int i = 0; i < ncluster && i < MAX; ++i) {
            float e = cluster_Et[i];
            if (e >= 10 && e < 20)
                h_cluster_Et_10_20_all->Fill(e);
            else if (e >= 20 && e < 25)
                h_cluster_Et_20_25_all->Fill(e);
            else if (e >= 25 && e < 30)
                h_cluster_Et_25_30_all->Fill(e);
            else if (e >= 30)
                h_cluster_Et_gt30_all->Fill(e);
        }

        // Loop over clusters
        for (int i = 0; i < ncluster && i < MAX; ++i) {
            if (cluster_Et[i] < 0.5) continue;
            
            //  the timing variables for each cluster
            float cluster_time = -999;
            float time_spread = -999;
            
            if (has_time_array && has_e_array) {
                cluster_time = calculateClusterTime(cluster_time_array[i], cluster_e_array[i]);
                time_spread = calculateTimeSpread(cluster_time_array[i], cluster_e_array[i]);
            }

            // Fill general histograms
            h_eta_phi_all->Fill(cluster_Phi[i], cluster_Eta[i]);
            h_phi_Et_all->Fill(cluster_Phi[i], cluster_Et[i]);
            h_weta_all->Fill(cluster_weta[i]);
            h_weta_cogx_all->Fill(cluster_weta_cogx[i]);
            h_cluster_Et_all->Fill(cluster_Et[i]);

            // Fill timing histograms
            if (cluster_time != -999) {
                h_cluster_time_all->Fill(cluster_time);
                h_time_vs_e->Fill(cluster_Et[i], cluster_time);
                h_time_vs_weta->Fill(cluster_weta[i], cluster_time);
            }
            
            if (time_spread != -999) {
                h_time_spread_all->Fill(time_spread);
                h_time_spread_vs_e->Fill(cluster_Et[i], time_spread);
            }

            // Hemisphere energy calculation
            if (TMath::Cos(cluster_Phi[i]) > 0)
                e_plus += cluster_Et[i];
            else
                e_minus += cluster_Et[i];

            // Timing Criteria 
            bool timing_cut = (cluster_time != -999) ? TMath::Abs(cluster_time) < 10.0 : true;
            bool timing_spread_cut = (time_spread != -999) ? time_spread > 5.0 : false;
        
            // Check cluster conditions for streak identification
            if (cluster_weta[i] > 0.5 && cluster_Et[i] >= 10.0) {
                cluster_condition_met = true;
            }

            // Find highest energy cluster
            if (cluster_Et[i] > max_Et) {
                max_Et = cluster_Et[i];
                max_idx = i;
            }
        }

        // Calculate hemisphere asymmetry
        float e_asym = (e_plus - e_minus) / (e_plus + e_minus + 1e-3);
        h_asymmetry->Fill(e_asym);

        // Jet analysis
        bool has_back_to_back_jets = false;
        float leading_jet_pt = 0;
        float subleading_jet_pt = 0;
        int leading_jet_idx = -1;
        int subleading_jet_idx = -1;

        // Find leading and subleading jets
        for (int j = 0; j < njet && j < MAX; ++j) {
            if (jet_Pt[j] > leading_jet_pt) {
                subleading_jet_pt = leading_jet_pt;
                subleading_jet_idx = leading_jet_idx;
                leading_jet_pt = jet_Pt[j];
                leading_jet_idx = j;
            } else if (jet_Pt[j] > subleading_jet_pt) {
                subleading_jet_pt = jet_Pt[j];
                subleading_jet_idx = j;
            }
        }

        // Fill jet 2D histograms    
        for (int j = 0; j < njet && j < MAX; ++j) {
            h_jet_pt_vs_phi->Fill(jet_Phi[j], jet_Pt[j]);
        }

        // Check for back-to-back jets
        for (int j = 0; j < njet && j < MAX; ++j) {
            if (jet_Pt[j] <= 10) continue;
            
            for (int k = j + 1; k < njet && k < MAX; ++k) {
                if (jet_Pt[k] <= 10) continue;
                
                float dphi = TMath::Abs(TMath::ACos(TMath::Cos(jet_Phi[j] - jet_Phi[k])));
                h_jet_dphi->Fill(dphi);
                
                // Fill 2D dphi vs pt ratio plot
                if (jet_Pt[k] > 0) {
                    float pt_ratio = jet_Pt[j] / jet_Pt[k];
                    h_dphi_vs_pt_ratio->Fill(dphi, pt_ratio);
                }
                
                if (dphi > TMath::Pi() / 2.0) {
                    has_back_to_back_jets = true;
                    break;
                }
            }
            if (has_back_to_back_jets) break;
        }

        // Calculating jet hemisphere asymmetry
        float jet_e_plus = 0, jet_e_minus = 0;
        for (int j = 0; j < njet; ++j) {
            if (cos(jet_Phi[j]) > 0)
                jet_e_plus += jet_E[j];
            else
                jet_e_minus += jet_E[j];
        }
        float jet_asym = (jet_e_plus - jet_e_minus) / (jet_e_plus + jet_e_minus + 1e-3);
        h_jet_asym->Fill(jet_asym);

        // Filling the 2D jet analysis histograms
        h_asym_vs_leadingjet->Fill(leading_jet_pt, jet_asym);
        h_jet_multiplicity_vs_asym->Fill(njet, jet_asym);

        // Streak event identification
        if (cluster_condition_met && !has_back_to_back_jets) {
            streakEvents.emplace_back(runnumber, eventnumber);
            run_streak_events[runnumber]++;  // To Count streak events per run
            streakCount++;

            // Fill streak-specific histograms
            for (int i = 0; i < ncluster && i < MAX; ++i) {
                if (cluster_Et[i] >= 0.5) {
                    h_eta_phi_streak->Fill(cluster_Phi[i], cluster_Eta[i]);
                    h_phi_Et_streak->Fill(cluster_Phi[i], cluster_Et[i]);
                    h_weta_streak->Fill(cluster_weta[i]);
                    h_weta_cogx_streak->Fill(cluster_weta_cogx[i]);
                    h_cluster_Et_streak->Fill(cluster_Et[i]);

                    // Fill timing histograms for streak events
                    if (has_time_array && has_e_array) {
                        float cluster_time = calculateClusterTime(cluster_time_array[i], cluster_e_array[i]);
                        float time_spread = calculateTimeSpread(cluster_time_array[i], cluster_e_array[i]);
                        
                        if (cluster_time != -999) {
                            h_cluster_time_streak->Fill(cluster_time);
                        }
                        
                        if (time_spread != -999) {
                            h_time_spread_streak->Fill(time_spread);
                        }
                    }
                }
            }

            // Filling energy bins for STREAK events
            for (int i = 0; i < ncluster; ++i) {
                float et = cluster_Et[i];
                if (et >= 10 && et < 20)
                    h_cluster_Et_10_20_streak->Fill(et);
                else if (et >= 20 && et < 25)
                    h_cluster_Et_20_25_streak->Fill(et);
                else if (et >= 25 && et < 30)
                    h_cluster_Et_25_30_streak->Fill(et);
                else if (et >= 30)
                    h_cluster_Et_gt30_streak->Fill(et);
            }
        }
    }

    // Write enhanced streak event list with run statistics
    std::ofstream fout_events(std::string(outputPrefix) + "_event_list.txt");
    fout_events << "# Streak Events List with Run Statistics\n";
    fout_events << "# RunNumber EventNumber TotalEventsInRun\n";
    fout_events << "# Format: Run Event Total\n";

    for (const auto& ev : streakEvents) {
        int run = ev.first;
        int event = ev.second;
        int total_in_run = run_total_events[run];
        fout_events << run << " " << event << " " << total_in_run << "\n";
    }
    fout_events.close();


    // Create output ROOT file
    std::string rootFileName = std::string(outputPrefix) + "_results.root";
    TFile* outFile = new TFile(rootFileName.c_str(), "RECREATE");
    
    // Write all histograms
    h_eta_phi_all->Write();
    h_eta_phi_streak->Write();
    h_phi_Et_all->Write();
    h_phi_Et_streak->Write();
    h_weta_all->Write();
    h_weta_streak->Write();
    h_weta_cogx_all->Write();
    h_weta_cogx_streak->Write();
    h_cluster_Et_all->Write();
    h_cluster_Et_streak->Write();
    h_asymmetry->Write();
    h_jet_dphi->Write();
    h_jet_asym->Write();
    //h_cluster_grid->Write();
    
    // Write energy bin histograms
    h_cluster_Et_10_20_all->Write();
    h_cluster_Et_20_25_all->Write();
    h_cluster_Et_25_30_all->Write();
    h_cluster_Et_gt30_all->Write();
    h_cluster_Et_10_20_streak->Write();
    h_cluster_Et_20_25_streak->Write();
    h_cluster_Et_25_30_streak->Write();
    h_cluster_Et_gt30_streak->Write();

    // Write 2D jet histograms
    h_asym_vs_leadingjet->Write();
    h_jet_pt_vs_phi->Write();
    h_jet_multiplicity_vs_asym->Write();
    h_dphi_vs_pt_ratio->Write();

    // Write timing histograms
    h_cluster_time_all->Write();
    h_cluster_time_streak->Write();
    h_time_spread_all->Write();
    h_time_spread_streak->Write();
    h_time_vs_e->Write();
    h_time_spread_vs_e->Write();
    h_time_vs_weta->Write();

    outFile->Close();

    
    // Plot 1: Eta vs Phi comparison
    TCanvas* c1 = new TCanvas("c1", "Eta vs Phi Comparison", 1400, 600);
    c1->Divide(2, 1);
    
    c1->cd(1);
    gPad->SetRightMargin(0.15);
    h_eta_phi_all->Draw("COLZ");
    
    c1->cd(2);
    gPad->SetRightMargin(0.15);
    h_eta_phi_streak->Draw("COLZ");
    
    c1->SaveAs((png_output_dir + Form("%s_eta_phi_comparison.png", outputPrefix)).c_str());

    // Plot 2: Phi vs E comparison
    TCanvas* c2 = new TCanvas("c2", "Phi vs ET Comparison", 1400, 600);
    c2->Divide(2, 1);
    
    c2->cd(1);
    gPad->SetRightMargin(0.15);
    h_phi_Et_all->Draw("COLZ");
    
    c2->cd(2);
    gPad->SetRightMargin(0.15);
    h_phi_Et_streak->Draw("COLZ");
    
    c2->SaveAs((png_output_dir + Form("%s_phi_ET_comparison.png", outputPrefix)).c_str());

    // Plot 3: weta comparison with cut line
    TCanvas* c3 = new TCanvas("c3", "weta Distribution", 1000, 700);
    
    h_weta_all->SetLineColor(kBlue);
    h_weta_all->SetLineWidth(2);
    h_weta_all->Draw();
    
    h_weta_streak->SetLineColor(kRed);
    h_weta_streak->SetLineWidth(3);
    h_weta_streak->Draw("SAME");
    
    TLegend* leg_weta = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg_weta->AddEntry(h_weta_all, "All Events", "l");
    leg_weta->AddEntry(h_weta_streak, "Streak Events", "l");
    leg_weta->Draw();
    
    TLine* cutLine0 = new TLine(0.5, 0, 0.5, h_weta_all->GetMaximum());
    cutLine0->SetLineColor(kRed);
    cutLine0->SetLineStyle(2);
    cutLine0->SetLineWidth(4);
    cutLine0->Draw();
    
    TText* cutText0 = new TText(0.55, h_weta_all->GetMaximum()*0.8, "w_{#eta} > 0.5 cut");
    cutText0->SetTextColor(kRed);
    cutText0->SetTextSize(0.04);
    cutText0->Draw();
    
    c3->SaveAs((png_output_dir + Form("%s_weta_comparison.png", outputPrefix)).c_str());

    // Plot 4: weta_cogx comparison with cut line
    TCanvas* c4 = new TCanvas("c4", "weta_cogx Distribution", 1000, 700);
    
    h_weta_cogx_all->SetLineColor(kBlue);
    h_weta_cogx_all->SetLineWidth(2);
    h_weta_cogx_all->Draw();
    
    h_weta_cogx_streak->SetLineColor(kRed);
    h_weta_cogx_streak->SetLineWidth(3);
    h_weta_cogx_streak->Draw("SAME");
    
    TLegend* leg_cogx = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg_cogx->AddEntry(h_weta_cogx_all, "All Events", "l");
    leg_cogx->AddEntry(h_weta_cogx_streak, "Streak Events", "l");
    leg_cogx->Draw();
    
    TLine* cutLine1 = new TLine(0.5, 0, 0.5, h_weta_cogx_all->GetMaximum());
    cutLine1->SetLineColor(kRed);
    cutLine1->SetLineStyle(2);
    cutLine1->SetLineWidth(4);
    cutLine1->Draw();
    
    TText* cutText1 = new TText(0.55, h_weta_cogx_all->GetMaximum()*0.8, "weta_cogx > 0.5 cut");
    cutText1->SetTextColor(kRed);
    cutText1->SetTextSize(0.04);
    cutText1->Draw();
    
    c4->SaveAs((png_output_dir + Form("%s_weta_cogx_comparison.png", outputPrefix)).c_str());

    // Plot 5: E comparison with cut line
    TCanvas* c5 = new TCanvas("c5", "Cluster ET Distribution", 1000, 700);
    
    h_cluster_Et_all->SetLineColor(kBlue);
    h_cluster_Et_all->SetLineWidth(2);
    h_cluster_Et_all->Draw();
    
    h_cluster_Et_streak->SetLineColor(kRed);
    h_cluster_Et_streak->SetLineWidth(3);
    h_cluster_Et_streak->Draw("SAME");
    
    TLegend* leg2 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg2->AddEntry(h_cluster_Et_all, "All Events", "l");
    leg2->AddEntry(h_cluster_Et_streak, "Streak Events", "l");
    leg2->Draw();
    
    TLine* cutLine3 = new TLine(10, 0, 10, h_cluster_Et_all->GetMaximum());
    cutLine3->SetLineColor(kRed);
    cutLine3->SetLineStyle(2);
    cutLine3->SetLineWidth(4);
    cutLine3->Draw();
    
    TText* cutText3 = new TText(12, h_cluster_Et_all->GetMaximum()*0.8, "Et ≥ 10 GeV cut");
    cutText3->SetTextColor(kRed);
    cutText3->SetTextSize(0.04);
    cutText3->Draw();
    
    c5->SaveAs((png_output_dir + Form("%s_cluster_Et_comparison.png", outputPrefix)).c_str());

    // Plot 6: Energy bin comparisons
    TCanvas* c6 = new TCanvas("c6", "Transverse Energy Bin Comparisons", 1600, 1200);
    c6->Divide(2, 2);
    
    c6->cd(1);
    h_cluster_Et_10_20_all->SetLineColor(kBlue);
    h_cluster_Et_10_20_all->SetLineWidth(2);
    h_cluster_Et_10_20_all->Draw();
    h_cluster_Et_10_20_streak->SetLineColor(kRed);
    h_cluster_Et_10_20_streak->SetLineWidth(2);
    h_cluster_Et_10_20_streak->Draw("SAME");
    TLegend* leg_10_20 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg_10_20->AddEntry(h_cluster_Et_10_20_all, "All Events", "l");
    leg_10_20->AddEntry(h_cluster_Et_10_20_streak, "Streak Events", "l");
    leg_10_20->Draw();
    
    c6->cd(2);
    h_cluster_Et_20_25_all->SetLineColor(kBlue);
    h_cluster_Et_20_25_all->SetLineWidth(2);
    h_cluster_Et_20_25_all->Draw();
    h_cluster_Et_20_25_streak->SetLineColor(kRed);
    h_cluster_Et_20_25_streak->SetLineWidth(2);
    h_cluster_Et_20_25_streak->Draw("SAME");
    TLegend* leg_20_25 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg_20_25->AddEntry(h_cluster_Et_20_25_all, "All Events", "l");
    leg_20_25->AddEntry(h_cluster_Et_20_25_streak, "Streak Events", "l");
    leg_20_25->Draw();
    
    c6->cd(3);
    h_cluster_Et_25_30_all->SetLineColor(kBlue);
    h_cluster_Et_25_30_all->SetLineWidth(2);
    h_cluster_Et_25_30_all->Draw();
    h_cluster_Et_25_30_streak->SetLineColor(kRed);
    h_cluster_Et_25_30_streak->SetLineWidth(2);
    h_cluster_Et_25_30_streak->Draw("SAME");
    TLegend* leg_25_30 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg_25_30->AddEntry(h_cluster_Et_25_30_all, "All Events", "l");
    leg_25_30->AddEntry(h_cluster_Et_25_30_streak, "Streak Events", "l");
    leg_25_30->Draw();
    
    c6->cd(4);
    h_cluster_Et_gt30_all->SetLineColor(kBlue);
    h_cluster_Et_gt30_all->SetLineWidth(2);
    h_cluster_Et_gt30_all->Draw();
    h_cluster_Et_gt30_streak->SetLineColor(kRed);
    h_cluster_Et_gt30_streak->SetLineWidth(2);
    h_cluster_Et_gt30_streak->Draw("SAME");
    TLegend* leg_gt30 = new TLegend(0.6, 0.7, 0.9, 0.9);
    leg_gt30->AddEntry(h_cluster_Et_gt30_all, "All Events", "l");
    leg_gt30->AddEntry(h_cluster_Et_gt30_streak, "Streak Events", "l");
    leg_gt30->Draw();
    
    c6->SaveAs((png_output_dir + Form("%s_transverse_energy_bin_comparisons.png", outputPrefix)).c_str());

    // Plot 7: 2D Jet Analysis
    TCanvas* c7 = new TCanvas("c7", "2D Jet Analysis", 1600, 1200);
    c7->Divide(2, 2);
    
    c7->cd(1);
    gPad->SetRightMargin(0.15);
    h_asym_vs_leadingjet->Draw("COLZ");
    
    c7->cd(2);
    gPad->SetRightMargin(0.15);
    h_jet_pt_vs_phi->Draw("COLZ");
    
    c7->cd(3);
    gPad->SetRightMargin(0.15);
    h_jet_multiplicity_vs_asym->Draw("COLZ");
    
    c7->cd(4);
    gPad->SetRightMargin(0.15);
    h_dphi_vs_pt_ratio->Draw("COLZ");
    
    c7->SaveAs((png_output_dir + Form("%s_2D_jet_analysis.png", outputPrefix)).c_str());

    // Plot 8: Timing Analysis (if timing data available)
    if (has_time_array) {
        TCanvas* c_timing = new TCanvas("c_timing", "Timing Analysis", 1600, 1200);
        c_timing->Divide(2, 2);

        c_timing->cd(1);
        h_cluster_time_all->SetLineColor(kBlue);
        h_cluster_time_all->SetLineWidth(2);
        h_cluster_time_all->Draw();

        h_cluster_time_streak->SetLineColor(kRed);
        h_cluster_time_streak->SetLineWidth(3);
        h_cluster_time_streak->Draw("SAME");

        TLegend* leg_timing = new TLegend(0.6, 0.7, 0.9, 0.9);
        leg_timing->AddEntry(h_cluster_time_all, "All Events", "l");
        leg_timing->AddEntry(h_cluster_time_streak, "Streak Events", "l");
        leg_timing->Draw();

        // Add timing cut lines
        TLine* timing_cut_neg = new TLine(-10, 0, -10, h_cluster_time_all->GetMaximum());
        timing_cut_neg->SetLineColor(kRed);
        timing_cut_neg->SetLineStyle(2);
        timing_cut_neg->SetLineWidth(4);
        timing_cut_neg->Draw();

        TLine* timing_cut_pos = new TLine(10, 0, 10, h_cluster_time_all->GetMaximum());
        timing_cut_pos->SetLineColor(kRed);
        timing_cut_pos->SetLineStyle(2);
        timing_cut_pos->SetLineWidth(4);
        timing_cut_pos->Draw();

        c_timing->cd(2);
        h_time_spread_all->SetLineColor(kBlue);
        h_time_spread_all->SetLineWidth(2);
        h_time_spread_all->Draw();

        h_time_spread_streak->SetLineColor(kRed);
        h_time_spread_streak->SetLineWidth(3);
        h_time_spread_streak->Draw("SAME");

        TLegend* leg_spread = new TLegend(0.6, 0.7, 0.9, 0.9);
        leg_spread->AddEntry(h_time_spread_all, "All Events", "l");
        leg_spread->AddEntry(h_time_spread_streak, "Streak Events", "l");
        leg_spread->Draw();

        // Add timing spread cut line
        TLine* spread_cut = new TLine(5.0, 0, 5.0, h_time_spread_all->GetMaximum());
        spread_cut->SetLineColor(kRed);
        spread_cut->SetLineStyle(2);
        spread_cut->SetLineWidth(4);
        spread_cut->Draw();

        c_timing->cd(3);
        gPad->SetRightMargin(0.15);
        h_time_vs_e->Draw("COLZ");

        c_timing->cd(4);
        gPad->SetRightMargin(0.15);
        h_time_spread_vs_e->Draw("COLZ");

        c_timing->SaveAs((png_output_dir + Form("%s_timing_analysis.png", outputPrefix)).c_str());
        delete c_timing;
    }

    // Plot 9: Diagnostic plots
    TCanvas* c8 = new TCanvas("c8", "Diagnostic Plots", 1200, 800);
    c8->Divide(2, 2);
    
    c8->cd(1);
    h_asymmetry->SetLineColor(kGreen+2);
    h_asymmetry->Draw();
    
    c8->cd(2);
    h_jet_dphi->SetLineColor(kOrange);
    h_jet_dphi->Draw();
    
    TLine* dphiCutLine = new TLine(TMath::Pi()/2.0, 0, TMath::Pi()/2.0, h_jet_dphi->GetMaximum());
    dphiCutLine->SetLineColor(kRed);
    dphiCutLine->SetLineStyle(2);
    dphiCutLine->SetLineWidth(4);
    dphiCutLine->Draw();
    
    c8->cd(3);
    h_jet_asym->SetLineColor(kMagenta);
    h_jet_asym->Draw();
    
    c8->cd(4);
    // Summary text
    TText* summary = new TText(0.1, 0.8, Form("Total Events: %lld", max_events));
    summary->SetNDC();
    summary->SetTextSize(0.06);
    summary->Draw();
    
    TText* summary2 = new TText(0.1, 0.7, Form("Streak Events: %d", streakCount));
    summary2->SetNDC();
    summary2->SetTextSize(0.06);
    summary2->Draw();
    
    TText* summary3 = new TText(0.1, 0.6, Form("Rate: %.2f%%", (double)streakCount/max_events*100));
    summary3->SetNDC();
    summary3->SetTextSize(0.06);
    summary3->Draw();
    
    TText* summary4 = new TText(0.1, 0.5, "Selection Criteria:");
    summary4->SetNDC();
    summary4->SetTextSize(0.05);
    summary4->Draw();
    
    TText* summary5 = new TText(0.1, 0.4, "weta > 0.5 & Et >= 10");
    summary5->SetNDC();
    summary5->SetTextSize(0.04);
    summary5->Draw();
    
    TText* summary6 = new TText(0.1, 0.3, "No back-to-back jets (pT>10, dphi>pi/2)");
    summary6->SetNDC();
    summary6->SetTextSize(0.04);
    summary6->Draw();
    
    c8->SaveAs((png_output_dir + Form("%s_diagnostics.png", outputPrefix)).c_str());

    // Print summary
    std::cout << "\n=== ANALYSIS COMPLETE ===" << std::endl;
    std::cout << "Total events processed: " << max_events << std::endl;
    std::cout << "Streak events found: " << streakCount << std::endl;
    std::cout << "Streak event rate: " << (double)streakCount/max_events*100 << "%" << std::endl;
    std::cout << "\nExpert's Criteria Applied:" << std::endl;
    std::cout << "1. Cluster: weta > 0.5 AND Et >= 10 GeV" << std::endl;
    std::cout << "2. Jets: NO back-to-back jets with pT > 10 GeV and dphi > pi/2" << std::endl;
    if (has_time_array) {
        std::cout << "3. Timing: Analysis included" << std::endl;
    } else {
        std::cout << "3. Timing: Not available in this dataset" << std::endl;
    }
    std::cout << "\nOutput files created:" << std::endl;
    std::cout << "- ROOT file: " << rootFileName << std::endl;
    std::cout << "- Event list: " << outputPrefix << "_event_list.txt" << std::endl;
    std::cout << "- PNG plots: " << png_output_dir << outputPrefix << "_*.png" << std::endl;
    std::cout << "\nCandidate streak events logged and visualized." << std::endl;

    // Create run summary file
    std::ofstream run_summary(std::string(outputPrefix) + "_run_summary.txt");
    run_summary << "# Run Summary: Streak Event Statistics\n";
    run_summary << "# RunNumber TotalEvents StreakEvents Rate(%)\n";
    run_summary << "#\n";

    // Print header to console
    std::cout << "\n=== RUN SUMMARY ===" << std::endl;
    std::cout << "RunNumber  TotalEvents  StreakEvents  Rate(%)" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    // Calculate and output statistics for each run
    for (const auto& run_pair : run_total_events) {
        int run_number = run_pair.first;
        int total_events = run_pair.second;
        int streak_events = run_streak_events[run_number];  // Will be 0 if no streaks in this run
        double rate = (total_events > 0) ? (double)streak_events / total_events * 100.0 : 0.0;
        
        // Write to file
        run_summary << run_number << " " << total_events << " " << streak_events 
                    << " " << std::fixed << std::setprecision(3) << rate << std::endl;
        
        // Print to console
        std::cout << std::setw(9) << run_number 
                << std::setw(12) << total_events 
                << std::setw(13) << streak_events 
                << std::setw(9) << std::fixed << std::setprecision(3) << rate << std::endl;
    }

    run_summary.close();


    // Calculate overall statistics
    int total_runs = run_total_events.size();
    int total_all_events = 0;
    int total_all_streaks = 0;

    for (const auto& run_pair : run_total_events) {
        total_all_events += run_pair.second;
        total_all_streaks += run_streak_events[run_pair.first];
    }

    double overall_rate = (total_all_events > 0) ? (double)total_all_streaks / total_all_events * 100.0 : 0.0;

    // Print overall summary
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << "TOTAL:     " << std::setw(12) << total_all_events 
            << std::setw(13) << total_all_streaks 
            << std::setw(9) << std::fixed << std::setprecision(3) << overall_rate << std::endl;
    std::cout << "\nTotal runs processed: " << total_runs << std::endl;
    std::cout << "Average streak rate: " << std::fixed << std::setprecision(3) << overall_rate << "%" << std::endl;

    // Add to summary file
    std::ofstream run_summary_append(std::string(outputPrefix) + "_run_summary.txt", std::ios::app);
    run_summary_append << "#\n";
    run_summary_append << "# OVERALL STATISTICS:\n";
    run_summary_append << "# Total runs: " << total_runs << std::endl;
    run_summary_append << "# Total events: " << total_all_events << std::endl;
    run_summary_append << "# Total streak events: " << total_all_streaks << std::endl;
    run_summary_append << "# Overall rate: " << std::fixed << std::setprecision(3) << overall_rate << "%" << std::endl;
    
    run_summary_append.close();


    // Create vector of pairs for sorting
    std::vector<std::tuple<int, int, int, double>> run_stats;
    for (const auto& run_pair : run_total_events) {
        int run_number = run_pair.first;
        int total_events = run_pair.second;
        int streak_events = run_streak_events[run_number];
        double rate = (total_events > 0) ? (double)streak_events / total_events * 100.0 : 0.0;
        run_stats.emplace_back(run_number, total_events, streak_events, rate);
    }

    // Sort by rate (descending)
    std::sort(run_stats.begin(), run_stats.end(), 
            [](const auto& a, const auto& b) { return std::get<3>(a) > std::get<3>(b); });

    std::cout << "\n=== RUNS SORTED BY STREAK RATE ===" << std::endl;
    for (const auto& stat : run_stats) {
        std::cout << "Run " << std::get<0>(stat) << ": " << std::get<3>(stat) << "% (" 
                << std::get<2>(stat) << "/" << std::get<1>(stat) << ")" << std::endl;
    }

    // Clean up
    f->Close();
    delete c1;
    delete c2;
    delete c3;
    delete c4;
    delete c5;
    delete c6;
    delete c7;
    delete c8;
}
