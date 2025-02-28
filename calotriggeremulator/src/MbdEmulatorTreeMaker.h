
#ifndef CALOEMULATORTREEMAKER_H
#define CALOEMULATORTREEMAKER_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <ffarawobjects/Gl1Packet.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <ffarawobjects/Gl1Packetv2.h>
#include <calotrigger/LL1Out.h>
#include <calotrigger/LL1Outv1.h>
#include <calotrigger/TriggerPrimitive.h>
#include <calotrigger/TriggerPrimitivev1.h>
#include <calotrigger/TriggerPrimitiveContainer.h>
#include <calotrigger/TriggerPrimitiveContainerv1.h>
#include <calotrigger/TriggerDefs.h>

#include "TTree.h"
#include "TFile.h"


class PHCompositeNode;
class TriggerPrimitive;
class TriggerPrimitiveContainer;
class LL1Out;
class TowerInfoContainer;
class MbdEmulatorTreeMaker : public SubsysReco
{
 public:

  MbdEmulatorTreeMaker(const std::string &name = "MbdEmulatorTreeMaker", const std::string &outfilename = "trees_caloemulator.root");

  virtual ~MbdEmulatorTreeMaker();

  int Init(PHCompositeNode *topNode) override;

  int InitRun(PHCompositeNode *topNode) override;

  int process_event(PHCompositeNode *topNode) override;
  
  void GetNodes (PHCompositeNode *topNode);

  int ResetEvent(PHCompositeNode *topNode) override;

  int EndRun(const int runnumber) override;

  int End(PHCompositeNode *topNode) override;

  int Reset(PHCompositeNode * /*topNode*/) override;

  void SetVerbosity(int verbo) ;
    
 private:
  void reset_tree_vars();

  int _verbosity;

  TFile *_f;
  TTree *_tree;
  std::string _foutname;
  std::string _nodename;
    
  int _i_event;
  
  LL1Out *_ll1out_mbd;
  Gl1Packet *_gl1_packet;
  TriggerPrimitive *_trigger_primitive;
  TriggerPrimitiveContainer *_trigger_primitives_mbd;

  unsigned int b_trigger_charge_sum[4*8]{0};
  unsigned int b_trigger_nhit[4]{0};
  unsigned int b_trigger_time_sum[4*4]{0};

  unsigned int b_trigger_south_tavg{0};
  unsigned int b_trigger_south_nhit{0};
  unsigned int b_trigger_north_tavg{0};
  unsigned int b_trigger_north_nhit{0};
  unsigned int b_trigger_vertex_sub{0};
  unsigned int b_trigger_vertex_add{0};
  uint64_t b_gl1_rawvec;
  uint64_t b_gl1_livevec;
  uint64_t b_gl1_scaledvec;
  uint64_t b_gl1_clock;
  uint64_t b_gl1_raw[64]; 
  uint64_t b_gl1_live[64]; 
  uint64_t b_gl1_scaled[64]; 
  float b_vertex_z;
};

#endif 
