#include "SimpleTrackingAnalysis.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
//#include <g4hough/PHG4HoughTransform.h> // CAUSES ERRORS DUE TO VertexFinder.h FAILING TO FIND <Eigen/LU>


#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxVertexEval.h>
#include <g4eval/SvtxTruthEval.h>

// --- common to all calorimeters
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h>
#include <g4eval/CaloRawTowerEval.h>

#include <TH1D.h>
#include <TH2D.h>
#include <TProfile2D.h>

#include <iostream>
//#include <cassert>

using namespace std;



SimpleTrackingAnalysis::SimpleTrackingAnalysis(const string &name) : SubsysReco(name)
{
  cout << "SimpleTrackingAnalysis class constructor called " << endl;
  nevents = 0;
  nerrors = 0;
  nwarnings = 0;
  nlayers = 7;
  verbosity = 0;
  magneticfield = 1.4; // default is 1.5 but Mike's tracking stuff is using 1.4 for now...
  //docalocuts = false;
}



int SimpleTrackingAnalysis::Init(PHCompositeNode *topNode)
{

  // --------------------------------------------------------------------------------
  // --- This is the class initialization method
  // --- Get a pointer to the Fun4All server to register histograms
  // --- Histograms are declared in class header file
  // --- Histograms are instantiated here and then registered with the Fun4All server
  // --------------------------------------------------------------------------------

  cout << "SimpleTrackingAnalysis::Init called" << endl;

  // --- get instance of
  Fun4AllServer *se = Fun4AllServer::instance();



  // --- special stuff...




  // --- additional tracking histograms for studying quality

  _recopt_quality = new TH2D("recopt_quality", "", 20,0.0,10.0, 100,0.0,5.0);
  _recopt_quality_tracks_all = new TH2D("recopt_quality_tracks_all", "", 20,0.0,10.0, 100,0.0,5.0);
  _recopt_quality_tracks_recoWithin4Percent = new TH2D("recopt_quality_tracks_recoWithin4Percent", "", 20,0.0,10.0, 100,0.0,5.0);
  _truept_quality_particles_recoWithin4Percent = new TH2D("truept_quality_particles_recoWithin4Percent", "", 20,0.0,10.0, 100,0.0,5.0);

  se->registerHisto(_recopt_quality);
  se->registerHisto(_recopt_quality_tracks_all);
  se->registerHisto(_recopt_quality_tracks_recoWithin4Percent);
  se->registerHisto(_truept_quality_particles_recoWithin4Percent);



  // --- histograms over true pt, used for finding efficiencies

  _truept_dca = new TH2D("truept_dca", "", 20,0.0,10.0, 200,-0.1,0.1);
  _truept_dptoverpt = new TH2D("truept_dptoverpt", "", 40,0.0,40.0, 200,-0.5,0.5);
  _truept_particles_leavingAllHits = new TH1D("truept_particles_leavingAllHits", "", 20,0.0,10.0);
  _truept_particles_recoWithExactHits = new TH1D("truept_particles_recoWithExactHits", "", 20,0.0,10.0);
  _truept_particles_recoWithin1Hit = new TH1D("truept_particles_recoWithin1Hit", "", 20,0.0,10.0);
  _truept_particles_recoWithin2Hits = new TH1D("truept_particles_recoWithin2Hits", "", 20,0.0,10.0);
  _truept_particles_recoWithin3Percent = new TH1D("truept_particles_recoWithin3Percent", "", 20,0.0,10.0);
  _truept_particles_recoWithin4Percent = new TH1D("truept_particles_recoWithin4Percent", "", 20,0.0,10.0);
  _truept_particles_recoWithin5Percent = new TH1D("truept_particles_recoWithin5Percent", "", 20,0.0,10.0);

  se->registerHisto(_truept_dca);
  se->registerHisto(_truept_dptoverpt);
  se->registerHisto(_truept_particles_leavingAllHits);
  se->registerHisto(_truept_particles_recoWithExactHits);
  se->registerHisto(_truept_particles_recoWithin1Hit);
  se->registerHisto(_truept_particles_recoWithin2Hits);
  se->registerHisto(_truept_particles_recoWithin3Percent);
  se->registerHisto(_truept_particles_recoWithin4Percent);
  se->registerHisto(_truept_particles_recoWithin5Percent);



  // --- (mostly) the same set of histograms over reconstructed pt, used for studying purity

  _recopt_tracks_all = new TH1D("recopt_tracks_all", "", 20,0.0,10.0);
  _recopt_tracks_recoWithExactHits = new TH1D("recopt_tracks_recoWithExactHits", "", 20,0.0,10.0);
  _recopt_tracks_recoWithin1Hit = new TH1D("recopt_tracks_recoWithin1Hit", "", 20,0.0,10.0);
  _recopt_tracks_recoWithin2Hits = new TH1D("recopt_tracks_recoWithin2Hits", "", 20,0.0,10.0);
  _recopt_tracks_recoWithin3Percent = new TH1D("recopt_tracks_recoWithin3Percent", "", 20,0.0,10.0);
  _recopt_tracks_recoWithin4Percent = new TH1D("recopt_tracks_recoWithin4Percent", "", 20,0.0,10.0);
  _recopt_tracks_recoWithin5Percent = new TH1D("recopt_tracks_recoWithin5Percent", "", 20,0.0,10.0);

  se->registerHisto(_recopt_tracks_all);
  se->registerHisto(_recopt_tracks_recoWithExactHits);
  se->registerHisto(_recopt_tracks_recoWithin1Hit);
  se->registerHisto(_recopt_tracks_recoWithin2Hits);
  se->registerHisto(_recopt_tracks_recoWithin3Percent);
  se->registerHisto(_recopt_tracks_recoWithin4Percent);
  se->registerHisto(_recopt_tracks_recoWithin5Percent);



  // --- purity study with calorimeter cuts

  th2d_recopt_tracks_withcalocuts_all = new TH2D(Form("th2d_recopt_tracks_withcalocuts_all"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithExactHits = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithExactHits"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin1Hit = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin1Hit"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin2Hits = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin2Hits"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin3Percent = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin3Percent"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin4Percent = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin4Percent"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin5Percent = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin5Percent"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin1Sigma = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin1Sigma"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin2Sigma = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin2Sigma"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_recopt_tracks_withcalocuts_recoWithin3Sigma = new TH2D(Form("th2d_recopt_tracks_withcalocuts_recoWithin3Sigma"), "", 80,0.0,40.0, 20,0.0,2.0);

  se->registerHisto(th2d_recopt_tracks_withcalocuts_all);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithExactHits);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin1Hit);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin2Hits);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin3Percent);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin4Percent);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin5Percent);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin1Sigma);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin2Sigma);
  se->registerHisto(th2d_recopt_tracks_withcalocuts_recoWithin3Sigma);



  // --- efficiency study with calorimter cuts

  th2d_truept_particles_withcalocuts_leavingAllHits = new TH2D(Form("th2d_truept_particles_withcalocuts_leavingAllHits"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithExactHits = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithExactHits"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin1Hit = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin1Hit"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin2Hits = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin2Hits"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin3Percent = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin3Percent"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin4Percent = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin4Percent"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin5Percent = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin5Percent"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin1Sigma = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin1Sigma"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin2Sigma = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin2Sigma"), "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_truept_particles_withcalocuts_recoWithin3Sigma = new TH2D(Form("th2d_truept_particles_withcalocuts_recoWithin3Sigma"), "", 80,0.0,40.0, 20,0.0,2.0);

  se->registerHisto(th2d_truept_particles_withcalocuts_leavingAllHits);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithExactHits);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin1Hit);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin2Hits);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin3Percent);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin4Percent);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin5Percent);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin1Sigma);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin2Sigma);
  se->registerHisto(th2d_truept_particles_withcalocuts_recoWithin3Sigma);



  // --- new additional (eventual replacement?) histograms for purity study

  th2d_reco_calo_nhits8 = new TH2D("th2d_reco_calo_nhits8", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_nhits7 = new TH2D("th2d_reco_calo_nhits7", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_nhits6 = new TH2D("th2d_reco_calo_nhits6", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_nhits5 = new TH2D("th2d_reco_calo_nhits5", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_nhits4 = new TH2D("th2d_reco_calo_nhits4", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_nhits3 = new TH2D("th2d_reco_calo_nhits3", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_nhits2 = new TH2D("th2d_reco_calo_nhits2", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_nhits1 = new TH2D("th2d_reco_calo_nhits1", "", 80,0.0,40.0, 20,0.0,2.0);

  th2d_reco_calo_pt1sigma = new TH2D("th2d_reco_calo_pt1sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_pt2sigma = new TH2D("th2d_reco_calo_pt2sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_pt3sigma = new TH2D("th2d_reco_calo_pt3sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_pt4sigma = new TH2D("th2d_reco_calo_pt4sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_pt5sigma = new TH2D("th2d_reco_calo_pt5sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_reco_calo_pt6sigma = new TH2D("th2d_reco_calo_pt6sigma", "", 80,0.0,40.0, 20,0.0,2.0);



  // --- new additional (eventual replacement?) histograms for purity study

  th2d_true_calo_nhits8 = new TH2D("th2d_true_calo_nhits8", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_nhits7 = new TH2D("th2d_true_calo_nhits7", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_nhits6 = new TH2D("th2d_true_calo_nhits6", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_nhits5 = new TH2D("th2d_true_calo_nhits5", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_nhits4 = new TH2D("th2d_true_calo_nhits4", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_nhits3 = new TH2D("th2d_true_calo_nhits3", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_nhits2 = new TH2D("th2d_true_calo_nhits2", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_nhits1 = new TH2D("th2d_true_calo_nhits1", "", 80,0.0,40.0, 20,0.0,2.0);

  th2d_true_calo_pt1sigma = new TH2D("th2d_true_calo_pt1sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_pt2sigma = new TH2D("th2d_true_calo_pt2sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_pt3sigma = new TH2D("th2d_true_calo_pt3sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_pt4sigma = new TH2D("th2d_true_calo_pt4sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_pt5sigma = new TH2D("th2d_true_calo_pt5sigma", "", 80,0.0,40.0, 20,0.0,2.0);
  th2d_true_calo_pt6sigma = new TH2D("th2d_true_calo_pt6sigma", "", 80,0.0,40.0, 20,0.0,2.0);



  // --- vertex residual histograms

  _dx_vertex = new TH1D("dx_vertex", "dx_vertex", 200,-0.03,0.03);
  _dy_vertex = new TH1D("dy_vertex", "dy_vertex", 200,-0.03,0.03);
  _dz_vertex = new TH1D("dz_vertex", "dz_vertex", 200,-0.03,0.03);
  se->registerHisto(_dy_vertex);
  se->registerHisto(_dx_vertex);
  se->registerHisto(_dz_vertex);

  hmult = new TH1D("hmult","",5000,-0.5,4999.5);
  hmult_vertex = new TH1D("hmult_vertex","",5000,-0.5,4999.5);
  se->registerHisto(hmult);
  se->registerHisto(hmult_vertex);


  return 0;

}



int SimpleTrackingAnalysis::process_event(PHCompositeNode *topNode)
{

  // --- This is the class process_event method
  // --- This is where the bulk of the analysis is done
  // --- Here we get the various data nodes we need to do the analysis
  // --- Then we use variables (accessed through class methods) to perform calculations

  if ( verbosity > -1 )
    {
      cout << endl;
      cout << "------------------------------------------------------------------------------------" << endl;
      cout << "Now processing event number " << nevents << endl; // would be good to add verbosity switch
    }

  ++nevents; // You may as youtself, why ++nevents (pre-increment) rather
  // than nevents++ (post-increment)?  The short answer is performance.
  // For simple types it probably doesn't matter, but it can really help
  // for complex types (like the iterators below).


  // --- Truth level information
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  if ( !truthinfo )
    {
      cerr << PHWHERE << " ERROR: Can't find G4TruthInfo" << endl;
      exit(-1);
    }

  // --- SvtxTrackMap
  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  if ( !trackmap )
    {
      cerr << PHWHERE << " ERROR: Can't find SvtxTrackMap" << endl;
      exit(-1);
    }

  // --- SvtxVertexMap
  SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
  if ( !vertexmap )
    {
      cerr << PHWHERE << " ERROR: Can't find SvtxVertexMap" << endl;
      exit(-1);
    }



  // --- Create SVTX eval stack
  SvtxEvalStack svtxevalstack(topNode);
  // --- Get evaluator objects from the eval stack
  SvtxVertexEval*   vertexeval = svtxevalstack.get_vertex_eval();
  SvtxTrackEval*     trackeval = svtxevalstack.get_track_eval();
  SvtxTruthEval*     trutheval = svtxevalstack.get_truth_eval();



  if ( verbosity > 0 ) cout << "Now going to loop over truth partcles..." << endl; // need verbosity switch

  // --- Loop over all truth particles
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter )
    {
      PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?
      // In C++ the iterator is a map, which has two members
      // first is the key (analogous the index of an arry),
      // second is the value (analogous to the value stored for the array index)
      int particleID = g4particle->get_pid();

      if ( trutheval->get_embed(g4particle) <= 0 && fabs(particleID) == 11 && verbosity > 0 )
        {
          cout << "NON EMBEDDED ELECTRON!!!  WHEE!!! " << particleID << " " << iter->first << endl;
        }

      if ( trutheval->get_embed(g4particle) <= 0 ) continue; // only look at embedded particles // no good for hits files
      bool iselectron = fabs(particleID) == 11;
      bool ispion = fabs(particleID) == 211;
      if ( verbosity > 0 ) cout << "embedded particle ID is " << particleID << " ispion " << ispion << " iselectron " << iselectron << " " << iter->first << endl;

      set<PHG4Hit*> g4hits = trutheval->all_truth_hits(g4particle);
      float ng4hits = g4hits.size();

      float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
      float true_energy = g4particle->get_e();

      // --- Get the reconsructed SvtxTrack based on the best candidate from the truth info
      SvtxTrack* track = trackeval->best_track_from(g4particle);
      if (!track) continue;
      float recopt = track->get_pt();
      float recop = track->get_p();

      if ( verbosity > 0 )
	{
	  cout << "truept is " << truept << endl;
	  cout << "recopt is " << recopt << endl;
	  cout << "true energy is " << true_energy << endl;
	}

      // --- energy variables directly from the track object
      float emc_energy_track = track->get_cal_energy_3x3(SvtxTrack::CEMC);
      float hci_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALIN);
      float hco_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALOUT);

      if ( verbosity > 0 )
	{
	  cout << "emc_energy_track is " << emc_energy_track << endl;
	  cout << "hci_energy_track is " << hci_energy_track << endl;
	  cout << "hco_energy_track is " << hco_energy_track << endl;
	}

      // -------------------------------------------------------------------------------------
      // --- IMPORTANT NOTE: according to Jin, dphi and deta will not work correctly in HIJING

      float emc_dphi_track = track->get_cal_dphi(SvtxTrack::CEMC);
      float hci_dphi_track = track->get_cal_dphi(SvtxTrack::HCALIN);
      float hco_dphi_track = track->get_cal_dphi(SvtxTrack::HCALOUT);

      float emc_deta_track = track->get_cal_deta(SvtxTrack::CEMC);
      float hci_deta_track = track->get_cal_deta(SvtxTrack::HCALIN);
      float hco_deta_track = track->get_cal_deta(SvtxTrack::HCALOUT);

      float assoc_dphi = 0.1; // adjust as needed, consider class set method
      float assoc_deta = 0.1; // adjust as needed, consider class set method
      bool good_emc_assoc = fabs(emc_dphi_track) < assoc_dphi && fabs(emc_deta_track) < assoc_deta;
      bool good_hci_assoc = fabs(hci_dphi_track) < assoc_dphi && fabs(hci_deta_track) < assoc_deta;
      bool good_hco_assoc = fabs(hco_dphi_track) < assoc_dphi && fabs(hco_deta_track) < assoc_deta;

      // ------------------------------------------------------------------------------------------


      float hct_energy_track = 0;
      if ( hci_energy_track >= 0 ) hct_energy_track += hci_energy_track;
      if ( hco_energy_track >= 0 ) hct_energy_track += hco_energy_track;

      float total_energy_dumb = 0;
      if ( emc_energy_track >= 0 ) total_energy_dumb += emc_energy_track;
      if ( hci_energy_track >= 0 ) total_energy_dumb += hci_energy_track;
      if ( hco_energy_track >= 0 ) total_energy_dumb += hco_energy_track;

      float total_energy_smart = 0;
      if ( good_emc_assoc ) total_energy_smart += emc_energy_track;
      if ( good_hci_assoc ) total_energy_smart += hci_energy_track;
      if ( good_hco_assoc ) total_energy_smart += hco_energy_track;



      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------
      // ----------------------------------------------------------------------

      //cout << "starting the main part of the truth analysis" << endl;

      // examine truth particles that leave all (7 or 8 depending on design) detector hits
      if ( ng4hits == nlayers )
	{
	  _truept_particles_leavingAllHits->Fill(truept);

	  unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);

	  unsigned int ndiff = abs((int)nfromtruth-(int)nlayers);
	  if ( ndiff <= 2 ) _truept_particles_recoWithin2Hits->Fill(truept);
	  if ( ndiff <= 1 ) _truept_particles_recoWithin1Hit->Fill(truept);
	  if ( ndiff == 0 ) _truept_particles_recoWithExactHits->Fill(truept);

	  float diff = fabs(recopt-truept)/truept;
	  if ( diff < 0.05 ) _truept_particles_recoWithin5Percent->Fill(truept);
	  if ( diff < 0.04 )
	    {
	      _truept_particles_recoWithin4Percent->Fill(truept);
	      _truept_quality_particles_recoWithin4Percent->Fill(truept,track->get_quality());
	    }
	  if ( diff < 0.03 ) _truept_particles_recoWithin3Percent->Fill(truept);

          double good_energy = total_energy_dumb - 3.14;


          double eoverp = good_energy/recop;
          double sigmapt = 0.011 + 0.0008*recopt;
          th2d_truept_particles_withcalocuts_leavingAllHits->Fill(truept,eoverp);
          if ( ndiff <= 2 ) th2d_truept_particles_withcalocuts_recoWithin2Hits->Fill(truept,eoverp);
          if ( ndiff <= 1 ) th2d_truept_particles_withcalocuts_recoWithin1Hit->Fill(truept,eoverp);
          if ( ndiff == 0 ) th2d_truept_particles_withcalocuts_recoWithExactHits->Fill(truept,eoverp);
          if ( diff < 0.05 ) th2d_truept_particles_withcalocuts_recoWithin5Percent->Fill(truept,eoverp);
          if ( diff < 0.04 ) th2d_truept_particles_withcalocuts_recoWithin4Percent->Fill(truept,eoverp);
          if ( diff < 0.03 ) th2d_truept_particles_withcalocuts_recoWithin3Percent->Fill(truept,eoverp);
          if ( diff < 1.0*sigmapt ) th2d_truept_particles_withcalocuts_recoWithin1Sigma->Fill(recopt,eoverp);
          if ( diff < 2.0*sigmapt ) th2d_truept_particles_withcalocuts_recoWithin2Sigma->Fill(recopt,eoverp);
          if ( diff < 3.0*sigmapt ) th2d_truept_particles_withcalocuts_recoWithin3Sigma->Fill(recopt,eoverp);


	} // end of requirement of ng4hits == nlayers

    } // end of loop over truth particles



  // loop over all reco particles
  int ntracks = 0;
  for ( SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter )
    {

      // --- Get the StxTrack object (from the iterator)
      SvtxTrack* track = iter->second;
      float recopt = track->get_pt();
      float recop = track->get_p();

      // --- Get the truth particle from the evaluator
      PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track);
      float truept = sqrt(pow(g4particle->get_px(),2)+pow(g4particle->get_py(),2));
      int particleID = g4particle->get_pid();
      if ( verbosity > 5 ) cout << "particle ID is " << particleID << endl;
      bool iselectron = fabs(particleID) == 11;
      bool ispion = fabs(particleID) == 211;

      // ---------------------
      // --- calorimeter stuff
      // ---------------------

      // --- get the energy values directly from the track
      float emc_energy_track = track->get_cal_energy_3x3(SvtxTrack::CEMC);
      float hci_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALIN);
      float hco_energy_track = track->get_cal_energy_3x3(SvtxTrack::HCALOUT);

      float total_energy = 0;
      if ( emc_energy_track > 0 ) total_energy += emc_energy_track;
      if ( hci_energy_track > 0 ) total_energy += hci_energy_track;
      if ( hco_energy_track > 0 ) total_energy += hco_energy_track;

      if ( verbosity > 2 ) cout << "total calo energy is " << total_energy << endl;

      if (trutheval->get_embed(g4particle) > 0)
	{
	  // embedded results (quality or performance measures)
	  _truept_dptoverpt->Fill(truept,(recopt-truept)/truept);
	  _truept_dca->Fill(truept,track->get_dca2d());
	  _recopt_quality->Fill(recopt,track->get_quality());
          if ( verbosity > 0 ) cout << "embedded particle ID is " << particleID << " ispion " << ispion << " iselectron " << iselectron << endl;
          // ---
	} // end if (embedded results)
      else
	{
          // electron and pion (hadron) id

	  // non-embedded results (purity measures)
	  _recopt_tracks_all->Fill(recopt);
	  _recopt_quality_tracks_all->Fill(recopt,track->get_quality());

	  unsigned int nfromtruth = trackeval->get_nclusters_contribution(track,g4particle);

	  unsigned int ndiff = abs((int)nfromtruth-(int)nlayers);
	  if ( ndiff <= 2 ) _recopt_tracks_recoWithin2Hits->Fill(recopt);
	  if ( ndiff <= 1 ) _recopt_tracks_recoWithin1Hit->Fill(recopt);
	  if ( ndiff == 0 ) _recopt_tracks_recoWithExactHits->Fill(recopt);

	  float diff = fabs(recopt-truept)/truept;
	  if ( diff < 0.05 ) _recopt_tracks_recoWithin5Percent->Fill(recopt);
	  if ( diff < 0.04 )
	    {
	      _recopt_tracks_recoWithin4Percent->Fill(recopt);
	      _recopt_quality_tracks_recoWithin4Percent->Fill(recopt,track->get_quality());
	    }
	  if ( diff < 0.03 ) _recopt_tracks_recoWithin3Percent->Fill(recopt);


	  // --------------------------------------
	  // --- same but now with calorimeter cuts
	  // --------------------------------------

          double good_energy = total_energy - 3.14;

          double eoverp = good_energy/recop;
          double sigmapt = 0.011 + 0.0008*recopt;
          th2d_recopt_tracks_withcalocuts_all->Fill(recopt,eoverp);
          if ( ndiff <= 2 ) th2d_recopt_tracks_withcalocuts_recoWithin2Hits->Fill(recopt,eoverp);
          if ( ndiff <= 1 ) th2d_recopt_tracks_withcalocuts_recoWithin1Hit->Fill(recopt,eoverp);
          if ( ndiff == 0 ) th2d_recopt_tracks_withcalocuts_recoWithExactHits->Fill(recopt,eoverp);
          if ( diff < 0.05 ) th2d_recopt_tracks_withcalocuts_recoWithin5Percent->Fill(recopt,eoverp);
          if ( diff < 0.04 ) th2d_recopt_tracks_withcalocuts_recoWithin4Percent->Fill(recopt,eoverp);
          if ( diff < 0.03 ) th2d_recopt_tracks_withcalocuts_recoWithin3Percent->Fill(recopt,eoverp);
          if ( diff < 1.0*sigmapt ) th2d_recopt_tracks_withcalocuts_recoWithin1Sigma->Fill(recopt,eoverp);
          if ( diff < 2.0*sigmapt ) th2d_recopt_tracks_withcalocuts_recoWithin2Sigma->Fill(recopt,eoverp);
          if ( diff < 3.0*sigmapt ) th2d_recopt_tracks_withcalocuts_recoWithin3Sigma->Fill(recopt,eoverp);

	  // --- done with reco tracks

	} // else (non-embedded results)

      ++ntracks;
    } // loop over reco tracks


  hmult->Fill(ntracks);

  // --- Get the leading vertex
  SvtxVertex* maxvertex = NULL;
  unsigned int maxtracks = 0;
  for ( SvtxVertexMap::Iter iter = vertexmap->begin(); iter != vertexmap->end(); ++iter )
    {
      SvtxVertex* vertex = iter->second;
      if ( vertex->size_tracks() > maxtracks )
	{
	  maxvertex = vertex;
	  maxtracks = vertex->size_tracks();
	}
    }
  if ( !maxvertex )
    {
      cerr << PHWHERE << " ERROR: cannot get reconstructed vertex (event number " << nevents << ")" << endl;
      ++nerrors;
      return Fun4AllReturnCodes::DISCARDEVENT;
    }

  // --- Get the coordinates for the vertex from the evaluator
  PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(maxvertex);
  if ( !point )
    {
      cerr << PHWHERE << " ERROR: cannot get truth vertex (event number " << nevents << ")" << endl;
      ++nerrors;
      return Fun4AllReturnCodes::DISCARDEVENT;
    }
  _dx_vertex->Fill(maxvertex->get_x() - point->get_x());
  _dy_vertex->Fill(maxvertex->get_y() - point->get_y());
  _dz_vertex->Fill(maxvertex->get_z() - point->get_z());

  hmult_vertex->Fill(ntracks);



  return Fun4AllReturnCodes::EVENT_OK;

}



int SimpleTrackingAnalysis::End(PHCompositeNode *topNode)
{
  cout << "End called, " << nevents << " events processed with " << nerrors << " errors and " << nwarnings << " warnings." << endl;
  return 0;
}

