// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef G4HITSHIFT_H
#define G4HITSHIFT_H

#include <fun4all/SubsysReco.h>

#include <string>

class PHCompositeNode;

class g4hitshift : public SubsysReco
{
 public:

  g4hitshift(const std::string &name = "g4hitshift");

  ~g4hitshift() override;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
};

#endif // G4HITSHIFT_H
