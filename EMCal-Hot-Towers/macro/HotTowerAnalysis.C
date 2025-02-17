// c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_set>
#include <filesystem>

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

namespace fs = std::filesystem;

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libphool.so)

namespace myAnalysis {

    Int_t init(const string &input);
    void init_hists();

    Int_t process_event(const string &outputRunStats);
    void finalize(const string &i_output, const string &outputMissingHotMap, const string &outputMissingBadChi2);

    TH1F* hBadTowers;
    TH1F* hBadTowersDead;
    TH1F* hBadTowersHot;
    TH1F* hBadTowersHotChi2;
    TH1F* hBadTowersCold;

    TH2F* h2BadTowers;
    TH2F* h2BadTowersDead;
    TH2F* h2BadTowersHot;
    TH2F* h2BadTowersHotChi2;
    TH2F* h2BadTowersCold;

    TH1F* hHotTowerStatus;

    TH1F* hSigmaNeverHot;
    TH1F* hSigmaHot;
    TH1F* hSigmaFreqHot;

    TH1F* hAcceptance;
    TH1F* hFracHot;
    TH1F* hFracDead;
    TH1F* hFracCold;
    TH1F* hFracBadChi2;

    TH1F* hAcceptanceVsTime;
    TH1F* hHotVsTime;
    TH1F* hDeadVsTime;
    TH1F* hColdVsTime;
    TH1F* hBadChi2VsTime;

    TH1F* hRunStatus;

    vector<TH2F*> h2HotTowerFrequency_vec;
    vector<TH1F*> hHotTowerSigma_vec;

    vector<pair<UInt_t,string>> runs;
    vector<UInt_t> runsMissingHotMap;
    vector<UInt_t> runsMissingBadChi2;
    recoConsts* rc;

    UInt_t m_ntowers     = 24576;
    UInt_t m_bins_phi    = 256;
    UInt_t m_bins_eta    = 96;
    UInt_t m_bins_status = 2;
    UInt_t m_nStatus     = 5;

    UInt_t m_bins_sigma  = 1000;
    Float_t m_sigma_low  = -50;
    Float_t m_sigma_high = 50;

    UInt_t m_bins_acceptance = 101;
    UInt_t m_bins_time       = 50000;
    Int_t m_maxRuns = 0;  // default process all

    // run threshold above which towers are considered to be frequently hot
    UInt_t m_threshold = 400;
    UInt_t m_threshold_low = 100;

    Float_t m_fraction_badChi2_threshold = 0.01;
    string m_detector = "CEMC";
    // string m_detector = "HCALIN";
    // string m_detector = "HCALOUT";
    string m_calibName_chi2 = m_detector + "_hotTowers_fracBadChi2";
    string m_fieldname_chi2 = "fraction";

    string m_calibName_hotMap = m_detector + "_BadTowerMap";
    string m_fieldname_hotMap = "status";
    string m_sigma_hotMap     = m_detector + "_sigma";

    Bool_t m_doHotChi2 = true;
    Bool_t m_doTime    = true;
}

