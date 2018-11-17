#ifndef __HFMLTriggerHepMCTrigger_H__
#define __HFMLTriggerHepMCTrigger_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <limits.h>
#include <cmath>
#include <fstream>  // std::fstream
#include <string>
#include <vector>

class TTree;
class TFile;
class TH1D;
class TH2F;
class TH3F;

class PHCompositeNode;
class PHHepMCGenEventMap;
class PdbParameterMap;

namespace HepMC
{
class GenEvent;
}

class HFMLTriggerHepMCTrigger : public SubsysReco
{
 public:
  HFMLTriggerHepMCTrigger(const std::string &moduleName, const std::string &filename);

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

  void set_RejectReturnCode(int r) { m_RejectReturnCode = r; }

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

  int m_RejectReturnCode;

  TFile *_f;

  std::string _foutname;

  double _eta_min;
  double _eta_max;

  //! The embedding ID for the HepMC subevent to be analyzed.
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int _embedding_id;

  PHHepMCGenEventMap *m_Geneventmap;
  PdbParameterMap *m_Flags;

  TH1D *m_hNorm;
  TH2F *m_DRapidity;
};

#endif  // __HFMLTriggerHepMCTrigger_H__
