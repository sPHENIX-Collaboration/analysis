//general fun4all and subsysreco includes
#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//calorimeter includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>

#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>
#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

//evaluation includes
#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4eval/JetEvalStack.h>
#include <g4eval/SvtxEvalStack.h>

//hepmc includes
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

//general c++ includes
#include <TLorentzVector.h>
#include <cassert>
#include <iostream>
#include <sstream>

#include "PhotonJet.h"

using namespace std;

PhotonJet::PhotonJet(const std::string &name)
  : SubsysReco("PHOTONJET")
{
  //just set all global values to -999 from the start so that they have a spot in memory
  initialize_values();

  outfilename = name;

  //initialize public member values

  //default don't use isocone algorithm
  use_isocone = 0;

  //default do not use tracked jets (or tracks)
  eval_tracked_jets = 0;

  //default use 0.3 jet cone
  jet_cone_size = 3;

  //default set beginning number of events to 0
  //this can be changed with one of the public functions if e.g. you want individual event number IDs for multiple MC blocks of events
  nevents = 0;

  //default to barrel sPHENIX acceptance
  etalow = -1;
  etahigh = 1;

  //default jetminptcut
  minjetpt = 5.;

  //default mincluscut
  mincluspt = 0.5;

  //default minimum direct photon p_T
  mindp_pt = 10;

  //default to use the trigger emulator
  usetrigger = 1;

  //default to using position corrected emcal clusters
  use_pos_cor_cemc = 1;

  //default to not AA and not using embedded background subtraction
  is_AA = 0;
}

int PhotonJet::Init(PHCompositeNode *topnode)
{
  if (Verbosity() > 1)
  {
    cout << "COLLECTING PHOTON-JET PAIRS FOR THE FOLLOWING: " << endl;
    cout << "GATHERING JETS: " << jet_cone_size << endl;
    cout << "GATHERING IN ETA: [" << etalow
         << "," << etahigh << "]" << endl;
    cout << "EVALUATING TRACKED JETS: " << eval_tracked_jets << endl;
    cout << "USING ISOLATION CONE: " << use_isocone << endl;
  }

  //create output photonjet tfile which contains output TTrees
  file = new TFile(outfilename.c_str(), "RECREATE");

  //create some basic histograms
  ntruthconstituents_h = new TH1F("ntruthconstituents", "", 200, 0, 200);
  percent_photon_h = new TH1F("percent_photon_h",
                              ";E_{photon}/E_{jet}; Counts", 200, 0, 2);
  histo = new TH1F("histo", "histo", 100, -3, 3);

  //create basic tree
  tree = new TTree("tree", "a tree");
  tree->Branch("nevents", &nevents, "nevents/I");

  //main trees are set in this fxn - branch addresses are defined to global data types
  Set_Tree_Branches();

  return 0;
}

