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


private:

  bool _verbose;

  int _ievent;
  int _total_pass;

  std::string _foutname;
  TFile *_fout_root;

  /* output tree and variables */
  TNtuple* _tree_quarkonia;

};

#endif // __Quarkonia2LeptonsMC_H__
