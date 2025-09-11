
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <regex>

#include <TChain.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <TSystemFile.h>
#include <TMath.h>

// sPHENIX classes
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <ffaobjects/EventHeader.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <CLHEP/Vector/ThreeVector.h>

using namespace std;

// sPHENIX detector geometry constants (Following JetKinematicCheck.cc)
const float SPHENIX_ETA_MIN = -1.1;
const float SPHENIX_ETA_MAX = 1.1;
const float SPHENIX_PHI_MIN = -TMath::Pi();    // [-π, π] as per sPHENIX standard
const float SPHENIX_PHI_MAX = TMath::Pi();

// Detector granularities
const int EMCAL_ETA_BINS = 96;
const int EMCAL_PHI_BINS = 256;
const int HCAL_ETA_BINS = 24;
const int HCAL_PHI_BINS = 64;

/**
 * Convert EMCal tower indices to physical coordinates
 */
pair<float, float> getEMCalPhysicalCoords(int ieta, int iphi) {
    const float EMCAL_DELTA_ETA = (SPHENIX_ETA_MAX - SPHENIX_ETA_MIN) / EMCAL_ETA_BINS;
    const float EMCAL_DELTA_PHI = (SPHENIX_PHI_MAX - SPHENIX_PHI_MIN) / EMCAL_PHI_BINS;
    
    float eta = SPHENIX_ETA_MIN + (ieta + 0.5) * EMCAL_DELTA_ETA;
    float phi = SPHENIX_PHI_MIN + (iphi + 0.5) * EMCAL_DELTA_PHI;
    return make_pair(eta, phi);
}

/**
 * Convert HCal tower indices to physical coordinates
 */
pair<float, float> getHCalPhysicalCoords(int ieta, int iphi) {
    const float HCAL_DELTA_ETA = (SPHENIX_ETA_MAX - SPHENIX_ETA_MIN) / HCAL_ETA_BINS;
    const float HCAL_DELTA_PHI = (SPHENIX_PHI_MAX - SPHENIX_PHI_MIN) / HCAL_PHI_BINS;
    
    float eta = SPHENIX_ETA_MIN + (ieta + 0.5) * HCAL_DELTA_ETA;
    float phi = SPHENIX_PHI_MIN + (iphi + 0.5) * HCAL_DELTA_PHI;
    return make_pair(eta, phi);
}

/**
 * Debug function to show coordinate mapping
 */
void printCoordinateMapping() {
    cout << "\n=== sPHENIX Coordinate Mapping ===" << endl;
    cout << "Physical ranges: η ∈ [" << SPHENIX_ETA_MIN << ", " << SPHENIX_ETA_MAX << "], "
         << "φ ∈ [" << SPHENIX_PHI_MIN << ", " << SPHENIX_PHI_MAX << "] rad" << endl;
    
    cout << "\nEMCal:" << endl;
    cout << "  Towers: " << EMCAL_ETA_BINS << "  " << EMCAL_PHI_BINS << endl;
    cout << "  Granularity: Δη ≈ " << (SPHENIX_ETA_MAX - SPHENIX_ETA_MIN) / EMCAL_ETA_BINS 
         << ", Δφ ≈ " << (SPHENIX_PHI_MAX - SPHENIX_PHI_MIN) / EMCAL_PHI_BINS << " rad" << endl;
    
    cout << "\nHCal:" << endl;
    cout << "  Towers: " << HCAL_ETA_BINS << "  " << HCAL_PHI_BINS << endl;
    cout << "  Granularity: Δη ≈ " << (SPHENIX_ETA_MAX - SPHENIX_ETA_MIN) / HCAL_ETA_BINS 
         << ", Δφ ≈ " << (SPHENIX_PHI_MAX - SPHENIX_PHI_MIN) / HCAL_PHI_BINS << " rad" << endl;
}

