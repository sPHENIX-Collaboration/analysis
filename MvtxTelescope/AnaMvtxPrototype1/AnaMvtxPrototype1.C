/*!
 *  \file     AnaMvtxPrototype1.C
 *  \brief    Analyze Mvtx 4 chip telescope from 2018 fermilab beam test
 *  \author   Darren McGlinchey
 */

#include "AnaMvtxPrototype1.h"

#include <trackbase/TrkrDefUtil.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>

#include <mvtxprototype1/MvtxStandaloneTracking.h>

#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <TTree.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TH1.h>
#include <TH2.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TDecompSVD.h>

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <memory>



#define LogDebug(exp)   std::cout<<"DEBUG: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)   std::cout<<"ERROR: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp) std::cout<<"WARNING: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"


AnaMvtxPrototype1::AnaMvtxPrototype1(const std::string &name,
                                     const std::string &ofName) :
  SubsysReco(name),
  clusters_(NULL),
  _foutname(ofName),
  _f(NULL),
  _ievent(0)
{

  hlayer = NULL;
  for (int il = 0; il < 4; il++)
  {
    hsize[il] = NULL;
    hsize_phi[il] = NULL;
    hsize_z[il] = NULL;
    hxz[il] = NULL;
    hdx[il] = NULL;
    hdz[il] = NULL;
  }

  mvtxtracking_ = new MvtxStandaloneTracking();
  mvtxtracking_->SetWindowX(10);
  mvtxtracking_->SetWindowZ(10);
  mvtxtracking_->Verbosity(0);
}

