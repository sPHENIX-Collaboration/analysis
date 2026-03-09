/*!
 *  \file     AnaMvtxTestBeam2019.C
 *  \brief    Analyze Mvtx 4 stave telescope from 2019 fermilab beam test
 *  \author   Yasser Corrales Morales and Darren McGlinchey
 *  \ref      AnaMvtxPrototype1.C
 */

#include "AnaMvtxTestBeam2019.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/getClass.h>
// #include <phool/phool.h>
// #include <phool/PHCompositeNode.h>
// #include <phool/PHIODataNode.h>
// #include <phool/PHNodeIterator.h>

// #include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterv2.h>
#include <trackbase/TrkrClusterContainerv1.h>
#include <trackbase/TrkrHitSetContainerv1.h>
#include <trackbase/TrkrHitSetv1.h>

// #include <TTree.h>
#include <TFile.h>
// #include <TLorentzVector.h>
#include <TH1D.h>
#include <TH2D.h>
// #include <TVectorD.h>
// #include <TMatrixD.h>
// #include <TDecompSVD.h>

// #include <iostream>
// #include <map>
// #include <memory>
// #include <utility>
// #include <vector>


#define LogDebug(exp)   std::cout<<"DEBUG: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)   std::cout<<"ERROR: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp) std::cout<<"WARNING: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"


//______________________________________________________________________________
AnaMvtxTestBeam2019::AnaMvtxTestBeam2019(const std::string &name,
                                         const std::string &ofName):
  SubsysReco(name),
  m_hits(nullptr),
  m_clusters(nullptr),
  m_lout_clusterQA(nullptr),
  m_lout_tracking(nullptr),
  m_foutname(ofName),
  do_tracking(false),
  m_ievent(0),
  m_ref_align_stave(1)
{
  h1d_nevents = NULL;
  h1d_hit_layer = NULL;
  h1d_clu_layer = NULL;
  h1d_clu_map = NULL;

  for (int il = 0; il < NLAYER; il++)
  {
    h2f_hit_map[il] = NULL;
    h2f_clu_xz[il] = NULL;
    h1d_clu_mean_dz[il] = NULL;
    h1d_clu_mean_dx[il] = NULL;
    h1d_clu_size[il] = NULL;
    h1d_clu_size_phi[il] = NULL;
    h1d_clu_size_z[il] = NULL;

    htrk_dz[il] = NULL;
    htrk_dx[il] = NULL;

    htrk_cut_dz[il] = NULL;
    htrk_cut_dx[il] = NULL;
  }

  htrk = NULL;
  htrk_clus = NULL;
  htrk_chi2xy = NULL;
  htrk_chi2zy = NULL;
  htrk_cut = NULL;
  htrk_cut_clus = NULL;
  htrk_cut_chi2xy = NULL;
  htrk_cut_chi2zy = NULL;

  m_mvtxtracking = new MvtxStandaloneTracking();
}