int PhotonJet::process_event(PHCompositeNode *topnode)
{
  // event number tracker,
  if (nevents % 10 == 0)
    cout << "at event number " << nevents << endl;

  //get the requested size jets
  ostringstream truthjetsize;
  ostringstream recojetsize;
  ostringstream trackjetsize;

  //these are the node names for Truth, Calorimeter tower, and tracked jets
  truthjetsize.str("");
  truthjetsize << "AntiKt_Truth_r";
  recojetsize.str("");
  recojetsize << "AntiKt_Tower_r";
  trackjetsize.str("");
  trackjetsize << "AntiKt_Track_r";

  if (jet_cone_size == 2)
  {
    truthjetsize << "02";
    recojetsize << "02";
    trackjetsize << "02";
  }
  else if (jet_cone_size == 3)
  {
    truthjetsize << "03";
    recojetsize << "03";
    trackjetsize << "03";
  }
  else if (jet_cone_size == 4)
  {
    truthjetsize << "04";
    recojetsize << "04";
    trackjetsize << "04";
  }
  else if (jet_cone_size == 5)
  {
    truthjetsize << "05";
    recojetsize << "05";
    trackjetsize << "05";
  }
  else if (jet_cone_size == 6)
  {
    truthjetsize << "06";
    recojetsize << "06";
    trackjetsize << "06";
  }

  else if (jet_cone_size == 7)
  {
    truthjetsize << "07";
    recojetsize << "07";
    trackjetsize << "07";
  }
  //if its some other number just set it to 0.4

  else
  {
    truthjetsize << "04";
    recojetsize << "04";
  }

  if (Verbosity() > 1)
  {
    cout << "Gathering RECO Jets:  " << recojetsize.str().c_str() << endl;
    cout << "Gathering TRUTH Jets:  " << truthjetsize.str().c_str() << endl;
  }

  //get the nodes from the NodeTree

  //JetMap nodes
  JetMap *truth_jets =
      findNode::getClass<JetMap>(topnode, truthjetsize.str().c_str());

  JetMap *reco_jets = 0;
  if (!is_AA)
  {
    reco_jets = findNode::getClass<JetMap>(topnode, recojetsize.str().c_str());
  }

  JetMap *tracked_jets =
      findNode::getClass<JetMap>(topnode, trackjetsize.str().c_str());

  //G4 truth particle node
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode, "G4TruthInfo");

  //EMCal raw tower cluster node
  RawClusterContainer *clusters = 0;
  if (!use_pos_cor_cemc)
    clusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_CEMC");

  //EMCal position calibrated cluster node
  if (use_pos_cor_cemc)
    clusters = findNode::getClass<RawClusterContainer>(topnode, "CLUSTER_POS_COR_CEMC");

  //SVTX tracks node
  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topnode, "SvtxTrackMap");

  //trigger emulator
  CaloTriggerInfo *trigger = findNode::getClass<CaloTriggerInfo>(topnode, "CaloTriggerInfo");

  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  //for truth track matching
  SvtxEvalStack *svtxevalstack = new SvtxEvalStack(topnode);
  svtxevalstack->next_event(topnode);

  if (is_AA)
  {
    recojetsize << "_Sub1";
    reco_jets =
        findNode::getClass<JetMap>(topnode, recojetsize.str().c_str());
  }

  //for truth jet matching
  JetEvalStack *_jetevalstack = 0;
  if (!is_AA)
    _jetevalstack =
        new JetEvalStack(topnode, recojetsize.str().c_str(),
                         truthjetsize.str().c_str());
  //the jet eval stack doesn't work for AA - so this is useless
  //in that case the code is setup to match reco and truth jets based on
  //their difference in deltaphi and deltaeta
  else
    _jetevalstack = new JetEvalStack(topnode,
                                     "AntiKt_Tower_r04_Sub1",
                                     "AntiKt_Truth_r04");

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topnode, "GlobalVertexMap");
  if (!vertexmap)
  {
    cout << "PhotonJet::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << endl;
    assert(vertexmap);  // force quit

    return 0;
  }

  if (vertexmap->empty())
  {
    cout << "PhotonJet::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << endl;
    return 0;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  if (vtx == nullptr) return 0;


  RawTowerContainer *_cemctowers = findNode::getClass<RawTowerContainer>(topnode,"TOWER_CALIB_CEMC");
  RawTowerContainer *_hcalintowers = findNode::getClass<RawTowerContainer>(topnode,"TOWER_CALIB_HCALIN");
  RawTowerContainer *_hcalouttowers = findNode::getClass<RawTowerContainer>(topnode,"TOWER_CALIB_HCALOUT");
  RawTowerGeomContainer *_cemctowergeom = findNode::getClass<RawTowerGeomContainer>(topnode,"TOWERGEOM_CEMC");
  RawTowerGeomContainer *_hcalintowergeom = findNode::getClass<RawTowerGeomContainer>(topnode,"TOWERGEOM_HCALIN");
  RawTowerGeomContainer *_hcalouttowergeom = findNode::getClass<RawTowerGeomContainer>(topnode,"TOWERGEOM_HCALOUT");









  //Make sure all nodes for analysis are here. If one isn't in the NodeTree, bail
  if (!trigger && usetrigger)
  {
    cout << "NO TRIGGER EMULATOR, BAILING" << endl;
    return 0;
  }
  if (!tracked_jets && eval_tracked_jets)
  {
    cout << "no tracked jets, bailing" << endl;
    return 0;
  }
  if (!truth_jets)
  {
    cout << "no truth jets" << endl;
    return 0;
  }
  if (!reco_jets)
  {
    cout << "no reco jets" << endl;
    return 0;
  }
  if (!truthinfo)
  {
    cout << "no truth track info" << endl;
    return 0;
  }
  if (!clusters)
  {
    cout << "no cluster info" << endl;
    return 0;
  }
  if (!trackmap && eval_tracked_jets)
  {
    cout << "no track map" << endl;
    return 0;
  }
  if (!_jetevalstack)
  {
    cout << "no good truth jets" << endl;
    return 0;
  }

  //For jet and track truth/reco matching
  JetRecoEval *recoeval = _jetevalstack->get_reco_eval();
  SvtxTrackEval *trackeval = svtxevalstack->get_track_eval();
  JetTruthEval *trutheval = _jetevalstack->get_truth_eval();



  //now we have all the nodes, so collect the data from the various nodes

  /***********************************************

  GET ALL THE HEPMC EVENT LEVEL TRUTH PARTICLES

  ************************************************/

  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topnode, "PHHepMCGenEventMap");

  if (!hepmceventmap)
  {
    cout << "hepmc event map node is missing, BAILING" << endl;
    //return 0;
  }

  if (Verbosity() > 1)
  {
    cout << "Getting HEPMC truth particles " << endl;
  }

  //you can iterate over the number of events in a hepmc event
  //for pile up events where you have multiple hard scatterings per bunch crossing
  for (PHHepMCGenEventMap::ConstIter iterr = hepmceventmap->begin();
       iterr != hepmceventmap->end();
       ++iterr)
  {
    PHHepMCGenEvent *hepmcevent = iterr->second;

    if (hepmcevent)
    {
      //get the event characteristics, inherited from HepMC classes
      HepMC::GenEvent *truthevent = hepmcevent->getEvent();
      if (!truthevent)
      {
        cout << PHWHERE << "no evt pointer under phhepmvgeneventmap found " << endl;
        return 0;
      }

      //get the interacting protons
      if (truthevent->valid_beam_particles())
      {
        HepMC::GenParticle *part1 = truthevent->beam_particles().first;

        //beam energy
        beam_energy = fabs(part1->momentum().pz());
      }

      //Parton info
      HepMC::PdfInfo *pdfinfo = truthevent->pdf_info();

      partid1 = pdfinfo->id1();
      partid2 = pdfinfo->id2();
      x1 = pdfinfo->x1();
      x2 = pdfinfo->x2();

      //are there multiple partonic intercations in a p+p event
      mpi = truthevent->mpi();

      numparticlesinevent = 0;

      //Get the PYTHIA signal process id identifying the 2-to-2 hard process
      process_id = truthevent->signal_process_id();

      //loop over all the truth particles and get their information
      for (HepMC::GenEvent::particle_const_iterator iter = truthevent->particles_begin();
           iter != truthevent->particles_end();
           ++iter)
      {
        //get each pythia particle characteristics
        truthenergy = (*iter)->momentum().e();
        truthpid = (*iter)->pdg_id();

        trutheta = (*iter)->momentum().pseudoRapidity();
        truthphi = (*iter)->momentum().phi();
        truthpx = (*iter)->momentum().px();
        truthpy = (*iter)->momentum().py();
        truthpz = (*iter)->momentum().pz();
        truthpt = sqrt(truthpx * truthpx + truthpy * truthpy);

        //fill the truth tree
        truthtree->Fill();
        numparticlesinevent++;
      }
    }
  }

  //these are global variables, i.e. the highest pt cluster in an event
  cluseventenergy = 0;
  cluseventphi = 0;
  cluseventpt = 0;
  cluseventeta = 0;
  float lastenergy = 0;

  if (Verbosity() > 1)
  {
    cout << "get G4 stable truth particles" << endl;
  }

  //loop over the G4 truth (stable) particles
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second;
       ++iter)
  {
    //get this particle
    PHG4Particle *truth = iter->second;

    //get this particles momentum, etc.
    truthpx = truth->get_px();
    truthpy = truth->get_py();
    truthpz = truth->get_pz();
    truthp = sqrt(truthpx * truthpx + truthpy * truthpy + truthpz * truthpz);
    truthenergy = truth->get_e();

    TLorentzVector vec;
    vec.SetPxPyPzE(truthpx, truthpy, truthpz, truthenergy);

    truthpt = sqrt(truthpx * truthpx + truthpy * truthpy);

    truthphi = vec.Phi();

    trutheta = TMath::ATanH(truthpz / truthenergy);
    //check for nans
    if (trutheta != trutheta)
      trutheta = -9999;
    truthpid = truth->get_pid();

    //find the highest energy photon in the event in the eta range
    if (truthpid == 22 && truthenergy > lastenergy 
	&& trutheta > etalow && trutheta < etahigh)
    {
      lastenergy = truthenergy;
      cluseventenergy = truthenergy;
      cluseventpt = truthpt;
      cluseventphi = truthphi;
      cluseventeta = trutheta;
    }
    //fill the g4 truth tree
    truth_g4particles->Fill();
  }

  /***************************************

   TRUTH JETS

   ***************************************/
  if (Verbosity() > 1)
  {
    cout << "get the truth jets" << endl;
  }

  //loop over the truth jets
  float hardest_jet = 0;
  // If not truth jet is found that satisfies the requirements,
  // then the jet 4 vector will be (0,0,0,0)
  hardest_jetpt = 0;
  hardest_jetenergy = 0;
  hardest_jeteta = 0;
  hardest_jetphi = 0;
  for (JetMap::Iter iter = truth_jets->begin();
       iter != truth_jets->end();
       ++iter)
  {
    Jet *jet = iter->second;

    truthjetpt = jet->get_pt();

    //only collect truthjets above the minjetpt cut
    if (truthjetpt < minjetpt)
      continue;

    truthjeteta = jet->get_eta();

    //make the width extra just to be safe and collect truth jets
    //that might be e.g. half in the sphenix acceptance
    if (truthjeteta < (etalow - 1) || truthjeteta > (etahigh + 1))
      continue;

    truthjetpx = jet->get_px();
    truthjetpy = jet->get_py();
    truthjetpz = jet->get_pz();
    truthjetphi = jet->get_phi();
    truthjetmass = jet->get_mass();
    truthjetp = jet->get_p();
    truthjetenergy = jet->get_e();

    //check that the jet isn't just a high pt photon

    //get the truth constituents of the jet
    std::set<PHG4Particle *> truthjetcomp =
        trutheval->all_truth_particles(jet);
    ntruthconstituents = 0;
    float truthjetenergysum = 0;
    float truthjethighestphoton = 0;
    //loop over the constituents of the truth jet
    for (std::set<PHG4Particle *>::iterator iter2 = truthjetcomp.begin();
         iter2 != truthjetcomp.end();
         ++iter2)
    {
      //get the particle of the truthjet
      PHG4Particle *truthpart = *iter2;
      if (!truthpart)
      {
        cout << "no truth particles in the jet??" << endl;
        break;
      }

      ntruthconstituents++;

      int constpid = truthpart->get_pid();
      float conste = truthpart->get_e();

      truthjetenergysum += conste;

      if (constpid == 22)
      {
        if (conste > truthjethighestphoton)
          truthjethighestphoton = conste;
      }
    }
    ntruthconstituents_h->Fill(ntruthconstituents);

    //we want jets that are real jets, not just an e.g. isolated photon
    //require that the number of constituents in the jet be larger than 3
    float percent_photon = truthjethighestphoton / truthjetenergy;
    percent_photon_h->Fill(percent_photon);

    //if there is a high energy photon that is 80% of the jets energy, skip it
    //it is likely the near-side direct photon
    if (percent_photon > 0.8)
    {
      continue;
    }

    //we also want jets to have at least 3 constituents
    if (ntruthconstituents < 3)
      continue;

    //identify the highest pt jet in the event
    if (truthjetpt > hardest_jet)
    {
      hardest_jet = truthjetpt;
      hardest_jetpt = truthjetpt;
      hardest_jetenergy = truthjetenergy;
      hardest_jeteta = truthjeteta;
      hardest_jetphi = truthjetphi;
    }

    //fill the truthjet tree
    truthjettree->Fill();
  }

  //fill the event tree with e.g. x1,x2 partonic momentum fractions,
  //process id, highest energy photon, etc.
  event_tree->Fill();

  if (Verbosity() > 1)
  {
    cout << "get trigger emulator info" << endl;
  }
  /***********************************************

   TRIGGER EMULATOR INFO

  ************************************************/
  if (usetrigger)
  {
    //get the 4x4 trigger tile info
    E_4x4 = trigger->get_best_EMCal_4x4_E();
    phi_4x4 = trigger->get_best_EMCal_4x4_phi();
    eta_4x4 = trigger->get_best_EMCal_4x4_eta();

    //get the 2x2 trigger tile info
    E_2x2 = trigger->get_best_EMCal_2x2_E();
    phi_2x2 = trigger->get_best_EMCal_2x2_phi();
    eta_2x2 = trigger->get_best_EMCal_2x2_eta();
  }

  /***********************************************

  GET THE EMCAL CLUSTERS

  ************************************************/
  if (Verbosity() > 1)
  {
    cout << "Get EMCal Cluster" << endl;
  }

  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator clusiter;

  //loop over the emcal clusters
  for (clusiter = begin_end.first;
       clusiter != begin_end.second;
       ++clusiter)
  {
    //get this cluster
    RawCluster *cluster = clusiter->second;

    //get cluster characteristics
    //this helper class determines the photon characteristics
    //depending on the vertex position
    //this is important for e.g. eta determination and E_T determination
    CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);
    clus_energy = E_vec_cluster.mag();
    clus_eta = E_vec_cluster.pseudoRapidity();
    clus_theta = E_vec_cluster.getTheta();
    clus_pt = E_vec_cluster.perp();
    clus_phi = E_vec_cluster.getPhi();

    if (clus_pt < mincluspt)
      continue;

    if (clus_eta < etalow)
      continue;
    if (clus_eta > etahigh)
      continue;

    TLorentzVector *clus = new TLorentzVector();
    clus->SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clus_energy);

    float dumarray[4];
    clus->GetXYZT(dumarray);
    clus_x = dumarray[0];
    clus_y = dumarray[1];
    clus_z = dumarray[2];
    clus_t = dumarray[3];

    clus_px = clus_pt * TMath::Cos(clus_phi);
    clus_py = clus_pt * TMath::Sin(clus_phi);
    clus_pz = sqrt(clus_energy * clus_energy - clus_px * clus_px - clus_py * clus_py);

    //fill the cluster tree with all emcal clusters
    cluster_tree->Fill();

    //now determine the direct photon
    //only interested in high pt photons to be isolated i.e. direct photons
    if (clus_pt < mindp_pt)
      continue;
    //require that the entire isolation cone fall within sPHENIX acceptance
    if (fabs(clus_eta) > (1.0 - isoconeradius) && use_isocone)
      continue;

    if (use_isocone)
    {
      //get the energy sum in the cone surrounding the photon
      float energysum = ConeSum(cluster, clusters, trackmap, isoconeradius, vtx);

      //check if energy sum is less than 10% of isolated photon energy
      bool conecut = energysum > 0.1 * clus_energy;
      if (conecut)
        continue;
    }

    //find the associated truth high pT photon with this reconstructed photon
    for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
         iter != range.second;
         ++iter)
    {
      //get this truth particle
      PHG4Particle *truth = iter->second;

      clustruthpid = truth->get_pid();

      //check that it is a photon, i.e. pid==22
      if (clustruthpid == 22)
      {
        clustruthpx = truth->get_px();
        clustruthpy = truth->get_py();
        clustruthpz = truth->get_pz();
        clustruthenergy = truth->get_e();
        clustruthpt = sqrt(clustruthpx * clustruthpx + clustruthpy * clustruthpy);
        if (clustruthpt < mindp_pt)
          continue;

        TLorentzVector vec;
        vec.SetPxPyPzE(clustruthpx, clustruthpy, clustruthpz, clustruthenergy);
        clustruthphi = vec.Phi();

        clustrutheta = TMath::ATanH(clustruthpz / clustruthenergy);
        //check for nans
        if (clustrutheta != clustrutheta)
          clustrutheta = -9999;

        //check that the truth photon has similar eta/phi to reco photon
        //the clustering has a resolution of about 0.005 rads so this is sufficient
        if (fabs(clustruthphi - clus_phi) > 0.02 || fabs(clustrutheta - clus_eta) > 0.02)
          continue;

        //once the photon is found and the values are set
        //just break out of the loop
        break;
      }
    }
    //fill isolated clusters tree, i.e. all isolated clusters regardless
    //of if an away-side jet is found also
    isolated_clusters->Fill();

    //get back to back reconstructed hadrons/jets for photon-jet processes
    //two different functions for Au+Au vs p+p due to the way truth jet matching
    //is performed between the two systems
    if (!is_AA)
      GetRecoHadronsAndJets(cluster, trackmap,
                            reco_jets, tracked_jets,
                            recoeval, trackeval,
                            truthinfo,
                            trutheval,
                            truth_jets,
                            vtx);

    if (is_AA)
      GetRecoHadronsAndJetsAA(cluster,
                              trackmap,
                              reco_jets,
                              truthinfo,
                              truth_jets,
                              vtx);
  }

  /***********************************************

  GET THE SVTX TRACKS

  ************************************************/
  if (eval_tracked_jets)
  {
    if (Verbosity() > 1)
    {
      cout << "Get the Tracks" << endl;
    }
    for (SvtxTrackMap::Iter iter = trackmap->begin();
         iter != trackmap->end();
         ++iter)
    {
      SvtxTrack *track = iter->second;

      //get reco info
      tr_px = track->get_px();
      tr_py = track->get_py();
      tr_pz = track->get_pz();
      tr_p = sqrt(tr_px * tr_px + tr_py * tr_py + tr_pz * tr_pz);

      tr_pt = sqrt(tr_px * tr_px + tr_py * tr_py);

      if (tr_pt < 0.5)
        continue;

      tr_phi = track->get_phi();
      tr_eta = track->get_eta();

      if (tr_eta < etalow || tr_eta > etahigh)
        continue;

      charge = track->get_charge();
      chisq = track->get_chisq();
      ndf = track->get_ndf();
      dca = track->get_dca();
      tr_x = track->get_x();
      tr_y = track->get_y();
      tr_z = track->get_z();

      //get truth track info
      PHG4Particle *truthtrack = trackeval->max_truth_particle_by_nclusters(track);
      truth_is_primary = truthinfo->is_primary(truthtrack);

      truthtrackpx = truthtrack->get_px();
      truthtrackpy = truthtrack->get_py();
      truthtrackpz = truthtrack->get_pz();
      truthtrackp = sqrt(truthtrackpx * truthtrackpx + truthtrackpy * truthtrackpy + truthtrackpz * truthtrackpz);

      truthtracke = truthtrack->get_e();

      TLorentzVector *Truthtrack = new TLorentzVector();
      Truthtrack->SetPxPyPzE(truthtrackpx, truthtrackpy, truthtrackpz, truthtracke);

      truthtrackpt = Truthtrack->Pt();
      truthtrackphi = Truthtrack->Phi();
      truthtracketa = Truthtrack->Eta();
      truthtrackpid = truthtrack->get_pid();

      tracktree->Fill();
    }
  }
  
  /***************************************

   RECONSTRUCTED JETS

   ***************************************/
  if (Verbosity() > 1)
  {
    cout << "Get all Reco Jets" << endl;
  }

  for (JetMap::Iter iter = reco_jets->begin();
       iter != reco_jets->end();
       ++iter)
  {
    Jet *jet = iter->second;
    Jet *truthjet = recoeval->max_truth_jet_by_energy(jet);
    recojetpt = jet->get_pt();
    if (recojetpt < minjetpt)
      continue;

    recojeteta = jet->get_eta();

    //reco jet eta better not be outside of the sPHENIX acceptance....
    if (recojeteta < (etalow - 1) || recojeteta > (etahigh + 1))
      continue;

    //get reco jet characteristics
    recojetid = jet->get_id();
    recojetpx = jet->get_px();
    recojetpy = jet->get_py();
    recojetpz = jet->get_pz();
    recojetphi = jet->get_phi();
    recojetmass = jet->get_mass();
    recojetp = jet->get_p();
    recojetenergy = jet->get_e();

    //if truthjet exists, then it is p+p and we can use the stackeval
    //for truthjet matching
    if (truthjet)
    {
      truthjetid = truthjet->get_id();
      truthjetp = truthjet->get_p();
      truthjetphi = truthjet->get_phi();
      truthjeteta = truthjet->get_eta();
      truthjetpt = truthjet->get_pt();
      truthjetenergy = truthjet->get_e();
      truthjetpx = truthjet->get_px();
      truthjetpy = truthjet->get_py();
      truthjetpz = truthjet->get_pz();
      truthjetmass = truthjet->get_mass();

      //check that the jet isn't just a photon or something like this
      //needs at least 2 constituents and that 80% of jet isn't from one photon
      std::set<PHG4Particle *> truthjetcomp =
          trutheval->all_truth_particles(truthjet);
      ntruthconstituents = 0;
      float truthjetenergysum = 0;
      float truthjethighestphoton = 0;
      for (std::set<PHG4Particle *>::iterator iter2 = truthjetcomp.begin();
           iter2 != truthjetcomp.end();
           ++iter2)
      {
        PHG4Particle *truthpart = *iter2;
        if (!truthpart)
        {
          cout << "no truth particles in the jet??" << endl;
          break;
        }
        ntruthconstituents++;

        int constpid = truthpart->get_pid();
        float conste = truthpart->get_e();

        truthjetenergysum += conste;

        if (constpid == 22)
        {
          if (conste > truthjethighestphoton)
            truthjethighestphoton = conste;
	}	
      }

      //if the highest energy photon in the jet is 80% of the jets energy
      //its probably an isolated photon and so we want to not include it in the tree
      float percent_photon = truthjethighestphoton / truthjetenergy;
      if (percent_photon > 0.8)
      {
        continue;
      }
      if (!is_AA && ntruthconstituents < 3)
        continue;
    }

    //if truthjet was null then we just match the reco-truth jets by
    //their distance in dphi deta space
    else
    {
      if (Verbosity() > 1)
      {
        cout << "matching by distance jet" << endl;
      }

      truthjetid = 0;
      truthjetp = 0;
      truthjetphi = 0;
      truthjeteta = 0;
      truthjetpt = 0;
      truthjetenergy = 0;
      truthjetpx = 0;
      truthjetpy = 0;
      truthjetpz = 0;

      //if the jetevalstack can't find a good truth jet
      //try to match reco jet with closest truth jet
      float closestjet = 9999;
      for (JetMap::Iter iter3 = truth_jets->begin();
           iter3 != truth_jets->end();
           ++iter3)
      {
        Jet *jet = iter3->second;

        float thisjetpt = jet->get_pt();
        if (thisjetpt < minjetpt)
          continue;

        float thisjeteta = jet->get_eta();
        float thisjetphi = jet->get_phi();

        float dphi = recojetphi - thisjetphi;
        if (dphi > 3. * pi / 2.)
          dphi -= pi * 2.;
        if (dphi < -1. * pi / 2.)
          dphi += pi * 2.;

        float deta = recojeteta - thisjeteta;

        dR = sqrt(pow(dphi, 2.) + pow(deta, 2.));

        if (dR < reco_jets->get_par() && dR < closestjet)
        {
          truthjetid = -9999;  //indicates matched with distance, not jetevalstack
          truthjetp = jet->get_p();
          truthjetphi = jet->get_phi();
          truthjeteta = jet->get_eta();
          truthjetpt = jet->get_pt();
          truthjetenergy = jet->get_e();
          truthjetpx = jet->get_px();
          truthjetpy = jet->get_py();
          truthjetpz = jet->get_pz();
          truthjetmass = jet->get_mass();
        }

        if (dR < closestjet)
        {
          closestjet = dR;
        }
      }
    }

    //get the reco jet constituents and calculate the dphi deta 
    //from the jet axis. Added to understand the truth jet - reco jet
    //azimuthal offset

  
    
    for(Jet::ConstIter constiter = jet->begin_comp();
	constiter != jet->end_comp();
	++constiter)
      {
	Jet::SRC source = constiter->first;
	unsigned int index = constiter->second;
	
	RawTower *thetower = 0;
	if(source == Jet::CEMC_TOWER)
	  {
	    thetower = _cemctowers->getTower(index);
	  }
	else if(source == Jet::HCALIN_TOWER)
	  {
	    thetower = _hcalintowers->getTower(index);
	  }
	else if(source == Jet::HCALOUT_TOWER)
	  {
	    thetower = _hcalouttowers->getTower(index);
	  }
	
	assert(thetower);

	int tower_phi_bin = thetower->get_binphi();
	int tower_eta_bin = thetower->get_bineta();
	
	double constphi = -9999;
	double consteta = -9999;
	if(source == Jet::CEMC_TOWER)
	  {
	    constphi = _cemctowergeom->get_phicenter(tower_phi_bin);
	    consteta = _cemctowergeom->get_etacenter(tower_eta_bin);
	  }
	else if(source == Jet::HCALIN_TOWER)
	  {
	    constphi = _hcalintowergeom->get_phicenter(tower_phi_bin);
	    consteta = _hcalintowergeom->get_etacenter(tower_eta_bin);
	  }
	else if(source == Jet::HCALOUT_TOWER)
	  {
	    constphi = _hcalouttowergeom->get_phicenter(tower_phi_bin);
	    consteta = _hcalouttowergeom->get_etacenter(tower_eta_bin);
	  }
	float checkdphi = truthjetphi - constphi;
	if(checkdphi < -1 * TMath::Pi() / 2.)
	  checkdphi += 2. * TMath::Pi();
	if(checkdphi > 3. * TMath::Pi() / 2.)
	  checkdphi -= 2. * TMath::Pi();
	  
	constituent_dphis.push_back(checkdphi);
	constituent_detas.push_back(truthjeteta - consteta);

      }

    recojettree->Fill();

    constituent_dphis.resize(0);
    constituent_detas.resize(0);

  }

  if (Verbosity() > 1)
  {
    cout << "finished event" << endl;
  }

  nevents++;
  tree->Fill();
  return 0;
}

