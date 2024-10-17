#include "AnaUPC.h"

/// Cluster/Calorimeter includes
/*
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>
*/

/// Tracking includes
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>

/// Truth evaluation includes
//#include <g4eval/JetEvalStack.h>
#include <g4eval/SvtxEvalStack.h>

/// HEPMC truth includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

/// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TTree.h>
//#include <TLorentzVector.h>
#include <Math/Vector4D.h>


/// C++ includes
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

/**
 * This class demonstrates the construction and use of an analysis module
 * within the sPHENIX Fun4All framework. It is intended to show how to
 * obtain physics objects from the analysis tree, and then write them out
 * to a ROOT tree and file for further offline analysis.
 */

/**
 * Constructor of module
 */
AnaUPC::AnaUPC(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , m_outfilename(filename)
  , m_hm(nullptr)
  //, m_mincluspt(0.25)
  , m_analyzeTracks(true)
  //, m_analyzeClusters(true)
  , m_analyzeTruth(false)
{
  /// Initialize variables and trees so we don't accidentally access
  /// memory that was never allocated
  initializeVariables();
  initializeTrees();
}

/**
 * Destructor of module
 */
AnaUPC::~AnaUPC()
{
  delete m_hm;
  delete m_hepmctree;
  delete m_tracktree;
}

/**
 * Initialize the module and prepare looping over events
 */
int AnaUPC::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in AnaUPC" << std::endl;
  }

  m_outfile = new TFile(m_outfilename.c_str(), "RECREATE");

  h_phi = new TH1F("h_phi", "#phi [rad]", 60, -M_PI, M_PI);
  h2_eta_phi = new TH2F("h2_phi_eta", ";#eta;#phi [rad]", 24, -5.0, 5.0, 60, -M_PI, M_PI);
  h_mass = new TH1F("h_mass", "mass [GeV]", 1200, 0, 6);
  h_pt = new TH1F("h_pt", "p_{T}", 200, 0, 2);
  h_y = new TH1F("h_y", "y", 24, -1.2, 1.2);
  h_eta = new TH1F("h_eta", "#eta", 24, -5.0, 5.0);

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int AnaUPC::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in AnaUPC" << std::endl;
  }
  /// Get the truth information
  if (m_analyzeTruth)
  {
    getHEPMCTruth(topNode);
    getPHG4Truth(topNode);
  }

  /// Get the tracks
  if (m_analyzeTracks)
  {
    getTracks(topNode);
  }

  /// Get calorimeter information
  /*
  if (m_analyzeClusters)
  {
    getEMCalClusters(topNode);
  }
  */

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int AnaUPC::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending AnaUPC analysis package" << std::endl;
  }

  /// Change to the outfile
  m_outfile->cd();

  /// If we analyzed the tracks, write the tree out
  if (m_analyzeTracks)
  {
    m_tracktree->Write();
  }

  /// If we analyzed the truth particles, write them out
  if (m_analyzeTruth)
  {
    m_hepmctree->Write();
    m_truthtree->Write();
  }

  /// If we analyzed the clusters, write them out
  /*
  if (m_analyzeClusters)
  {
    m_clustertree->Write();
  }
  */

  /// Write and close the outfile
  m_outfile->Write();
  m_outfile->Close();

  /// Clean up pointers and associated histos/trees in TFile
  delete m_outfile;

  if (Verbosity() > 1)
  {
    std::cout << "Finished AnaUPC analysis package" << std::endl;
  }

  return 0;
}

/**
 * This method gets all of the HEPMC truth particles from the node tree
 * and stores them in a ROOT TTree. The HEPMC truth particles are what,
 * for example, directly comes out of PYTHIA and thus gives you all of
 * the associated parton information
 */
