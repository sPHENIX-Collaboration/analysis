// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOONLY_H
#define CALOONLY_H

#include <fun4all/SubsysReco.h>

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h> 
#include <calobase/RawClusterContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertexMap.h>
#include <globalvertex/SvtxVertex.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase_historic/TrackSeed.h>
#include <trackreco/ActsPropagator.h>

#include <Acts/Geometry/GeometryIdentifier.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>
#include <Acts/MagneticField/MagneticFieldProvider.hpp>
#include <Acts/Surfaces/CylinderSurface.hpp>
#include <Acts/Surfaces/PerigeeSurface.hpp>
#include <Acts/Geometry/TrackingGeometry.hpp>

#include <CLHEP/Vector/ThreeVector.h>
#include <math.h>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>

#include <string>
#include <vector>

class PHCompositeNode;
class TH1;
class TH2;
class TFile;
class TTree;

class CaloOnly : public SubsysReco
{
 public:

  CaloOnly(const std::string &name = "CaloOnly", const std::string &file = "output.root");

  ~CaloOnly() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  void ResetTreeVectors();
  void FillTree();

private:
    std::string _outfilename;
    TFile *_outfile = nullptr;
    TTree *_tree = nullptr;

    std::vector<float> _mbd_z;

    std::vector<float> _run_test;
    // EMCal tower vectors
    std::vector<float> _emcalgeo_id;
    std::vector<float> _emcalgeo_phibin;
    std::vector<float> _emcalgeo_etabin;

    std::vector<float> _emcal_e;
    std::vector<float> _emcal_phi;
    std::vector<float> _emcal_eta;
    std::vector<int>   _emcal_iphi;
    std::vector<int>   _emcal_ieta;
    std::vector<float> _emcal_time;
    std::vector<float> _emcal_chi2;
    std::vector<float> _emcal_pedestal;
    // IHCAL tower vectors
    std::vector<float> _ihcal_e;
    std::vector<float> _ihcal_phi;
    std::vector<float> _ihcal_eta;
    std::vector<int>   _ihcal_iphi;
    std::vector<int>   _ihcal_ieta;
    
    std::vector<float> _ihcal_time;
    std::vector<float> _ihcal_chi2;
    std::vector<float> _ihcal_pedestal;
    // OHCAL tower vectors
    std::vector<float> _ohcal_e;
    std::vector<float> _ohcal_phi;
    std::vector<float> _ohcal_eta;
    std::vector<int>   _ohcal_iphi;
    std::vector<int>   _ohcal_ieta;
    std::vector<float> _ohcal_time;
    std::vector<float> _ohcal_chi2;
    std::vector<float> _ohcal_pedestal;

    // EMCal cluster information
    std::vector<int>   _emcal_cluster_id;
    std::vector<float> _emcal_cluster_e;
    std::vector<float> _emcal_cluster_phi;
    std::vector<float> _emcal_cluster_eta;
    std::vector<float> _emcal_cluster_x;
    std::vector<float> _emcal_cluster_y;
    std::vector<float> _emcal_cluster_z;
    std::vector<float> _emcal_cluster_R;
    std::vector<float> _emcal_cluster_ecore;
    std::vector<float> _emcal_cluster_chi2;
    std::vector<float> _emcal_cluster_prob;
    // EMCal full cluster corrections
    std::vector<float> _emcal_clusfull_e;
    std::vector<float> _emcal_clusfull_eta;
    std::vector<float> _emcal_clusfull_phi;
    std::vector<float> _emcal_clusfull_x;
    std::vector<float> _emcal_clusfull_y;
    std::vector<float> _emcal_clusfull_z;
    std::vector<float> _emcal_clusfull_R;
    std::vector<float> _emcal_clusfull_pt;
    // EMCal core cluster corrections
    std::vector<float> _emcal_cluscore_e;
    std::vector<float> _emcal_cluscore_eta;
    std::vector<float> _emcal_cluscore_phi;
    std::vector<float> _emcal_cluscore_x;
    std::vector<float> _emcal_cluscore_y;
    std::vector<float> _emcal_cluscore_z;
    std::vector<float> _emcal_cluscore_R;
    std::vector<float> _emcal_cluscore_pt;

    RawTowerGeomContainer* towerGeom;

    RawClusterContainer * clustersEM;
    RawClusterContainer * clustersIH;
    RawClusterContainer * clustersOH;

    TowerInfoContainer * EMCAL_Container;
    TowerInfoContainer * IHCAL_Container;
    TowerInfoContainer * OHCAL_Container;

    RawTowerGeomContainer * EMCalGeo;
    RawTowerGeomContainer * IHCalGeo;
    RawTowerGeomContainer * OHCalGeo;

    RawTowerContainer * EMCal_RawTowerContainer;

    double m_emcal_e_low_cut = 0.1;
};

#endif // CALOONLY_H
