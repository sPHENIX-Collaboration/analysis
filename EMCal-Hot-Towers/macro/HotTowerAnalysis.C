// c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

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
using std::vector;
using std::pair;

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)

namespace myAnalysis {

    Int_t init(const string &input);
    void init_hists();

    void process_event();
    void finalize(const string &i_output);

    TH1F* hBadTowers;
    TH1F* hBadTowersDead;
    TH1F* hBadTowersHot;
    TH1F* hBadTowersCold;

    TH2F* h2BadTowers;
    TH2F* h2BadTowersDead;
    TH2F* h2BadTowersHot;
    TH2F* h2BadTowersCold;

    TH1F* hHotTowerStatus;

    vector<UInt_t> runs;
    recoConsts* rc;

    UInt_t ntowers  = 24576;
    UInt_t bins_phi = 256;
    UInt_t bins_eta = 96;
    UInt_t nStatus  = 4;

    Float_t fraction_badChi2_threshold = 0.01;
    string m_detector = "CEMC";
    string m_calibName_chi2 = m_detector + "_hotTowers_fracBadChi2";
    string m_fieldname_chi2 = "fraction";

    string m_calibName_hotMap = m_detector + "_BadTowerMap";
    string m_fieldname_hotMap = "status";

    Bool_t m_doHotChi2 = false;
}

void myAnalysis::init_hists() {

    hBadTowers   = new TH1F("hBadTowers", "Bad Towers; Tower Index; Runs", ntowers, -0.5, ntowers-0.5);
    hBadTowersDead = new TH1F("hBadTowersDead", "Bad Towers: Dead; Tower Index; Runs", ntowers, -0.5, ntowers-0.5);
    hBadTowersHot = new TH1F("hBadTowersHot", "Bad Towers: Hot; Tower Index; Runs", ntowers, -0.5, ntowers-0.5);
    hBadTowersCold = new TH1F("hBadTowersCold", "Bad Towers: Cold; Tower Index; Runs", ntowers, -0.5, ntowers-0.5);

    h2BadTowers   = new TH2F("h2BadTowers", "Bad Towers; #phi Index; #eta Index", bins_phi, -0.5, bins_phi-0.5, bins_eta, -0.5, bins_eta-0.5);
    h2BadTowersDead = new TH2F("h2BadTowersDead", "Bad Towers: Dead; #phi Index; #eta Index", bins_phi, -0.5, bins_phi-0.5, bins_eta, -0.5, bins_eta-0.5);
    h2BadTowersHot = new TH2F("h2BadTowersHot", "Bad Towers: Hot; #phi Index; #eta Index", bins_phi, -0.5, bins_phi-0.5, bins_eta, -0.5, bins_eta-0.5);
    h2BadTowersCold = new TH2F("h2BadTowersCold", "Bad Towers: Cold; #phi Index; #eta Index", bins_phi, -0.5, bins_phi-0.5, bins_eta, -0.5, bins_eta-0.5);

    hHotTowerStatus = new TH1F("hHotTowerStatus", "Hot Tower Status; Status; Towers", nStatus, -0.5, nStatus-0.5);
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

    for(auto run : runs) {
        cout << "Run: " << run << endl;

        rc->set_uint64Flag("TIMESTAMP", run);

        string calibdir = CDBInterface::instance()->getUrl(m_calibName_hotMap);

        if (calibdir.empty()) {
            cout << "Run: " << run << ", Missing: " << m_calibName_hotMap << endl;
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

        for (UInt_t channel = 0; channel < ntowers; ++channel) {
            UInt_t key    = TowerInfoDefs::encode_emcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            Float_t fraction_badChi2 = 0;
            if(m_doHotChi2) {
                fraction_badChi2 = m_cdbttree_chi2->GetFloatValue(key, m_fieldname_chi2);
            }
            Float_t hotMap_val = m_cdbttree_hotMap->GetIntValue(key, m_fieldname_hotMap);

            if ((m_doHotChi2 && fraction_badChi2 > fraction_badChi2_threshold) || hotMap_val != 0) {
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
                }
                // Cold
                if(hotMap_val == 3) {
                    hBadTowersCold->Fill(channel);
                    h2BadTowersCold->Fill(phibin, etabin);
                }
            }
        }
    }

  // print used DB files
  CDBInterface::instance()->Print();
}

void myAnalysis::finalize(const string &i_output) {
     TFile output(i_output.c_str(),"recreate");
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

     output.Close();
}


void Fun4All_HotTower(const string &input, const string &output = "test.root") {
    cout << "#############################" << endl;
    cout << "Input Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    Int_t ret = myAnalysis::init(input);
    if(ret) return;

    myAnalysis::process_event();
    myAnalysis::finalize(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./hotAna inputFile [outputFile]" << endl;
        cout << "inputFile: containing list of run numbers" << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        return 1;
    }

    string outputFile  = "test.root";

    if(argc >= 3) {
        outputFile = argv[2];
    }

    Fun4All_HotTower(argv[1], outputFile);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
