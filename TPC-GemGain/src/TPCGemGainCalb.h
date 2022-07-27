// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TPCGEMGAINCALB_H
#define TPCGEMGAINCALB_H

#include <string>

#include <fun4all/SubsysReco.h>

#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterContainer.h>

class PHCompositeNode;
class SvtxTrackMap;
class SvtxTrack;
class SvtxVertexMap;

class TF1;
class TNtuple;
class TFile;

class TPCGemGainCalb : public SubsysReco
{
 public:

 TPCGemGainCalb(const std::string &name = "TPCGemGainCalb");

  virtual ~TPCGemGainCalb();

  void set_track_map_name(const std::string &map_name) { _track_map_name = map_name; }

 //! run initialization
  int InitRun(PHCompositeNode *topNode);

  //! event processing
  int process_event(PHCompositeNode *topNode);

  int EndRun(PHCompositeNode *topNode);

  //! end of process
int End(PHCompositeNode *topNode);

 protected:
  
 private:

  int GetNodes(PHCompositeNode* topNode);

  std::string _track_map_name;

  SvtxTrackMap *_track_map{nullptr};
  SvtxTrack *_track{nullptr};
  SvtxVertexMap *_vertex_map{nullptr};
  TrkrClusterContainer *_cluster_map;

  TNtuple *ntp{nullptr};
  TFile *fout;

};

#endif // TPCGEMGAINCALB_H
