// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SMDHISTGEN_H
#define SMDHISTGEN_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TFile;
class TTree;
class TH1;
class TH2;
class TGraphErrors;
/* class Packet; */
class CaloWaveformFitting;
class RunHeaderv1;
class TowerInfoContainer;
class CaloPacketContainerv1;
class CaloPacket;

class SmdHistGen : public SubsysReco
{
 public:

  SmdHistGen(const std::string &name = "SmdHistGen", const std::string& which_mode = "dst", const char* outname = "SmdHists.root");

  ~SmdHistGen() override;

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

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
  std::string mode;
  const char* outfilename;
  TFile *outfile;

  void GetRunNumber();
  int GetSpinPatterns();
  void GetAdcs();
  void GetAdcsDst();
  void GetAdcsRaw();
  void CompSmdAdc();
  void CompSmdPos();
  void CompSmdPosCorr();
  void CompSumSmd();
  void CountSMDHits();
  bool NeutronSelection(std::string which, bool centerCorrected = false);
  std::vector<float> anaWaveformFast(CaloPacket *p, const int channel);
  int CountAdjacentHits(std::vector<int> channels);

  double PI = 3.14159;
  static const int NBUNCHES = 120;
  // packet id numbers
  const int packet_GL1 = 14001;
  const int packet_blue = 14902;
  const int packet_yellow = 14903;
  const int packet_smd = 12001;

  // spin info
  int spinPatternBlue[NBUNCHES] = {0};
  int spinPatternYellow[NBUNCHES] = {0};
  int bunchNum = 0;
  int crossingShift = 0;

  // run and ZDC containers
  RunHeaderv1 *runHeader = nullptr;
  int runNum = 0;
  int evtctr = 0;
  int gl1event = 0;
  int smdevent = 0;
  TowerInfoContainer *towerinfosZDC = nullptr;
  CaloPacketContainerv1 *packetsZDC = nullptr;
  CaloWaveformFitting *WaveformProcessingFast = nullptr;

  // ADC and timing info
  float smd_adc[32] = {0.0f};
  float zdc_adc[16] = {0.0f};
  float veto_adc[4] = {0.0f};
  float smd_time[32] = {0.0f};
  float zdc_time[16] = {0.0f};
  float veto_time[4] = {0.0f};
  float smd_sum[4] = {0.0f}; 
  float smd_pos[4] = {0.0f};
  float smd_pos_corr[4] = {0.0f};

  float gain[32] = {0.0f};
  float smd_south_rgain[16] = {0.0f};
  float smd_north_rgain[16] = {0.0f};

  int n_hor_numhits  = 0;
  int n_ver_numhits  = 0;
  int s_hor_numhits = 0;
  int s_ver_numhits = 0;

  // ADC and timing cuts
  int minSMDcut = 5;
  int minZDC1cut = 100;
  int minZDC2cut = 15;
  int maxVetocut = 150;
  int minSMDhits = 2;
  float zdc_t_low = 5.5;
  float zdc_t_high = 9.5;
  float smd_north_t_low = 9.0;
  float smd_north_t_high = 14.5;
  float smd_south_t_low = 6.5;
  float smd_south_t_high = 12.5;
  float veto_north_t_low = 5.5;
  float veto_north_t_high = 9.5;
  float veto_south_t_low = 6.5;
  float veto_south_t_high = 12.5;
  float radius_low = 2.0;
  float radius_high = 4.0;

  // Histograms and graphs
  // hits per channel and hit multiply
  TH1 *smd_hor_north_total_hits = nullptr;
  TH1 *smd_ver_north_total_hits = nullptr;
  TH1 *smd_hor_north_neutron_hits = nullptr;
  TH1 *smd_ver_north_neutron_hits = nullptr;
  TH1 *smd_hor_north_multiplicity = nullptr;
  TH1 *smd_ver_north_multiplicity = nullptr;
  TH1 *smd_hor_south_total_hits = nullptr;
  TH1 *smd_ver_south_total_hits = nullptr;
  TH1 *smd_hor_south_neutron_hits = nullptr;
  TH1 *smd_ver_south_neutron_hits = nullptr;
  TH1 *smd_hor_south_multiplicity = nullptr;
  TH1 *smd_ver_south_multiplicity = nullptr;

