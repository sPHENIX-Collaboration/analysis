#ifndef __Quarkonia2LeptonsMC_H__
#define __Quarkonia2LeptonsMC_H__

#include <fun4all/SubsysReco.h>
//#include <vector>
//#include <cmath>
//#include <string>
//#include <limits.h>
//#include <TNtuple.h>

class TNtuple;
class TFile;
//class TH2D;
class TH1D;

class PHCompositeNode;
namespace HepMC
{
  class GenEvent;
}

class Quarkonia2LeptonsMC : public SubsysReco
{

public:

  Quarkonia2LeptonsMC(std::string filename);

  int
  Init(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);


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
  TNtuple* _tree_quarkonia;

  //! The embedding ID for the HepMC subevent to be analyzed.
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int _embedding_id;
};

#endif // __Quarkonia2LeptonsMC_H__
