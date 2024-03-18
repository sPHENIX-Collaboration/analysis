// Tell emacs that this is a C++ source
//  -*- C++ -*-.

#ifndef MBDINFO_H
#define MBDINFO_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <TMath.h>


class TFile;
class TTree;
class TNtuple;

class PHCompositeNode;

class MBDinfo : public SubsysReco
{
 public:

  MBDinfo(const std::string &name = "MBDinfo");

  ~MBDinfo() override;

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
 double StoreMBDInfo(std::vector < float > _m);
  TNtuple *mbd;
  std::vector < float > _f;
  float mtower_info[5] = {};
  int _event;



};

#endif // MBDINFO_H
