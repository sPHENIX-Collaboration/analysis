/*!
 *  \file     AnaMvtxTelescopeHits.C
 *  \brief    Analyze Mvtx 4 ladder cosmic telescope in simulations
 *  \details  Analyze simulations of hits in the 4 ladder
 *            Mvtx cosimic ray telescope
 *  \author   Darren McGlinchey
 */

#include "AnaMvtxTelescopeHits.h"

#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>

#include <phgeom/PHGeomUtility.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>

#include <g4hough/SvtxCluster.h>
#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxHitMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxVertexMap.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CylinderGeom_MAPS.h>
#include <g4detectors/PHG4CylinderGeom_Siladders.h>

#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/Track.h>
#include <phgenfit/SpacepointMeasurement.h>

//GenFit
#include <GenFit/FieldManager.h>
#include <GenFit/GFRaveConverters.h>
#include <GenFit/GFRaveVertex.h>
#include <GenFit/GFRaveVertexFactory.h>
#include <GenFit/MeasuredStateOnPlane.h>
#include <GenFit/RKTrackRep.h>
#include <GenFit/StateOnPlane.h>
#include <GenFit/Track.h>

//Rave
#include <rave/Version.h>
#include <rave/Track.h>
#include <rave/VertexFactory.h>
#include <rave/ConstantMagneticField.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

#include <HFJetTruthGeneration/HFJetDefs.h>

#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TH1.h"
#include "TH2.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include "TDecompSVD.h"


#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <memory>



#define LogDebug(exp)   std::cout<<"DEBUG: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)   std::cout<<"ERROR: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp) std::cout<<"WARNING: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"


AnaMvtxTelescopeHits::AnaMvtxTelescopeHits(const std::string &name,
    const std::string &ofName) :
  SubsysReco(name),
  _clustermap(NULL),
  _foutname(ofName),
  _f(NULL)
{

  hlayer = NULL;
  for (int il = 0; il < 4; il++)
  {
    hsize_phi[il] = NULL;
    hsize_z[il] = NULL;
    hphiz[il] = NULL;
    hdphi[il] = NULL;
    hdz[il] = NULL;
  }
}

int AnaMvtxTelescopeHits::Init(PHCompositeNode *topNode)
{

  //-- Open file
  _f = new TFile(_foutname.c_str(), "RECREATE");

  //-- Initialize histograms
  hlayer = new TH1D("hlayer", ";layer", 6, -0.5, 5.5);

  for (int il = 0; il < 4; il++)
  {
    hsize_phi[il] = new TH1D(Form("hsize_phi_l%i", il),
                             ";cluster size #phi [cm]",
                             100, 0, 0.1);

    hsize_z[il] = new TH1D(Form("hsize_z_l%i", il),
                           ";cluster size z [cm]",
                           100, 0, 0.1);

    hphiz[il] = new TH2D(Form("hphiz_l%i", il),
                         ";z [cm]; #phi",
                         1000, -10, 10,
                         100, -0.1, 0.1);

    hdphi[il] = new TH1D(Form("hdphi_l%i", il),
                         "; d#phi [#mum]",
                         2000, -100, 100);

    hdz[il] = new TH1D(Form("hdz_l%i", il),
                         "; dz [#mum]",
                         2000, -100, 100);
  } // il



  return 0;

}

int AnaMvtxTelescopeHits::InitRun(PHCompositeNode *topNode)
{

  // TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);

  // _fitter = PHGenFit::Fitter::getInstance(tgeo_manager, _mag_field_file_name.data(),
  //                                         (_reverse_mag_field) ? -1. * _mag_field_re_scaling_factor : _mag_field_re_scaling_factor,
  //                                         _track_fitting_alg_name, "RKTrackRep", _do_evt_display);

  // if (!_fitter) {
  //   std::cerr << PHWHERE << std::endl;
  //   return Fun4AllReturnCodes::ABORTRUN;
  // }

  //-- set counters
  _ievent = -1; // since we count at the beginning of the event, start at -1


  return 0;

}

