/*!
 *  \file     AnaMvtxTelescopeHits.h
 *  \brief    Analyze Mvtx 4 ladder cosmic telescope in simulations
 *  \details  Analyze simulations of hits in the 4 ladder
 *            Mvtx cosimic ray telescope
 *  \author   Darren McGlinchey
 */

#ifndef __AnaMvtxTelescopeHits_H__
#define __AnaMvtxTelescopeHits_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
// #include <GenFit/GFRaveVertex.h>
// #include <GenFit/Track.h>
#include <string>
#include <vector>

#include "TMath.h"


namespace PHGenFit {
  class Track;
} /* namespace PHGenFit */

namespace genfit {
  class GFRaveVertexFactory;
  class Track;
  class GFRaveVertex;
} /* namespace genfit */

namespace PHGenFit {
  class Fitter;
} /* namespace PHGenFit */

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;
class SvtxVertex;
class PHCompositeNode;
class PHG4TruthInfoContainer;
class SvtxClusterMap;
class SvtxEvalStack;
class JetMap;

class TFile;
class TTree;
class TH1D;
class TH2D;

class AnaMvtxTelescopeHits: public SubsysReco {

public:

  AnaMvtxTelescopeHits(const std::string &name = "AnaMvtxTelescopeHits",
               const std::string &ofName = "out.root");

  int Init(PHCompositeNode*);
  int InitRun(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

private:

  //-- Functions
  int GetNodes(PHCompositeNode *);   //! Get all nodes

  // PHGenFit::Track* MakeGenFitTrack(PHCompositeNode *, 
  //                                  const SvtxTrack* intrack, 
  //                                  const SvtxVertex* vertex);

  //-- Nodes
  SvtxClusterMap* _clustermap;

  //-- Flags
  // TODO: Make setters & getters
  bool _verbose;                     //! Verbose printing

  //-- Output
  std::string _foutname;  
  TFile *_f;

  TH1D* hlayer;
  TH1D* hsize_phi[4];
  TH1D* hsize_z[4]; 
  TH2D* hphiz[4];


  //-- internal variables
  int _ievent;

  int _primary_pid_guess;            //! Particle ID guess for GenFit

  std::string _track_fitting_alg_name;
  PHGenFit::Fitter* _fitter;

};

#endif // __AnaMvtxTelescopeHits_H__
