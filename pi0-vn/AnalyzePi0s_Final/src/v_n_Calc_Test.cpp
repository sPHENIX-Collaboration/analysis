#include <map>
#include <string>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>


// Global constants
const float energy_cut = 0.5;
const float asym_cut = 0.5;
const float delR_cut = 0;
const float chi2_cut = 4;

map<int, pair<float, float>> signalBounds;

const float processingFraction = 1.0;

void readCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return;
    }

    string line;
    bool firstLine = true;  // To skip the header if it exists
    while (getline(file, line)) {
        if (firstLine) {  // Skip the first line if it's a header
            firstLine = false;
            continue;
        }

        stringstream ss(line);
        string token;
        vector<string> tokens;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        try {
            int index = stoi(tokens[0]);
            float lowerBound = stof(tokens[15]);
            float upperBound = stof(tokens[16]);
            signalBounds[index] = make_pair(lowerBound, upperBound);
        } catch (const std::invalid_argument& e) {
            cout << "Invalid argument: " << e.what() << " in line: " << line << endl;
        } catch (const std::out_of_range& e) {
            cout << "Out of range error: " << e.what() << " in line: " << line << endl;
        }
    }

    file.close();

    // Print the contents of signalBounds
    for (const auto& kv : signalBounds) {
        cout << "Index: " << kv.first << " LowerBound: " << kv.second.first << ", UpperBound: " << kv.second.second << endl;
    }
}