void AnaUPC::getHEPMCTruth(PHCompositeNode *topNode)
{
  /// Get the node from the node tree
  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");

  /// If the node was not properly put on the tree, return
  if (!hepmceventmap)
  {
    std::cout << PHWHERE
              << "HEPMC event map node is missing, can't collected HEPMC truth particles"
              << std::endl;
    return;
  }

  /// Could have some print statements for debugging with verbosity
  if (Verbosity() > 1)
  {
    std::cout << "Getting HEPMC truth particles " << std::endl;
  }

  /// You can iterate over the number of events in a hepmc event
  /// for pile up events where you have multiple hard scatterings per bunch crossing
  for (PHHepMCGenEventMap::ConstIter eventIter = hepmceventmap->begin();
       eventIter != hepmceventmap->end();
       ++eventIter)
  {
    /// Get the event
    PHHepMCGenEvent *hepmcevent = eventIter->second;

    if (hepmcevent)
    {
      /// Get the event characteristics, inherited from HepMC classes
      HepMC::GenEvent *truthevent = hepmcevent->getEvent();
      if (!truthevent)
      {
        std::cout << PHWHERE
                  << "no evt pointer under phhepmvgeneventmap found "
                  << std::endl;
        return;
      }

      /// Get the parton info
      HepMC::PdfInfo *pdfinfo = truthevent->pdf_info();

      /// Get the parton info as determined from HEPMC
      m_partid1 = pdfinfo->id1();
      m_partid2 = pdfinfo->id2();
      m_x1 = pdfinfo->x1();
      m_x2 = pdfinfo->x2();

      /// Are there multiple partonic intercations in a p+p event
      m_mpi = truthevent->mpi();

      /// Get the PYTHIA signal process id identifying the 2-to-2 hard process
      m_process_id = truthevent->signal_process_id();

      if (Verbosity() > 2)
      {
        std::cout << " Iterating over an event" << std::endl;
      }
      /// Loop over all the truth particles and get their information
      for (HepMC::GenEvent::particle_const_iterator iter = truthevent->particles_begin(); iter != truthevent->particles_end(); ++iter)
      {
        /// Get each pythia particle characteristics
        m_truthenergy = (*iter)->momentum().e();
        m_truthpid = (*iter)->pdg_id();

        m_trutheta = (*iter)->momentum().pseudoRapidity();
        m_truthphi = (*iter)->momentum().phi();
        m_truthpx = (*iter)->momentum().px();
        m_truthpy = (*iter)->momentum().py();
        m_truthpz = (*iter)->momentum().pz();
        m_truthpt = sqrt(m_truthpx * m_truthpx + m_truthpy * m_truthpy);

        /// Fill the truth tree
        m_hepmctree->Fill();
        m_numparticlesinevent++;
      }
    }
  }
}

/**
 * This function collects the truth PHG4 stable particles that
 * are produced from PYTHIA, or some other event generator. These
 * are the stable particles, e.g. there are not any (for example)
 * partons here.
 */
void AnaUPC::getPHG4Truth(PHCompositeNode *topNode)
{
  /// G4 truth particle node
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  if (!truthinfo)
  {
    std::cout << PHWHERE
              << "PHG4TruthInfoContainer node is missing, can't collect G4 truth particles"
              << std::endl;
    return;
  }

  /// Get the primary particle range
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  /// Loop over the G4 truth (stable) particles
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
  {
    /// Get this truth particle
    const PHG4Particle *truth = iter->second;

    /// Get this particles momentum, etc.
    m_truthpx = truth->get_px();
    m_truthpy = truth->get_py();
    m_truthpz = truth->get_pz();
    m_truthp = sqrt(m_truthpx * m_truthpx + m_truthpy * m_truthpy + m_truthpz * m_truthpz);
    m_truthenergy = truth->get_e();

    m_truthpt = sqrt(m_truthpx * m_truthpx + m_truthpy * m_truthpy);

    m_truthphi = atan(m_truthpy / m_truthpx);

    m_trutheta = atanh(m_truthpz / m_truthenergy);
    /// Check for nans
    if (!std::isfinite(m_trutheta))
    {
      m_trutheta = -99;
    }
    m_truthpid = truth->get_pid();

    /// Fill the g4 truth tree
    m_truthtree->Fill();
  }
}

/**
 * This method gets the tracks as reconstructed from the tracker. It also
 * compares the reconstructed track to its truth track counterpart as determined
 * by the
 */
