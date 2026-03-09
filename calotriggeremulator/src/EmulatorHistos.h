#ifndef EMULATORHISTOS_H
#define EMULATORHISTOS_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

// Forward declarations
class Fun4AllHistoManager;
class PHCompositeNode;
class TFile;
class TNtuple;
class TTree;
class TH2F;
class TH1F;
class TH1;
class TProfile2D;

class EmulatorHistos : public SubsysReco
{
 public:
  //! constructor
  EmulatorHistos(const std::string& name = "EmulatorHistos");

  //! destructor
  ~EmulatorHistos() override = default;

  //! full initialization
  int Init(PHCompositeNode*) override;
  int End(PHCompositeNode*) override;
  //! event processing method
  int process_event(PHCompositeNode*) override;
  int process_towers(PHCompositeNode*);
  int process_ll1out(PHCompositeNode*);

  void set_debug(bool debug) { m_debug = debug; }

 private:

  int Getpeaktime(TH1* h);

  bool m_debug{0};
  int i_event{0};
  int _range{1};
};

#endif
