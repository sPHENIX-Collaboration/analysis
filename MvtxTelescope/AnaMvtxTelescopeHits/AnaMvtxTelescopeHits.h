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
#include <map>

#include "TMath.h"

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;
class SvtxVertex;
class PHCompositeNode;
class PHG4TruthInfoContainer;
class SvtxClusterMap;
class SvtxCluster;
class SvtxEvalStack;
class JetMap;

class TFile;
class TTree;
class TH1D;
class TH2D;


typedef std::vector<SvtxCluster*> ClusVec;
typedef std::vector<ClusVec> TrkVec;
typedef std::multimap<unsigned int, SvtxCluster*> LyrClusMap;


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

  // int GetTrackCandidates(LyrClusMap* clusmap, TrkVec* trkvec);

  // void LinkClusters(ClusVec* trk, unsigned int next_lyr, LyrClusMap* clusmap, TrkVec* trkvec);

  // ClusVec ChooseBestTrk(TrkVec* trkcnd);

  // double FitTrk(ClusVec* trk);

  double CalcSlope(double x0, double y0, double x1, double y1);
  double CalcIntecept(double x0, double y0, double m);
  double CalcProjection(double x, double m, double b);
  //-- Nodes
  SvtxClusterMap* _clustermap;

  //-- Flags


  //-- Output
  std::string _foutname;  
  TFile *_f;

  TH1D* hlayer;
  TH1D* hsize_phi[4];
  TH1D* hsize_z[4]; 
  TH2D* hphiz[4];
  TH1D* hdphi[4];
  TH1D* hdz[4];


  //-- internal variables
  int _ievent;


};

#endif // __AnaMvtxTelescopeHits_H__
