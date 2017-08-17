#ifndef SigmaTimingNtuple_h__
#define SigmaTimingNtuple_h__

#include <fun4all/SubsysReco.h>

#ifndef __CINT__
#include <gsl/gsl_rng.h>
#endif

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

class SigmaTimingNtuple: public SubsysReco
{
 public:

  //! constructor
  SigmaTimingNtuple( const std::string &name = "SigmaTimingNtuple", const std::string &filename = "SigmaTimingNtuple.root" );

  //! destructor
  virtual ~SigmaTimingNtuple();

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
  TNtuple *ntupprim;
  TNtuple *ntupsec;
  TNtuple *ntupt;
  TNtuple *ntupsigma;
  TFile *outfile;
#ifndef __CINT__
  gsl_rng *RandomGenerator;
#endif
};

#endif 
