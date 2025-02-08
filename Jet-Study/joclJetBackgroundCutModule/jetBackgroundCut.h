#ifndef JETBACKGROUNDCUT_H
#define JETBACKGROUNDCUT_H

#include <fun4all/SubsysReco.h>
#include <string>
#include <cmath>
#include <phool/recoConsts.h>
#include <globalvertex/GlobalVertex.h>
class PHCompositeNode;
class CentralityInfo;
class jetBackgroundCut : public SubsysReco
{
 public:

  jetBackgroundCut(const std::string jetNodeName, const std::string &name = "jetBackgroundCutModule", const int debug = 0, const bool doAbort = 0, GlobalVertex::VTXTYPE vtxtype = GlobalVertex::MBD);

  virtual ~jetBackgroundCut();

  bool failsLoEmFracETCut(float emFrac, float ET, bool dPhiCut, bool isDijet)
  {
    return (emFrac < 0.1 && ET > (50*emFrac+20)) && (dPhiCut || !isDijet);
  }

  bool failsHiEmFracETCut(float emFrac, float ET, bool dPhiCut, bool isDijet)
  {
    return (emFrac > 0.9 && ET > (-50*emFrac+70)) && (dPhiCut || !isDijet);
  }

  bool failsIhFracCut(float emFrac, float ohFrac)
  {
    return emFrac + ohFrac < 0.65;
  }

  bool failsdPhiCut(float dPhi, bool isDijet)
  {
    return dPhi < 3*M_PI/4 && isDijet;
  }

  int Init(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;

  int ResetEvent(PHCompositeNode *topNode) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;


 private:
  recoConsts *_rc;
  bool _doAbort;
  std::string _name;
  int _debug;
  bool _missingInfoWarningPrinted = false;
  std::string _jetNodeName;
  GlobalVertex::VTXTYPE _vtxtype;
};

#endif // R24TREEMAKER