int PhotonJet::End(PHCompositeNode *topnode)
{
  std::cout << " DONE PROCESSING PHOTONJET PACKAGE" << endl;

  file->Write();
  file->Close();
  return 0;
}
void PhotonJet::GetRecoHadronsAndJetsAA(RawCluster *trig,
                                        SvtxTrackMap *tracks,
                                        JetMap *recojets,
                                        PHG4TruthInfoContainer *alltruth,
                                        JetMap *truthjets,
                                        GlobalVertex *vtx)
{
  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*trig, vertex);

  float trig_phi = E_vec_cluster.getPhi();
  float trig_eta = E_vec_cluster.pseudoRapidity();

  PHG4TruthInfoContainer::Range range = alltruth->GetPrimaryParticleRange();

  //find the matching truth photon to the reco photon
  //for the values in the photon-jet tree
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second;
       ++iter)
  {
    PHG4Particle *truth = iter->second;

    clustruthpid = truth->get_pid();
    if (clustruthpid == 22)
    {
      clustruthpx = truth->get_px();
      clustruthpy = truth->get_py();
      clustruthpz = truth->get_pz();
      clustruthenergy = truth->get_e();
      clustruthpt = sqrt(clustruthpx * clustruthpx + clustruthpy * clustruthpy);
      if (clustruthpt < mincluspt)
        continue;

      TLorentzVector vec;
      vec.SetPxPyPzE(clustruthpx, clustruthpy, clustruthpz, clustruthenergy);
      clustruthphi = vec.Phi();

      clustrutheta = TMath::ATanH(clustruthpz / clustruthenergy);
      //check for nans
      if (clustrutheta != clustrutheta)
        clustrutheta = -9999;

      if (fabs(clustruthphi - trig_phi) > 0.02 || fabs(clustrutheta - trig_eta) > 0.02)
        continue;

      //once the values are set, we've found the truth photon so just break out of this loop
      break;
    }
  }

  //find the away-side jets from the direct photon
  for (JetMap::Iter iter = recojets->begin();
       iter != recojets->end();
       ++iter)
  {
    Jet *jet = iter->second;

    _recojetpt = jet->get_pt();
    if (_recojetpt < minjetpt)
      continue;

    _recojeteta = jet->get_eta();
    if (_recojeteta < etalow || _recojeteta > etahigh)
      continue;

    _recojetpx = jet->get_px();
    _recojetpy = jet->get_py();
    _recojetpz = jet->get_pz();
    _recojetphi = jet->get_phi();
    _recojetmass = jet->get_mass();
    _recojetp = jet->get_p();
    _recojetenergy = jet->get_e();
    _recojetid = jet->get_id();

    _truthjetid = 0;
    _truthjetp = 0;
    _truthjetphi = 0;
    _truthjeteta = 0;
    _truthjetpt = 0;
    _truthjetenergy = 0;
    _truthjetpx = 0;
    _truthjetpy = 0;
    _truthjetpz = 0;

    //try to match reco jet with closest truth jet
    //this is A+A so we know the jet eval stack doesn't work
    //so just match the truth-reco jet pair by distance
    float closestjet = 9999;
    for (JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter)
    {
      Jet *truthjet = iter->second;

      float thisjetpt = truthjet->get_pt();
      if (thisjetpt < minjetpt)
        continue;

      float thisjeteta = truthjet->get_eta();
      float thisjetphi = truthjet->get_phi();

      float dphi = recojetphi - thisjetphi;
      if (dphi > 3. * pi / 2.)
        dphi -= pi * 2.;
      if (dphi < -1. * pi / 2.)
        dphi += pi * 2.;

      float deta = recojeteta - thisjeteta;

      float dR = sqrt(pow(dphi, 2.) + pow(deta, 2.));

      if (dR < recojets->get_par() && dR < closestjet)
      {
        _truthjetid = -9999;  //indicates matched with distance, not evalstack
        _truthjetp = truthjet->get_p();
        _truthjetphi = truthjet->get_phi();
        _truthjeteta = truthjet->get_eta();
        _truthjetpt = truthjet->get_pt();
        _truthjetenergy = truthjet->get_e();
        _truthjetpx = truthjet->get_px();
        _truthjetpy = truthjet->get_py();
        _truthjetpz = truthjet->get_pz();
        _truthjetmass = truthjet->get_mass();
      }

      if (dR < closestjet)
        closestjet = dR;
    }

    jetdphi = trig_phi - _recojetphi;
    if (jetdphi < pi2)
      jetdphi += 2. * pi;
    if (jetdphi > threepi2)
      jetdphi -= 2. * pi;

    if (fabs(jetdphi) < 0.05)
      //don't care about matching the reco photon with itself
      continue;

    jetdeta = trig_eta - _recojeteta;
    jetpout = _recojetpt * TMath::Sin(jetdphi);

    isophot_jet_tree->Fill();
  }
}
void PhotonJet::GetRecoHadronsAndJets(RawCluster *trig,
                                      SvtxTrackMap *tracks,
                                      JetMap *jets,
                                      JetMap *trackedjets,
                                      JetRecoEval *recoeval,
                                      SvtxTrackEval *trackeval,
                                      PHG4TruthInfoContainer *alltruth,
                                      JetTruthEval *trutheval,
                                      JetMap *truthjets,
                                      GlobalVertex *vtx)
{
  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*trig, vertex);

  float trig_phi = E_vec_cluster.getPhi();
  float trig_eta = E_vec_cluster.pseudoRapidity();

  PHG4TruthInfoContainer::Range range = alltruth->GetPrimaryParticleRange();

  //Match the reco photon with the truth photon
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second;
       ++iter)
  {
    PHG4Particle *truth = iter->second;

    clustruthpid = truth->get_pid();
    if (clustruthpid == 22)
    {
      clustruthpx = truth->get_px();
      clustruthpy = truth->get_py();
      clustruthpz = truth->get_pz();
      clustruthenergy = truth->get_e();
      clustruthpt = sqrt(clustruthpx * clustruthpx + clustruthpy * clustruthpy);
      if (clustruthpt < mincluspt)
        continue;

      TLorentzVector vec;
      vec.SetPxPyPzE(clustruthpx, clustruthpy, clustruthpz, clustruthenergy);
      clustruthphi = vec.Phi();

      clustrutheta = TMath::ATanH(clustruthpz / clustruthenergy);
      //check for nans
      if (clustrutheta != clustrutheta)
        clustrutheta = -9999;

      if (fabs(clustruthphi - trig_phi) > 0.02 || fabs(clustrutheta - trig_eta) > 0.02)
        continue;

      //once the values are set, we've found the truth photon so just break out of this loop
      break;
    }
  }

  if (eval_tracked_jets)
  {
    if (Verbosity() > 1)
    {
      cout << "evaluating tracked hadrons opposite the direct photon" << endl;
    }
    for (SvtxTrackMap::Iter iter = tracks->begin();
         iter != tracks->end();
         ++iter)
    {
      SvtxTrack *track = iter->second;

      _tr_px = track->get_px();
      _tr_py = track->get_py();
      _tr_pz = track->get_pz();
      _tr_pt = sqrt(_tr_px * _tr_px + _tr_py * _tr_py);
      if (_tr_pt < 0.5)
        continue;

      _tr_p = sqrt(_tr_px * _tr_px + _tr_py * _tr_py + _tr_pz * _tr_pz);
      _tr_phi = track->get_phi();
      _tr_eta = track->get_eta();
      _charge = track->get_charge();
      _chisq = track->get_chisq();
      //can find appropriate values to cut on for these later
      _ndf = track->get_ndf();
      _dca = track->get_dca();

      _tr_x = track->get_x();
      _tr_y = track->get_y();
      _tr_z = track->get_z();

      haddphi = trig_phi - _tr_phi;
      if (haddphi < pi2)
        haddphi += 2. * pi;
      if (haddphi > threepi2)
        haddphi -= 2. * pi;

      //get the truth track info
      PHG4Particle *truthtrack = trackeval->max_truth_particle_by_nclusters(track);
      _truth_is_primary = alltruth->is_primary(truthtrack);

      _truthtrackpx = truthtrack->get_px();
      _truthtrackpy = truthtrack->get_py();
      _truthtrackpz = truthtrack->get_pz();
      _truthtrackp = sqrt(truthtrackpx * truthtrackpx + truthtrackpy * truthtrackpy + truthtrackpz * truthtrackpz);

      _truthtracke = truthtrack->get_e();
      TLorentzVector *Truthtrack = new TLorentzVector();
      Truthtrack->SetPxPyPzE(truthtrackpx, truthtrackpy, truthtrackpz, truthtracke);
      _truthtrackpt = Truthtrack->Pt();
      _truthtrackphi = Truthtrack->Phi();
      _truthtracketa = Truthtrack->Eta();
      _truthtrackpid = truthtrack->get_pid();

      haddeta = trig_eta - _tr_eta;

      hadpout = _tr_pt * TMath::Sin(haddphi);

      isophot_had_tree->Fill();
    }

    //now collect the away-sidet tracked jets from the direct photon
    for (JetMap::Iter iter = trackedjets->begin();
         iter != trackedjets->end();
         ++iter)
    {
      Jet *jet = iter->second;
      Jet *truthjet = recoeval->max_truth_jet_by_energy(jet);

      _trecojetpt = jet->get_pt();

      if (_trecojetpt < minjetpt)
        continue;

      _trecojeteta = jet->get_eta();
      if (fabs(_trecojeteta) > 1.)
        continue;

      _trecojetpx = jet->get_px();
      _trecojetpy = jet->get_py();
      _trecojetpz = jet->get_pz();
      _trecojetphi = jet->get_phi();
      _trecojetmass = jet->get_mass();
      _trecojetp = jet->get_p();
      _trecojetenergy = jet->get_e();
      _trecojetid = jet->get_id();

      if (truthjet)
      {
        _ttruthjetid = truthjet->get_id();
        _ttruthjetp = truthjet->get_p();
        _ttruthjetphi = truthjet->get_phi();
        _ttruthjeteta = truthjet->get_eta();
        _ttruthjetpt = truthjet->get_pt();
        _ttruthjetenergy = truthjet->get_e();
        _ttruthjetpx = truthjet->get_px();
        _ttruthjetpy = truthjet->get_py();
        _ttruthjetpz = truthjet->get_pz();
      }
      //for some reason jeteval stack doesn't have matched jets, just set them to -999
      else
      {
        _ttruthjetid = -9999;
        _ttruthjetp = -9999;
        _ttruthjetphi = -9999;
        _ttruthjeteta = -9999;
        _ttruthjetpt = -9999;
        _ttruthjetenergy = -9999;
        _ttruthjetpx = -9999;
        _ttruthjetpy = -9999;
        _ttruthjetpz = -9999;
      }

      tjetdphi = trig_phi - _trecojetphi;
      if (tjetdphi < pi2)
        tjetdphi += 2. * pi;
      if (tjetdphi > threepi2)
        tjetdphi -= 2. * pi;

      if (fabs(tjetdphi) < 0.05)
        //just pairedthe photon with itself instead of with a jet, so skip it
        continue;

      tjetdeta = trig_eta - _trecojeteta;
      tjetpout = _trecojetpt * TMath::Sin(jetdphi);

      isophot_trackjet_tree->Fill();
    }
  }

  //now collect awayside cluster jets

  for (JetMap::Iter iter = jets->begin();
       iter != jets->end();
       ++iter)
  {
    Jet *jet = iter->second;
    Jet *truthjet = recoeval->max_truth_jet_by_energy(jet);

    _recojetpt = jet->get_pt();
    if (_recojetpt < minjetpt)
      continue;

    _recojeteta = jet->get_eta();
    if (_recojeteta < etalow || _recojeteta > etahigh)
      continue;

    _recojetpx = jet->get_px();
    _recojetpy = jet->get_py();
    _recojetpz = jet->get_pz();
    _recojetphi = jet->get_phi();
    _recojetmass = jet->get_mass();
    _recojetp = jet->get_p();
    _recojetenergy = jet->get_e();
    _recojetid = jet->get_id();
    int pair_ntruthconstituents = 0;
    if (truthjet)
    {
      _truthjetid = truthjet->get_id();
      _truthjetp = truthjet->get_p();
      _truthjetphi = truthjet->get_phi();
      _truthjeteta = truthjet->get_eta();
      _truthjetpt = truthjet->get_pt();
      _truthjetenergy = truthjet->get_e();
      _truthjetpx = truthjet->get_px();
      _truthjetpy = truthjet->get_py();
      _truthjetpz = truthjet->get_pz();
      _truthjetmass = truthjet->get_mass();

      //check that the jet isn't just a photon or something like this
      //needs at least 2 constituents and that 90% of jet isn't from one photon
      std::set<PHG4Particle *> truthjetcomp =
          trutheval->all_truth_particles(truthjet);

      float truthjetenergysum = 0;
      float truthjethighestphoton = 0;
      for (std::set<PHG4Particle *>::iterator iter2 = truthjetcomp.begin();
           iter2 != truthjetcomp.end();
           ++iter2)
      {
        PHG4Particle *truthpart = *iter2;
        if (!truthpart)
        {
          cout << "no truth particles in the jet??" << endl;
          break;
        }
        pair_ntruthconstituents++;

        int constpid = truthpart->get_pid();
        float conste = truthpart->get_e();

        truthjetenergysum += conste;

        if (constpid == 22)
        {
          if (conste > truthjethighestphoton)
            truthjethighestphoton = conste;
        }
      }
    }

    else
    {
      _truthjetid = 0;
      _truthjetp = 0;
      _truthjetphi = 0;
      _truthjeteta = 0;
      _truthjetpt = 0;
      _truthjetenergy = 0;
      _truthjetpx = 0;
      _truthjetpy = 0;
      _truthjetpz = 0;

      //if the jetevalstack can't find a good truth jet
      //try to match reco jet with closest truth jet
      float closestjet = 9999;
      for (JetMap::Iter iter = truthjets->begin(); iter != truthjets->end(); ++iter)
      {
        Jet *trutherjet = iter->second;

        float thisjetpt = trutherjet->get_pt();
        if (thisjetpt < minjetpt)
          continue;

        float thisjeteta = trutherjet->get_eta();
        float thisjetphi = trutherjet->get_phi();

        float dphi = recojetphi - thisjetphi;
        if (dphi > 3. * pi / 2.)
          dphi -= pi * 2.;
        if (dphi < -1. * pi / 2.)
          dphi += pi * 2.;

        float deta = recojeteta - thisjeteta;

        float dR = sqrt(pow(dphi, 2.) + pow(deta, 2.));

        if (dR < jets->get_par() && dR < closestjet)
        {
          _truthjetid = -9999;  //indicates matched with distance, not evalstack
          _truthjetp = trutherjet->get_p();
          _truthjetphi = trutherjet->get_phi();
          _truthjeteta = trutherjet->get_eta();
          _truthjetpt = trutherjet->get_pt();
          _truthjetenergy = trutherjet->get_e();
          _truthjetpx = trutherjet->get_px();
          _truthjetpy = trutherjet->get_py();
          _truthjetpz = trutherjet->get_pz();
          _truthjetmass = trutherjet->get_mass();
        }

        if (dR < closestjet)
          closestjet = dR;
      }
    }

    //make sure the jet has at least 3 constiutents
    if (!is_AA && pair_ntruthconstituents < 3)
      continue;

    jetdphi = trig_phi - _recojetphi;
    if (jetdphi < pi2)
      jetdphi += 2. * pi;
    if (jetdphi > threepi2)
      jetdphi -= 2. * pi;

    //just pairedthe photon with itself instead of with a jet, so skip it
    if (fabs(jetdphi) < 0.05)
      continue;

    jetdeta = trig_eta - _recojeteta;
    jetpout = _recojetpt * TMath::Sin(jetdphi);

    isophot_jet_tree->Fill();
  }
}

