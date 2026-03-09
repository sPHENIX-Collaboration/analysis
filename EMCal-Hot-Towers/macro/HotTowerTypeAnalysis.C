// c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_set>

// root includes --
#include <TSystem.h>
#include <TROOT.h>
#include <TH2F.h>
#include <TFile.h>
#include <TMath.h>
#include <TDatime.h>
#include <TGraph.h>

#include <calobase/TowerInfoDefs.h>
#include <cdbobjects/CDBTTree.h>
#include <ffamodules/CDBInterface.h>
#include <phool/recoConsts.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::ofstream;
using std::vector;
using std::pair;
using std::min;
using std::max;
using std::unordered_set;

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)

namespace myAnalysis {

    Int_t init(const string &input);
    void init_hists();
    Int_t readFile(const string &input);

    Int_t process_event();
    void finalize(const string &i_output);

    TH1F* hBadTowersHot;

    TH1F* hSigmaNeverHot;
    TH1F* hSigmaHot;
    TH1F* hSigmaFreqHot;
    TH1F* hSigmaTypeA;
    TH1F* hSigmaTypeB;
    TH1F* hSigmaTypeC;

    vector<TH1F*> hHotTowerSigma_vec;

    vector<pair<UInt_t,string>> runs;
    vector<UInt_t> runsMissingHotMap;
    recoConsts* rc;

    UInt_t m_ntowers     = 24576;
    UInt_t m_bins_phi    = 256;
    UInt_t m_bins_eta    = 96;

    UInt_t m_bins_sigma  = 1000;
    Float_t m_sigma_low  = -50;
    Float_t m_sigma_high = 50;

    Int_t m_maxRuns = 0;  // default process all

    // run threshold above which towers are considered to be frequently hot
    UInt_t m_threshold = 400;

    string m_detector = "CEMC";
    // string m_detector = "HCALIN";
    // string m_detector = "HCALOUT";

    string m_calibName_hotMap = m_detector + "_BadTowerMap";
    string m_fieldname_hotMap = "status";
    string m_sigma_hotMap     = m_detector + "_sigma";

    unordered_set<UInt_t> hotTypeA;
    unordered_set<UInt_t> hotTypeB;
    unordered_set<UInt_t> hotTypeC;
}

