#ifndef __HFMLTriggerOccupancy_H__
#define __HFMLTriggerOccupancy_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <limits.h>
#include <cmath>
#include <fstream>  // std::fstream
#include <string>
#include <vector>

class TTree;
class TFile;
class TH1F;
class TH2F;
class TH3F;

class PHCompositeNode;
class PHG4CylinderGeomContainer;
class PHG4TruthInfoContainer;
class TrkrHitSetContainer;
class SvtxEvalStack;
class PdbParameterMap;

namespace HepMC
{
class GenEvent;
}

class HFMLTriggerOccupancy : public SubsysReco
{
 public:
  HFMLTriggerOccupancy(std::string filename);

  int Init(PHCompositeNode *);
  int InitRun(PHCompositeNode *);
  int process_event(PHCompositeNode *);
  int End(PHCompositeNode *);

  double
  get_eta_max() const
  {
    return _eta_max;
  }

  void
  set_eta_max(double etaMax)
  {
    _eta_max = etaMax;
  }

  double
  get_eta_min() const
  {
    return _eta_min;
  }

  void
  set_eta_min(double etaMin)
  {
    _eta_min = etaMin;
  }

  //! The embedding ID for the HepMC subevent to be analyzed.
  //! embedding ID for the event
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int get_embedding_id() const { return _embedding_id; }
  //
  //! The embedding ID for the HepMC subevent to be analyzed.
  //! embedding ID for the event
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  void set_embedding_id(int id) { _embedding_id = id; }

 private:
  int _ievent;

  TFile *_f;
  std::fstream m_jsonOut;

  std::string _foutname;

  double _eta_min;
  double _eta_max;

  //! The embedding ID for the HepMC subevent to be analyzed.
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int _embedding_id;

  unsigned int _nlayers_maps;
  enum {kNCHip = 9};

  // eval stack
  SvtxEvalStack *_svtxevalstack;
  TrkrHitSetContainer *m_hitsetcont;
  PHG4CylinderGeomContainer *m_Geoms;
  PHG4TruthInfoContainer *m_truthInfo;
  PdbParameterMap *m_Flags;

  TH1F *m_hNorm;
  TH1F *m_hNChEta;
  TH1F *m_hVertexZ;

  TH2F *m_hitStaveLayer;
  TH2F *m_hitModuleHalfStave;
  TH2F *m_hitChipModule;

  TH3F *m_hitLayerMap;
  TH3F *m_hitPixelPhiMap;
  TH3F *m_hitPixelPhiMapHL;
  TH3F *m_hitPixelZMap;

  TH1F *m_Multiplicity;
  TH2F *m_LayerMultiplicity;
  TH3F *m_LayerChipMultiplicity;

};

#endif  // __HFMLTriggerOccupancy_H__
