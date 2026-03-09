// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef BEAMWAVEFORM_H
#define BEAMWAVEFORM_H

#include <calobase/TowerInfoDefs.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <TFile.h>
#include <TTree.h>
#include <TProfile.h>
#include <TH2F.h>

#include "caloreco/CaloTowerBuilder.h"
#include <caloreco/CaloWaveformProcessing.h>


#include <string>
#include <vector>

class PHCompositeNode;
class CaloWaveformProcessing;
class TGraph;
class TProfile2D;

class BeamWaveform : public SubsysReco
{
public:
    BeamWaveform(const std::string &name = "BeamWaveform");

    ~BeamWaveform() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
     */
    int Init(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
     */
    int process_event(PHCompositeNode *topNode) override;

    /// Clean up internals after each event.
    int ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    void Print(const std::string &what = "ALL") const override;

    void SetDetector(const std::string &name) { m_Detector = name; }
    void SetPacketRange(int low, int high)
    {
        m_packet_low = low;
        m_packet_high = high;
    }

private:
    unsigned int (*encode_tower)(const unsigned int) = TowerInfoDefs::encode_emcal;
    static const int netabins = 98;
    static const int nphibins = 258;
    float m_peakfixedtime[netabins][nphibins] = {{6}};
    TProfile *m_template[netabins] = {nullptr};
    void SetNSamples(int n) { m_n_samples = n; }
    std::string m_Detector = "HCALOUT";
    int m_packet_low = 6001;
    int m_packet_high = 6128;
    int m_n_samples = 12;
    TFile *m_file = nullptr;
    TTree *m_tree = nullptr;
    std::vector<int> m_etabin;
    std::vector<int> m_phibin;
    std::vector<double> m_peak;
    std::vector<double> m_peaktime;
    std::vector<double> m_pedestal;
    std::vector<double> m_ADC_peak;
    std::vector<double> m_ADC_peaktime;
    std::vector<double> m_1par_peak;
    std::vector<double> m_1par_pedestal;
    std::vector<double> m_2par_peak;
    std::vector<double> m_2par_pedestal;
    std::vector<double> m_par_peaktime;
    std::vector<std::vector<double>> m_waveform;
    // a map of a pair
    std::map<std::pair<unsigned int, unsigned int>, int> m_masktower;
    Fun4AllHistoManager *hm = nullptr;
    TProfile *hpwaveform_1[netabins] = {nullptr}; //50 - 500 (beam).
    TProfile *hpwaveform_2[netabins] = {nullptr}; //500 - 1000 (beam).
    TProfile *hpwaveform_3[netabins] = {nullptr}; //> 1000 (beam).
    TProfile *hpwaveform_1_sum; //50 - 500 (beam).
    TProfile *hpwaveform_2_sum; //500 - 1000 (beam).
    TProfile *hpwaveform_3_sum; //> 1000 (beam).
    TH2F* h_sig_chi2;
    TH1F* h_chi2;
    TH2F* h_sig_chi2_nt;
    TH2F* h_sig1_sig2;
    TH2F* h_dif_peak_time;
    TH2F* h_chi1_chi2;
    TH2F* h_ped1_ped2;
    TH2F* h_sigP1_sigP2;
    int eventCount = 0;
    const int NWFs = 100;
    int CWFs = 0;
    TGraph* h_wf[1000];
    TProfile2D *p_chi2;
    TProfile2D *h_etaphi_ped;
    TProfile2D *h_etaphi_ped2;
    TH1F* h_time1;
    TH1F* h_time2;
 
    CaloWaveformProcessing *WaveformProcessing;
    CaloWaveformProcessing *WaveformProcessing2;
};

#endif // BEAMWAVEFORM_H
