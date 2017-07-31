#ifndef EdepNtuple_h__
#define EdepNtuple_h__

#include <fun4all/SubsysReco.h>
#include <map>
#include <set>
#include <string>
#include <vector>

// Forward declerations
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TH1;
class TH2;
class TNtuple;

class EdepNtuple: public SubsysReco
{
 public:

  //! constructor
  EdepNtuple( const std::string &name = "EdepNtuple", const std::string &filename = "EdepNtuple.root" );

  //! destructor
  virtual ~EdepNtuple();

  //! full initialization
  int Init(PHCompositeNode *);

  //! event processing method
  int process_event(PHCompositeNode *);

  //! end of run method
  int End(PHCompositeNode *);

  void AddNode(const std::string &name, const int detid=0);

protected:
  int nblocks;
  Fun4AllHistoManager *hm;
  std::vector<TH1 *> nhits;
  std::vector<TH1 *> eloss;
  //  std::vector<TH2 *> nhit_edep;
  std::string _filename;
  std::set<std::string> _node_postfix;
  std::map<std::string, int> _detid;
  TNtuple *ntup;
  TNtuple *ntupe;
  TFile *outfile;
};

#endif 