//______________________________________________________________________________
int
AnaMvtxTestBeam2019::Init(PHCompositeNode *topNode)
{
  int chipColor[] = {kBlue, kRed, kGreen+2, kMagenta+2};

  //-- Initialize list containe for clusterQA
  m_lout_clusterQA = new TList();
  m_lout_clusterQA->SetName("ClusterQA");
  m_lout_clusterQA->SetOwner(true);

  //-- Initialize histograms
  h1d_nevents = new TH1D("h1d_nevents", ";;Nevts", 6, -.5, 5.5);
  h1d_hit_layer = new TH1D("h1d_hit_layer",
                           "# hits/layer;layer;Counts", 6, -.5, 5.5);
  h1d_clu_layer = new TH1D("h1d_clu_layer",
                           "# clusters/layer;layer;Counts", 6, -.5, 5.5);
  h1d_clu_map = new TH1D("h1d_clu_map",
                           "cluster map;cluster_map;Counts", 16, -.5, 15.5);
  m_lout_clusterQA->Add(h1d_nevents);
  m_lout_clusterQA->Add(h1d_hit_layer);
  m_lout_clusterQA->Add(h1d_clu_layer);
  m_lout_clusterQA->Add(h1d_clu_map);

  for (int il = 0; il < NLAYER; il++)
  {
    h2f_hit_map[il] = new TH2F(Form("h2d_hit_map_l%i", il),
                          ";col [px];row [px]",
                          9 * NCOL, -.5, (9 * NCOL) - .5,
                          NROW, -.5, NROW - .5);

    h2f_clu_xz[il] = new TH2F(Form("h2d_clu_xz_l%i", il),
                         ";Z [cm];X [cm]",
                         3E4, -15., 15.,
                         2E3, -1., 1.);

    h1d_clu_mean_dz[il] = new TH1D(Form("h1d_clu_mean_dz_l%i", il),
                         ";#DeltaZ (cm);Counts/30 #mum",
                         3E3, -1.5, 1.5);
    h1d_clu_mean_dz[il]->SetLineWidth(2);
    h1d_clu_mean_dz[il]->SetLineColor(chipColor[il]);

    h1d_clu_mean_dx[il] = new TH1D(Form("h1d_clu_mean_dx_l%i", il),
                         ";#DeltaX (cm);Counts/30 #mum",
                         3E3, -1.5, 1.5);
    h1d_clu_mean_dx[il]->SetLineWidth(2);
    h1d_clu_mean_dx[il]->SetLineColor(chipColor[il]);

    h1d_clu_size[il] = new TH1D(Form("h1d_clu_size_l%i", il),
                           ";cluster size [px];Counts", 100, -.5, 99.5);

    h1d_clu_size_phi[il] = new TH1D(Form("h1d_clu_size_phi_l%i", il),
                               ";cluster size phi [px];Counts", 100, -.5, 99.5);

    h1d_clu_size_z[il] = new TH1D(Form("h1d_clu_size_z_l%i", il),
                               ";cluster size z [px];Counts", 100, -.5, 99.5);

    m_lout_clusterQA->Add(h2f_hit_map[il]);
    m_lout_clusterQA->Add(h2f_clu_xz[il]);
    m_lout_clusterQA->Add(h1d_clu_mean_dz[il]);
    m_lout_clusterQA->Add(h1d_clu_mean_dx[il]);
    m_lout_clusterQA->Add(h1d_clu_size[il]);
    m_lout_clusterQA->Add(h1d_clu_size_phi[il]);
    m_lout_clusterQA->Add(h1d_clu_size_z[il]);
  } // il

  if ( do_tracking )
  {
    m_lout_tracking = new TList();
    m_lout_tracking->SetName("Tracking");
    m_lout_tracking->SetOwner(true);

    //-- results from tracking
    htrk = new TH1D("htrk", ";trks / event", 16, -0.5, 15.5);
    htrk_clus = new TH1D("htrk_clus", ";#cluster/track;Counts", 10, -.5, 9.5);
    htrk_cut = new TH1D("htrk_cut", ";trks / event", 16, -0.5, 15.5);
    htrk_cut_clus = new TH1D("htrk_cut_clus", ";#cluster/track;Counts", 5, -.5, 4.5);
    m_lout_tracking->Add(htrk);
    m_lout_tracking->Add(htrk_clus);
    m_lout_tracking->Add(htrk_cut);
    m_lout_tracking->Add(htrk_cut_clus);

    for (int il = 0; il < 4; il++)
    {
      htrk_dx[il] = new TH1D(Form("htrk_dx_l%i", il),
                             ";track dx [cm]",
                             2000, -.5, .5);

      htrk_dz[il] = new TH1D(Form("htrk_dz_l%i", il),
                             ";track dz [cm]",
                             2000, -.5, .5);

      htrk_cut_dx[il] = new TH1D(Form("htrk_cut_dx_l%i", il),
                                 ";track dx [pixels]",
                                 2000, -.5, .5);

      htrk_cut_dz[il] = new TH1D(Form("htrk_cut_dz_l%i", il),
                                 ";track dz [cm]",
                                 2000, -.5, .5);

      m_lout_tracking->Add(htrk_dx[il]);
      m_lout_tracking->Add(htrk_dz[il]);
      m_lout_tracking->Add(htrk_cut_dx[il]);
      m_lout_tracking->Add(htrk_cut_dz[il]);
    }

    htrk_chi2xy = new TH1D("htrk_chi2xy",
                           ";track chi2/ndf in x vs y",
                           500, 0, 100);

    htrk_chi2zy = new TH1D("htrk_chi2zy",
                           ";track chi2/ndf in z vs y",
                           500, 0, 100);

    htrk_cut_chi2xy = new TH1D("htrk_cut_chi2xy",
                               ";track chi2/ndf in x vs y",
                               500, 0, 100);

    htrk_cut_chi2zy = new TH1D("htrk_cut_chi2zy",
                               ";track chi2/ndf in z vs y",
                               500, 0, 100);

    m_lout_tracking->Add(htrk_chi2xy);
    m_lout_tracking->Add(htrk_chi2zy);
    m_lout_tracking->Add(htrk_cut_chi2xy);
    m_lout_tracking->Add(htrk_cut_chi2zy);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}


//______________________________________________________________________________
int
AnaMvtxTestBeam2019::InitRun(PHCompositeNode *topNode)
{
  //-- set counters
  m_ievent = -1; // since we count at the beginning of the event, start at -1

  return Fun4AllReturnCodes::EVENT_OK;
}


//______________________________________________________________________________
int
AnaMvtxTestBeam2019::process_event(PHCompositeNode *topNode)
{
  if ( Verbosity() > VERBOSITY_MORE )
    std::cout << "AnaMvtxTestBeam2019::process_event()" << std::endl;

  // count event
  m_ievent++;
  h1d_nevents->Fill(0);

  //------
  //-- get the nodes
  //------
  int retval = GetNodes(topNode);
  if ( retval != Fun4AllReturnCodes::EVENT_OK )
    return retval;

  h1d_nevents->Fill(1);

  // loop over each MvtxHitSet object (chip)
  unsigned int n_hits = 0;
  auto hitsetrange = m_hits->getHitSets(TrkrDefs::TrkrId::mvtxId);
  for (auto hitsetitr = hitsetrange.first;
      hitsetitr != hitsetrange.second;
      ++hitsetitr)
  {
    auto hitset = hitsetitr->second;

    auto layer = TrkrDefs::getLayer(hitset->getHitSetKey());
    auto stave = MvtxDefs::getStaveId(hitset->getHitSetKey());
    auto chip  = MvtxDefs::getChipId(hitset->getHitSetKey());
    if (Verbosity() > VERBOSITY_A_LOT)
    {
      std::cout << "layer: " << layer << " stave: " \
      << stave << " chip: " << chip << std::endl;
    }

    //------
    //-- Loop over hits
    //------
    auto hitrangei = hitset->getHits();
    for (auto hitr = hitrangei.first;
        hitr != hitrangei.second;
        ++hitr)
    {
      ++n_hits;
      auto col = MvtxDefs::getCol( hitr->first);
      auto row = MvtxDefs::getRow( hitr->first);
      auto stave_col = chip * SegmentationAlpide::NCols + col;

      h1d_hit_layer->Fill(layer);
      h2f_hit_map[layer]->Fill(stave_col, row);
    }
  }

  if (m_clusters->size() > n_hits)
    std::cout << __PRETTY_FUNCTION__ \
              << " : WARNING!! Nclus " << m_clusters->size() \
              << " > Nhits " << n_hits << std::endl;

  //------
  //-- Loop over clusters
  //------
  if (Verbosity() > VERBOSITY_MORE)
  {
    std::cout << "-- Looping over clusters." << std::endl;
  }

  LyrClusMap m_LyrClusMap;

  int   n_clu_per_layer[NLAYER] = {0};
  float mean_clu_x[NLAYER] = {0.};
  float mean_clu_z[NLAYER] = {0.};

  int n_clus = 0;
  auto clus_range = m_clusters->getClusters();
  for (auto iter_clus = clus_range.first;
       iter_clus != clus_range.second;
       ++iter_clus)
  {
    auto clus = iter_clus->second;
    auto ckey = clus->getClusKey();
    auto lyr  = TrkrDefs::getLayer(ckey);

    h1d_clu_layer->Fill(lyr);

    if ((lyr < 0) || (lyr > 3)) continue;

    h1d_clu_size[lyr]->Fill(clus->getAdc());
    h1d_clu_size_phi[lyr]->Fill(clus->getPhiSize()/SegmentationAlpide::PitchRow);
    h1d_clu_size_z[lyr]->Fill(clus->getZSize()/SegmentationAlpide::PitchCol);

    h2f_clu_xz[lyr]->Fill(clus->getZ(), clus->getX());

    n_clu_per_layer[lyr]++;
    mean_clu_z[lyr] += clus->getZ();
    mean_clu_x[lyr] += clus->getX();

    // insert into map
    m_LyrClusMap.insert(std::make_pair(lyr, clus));
    n_clus++;
  }

  char clu_map = 0;
  for (int ilyr = 0; ilyr < NLAYER; ++ilyr)
  {
    if (n_clu_per_layer[ilyr] > 0)
    {
      mean_clu_z[ilyr] /= (float)n_clu_per_layer[ilyr];
      mean_clu_x[ilyr] /= (float)n_clu_per_layer[ilyr];

      h1d_clu_mean_dz[ilyr]->Fill(mean_clu_z[ilyr] - mean_clu_z[m_ref_align_stave]);
      h1d_clu_mean_dx[ilyr]->Fill(mean_clu_x[ilyr] - mean_clu_x[m_ref_align_stave]);
      clu_map |= ( 1 << ilyr );
    }
  }
  h1d_clu_map->Fill(clu_map);

  if (Verbosity() > VERBOSITY_MORE)
  {
    std::cout << "-- evnt:  " << m_ievent << std::endl;
    std::cout << "-- Nhits: " << n_hits << std::endl;
    std::cout << "-- Nclus: " << n_clus << std::endl;
    std::cout << "-- m_LyrClusMap.size(): " << m_LyrClusMap.size() << std::endl;
  }

  //------
  //-- Simple tracking
  //------
  if (do_tracking)
  {
    //------
    // Try full tracking
    //------
    MvtxStandaloneTracking::MvtxTrackList tracklist;

    std::vector<unsigned int> use_layers;
    use_layers.push_back(3);
    use_layers.push_back(2);
    use_layers.push_back(0);
    use_layers.push_back(1);

    m_mvtxtracking->RunTracking(m_LyrClusMap, tracklist, use_layers);

    htrk->Fill(tracklist.size());

    int ncut = 0;
    for (unsigned int i = 0; i < tracklist.size(); i++)
    {
      htrk_clus->Fill(tracklist.at(i).ClusterList.size());
      htrk_chi2xy->Fill(tracklist.at(i).chi2_xy);
      htrk_chi2zy->Fill(tracklist.at(i).chi2_zy);

      if (tracklist.at(i).chi2_xy < 5 && tracklist.at(i).chi2_zy < 5)
      {
        ++ncut;
        htrk_cut_chi2xy->Fill(tracklist.at(i).chi2_xy);
        htrk_cut_chi2zy->Fill(tracklist.at(i).chi2_zy);
        htrk_cut_clus->Fill(tracklist.at(i).ClusterList.size());
      }

      for (unsigned int j = 0; j < tracklist.at(i).ClusterList.size(); j++)
      {
        auto ckey = tracklist.at(i).ClusterList.at(j)->getClusKey();
        auto lyr  = TrkrDefs::getLayer(ckey);

        if ((lyr < 0) || (lyr >= 4))
        {
          std::cout << PHWHERE << " WARNING: bad layer from track cluster. lyr:" << lyr << std::endl;
          continue;
        }

        htrk_dx[lyr]->Fill(tracklist.at(i).dx.at(j));
        htrk_dz[lyr]->Fill(tracklist.at(i).dz.at(j));

        if (tracklist.at(i).chi2_xy < 5 && tracklist.at(i).chi2_zy < 5)
        {
          htrk_cut_dx[lyr]->Fill(tracklist.at(i).dx.at(j));
          htrk_cut_dz[lyr]->Fill(tracklist.at(i).dz.at(j));
        }
      } // j
    } // i
    htrk_cut->Fill(ncut);
  }
  //-- End
  return Fun4AllReturnCodes::EVENT_OK;
 }


//______________________________________________________________________________
int
AnaMvtxTestBeam2019::End(PHCompositeNode *topNode)
{
  //-- Open file
  PHTFileServer::get().open(m_foutname, "RECREATE");

  PHTFileServer::get().cd(m_foutname);
  gDirectory->mkdir(m_lout_clusterQA->GetName())->cd();
  m_lout_clusterQA->Write();

  if (m_lout_tracking)
  {
    PHTFileServer::get().cd(m_foutname);
    gDirectory->mkdir(m_lout_tracking->GetName())->cd();
    m_lout_tracking->Write();
  }
  PHTFileServer::get().write(m_foutname);
  PHTFileServer::get().close();

  return Fun4AllReturnCodes::EVENT_OK;
}


/*
 * get all the necessary nodes from the node tree
 */
//______________________________________________________________________________
int
AnaMvtxTestBeam2019::GetNodes(PHCompositeNode *topNode)
{
  // Input Hits
  m_hits = findNode::getClass<TrkrHitSetContainerv1>(topNode, "TRKR_HITSET");
  if (! m_hits)
  {
    std::cout << PHWHERE << " HITSETS node was not found on node tree" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  // Input Svtx Clusters
  m_clusters = findNode::getClass<TrkrClusterContainerv1>(topNode, "TRKR_CLUSTER");
  if (! m_clusters)
  {
    std::cout << PHWHERE << " TRKR_CLUSTER node not found on node tree" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}


/*
 * Simple helper function for calculating the slope between two points
 */
//______________________________________________________________________________
double
AnaMvtxTestBeam2019::CalcSlope(double x0, double y0, double x1, double y1)
{
  return (y1 - y0) / (x1 - x0);
}


/*
 * Simple helper function for calculating intercept
 */
//______________________________________________________________________________
double
AnaMvtxTestBeam2019::CalcIntecept(double x0, double y0, double m)
{
  return y0 - x0 * m;
}


/*
 * Simple helper function for calculating projection
 */
//______________________________________________________________________________
double
AnaMvtxTestBeam2019::CalcProjection(double x, double m, double b)
{
  return m * x + b;
}
