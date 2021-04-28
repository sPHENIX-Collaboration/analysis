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

#include <TFile.h>
#include <TNtuple.h>

// forward declarations
class PHCompositeNode;
class SvtxTrackMap;

class SvtxTrack;

class ElectronPid  : public SubsysReco
{
public:
  ElectronPid(const std::string &name = "ElectronPid", const std::string &filename = "_ElectronPid.root");
  virtual ~ElectronPid();

  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  /// Set the cemce3x3/p cut limits for electrons; default: 0.0<cemce3x3/p<100.0, means without cuts
  void setEMOPcutlimits(float EMOPlowerlimit, float EMOPhigherlimit) { EMOP_lowerlimit = EMOPlowerlimit; EMOP_higherlimit = EMOPhigherlimit; }

  /// Set the hcaline3x3/cemce3x3 cut limit for electrons; default: hcaline3x3/cemce3x3<100.0, means without cut
  void setHinOEMcutlimit(float HinOEMhigherlimit) { HinOEM_higherlimit = HinOEMhigherlimit; }

  /// Set the pt cut limit for Upsilon decay electrons; default: 0.0<pt<100.0 GeV, means without cut
  void setPtcutlimit(float Ptlowerlimit, float Pthigherlimit) { Pt_lowerlimit = Ptlowerlimit; Pt_higherlimit = Pthigherlimit; }

  /// Set the (hcaline3x3+hcaloute3x3)/p cut lower limit for hadrons; default: 0.0<(hcaline3x3+hcaloute3x3)/p, means without cut
  void setHOPcutlimit(float HOPlowerlimit) { HOP_lowerlimit = HOPlowerlimit; }

  /// Set the track cut limits; default: nmvtx>=0, nintt>=0, ntpc>=0; quality<100
  void setTrackcutlimits(int Nmvtxlowerlimit, int Ninttlowerlimit, int Ntpclowerlimit, int Nqualityhigherlimit) { 
     Nmvtx_lowerlimit = Nmvtxlowerlimit; 
     Nintt_lowerlimit = Ninttlowerlimit; 
     Ntpc_lowerlimit = Ntpclowerlimit;
     Nquality_higherlimit = Nqualityhigherlimit;
  }


  void set_output_ntuple(bool outputntuple) {output_ntuple = outputntuple;}

protected:
  bool output_ntuple;

  TFile* OutputNtupleFile;
  std::string OutputFileName;
  TNtuple* ntpbeforecut; //write ntuple before any cuts
  TNtuple* ntpcutEMOP; //write ntuple with only EMOP cut
  TNtuple* ntpcutEMOP_HinOEM; //write ntuple with EMOP & HinOEM cuts
  TNtuple* ntpcutEMOP_HinOEM_Pt; //write ntuple with EMOP & HinOEM & Pt cuts
  TNtuple* ntpcutEMOP_HinOEM_Pt_read; //write ntuple with EMOP & HinOEM & Pt cuts in the situation of reading back the association map.

  TNtuple* ntpcutHOP; //write ntuple with only HOP cut

  int EventNumber;

private:
/// fetch node pointers
int GetNodes(PHCompositeNode *topNode);

 TrackPidAssoc *_track_pid_assoc;
 SvtxTrackMap *_track_map;

/// A float lower limit for cutting on cemce3x3/p
  float EMOP_lowerlimit;
/// A float higher limit for cutting on cemce3x3/p
  float EMOP_higherlimit;

/// A float higher limit for cutting on hcaline3x3/cemce3x3
  float HinOEM_higherlimit;

/// A float lower limit for cutting on pt
  float Pt_lowerlimit;
/// A float higher limit for cutting on pt
  float Pt_higherlimit;

/// A float lower limit for cutting on (hcaline3x3+hcaloute3x3)/p
  float HOP_lowerlimit;

/// A float lower limit for cutting on nmvtx
  int Nmvtx_lowerlimit;

/// A float lower limit for cutting on nintt
  int Nintt_lowerlimit;

/// A float lower limit for cutting on ntpc
  int Ntpc_lowerlimit;

/// A float higher limit for cutting on quality
  int Nquality_higherlimit;

  unsigned int _nlayers_maps = 3;
  unsigned int _nlayers_intt = 4;
  unsigned int _nlayers_tpc = 48;


};

#endif