void AnaUPC::getTracks(PHCompositeNode *topNode)
{
  /// SVTX tracks node
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  if (!trackmap)
  {
    std::cout << PHWHERE
              << "SvtxTrackMap node is missing, can't collect tracks"
              << std::endl;
    return;
  }

  /// EvalStack for truth track matching
  if (!m_svtxEvalStack)
  {
    m_svtxEvalStack = new SvtxEvalStack(topNode);
    m_svtxEvalStack->set_verbosity(Verbosity());
  }

  m_svtxEvalStack->next_event(topNode);

  /// Get the track evaluator
  SvtxTrackEval *trackeval = m_svtxEvalStack->get_track_eval();

  /// Get the range for primary tracks
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  if (Verbosity() > 1)
  {
    std::cout << "Get the SVTX tracks" << std::endl;
  }
  for (auto &iter : *trackmap)
  {
    SvtxTrack *track = iter.second;

    /// Get the reconstructed track info
    m_tr_px = track->get_px();
    m_tr_py = track->get_py();
    m_tr_pz = track->get_pz();
    m_tr_p = sqrt(m_tr_px * m_tr_px + m_tr_py * m_tr_py + m_tr_pz * m_tr_pz);

    m_tr_pt = sqrt(m_tr_px * m_tr_px + m_tr_py * m_tr_py);

    // Make some cuts on the track to clean up sample
    if (m_tr_pt < 0.5)
    {
      continue;
    }
    m_tr_phi = track->get_phi();
    m_tr_eta = track->get_eta();

    m_charge = track->get_charge();
    m_chisq = track->get_chisq();
    m_ndf = track->get_ndf();
    m_dca = track->get_dca();
    m_tr_x = track->get_x();
    m_tr_y = track->get_y();
    m_tr_z = track->get_z();

    /// Get truth track info that matches this reconstructed track
    PHG4Particle *truthtrack = trackeval->max_truth_particle_by_nclusters(track);
    if ( truthtrack != nullptr )
    {
      m_truth_is_primary = truthinfo->is_primary(truthtrack);

      m_truthtrackpx = truthtrack->get_px();
      m_truthtrackpy = truthtrack->get_py();
      m_truthtrackpz = truthtrack->get_pz();
      m_truthtrackp = std::sqrt(m_truthtrackpx * m_truthtrackpx + m_truthtrackpy * m_truthtrackpy + m_truthtrackpz * m_truthtrackpz);

      m_truthtracke = truthtrack->get_e();

      m_truthtrackpt = sqrt(m_truthtrackpx * m_truthtrackpx + m_truthtrackpy * m_truthtrackpy);
      m_truthtrackphi = atan(m_truthtrackpy / m_truthtrackpx);
      m_truthtracketa = atanh(m_truthtrackpz / m_truthtrackp);
      m_truthtrackpid = truthtrack->get_pid();
    }
    else
    {
      std::cout << "Missing truth track" << std::endl;
      m_truth_is_primary = -9999;

      m_truthtrackpx = 0.;
      m_truthtrackpy = 0.;
      m_truthtrackpz = 0.;
      m_truthtrackp = 0.;

      m_truthtracke = 0.;

      m_truthtrackpt = 0.;
      m_truthtrackphi = 0.;
      m_truthtracketa = 0.;
      m_truthtrackpid = 0;
    }

    m_tracktree->Fill();
  }
  
  //TLorentzVector v1, v2;
  ROOT::Math::XYZTVector v1, v2;

  // make pairs
  //for (auto &iter1 : *trackmap)
  for (auto iter1 = trackmap->begin(); iter1 != trackmap->end(); iter1++)
  {
    for (auto iter2 = iter1; iter2 != trackmap->end(); iter2++)
    {
      if ( iter2 == iter1 ) continue;

      //SvtxTrack *track2 = iter2.second;
      std::cout << "XXX " << iter1->first << "\t" << iter2->first << std::endl;
      SvtxTrack *track1 = iter1->second;
      SvtxTrack *track2 = iter2->second;

      double px1 = track1->get_px();
      double py1 = track1->get_py();
      double pz1 = track1->get_pz();
      double e1 = sqrt( E_MASS*E_MASS + px1*px1 + py1*py1 + pz1*pz1 );
      v1.SetPxPyPzE( px1, py1, pz1, e1 );

      double px2 = track2->get_px();
      double py2 = track2->get_py();
      double pz2 = track2->get_pz();
      double e2 = sqrt( E_MASS*E_MASS + px2*px2 + py2*py2 + pz2*pz2 );
      v2.SetPxPyPzE( px2, py2, pz2, e2 );

      //TLorentzVector sum = v1 + v2;
      ROOT::Math::XYZTVector sum = v1 + v2;
      double invmass = sum.M();
      double pt = sum.Pt();
      double y = sum.Rapidity();
      double eta = sum.Eta();
      double phi = sum.Phi();
      h_mass->Fill( invmass );
      h_pt->Fill( pt );
      h_y->Fill( y );
      h_eta->Fill( eta );
      h2_eta_phi->Fill( eta, phi );
      h_phi->Fill( phi );
    }
  }

}