void myAnalysis::init_hists() {

    hBadTowers   = new TH1F("hBadTowers", "Bad Towers; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);
    hBadTowersDead = new TH1F("hBadTowersDead", "Bad Towers: Dead; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);
    hBadTowersHot = new TH1F("hBadTowersHot", "Bad Towers: Hot; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);
    hBadTowersHotChi2 = new TH1F("hBadTowersHotChi2", "Bad Towers: Hot Chi2; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);
    hBadTowersCold = new TH1F("hBadTowersCold", "Bad Towers: Cold; Tower Index; Runs", m_ntowers, -0.5, m_ntowers-0.5);

    h2BadTowers   = new TH2F("h2BadTowers", "Bad Towers; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);
    h2BadTowersDead = new TH2F("h2BadTowersDead", "Bad Towers: Dead; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);
    h2BadTowersHot = new TH2F("h2BadTowersHot", "Bad Towers: Hot; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);
    h2BadTowersHotChi2 = new TH2F("h2BadTowersHotChi2", "Bad Towers: Hot Chi2; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);
    h2BadTowersCold = new TH2F("h2BadTowersCold", "Bad Towers: Cold; #phi Index; #eta Index", m_bins_phi, -0.5, m_bins_phi-0.5, m_bins_eta, -0.5, m_bins_eta-0.5);

    hHotTowerStatus = new TH1F("hHotTowerStatus", "Hot Tower Status; Status; Towers", m_nStatus, -0.5, m_nStatus-0.5);

    hSigmaHot        = new TH1F("hSigmaHot", "Towers (Flagged Hot #leq 50% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaNeverHot    = new TH1F("hSigmaNeverHot", "Towers (Flagged Hot for 0% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);
    hSigmaFreqHot = new TH1F("hSigmaFreqHot", "Towers (Flagged Hot > 50% of Runs); sigma; Counts", m_bins_sigma, m_sigma_low, m_sigma_high);

    hAcceptance  = new TH1F("hAcceptance", "Acceptance; Acceptance [%]; Runs", m_bins_acceptance, 0, m_bins_acceptance);
    hFracHot     = new TH1F("hFracHot", "Fraction of Hot Towers; Hot Towers [%]; Runs", m_bins_acceptance, 0, m_bins_acceptance);
    hFracDead    = new TH1F("hFracDead", "Fraction of Dead Towers; Dead Towers [%]; Runs", m_bins_acceptance, 0, m_bins_acceptance);
    hFracCold    = new TH1F("hFracCold", "Fraction of Cold Towers; Cold Towers [%]; Runs", m_bins_acceptance, 0, m_bins_acceptance);
    hFracBadChi2 = new TH1F("hFracBadChi2", "Fraction of Bad Chi2 Towers; Bad Chi2 Towers [%]; Runs", m_bins_acceptance, 0, m_bins_acceptance);

    hRunStatus = new TH1F("hRunStatus", "Run Status; Status; Runs", m_bins_status, 0, m_bins_status);

    if(m_doTime) {
      TDatime d(runs[0].second.c_str());
      UInt_t start = d.Convert();

      d.Set(runs[runs.size()-1].second.c_str());
      UInt_t end = d.Convert();

      hAcceptanceVsTime = new TH1F("hAcceptanceVsTime", "Acceptance; Time; Acceptance [%]", m_bins_time, start, end);
      hAcceptanceVsTime->SetTitle("Acceptance; Time; Acceptance [%]");
      hAcceptanceVsTime->GetXaxis()->SetTimeDisplay(1);  // The X axis is a time axis
      hAcceptanceVsTime->GetXaxis()->SetTimeFormat("%m/%d");
      // hAcceptanceVsTime->GetXaxis()->SetTimeFormat("%m/%d %H");

      hHotVsTime = new TH1F("hHotVsTime", "Hot; Time; Hot Towers [%]", m_bins_time, start, end);
      hHotVsTime->GetXaxis()->SetTimeDisplay(1);  // The X axis is a time axis
      hHotVsTime->GetXaxis()->SetTimeFormat("%m/%d");

      hDeadVsTime = new TH1F("hDeadVsTime", "Dead; Time; Dead Towers [%]", m_bins_time, start, end);
      hDeadVsTime->GetXaxis()->SetTimeDisplay(1);  // The X axis is a time axis
      hDeadVsTime->GetXaxis()->SetTimeFormat("%m/%d");

      hColdVsTime = new TH1F("hColdVsTime", "Cold; Time; Cold Towers [%]", m_bins_time, start, end);
      hColdVsTime->GetXaxis()->SetTimeDisplay(1);  // The X axis is a time axis
      hColdVsTime->GetXaxis()->SetTimeFormat("%m/%d");

      hBadChi2VsTime = new TH1F("hBadChi2VsTime", "BadChi2; Time; Bad Chi2 Towers [%]", m_bins_time, start, end);
      hBadChi2VsTime->GetXaxis()->SetTimeDisplay(1);  // The X axis is a time axis
      hBadChi2VsTime->GetXaxis()->SetTimeFormat("%m/%d");
    }

    stringstream name, title;
    for (UInt_t i = 0; i < m_ntowers; ++i) {
      UInt_t key = (m_detector == "CEMC") ? TowerInfoDefs::encode_emcal(i) : TowerInfoDefs::encode_hcal(i);
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

        if (m_doTime && lineStream >> runnumber >> comma >> date >> time) {
            timestamp = date+" "+time;
            runs.push_back(make_pair(runnumber,timestamp));
        }
        else if (lineStream >> runnumber) {
            timestamp = "";
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

Int_t myAnalysis::process_event(const string &outputRunStats) {

    UInt_t bin_run = 1;
    Float_t sigma_min = 9999;
    Float_t sigma_max = 0;
    UInt_t ctr_overwrite = 0;
    std::unordered_set<UInt_t> badSigma;

    ofstream file(outputRunStats);
    file << "Run,Acceptance,Dead,Hot,Cold,BadChi2" << endl;

    for(auto p : runs) {
        if (m_maxRuns && bin_run > m_maxRuns) break;

        UInt_t run = p.first;
        string timestamp = p.second;
        TDatime d;

        if(m_doTime) {
          d.Set(timestamp.c_str());
          cout << "Run: " << run << ", Timestamp: " << timestamp << endl;
        }
        else cout << "Run: " << run << endl;

        rc->set_uint64Flag("TIMESTAMP", run);

        Bool_t hasHotMap = true;
        string calibdir = CDBInterface::instance()->getUrl(m_calibName_hotMap);

        std::unique_ptr<CDBTTree> m_cdbttree_hotMap = nullptr;

        if (!calibdir.empty()) {
          m_cdbttree_hotMap = std::make_unique<CDBTTree>(calibdir);
          hRunStatus->Fill(0);
        }
        else {
            cout << "Run: " << run << ", Missing: " << m_calibName_hotMap << endl;
            runsMissingHotMap.push_back(run);
            hasHotMap = false;
        }

        std::unique_ptr<CDBTTree> m_cdbttree_chi2 = nullptr;
        Bool_t hasHotChi2 = m_doHotChi2;

        if(m_doHotChi2) {
            calibdir = CDBInterface::instance()->getUrl(m_calibName_chi2);

            if(!calibdir.empty()) {
              m_cdbttree_chi2 = std::make_unique<CDBTTree>(calibdir);
              hRunStatus->Fill(1);
            }
            else {
              cout << "Run: " << run << ", Missing: " << m_calibName_chi2 << endl;
              runsMissingBadChi2.push_back(run);
              hasHotChi2 = false;
            }
        }

        Bool_t hasBadSigma = false;
        UInt_t acceptance = 0;
        UInt_t bad_ctr[4] = {0};

        for (UInt_t channel = 0; channel < m_ntowers; ++channel) {
            UInt_t key = (m_detector == "CEMC") ? TowerInfoDefs::encode_emcal(channel) : TowerInfoDefs::encode_hcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            Float_t fraction_badChi2 = 0;
            if(hasHotChi2) {
                fraction_badChi2 = m_cdbttree_chi2->GetFloatValue(key, m_fieldname_chi2);
            }

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
                hBadTowers->Fill(channel);
                h2BadTowers->Fill(phibin, etabin);
                hHotTowerStatus->Fill(hotMap_val);

                // Dead
                if(hotMap_val == 1) {
                    hBadTowersDead->Fill(channel);
                    h2BadTowersDead->Fill(phibin, etabin);
                    ++bad_ctr[0];
                }
                // Hot
                if(hotMap_val == 2) {
                    hBadTowersHot->Fill(channel);
                    h2BadTowersHot->Fill(phibin, etabin);
                    h2HotTowerFrequency_vec[channel]->SetBinContent(bin_run,2,1);
                    ++bad_ctr[1];

                    if(!hasBadSigma) {
                      sigma_min = min(sigma_min, sigma_val);
                      if(sigma_val < 100) sigma_max = max(sigma_max, sigma_val);
                    }
                }
                // Cold
                if(hotMap_val == 3) {
                    hBadTowersCold->Fill(channel);
                    h2BadTowersCold->Fill(phibin, etabin);
                    ++bad_ctr[2];
                }
            }
            // Normal Status but Hot due to badChi2
            else if (hasHotChi2 && fraction_badChi2 > m_fraction_badChi2_threshold) {
                hotMap_val = 4; // assign a separate status for Hot due to badChi2

                hBadTowers->Fill(channel);
                h2BadTowers->Fill(phibin, etabin);

                hBadTowersHotChi2->Fill(channel);
                h2BadTowersHotChi2->Fill(phibin, etabin);

                hHotTowerStatus->Fill(hotMap_val);
                ++bad_ctr[3];
            }

            if(hasHotMap && hotMap_val == 0) {
                h2HotTowerFrequency_vec[channel]->SetBinContent(bin_run,1,1);
                ++acceptance;
            }
        }

        if(hasHotMap) {
          stringstream s;
          Float_t acc = (Int_t)(acceptance*1e4/m_ntowers)/100.;
          hAcceptance->Fill(acc);

          Float_t accDead = (Int_t)(bad_ctr[0]*1e4/m_ntowers)/100.;
          hFracDead->Fill(accDead);

          Float_t accHot = (Int_t)(bad_ctr[1]*1e4/m_ntowers)/100.;
          hFracHot->Fill(accHot);

          Float_t accCold = (Int_t)(bad_ctr[2]*1e4/m_ntowers)/100.;
          hFracCold->Fill(accCold);

          Float_t accBadChi2 = (Int_t)(bad_ctr[3]*1e4/m_ntowers)/100.;
          hFracBadChi2->Fill(accBadChi2);

          if(m_doTime) {
            Int_t bin_time = hAcceptanceVsTime->FindBin(d.Convert());
            if(hAcceptanceVsTime->GetBinContent(bin_time)) {
               cout << "WARNING: Bin Overwrite" << endl;
               ++ctr_overwrite;
            }
            hAcceptanceVsTime->SetBinContent(bin_time, acc);

            hHotVsTime->SetBinContent(bin_time, accHot);
            hDeadVsTime->SetBinContent(bin_time, accDead);
            hColdVsTime->SetBinContent(bin_time, accCold);
            hBadChi2VsTime->SetBinContent(bin_time, accBadChi2);
          }
          file << run << "," << acc
                      << "," << accDead
                      << "," << accHot
                      << "," << accCold
                      << "," << accBadChi2
                             << endl;
        }

        ++bin_run;
    }

    file.close();

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
  cout << "Runs Missing Bad Chi2: " << runsMissingBadChi2.size() << endl;
  cout << "Runs with overlap time bin: " << ctr_overwrite << endl;
  cout << "Runs Processed: " << runs_processed << ", " << runs_processed*100./(bin_run-1) << " %" << endl;

  return 0;
}

void myAnalysis::finalize(const string &i_output, const string &outputMissingHotMap, const string &outputMissingBadChi2) {

  m_threshold = min(m_threshold, (UInt_t)(runs.size() - runsMissingHotMap.size()) / 2);
  if(m_maxRuns) m_threshold = min(m_threshold, (UInt_t)(m_maxRuns - runsMissingHotMap.size()) / 2);
  cout << "threshold: " << m_threshold << endl;

  ofstream file(outputMissingHotMap);

  for(auto run : runsMissingHotMap) file << run << endl;

  file.close();

  file.open(outputMissingBadChi2);

  for(auto run : runsMissingBadChi2) file << run << endl;

  file.close();

  TFile output(i_output.c_str(), "recreate");
  output.cd();

  hRunStatus->Write();
  hBadTowers->Write();
  hBadTowersDead->Write();
  hBadTowersHot->Write();
  hBadTowersCold->Write();
  hBadTowersHotChi2->Write();

  h2BadTowers->Write();
  h2BadTowersDead->Write();
  h2BadTowersHot->Write();
  h2BadTowersCold->Write();
  h2BadTowersHotChi2->Write();

  hHotTowerStatus->Write();

  hAcceptance->Write();
  hFracDead->Write();
  hFracHot->Write();
  hFracCold->Write();
  hFracBadChi2->Write();
  if(m_doTime) {
    hAcceptanceVsTime->Write();
    hDeadVsTime->Write();
    hHotVsTime->Write();
    hColdVsTime->Write();
    hBadChi2VsTime->Write();
  }

  output.mkdir("h2HotTowerFrequency");
  output.mkdir("hHotTowerSigma");
  output.mkdir("hHotTowerSigmaLessFreq");

  UInt_t ctr[5] = {0};
  UInt_t ctr_freq[5] = {0};
  for (UInt_t i = 0; i < m_ntowers; ++i) {
        if (hBadTowersHot->GetBinContent(i + 1))     ++ctr[0];
        else hSigmaNeverHot->Add(hHotTowerSigma_vec[i]);

        if (hBadTowersDead->GetBinContent(i + 1))    ++ctr[1];
        if (hBadTowersCold->GetBinContent(i + 1))    ++ctr[2];
        if (hBadTowersHotChi2->GetBinContent(i + 1)) ++ctr[3];
        if (hBadTowers->GetBinContent(i + 1))        ++ctr[4];

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
            ++ctr_freq[0];
        }
        else if (hBadTowersHot->GetBinContent(i + 1) >= m_threshold_low) {
            output.cd("hHotTowerSigmaLessFreq");
            hHotTowerSigma_vec[i]->Write();
        }
        else hSigmaHot->Add(hHotTowerSigma_vec[i]);

        if (hBadTowersDead->GetBinContent(i + 1)    >= m_threshold) ++ctr_freq[1];
        if (hBadTowersCold->GetBinContent(i + 1)    >= m_threshold) ++ctr_freq[2];
        if (hBadTowersHotChi2->GetBinContent(i + 1) >= m_threshold) ++ctr_freq[3];
        if (hBadTowers->GetBinContent(i + 1) >= m_threshold)        ++ctr_freq[4];
  }

  output.cd();
  hSigmaNeverHot->Write();
  hSigmaHot->Write();
  hSigmaFreqHot->Write();

  cout << "Towers flagged hot: " << ctr[0] << ", " << ctr[0]*100./m_ntowers << " %" << endl;
  cout << "Towers flagged hot more than 50% of runs: " << ctr_freq[0] << ", " << ctr_freq[0]*100./m_ntowers << " %" << endl;

  cout << "Towers flagged dead: " << ctr[1] << ", " << ctr[1]*100./m_ntowers << " %" << endl;
  cout << "Towers flagged dead more than 50% of runs: " << ctr_freq[1] << ", " << ctr_freq[1]*100./m_ntowers << " %" << endl;

  cout << "Towers flagged cold: " << ctr[2] << ", " << ctr[2]*100./m_ntowers << " %" << endl;
  cout << "Towers flagged cold more than 50% of runs: " << ctr_freq[2] << ", " << ctr_freq[2]*100./m_ntowers << " %" << endl;

  cout << "Towers flagged hot chi2: " << ctr[3] << ", " << ctr[3]*100./m_ntowers << " %" << endl;
  cout << "Towers flagged hot chi2 more than 50% of runs: " << ctr_freq[3] << ", " << ctr_freq[3]*100./m_ntowers << " %" << endl;

  cout << "Towers flagged bad: " << ctr[4] << ", " << ctr[4]*100./m_ntowers << " %" << endl;
  cout << "Towers flagged bad more than 50% of runs: " << ctr_freq[4] << ", " << ctr_freq[4]*100./m_ntowers << " %" << endl;

  output.Close();
}

void HotTowerAnalysis(const string &input,
                      const string &output = "test.root",
                      const Bool_t doTime = true,
                      const Int_t maxRuns = 0,
                      string outputMissingHotMap = "missingHotMap.csv",
                      string outputMissingBadChi2 = "missingBadChi2.csv",
                      string outputRunStats = "acceptance.csv") {

    cout << "#############################" << endl;
    cout << "Input Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "doTime: " << doTime << endl;
    cout << "outputMissingHotMap: " << outputMissingHotMap << endl;
    cout << "outputMissingBadChi2: " << outputMissingBadChi2 << endl;
    cout << "outputRunStats: " << outputRunStats << endl;

    myAnalysis::m_doTime = doTime;
    myAnalysis::m_maxRuns = maxRuns;
    string m_outputDir = fs::absolute(output).parent_path().string();

    fs::create_directories(m_outputDir);

    outputRunStats = m_outputDir + "/" + outputRunStats;
    outputMissingHotMap = m_outputDir + "/" + outputMissingHotMap;
    outputMissingBadChi2 = m_outputDir + "/" + outputMissingBadChi2;

    if (myAnalysis::m_detector == "HCALIN" || myAnalysis::m_detector == "HCALOUT") {
      myAnalysis::m_bins_phi = 64;
      myAnalysis::m_bins_eta = 24;
      myAnalysis::m_ntowers = myAnalysis::m_bins_phi * myAnalysis::m_bins_eta;
    }

    cout << "Detector: " << myAnalysis::m_detector << endl;

    Int_t ret = myAnalysis::init(input);
    if(ret) return;

    if(maxRuns) cout << "Processing: " << min((Int_t) myAnalysis::runs.size(), maxRuns) << " Runs" << endl;
    else        cout << "Processing: " << myAnalysis::runs.size() << " Runs" << endl;
    cout << "#############################" << endl;

    ret = myAnalysis::process_event(outputRunStats);
    if(ret) return;

    myAnalysis::finalize(output, outputMissingHotMap, outputMissingBadChi2);

    cout << "======================================" << endl;
    cout << "done" << endl;
    std::quick_exit(0);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 8) {
        cout << "usage: ./hotAna inputFile [outputFile] [doTime] [maxRuns] [outputMissingHotMap] [outputMissingBadChi2] [outputRunStats]" << endl;
        cout << "inputFile: containing list of run numbers" << endl;
        cout << "outputFile: location of output file. Default: test.root." << endl;
        cout << "doTime: Do Time Analysis, requires input list to be <run>, <timestamp>. Default: true." << endl;
        cout << "maxRuns: Max runs to process (useful when testing). Default: 0 (means run all)." << endl;
        cout << "outputMissingHotMap: location of outputMissingHotMap file. Default: missingHotMap.csv." << endl;
        cout << "outputMissingBadChi2: location of outputMissingBadChi2 file. Default: missingBadChi2.csv." << endl;
        cout << "outputRunStats: location of outputRunStats file. Default: acceptance.csv." << endl;
        return 1;
    }

    string outputFile           = "test.root";
    Bool_t doTime               = true;
    Int_t maxRuns               = 0;
    string outputMissingHotMap  = "missingHotMap.csv";
    string outputMissingBadChi2 = "missingBadChi2.csv";
    string outputRunStats       = "acceptance.csv";

    if (argc >= 3) {
        outputFile = argv[2];
    }
    if (argc >= 4) {
        doTime = atoi(argv[3]);
    }
    if (argc >= 5) {
        maxRuns = atoi(argv[4]);
    }
    if (argc >= 6) {
        outputMissingHotMap = argv[5];
    }
    if (argc >= 7) {
        outputMissingBadChi2 = argv[6];
    }
    if (argc >= 8) {
        outputRunStats = argv[7];
    }

    HotTowerAnalysis(argv[1], outputFile, doTime, maxRuns, outputMissingHotMap, outputMissingBadChi2, outputRunStats);

    return 0;
}
# endif
