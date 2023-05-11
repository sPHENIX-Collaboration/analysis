#ifndef TimingNtuple_h__
#define TimingNtuple_h__

#include <fun4all/SubsysReco.h>
#include <map>
#include <set>
#include <string>
#include <vector>

// Forward declerations
class Fun4AllHistoManager;
class PHCompositeNode;
class PHG4Hit;
class TFile;
class TH1;
class TH2;
class TNtuple;

class TimingNtuple: public SubsysReco
{
 public:

  //! constructor
  TimingNtuple( const std::string &name = "TimingNtuple", const std::string &filename = "TimingNtuple.root" );

  //! destructor
  virtual ~TimingNtuple();

  //! full initialization
  int Init(PHCompositeNode *);

  //! event processing method
  int process_event(PHCompositeNode *);

  //! end of run method
  int End(PHCompositeNode *);

  void AddNode(const std::string &name, const int detid=0);

protected:
  double get_dtotal(const PHG4Hit *hit, const double phi, const double theta);
  int nblocks;
  Fun4AllHistoManager *hm;
  std::vector<TH1 *> nhits;
  std::vector<TH1 *> eloss;
  //  std::vector<TH2 *> nhit_edep;
  std::string _filename;
  std::set<std::string> _node_postfix;
  std::map<std::string, int> _detid;
  TNtuple *ntup;
  TNtuple *ntupt;
  TFile *outfile;
};

#endif 