int AnaMvtxPrototype1::Init(PHCompositeNode *topNode)
{

  //-- Open file
  _f = new TFile(_foutname.c_str(), "RECREATE");

  //-- Initialize histograms
  hlayer = new TH1D("hlayer", ";layer", 6, -0.5, 5.5);

  for (int il = 0; il < 4; il++)
  {
    hsize[il] = new TH1D(Form("hsize_l%i", il),
                         ";cluster size total [pixels]",
                         100, -0.5, 99.5);

    hsize_phi[il] = new TH1D(Form("hsize_phi_l%i", il),
                             ";cluster size #phi [pixels]",
                             100, -0.5, 99.5);

    hsize_z[il] = new TH1D(Form("hsize_z_l%i", il),
                           ";cluster size z [pixels]",
                           100, -0.5, 100.5);

    hxz[il] = new TH2D(Form("hxz_l%i", il),
                       ";z;x",
                       1024, -0.5, 1023.5,
                       512, -0.5, 511.5);

    hdx[il] = new TH1D(Form("hdx_l%i", il),
                       "; dx [pixels]",
                       2000, -500, 500);

    hdz[il] = new TH1D(Form("hdz_l%i", il),
                       "; dz [pixels]",
                       2000, -500, 500);
  } // il

  //-- results from tracking
  htrk = new TH1D("htrk", ";trks / event", 16, -0.5, 15.5);
  htrk_cut = new TH1D("htrk_cut", ";trks / event", 16, -0.5, 15.5);

  for (int il = 0; il < 4; il++)
  {
    htrk_dx[il] = new TH1D(Form("htrk_dx_l%i", il),
                           ";track dx [pixels]",
                           500, -25, 25);

    htrk_dz[il] = new TH1D(Form("htrk_dz_l%i", il),
                           ";track dz [pixels]",
                           500, -25, 25);

    htrk_cut_dx[il] = new TH1D(Form("htrk_cut_dx_l%i", il),
                               ";track dx [pixels]",
                               500, -25, 25);

    htrk_cut_dz[il] = new TH1D(Form("htrk_cut_dz_l%i", il),
                               ";track dz [pixels]",
                               500, -25, 25);

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

  return 0;

}

int AnaMvtxPrototype1::InitRun(PHCompositeNode *topNode)
{

  //-- set counters
  _ievent = -1; // since we count at the beginning of the event, start at -1


  return 0;

}

int AnaMvtxPrototype1::process_event(PHCompositeNode *topNode)
{

  if ( verbosity > VERBOSITY_MORE )
    std::cout << "AnaMvtxPrototype1::process_event()" << std::endl;

  // count event
  _ievent++;

  //------
  //-- Get the nodes
  //------
  int retval = GetNodes(topNode);
  if ( retval != Fun4AllReturnCodes::EVENT_OK )
    return retval;

  if ( verbosity > VERBOSITY_MORE )
  {
    std::cout << "-- evnt:" << _ievent << std::endl;
    std::cout << "-- Nclus:" << clusters_->size() << std::endl;
  }

  //------
  //-- Misalign clusters
  //------
  // Misalign();

  //------
  //-- Loop over clusters
  //------

  if ( verbosity > VERBOSITY_MORE )
    std::cout << "-- Looping over clusters" << std::endl;

  LyrClusMap _mmap_lyr_clus;

  TrkrDefUtil trkrutil;

  TrkrClusterContainer::ConstRange clusrange = clusters_->GetClusters();
  for (TrkrClusterContainer::ConstIterator iter = clusrange.first;
       iter != clusrange.second;
       ++iter)
  {
    TrkrCluster *clus = iter->second;
    // clus->identify();

    TrkrDefs::cluskey ckey = clus->GetClusKey();

    int lyr = trkrutil.GetLayer(ckey);

    hlayer->Fill(lyr);

    if (lyr < 0 || lyr > 3)
      continue;
    hsize[lyr]->Fill(clus->GetAdc());
    hsize_phi[lyr]->Fill(clus->GetPhiSize());
    hsize_z[lyr]->Fill(clus->GetZSize());
    hxz[lyr]->Fill(clus->GetZ(), clus->GetX());

    // insert into map
    _mmap_lyr_clus.insert(std::make_pair(lyr, clus));
  }

  if ( verbosity > VERBOSITY_MORE )
    std::cout << "-- _mmap_lyr_clus.size():" << _mmap_lyr_clus.size() << std::endl;

  //------
  //-- Simple tracking
  //------

  if ( verbosity > VERBOSITY_MORE )
    std::cout << "-- Performing simple tracking" << std::endl;

  static bool check_range = true;
  if ( check_range )
  {
    TrkrDefs::cluskey mvtxl = trkrutil.GetClusKeyLo(TrkrDefs::TRKRID::mvtx_id);
    TrkrDefs::cluskey mvtxh = trkrutil.GetClusKeyHi(TrkrDefs::TRKRID::mvtx_id);

    std::cout << PHWHERE << " Mvtx key range: 0x" << std::hex << mvtxl << " - 0x" << mvtxh << std::dec << std::endl;

    for (int i = 0; i < 4; i++)
    {
      TrkrDefs::cluskey lyrl = trkrutil.GetClusKeyLo(TrkrDefs::TRKRID::mvtx_id, i);
      TrkrDefs::cluskey lyrh = trkrutil.GetClusKeyHi(TrkrDefs::TRKRID::mvtx_id, i);

      std::cout << PHWHERE << " Mvtx key range lyr " << i << ": 0x" << std::hex << lyrl << " - 0x" << lyrh << std::dec << std::endl;
    }

    check_range = false;
  }


  // loop over clusters in layer 0
  TrkrClusterContainer::ConstRange lyr0range = clusters_->GetClusters(TrkrDefs::TRKRID::mvtx_id, 0);
  for ( TrkrClusterContainer::ConstIterator lyr0_itr = lyr0range.first;
        lyr0_itr != lyr0range.second;
        ++lyr0_itr)
  {
    TrkrCluster* clus0 = lyr0_itr->second;

    if ( !clus0 )
    {
      std::cout << "WARNING: clus0 not found" << std::endl;
      continue;
    }

    // check layer
    int lyr = trkrutil.GetLayer(clus0->GetClusKey());
    if ( lyr != 0 )
    {
      std::cout << PHWHERE << "ERRORL: Expected clusters from layer 0 only. Found lyr=" << lyr << std::endl;
      continue;
    }



    auto misiter = _misalign.find(0);
    // get (x, y, z)
    double x0 = misiter != _misalign.end() ? clus0->GetX() + (misiter->second).dx : clus0->GetX();
    double y0 = misiter != _misalign.end() ? clus0->GetY() + (misiter->second).dy : clus0->GetY();
    double z0 = misiter != _misalign.end() ? clus0->GetZ() + (misiter->second).dz : clus0->GetZ();

    if ( verbosity > VERBOSITY_MORE )
    {
      std::cout << "-- clus0:(" << x0 << ", " << y0 << ", " << z0 << ")" << std::endl;
    }

    // loop over all clusters in the outer layer
    TrkrClusterContainer::ConstRange lyr3range = clusters_->GetClusters(TrkrDefs::TRKRID::mvtx_id, 3);
    for ( TrkrClusterContainer::ConstIterator lyr3_itr = lyr3range.first;
          lyr3_itr != lyr3range.second;
          ++lyr3_itr)
    {
      TrkrCluster* clus3 = lyr3_itr->second;

      if ( !clus3 )
      {
        std::cout << "WARNING: clus3 not found" << std::endl;
        continue;
      }

      auto misiter = _misalign.find(3);
      // get (x, y, z)
      double x3 = misiter != _misalign.end() ? clus3->GetX() + (misiter->second).dx : clus3->GetX();
      double y3 = misiter != _misalign.end() ? clus3->GetY() + (misiter->second).dy : clus3->GetY();
      double z3 = misiter != _misalign.end() ? clus3->GetZ() + (misiter->second).dz : clus3->GetZ();


      // calc m, b in (phi, r) space
      double mpr = CalcSlope(y0, x0, y3, x3);
      double bpr = CalcIntecept(y0, x0, mpr);

      // calc m, b in (z, r) space
      double mzr = CalcSlope(y0, z0, y3, z3);
      double bzr = CalcIntecept(y0, z0, mzr);


      if ( verbosity > VERBOSITY_MORE )
      {
        std::cout << "-- clus3:(" << x3 << ", " << y3 << ", " << z3 << ")" << std::endl;
        std::cout << "-- mpr:" << mpr << " bpr:" << bpr << std::endl;
        std::cout << "-- mzr:" << mzr << " bzr:" << bzr << std::endl;
      }



      // loop over all clusters in layer 1 & 2 and calc displacement
      for (int ilyr = 1; ilyr <= 2; ilyr++)
      {
        TrkrClusterContainer::ConstRange lyrrange = clusters_->GetClusters(TrkrDefs::TRKRID::mvtx_id, ilyr);
        for ( TrkrClusterContainer::ConstIterator lyr_itr = lyrrange.first;
              lyr_itr != lyrrange.second;
              ++lyr_itr)
        {
          TrkrCluster* clus = lyr_itr->second;

          if ( !clus )
            continue;

          auto misiter = _misalign.find(ilyr);
          // get (x, y, z)
          double x = misiter != _misalign.end() ? clus->GetX() + (misiter->second).dx : clus->GetX();
          double y = misiter != _misalign.end() ? clus->GetY() + (misiter->second).dy : clus->GetY();
          double z = misiter != _misalign.end() ? clus->GetZ() + (misiter->second).dz : clus->GetZ();


          // calc track projection in phi, z
          double x_proj = CalcProjection(y, mpr, bpr);
          double z_proj = CalcProjection(y, mzr, bzr);

          // calc proj - true
          double dx = (x_proj - x);
          double dz = (z_proj - z);

          if ( !hdx[ilyr] || !hdz[ilyr] )
          {
            std::cout << "WARNING!! can't find hdphi or hdz for lyr:" << ilyr << std::endl;
          }
          else
          {
            hdx[ilyr]->Fill(dx);
            hdz[ilyr]->Fill(dz);
          }

          if ( verbosity > VERBOSITY_MORE )
          {
            std::cout << "------------------" << std::endl;
            std::cout << "-- clus" << trkrutil.GetLayer(clus->GetClusKey()) << ":(" << x3 << ", " << y3 << ", " << z3 << ")" << std::endl;
            std::cout << "-- proj:(" << x_proj << ", " << z_proj << ")" << std::endl;
            std::cout << "-- dx [pixels]: " << dx << std::endl;
            std::cout << "-- dz [pixels]: " << dz << std::endl;
            std::cout << "------------------" << std::endl;
          }

        } // lyr1_itr
      } // ilyr;

    } // lyr3_itr

  } //lyr0_itr

  // // loop over clusters in layer 0
  // LyrClusMap::iterator lyr0_itr;
  // LyrClusMap::iterator lyr_itr;
  // LyrClusMap::iterator lyr3_itr;
  // for ( lyr0_itr = _mmap_lyr_clus.lower_bound(0);
  //       lyr0_itr != _mmap_lyr_clus.upper_bound(0);
  //       ++lyr0_itr)
  // {
  //   TrkrCluster* clus0 = lyr0_itr->second;

  //   if ( !clus0 )
  //   {
  //     std::cout << "WARNING: clus0 not found" << std::endl;
  //     continue;
  //   }
  //   auto misiter = _misalign.find(0);
  //   // get (x, y, z)
  //   double x0 = misiter != _misalign.end() ? clus0->GetX() + (misiter->second).dx : clus0->GetX();
  //   double y0 = misiter != _misalign.end() ? clus0->GetY() + (misiter->second).dy : clus0->GetY();
  //   double z0 = misiter != _misalign.end() ? clus0->GetZ() + (misiter->second).dz : clus0->GetZ();

  //   if ( verbosity > VERBOSITY_MORE )
  //   {
  //     std::cout << "-- clus0:(" << x0 << ", " << y0 << ", " << z0 << ")" << std::endl;
  //   }

  //   // loop over all clusters in the outer layer
  //   for ( lyr3_itr = _mmap_lyr_clus.lower_bound(3);
  //         lyr3_itr != _mmap_lyr_clus.upper_bound(3);
  //         ++lyr3_itr)
  //   {
  //     TrkrCluster* clus3 = lyr3_itr->second;

  //     if ( !clus3 )
  //     {
  //       std::cout << "WARNING: clus3 not found" << std::endl;
  //       continue;
  //     }

  //     auto misiter = _misalign.find(3);
  //     // get (x, y, z)
  //     double x3 = misiter != _misalign.end() ? clus3->GetX() + (misiter->second).dx : clus3->GetX();
  //     double y3 = misiter != _misalign.end() ? clus3->GetY() + (misiter->second).dy : clus3->GetY();
  //     double z3 = misiter != _misalign.end() ? clus3->GetZ() + (misiter->second).dz : clus3->GetZ();


  //     // calc m, b in (phi, r) space
  //     double mpr = CalcSlope(y0, x0, y3, x3);
  //     double bpr = CalcIntecept(y0, x0, mpr);

  //     // calc m, b in (z, r) space
  //     double mzr = CalcSlope(y0, z0, y3, z3);
  //     double bzr = CalcIntecept(y0, z0, mzr);


  //     if ( verbosity > VERBOSITY_MORE )
  //     {
  //       std::cout << "-- clus3:(" << x3 << ", " << y3 << ", " << z3 << ")" << std::endl;
  //       std::cout << "-- mpr:" << mpr << " bpr:" << bpr << std::endl;
  //       std::cout << "-- mzr:" << mzr << " bzr:" << bzr << std::endl;
  //     }



  //     // loop over all clusters in layer 1 & 2 and calc displacement
  //     for (int ilyr = 1; ilyr <= 2; ilyr++)
  //     {
  //       for ( lyr_itr = _mmap_lyr_clus.lower_bound(ilyr);
  //             lyr_itr != _mmap_lyr_clus.upper_bound(ilyr);
  //             ++lyr_itr)
  //       {
  //         TrkrCluster* clus = lyr_itr->second;

  //         if ( !clus )
  //           continue;

  //         auto misiter = _misalign.find(ilyr);
  //         // get (x, y, z)
  //         double x = misiter != _misalign.end() ? clus->GetX() + (misiter->second).dx : clus->GetX();
  //         double y = misiter != _misalign.end() ? clus->GetY() + (misiter->second).dy : clus->GetY();
  //         double z = misiter != _misalign.end() ? clus->GetZ() + (misiter->second).dz : clus->GetZ();


  //         // calc track projection in phi, z
  //         double x_proj = CalcProjection(y, mpr, bpr);
  //         double z_proj = CalcProjection(y, mzr, bzr);

  //         // calc proj - true
  //         double dx = (x_proj - x);
  //         double dz = (z_proj - z);

  //         if ( !hdx[ilyr] || !hdz[ilyr] )
  //         {
  //           std::cout << "WARNING!! can't find hdphi or hdz for lyr:" << ilyr << std::endl;
  //         }
  //         else
  //         {
  //           hdx[ilyr]->Fill(dx);
  //           hdz[ilyr]->Fill(dz);
  //         }

  //         if ( verbosity > VERBOSITY_MORE )
  //         {
  //           std::cout << "------------------" << std::endl;
  //           std::cout << "-- clus" << trkrutil.GetLayer(clus->GetClusKey()) << ":(" << x3 << ", " << y3 << ", " << z3 << ")" << std::endl;
  //           std::cout << "-- proj:(" << x_proj << ", " << z_proj << ")" << std::endl;
  //           std::cout << "-- dx [pixels]: " << dx << std::endl;
  //           std::cout << "-- dz [pixels]: " << dz << std::endl;
  //           std::cout << "------------------" << std::endl;
  //         }

  //       } // lyr1_itr
  //     } // ilyr;

  //   } // lyr3_itr

  // } //lyr0_itr


  if ( verbosity > VERBOSITY_MORE )
    std::cout << "-- Done simple tracking" << std::endl;



  // //------
  // //-- Track candidates using all layers
  // //------

  // LyrClusMap::iterator lyr_itr;
  // TrkVec _vec_trk;
  // ClusVec _trk_candidate;

  // for ( lyr_itr = _mmap_lyr_clus.lower_bound(0);
  //       lyr_itr != _mmap_lyr_clus.upper_bound(0);
  //       ++lyr_itr)
  // {
  //   TrkrCluster* clus0 = lyr_itr->second;

  // }


  //------
  // Try full tracking
  //------
  MvtxStandaloneTracking::MvtxTrackList tracklist;

  std::vector<int> uselayers;
  uselayers.push_back(0);
  uselayers.push_back(1);
  uselayers.push_back(2);
  uselayers.push_back(3);

  mvtxtracking_->RunTracking(topNode, tracklist, uselayers);

  htrk->Fill(tracklist.size());

  int ncut = 0;
  for ( unsigned int i = 0; i < tracklist.size(); i++)
  {

    htrk_chi2xy->Fill(tracklist.at(i).chi2_xy);
    htrk_chi2zy->Fill(tracklist.at(i).chi2_zy);

    if ( tracklist.at(i).chi2_xy < 5 && tracklist.at(i).chi2_zy < 5)
    {
      ncut++;
      htrk_cut_chi2xy->Fill(tracklist.at(i).chi2_xy);
      htrk_cut_chi2zy->Fill(tracklist.at(i).chi2_zy);
    }

    for ( unsigned int j = 0; j < tracklist.at(i).ClusterList.size(); j++)
    {
      TrkrDefs::cluskey ckey = tracklist.at(i).ClusterList.at(j)->GetClusKey();

      int lyr = trkrutil.GetLayer(ckey);

      if ( lyr < 0 || lyr >= 4 )
      {
        std::cout << PHWHERE << " WARNING: bad layer from track cluster. lyr:" << lyr << std::endl;
        continue;
      }

      htrk_dx[lyr]->Fill(tracklist.at(i).dx.at(j));
      htrk_dz[lyr]->Fill(tracklist.at(i).dz.at(j));

      if ( tracklist.at(i).chi2_xy < 5 && tracklist.at(i).chi2_zy < 5)
      {
        htrk_cut_dx[lyr]->Fill(tracklist.at(i).dx.at(j));
        htrk_cut_dz[lyr]->Fill(tracklist.at(i).dz.at(j));
      }
    } // j


  } // i

  htrk_cut->Fill(ncut);
  
  //-- Cleanup


  //-- End
  return 0;

}

int AnaMvtxPrototype1::End(PHCompositeNode * topNode)
{

  PrintMisalign();

  _f->Write();
  _f->Close();

  return 0;
}

/*
 * Get all the necessary nodes from the node tree
 */
int AnaMvtxPrototype1::GetNodes(PHCompositeNode * topNode)
{

  // Input Svtx Clusters
  clusters_ = findNode::getClass<TrkrClusterContainer>(topNode, "TrkrClusterContainer");
  if (!clusters_ && _ievent < 2)
  {
    std::cout << PHWHERE << " TrkrClusterContainer node not found on node tree"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;

}

/*
 * Simple helper function for calculating the slope between two points
 */
double
AnaMvtxPrototype1::CalcSlope(double x0, double y0, double x1, double y1)
{
  return (y1 - y0) / (x1 - x0);
}

/*
 * Simple helper function for calculating intercept
 */
double
AnaMvtxPrototype1::CalcIntecept(double x0, double y0, double m)
{
  return y0 - x0 * m;
}

/*
 * Simple helper function for calculating projection
 */
double
AnaMvtxPrototype1::CalcProjection(double x, double m, double b)
{
  return m * x + b;
}

/*
 *
 */
void
AnaMvtxPrototype1::MisalignLayer(int lyr, double dx, double dy, double dz)
{
  mis align;
  align.dx = dx;
  align.dy = dy;
  align.dz = dz;

  _misalign.insert( std::make_pair(lyr, align) );
}

/*
 * Misalign clusters for each layer
 */
void
AnaMvtxPrototype1::Misalign()
{
  // only bother if we've set some misalignments to non-zero values
  if ( _misalign.size() < 1 )
    return;

  TrkrClusterContainer::ConstRange clusrange = clusters_->GetClusters();
  for (TrkrClusterContainer::ConstIterator iter = clusrange.first;
       iter != clusrange.second;
       ++iter)
  {
    // TrkrCluster *clus = iter->second;
    /*
    // get misalignments
    auto misitr = _misalign.find(clus->get_layer());
    if ( misitr != _misalign.end() )
    {
      mis align = misitr->second;

      float x = clus->GetX();
      float y = clus->GetY();
      float z = clus->GetZ();
      float r = TMath::Sqrt(x * x + y * y);
      float phi = TMath::ATan2(y, x);
      float s = r * phi;

      double zp = z;
      double rp = r;
      double sp = s;

      // --- pitch, yaw, roll
      // pitch is dr(z)
      rp += z * TMath::Sin(align.dpitch);

      // yaw is ds(z)
      sp += z * TMath::Sin(align.dyaw);

      // roll is dr(s)
      rp += s * TMath::Sin(align.droll);

      // --- translate (s, z, r)
      rp += align.dr;
      sp += align.ds;
      zp += align.dz;

      // --- recalculat x', y', z'
      float phip = sp / rp;
      float xp = rp * TMath::Cos(phip);
      float yp = rp * TMath::Sin(phip);

      // --- set new coordinates
      clus->set_x(xp);
      clus->set_y(yp);
      clus->set_z(zp);

      if ( false )
      {
        std::cout << " lyr:" << clus->get_layer()
                  << " ds:" << align.ds
                  << " dz:" << align.dz
                  << " dr:" << align.dr
                  << " dpitch:" << align.dpitch
                  << " dyaw:" << align.dyaw
                  << " droll:" << align.droll
                  << " phi:" << phi << " phip:" << phip
                  << " z:" << z << " zp:" << zp
                  << " x:" << x << " xp:" << xp
                  << " y:" << y << " yp:" << yp
                  << std::endl;
      }

    }
    */
  } // iter
}

/*
 *
 */
void
AnaMvtxPrototype1::PrintMisalign()
{
  std::cout << "================================" << std::endl;
  std::cout << "== " << PHWHERE << std::endl;

  for (auto iter = _misalign.begin(); iter != _misalign.end(); ++iter)
  {
    std::cout << "== Lyr: " << iter->first
              << " dx[cm]:" << (iter->second).dx
              << " dy[cm]:" << (iter->second).dy
              << " dz[cm]:" << (iter->second).dz
              << std::endl;
  } // iter

  std::cout << "================================" << std::endl;
}