  // signals and waveforms
  TH1* smd_north_signal = nullptr;
  TH1* smd_south_signal = nullptr;
  TH1* smd_north_signals[15] = {nullptr};
  TH1* smd_south_signals[15] = {nullptr};
  TH1 *smd_sum_hor_north = nullptr;
  TH1 *smd_sum_ver_north = nullptr;
  TH1 *smd_sum_hor_south = nullptr;
  TH1 *smd_sum_ver_south = nullptr;
  TH2* smd_north_waveforms = nullptr;
  TH2* smd_north_waveforms_zoomed = nullptr;
  TH2* smd_north_waveforms_raw = nullptr;
  TH2* smd_north_channel_waveforms[15] = {nullptr};
  TH2* smd_south_waveforms = nullptr;
  TH2* smd_south_waveforms_zoomed = nullptr;
  TH2* smd_south_waveforms_raw = nullptr;
  TH2* smd_south_channel_waveforms[15] = {nullptr};
  TH1* smd_north_pedestal = nullptr;
  TH1* smd_south_pedestal = nullptr;
  TH1 *zdc1_north = nullptr;
  TH1 *zdc2_north = nullptr;
  TH2* zdc_north_waveforms = nullptr;
  TH1 *zdc1_south = nullptr;
  TH1 *zdc2_south = nullptr;
  TH2* zdc_south_waveforms = nullptr;
  TH1 *vetofront_north = nullptr;
  TH1 *vetoback_north = nullptr;
  TH2* veto_north_waveforms = nullptr;
  TH1 *vetofront_south = nullptr;
  TH1 *vetoback_south = nullptr;
  TH2* veto_south_waveforms = nullptr;

  // xy distributions
  TH2 *smd_xy_all_north = nullptr;
  TH2 *smd_xy_all_corrected_north = nullptr;
  TH2 *smd_xy_neutron_north = nullptr;
  TH2 *smd_xy_neutron_corrected_north = nullptr;
  TH2 *smd_xy_all_south = nullptr;
  TH2 *smd_xy_all_corrected_south = nullptr;
  TH2 *smd_xy_neutron_south = nullptr;
  TH2 *smd_xy_neutron_corrected_south = nullptr;

  // 1D spin dependent x and y
  TH1 *smd_hor_north = nullptr;
  TH1 *smd_ver_north = nullptr;
  TH1 *smd_hor_north_up = nullptr;
  TH1 *smd_ver_north_up = nullptr;
  TH1 *smd_hor_north_down = nullptr;
  TH1 *smd_ver_north_down = nullptr;
  TH1 *smd_hor_south = nullptr;
  TH1 *smd_ver_south = nullptr;
  TH1 *smd_hor_south_up = nullptr;
  TH1 *smd_ver_south_up = nullptr;
  TH1 *smd_hor_south_down = nullptr;
  TH1 *smd_ver_south_down = nullptr;

  // beam center correction
  TH1 *smd_hor_north_corrected = nullptr;
  TH1 *smd_ver_north_corrected = nullptr;
  TH1 *smd_hor_north_corrected_up = nullptr;
  TH1 *smd_ver_north_corrected_up = nullptr;
  TH1 *smd_hor_north_corrected_down = nullptr;
  TH1 *smd_ver_north_corrected_down = nullptr;
  TH1 *smd_hor_south_corrected = nullptr;
  TH1 *smd_ver_south_corrected = nullptr;
  TH1 *smd_hor_south_corrected_up = nullptr;
  TH1 *smd_ver_south_corrected_up = nullptr;
  TH1 *smd_hor_south_corrected_down = nullptr;
  TH1 *smd_ver_south_corrected_down = nullptr;

  // phi distributions for asymmetry
  TH1 *smd_north_phi_up = nullptr;
  TH1 *smd_north_phi_down = nullptr;
  TH1 *smd_north_phi_sum = nullptr;
  TH1 *smd_north_phi_diff = nullptr;
  TH1 *smd_north_phi_L_up = nullptr;
  TH1 *smd_north_phi_L_down = nullptr;
  TH1 *smd_north_phi_R_up = nullptr;
  TH1 *smd_north_phi_R_down = nullptr;
  TH1 *smd_south_phi_up = nullptr;
  TH1 *smd_south_phi_down = nullptr;
  TH1 *smd_south_phi_sum = nullptr;
  TH1 *smd_south_phi_diff = nullptr;
  TH1 *smd_south_phi_L_up = nullptr;
  TH1 *smd_south_phi_L_down = nullptr;
  TH1 *smd_south_phi_R_up = nullptr;
  TH1 *smd_south_phi_R_down = nullptr;

  // beam center correction
  TH1 *smd_north_phi_up_corrected = nullptr;
  TH1 *smd_north_phi_down_corrected = nullptr;
  TH1 *smd_north_phi_sum_corrected = nullptr;
  TH1 *smd_north_phi_diff_corrected = nullptr;
  TH1 *smd_north_phi_L_up_corrected = nullptr;
  TH1 *smd_north_phi_L_down_corrected = nullptr;
  TH1 *smd_north_phi_R_up_corrected = nullptr;
  TH1 *smd_north_phi_R_down_corrected = nullptr;
  TH1 *smd_south_phi_up_corrected = nullptr;
  TH1 *smd_south_phi_down_corrected = nullptr;
  TH1 *smd_south_phi_sum_corrected = nullptr;
  TH1 *smd_south_phi_diff_corrected = nullptr;
  TH1 *smd_south_phi_L_up_corrected = nullptr;
  TH1 *smd_south_phi_L_down_corrected = nullptr;
  TH1 *smd_south_phi_R_up_corrected = nullptr;
  TH1 *smd_south_phi_R_down_corrected = nullptr;

};

#endif // SMDHISTGEN_H
