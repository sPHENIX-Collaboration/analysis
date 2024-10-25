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

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)

namespace myAnalysis {

    Int_t init(const string &input);
    void init_hists();

    void process_event();
    void finalize(const string &i_output, const string &outputMissingHotMap);

    TH1F* hBadTowers;
    TH1F* hBadTowersDead;
    TH1F* hBadTowersHot;
    TH1F* hBadTowersCold;

    TH2F* h2BadTowers;
    TH2F* h2BadTowersDead;
    TH2F* h2BadTowersHot;
    TH2F* h2BadTowersCold;

    TH1F* hHotTowerStatus;

    TH1F* hSigmaGood;
    TH1F* hSigma;
    TH1F* hSigmaFreqHot;

    vector<TH2F*> h2HotTowerFrequency_vec;
    vector<TH1F*> hHotTowerSigma_vec;

    vector<UInt_t> runs;
    vector<UInt_t> runsMissingHotMap;
    recoConsts* rc;

    UInt_t m_ntowers     = 24576;
    UInt_t m_bins_phi    = 256;
    UInt_t m_bins_eta    = 96;
    UInt_t m_bins_status = 2;
    UInt_t m_nStatus     = 4;

    UInt_t m_bins_sigma  = 500;
    Float_t m_sigma_low  = 0;
    Float_t m_sigma_high = 50;

    // run threshold above which towers are considered to be frequently hot
    UInt_t m_threshold;

    Float_t m_fraction_badChi2_threshold = 0.01;
    string m_detector = "CEMC";
    string m_calibName_chi2 = m_detector + "_hotTowers_fracBadChi2";
    string m_fieldname_chi2 = "fraction";

    string m_calibName_hotMap = m_detector + "_BadTowerMap";
    string m_fieldname_hotMap = "status";
    string m_sigma_hotMap     = "CEMC_sigma";

    Bool_t m_doHotChi2 = false;
}