/**
 * This method gets clusters from the EMCal and stores them in a tree. It
 * also demonstrates how to get trigger emulator information. Clusters from
 * other containers can be obtained in a similar way (e.g. clusters from
 * the IHCal, etc.)
 */
/*
void AnaUPC::getEMCalClusters(PHCompositeNode *topNode)
{
  /// Get the raw cluster container
  /// Note: other cluster containers exist as well. Check out the node tree when
  /// you run a simulation
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");

  if (!clusters)
  {
    std::cout << PHWHERE
              << "EMCal cluster node is missing, can't collect EMCal clusters"
              << std::endl;
    return;
  }

  /// Get the global vertex to determine the appropriate pseudorapidity of the clusters
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    std::cout << "AnaUPC::getEmcalClusters - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
    assert(vertexmap);  // force quit

    return;
  }

  if (vertexmap->empty())
  {
    std::cout << "AnaUPC::getEmcalClusters - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
    return;
  }

  /// just take a bbc vertex
  GlobalVertex *vtx = nullptr;
  for(auto iter = vertexmap->begin(); iter!= vertexmap->end(); ++iter)
    {
      GlobalVertex* vertex = iter->second;
      if(vertex->find_vtxids(GlobalVertex::MBD) != vertex->end_vtxids())
	{
	  vtx = vertex;
	}
    }
  if (vtx == nullptr)
  {
    return;
  }

  /// Trigger emulator
  CaloTriggerInfo *trigger = findNode::getClass<CaloTriggerInfo>(topNode, "CaloTriggerInfo");

  /// Can obtain some trigger information if desired
  if (trigger)
  {
    m_E_4x4 = trigger->get_best_EMCal_4x4_E();
  }
  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator clusIter;

  /// Loop over the EMCal clusters
  for (clusIter = begin_end.first; clusIter != begin_end.second; ++clusIter)
  {
    /// Get this cluster
    const RawCluster *cluster = clusIter->second;

    /// Get cluster characteristics
    /// This helper class determines the photon characteristics
    /// depending on the vertex position
    /// This is important for e.g. eta determination and E_T determination
    CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);
    m_clusenergy = E_vec_cluster.mag();
    m_cluseta = E_vec_cluster.pseudoRapidity();
    m_clustheta = E_vec_cluster.getTheta();
    m_cluspt = E_vec_cluster.perp();
    m_clusphi = E_vec_cluster.getPhi();

    if (m_cluspt < m_mincluspt)
    {
      continue;
    }

    m_cluspx = m_cluspt * cos(m_clusphi);
    m_cluspy = m_cluspt * sin(m_clusphi);
    m_cluspz = sqrt(m_clusenergy * m_clusenergy - m_cluspx * m_cluspx - m_cluspy * m_cluspy);

    // fill the cluster tree with all emcal clusters
    m_clustertree->Fill();
  }
}
*/

/**
 * This function puts all of the tree branch assignments in one place so as to not
 * clutter up the AnaUPC::Init function.
 */
