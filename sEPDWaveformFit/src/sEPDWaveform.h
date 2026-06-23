#ifndef SEPDWAVEFORM_H
#define SEPDWAVEFORM_H

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

class sEPDWaveform : public SubsysReco
{
public:
    sEPDWaveform(const std::string &name = "sEPDWaveform");

    ~sEPDWaveform() override;

    int Init(PHCompositeNode *topNode) override;

    int process_event(PHCompositeNode *topNode) override;

    int ResetEvent(PHCompositeNode *topNode) override;

    int End(PHCompositeNode *topNode) override;

    void Print(const std::string &what = "ALL") const override;

    void SetDetector(const std::string &name) { m_Detector = name; }
    void SetPacketRange(int low, int high)
    {
        m_packet_low = low;
        m_packet_high = high;
    }
    void SetNSamples(int n) { m_n_samples = n; }

    void SetUseOfflinePackets(bool b) { m_offline = b; }
    void SetOutputFileName(const std::string &name) { m_outputFileName = name; }

    void SetFillGeometryHistograms(bool fill) { m_fill_geometry_histos = fill; }
    void SetFillGlobalHistograms(bool fill) { m_fill_global_histos = fill; }
    void SetProgressInterval(int interval) { m_progress_interval = interval; }
    void SetShowProgress(bool show) { m_show_progress = show; }

private:

    void ShowProgress(int current, int total);

    unsigned int (*encode_tower)(const unsigned int) = TowerInfoDefs::encode_epd;

    static const int NARM = 2;
    static const int NRING = 16;
    static const int NPHI = 24;
    static const int NPHI_RING0 = 12;

    std::string m_Detector = "SEPD";
    std::string m_outputFileName = "sEPD_templates.root";
    int m_packet_low = 9001;
    int m_packet_high = 9006;
    int m_nchannels = 128;
    int m_n_samples = 12;

    int m_template_nbins = 300;

    double m_target_hm = 4.5;
    bool m_offline = true;

    bool m_fill_geometry_histos = true;
    bool m_fill_global_histos = true;
    bool m_show_progress = true;
    int m_progress_interval = 100;

    TFile *m_file = nullptr;
    TTree *m_tree = nullptr;

    std::vector<int> m_arm;
    std::vector<int> m_ring;
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

    std::map<std::pair<unsigned int, unsigned int>, int> m_masktower;

    Fun4AllHistoManager *hm = nullptr;

    TProfile *hpwaveform_1[NARM][NRING] = {{nullptr}};
    TProfile *hpwaveform_2[NARM][NRING] = {{nullptr}};
    TProfile *hpwaveform_3[NARM][NRING] = {{nullptr}};
    TProfile *hpwaveform_1_sum = nullptr;
    TProfile *hpwaveform_2_sum = nullptr;
    TProfile *hpwaveform_3_sum = nullptr;

    TH2F *h2waveform_1[NARM][NRING] = {{nullptr}};
    TH2F *h2waveform_2[NARM][NRING] = {{nullptr}};
    TH2F *h2waveform_3[NARM][NRING] = {{nullptr}};
    TH2F *h2waveform_1_sum = nullptr;
    TH2F *h2waveform_2_sum = nullptr;
    TH2F *h2waveform_3_sum = nullptr;

    TProfile *hpwaveform_global_1 = nullptr;
    TProfile *hpwaveform_global_2 = nullptr;
    TProfile *hpwaveform_global_3 = nullptr;
    TH2F *h2waveform_global_1 = nullptr;
    TH2F *h2waveform_global_2 = nullptr;
    TH2F *h2waveform_global_3 = nullptr;

    TProfile *waveform_template = nullptr;

    TH2F* h_sig_chi2 = nullptr;
    TH1F* h_chi2 = nullptr;
    TH2F* h_sig_chi2_nt = nullptr;
    TH2F* h_sig1_sig2 = nullptr;
    TH2F* h_dif_peak_time = nullptr;
    TH2F* h_chi1_chi2 = nullptr;
    TH2F* h_ped1_ped2 = nullptr;
    TH2F* h_sigP1_sigP2 = nullptr;
    TProfile2D *p_chi2 = nullptr;
    TProfile2D *h_etaphi_ped = nullptr;
    TProfile2D *h_etaphi_ped2 = nullptr;
    TH1F* h_time1 = nullptr;
    TH1F* h_time2 = nullptr;

    int eventCount = 0;
    const int NWFs = 100;
    int CWFs = 0;
    TGraph* h_wf[1000] = {nullptr};

    CaloWaveformProcessing *WaveformProcessing = nullptr;
    CaloWaveformProcessing *WaveformProcessing2 = nullptr;
};

#endif
