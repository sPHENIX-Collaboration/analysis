/*!
 *  \file     AnaMvtxTestBeam2019.h
 *  \brief    Analyze Mvtx 4 stave telescope from 2019 fermilab testbeam
 *  \details
 *  \author   Yasser Corrales Morales and Darren McGlinchey
 *  \ref      AnaMvtxPrototype1.h
 */

#ifndef __AnaMvtxTestBeam2019_H__
#define __AnaMvtxTestBeam2019_H__

// --- need to check all these includes...
#include <mvtxprototype2/MvtxPrototype2Geom.h>
#include <mvtxprototype2/MvtxStandaloneTracking.h>

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>
#include <map>

class PHCompositeNode;

class TrkrHit;
class TrkrHitSetContainerv1;
class TrkrClusterContainerv1;
class TrkrCluster;

class TFile;
class TH1D;
class TH2F;
class TList;
class TTree;

typedef std::vector<TrkrCluster*> ClusVec;
typedef std::vector<ClusVec> TrkVec;
typedef std::multimap<unsigned int, TrkrCluster*> LyrClusMap;

class AnaMvtxTestBeam2019 : public SubsysReco {

public:

  AnaMvtxTestBeam2019(const std::string &name = "AnaMvtxTestBeam2019",
                      const std::string &ofName = "out.root");

  virtual ~AnaMvtxTestBeam2019()
  {
    if (m_mvtxtracking)
      delete m_mvtxtracking;
  }

  int Init(PHCompositeNode*);
  int InitRun(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

  MvtxStandaloneTracking *getStandaloneTracking()
  { return m_mvtxtracking; }

  void set_out_filename(const char* _fname) { m_foutname = _fname; }
  void set_do_tracking( bool _do ) { do_tracking = _do; }

  void set_ref_align_stave(int _ref_stave) { m_ref_align_stave = _ref_stave; }
private:

  //-- Functions
  int GetNodes(PHCompositeNode *);   //! Get all nodes

  double CalcSlope(double x0, double y0, double x1, double y1);
  double CalcIntecept(double x0, double y0, double m);
  double CalcProjection(double x, double m, double b);

  //-- Nodes
  TrkrHitSetContainerv1  *m_hits;
  TrkrClusterContainerv1 *m_clusters;

  //-- Output
  TList* m_lout_clusterQA;
  TList* m_lout_tracking;

  TH1D* h1d_nevents;            //! number of events
  TH1D* h1d_hit_layer;          //! number of hits per layers
  TH1D* h1d_clu_layer;          //! number of clusters per layers
  TH1D* h1d_clu_map;            //! cluster map
  TH2F* h2f_hit_map[NLAYER];
  TH2F* h2f_clu_xz[NLAYER];

  TH1D* h1d_clu_mean_dx[NLAYER];
  TH1D* h1d_clu_mean_dz[NLAYER];
  TH1D* h1d_clu_size[NLAYER];
  TH1D* h1d_clu_size_phi[NLAYER];
  TH1D* h1d_clu_size_z[NLAYER];

  TH1D* htrk;
  TH1D* htrk_clus;
  TH1D* htrk_dz[NLAYER];
  TH1D* htrk_dx[NLAYER];
  TH1D* htrk_chi2xy;
  TH1D* htrk_chi2zy;

  TH1D* htrk_cut;
  TH1D* htrk_cut_clus;
  TH1D* htrk_cut_dx[NLAYER];
  TH1D* htrk_cut_dz[NLAYER];
  TH1D* htrk_cut_chi2xy;
  TH1D* htrk_cut_chi2zy;

  //-- internal variables
  std::string m_foutname;

  //-- Flags
  bool do_tracking;

  int m_ievent;
  int m_ref_align_stave;

  MvtxStandaloneTracking* m_mvtxtracking;

};

#endif // __AnaMvtxTestBeam2019_H__