int AnaMvtxTelescopeHits::process_event(PHCompositeNode *topNode)
{

  if ( verbosity > VERBOSITY_MORE )
    std::cout << "AnaMvtxTelescopeHits::process_event()" << std::endl;

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
    std::cout << "-- Nclus:" << _clustermap->size() << std::endl;
  }


  //------
  //-- Loop over clusters
  //------

  if ( verbosity > VERBOSITY_MORE )
    std::cout << "-- Looping over clusters" << std::endl;

  LyrClusMap _mmap_lyr_clus;

  for (SvtxClusterMap::Iter iter = _clustermap->begin();
       iter != _clustermap->end();
       ++iter)
  {
    SvtxCluster *clus = iter->second;

    hlayer->Fill(clus->get_layer());

    unsigned int lyr = clus->get_layer();
    if (lyr < 0 || lyr > 3)
      continue;
    hsize_phi[lyr]->Fill(clus->get_phi_size());
    hsize_z[lyr]->Fill(clus->get_z_size());

    double phi = TMath::ATan2(clus->get_y(), clus->get_x());
    hphiz[lyr]->Fill(clus->get_z(), phi);

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

  // loop over clusters in layer 0
  LyrClusMap::iterator lyr0_itr;
  LyrClusMap::iterator lyr_itr;
  LyrClusMap::iterator lyr3_itr;
  for ( lyr0_itr = _mmap_lyr_clus.lower_bound(0);
        lyr0_itr != _mmap_lyr_clus.upper_bound(0);
        ++lyr0_itr)
  {
    SvtxCluster* clus0 = lyr0_itr->second;

    if ( !clus0 )
    {
      std::cout << "WARNING: clus0 not found" << std::endl;
      continue;
    }

    // get (r, phi, z)
    double r0 = TMath::Sqrt(TMath::Power(clus0->get_x(), 2) + TMath::Power(clus0->get_y(), 2));
    double p0 = TMath::ATan2(clus0->get_y(), clus0->get_x());
    double z0 = clus0->get_z();

    if ( verbosity > VERBOSITY_MORE )
    {
      std::cout << "-- clus0:(" << r0 << ", " << p0 << ", " << z0 << ")" << std::endl;
    }

    // loop over all clusters in the outer layer
    for ( lyr3_itr = _mmap_lyr_clus.lower_bound(3);
          lyr3_itr != _mmap_lyr_clus.upper_bound(3);
          ++lyr3_itr)
    {
      SvtxCluster* clus3 = lyr3_itr->second;

      if ( !clus3 )
      {
        std::cout << "WARNING: clus3 not found" << std::endl;
        continue;
      }

      // get (r, phi, z)
      double r3 = TMath::Sqrt(TMath::Power(clus3->get_x(), 2) + TMath::Power(clus3->get_y(), 2));
      double p3 = TMath::ATan2(clus3->get_y(), clus3->get_x());
      double z3 = clus3->get_z();

      // calc m, b in (phi, r) space
      double mpr = CalcSlope(r0, p0, r3, p3);
      double bpr = CalcIntecept(r0, p0, mpr);

      // calc m, b in (z, r) space
      double mzr = CalcSlope(r0, z0, r3, z3);
      double bzr = CalcIntecept(r0, z0, mzr);


      if ( verbosity > VERBOSITY_MORE )
      {
        std::cout << "-- clus3:(" << r3 << ", " << p3 << ", " << z3 << ")" << std::endl;
        std::cout << "-- mpr:" << mpr << " bpr:" << bpr << std::endl;
        std::cout << "-- mzr:" << mzr << " bzr:" << bzr << std::endl;
      }



      // loop over all clusters in layer 1 & 2 and calc displacement
      for (int ilyr = 1; ilyr <= 2; ilyr++)
      {
        for ( lyr_itr = _mmap_lyr_clus.lower_bound(ilyr);
              lyr_itr != _mmap_lyr_clus.upper_bound(ilyr);
              ++lyr_itr)
        {
          SvtxCluster* clus = lyr_itr->second;

          if ( !clus )
            continue;

          // get (r, phi, z)
          double r = TMath::Sqrt(TMath::Power(clus->get_x(), 2) + TMath::Power(clus->get_y(), 2));
          double phi = TMath::ATan2(clus->get_y(), clus->get_x());
          double z = clus->get_z();

          // calc track projection in phi, z
          double phi_proj = CalcProjection(r, mpr, bpr);
          double z_proj = CalcProjection(r, mzr, bzr);

          // calc proj - true
          double dphi = (phi_proj - phi) * 1e4;
          double dz = (z_proj - z) * 1e4;

          if ( !hdphi[ilyr] || !hdz[ilyr] )
          {
            std::cout << "WARNING!! can't find hdphi or hdz for lyr:" << ilyr << std::endl;
          }
          else
          {
            hdphi[ilyr]->Fill(dphi);
            hdz[ilyr]->Fill(dz);
          }

          if ( verbosity > VERBOSITY_MORE )
          {
            std::cout << "------------------" << std::endl;
            std::cout << "-- clus" << clus->get_layer() << ":(" << r3 << ", " << p3 << ", " << z3 << ")" << std::endl;
            std::cout << "-- proj:(" << phi_proj << ", " << z_proj << ")" << std::endl;
            std::cout << "-- dphi [micron]: " << dphi << std::endl;
            std::cout << "-- dz [micron]:   " << dz << std::endl;
            std::cout << "------------------" << std::endl;
          }

        } // lyr1_itr
      } // ilyr;

    } // lyr3_itr

  } //lyr0_itr


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
  //   SvtxCluster* clus0 = lyr_itr->second;

  // }


  //-- Cleanup


  //-- End
  return 0;

}

