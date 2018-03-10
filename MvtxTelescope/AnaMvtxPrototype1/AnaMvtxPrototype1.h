/*!
 *  \file     AnaMvtxPrototype1.h
 *  \brief    Analyze Mvtx 4 ladder cosmic telescope in simulations
 *  \details  Analyze simulations of hits in the 4 ladder
 *            Mvtx cosimic ray telescope
 *  \author   Darren McGlinchey
 */

#ifndef __AnaMvtxPrototype1_H__
#define __AnaMvtxPrototype1_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <map>

#include "TMath.h"

class PHCompositeNode;

class TrkrClusterContainer;
class TrkrCluster;

class MvtxStandaloneTracking;

class TFile;
class TTree;
class TH1D;
class TH2D;


typedef std::vector<TrkrCluster*> ClusVec;
typedef std::vector<ClusVec> TrkVec;
typedef std::multimap<unsigned int, TrkrCluster*> LyrClusMap;

struct mis {
  double dx;
  double dy;
  double dz;
};

class AnaMvtxPrototype1: public SubsysReco {

public:

  AnaMvtxPrototype1(const std::string &name = "AnaMvtxPrototype1",
               const std::string &ofName = "out.root");

  int Init(PHCompositeNode*);
  int InitRun(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

  /**
   * Misalign chip in x, y, z (pixel index)
   */
  void MisalignLayer(int lyr, double dx, double dy, double dz);

private:

  //-- Functions
  int GetNodes(PHCompositeNode *);   //! Get all nodes


  double CalcSlope(double x0, double y0, double x1, double y1);
  double CalcIntecept(double x0, double y0, double m);
  double CalcProjection(double x, double m, double b);

  void Misalign();
  void PrintMisalign();

  //-- Nodes
  TrkrClusterContainer* clusters_;

  //-- Flags


  //-- Output
  std::string _foutname;  
  TFile *_f;

  TH1D* hlayer;
  TH1D* hsize[4];
  TH1D* hsize_phi[4];
  TH1D* hsize_z[4]; 
  TH2D* hxz[4];
  TH1D* hdx[4];
  TH1D* hdz[4];

  TH1D* htrk;
  TH1D* htrk_dx[4];
  TH1D* htrk_dz[4];
  TH1D* htrk_chi2xy;
  TH1D* htrk_chi2zy;

  TH1D* htrk_cut;
  TH1D* htrk_cut_dx[4];
  TH1D* htrk_cut_dz[4];
  TH1D* htrk_cut_chi2xy;
  TH1D* htrk_cut_chi2zy;


  //-- internal variables
  int _ievent;
  std::map<int, mis> _misalign; /// map for misaligning clusters
  MvtxStandaloneTracking* mvtxtracking_;

};

#endif // __AnaMvtxPrototype1_H__