void AnaUPC::initializeTrees()
{
  /*
  m_recojettree = new TTree("jettree", "A tree with reconstructed jets");
  m_recojettree->Branch("m_recojetpt", &m_recojetpt, "m_recojetpt/D");
  m_recojettree->Branch("m_recojetid", &m_recojetid, "m_recojetid/I");
  m_recojettree->Branch("m_recojetpx", &m_recojetpx, "m_recojetpx/D");
  m_recojettree->Branch("m_recojetpy", &m_recojetpy, "m_recojetpy/D");
  m_recojettree->Branch("m_recojetpz", &m_recojetpz, "m_recojetpz/D");
  m_recojettree->Branch("m_recojetphi", &m_recojetphi, "m_recojetphi/D");
  m_recojettree->Branch("m_recojeteta", &m_recojeteta, "m_recojeteta/D");
  m_recojettree->Branch("m_recojetenergy", &m_recojetenergy, "m_recojetenergy/D");
  m_recojettree->Branch("m_truthjetid", &m_truthjetid, "m_truthjetid/I");
  m_recojettree->Branch("m_truthjetp", &m_truthjetp, "m_truthjetp/D");
  m_recojettree->Branch("m_truthjetphi", &m_truthjetphi, "m_truthjetphi/D");
  m_recojettree->Branch("m_truthjeteta", &m_truthjeteta, "m_truthjeteta/D");
  m_recojettree->Branch("m_truthjetpt", &m_truthjetpt, "m_truthjetpt/D");
  m_recojettree->Branch("m_truthjetenergy", &m_truthjetenergy, "m_truthjetenergy/D");
  m_recojettree->Branch("m_truthjetpx", &m_truthjetpx, "m_truthjetpx/D");
  m_recojettree->Branch("m_truthjetpy", &m_truthjetpy, "m_truthjyetpy/D");
  m_recojettree->Branch("m_truthjetpz", &m_truthjetpz, "m_truthjetpz/D");
  m_recojettree->Branch("m_dR", &m_dR, "m_dR/D");

  m_truthjettree = new TTree("truthjettree", "A tree with truth jets");
  m_truthjettree->Branch("m_truthjetid", &m_truthjetid, "m_truthjetid/I");
  m_truthjettree->Branch("m_truthjetp", &m_truthjetp, "m_truthjetp/D");
  m_truthjettree->Branch("m_truthjetphi", &m_truthjetphi, "m_truthjetphi/D");
  m_truthjettree->Branch("m_truthjeteta", &m_truthjeteta, "m_truthjeteta/D");
  m_truthjettree->Branch("m_truthjetpt", &m_truthjetpt, "m_truthjetpt/D");
  m_truthjettree->Branch("m_truthjetenergy", &m_truthjetenergy, "m_truthjetenergy/D");
  m_truthjettree->Branch("m_truthjetpx", &m_truthjetpx, "m_truthjetpx/D");
  m_truthjettree->Branch("m_truthjetpy", &m_truthjetpy, "m_truthjetpy/D");
  m_truthjettree->Branch("m_truthjetpz", &m_truthjetpz, "m_truthjetpz/D");
  m_truthjettree->Branch("m_dR", &m_dR, "m_dR/D");
  m_truthjettree->Branch("m_recojetpt", &m_recojetpt, "m_recojetpt/D");
  m_truthjettree->Branch("m_recojetid", &m_recojetid, "m_recojetid/I");
  m_truthjettree->Branch("m_recojetpx", &m_recojetpx, "m_recojetpx/D");
  m_truthjettree->Branch("m_recojetpy", &m_recojetpy, "m_recojetpy/D");
  m_truthjettree->Branch("m_recojetpz", &m_recojetpz, "m_recojetpz/D");
  m_truthjettree->Branch("m_recojetphi", &m_recojetphi, "m_recojetphi/D");
  m_truthjettree->Branch("m_recojeteta", &m_recojeteta, "m_recojeteta/D");
  m_truthjettree->Branch("m_recojetenergy", &m_recojetenergy, "m_recojetenergy/D");
  */

  m_tracktree = new TTree("tracktree", "A tree with svtx tracks");
  m_tracktree->Branch("m_tr_px", &m_tr_px, "m_tr_px/D");
  m_tracktree->Branch("m_tr_py", &m_tr_py, "m_tr_py/D");
  m_tracktree->Branch("m_tr_pz", &m_tr_pz, "m_tr_pz/D");
  m_tracktree->Branch("m_tr_p", &m_tr_p, "m_tr_p/D");
  m_tracktree->Branch("m_tr_pt", &m_tr_pt, "m_tr_pt/D");
  m_tracktree->Branch("m_tr_phi", &m_tr_phi, "m_tr_phi/D");
  m_tracktree->Branch("m_tr_eta", &m_tr_eta, "m_tr_eta/D");
  m_tracktree->Branch("m_charge", &m_charge, "m_charge/I");
  m_tracktree->Branch("m_chisq", &m_chisq, "m_chisq/D");
  m_tracktree->Branch("m_ndf", &m_ndf, "m_ndf/I");
  m_tracktree->Branch("m_dca", &m_dca, "m_dca/D");
  m_tracktree->Branch("m_tr_x", &m_tr_x, "m_tr_x/D");
  m_tracktree->Branch("m_tr_y", &m_tr_y, "m_tr_y/D");
  m_tracktree->Branch("m_tr_z", &m_tr_z, "m_tr_z/D");
  m_tracktree->Branch("m_truth_is_primary", &m_truth_is_primary, "m_truth_is_primary/I");
  m_tracktree->Branch("m_truthtrackpx", &m_truthtrackpx, "m_truthtrackpx/D");
  m_tracktree->Branch("m_truthtrackpy", &m_truthtrackpy, "m_truthtrackpy/D");
  m_tracktree->Branch("m_truthtrackpz", &m_truthtrackpz, "m_truthtrackpz/D");
  m_tracktree->Branch("m_truthtrackp", &m_truthtrackp, "m_truthtrackp/D");
  m_tracktree->Branch("m_truthtracke", &m_truthtracke, "m_truthtracke/D");
  m_tracktree->Branch("m_truthtrackpt", &m_truthtrackpt, "m_truthtrackpt/D");
  m_tracktree->Branch("m_truthtrackphi", &m_truthtrackphi, "m_truthtrackphi/D");
  m_tracktree->Branch("m_truthtracketa", &m_truthtracketa, "m_truthtracketa/D");
  m_tracktree->Branch("m_truthtrackpid", &m_truthtrackpid, "m_truthtrackpid/I");

  m_hepmctree = new TTree("hepmctree", "A tree with hepmc truth particles");
  m_hepmctree->Branch("m_partid1", &m_partid1, "m_partid1/I");
  m_hepmctree->Branch("m_partid2", &m_partid2, "m_partid2/I");
  m_hepmctree->Branch("m_x1", &m_x1, "m_x1/D");
  m_hepmctree->Branch("m_x2", &m_x2, "m_x2/D");
  m_hepmctree->Branch("m_mpi", &m_mpi, "m_mpi/I");
  m_hepmctree->Branch("m_process_id", &m_process_id, "m_process_id/I");
  m_hepmctree->Branch("m_truthenergy", &m_truthenergy, "m_truthenergy/D");
  m_hepmctree->Branch("m_trutheta", &m_trutheta, "m_trutheta/D");
  m_hepmctree->Branch("m_truthphi", &m_truthphi, "m_truthphi/D");
  m_hepmctree->Branch("m_truthpx", &m_truthpx, "m_truthpx/D");
  m_hepmctree->Branch("m_truthpy", &m_truthpy, "m_truthpy/D");
  m_hepmctree->Branch("m_truthpz", &m_truthpz, "m_truthpz/D");
  m_hepmctree->Branch("m_truthpt", &m_truthpt, "m_truthpt/D");
  m_hepmctree->Branch("m_numparticlesinevent", &m_numparticlesinevent, "m_numparticlesinevent/I");
  m_hepmctree->Branch("m_truthpid", &m_truthpid, "m_truthpid/I");

  m_truthtree = new TTree("truthg4tree", "A tree with truth g4 particles");
  m_truthtree->Branch("m_truthenergy", &m_truthenergy, "m_truthenergy/D");
  m_truthtree->Branch("m_truthp", &m_truthp, "m_truthp/D");
  m_truthtree->Branch("m_truthpx", &m_truthpx, "m_truthpx/D");
  m_truthtree->Branch("m_truthpy", &m_truthpy, "m_truthpy/D");
  m_truthtree->Branch("m_truthpz", &m_truthpz, "m_truthpz/D");
  m_truthtree->Branch("m_truthpt", &m_truthpt, "m_truthpt/D");
  m_truthtree->Branch("m_truthphi", &m_truthphi, "m_truthphi/D");
  m_truthtree->Branch("m_trutheta", &m_trutheta, "m_trutheta/D");
  m_truthtree->Branch("m_truthpid", &m_truthpid, "m_truthpid/I");

  /*
  m_clustertree = new TTree("clustertree", "A tree with emcal clusters");
  m_clustertree->Branch("m_clusenergy", &m_clusenergy, "m_clusenergy/D");
  m_clustertree->Branch("m_cluseta", &m_cluseta, "m_cluseta/D");
  m_clustertree->Branch("m_clustheta", &m_clustheta, "m_clustheta/D");
  m_clustertree->Branch("m_cluspt", &m_cluspt, "m_cluspt/D");
  m_clustertree->Branch("m_clusphi", &m_clusphi, "m_clusphi/D");
  m_clustertree->Branch("m_cluspx", &m_cluspx, "m_cluspx/D");
  m_clustertree->Branch("m_cluspy", &m_cluspy, "m_cluspy/D");
  m_clustertree->Branch("m_cluspz", &m_cluspz, "m_cluspz/D");
  m_clustertree->Branch("m_E_4x4", &m_E_4x4, "m_E_4x4/D");
  */
}