int AnaMvtxTelescopeHits::End(PHCompositeNode * topNode)
{
  _f->Write();
  _f->Close();

  return 0;
}

/*
 * Get all the necessary nodes from the node tree
 */
int AnaMvtxTelescopeHits::GetNodes(PHCompositeNode * topNode)
{

  // Input Svtx Clusters
  _clustermap = findNode::getClass<SvtxClusterMap>(topNode, "SvtxClusterMap");
  if (!_clustermap && _ievent < 2) {
    std::cout << PHWHERE << " SvtxClusterMap node not found on node tree"
              << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;

}

/*
 * Simple helper function for calculating the slope between two points
 */
double
AnaMvtxTelescopeHits::CalcSlope(double x0, double y0, double x1, double y1)
{
  return (y1 - y0) / (x1 - x0);
}

/*
 * Simple helper function for calculating intercept
 */
double
AnaMvtxTelescopeHits::CalcIntecept(double x0, double y0, double m)
{
  return y0 - x0 * m;
}

/*
 * Simple helper function for calculating projection
 */
double
AnaMvtxTelescopeHits::CalcProjection(double x, double m, double b)
{
  return m * x + b;
}


// /*
//  *
//  */
// int AnaMvtxTelescopeHits::GetTrackCandidates(LyrClusMap* clusmap,
//     TrkVec* trkvec)
// {

//   //-- Make a vector to hold all track candidates for a given first cluster
//   TrkVec trkcand;

//   //-- Loop over all clusters in the first layer of the cluster map
//   unsigned int lyr = (clusmap->begin())->first;
//   for (LyrClusMap::iterator itr = clusmap->lower_bound(lyr);
//        itr != clusmap->upper_bound(lyr);
//        ++itr)
//   {
//     trkcand.clear();

//     //-- Find all track candidates for this cluster
//     ClusVec trk;
//     trk.insert(itr->second);
//     LinkClusters(&trk, lyr+1, clusmap, &trkcand);

//     //-- Choose the best track candidate using this cluster
//     ClusVec best_trk = ChooseBestTrk(&trkcand);
//     if ( best_trk.size() > 2 )
//       trkvec->insert(best_trk);
//   }

//   return trkvec->size();
// }

// /*
//  *
//  */
// void AnaMvtxTelescopeHits::LinkClusters(ClusVec* trk,
//                                         unsigned int next_lyr,
//                                         LyrClusMap* clusmap,
//                                         TrkVec* trkvec)
// {

//   //-- Check layer
//   if ( next_lyr > 3 )
//   {
//     // add this trk to the trk vector, we're done with this trk
//     // we only consider tracks with at least 3 clusters
//     if ( trk->size() > 2 )
//       trkvec->push_back(*trk);
//     return;
//   }

//   bool found = false;

//   //-- Search for clusters in the next layer
//   for ( LyrClusMap::iterator itr = LyrClusMap.lower_bound(next_lyr);
//         itr != LyrClusMap.upper_bound(next_lyr);
//         ++itr)
//   {
//     // make a copy of this trk candidate and run the recursion
//     ClusVec new_trk = *trk;
//     new_trk.insert(itr->second);
//     LinkClusters(&new_trk, next_lyr + 1, clusmap, trkvec);
//     found = true;
//   }

//   //-- if no cluster found in next layer, try next layer
//   if ( !found )
//   {
//     LinkClusters(*trk, next_lyr + 2, clusmap, trkvec);
//   }

// }

// /*
//  * Choose the best track candidates which all start with the same cluster
//  */
// ClusVec AnaMvtxTelescopeHits::ChooseBestTrk(TrkVec* trkcnd)
// {

//   int best_idx = -1;
//   double best_chi2 = 100;
//   for (TrkVec::iterator itr = trkcnd->begin();
//        itr != trkcnd->end();
//        ++itr)
//   {
//     double chi2 = FitTrk(&itr->second);
//     if ( chi2 < best_chi2 )
//     {
//       best_chi2 = chi2;
//       best_idx = itr->first;
//     }
//   }

//   if ( best_idx >= 0 && best_chi2 < 5 )
//   {
//     return trkcnd->at(best_idx);
//   }
//   else
//   {
//     return ClusVec;
//   }
// }

// /*
//  *
//  */
// double AnaMvtxTelescopeHits::FitTrk(ClusVec* trk)
// {
//   return 0;
// }

/*
 * Simple generalized linear least-squares fitter.
 * Solve y(X) = beta'*X + error(L) for beta (vector of regression coefs.)
 * L is the inverse covariance matrix for y.
 * Least-squares solution involves solving X' L X beta = X' L y
 *
 */
/*
TVectorD
AnaMvtxTelescopeHits::SolveGLS(TMatrixD &X, TVectorD &y, TMatrixD &L, TMatrixD &cov)
{

  double tol = 1.e-15;
  TMatrixD XT(X); XT.T();
  TMatrixD A = XT * L * X;
  TVectorD b = XT * L * y;

  // Now solve A*beta = b using SVD. Decompose A = U S V'.
  TDecompSVD svd(A);
  TMatrixD UT = svd.GetU(); UT.T();
  TMatrixD V  = svd.GetV();

  // Construct Moore-Penrose pseudoinverse of S
  TVectorD s = svd.GetSig();
  int n = s.GetNrows();
  TMatrixD Sd(n, n);
  for (int i = 0; i < n; i++)
    Sd(i, i) = s(i) > tol ? 1. / s(i) : 0.;

  // Result
  TVectorD beta = V * Sd * UT * b;

  // and covariance matrix
  V *= Sd;
  TMatrixD C(V, TMatrixD::kMultTranspose, V);
  cov.ResizeTo(C);
  cov = C;

  return beta;
}
*/

