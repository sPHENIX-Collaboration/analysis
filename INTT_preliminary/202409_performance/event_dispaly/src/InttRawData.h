#ifndef INTTRAWDATA_H__
#define INTTRAWDATA_H__

#include <fun4all/SubsysReco.h>
#include <InttHotMap.h>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TTree;
class InttEvent;

/// Definition of this analysis module class
class InttRawData : public SubsysReco
{
 public:
  /// Constructor
  InttRawData(const std::string &name = "InttRawData",
              const std::string &fname = "inttevent.root");

  // Destructor
  virtual ~InttRawData();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);

  void SetHotMapFile(const char* filename) { hotfilename_ = std::string(filename); }

  InttEvent* getInttEvent(){ return inttEvt_; }

  struct OfflineRawdata{
    int layer;
    int ladder_z;
    int ladder_phi;
    int bco;
    int chip;
    int chan;
    int adc;

    void set(int Layer, int Ladder_z, int Ladder_phi, int Bco, int Chip, int Chan, int Adc){
      layer       = Layer;
      ladder_z    = Ladder_z;
      ladder_phi  = Ladder_phi;
      bco         = Bco;
      chip        = Chip;
      chan        = Chan;
      adc         = Adc;
    }
  };


  std::string fname_;
  std::string hotfilename_;

  InttHotMap hotmap_;

  TFile* inFile_;
  TTree* tree_;
  InttEvent* inttEvt_;
  int        ievent_;
};

#endif