float PhotonJet::ConeSum(RawCluster *cluster,
                         RawClusterContainer *cluster_container,
                         SvtxTrackMap *trackmap,
                         float coneradius,
                         GlobalVertex *vtx)
{
  float energyptsum = 0;

  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

  RawClusterContainer::ConstRange begin_end = cluster_container->getClusters();
  RawClusterContainer::ConstIterator clusiter;

  for (clusiter = begin_end.first;
       clusiter != begin_end.second;
       ++clusiter)
  {
    RawCluster *conecluster = clusiter->second;

    //check to make sure that the candidate isolated photon isn't being counted in the energy sum
    if (conecluster->get_energy() == cluster->get_energy())
      if (conecluster->get_phi() == cluster->get_phi())
        if (conecluster->get_z() == cluster->get_z())
          continue;

    CLHEP::Hep3Vector E_vec_conecluster = RawClusterUtility::GetECoreVec(*conecluster, vertex);

    float cone_pt = E_vec_conecluster.perp();

    if (cone_pt < 0.2)
      continue;

    float cone_e = conecluster->get_energy();
    float cone_eta = E_vec_conecluster.pseudoRapidity();
    float cone_phi = E_vec_conecluster.getPhi();

    float dphi = cluster->get_phi() - cone_phi;
    if (dphi < -1 * pi)
      dphi += 2. * pi;
    if (dphi > pi)
      dphi -= 2. * pi;

    float deta = E_vec_cluster.pseudoRapidity() - cone_eta;

    float radius = sqrt(dphi * dphi + deta * deta);

    if (radius < coneradius)
    {
      energyptsum += cone_e;
    }
  }

  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;

    float trackpx = track->get_px();
    float trackpy = track->get_py();
    float trackpt = sqrt(trackpx * trackpx + trackpy * trackpy);
    if (trackpt < 0.2)
      continue;
    float trackphi = track->get_phi();
    float tracketa = track->get_eta();
    float dphi = E_vec_cluster.getPhi() - trackphi;
    float deta = E_vec_cluster.pseudoRapidity() - tracketa;
    float radius = sqrt(dphi * dphi + deta * deta);

    if (radius < coneradius)
    {
      energyptsum += trackpt;
    }
  }

  return energyptsum;
}

