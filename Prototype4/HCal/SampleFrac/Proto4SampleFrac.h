#ifndef __Proto4SampleFrac_H__
#define __Proto4SampleFrac_H__

#include <TFile.h>
#include <TNtuple.h>
#include <fun4all/SubsysReco.h>
#include <stdint.h>
#include <fstream>
#include <string>

class PHCompositeNode;
class PHG4HitContainer;
class PHG4TruthInfoContainer;
class Fun4AllHistoManager;
class TH1F;
class TH2F;
class TH3F;
class TTree;
class PHG4Particle;
class RawTowerGeom;
class RawTowerContainer;

/// \class Proto4SampleFrac to help you get started
class Proto4SampleFrac : public SubsysReco
{
 public:
  //! constructor
  Proto4SampleFrac(const std::string &calo_name, const std::string &filename);

  //! destructor
  virtual ~Proto4SampleFrac();

  //! Standard function called at initialization
  int Init(PHCompositeNode *topNode);

  //! Standard function called when a new run is processed
  int InitRun(PHCompositeNode *topNode);

  //! Standard function called at each event
  int process_event(PHCompositeNode *topNode);

  //! Standard function called at the end of processing. Save your stuff here.
  int End(PHCompositeNode *topNode);

  //! Is processing simulation files?
  void
  is_sim(bool b)
  {
    _is_sim = b;
  }

  //! common prefix for QA histograms
  std::string get_histo_prefix();

 private:
  // calorimeter size
  enum
  {
    n_size = 8
  };

  //! is processing simulation files?
  bool _is_sim;

  //! get manager of histograms
  Fun4AllHistoManager *get_HistoManager();

  std::pair<int, int>
  find_max(RawTowerContainer *towers, int cluster_size);

  //! output root file name
  std::string _filename;
  std::string _calo_name;

  PHG4HitContainer* _calo_hit_container; // G4Hit for calorimeter
  PHG4HitContainer* _calo_abs_hit_container; // G4Hit for absorber
  PHG4TruthInfoContainer* _truth_container;
};

#endif  // __Proto4SampleFrac_H__
