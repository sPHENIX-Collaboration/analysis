// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EMCAL_TOWER_MASKING_H
#define EMCAL_TOWER_MASKING_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
class PHCompositeNode;

class Emcal_Tower_Masking : public SubsysReco
{
 public:

  Emcal_Tower_Masking(const std::string &name = "Emcal_Tower_Masking");

  ~Emcal_Tower_Masking() override;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;
  void Print(const std::string &what = "ALL") const override;
  void CreateNodeTree(PHCompositeNode *topNode);

  void addhottower(int etabin, int phibin)
  {
    etamasklist.push_back(etabin);
    phimasklist.push_back(phibin);
  }
  void addpacketmask(int packetnum)
  {
    packetmasklist.push_back(packetnum);
  }
 private:
  
  std::vector<unsigned int> etamasklist;
  std::vector<unsigned int> phimasklist;
  std::vector<unsigned int> packetmasklist;
};

#endif // MOCKTOWERCALIB_H