void PhotonJet::Set_Tree_Branches()
{
  cluster_tree = new TTree("clustertree", "A tree with EMCal cluster information");
  cluster_tree->Branch("clus_energy", &clus_energy, "clus_energy/F");
  cluster_tree->Branch("clus_eta", &clus_eta, "clus_eta/F");
  cluster_tree->Branch("clus_phi", &clus_phi, "clus_phi/F");
  cluster_tree->Branch("clus_pt", &clus_pt, "clus_pt/F");
  cluster_tree->Branch("clus_theta", &clus_theta, "clus_theta/F");
  cluster_tree->Branch("clus_x", &clus_x, "clus_x/F");
  cluster_tree->Branch("clus_y", &clus_y, "clus_y/F");
  cluster_tree->Branch("clus_z", &clus_z, "clus_z/F");
  cluster_tree->Branch("clus_t", &clus_t, "clus_t/F");
  cluster_tree->Branch("clus_px", &clus_px, "clus_px/F");
  cluster_tree->Branch("clus_py", &clus_py, "clus_py/F");
  cluster_tree->Branch("clus_pz", &clus_pz, "clus_pz/F");
  cluster_tree->Branch("nevents", &nevents, "nevents/I");
  cluster_tree->Branch("process_id", &process_id, "process_id/I");
  cluster_tree->Branch("x1", &x1, "x1/F");
  cluster_tree->Branch("x2", &x2, "x2/F");
  cluster_tree->Branch("partid1", &partid1, "partid1/I");
  cluster_tree->Branch("partid2", &partid2, "partid2/I");
  cluster_tree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  cluster_tree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  cluster_tree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  cluster_tree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  cluster_tree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  cluster_tree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  isolated_clusters = new TTree("isolated_clusters", "a tree with isolated EMCal clusters");
  isolated_clusters->Branch("clus_energy", &clus_energy, "clus_energy/F");
  isolated_clusters->Branch("clus_eta", &clus_eta, "clus_eta/F");
  isolated_clusters->Branch("clus_phi", &clus_phi, "clus_phi/F");
  isolated_clusters->Branch("clus_pt", &clus_pt, "clus_pt/F");
  isolated_clusters->Branch("clus_theta", &clus_theta, "clus_theta/F");
  isolated_clusters->Branch("clus_x", &clus_x, "clus_x/F");
  isolated_clusters->Branch("clus_y", &clus_y, "clus_y/F");
  isolated_clusters->Branch("clus_z", &clus_z, "clus_z/F");
  isolated_clusters->Branch("clus_t", &clus_t, "clus_t/F");
  isolated_clusters->Branch("clus_px", &clus_px, "clus_px/F");
  isolated_clusters->Branch("clus_py", &clus_py, "clus_py/F");
  isolated_clusters->Branch("clus_pz", &clus_pz, "clus_pz/F");
  isolated_clusters->Branch("nevents", &nevents, "nenvents/I");
  isolated_clusters->Branch("clustruthenergy", &clustruthenergy, "clustruthenergy/F");
  isolated_clusters->Branch("clustruthpt", &clustruthpt, "clustruthpt/F");
  isolated_clusters->Branch("clustruthphi", &clustruthphi, "clustruthphi/F");
  isolated_clusters->Branch("clustrutheta", &clustrutheta, "clustrutheta/F");
  isolated_clusters->Branch("clustruthpx", &clustruthpx, "clustruthpx/F");
  isolated_clusters->Branch("clustruthpy", &clustruthpy, "clustruthpy/F");
  isolated_clusters->Branch("clustruthpz", &clustruthpz, "clustruthpz/F");
  isolated_clusters->Branch("process_id", &process_id, "process_id/I");
  isolated_clusters->Branch("x1", &x1, "x1/F");
  isolated_clusters->Branch("x2", &x2, "x2/F");
  isolated_clusters->Branch("partid1", &partid1, "partid1/I");
  isolated_clusters->Branch("partid2", &partid2, "partid2/I");
  isolated_clusters->Branch("E_4x4", &E_4x4, "E_4x4/F");
  isolated_clusters->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  isolated_clusters->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  isolated_clusters->Branch("E_2x2", &E_2x2, "E_2x2/F");
  isolated_clusters->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  isolated_clusters->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  tracktree = new TTree("tracktree", "a tree with svtx tracks");
  tracktree->Branch("tr_px", &tr_px, "tr_px/F");
  tracktree->Branch("tr_py", &tr_py, "tr_py/F");
  tracktree->Branch("tr_pz", &tr_pz, "tr_pz/F");
  tracktree->Branch("tr_pt", &tr_pt, "tr_pt/F");
  tracktree->Branch("tr_phi", &tr_phi, "tr_phi/F");
  tracktree->Branch("tr_eta", &tr_eta, "tr_eta/F");
  tracktree->Branch("charge", &charge, "charge/I");
  tracktree->Branch("chisq", &chisq, "chisq/F");
  tracktree->Branch("ndf", &ndf, "ndf/I");
  tracktree->Branch("dca", &dca, "dca/F");
  tracktree->Branch("tr_x", &tr_x, "tr_x/F");
  tracktree->Branch("tr_y", &tr_y, "tr_y/F");
  tracktree->Branch("tr_z", &tr_z, "tr_z/F");
  tracktree->Branch("nevents", &nevents, "nevents/i");

  tracktree->Branch("truthtrackpx", &truthtrackpx, "truthtrackpx/F");
  tracktree->Branch("truthtrackpy", &truthtrackpy, "truthtrackpy/F");
  tracktree->Branch("truthtrackpz", &truthtrackpz, "truthtrackpz/F");
  tracktree->Branch("truthtrackp", &truthtrackp, "truthtrackp/F");
  tracktree->Branch("truthtracke", &truthtracke, "truthtracke/F");
  tracktree->Branch("truthtrackpt", &truthtrackpt, "truthtrackpt/F");
  tracktree->Branch("truthtrackphi", &truthtrackphi, "truthtrackphi/F");
  tracktree->Branch("truthtracketa", &truthtracketa, "truthtracketa/F");
  tracktree->Branch("truthtrackpid", &truthtrackpid, "truthtrackpid/I");
  tracktree->Branch("truth_is_primary", &truth_is_primary, "truth_is_primary/B");
  tracktree->Branch("process_id", &process_id, "process_id/I");
  tracktree->Branch("x1", &x1, "x1/F");
  tracktree->Branch("x2", &x2, "x2/F");
  tracktree->Branch("partid1", &partid1, "partid1/I");
  tracktree->Branch("partid2", &partid2, "partid2/I");

  truthjettree = new TTree("truthjettree", "a tree with truth jets");
  truthjettree->Branch("ntruthconstituents", &ntruthconstituents, "ntruthconstituents/I");
  truthjettree->Branch("truthjetpt", &truthjetpt, "truthjetpt/F");
  truthjettree->Branch("truthjetpx", &truthjetpx, "truthjetpx/F");
  truthjettree->Branch("truthjetpy", &truthjetpy, "truthjetpy/F");
  truthjettree->Branch("truthjetpz", &truthjetpz, "truthjetpz/F");
  truthjettree->Branch("truthjetphi", &truthjetphi, "truthjetphi/F");
  truthjettree->Branch("truthjeteta", &truthjeteta, "truthjeteta/F");
  truthjettree->Branch("truthjetmass", &truthjetmass, "truthjetmass/F");
  truthjettree->Branch("truthjetp", &truthjetp, "truthjetp/F");
  truthjettree->Branch("truthjetenergy", &truthjetenergy, "truthjetenergy/F");
  truthjettree->Branch("nevents", &nevents, "nevents/I");
  truthjettree->Branch("process_id", &process_id, "process_id/I");
  truthjettree->Branch("x1", &x1, "x1/F");
  truthjettree->Branch("x2", &x2, "x2/F");
  truthjettree->Branch("partid1", &partid1, "partid1/I");
  truthjettree->Branch("partid2", &partid2, "partid2/I");
  truthjettree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  truthjettree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  truthjettree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  truthjettree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  truthjettree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  truthjettree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  recojettree = new TTree("recojettree", "a tree with reco jets");
  recojettree->Branch("dR", &dR, "dR/F");
  recojettree->Branch("recojetpt", &recojetpt, "recojetpt/F");
  recojettree->Branch("recojetpx", &recojetpx, "recojetpx/F");
  recojettree->Branch("recojetpy", &recojetpy, "recojetpy/F");
  recojettree->Branch("recojetpz", &recojetpz, "recojetpz/F");
  recojettree->Branch("recojetphi", &recojetphi, "recojetphi/F");
  recojettree->Branch("recojeteta", &recojeteta, "recojeteta/F");
  recojettree->Branch("recojetmass", &recojetmass, "recojetmass/F");
  recojettree->Branch("recojetp", &recojetp, "recojetp/F");
  recojettree->Branch("recojetenergy", &recojetenergy, "recojetenergy/F");
  recojettree->Branch("nevents", &nevents, "nevents/I");
  recojettree->Branch("recojetid", &recojetid, "recojetid/F");
  recojettree->Branch("truthjetid", &truthjetid, "truthjetid/F");
  recojettree->Branch("truthjetp", &truthjetp, "truthjetp/F");
  recojettree->Branch("truthjetphi", &truthjetphi, "truthjetphi/F");
  recojettree->Branch("truthjeteta", &truthjeteta, "truthjeteta/F");
  recojettree->Branch("truthjetpt", &truthjetpt, "truthjetpt/F");
  recojettree->Branch("truthjetenergy", &truthjetenergy, "truthjetenergy/F");
  recojettree->Branch("truthjetpx", &truthjetpx, "truthjetpx/F");
  recojettree->Branch("truthjetpy", &truthjetpy, "truthjetpy/F");
  recojettree->Branch("truthjetpz", &truthjetpz, "truthjetpz/F");
  recojettree->Branch("process_id", &process_id, "process_id/I");
  recojettree->Branch("truthjetmass", &truthjetmass, "truthjetmass/F");
  recojettree->Branch("x1", &x1, "x1/F");
  recojettree->Branch("x2", &x2, "x2/F");
  recojettree->Branch("partid1", &partid1, "partid1/I");
  recojettree->Branch("partid2", &partid2, "partid2/I");
  recojettree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  recojettree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  recojettree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  recojettree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  recojettree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  recojettree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");
  recojettree->Branch("constituent_dphis","std::vector<float>",&constituent_dphis);
  recojettree->Branch("constituent_detas","std::vector<float>",&constituent_detas);


  isophot_jet_tree = new TTree("isophoton-jets",
                               "a tree with correlated isolated photons and jets");
  isophot_jet_tree->Branch("clus_energy", &clus_energy, "clus_energy/F");
  isophot_jet_tree->Branch("clus_eta", &clus_eta, "clus_eta/F");
  isophot_jet_tree->Branch("clus_phi", &clus_phi, "clus_phi/F");
  isophot_jet_tree->Branch("clus_pt", &clus_pt, "clus_pt/F");
  isophot_jet_tree->Branch("clus_theta", &clus_theta, "clus_theta/F");
  isophot_jet_tree->Branch("clus_x", &clus_x, "clus_x/F");
  isophot_jet_tree->Branch("clus_y", &clus_y, "clus_y/F");
  isophot_jet_tree->Branch("clus_z", &clus_z, "clus_z/F");
  isophot_jet_tree->Branch("clus_t", &clus_t, "clus_t/F");
  isophot_jet_tree->Branch("clus_px", &clus_px, "clus_px/F");
  isophot_jet_tree->Branch("clus_py", &clus_py, "clus_py/F");
  isophot_jet_tree->Branch("clus_pz", &clus_pz, "clus_pz/F");
  isophot_jet_tree->Branch("clustruthenergy", &clustruthenergy, "clustruthenergy/F");
  isophot_jet_tree->Branch("clustruthpt", &clustruthpt, "clustruthpt/F");
  isophot_jet_tree->Branch("clustruthphi", &clustruthphi, "clustruthphi/F");
  isophot_jet_tree->Branch("clustrutheta", &clustrutheta, "clustrutheta/F");
  isophot_jet_tree->Branch("clustruthpx", &clustruthpx, "clustruthpx/F");
  isophot_jet_tree->Branch("clustruthpy", &clustruthpy, "clustruthpy/F");
  isophot_jet_tree->Branch("clustruthpz", &clustruthpz, "clustruthpz/F");
  isophot_jet_tree->Branch("_recojetpt", &_recojetpt, "_recojetpt/F");
  isophot_jet_tree->Branch("_recojetpx", &_recojetpx, "_recojetpx/F");
  isophot_jet_tree->Branch("_recojetpy", &_recojetpy, "_recojetpy/F");
  isophot_jet_tree->Branch("_recojetpz", &_recojetpz, "_recojetpz/F");
  isophot_jet_tree->Branch("_recojetphi", &_recojetphi, "_recojetphi/F");
  isophot_jet_tree->Branch("_recojeteta", &_recojeteta, "_recojeteta/F");
  isophot_jet_tree->Branch("_recojetmass", &_recojetmass, "_recojetmass/F");
  isophot_jet_tree->Branch("_recojetp", &_recojetp, "_recojetp/F");
  isophot_jet_tree->Branch("_recojetenergy", &_recojetenergy, "_recojetenergy/F");
  isophot_jet_tree->Branch("jetdphi", &jetdphi, "jetdphi/F");
  isophot_jet_tree->Branch("jetdeta", &jetdeta, "jetdeta/F");
  isophot_jet_tree->Branch("jetpout", &jetpout, "jetpout/F");
  isophot_jet_tree->Branch("nevents", &nevents, "nevents/I");
  isophot_jet_tree->Branch("_recojetid", &_recojetid, "_recojetid/F");
  isophot_jet_tree->Branch("_truthjetid", &_truthjetid, "_truthjetid/F");
  isophot_jet_tree->Branch("_truthjetp", &_truthjetp, "_truthjetp/F");
  isophot_jet_tree->Branch("_truthjetphi", &_truthjetphi, "_truthjetphi/F");
  isophot_jet_tree->Branch("_truthjeteta", &_truthjeteta, "_truthjeteta/F");
  isophot_jet_tree->Branch("_truthjetpt", &_truthjetpt, "_truthjetpt/F");
  isophot_jet_tree->Branch("_truthjetenergy", &_truthjetenergy, "_truthjetenergy/F");
  isophot_jet_tree->Branch("_truthjetpx", &_truthjetpx, "_truthjetpx/F");
  isophot_jet_tree->Branch("_truthjetpy", &_truthjetpy, "_truthjetpy/F");
  isophot_jet_tree->Branch("_truthjetpz", &_truthjetpz, "_truthjetpz/F");
  isophot_jet_tree->Branch("process_id", &process_id, "process_id/I");
  isophot_jet_tree->Branch("x1", &x1, "x1/F");
  isophot_jet_tree->Branch("x2", &x2, "x2/F");
  isophot_jet_tree->Branch("partid1", &partid1, "partid1/I");
  isophot_jet_tree->Branch("partid2", &partid2, "partid2/I");
  isophot_jet_tree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  isophot_jet_tree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  isophot_jet_tree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  isophot_jet_tree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  isophot_jet_tree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  isophot_jet_tree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  isophot_trackjet_tree = new TTree("isophoton-trackjets",
                                    "a tree with correlated isolated photons and track jets");
  isophot_trackjet_tree->Branch("clus_energy", &clus_energy, "clus_energy/F");
  isophot_trackjet_tree->Branch("clus_eta", &clus_eta, "clus_eta/F");
  isophot_trackjet_tree->Branch("clus_phi", &clus_phi, "clus_phi/F");
  isophot_trackjet_tree->Branch("clus_pt", &clus_pt, "clus_pt/F");
  isophot_trackjet_tree->Branch("clus_theta", &clus_theta, "clus_theta/F");
  isophot_trackjet_tree->Branch("clus_x", &clus_x, "clus_x/F");
  isophot_trackjet_tree->Branch("clus_y", &clus_y, "clus_y/F");
  isophot_trackjet_tree->Branch("clus_z", &clus_z, "clus_z/F");
  isophot_trackjet_tree->Branch("clus_t", &clus_t, "clus_t/F");
  isophot_trackjet_tree->Branch("clus_px", &clus_px, "clus_px/F");
  isophot_trackjet_tree->Branch("clus_py", &clus_py, "clus_py/F");
  isophot_trackjet_tree->Branch("clus_pz", &clus_pz, "clus_pz/F");
  isophot_trackjet_tree->Branch("clustruthenergy", &clustruthenergy, "clustruthenergy/F");
  isophot_trackjet_tree->Branch("clustruthpt", &clustruthpt, "clustruthpt/F");
  isophot_trackjet_tree->Branch("clustruthphi", &clustruthphi, "clustruthphi/F");
  isophot_trackjet_tree->Branch("clustrutheta", &clustrutheta, "clustrutheta/F");
  isophot_trackjet_tree->Branch("clustruthpx", &clustruthpx, "clustruthpx/F");
  isophot_trackjet_tree->Branch("clustruthpy", &clustruthpy, "clustruthpy/F");
  isophot_trackjet_tree->Branch("clustruthpz", &clustruthpz, "clustruthpz/F");
  isophot_trackjet_tree->Branch("_trecojetpt", &_trecojetpt, "_trecojetpt/F");
  isophot_trackjet_tree->Branch("_trecojetpx", &_trecojetpx, "_trecojetpx/F");
  isophot_trackjet_tree->Branch("_trecojetpy", &_trecojetpy, "_trecojetpy/F");
  isophot_trackjet_tree->Branch("_trecojetpz", &_trecojetpz, "_trecojetpz/F");
  isophot_trackjet_tree->Branch("_trecojetphi", &_trecojetphi, "_trecojetphi/F");
  isophot_trackjet_tree->Branch("_trecojeteta", &_trecojeteta, "_trecojeteta/F");
  isophot_trackjet_tree->Branch("_trecojetmass", &_trecojetmass, "_trecojetmass/F");
  isophot_trackjet_tree->Branch("_trecojetp", &_trecojetp, "_trecojetp/F");
  isophot_trackjet_tree->Branch("_trecojetenergy", &_trecojetenergy, "_trecojetenergy/F");
  isophot_trackjet_tree->Branch("tjetdphi", &tjetdphi, "tjetdphi/F");
  isophot_trackjet_tree->Branch("tjetdeta", &tjetdeta, "tjetdeta/F");
  isophot_trackjet_tree->Branch("tjetpout", &tjetpout, "tjetpout/F");
  isophot_trackjet_tree->Branch("nevents", &nevents, "nevents/I");
  isophot_trackjet_tree->Branch("_trecojetid", &_trecojetid, "_trecojetid/F");
  isophot_trackjet_tree->Branch("_ttruthjetid", &_ttruthjetid, "_ttruthjetid/F");
  isophot_trackjet_tree->Branch("_ttruthjetp", &_ttruthjetp, "_ttruthjetp/F");
  isophot_trackjet_tree->Branch("_ttruthjetphi", &_ttruthjetphi, "_ttruthjetphi/F");
  isophot_trackjet_tree->Branch("_ttruthjeteta", &_ttruthjeteta, "_ttruthjeteta/F");
  isophot_trackjet_tree->Branch("_ttruthjetpt", &_ttruthjetpt, "_ttruthjetpt/F");
  isophot_trackjet_tree->Branch("_ttruthjetenergy", &_ttruthjetenergy, "_ttruthjetenergy/F");
  isophot_trackjet_tree->Branch("_ttruthjetpx", &_ttruthjetpx, "_ttruthjetpx/F");
  isophot_trackjet_tree->Branch("_ttruthjetpy", &_ttruthjetpy, "_ttruthjetpy/F");
  isophot_trackjet_tree->Branch("_ttruthjetpz", &_ttruthjetpz, "_ttruthjetpz/F");
  isophot_trackjet_tree->Branch("process_id", &process_id, "process_id/I");
  isophot_trackjet_tree->Branch("x1", &x1, "x1/F");
  isophot_trackjet_tree->Branch("x2", &x2, "x2/F");
  isophot_trackjet_tree->Branch("partid1", &partid1, "partid1/I");
  isophot_trackjet_tree->Branch("partid2", &partid2, "partid2/I");
  isophot_trackjet_tree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  isophot_trackjet_tree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  isophot_trackjet_tree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  isophot_trackjet_tree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  isophot_trackjet_tree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  isophot_trackjet_tree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  isophot_had_tree = new TTree("isophoton-hads", "a tree with correlated isolated photons and hadrons");
  isophot_had_tree->Branch("clus_energy", &clus_energy, "clus_energy/F");
  isophot_had_tree->Branch("clus_eta", &clus_eta, "clus_eta/F");
  isophot_had_tree->Branch("clus_phi", &clus_phi, "clus_phi/F");
  isophot_had_tree->Branch("clus_pt", &clus_pt, "clus_pt/F");
  isophot_had_tree->Branch("clus_theta", &clus_theta, "clus_theta/F");
  isophot_had_tree->Branch("clus_x", &clus_x, "clus_x/F");
  isophot_had_tree->Branch("clus_y", &clus_y, "clus_y/F");
  isophot_had_tree->Branch("clus_z", &clus_z, "clus_z/F");
  isophot_had_tree->Branch("clus_t", &clus_t, "clus_t/F");
  isophot_had_tree->Branch("clus_px", &clus_px, "clus_px/F");
  isophot_had_tree->Branch("clus_py", &clus_py, "clus_py/F");
  isophot_had_tree->Branch("clus_pz", &clus_pz, "clus_pz/F");
  isophot_had_tree->Branch("clustruthenergy", &clustruthenergy, "clustruthenergy/F");
  isophot_had_tree->Branch("clustruthpt", &clustruthpt, "clustruthpt/F");
  isophot_had_tree->Branch("clustruthphi", &clustruthphi, "clustruthphi/F");
  isophot_had_tree->Branch("clustrutheta", &clustrutheta, "clustrutheta/F");
  isophot_had_tree->Branch("clustruthpx", &clustruthpx, "clustruthpx/F");
  isophot_had_tree->Branch("clustruthpy", &clustruthpy, "clustruthpy/F");
  isophot_had_tree->Branch("clustruthpz", &clustruthpz, "clustruthpz/F");

  isophot_had_tree->Branch("_tr_px", &_tr_px, "_tr_px/F");
  isophot_had_tree->Branch("_tr_py", &_tr_py, "_tr_py/F");
  isophot_had_tree->Branch("_tr_pz", &_tr_pz, "_tr_pz/F");
  isophot_had_tree->Branch("_tr_pt", &_tr_pt, "_tr_pt/F");
  isophot_had_tree->Branch("_tr_phi", &_tr_phi, "_tr_phi/F");
  isophot_had_tree->Branch("_tr_eta", &_tr_eta, "_tr_eta/F");
  isophot_had_tree->Branch("_charge", &_charge, "_charge/I");
  isophot_had_tree->Branch("_chisq", &_chisq, "_chisq/F");
  isophot_had_tree->Branch("_ndf", &_ndf, "_ndf/I");
  isophot_had_tree->Branch("_dca", &_dca, "_dca/F");
  isophot_had_tree->Branch("_tr_x", &_tr_x, "_tr_x/F");
  isophot_had_tree->Branch("_tr_y", &_tr_y, "_tr_y/F");
  isophot_had_tree->Branch("_tr_z", &_tr_z, "_tr_z/F");
  isophot_had_tree->Branch("haddphi", &haddphi, "haddphi/F");
  isophot_had_tree->Branch("hadpout", &hadpout, "hadpout/F");
  isophot_had_tree->Branch("haddeta", &haddeta, "haddeta/F");
  isophot_had_tree->Branch("nevents", &nevents, "nevents/I");
  isophot_had_tree->Branch("_truthtrackpx", &_truthtrackpx, "_truthtrackpx/F");
  isophot_had_tree->Branch("_truthtrackpy", &_truthtrackpy, "_truthtrackpy/F");
  isophot_had_tree->Branch("_truthtrackpz", &_truthtrackpz, "_truthtrackpz/F");
  isophot_had_tree->Branch("_truthtrackp", &_truthtrackp, "_truthtrackp/F");
  isophot_had_tree->Branch("_truthtracke", &_truthtracke, "_truthtracke/F");
  isophot_had_tree->Branch("_truthtrackpt", &_truthtrackpt, "_truthtrackpt/F");
  isophot_had_tree->Branch("_truthtrackphi", &_truthtrackphi, "_truthtrackphi/F");
  isophot_had_tree->Branch("_truthtracketa", &_truthtracketa, "_truthtracketa/F");
  isophot_had_tree->Branch("_truthtrackpid", &_truthtrackpid, "_truthtrackpid/I");
  isophot_had_tree->Branch("_truth_is_primary", &_truth_is_primary, "_truth_is_primary/B");
  isophot_had_tree->Branch("process_id", &process_id, "process_id/I");
  isophot_had_tree->Branch("x1", &x1, "x1/F");
  isophot_had_tree->Branch("x2", &x2, "x2/F");
  isophot_had_tree->Branch("partid1", &partid1, "partid1/I");
  isophot_had_tree->Branch("partid2", &partid2, "partid2/I");
  isophot_had_tree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  isophot_had_tree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  isophot_had_tree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  isophot_had_tree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  isophot_had_tree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  isophot_had_tree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  truth_g4particles = new TTree("truthtree_g4", "a tree with all truth g4 particles");
  truth_g4particles->Branch("truthpx", &truthpx, "truthpx/F");
  truth_g4particles->Branch("truthpy", &truthpy, "truthpy/F");
  truth_g4particles->Branch("truthpz", &truthpz, "truthpz/F");
  truth_g4particles->Branch("truthp", &truthp, "truthp/F");
  truth_g4particles->Branch("truthenergy", &truthenergy, "truthenergy/F");
  truth_g4particles->Branch("truthphi", &truthphi, "truthphi/F");
  truth_g4particles->Branch("trutheta", &trutheta, "trutheta/F");
  truth_g4particles->Branch("truthpt", &truthpt, "truthpt/F");
  truth_g4particles->Branch("truthpid", &truthpid, "truthpid/I");
  truth_g4particles->Branch("nevents", &nevents, "nevents/I");
  truth_g4particles->Branch("process_id", &process_id, "process_id/I");
  truth_g4particles->Branch("x1", &x1, "x1/F");
  truth_g4particles->Branch("x2", &x2, "x2/F");
  truth_g4particles->Branch("partid1", &partid1, "partid1/I");
  truth_g4particles->Branch("partid2", &partid2, "partid2/I");
  truth_g4particles->Branch("E_4x4", &E_4x4, "E_4x4/F");
  truth_g4particles->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  truth_g4particles->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  truth_g4particles->Branch("E_2x2", &E_2x2, "E_2x2/F");
  truth_g4particles->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  truth_g4particles->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");

  truthtree = new TTree("truthtree", "a tree with all truth pythia particles");
  truthtree->Branch("truthpx", &truthpx, "truthpx/F");
  truthtree->Branch("truthpy", &truthpy, "truthpy/F");
  truthtree->Branch("truthpz", &truthpz, "truthpz/F");
  truthtree->Branch("truthp", &truthp, "truthp/F");
  truthtree->Branch("truthenergy", &truthenergy, "truthenergy/F");
  truthtree->Branch("truthphi", &truthphi, "truthphi/F");
  truthtree->Branch("trutheta", &trutheta, "trutheta/F");
  truthtree->Branch("truthpt", &truthpt, "truthpt/F");
  truthtree->Branch("truthpid", &truthpid, "truthpid/I");
  truthtree->Branch("nevents", &nevents, "nevents/I");
  truthtree->Branch("numparticlesinevent", &numparticlesinevent, "numparticlesinevent/I");
  truthtree->Branch("process_id", &process_id, "process_id/I");
  truthtree->Branch("x1", &x1, "x1/F");
  truthtree->Branch("x2", &x2, "x2/F");
  truthtree->Branch("partid1", &partid1, "partid1/I");
  truthtree->Branch("partid2", &partid2, "partid2/I");

  event_tree = new TTree("eventtree", "A tree with 2 to 2 event info");
  event_tree->Branch("mpi", &mpi, "mpi/F");
  event_tree->Branch("x1", &x1, "x1/F");
  event_tree->Branch("x2", &x2, "x2/F");
  event_tree->Branch("partid1", &partid1, "partid1/I");
  event_tree->Branch("partid2", &partid2, "partid2/I");
  event_tree->Branch("process_id", &process_id, "process_id/I");
  event_tree->Branch("nevents", &nevents, "nevents/I");
  event_tree->Branch("cluseventenergy", &cluseventenergy, "cluseventenergy/F");
  event_tree->Branch("cluseventeta", &cluseventeta, "cluseventeta/F");
  event_tree->Branch("cluseventpt", &cluseventpt, "cluseventpt/F");
  event_tree->Branch("cluseventphi", &cluseventphi, "cluseventphi/F");
  event_tree->Branch("hardest_jetpt", &hardest_jetpt, "hardest_jetpt/F");
  event_tree->Branch("hardest_jetphi", &hardest_jetphi, "hardest_jetphi/F");
  event_tree->Branch("hardest_jeteta", &hardest_jeteta, "hardest_jeteta/F");
  event_tree->Branch("hardest_jetenergy", &hardest_jetenergy, "hardest_jetenergy/F");
  event_tree->Branch("E_4x4", &E_4x4, "E_4x4/F");
  event_tree->Branch("phi_4x4", &phi_4x4, "phi_4x4/F");
  event_tree->Branch("eta_4x4", &eta_4x4, "eta_4x4/F");
  event_tree->Branch("E_2x2", &E_2x2, "E_2x2/F");
  event_tree->Branch("phi_2x2", &phi_2x2, "phi_2x2/F");
  event_tree->Branch("eta_2x2", &eta_2x2, "eta_2x2/F");
}