// Function to extract run numbers from filenames in a directory
set<int> getRunNumbers(const string& dir_path) {
    set<int> run_numbers;
    
    TSystemDirectory dir(".", dir_path.c_str());
    TList* files = dir.GetListOfFiles();
    
    if (files) {
        TSystemFile* file;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
            if (file->IsDirectory()) continue;
            
            string filename = file->GetName();
            if (filename.find("OUTTREE_DST_JET_run2pp_ana468_") != string::npos) {
                // Extract run number using regex
                regex run_pattern("OUTTREE_DST_JET_run2pp_ana468_.*-([0-9]{8})-");
                smatch match;
                if (regex_search(filename, match, run_pattern)) {
                    int run_num = stoi(match[1].str());
                    run_numbers.insert(run_num);
                }
            }
        }
    }
    
    return run_numbers;
}

// Function to get all OutDir directories
vector<string> getOutDirList(const string& base_path) {
    vector<string> outDirs;
    
    TSystemDirectory dir(".", base_path.c_str());
    TList* files = dir.GetListOfFiles();
    
    if (files) {
        TSystemFile* file;
        TIter next(files);
        while ((file = (TSystemFile*)next())) {
            string dirname = file->GetName();
            if (dirname.find("OutDir") == 0 && file->IsDirectory()) {
                outDirs.push_back(dirname);
            }
        }
    }
    
    // Sort numerically
    sort(outDirs.begin(), outDirs.end(), [](const string& a, const string& b) {
        int num_a = stoi(a.substr(6)); // Extract number after "OutDir"
        int num_b = stoi(b.substr(6));
        return num_a < num_b;
    });
    
    return outDirs;
}

