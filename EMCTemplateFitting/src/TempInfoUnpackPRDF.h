// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef PROTOTYPE4_TEMPINFOUNPACKPRDF_H
#define PROTOTYPE4_TEMPINFOUNPACKPRDF_H

#include <fun4all/SubsysReco.h>

#include <ctime>  // for time_t

class Packet;
class PHCompositeNode;
class RawTowerContainer;

class TempInfoUnpackPRDF : public SubsysReco
{
 public:
  TempInfoUnpackPRDF();
  virtual ~TempInfoUnpackPRDF() {}

  int InitRun(PHCompositeNode *topNode);

  int process_event(PHCompositeNode *topNode);

  void CreateNodeTree(PHCompositeNode *topNode);

 protected:
  int addPacketInfo(Packet *p, PHCompositeNode *topNode, const time_t etime,
                    const int evtnr);

  RawTowerContainer *hcalin_temperature;
  RawTowerContainer *hcalout_temperature;
  RawTowerContainer *emcal_temperature;
};

#endif
