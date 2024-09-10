#ifndef INTTDUMMYDATA_H__
#define INTTDUMMYDATA_H__

#include <fun4all/SubsysReco.h>

/// Class declarations for use in the analysis module
class PHCompositeNode;
class TFile;
class TTree;
class InttEvent;

/// Definition of this analysis module class
class InttDummyData : public SubsysReco
{
 public:
  /// Constructor
  InttDummyData(const std::string &name = "InttDummyData",
              const std::string &fname = "inttevent.root");

  // Destructor
  virtual ~InttDummyData();

  /// SubsysReco initialize processing method
  int Init(PHCompositeNode *);
  
  /// SubsysReco initialize processing method
  int InitRun(PHCompositeNode *);

  /// SubsysReco event processing method
  int process_event(PHCompositeNode *);

  /// SubsysReco end processing method
  int End(PHCompositeNode *);


  void setDummyInttEvent(const int ievent);

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

  InttEvent* inttEvt_;
  int        ievent_;
};

#endif