void process_single_outdir(const string& input_base_path, const string& outdir_name, 
                          const string& output_base_path, const std::set<unsigned int>& target_events) {
    
    cout << "\n" << string(80, '=') << endl;
    cout << "PROCESSING: " << outdir_name << endl;
    cout << string(80, '=') << endl;
    
    // Construct paths
    string input_path = input_base_path + "/" + outdir_name;
    string output_path = output_base_path + "/" + outdir_name;
    
    cout << "Input path: " << input_path << endl;
    cout << "Output path: " << output_path << endl;
    
    // Create output directory
    gSystem->mkdir(output_path.c_str(), true);
    
    // Get run numbers in this directory
    set<int> run_numbers = getRunNumbers(input_path);
    
    if (run_numbers.empty()) {
        cout << "WARNING: No OUTTREE files found in " << input_path << endl;
        return;
    }
    
    cout << "Found " << run_numbers.size() << " run(s) in " << outdir_name << ": ";
    for (int run : run_numbers) {
        cout << run << " ";
    }
    cout << endl;
    
    // Process each run found in this directory
    for (int run_number : run_numbers) {
        cout << "\n--- Processing Run " << run_number << " in " << outdir_name << " ---" << endl;
        
        string input_pattern = input_path + "/OUTTREE_DST_JET_run2pp_ana468_*-" + 
                              Form("%08d", run_number) + "-*.root";
        
        cout << "Looking for pattern: " << input_pattern << endl;
        
        // Load files for this specific run
        TChain chain("T");
        int files_added = chain.Add(input_pattern.c_str());
        
        if (files_added == 0) {
            cout << "WARNING: No files found for run " << run_number << " in " << outdir_name << endl;
            continue;
        }
        
        cout << "Added " << files_added << " files for run " << run_number << endl;
        
        // Set up containers
        TowerInfoContainer* emc_towers = nullptr;
        TowerInfoContainer* ihcal_towers = nullptr;
        TowerInfoContainer* ohcal_towers = nullptr;
        RawClusterContainer* emc_clusters = nullptr;
        EventHeader* eventHeader = nullptr;
        GlobalVertexMap* vertexMap = nullptr;
        
        // Branch connections
        if (chain.GetBranch("DST#CEMC#TOWERINFO_CALIB_CEMC")) {
            chain.SetBranchAddress("DST#CEMC#TOWERINFO_CALIB_CEMC", &emc_towers);
        }
        
        if (chain.GetBranch("DST#HCALIN#TOWERINFO_CALIB_HCALIN")) {
            chain.SetBranchAddress("DST#HCALIN#TOWERINFO_CALIB_HCALIN", &ihcal_towers);
        }
        
        if (chain.GetBranch("DST#HCALOUT#TOWERINFO_CALIB_HCALOUT")) {
            chain.SetBranchAddress("DST#HCALOUT#TOWERINFO_CALIB_HCALOUT", &ohcal_towers);
        }
        
        if (chain.GetBranch("DST#CEMC#CLUSTERINFO_CEMC")) {
            chain.SetBranchAddress("DST#CEMC#CLUSTERINFO_CEMC", &emc_clusters);
        }
        
        if (chain.GetBranch("DST#EventHeader")) {
            chain.SetBranchAddress("DST#EventHeader", &eventHeader);
        }
        
        if (chain.GetBranch("DST#GLOBAL#GlobalVertexMap")) {
            chain.SetBranchAddress("DST#GLOBAL#GlobalVertexMap", &vertexMap);
        }
        
        const float energy_threshold = 0.1; // 0.1 or 0.5 GeV
        Long64_t nentries = chain.GetEntries();
        int processed_events = 0;
        
        cout << "Entries for run " << run_number << ": " << nentries << endl;
        
        for (Long64_t i = 0; i < nentries; ++i) {
            // Show progress
            if (i % 10000 == 0 && i > 0) {
                cout << "Processing entry " << i << " / " << nentries 
                     << " (" << (100.0 * i / nentries) << "%) - Found " << processed_events << " streak events" << endl;
            }
            
            chain.GetEntry(i);
            
            int event_number = i;
            
            if (eventHeader) {
                event_number = eventHeader->get_EvtSequence();
            }
            
            // Check if this is a target streak event
            if (target_events.find(event_number) == target_events.end()) continue;
            
            cout << "\n*** PROCESSING STREAK EVENT " << event_number << " (Run " << run_number << ") ***" << endl;
            
            // Get vertex information
            CLHEP::Hep3Vector vertex(0, 0, 0);
            if (vertexMap && !vertexMap->empty()) {
                GlobalVertex* vtx = vertexMap->begin()->second;
                if (vtx) {
                    vertex.setZ(vtx->get_z());
                }
            }
            
            // Hit Maps Creation
            
            // 1. EMCAL Tower Distribution (PHYSICAL coordinates)
            TH2F* h2TowerMap_EMCal = new TH2F(Form("h2TowerMap_EMCal_evt%d", event_number),
                                             Form("EMCal Tower Distribution - Run %d Event %d;#eta (pseudorapidity);#phi [rad] (azimuth)", run_number, event_number),
                                             EMCAL_ETA_BINS, SPHENIX_ETA_MIN, SPHENIX_ETA_MAX,    // Physical eta range
                                             EMCAL_PHI_BINS, SPHENIX_PHI_MIN, SPHENIX_PHI_MAX);   // Physical phi range 
            
            // 2. IHCAL Tower Distribution (PHYSICAL coordinates)
            TH2F* h2TowerMap_IHCal = new TH2F(Form("h2TowerMap_IHCal_evt%d", event_number),
                                             Form("IHCal Tower Distribution - Run %d Event %d;#eta (pseudorapidity);#phi [rad] (azimuth)", run_number, event_number),
                                             HCAL_ETA_BINS, SPHENIX_ETA_MIN, SPHENIX_ETA_MAX,     // Physical eta range
                                             HCAL_PHI_BINS, SPHENIX_PHI_MIN, SPHENIX_PHI_MAX);    // Physical phi range 
            
            // 3. OHCAL Tower Distribution (PHYSICAL coordinates)
            TH2F* h2TowerMap_OHCal = new TH2F(Form("h2TowerMap_OHCal_evt%d", event_number),
                                             Form("OHCal Tower Distribution - Run %d Event %d;#eta (pseudorapidity);#phi [rad] (azimuth)", run_number, event_number),
                                             HCAL_ETA_BINS, SPHENIX_ETA_MIN, SPHENIX_ETA_MAX,     // Physical eta range
                                             HCAL_PHI_BINS, SPHENIX_PHI_MIN, SPHENIX_PHI_MAX);    // Physical phi range 
            
            // 4. EMCAL Cluster Hitmap (already in physical coordinates)
            TH2F* h2HitMap_EMCal = new TH2F(Form("h2HitMap_EMCal_evt%d", event_number),
                                           Form("EMCal Hit Map - Run %d Event %d;#eta (pseudorapidity);#phi [rad] (azimuth)", run_number, event_number),
                                           100, SPHENIX_ETA_MIN, SPHENIX_ETA_MAX,               // Physical eta range
                                           64, SPHENIX_PHI_MIN, SPHENIX_PHI_MAX);               // Physical phi range 
            
            // 5. Time maps for each tower type
            TH2F* h2TimeMap_EMCal = new TH2F(Form("h2TimeMap_EMCal_evt%d", event_number),
                                             Form("EMCal Tower Time Map - Run %d Event %d;#eta (pseudorapidity);#phi [rad] (azimuth)", run_number, event_number),
                                             EMCAL_ETA_BINS, SPHENIX_ETA_MIN, SPHENIX_ETA_MAX,    // Physical eta range
                                             EMCAL_PHI_BINS, SPHENIX_PHI_MIN, SPHENIX_PHI_MAX);   // Physical phi range           
            TH2F* h2TimeMap_IHCal = new TH2F(Form("h2TimeMap_IHCal_evt%d", event_number),
                                             Form("IHCal Tower Time Map - Run %d Event %d;#eta (pseudorapidity);#phi [rad] (azimuth)", run_number, event_number ),
                                             HCAL_ETA_BINS, SPHENIX_ETA_MIN, SPHENIX_ETA_MAX,     // Physical eta range
                                             HCAL_PHI_BINS, SPHENIX_PHI_MIN, SPHENIX_PHI_MAX);    // Physical phi range
            TH2F* h2TimeMap_OHCal = new TH2F(Form("h2TimeMap_OHCal_evt%d", event_number),
                                             Form("OHCal Tower Time Map - Run %d Event %d;#eta (pseudorapidity);#phi [rad] (azimuth)", run_number, event_number),
                                             HCAL_ETA_BINS, SPHENIX_ETA_MIN, SPHENIX_ETA_MAX,     // Physical eta range
                                             HCAL_PHI_BINS, SPHENIX_PHI_MIN, SPHENIX_PHI_MAX);    // Physical phi range   

            // filling EMCAL towers using physical coordinates
            if (emc_towers) {
                int emc_count = 0;
                for (unsigned int ich = 0; ich < emc_towers->size(); ++ich) {
                    TowerInfo* tower = emc_towers->get_tower_at_channel(ich);
                    if (!tower || !tower->get_isGood()) continue;
                    
                    float energy = tower->get_energy();
                    if (energy < energy_threshold) continue;
                    float time   = tower->get_time();  //time information

                                              if (time < -9999) continue; // Skip invalid time values
                     // Print or check time
                    if (ich < 10) std::cout << "EMCal Tower " << ich << " time: " << time << std::endl; 

                    unsigned int towerkey = emc_towers->encode_key(ich);
                    int ieta = TowerInfoDefs::getCaloTowerEtaBin(towerkey);
                    int iphi = TowerInfoDefs::getCaloTowerPhiBin(towerkey);
                    
                    // Convert to physical coordinates
                    auto coords = getEMCalPhysicalCoords(ieta, iphi);
                    float eta_phys = coords.first;
                    float phi_phys = coords.second;
                    
                    h2TowerMap_EMCal->Fill(eta_phys, phi_phys, energy);
                    h2TimeMap_EMCal->Fill(eta_phys, phi_phys, time);
                    emc_count++;
                }
                cout << "EMCAL towers: " << emc_count << " (filled with physical coordinates)" << endl;
            }
            
            // Fill IHCAL towers using PHYSICAL coordinates
            if (ihcal_towers) {
                int ihcal_count = 0;
                for (unsigned int ich = 0; ich < ihcal_towers->size(); ++ich) {
                    TowerInfo* tower = ihcal_towers->get_tower_at_channel(ich);
                    if (!tower || !tower->get_isGood()) continue;
                    
                    float energy = tower->get_energy();
                    if (energy < energy_threshold) continue;

                    float time   = tower->get_time();  //time information
                    if (time < -9999) continue; // Skip invalid time values
                     // Print or check time
                    if (ich < 10) std::cout << "IHCal Tower " << ich << " time: " << time << std::endl; 

                    unsigned int towerkey = ihcal_towers->encode_key(ich);
                    int ieta = TowerInfoDefs::getCaloTowerEtaBin(towerkey);
                    int iphi = TowerInfoDefs::getCaloTowerPhiBin(towerkey);
                    
                    // Convert to physical coordinates (CRITICAL STEP)
                    auto coords = getHCalPhysicalCoords(ieta, iphi);
                    float eta_phys = coords.first;
                    float phi_phys = coords.second;
                    
                    h2TowerMap_IHCal->Fill(eta_phys, phi_phys, energy);
                    h2TimeMap_IHCal->Fill(eta_phys, phi_phys, time);
                    ihcal_count++;
                }
                cout << "IHCAL towers: " << ihcal_count << " (filled with physical coordinates)" << endl;
            }
            
            // Fill OHCAL towers using PHYSICAL coordinates
            if (ohcal_towers) {
                int ohcal_count = 0;
                for (unsigned int ich = 0; ich < ohcal_towers->size(); ++ich) {
                    TowerInfo* tower = ohcal_towers->get_tower_at_channel(ich);
                    if (!tower || !tower->get_isGood()) continue;
                    
                    float energy = tower->get_energy();
                    if (energy < energy_threshold) continue;

                    float time   = tower->get_time();  //time information
                                        if (time < -9999) continue; // Skip invalid time values
                     // Print or check time
                    if (ich < 10) std::cout << "OHCal Tower " << ich << " time: " << time << std::endl; 
                    
                    unsigned int towerkey = ohcal_towers->encode_key(ich);
                    int ieta = TowerInfoDefs::getCaloTowerEtaBin(towerkey);
                    int iphi = TowerInfoDefs::getCaloTowerPhiBin(towerkey);
                    
                    // Convert to physical coordinates (CRITICAL STEP)
                    auto coords = getHCalPhysicalCoords(ieta, iphi);
                    float eta_phys = coords.first;
                    float phi_phys = coords.second;
                    
                    h2TowerMap_OHCal->Fill(eta_phys, phi_phys, energy);
                    h2TimeMap_OHCal->Fill(eta_phys, phi_phys, time);
                    ohcal_count++;
                }
                cout << "OHCAL towers: " << ohcal_count << " (filled with physical coordinates)" << endl;
            }
            
            // Fill EMCAL clusters (already using physical coordinates)
            if (emc_clusters) {
                RawClusterContainer::ConstRange begin_end_EMC = emc_clusters->getClusters();
                int cluster_count = 0;
                
                // Loop over the EMCal clusters
                for (RawClusterContainer::ConstIterator clusIter_EMC = begin_end_EMC.first; 
                     clusIter_EMC != begin_end_EMC.second; ++clusIter_EMC) {
                    
                    RawCluster *cluster = clusIter_EMC->second;
                    if (!cluster) continue;
                    
                    // Calculate cluster position using vertex information
                    float cluster_energy = RawClusterUtility::GetET(*cluster, vertex);
                    float cluster_phi = RawClusterUtility::GetAzimuthAngle(*cluster, vertex);
                    float cluster_eta = RawClusterUtility::GetPseudorapidity(*cluster, vertex);
                    
                    if (cluster_energy < 0.5) continue; // Energy threshold for clusters
                    
                    h2HitMap_EMCal->Fill(cluster_eta, cluster_phi, cluster_energy);
                    cluster_count++;
                }
                cout << "EMCAL clusters: " << cluster_count << " (already using physical coordinates)" << endl;
            }
            
            // Save individual ROOT file for this event
            string root_filename = Form("%s/streak_analysis_run%d_evt%d.root", 
                                       output_path.c_str(), run_number, event_number);
            TFile* fout = new TFile(root_filename.c_str(), "RECREATE");
            
            // Write all 4 histograms
            h2TowerMap_EMCal->Write();
            h2TowerMap_IHCal->Write();
            h2TowerMap_OHCal->Write();
            h2HitMap_EMCal->Write();
           
            //time information
            h2TimeMap_EMCal->Write();
            h2TimeMap_IHCal->Write();
            h2TimeMap_OHCal->Write();

            // Write event information
            TNamed event_info("event_info", Form("Run %d Event %d from %s", run_number, event_number, outdir_name.c_str()));
            event_info.Write();
            
            fout->Close();
            delete fout;
            
            // Cleanup histograms
            delete h2TowerMap_EMCal;
            delete h2TowerMap_IHCal;
            delete h2TowerMap_OHCal;
            delete h2HitMap_EMCal;
            delete h2TimeMap_EMCal;
            delete h2TimeMap_IHCal;
            delete h2TimeMap_OHCal;

            
            processed_events++;
            cout << "SAVED: " << root_filename << " (Event " << processed_events << ")" << endl;
        }
        
        cout << "Run " << run_number << " in " << outdir_name << " complete: " << processed_events << " streak events" << endl;
    }
}