void myAnalysis::init_hists() {

    hBadTowers   = new TH1F("hBadTowers", "Bad Towers; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);
    hBadTowersDead = new TH1F("hBadTowersDead", "Bad Towers: Dead; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);
    hBadTowersHot = new TH1F("hBadTowersHot", "Bad Towers: Hot; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);
    hBadTowersCold = new TH1F("hBadTowersCold", "Bad Towers: Cold; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);

    h2BadTowers   = new TH2F("h2BadTowers", "Bad Towers; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);
    h2BadTowersDead = new TH2F("h2BadTowersDead", "Bad Towers: Dead; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);
    h2BadTowersHot = new TH2F("h2BadTowersHot", "Bad Towers: Hot; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);
    h2BadTowersCold = new TH2F("h2BadTowersCold", "Bad Towers: Cold; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

    hHotTowerStatus = new TH1F("hHotTowerStatus", "Hot Tower Status; Status; Towers", m_nStatus, -0.5, m_nStatus-0.5);

    hSigma        = new TH1F("hSigma", "Towers (Flagged Hot #leq 50% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaGood    = new TH1F("hSigmaGood", "Towers (Flagged Hot for 0% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaFreqHot = new TH1F("hSigmaFreqHot", "Towers (Flagged Hot > 50% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);

    stringstream name, title;
    for (UInt_t i = 0; i < m_ntowers; ++i) {
      UInt_t key = TowerInfoDefs::encode_emcal(i);
      UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
      UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);
      name.str("");
      title.str("");
      name << "h2HotTowerFrequency_" << phibin << "_" << etabin;
      title << "Hot Tower Run Frequency: (" << phibin << "," << etabin << "); Run; isHot";

      auto h2 = new TH2F(name.str().c_str(), title.str().c_str(), runs.size(), 0, runs.size(), m_bins_status, -0.5, m_bins_status - 0.5);

      h2HotTowerFrequency_vec.push_back(h2);

      name.str("");
      title.str("");

      name << "hHotTowerSigma_" << phibin << "_" << etabin;
      title << "Hot Tower Sigma: (" << phibin << "," << etabin << "); sigma; Runs";

      auto h1 = new TH1F(name.str().c_str(), title.str().c_str(), m_bins_sigma, m_sigma_low, m_sigma_high);

      hHotTowerSigma_vec.push_back(h1);
    }
}

Int_t myAnalysis::init(const string &input) {
    rc = recoConsts::instance();
    rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
    CDBInterface::instance()->Verbosity(0);

    std::ifstream file(input);

    // Check if the file was successfully opened
    if (!file.is_open()) {
        cerr << "Failed to open cuts file: " << input << endl;
        return 1;
    }

    string line;

    // loop over each run
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);

        UInt_t runnumber;

        if (lineStream >> runnumber) {
            runs.push_back(runnumber);
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

void myAnalysis::process_event() {

    UInt_t bin_run = 1;
    Float_t sigma_min = 9999;
    Float_t sigma_max = 0;
    std::unordered_set<UInt_t> badSigma;

    for(auto run : runs) {
        cout << "Run: " << run << endl;

        rc->set_uint64Flag("TIMESTAMP", run);

        string calibdir = CDBInterface::instance()->getUrl(m_calibName_hotMap);

        if (calibdir.empty()) {
            cout << "Run: " << run << ", Missing: " << m_calibName_hotMap << endl;
            runsMissingHotMap.push_back(run);
            ++bin_run;
            continue;
        }

        CDBTTree* m_cdbttree_hotMap = new CDBTTree(calibdir);

        CDBTTree* m_cdbttree_chi2;

        if(m_doHotChi2) {
            calibdir = CDBInterface::instance()->getUrl(m_calibName_chi2);

            if(!calibdir.empty()) m_cdbttree_chi2 = new CDBTTree(calibdir);
            else {
                m_doHotChi2 = false;
                cout << "Run: " << run << ", Missing: " << m_calibName_chi2 << endl;
            }
        }

        Bool_t hasBadSigma = false;

        for (UInt_t channel = 0; channel < m_ntowers; ++channel) {
            UInt_t key    = TowerInfoDefs::encode_emcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            Float_t fraction_badChi2 = 0;
            if(m_doHotChi2) {
                fraction_badChi2 = m_cdbttree_chi2->GetFloatValue(key, m_fieldname_chi2);
            }
            Int_t hotMap_val = m_cdbttree_hotMap->GetIntValue(key, m_fieldname_hotMap);
            Float_t sigma_val = 0;
            if(!hasBadSigma) sigma_val = m_cdbttree_hotMap->GetFloatValue(key, m_sigma_hotMap);
            if(std::isnan(sigma_val) || std::isinf(sigma_val)) {
              hasBadSigma = true;
              badSigma.insert(run);
            }

            if(!hasBadSigma) hHotTowerSigma_vec[channel]->Fill(sigma_val);

            if ((m_doHotChi2 && fraction_badChi2 > m_fraction_badChi2_threshold) || hotMap_val != 0) {
                hBadTowers->Fill(channel);
                h2BadTowers->Fill(phibin, etabin);
                hHotTowerStatus->Fill(hotMap_val);

                // Dead
                if(hotMap_val == 1) {
                    hBadTowersDead->Fill(channel);
                    h2BadTowersDead->Fill(phibin, etabin);
                }
                // Hot
                if(hotMap_val == 2) {
                    hBadTowersHot->Fill(channel);
                    h2BadTowersHot->Fill(phibin, etabin);
                    h2HotTowerFrequency_vec[channel]->SetBinContent(bin_run,2,1);

                    if(!hasBadSigma) {
                      sigma_min = min(sigma_min, sigma_val);
                      if(sigma_val < 100) sigma_max = max(sigma_max, sigma_val);
                    }
                }
                // Cold
                if(hotMap_val == 3) {
                    hBadTowersCold->Fill(channel);
                    h2BadTowersCold->Fill(phibin, etabin);
                }
            }

            if(hotMap_val != 2) {
                h2HotTowerFrequency_vec[channel]->SetBinContent(bin_run,1,1);
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

  UInt_t runs_processed = runs.size()-runsMissingHotMap.size();
  cout << "Sigma Min: " << sigma_min << ", Sigma Max: " << sigma_max << endl;
  cout << "Runs with bad sigma (nan or inf): " << badSigma.size() << endl;
  cout << "Runs Missing HotMap: " << runsMissingHotMap.size() << endl;
  cout << "Runs Processed: " << runs_processed << ", " << runs_processed*100./runs.size() << " %" << endl;
}

void myAnalysis::finalize(const string &i_output, const string &outputMissingHotMap) {

  m_threshold = (runs.size() - runsMissingHotMap.size()) / 2;
  cout << "threshold: " << m_threshold << endl;

  ofstream file(outputMissingHotMap);

  for(auto run : runsMissingHotMap) file << run << endl;

  file.close();

  TFile output(i_output.c_str(), "recreate");
  output.cd();

  hBadTowers->Write();
  hBadTowersDead->Write();
  hBadTowersHot->Write();
  hBadTowersCold->Write();

  h2BadTowers->Write();
  h2BadTowersDead->Write();
  h2BadTowersHot->Write();
  h2BadTowersCold->Write();

  hHotTowerStatus->Write();

  output.mkdir("h2HotTowerFrequency");
  output.mkdir("hHotTowerSigma");

  UInt_t ctr[2] = {0};
  for (UInt_t i = 0; i < m_ntowers; ++i) {
        if (hBadTowersHot->GetBinContent(i + 1)) ++ctr[0];
        else hSigmaGood->Add(hHotTowerSigma_vec[i]);

        if (hBadTowersHot->GetBinContent(i + 1) >= m_threshold) {
            for (UInt_t j = 1; j <= runs.size(); ++j) {
                h2HotTowerFrequency_vec[i]->GetYaxis()->SetBinLabel(1, "0");
                h2HotTowerFrequency_vec[i]->GetYaxis()->SetBinLabel(2, "1");
            }
            output.cd("h2HotTowerFrequency");
            h2HotTowerFrequency_vec[i]->Write();

            output.cd("hHotTowerSigma");
            hHotTowerSigma_vec[i]->Write();

            hSigmaFreqHot->Add(hHotTowerSigma_vec[i]);
            ++ctr[1];
        }
        else hSigma->Add(hHotTowerSigma_vec[i]);
  }

  output.cd();
  hSigmaGood->Write();
  hSigma->Write();
  hSigmaFreqHot->Write();

  cout << "Towers flagged hot: " << ctr[0] << ", " << ctr[0]*100./m_ntowers << " %" << endl;
  cout << "Towers flagged hot more than 50% of runs: " << ctr[1] << ", " << ctr[1]*100./m_ntowers << " %" << endl;

  output.Close();
}

void Fun4All_HotTower(const string &input, const string &output = "test.root", const string &outputMissingHotMap = "missingHotMap.csv") {
    cout << "#############################" << endl;
    cout << "Input Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "outputMissingHotMap: " << outputMissingHotMap << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(input);
    if(ret) return;

    myAnalysis::process_event();
    myAnalysis::finalize(output, outputMissingHotMap);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 4) {
        cout << "usage: ./hotAna inputFile [outputFile], [outputMissingHotMap]" << endl;
        cout << "inputFile: containing list of run numbers" << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "outputMissingHotMap: location of outputMissingHotMap file. Default: missingHotMap.csv." << endl;
        return 1;
    }

    string outputFile          = "test.root";
    string outputMissingHotMap = "missingHotMap.csv";

    if(argc >= 3) {
        outputFile = argv[2];
    }
    if(argc >= 4) {
        outputMissingHotMap = argv[3];
    }

    Fun4All_HotTower(argv[1], outputFile, outputMissingHotMap);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