void v_n_Calc_Test() {
    readCSV("/sphenix/u/patsfan753/v_n_Analysis/PlotByPlotOutputJamiesUpdatedMasking_edits.csv");
    // Define centrality and pT categories
    map<string, pair<Float_t, Float_t>> centrality = {{"40-60", make_pair(177.222, 465.741)},
                                                      {"20-40", make_pair(465.741, 935.37)}};
    map<string, pair<Float_t, Float_t>> diphoton_pt = {{"2-2.5", make_pair(2, 2.5)},
                                                       {"2.5-3", make_pair(2.5, 3)},
                                                       {"3-3.5", make_pair(3, 3.5)},
                                                       {"3.5-4", make_pair(3.5, 4)},
                                                       {"4-4.5", make_pair(4, 4.5)},
                                                       {"4.5-5", make_pair(4.5, 5)}};
    
    
    vector<string> cent_key = {"40-60", "20-40"};
    vector<string> pt_key   = {"2-2.5", "2.5-3", "3-3.5", "3.5-4", "4-4.5", "4.5-5"};

    TH1F* pt_dum_vec   = new TH1F("pt_dum_vec","",6,2,5);
    TH1F* cent_dum_vec = new TH1F("cent_dum_vec","", 2, new Double_t[3] {177.222, 465.741, 935.37});

    vector<Float_t> pi0_ctr(cent_key.size()*pt_key.size());

    vector<UInt_t> evt_ctr(cent_key.size());

    vector<Float_t> QQ(cent_key.size());

    vector<Float_t> qQ(cent_key.size()*pt_key.size());


    // Create a TChain
    TChain *chain = new TChain("T2");

    // Open the list file and add each ROOT file to the chain
    ifstream listFile("/direct/sphenix+tg+tg01/bulk/anarde/pi0-vn/pi0.list");
    string rootFilePath;
    int fileCount = 0; // Counter for the number of files processed
    int totalCount = 0; // Total number of files in the list

    // First, count the total number of files in the list
    while (getline(listFile, rootFilePath)) {
        totalCount++;
    }

    // Reset file stream to the beginning of the file
    listFile.clear();
    listFile.seekg(0, ios::beg);

    // Calculate the number of files to process
    int filesToProcess = static_cast<int>(totalCount * processingFraction);

    // Now add only a fraction of files to the TChain
    while (getline(listFile, rootFilePath) && fileCount < filesToProcess) {
        chain->Add(rootFilePath.c_str());
        //cout << "Added file to chain: " << rootFilePath << endl;
        fileCount++;
    }

    /*
     6674 PRINTS FOR TOTAL NUMBER WITHOUT THE PERCENTAGE OF FILES CODE--checked and this code works properly when set to 1.0 for processingFraction
     */
    cout << "Total number of files added to TChain: " << fileCount << endl;

    // Check if the chain is valid
    if (chain->IsZombie()) {
        cout << "Error creating TChain" << endl;
        return;
    }

    

    // Check if the chain is valid
    if (chain->IsZombie()) {
        cout << "Error creating TChain" << endl;
        return;
    }


    Int_t   run;
    Int_t   event;
    Float_t totalMBD;
    Float_t Q_N_x;
    Float_t Q_N_y;
    Float_t Q_P_x;
    Float_t Q_P_y;
    vector<Float_t>* pi0_mass   = 0;
    vector<Float_t>* pi0_pt     = 0;
    vector<Float_t>* pi0_phi    = 0;
    vector<Float_t>* pi0_eta    = 0;
    vector<Float_t>* pi0_asym   = 0;
    vector<Float_t>* pi0_deltaR = 0;
    vector<Float_t>* ecore_min  = 0;
    vector<Float_t>* chi2_max   = 0;
    // After the event loop fill in vector with vN values
    vector<Float_t> v_n_SP(cent_key.size() * pt_key.size());

    chain->SetBranchAddress("run",       &run);
    chain->SetBranchAddress("event",     &event);
    chain->SetBranchAddress("totalMBD",  &totalMBD);
    chain->SetBranchAddress("Q_N_x",     &Q_N_x);
    chain->SetBranchAddress("Q_N_y",     &Q_N_y);
    chain->SetBranchAddress("Q_P_x",     &Q_P_x);
    chain->SetBranchAddress("Q_P_y",     &Q_P_y);
    chain->SetBranchAddress("pi0_mass",  &pi0_mass);
    chain->SetBranchAddress("pi0_pt",    &pi0_pt);
    chain->SetBranchAddress("pi0_phi",   &pi0_phi);
    chain->SetBranchAddress("pi0_eta",   &pi0_eta);
    chain->SetBranchAddress("asym",  &pi0_asym);
    chain->SetBranchAddress("deltaR", &pi0_deltaR);
    chain->SetBranchAddress("ecore_min", &ecore_min);
    chain->SetBranchAddress("chi2_max",  &chi2_max);

    UInt_t max_npi0 = 0;
    
    Long64_t nentries = chain->GetEntries();
    cout << "Events to process: " << nentries << endl;


    // Calculate the entry numbers for 25%, 50%, and 75% completion
    Long64_t entryOneQuarter = nentries / 4;
    Long64_t entryHalf = nentries / 2;
    Long64_t entryThreeQuarters = (nentries * 3) / 4;

    // Loop over each event
    for (Long64_t i = 0; i < nentries; ++i) {
        chain->GetEntry(i);
        
        // Print an update on the processing status at 25%, 50%, and 75%
        if (i == entryOneQuarter) {
            cout << "Processed 25% of events (" << i << " out of " << nentries << ")" << endl;
        } else if (i == entryHalf) {
            cout << "Processed 50% of events (" << i << " out of " << nentries << ")" << endl;
        } else if (i == entryThreeQuarters) {
            cout << "Processed 75% of events (" << i << " out of " << nentries << ")" << endl;
        }
        
        
        Int_t cent_idx = cent_dum_vec->FindBin(totalMBD)-1;

        // check if centrality is found in one of the specified bins
        if(cent_idx < 0 || cent_idx >= 2) continue;

        ++evt_ctr[cent_idx];
        
        /*
         Denominator Quantity
         */
        QQ[cent_idx] += Q_N_x*Q_P_x + Q_N_y*Q_P_y;
        
        UInt_t n = pi0_mass->size();

        max_npi0 = max(max_npi0, n);
        
        // loop over all diphoton candidates
        for(UInt_t j = 0; j < n; ++j) {
            Float_t pi0_pt_val = pi0_pt->at(j);
            Int_t pt_idx = pt_dum_vec->FindBin(pi0_pt_val)-1;
            
            // check if pt is found in one of the specified bins
            if(pt_idx < 0 || pt_idx >= 6) continue;
            
            pair<string,string> key = make_pair(cent_key[cent_idx], pt_key[pt_idx]);
            
            Float_t pi0_mass_val  = pi0_mass->at(j);
            Float_t pi0_phi_val   = pi0_phi->at(j);
            Float_t pi0_eta_val   = pi0_eta->at(j);
            Float_t asym_val      = pi0_asym->at(j);
            Float_t deltaR_val    = pi0_deltaR->at(j);
            Float_t ecore_min_val = ecore_min->at(j);
            Float_t chi2_max_val  = chi2_max->at(j);
            
            /*
             Q value in numerator based on this, the Q^N/P value
             */
            Float_t Q_x = (pi0_eta_val < 0) ? Q_P_x : Q_N_x;
            Float_t Q_y = (pi0_eta_val < 0) ? Q_P_y : Q_N_y;
            
            Float_t q_x = cos(2*pi0_phi_val);
            Float_t q_y = sin(2*pi0_phi_val);
            
            /*
             Numerator Quantity Calculation
             */
            Float_t qQ_val = q_x*Q_x + q_y*Q_y;
            
            Int_t idx = cent_idx*pt_key.size()+pt_idx;
            
            auto bounds = signalBounds[idx];
//            cout << "Bounds for candidate " << j << ": lower = " << bounds.first << ", upper = " << bounds.second << endl;

            if(ecore_min_val >= energy_cut && asym_val < asym_cut && deltaR_val >= delR_cut && chi2_max_val < chi2_cut && pi0_mass_val >= bounds.first && pi0_mass_val < bounds.second) {
                ++pi0_ctr[idx];
                qQ[idx] += qQ_val;
            }
        }
    }
    for (size_t cent = 0; cent < cent_key.size(); ++cent) {
        //cout << "Centrality: " << cent_key[cent] << endl;
        //cout << "QQ[" << cent << "]: " << QQ[cent] << ", evt_ctr[" << cent << "]: " << evt_ctr[cent] << endl;

        if (evt_ctr[cent] == 0) {
            //cout << "Warning: No events processed for this centrality." << endl;
            continue;  // Skip to next centrality if no events
        }

        Float_t QQ_avg = QQ[cent] / evt_ctr[cent];
        Float_t QQ_sqrt = sqrt(QQ_avg);


        for (size_t pt = 0; pt < pt_key.size(); ++pt) {
            size_t idx = cent * pt_key.size() + pt;

            if (pi0_ctr[idx] == 0) {
                continue;  // Skip to next pT range if no candidates
            }

            Float_t qQ_avg = qQ[idx] / pi0_ctr[idx];

            // Compute v_n{SP} and store it
            v_n_SP[idx] = qQ_avg / QQ_sqrt;

            // Print statements for verification
            cout << "Index: " << idx
                 << ", v_n{SP}: " << v_n_SP[idx]
                 << ", pT Range: " << pt_key[pt]
                 << ", MBD Charge Range: " << (cent == 0 ? "40-60%" : "20-40%")
                 << endl;
        }
    }

    
    // After the calculations...
    ofstream outFile("/sphenix/u/patsfan753/v_n_Analysis/updated_fullData.csv");
    ifstream inFile("/sphenix/u/patsfan753/v_n_Analysis/PlotByPlotOutputJamiesUpdatedMasking_edits.csv");

    string line;
    int lineIndex = 0;
    while (getline(inFile, line)) {
        if (lineIndex > 0) { // Skip header line
            stringstream ss(line);
            string token;
            getline(ss, token, ',');
            int idx = stoi(token); // Get the index from the CSV row

            outFile << line << "," << v_n_SP[idx] << endl; // Append v_n{SP} value
        } else {
            outFile << line << ",v_n{SP}" << endl; // Add header for the new column
        }
        ++lineIndex;
    }

    outFile.close();
    inFile.close();

}