void process_streak_events(const string& event_list_file = "streak_events_all.txt") {
    
    // Set ROOT style
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);
    
    cout << "=== sPHENIX Individual Streak Event Analysis - PHYSICAL COORDINATES VERSION ===" << endl;
    cout << "Features:" << endl;
    cout << "  - Extracts run numbers from filenames automatically" << endl;
    cout << "  - Individual ROOT file per streak event" << endl;
    cout << "  - 4 histograms per event with PHYSICAL coordinates" << endl;
    cout << "  - Tower indices converted to η-φ coordinates" << endl;
    cout << "  - Compatible with continuity analysis" << endl;
    cout << "  - Time information added to tower maps" << endl;
    
    // Display coordinate mapping
    printCoordinateMapping();
    
    // CORRECTED PATHS
    string input_base_path = "/sphenix/user/shuhangli/ppg12/eventskimmer/macro/condorout";
    string output_base_path = "/sphenix/tg/tg01/jets/muhammadibrahim/streak_events/hitmaps/out_root";
    
    cout << "\nPaths:" << endl;
    cout << "Input:  " << input_base_path << endl;
    cout << "Output: " << output_base_path << endl;
    
    // Read event list
    std::set<unsigned int> target_events;
    ifstream infile(event_list_file);
    
    if (!infile.is_open()) {
        cout << "ERROR: Could not open event list file: " << event_list_file << endl;
        return;
    }
    
    string line;
    int line_count = 0;
    while (getline(infile, line)) {
        line_count++;
        if (line.empty() || line[0] == '#') continue;
        
        istringstream iss(line);
        int run, event;
        if (iss >> run >> event) {
            target_events.insert(event);
        } else {
            cout << "Warning: Could not parse line " << line_count << ": " << line << endl;
        }
    }
    infile.close();
    
    if (target_events.empty()) {
        cout << "ERROR: No valid events found in " << event_list_file << endl;
        return;
    }
    
    cout << "\nTarget streak events: " << target_events.size() << endl;
    
    // Get list of OutDir directories
    vector<string> outDirs = getOutDirList(input_base_path);
    
    if (outDirs.empty()) {
        cout << "ERROR: No OutDir directories found in " << input_base_path << endl;
        return;
    }
    
    cout << "Found " << outDirs.size() << " OutDir directories" << endl;
    
    // Create base output directory
    gSystem->mkdir(output_base_path.c_str(), true);
    
    // Process each OutDir individually
    int dir_count = 0;
    for (const string& outdir : outDirs) {
        dir_count++;
        
        cout << "\n" << string(100, '=') << endl;
        cout << "PROCESSING DIRECTORY " << dir_count << "/" << outDirs.size() << ": " << outdir << endl;
        cout << string(100, '=') << endl;
        
        process_single_outdir(input_base_path, outdir, output_base_path, target_events);
        
        // Small delay between directories
        gSystem->Sleep(1000);
    }
    
    cout << "\n" << string(100, '=') << endl;
    cout << "ANALYSIS COMPLETE" << endl;
    cout << string(100, '=') << endl;
    cout << "Summary:" << endl;
    cout << "  - Processed " << outDirs.size() << " OutDir directories" << endl;
    cout << "  - Target streak events: " << target_events.size() << endl;
    cout << "  - Output: " << output_base_path << "/OutDir*/streak_analysis_runN_evtM.root" << endl;
    cout << "  - All histograms now use physical coordinates!" << endl;
}
