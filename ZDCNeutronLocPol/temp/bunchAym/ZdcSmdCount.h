// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef ZDCSMDCOUNT_H
#define ZDCSMDCOUNT_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <map>
class TFile;
class TTree;
class TH1;
class TH2;
class PHCompositeNode;
class TGraphErrors;

class ZdcSmdCount : public SubsysReco
{
public:
    ZdcSmdCount(const std::string &name = "ZdcSmdCount", const int runnumber = 0,float cutvalue=0.,int roller=0);

    ~ZdcSmdCount() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
     */
    int Init(PHCompositeNode *topNode) override;

    /** Called for first event when run number is known.
        Typically this is where you may want to fetch data from
        database, because you know the run number. A place
        to book histograms which have to know the run number.
     */
    int InitRun(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
     */
    int process_event(PHCompositeNode *topNode) override;
    int process_event_2(PHCompositeNode *topNode);

    /// Clean up internals after each event.
    int ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of each run.
    int EndRun(const int runnumber) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    /// Reset
    int Reset(PHCompositeNode * /*topNode*/) override;

    void Print(const std::string &what = "ALL") const override;

    ////////////// Functions ///////////////////////////////
    void SetZDCAdcCut(unsigned int side, unsigned int module, float value); // side 0 = north side = 1 south
    void GetSpinPatterns();
    
    void InitGraph();
    void WriteGraph();
    void InitHist();

private:
    std::map<int, int> Chmapping;
    const unsigned int kZdc_num_ = 3;
    const unsigned int kSmd_num_ = 16;
    const unsigned int kSide_num_ = 2; // side 0 = north side 1 = south
    static const int NBUNCHES = 120;
    float zdc_adc_cut_[2][3] = {0.}; // ADC cut for each ZDC module ::array[side][module]
    float smd_adc_cut_[2][2] = {0.}; // ADC cut for each smd  ::array[side][direction]
    int spinPatternBlue[NBUNCHES] = {0};
    int spinPatternYellow[NBUNCHES] = {0};
    int crossingShift = 0;
    int runNum = 0;
    int event_counter_;
    int runnumber_;
    int bunchNum_;
    int bunchNum_previous_;
    bool b_s_smd_v12_ = false;
    bool b_n_smd_h12_ = false;
    bool b_s_smd_v67_ = false;
    bool b_n_smd_v67_ = false;
    bool b_s_smd_h12_ = false;
    bool b_n_smd_v12_ = false;
    bool b_s_smd_h78_ = false;
    bool b_n_smd_h78_ = false;

    float cut_value_ = 5;
    ///////////// variables ///////////////////

    /////////// TFile / TTree / NTuple / Histograms//////////
    TFile *outfile_;
    std::string outfile_name_;
    TH1 *h1_NorthZDC_adc_;
    TH1 *h1_SouthZDC_adc_;
    TH1 *h1_SouthZDC_adc_origin_;
    TH1 *h1_NorthZDC_adc_origin_;
    TH1 *h1_SouthSMD_adc_origin_;
    TH1 *h1_NorthSMD_adc_origin_;
    TH1 *h1_SouthVeto_adc_origin_;
    TH1 *h1_NorthVeto_adc_origin_;
    TH1 *h1_SouthZDC_adc_ped_;
    TH1 *h1_NorthZDC_adc_ped_;
    TH1 *h1_SouthSMD_adc_ped_;
    TH1 *h1_NorthSMD_adc_ped_;
    TH1 *h1_SouthVeto_adc_;
    TH1 *h1_NorthVeto_adc_;
    TH1 *h1_SouthVeto_adc_ped_;
    TH1 *h1_NorthVeto_adc_ped_;
    TH1 *h1_NorthSMD_adc_;
    TH1 *h1_SouthSMD_adc_;
    TH1 *h1_NorthZDC1_adc_;
    TH1 *h1_SouthZDC1_adc_;
    TH1 *h1_NorthZDC2_adc_;
    TH1 *h1_SouthZDC2_adc_;
    TH1 *h1_NorthZDC3_adc_;
    TH1 *h1_SouthZDC3_adc_;

    TH1 *h1_NorthSMD_Vertical_12_;
    TH1 *h1_NorthSMD_Vertical_67_;
    TH1 *h1_NorthSMD_Horizontal_12_;
    TH1 *h1_NorthSMD_Horizontal_78_;
    TH1 *h1_SouthSMD_Vertical_12_;
    TH1 *h1_SouthSMD_Vertical_67_;
    TH1 *h1_SouthSMD_Horizontal_12_;
    TH1 *h1_SouthSMD_Horizontal_78_;





//////////////// not used ////////////////////
    TH1 *h1_smd_hor_north_;
    TH1 *h1_smd_ver_north_;
    TH1 *h1_smd_hor_south_;
    TH1 *h1_smd_ver_south_;

    TH1 *h1_smd_hor_south_up_;
    TH1 *h1_smd_ver_south_up_;
    TH1 *h1_smd_hor_south_down_;
    TH1 *h1_smd_ver_south_down_;
    TH1 *h1_smd_hor_north_up_;
    TH1 *h1_smd_ver_north_up_;
    TH1 *h1_smd_hor_north_down_;
    TH1 *h1_smd_ver_north_down_;

    TH2 *h2_SouthZDC_;
    TH2 *h2_NorthZDC_;
    TH2 *h2_SouthSMD_;
    TH2 *h2_NorthSMD_;
    TH2 *h2_SouthVeto_;
    TH2 *h2_NorthVeto_;
    TH2 *h2_SouthZDC_cut_;
    TH2 *h2_NorthZDC_cut_;
    TH2 *h2_SouthSMD_cut_;
    TH2 *h2_NorthSMD_cut_;
    TH2 *h2_SouthVeto_cut_;
    TH2 *h2_NorthVeto_cut_;

    // simple asymmetries
    TGraphErrors *b_asymLR_north = nullptr;
    TGraphErrors *b_asymUD_north = nullptr;
    TGraphErrors *b_asym_north = nullptr;
    TGraphErrors *b_asymLR_south = nullptr;
    TGraphErrors *b_asymUD_south = nullptr;
    TGraphErrors *b_asym_south = nullptr;

    TGraphErrors *y_asymLR_north = nullptr;
    TGraphErrors *y_asymUD_north = nullptr;
    TGraphErrors *y_asym_north = nullptr;
    TGraphErrors *y_asymLR_south = nullptr;
    TGraphErrors *y_asymUD_south = nullptr;
    TGraphErrors *y_asym_south = nullptr;

    // square root asymmetries
    TGraphErrors *b_sqasymLR_north = nullptr;
    TGraphErrors *b_sqasymUD_north = nullptr;
    TGraphErrors *b_sqasym_north = nullptr;
    TGraphErrors *b_sqasymLR_south = nullptr;
    TGraphErrors *b_sqasymUD_south = nullptr;
    TGraphErrors *b_sqasym_south = nullptr;

    TGraphErrors *y_sqasymLR_north = nullptr;
    TGraphErrors *y_sqasymUD_north = nullptr;
    TGraphErrors *y_sqasym_north = nullptr;
    TGraphErrors *y_sqasymLR_south = nullptr;
    TGraphErrors *y_sqasymUD_south = nullptr;
    TGraphErrors *y_sqasym_south = nullptr;
};

#endif // ZDCSMDCOUT_H
