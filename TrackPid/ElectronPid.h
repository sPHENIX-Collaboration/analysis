/*!
 *  \file		PHTruthClustering.h
 *  \brief		Clustering using truth info
 *  \author		Tony Frawley <afrawley@fsu.edu>
 */

#ifndef TRACKRECO_PHTRUTHCLUSTERING_H
#define TRACKRECO_PHTRUTHCLUSTERING_H

#include <fun4all/SubsysReco.h>

#include "TrackPidAssoc.h"

// rootcint barfs with this header so we need to hide it
#include <gsl/gsl_rng.h>


#include <string>             // for string
#include <vector>
#include <map>
#include <set>
#include <memory>

// forward declarations
class PHCompositeNode;
class SvtxTrackMap;

class ElectronPid  : public SubsysReco
{
public:
  ElectronPid(const std::string &name = "ElectronPid");
  virtual ~ElectronPid();

  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

private:
/// fetch node pointers
int GetNodes(PHCompositeNode *topNode);

 TrackPidAssoc *_track_pid_assoc;
 SvtxTrackMap *_track_map;

 TTree *PID_tracktree;
 
 void initializeVariables();
 void initializeTrees();

 double PID_tr_p;
 double PID_tr_pt;

 double PID_cemcdphi; 
 double PID_cemcdeta; 
 double PID_cemce3x3; 
 
 double PID_hcalindphi; 
 double PID_hcalindeta; 
 double PID_hcaline3x3;

 double PID_hcaloute3x3;

 double PID_EcemcOP;
 double PID_EcemcOP_cut;
 
 double PID_EhcalOP;
 double PID_EhcalOP_cut;


};

#endif