/**
 * This function initializes all of the member variables in this class so that there
 * are no variables that might not be set before e.g. writing them to the output
 * trees.
 */
void AnaUPC::initializeVariables()
{
  m_partid1 = -99;
  m_partid2 = -99;
  m_x1 = -99;
  m_x2 = -99;
  m_mpi = -99;
  m_process_id = -99;
  m_truthenergy = -99;
  m_trutheta = -99;
  m_truthphi = -99;
  m_truthp = -99;
  m_truthpx = -99;
  m_truthpy = -99;
  m_truthpz = -99;
  m_truthpt = -99;
  m_numparticlesinevent = -99;
  m_truthpid = -99;

  m_tr_px = -99;
  m_tr_py = -99;
  m_tr_pz = -99;
  m_tr_p = -99;
  m_tr_pt = -99;
  m_tr_phi = -99;
  m_tr_eta = -99;
  m_charge = -99;
  m_chisq = -99;
  m_ndf = -99;
  m_dca = -99;
  m_tr_x = -99;
  m_tr_y = -99;
  m_tr_z = -99;
  m_truth_is_primary = -99;
  m_truthtrackpx = -99;
  m_truthtrackpy = -99;
  m_truthtrackpz = -99;
  m_truthtrackp = -99;
  m_truthtracke = -99;
  m_truthtrackpt = -99;
  m_truthtrackphi = -99;
  m_truthtracketa = -99;
  m_truthtrackpid = -99;

  /*
  m_recojetpt = -99;
  m_recojetid = -99;
  m_recojetpx = -99;
  m_recojetpy = -99;
  m_recojetpz = -99;
  m_recojetphi = -99;
  m_recojetp = -99;
  m_recojetenergy = -99;
  m_recojeteta = -99;
  m_truthjetid = -99;
  m_truthjetp = -99;
  m_truthjetphi = -99;
  m_truthjeteta = -99;
  m_truthjetpt = -99;
  m_truthjetenergy = -99;
  m_truthjetpx = -99;
  m_truthjetpy = -99;
  m_truthjetpz = -99;
  m_dR = -99;
  */
}