void myAnalysis::init_hists() {

    hBadTowersHot = new TH1F("hBadTowersHot", "Bad Towers: Hot; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);

    hSigmaHot      = new TH1F("hSigmaHot", "Towers (Flagged Hot #leq 50% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaNeverHot = new TH1F("hSigmaNeverHot", "Towers (Flagged Hot for 0% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaFreqHot  = new TH1F("hSigmaFreqHot", "Towers (Flagged Hot > 50% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaTypeA    = new TH1F("hSigmaTypeA", "Towers (Type A); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaTypeB    = new TH1F("hSigmaTypeB", "Towers (Type B); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaTypeC    = new TH1F("hSigmaTypeC", "Towers (Type C); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);

    stringstream name, title;
    for (UInt_t i = 0; i < m_ntowers; ++i) {
      UInt_t key = (m_detector == "CEMC") ? TowerInfoDefs::encode_emcal(i) : TowerInfoDefs::encode_hcal(i);
      UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
      UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);
      name.str("");
      title.str("");

      name << "hHotTowerSigma_" << phibin << "_" << etabin;
      title << "Hot Tower Sigma: (" << phibin << "," << etabin << "); sigma; Runs";

      auto h = new TH1F(name.str().c_str(), title.str().c_str(), m_bins_sigma, m_sigma_low, m_sigma_high);

      hHotTowerSigma_vec.push_back(h);
    }
}

Int_t myAnalysis::init(const string &input) {
    rc = recoConsts::instance();
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    CDBInterface::instance()->Verbosity(0);

    std::ifstream file(input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open file: " << input << endl;
        return 1;
    }

    string line;

    // loop over each run
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        UInt_t runnumber;
        string timestamp;
        string date;
        string time;
        char comma;

        if (lineStream >> runnumber >> comma >> date >> time) {
            timestamp = date+" "+time;
            runs.push_back(make_pair(runnumber,timestamp));
        }
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
    }

    // Close the file
    file.close();

    init_hists();

    return 0;
}

Int_t myAnalysis::readFile(const string &input) {
    std::ifstream file(input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open file: " << input << endl;
        return 1;
    }

    string line;

    // skip the header
    std::getline(file, line);

    // loop over each run
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        char hotType;
        UInt_t phibin;
        UInt_t etabin;
        UInt_t towerIndex;
        char comma;

        if (lineStream >> hotType >> comma >> phibin >> comma >> etabin) {
            towerIndex = (m_detector == "CEMC") ? TowerInfoDefs::decode_emcal(TowerInfoDefs::encode_emcal(etabin, phibin)) :
                                                  TowerInfoDefs::decode_hcal(TowerInfoDefs::encode_hcal(etabin, phibin));

            if(hotType == 'A')      hotTypeA.insert(towerIndex);
            else if(hotType == 'B') hotTypeB.insert(towerIndex);
            else if(hotType == 'C') hotTypeC.insert(towerIndex);
            else {
                cout << "ERROR: Unknown Hot Tower Type: " << hotType << endl;
                return 2;
            }
            cout << "Hot Type: " << hotType << ", iphi: " << phibin << ", ieta: " << etabin << ", towerIndex: " << towerIndex << endl;
        }
        else {
            cerr << "Failed to parse line: " << line << endl;
            return 1;
        }
    }

    // Close the file
    file.close();

    return 0;
}

Int_t myAnalysis::process_event() {

    UInt_t bin_run = 1;
    Float_t sigma_min = 9999;
    Float_t sigma_max = 0;
    UInt_t ctr_overwrite = 0;
    unordered_set<UInt_t> badSigma;

    for(auto p : runs) {
        if (m_maxRuns && bin_run > m_maxRuns) break;

        UInt_t run = p.first;
        string timestamp = p.second;
        TDatime d;

        cout << "Run: " << run << ", Timestamp: " << timestamp << endl;

        rc->set_uint64Flag("TIMESTAMP", run);

        Bool_t hasHotMap = true;
        string calibdir = CDBInterface::instance()->getUrl(m_calibName_hotMap);

        std::unique_ptr<CDBTTree> m_cdbttree_hotMap = nullptr;

        if (!calibdir.empty()) {
          m_cdbttree_hotMap = std::make_unique<CDBTTree>(calibdir);
        }
        else {
            cout << "Run: " << run << ", Missing: " << m_calibName_hotMap << endl;
            runsMissingHotMap.push_back(run);
            hasHotMap = false;
        }

        Bool_t hasBadSigma = false;
        UInt_t acceptance = 0;
        UInt_t bad_ctr[4] = {0};

        for (UInt_t channel = 0; channel < m_ntowers; ++channel) {
            UInt_t key = (m_detector == "CEMC") ? TowerInfoDefs::encode_emcal(channel) : TowerInfoDefs::encode_hcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            Int_t hotMap_val = 0;
            if (hasHotMap) {
                hotMap_val = m_cdbttree_hotMap->GetIntValue(key, m_fieldname_hotMap);
            }

            // ensure hotMap_val is within range
            // 0: Good
            // 1: Dead
            // 2: Hot
            // 3: Cold
            if(hotMap_val > 3) {
              cout << "ERROR: Unknown hotMap value: " << hotMap_val << endl;
              return 1;
            }

            Float_t sigma_val = 0;
            if(!hasBadSigma && hasHotMap) {
              sigma_val = m_cdbttree_hotMap->GetFloatValue(key, m_sigma_hotMap);
              if(std::isnan(sigma_val) || std::isinf(sigma_val)) {
                cout << "WARNING: sigma does not exist for channel: " << channel << ", key: " << key << endl;
                hasBadSigma = true;
                badSigma.insert(run);
              }
              else {
                hHotTowerSigma_vec[channel]->Fill(sigma_val);
              }
            }

            if (hasHotMap && hotMap_val != 0) {
                // Hot
                if(hotMap_val == 2) {
                    hBadTowersHot->Fill(channel);
                    if(!hasBadSigma) {
                      sigma_min = min(sigma_min, sigma_val);
                      if(sigma_val < 100) sigma_max = max(sigma_max, sigma_val);
                    }
                }
            }
        }
        ++bin_run;
    }

    cout << "###################" << endl;
    cout << "Runs with Bad Sigma" << endl;
    for(auto run : badSigma) {
      cout << "Run: " << run << endl;
    }

    cout << "###################" << endl;

  // print used DB files
  CDBInterface::instance()->Print();

  UInt_t runs_processed = bin_run-runsMissingHotMap.size()-1;
  cout << "Sigma Min: " << sigma_min << ", Sigma Max: " << sigma_max << endl;
  cout << "Runs with bad sigma (nan or inf): " << badSigma.size() << endl;
  cout << "Runs Missing HotMap: " << runsMissingHotMap.size() << endl;
  cout << "Runs Processed: " << runs_processed << ", " << runs_processed*100./(bin_run-1) << " %" << endl;

  return 0;
}

void myAnalysis::finalize(const string &i_output) {

  m_threshold = min(m_threshold, (UInt_t)(runs.size() - runsMissingHotMap.size()) / 2);
  if(m_maxRuns) m_threshold = min(m_threshold, (UInt_t)(m_maxRuns - runsMissingHotMap.size()) / 2);
  cout << "threshold: " << m_threshold << endl;

  TFile output(i_output.c_str(), "recreate");
  output.cd();

  for (UInt_t i = 0; i < m_ntowers; ++i) {
        if (hBadTowersHot->GetBinContent(i + 1) >= m_threshold) {
            hSigmaFreqHot->Add(hHotTowerSigma_vec[i]);

            if(hotTypeA.find(i) != hotTypeA.end()) hSigmaTypeA->Add(hHotTowerSigma_vec[i]);
            if(hotTypeB.find(i) != hotTypeB.end()) hSigmaTypeB->Add(hHotTowerSigma_vec[i]);
            if(hotTypeC.find(i) != hotTypeC.end()) hSigmaTypeC->Add(hHotTowerSigma_vec[i]);
        }
        else hSigmaHot->Add(hHotTowerSigma_vec[i]);
  }

  output.cd();
  hSigmaNeverHot->Write();
  hSigmaHot->Write();
  hSigmaFreqHot->Write();
  hSigmaTypeA->Write();
  hSigmaTypeB->Write();
  hSigmaTypeC->Write();

  output.Close();
}

void HotTowerTypeAnalysis(const string &input,
                          const string &hotTypeList,
                          const string &output = "hotType.root",
                          const Int_t maxRuns = 0) {

    cout << "#############################" << endl;
    cout << "Input Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "Hot Type List: " << hotTypeList << endl;
    cout << "output: " << output << endl;

    myAnalysis::m_maxRuns = maxRuns;

    if (myAnalysis::m_detector == "HCALIN" || myAnalysis::m_detector == "HCALOUT") {
      myAnalysis::m_bins_phi = 64;
      myAnalysis::m_bins_eta = 24;
      myAnalysis::m_ntowers = myAnalysis::m_bins_phi * myAnalysis::m_bins_eta;
    }

    cout << "Detector: " << myAnalysis::m_detector << endl;

    Int_t ret = myAnalysis::init(input);
    if(ret) return;

    ret = myAnalysis::readFile(hotTypeList);
    if(ret) return;

    if(maxRuns) cout << "Processing: " << min((Int_t) myAnalysis::runs.size(), maxRuns) << " Runs" << endl;
    else        cout << "Processing: " << myAnalysis::runs.size() << " Runs" << endl;
    cout << "#############################" << endl;

    ret = myAnalysis::process_event();
    if(ret) return;

    myAnalysis::finalize(output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    std::quick_exit(0);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 3 || argc > 5) {
        cout << "usage: ./hotAna inputFile hotTypeList [outputFile] [maxRuns]" << endl;
        cout << "inputFile: containing list of run numbers" << endl;
        cout << "Hot Type List: hot tower list by types." << endl;
        cout << "outputFile: location of output file. Default: hotType.root." << endl;
        cout << "maxRuns: Max runs to process (useful when testing). Default: 0 (means run all)." << endl;
        return 1;
    }

    string outputFile = "hotType.root";
    Int_t maxRuns     = 0;

    if (argc >= 4) {
        outputFile = argv[3];
    }
    if (argc >= 5) {
        maxRuns = atoi(argv[4]);
    }

    HotTowerTypeAnalysis(argv[1], argv[2], outputFile, maxRuns);

    return 0;
}
# endif
