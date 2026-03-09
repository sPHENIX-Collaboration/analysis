#ifndef __HFMLTriggerInterface_H__
#define __HFMLTriggerInterface_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <limits.h>
#include <cmath>
#include <fstream>  // std::fstream
#include <string>
#include <vector>
#include <map>

class TTree;
class TFile;
class TH2F;
class TH3F;

class PdbParameterMap;
class PHCompositeNode;
class PHG4CylinderGeomContainer;
class PHG4HitContainer;
class PHHepMCGenEventMap;
class PHG4TruthInfoContainer;
class PHG4Hit;
//class SvtxEvalStack;
//class TrkrClusterHitAssoc;
class TrkrHitSetContainer;
//class TrkrHitTruthAssoc;

namespace HepMC
{
class GenEvent;
}

class HFMLTriggerInterface : public SubsysReco
{
 public:
  HFMLTriggerInterface(std::string filename);

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

  int load_nodes(PHCompositeNode* topNode);

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

  // eval stack
  //SvtxEvalStack *_svtxevalstack;

  TrkrHitSetContainer *m_hitsets;

  PHHepMCGenEventMap *m_GenEventMap;
  PHG4TruthInfoContainer *m_truthInfo;
  PHG4HitContainer *m_g4hits_mvtx;

  //TrkrHitTruthAssoc   *m_hit_truth_map;
  //TrkrClusterHitAssoc *m_cluster_hit_map;

  PdbParameterMap *m_Flags;

  PHG4CylinderGeomContainer *m_Geoms;

  TH2F *m_hitStaveLayer;
  TH2F *m_hitModuleHalfStave;
  TH2F *m_hitChipModule;

  TH3F *m_hitLayerMap;
  TH3F *m_hitPixelPhiMap;
  TH3F *m_hitPixelPhiMapHL;
  TH3F *m_hitPixelZMap;

};

#endif  // __HFMLTriggerInterface_H__
