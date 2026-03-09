#ifndef ELIMINATEBACKGROUND_H
#define ELIMINATEBACKGROUND_H

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <ostream>
#include <iostream>
// Forward declarations
class PHCompositeNode;
class TH1;
class TH2;

class EliminateBackground : public SubsysReco
{
 public:
  //! constructor
  EliminateBackground(const std::string& name = "EliminateBackground");

  //! destructor
  ~EliminateBackground() override;

  //! full initialization
  int Init(PHCompositeNode*) override;
  int InitRun(PHCompositeNode*) override;

  //! event processing method
  int process_event(PHCompositeNode*) override;

  //! end of run method
  int End(PHCompositeNode*) override;

 private:
  TH2* hcal{nullptr};
  TH1* hcal_phi{nullptr};
  TH1* hcal_phi_consec{nullptr};

  float HCAL_TOWER_ENERGY_CUT{0.03};
  float CONSECUTIVE_ENERGY_CUT{0.03};

  float CONSECUTIVE_COUNT_CUT{6};
  float COUNT_CUT{8};
};

#endif
