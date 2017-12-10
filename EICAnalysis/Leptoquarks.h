#ifndef __Leptoquarks_H__
#define __Leptoquarks_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class TNtuple;
class TFile;
class TH1D;

class PHCompositeNode;
namespace HepMC
{
  class GenEvent;
  class GenParticle;
}

class Leptoquarks : public SubsysReco
{

public:

  Leptoquarks(std::string filename);

  int
  Init(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);

  void
  set_beam_energies( float beam_electron , float beam_proton )
  {
    _ebeam_E = fabs( beam_electron );
    _pbeam_E = fabs( beam_proton );
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

  bool _verbose;

  int _ievent;
  int _total_pass;

  std::string _foutname;
  TFile *_fout_root;

  /* output tree and variables */
  TNtuple* _tree_event;

  /* beam energies electron and proton */
  float _ebeam_E;
  float _pbeam_E;
  //! The embedding ID for the HepMC subevent to be analyzed.
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int _embedding_id;

  /** Check if this is the final state particle or not- update pointer if needed */
  void UpdateFinalStateParticle( HepMC::GenParticle *& );

};

#endif // __Leptoquarks_H__