void PhotonJet::initialize_values()
{
  E_4x4 = 0;
  phi_4x4 = 0;
  eta_4x4 = 0;
  E_2x2 = 0;
  phi_2x2 = 0;
  eta_2x2 = 0;

  event_tree = 0;
  tree = 0;
  cluster_tree = 0;
  truth_g4particles = 0;
  truthtree = 0;
  isolated_clusters = 0;
  tracktree = 0;
  truthjettree = 0;
  recojettree = 0;
  isophot_jet_tree = 0;
  isophot_trackjet_tree = 0;
  isophot_had_tree = 0;

  histo = 0;
  dR = 0;
  percent_photon_h = 0;
  _truthjetmass = -999;
  clus_energy = -999;
  clus_eta = -999;
  clus_phi = -999;
  clus_pt = -999;
  clus_px = -999;
  clus_py = -999;
  clus_pz = -999;
  clus_theta = -999;
  clus_x = -999;
  clus_y = -999;
  clus_z = -999;
  clus_t = -999;

  tr_px = -999;
  tr_py = -999;
  tr_pz = -999;
  tr_p = -999;
  tr_pt = -999;
  tr_phi = -999;
  tr_eta = -999;
  charge = -999;
  chisq = -999;
  ndf = -999;
  dca = -999;
  tr_x = -999;
  tr_y = -999;
  tr_z = -999;
  truthtrackpx = -999;
  truthtrackpy = -999;
  truthtrackpz = -999;
  truthtrackp = -999;
  truthtracke = -999;
  truthtrackpt = -999;
  truthtrackphi = -999;
  truthtracketa = -999;
  truthtrackpid = -999;
  truth_is_primary = false;

  truthjetpt = -999;
  truthjetpx = -999;
  truthjetpy = -999;
  truthjetpz = -999;
  truthjetphi = -999;
  truthjeteta = -999;
  truthjetmass = -999;
  truthjetp = -999;
  truthjetenergy = -999;
  recojetpt = -999;
  recojetpx = -999;
  recojetpy = -999;
  recojetpz = -999;
  recojetphi = -999;
  recojeteta = -999;
  recojetmass = -999;
  recojetp = -999;
  recojetenergy = -999;
  recojetid = -999;
  truthjetid = -999;

  _recojetid = -999;
  _recojetpt = -999;
  _recojetpx = -999;
  _recojetpy = -999;
  _recojetpz = -999;
  _recojetphi = -999;
  _recojeteta = -999;
  _recojetmass = -999;
  _recojetp = -999;
  _recojetenergy = -999;
  jetdphi = -999;
  jetpout = -999;
  jetdeta = -999;
  _truthjetid = -999;
  _truthjetpt = -999;
  _truthjetpx = -999;
  _truthjetpy = -999;
  _truthjetpz = -999;
  _truthjetphi = -999;
  _truthjeteta = -999;
  _truthjetmass = -999;
  _truthjetp = -999;
  _truthjetenergy = -999;

  _trecojetid = -999;
  _trecojetpt = -999;
  _trecojetpx = -999;
  _trecojetpy = -999;
  _trecojetpz = -999;
  _trecojetphi = -999;
  _trecojeteta = -999;
  _trecojetmass = -999;
  _trecojetp = -999;
  _trecojetenergy = -999;
  tjetdphi = -999;
  tjetpout = -999;
  tjetdeta = -999;
  _ttruthjetid = -999;
  _ttruthjetpt = -999;
  _ttruthjetpx = -999;
  _ttruthjetpy = -999;
  _ttruthjetpz = -999;
  _ttruthjetphi = -999;
  _ttruthjeteta = -999;
  _ttruthjetmass = -999;
  _ttruthjetp = -999;
  _ttruthjetenergy = -999;

  _tr_px = -999;
  _tr_py = -999;
  _tr_pz = -999;
  _tr_p = -999;
  _tr_pt = -999;
  _tr_phi = -999;
  _tr_eta = -999;
  _charge = -999;
  _chisq = -999;
  _ndf = -999;
  _dca = -999;
  _tr_x = -999;
  _tr_y = -999;
  _tr_z = -999;
  haddphi = -999;
  hadpout = -999;
  haddeta = -999;
  _truth_is_primary = false;
  _truthtrackpx = -999;
  _truthtrackpy = -999;
  _truthtrackpz = -999;
  _truthtrackp = -999;
  _truthtracke = -999;
  _truthtrackpt = -999;
  _truthtrackphi = -999;
  _truthtracketa = -999;
  _truthtrackpid = -999;

  truthpx = -999;
  truthpy = -999;
  truthpz = -999;
  truthp = -999;
  truthphi = -999;
  trutheta = -999;
  truthpt = -999;
  truthenergy = -999;
  truthpid = -999;
  numparticlesinevent = -999;
  process_id = -999;

  clustruthpx = -999;
  clustruthpy = -999;
  clustruthpz = -999;
  clustruthenergy = -999;
  clustruthpt = -999;
  clustruthphi = -999;
  clustrutheta = -999;
  clustruthpid = -999;

  beam_energy = 0;
  x1 = 0;
  x2 = 0;
  partid1 = 0;
  partid2 = 0;

  hardest_jetpt = 0;
  hardest_jetphi = 0;
  hardest_jeteta = 0;
  hardest_jetenergy = 0;
}
