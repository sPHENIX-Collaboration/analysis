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

//class TFile;
//class TNtuple;
//class SvtxTrack;

class ElectronPid  : public SubsysReco
{
public:
  ElectronPid(const std::string &name = "ElectronPid", const std::string &filename = "_ElectronPid.root");
  virtual ~ElectronPid();

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  /// Set the cemce3x3/p cut limits
  void setEOPcutlimits(float EOPlowerlimit, float EOPhigherlimit) { EOP_lowerlimit = EOPlowerlimit;EOP_higherlimit = EOPhigherlimit; }
  /// Set the (hcaline3x3+hcaloute3x3)/p cut lower limit
  void setHOPcutlimit(float HOPlowerlimit) { HOP_lowerlimit = HOPlowerlimit; }

  void set_output_ntuple(bool outputntuple) {output_ntuple = outputntuple;}

protected
  bool output_ntuple;

  TFile* OutputNtupleFile;
  std::string OutputFileName;
  TNtuple* ntp2;

  int EventNumber;

private:
/// fetch node pointers
int GetNodes(PHCompositeNode *topNode);

 TrackPidAssoc *_track_pid_assoc;
 SvtxTrackMap *_track_map;

/// A float lower limit for cutting on cemce3x3/p
  float EOP_lowerlimit;

/// A float higher limit for cutting on cemce3x3/p
  float EOP_higherlimit;

/// A float lower limit for cutting on (hcaline3x3+hcaloute3x3)/p
  float HOP_lowerlimit;

 // bool output_ntuple;

 // TFile* OutputNtupleFile;
 // std::string OutputFileName;
 // TNtuple* ntp2;

 // int EventNumber;



};

#endif
