#ifndef TRACKRECO_TPCGAINDEBUG_H
#define TRACKRECO_TPCGAINDEBUG_H

//===============================================
/// \file 
/// \brief Calculate preliminary gain from hits
/// \author Yeonju Go 
//===============================================

#include <fun4all/SubsysReco.h>
#include <trackbase/TrkrDefs.h>

#include <TMatrixFfwd.h>
#include <TMatrixT.h>
#include <TMatrixTUtils.h>
#include <string>
#include <map>
#include <set>

class PHCompositeNode;
class PHTimer;
class TrkrCluster;
class TFile;
class TH1F;
class TH1I;
class TNtuple;
class TTree;
class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;

class TPCGainDebug : public SubsysReco
{
 public:
  TPCGainDebug(const std::string &name = "TPCGAINDEBUG",
                const std::string &filename = "tpc_gain_debug.root",
                const int runnumber = 25926
                );
  ~TPCGainDebug() override;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int End(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *) override;

  void save_ntuple(bool b) { _save_ntuple = b; }
  void save_debugHist(bool b) { _save_debugHist= b; }
  void save_sharkFin(bool b) { _save_sharkFin= b; }
  void do_gain(bool b) { _do_gain= b; }
  void debugNtuple_adcThreshold(float b) { AllHitADCThreshold = b; }
  void do_pedSigmaCut(float b) { _do_pedSigmaCut = b; }
  void set_adcSigmaThreshold(unsigned short b) { adcSigmaThreshold = b; }
  void skipNevent(int b) { startevt = b; }
  void event_range(int a, int b) { startevt = a;  endevt = b; }
  void set_eventOffset(int a) { _eventOffset = a;}

  // TH1F* pedhist;

 private:
  int _ievent;
  unsigned int _iseed;
  float m_fSeed;
  bool _save_ntuple;
  bool _do_gain;
  bool _save_debugHist;
  bool _do_pedSigmaCut;
  bool _save_sharkFin;
  bool _isSharkFin;
  // bool _isSharkFinForRun;
  // TNtuple *_ntp_hit;
  std::string _filename;
  const int _runnumber;
  TFile *_tfile;
  PHTimer *_timer;
  float AllHitADCThreshold;

  TTree *gaintree = nullptr;
  TTree *_ntp_hit= nullptr;
  TH1F *h1F_tbin;
  TH1F *h1F_tbin_prev;
  TH1F *h1F_tbin_next;
  TH1I *h1I_event_shark;

  double AdcClockPeriod = 53.0; // ns
  int startevt = 0;
  int endevt = 999999;
  int _eventOffset = 0;
  //// eval stack

  ////----------------------------------
  //// gain calculations
  unsigned short adcThreshold = 50;
  unsigned short adcSigmaThreshold = 5;
  unsigned short adcSigmaThreshold_phisize = 4;
  unsigned short clusterPhibinArea = 4;
  unsigned short clusterTbinArea = 5;
  unsigned short seedPhibinDistance = 4;
  unsigned short seedTbinDistance = 4;

  struct ihit{
    short adc = 0;
    unsigned short phibin = 0;
    unsigned short tbin = 0;
  };

  struct igain{
    short adc = 0;
    unsigned short phibin = 0;
    unsigned short tbin = 0;
    float pathlength = 0;
    int phisize= 0;
  };

  struct ADCInfo{
    float maxADC = 0;
    float tbin = 0;
    bool isSignal = 0;
    // You can add more members if needed
  };

  const int maxside = 2;
  const int maxsector = 12;
  const int maxlayer = 60;

  const int sampleMax = 360;
  const int binWidthForPedEst = 4;

  std::vector<std::vector<ihit>> hpl_seed; // [layer][hits in a given layer] 

  // std::vector<std::vector<ihit>> hpl_seed; // [layer][hits in a given layer] 
  // std::vector<std::vector<ihit>> hpl_all; // [layer][hits in a given layer] 

  void clearHitsPerLayer(){
    for(int i=0; i<maxlayer; i++){
      hpl_seed[i].clear();
      // hpl_all[i].clear();
    }
  }

  void resizeHitsPerLayer(){
    hpl_seed.resize(maxlayer);
    // hpl_all.resize(maxlayer);
  }

  ////----------------------------------
  //// gain tree branchs 
  int m_run;
  int m_event;
  std::vector<int> m_side;
  std::vector<int> m_sector;
  std::vector<int> m_layer;
  std::vector<int> m_phibin;
  std::vector<int> m_tbin;
  std::vector<float> m_adcsum;
  std::vector<float> m_pathlength;
  std::vector<int> m_phisize;
  // std::vector<int> m_errorcode;

  ////----------------------------------
  //// all hit tree branchs 
  int a_side;
  int a_sector;
  int a_layer;
  int a_phibin;
  float a_phi;
  float a_pedmean;
  float a_pedwidth;
  bool a_isSharkFin;
  std::vector<int> a_tbin;
  std::vector<float> a_adc;
  std::vector<float> a_x;
  std::vector<float> a_y;
  std::vector<float> a_z;
  // std::vector<int> m_tbin;
  // std::vector<float> m_adcsum;

  ////----------------------------------
  //// temp shark fin position variables 
  int sf_side = 0;
  int sf_layer = 0;
  int sf_phibin = 0;

  //// output subroutines
  void fillOutputNtuplesAllhits(PHCompositeNode *topNode);  ///< dump the evaluator information into ntuple for external analysis
  void CalculateGain(PHCompositeNode *topNode);  ///< dump the evaluator information into ntuple for external analysis

  double SignalShape_PowerLawExp(double *x, double *par);
  double SignalShape_PowerLawDoubleExp(double *x, double *par);
 
};

#endif  // G4EVAL_SVTXEVALUATOR_H
