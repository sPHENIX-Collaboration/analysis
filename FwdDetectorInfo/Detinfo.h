// Tell emacs that this is a C++ source
//  -*- C++ -*-.

#ifndef DETINFO_H
#define DETINFO_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TMath.h>


class TFile;
class TTree;
class TNtuple;
class TH1;

class PHCompositeNode;
class CDBTTree;

class Detinfo : public SubsysReco
{
 public:

  Detinfo(const std::string &name = "Detinfo");

  ~Detinfo() override;

  int Init(PHCompositeNode *topNode) override;
  int InitRun(PHCompositeNode *topNode) override;
  int process_event(PHCompositeNode *topNode) override;
  int ResetEvent(PHCompositeNode *topNode) override;
  int EndRun(const int runnumber) override;
  int End(PHCompositeNode *topNode) override;
  int Reset(PHCompositeNode * /*topNode*/) override;
  
  void set_filename(const char* file)
  { if(file) _outfile_name = file; }
    
 private:
 std::string _outfile_name;
 double StoreZDCInfo(std::vector < float > _z);
 double StoreEPDInfo(std::vector < float > _e);
 double StoreMBDInfo(std::vector < float > _m); 

  TNtuple *zdc;
  TNtuple *sepd;
  TNtuple *mbd;

  TNtuple *coor;

  TH1* hepdtime;
  TH1* hzdctime;
  TH1* hzvertex;


  std::vector < float > _f; //zdc
  std::vector < float > _g; //sepd
  std::vector < float > _b; //mbd


  float ztower_info[32] = {};
  float etower_info[1536] = {};
  float mtower_info[256] = {};
  float event_info[5] = {};

  int _event;

  CDBTTree *cdbttree = nullptr;

};

#endif // DETINFO_H
