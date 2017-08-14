#include "JEWELTest.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>

#include <TLorentzVector.h>
#include <iostream>
#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeomContainer.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawTowerGeom.h>

#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4eval/JetEvalStack.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include <g4eval/SvtxEvalStack.h>

#include <HepMC/GenEvent.h>

#include <list>

using namespace std;

float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
  const float pi = 3.1415926;
  float deta = eta1 - eta2;
  float dphi = phi1 - phi2;
  if (dphi > pi) dphi -= 2*pi;
  if (dphi < -pi) dphi += 2*pi;

  return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );

}

JEWELTest::JEWELTest(const std::string &name) : SubsysReco("JEWELTEST")
{
  outfilename = name;

  //add other initializers here
}

int JEWELTest::Init(PHCompositeNode *topnode)
{

  file = new TFile(outfilename.c_str(),"RECREATE");
  nevents=0;

  histo = new TH1F("histo","histo",100,-3,3);

  tree = new TTree("tree","a tree");
  tree->Branch("nevents",&nevents,"nevents/I");

  Set_Tree_Branches();

  return 0;
}

int JEWELTest::process_event(PHCompositeNode *topnode)
{
 
  if(printDebug) std::cout<<"at event number "<<nevents<<std::endl;

  //get the nodes from the NodeTree
  JetMap* truth_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Truth_r06");
  JetMap *reco_jets = findNode::getClass<JetMap>(topnode,"AntiKt_Tower_r06");
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topnode,"G4TruthInfo");
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topnode,"CLUSTER_CEMC");
  RawTowerContainer *cemctowers = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
  RawTowerContainer *hcalintowers = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALIN");
  RawTowerContainer *hcalouttowers = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALOUT");
  RawTowerGeomContainer *cemcgeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *hcalingeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *hcaloutgeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  // SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topnode,"SvtxTrackMap");
  JetEvalStack* _jetevalstack = new JetEvalStack(topnode,"AntiKt_Tower_r06","AntiKt_Truth_r06");
  PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topnode, "PHHepMCGenEvent");

  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  if(!truth_jets){
      if(printDebug) std::cout<<"no truth jets"<<std::endl;
      return 0;
    }
  if(!reco_jets){
      if(printDebug) std::cout<<"no reco jets"<<std::endl;
      return 0;
    }
  if(!truthinfo){
      if(printDebug) std::cout<<"no truth track info"<<std::endl;
      return 0;
    }
  if(!clusters){
    if(printDebug) std::cout<<"no cluster info"<<std::endl;
    return 0;
  }
  if(!cemctowers){
    if(printDebug) std::cout<<"no cemctowers info"<<std::endl;
    // return 0;
  }
  if(!hcalintowers){
    if(printDebug) std::cout<<"no hcalintowers info"<<std::endl;
    // return 0;
  }
  if(!hcalouttowers){
    if(printDebug) std::cout<<"no hcalouttowers info"<<std::endl;
    // return 0;
  }
  if(!cemcgeom){
    if(printDebug) std::cout<<"no cemcgeom info"<<std::endl;
    // return 0;
  }
  if(!hcalingeom){
    if(printDebug) std::cout<<"no hcalingeom info"<<std::endl;
    // return 0;
  }
  if(!hcaloutgeom){
    if(printDebug) std::cout<<"no hcaloutgeom info"<<std::endl;
    // return 0;
  }
  // if(!trackmap){
  //   if(printDebug) std::cout<<"no track map"<<std::endl;
  //   return 0;
  // }
  if(!_jetevalstack){
    if(printDebug) std::cout<<"no good truth jets"<<std::endl;
    return 0;
  }


  
  HepMC::GenEvent *evt = genevt->getEvent();
  if (!evt) {
    if(printDebug) std::cout << PHWHERE << " no evt pointer under HEPMC Node found" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

     
  //! Checking if additional scattering centers are available from HepMC file 
  //! get the scattered particles from the gen event:
  std::list<HepMC::GenParticle *> scatcenterparticles;
  std::list<HepMC::GenParticle *> finalstateparticles;
  for (HepMC::GenEvent::vertex_iterator v = evt->vertices_begin(); v != evt->vertices_end(); ++v) {    
    scatcenterparticles.clear();
    finalstateparticles.clear();
    for (HepMC::GenVertex::particle_iterator p = (*v)->particles_begin(HepMC::children);
	 p != (*v)->particles_end(HepMC::children); ++p) {
      if ((*p)->status() == 3) {
	scatcenterparticles.push_back(*p);
      }
      if ((*p)->status() == 1) {
	finalstateparticles.push_back(*p);
      }
    }
  }
  
  std::list<HepMC::GenParticle *>::const_iterator fiter;
  for (fiter = finalstateparticles.begin(); fiter != finalstateparticles.end(); ++fiter) {
    if (verbosity > 1) (*fiter)->print();
    finalstatepx = (*fiter)->momentum().px();
    finalstatepy = (*fiter)->momentum().py();
    finalstatepz = (*fiter)->momentum().pz();
    finalstatep = sqrt(finalstatepx*finalstatepx+finalstatepy*finalstatepy+finalstatepz*finalstatepz);
    finalstateenergy = (*fiter)->momentum().e();

    TLorentzVector vec;
    vec.SetPxPyPzE(finalstatepx,finalstatepy,finalstatepz,finalstateenergy);      
    finalstatept = sqrt(finalstatepx*finalstatepx+finalstatepy*finalstatepy);
    finalstatephi = vec.Phi();
    finalstateeta = vec.Eta();

    finalstate->Fill();
      
  }

  
  bool doSubtraction = true;
  if(scatcenterparticles.size() == 0)
    doSubtraction = false;

  if(doSubtraction){
    std::list<HepMC::GenParticle *>::const_iterator fiter;
    for (fiter = scatcenterparticles.begin(); fiter != scatcenterparticles.end(); ++fiter) {
      if (verbosity > 1) (*fiter)->print();
      scattcenterpx = (*fiter)->momentum().px();
      scattcenterpy = (*fiter)->momentum().py();
      scattcenterpz = (*fiter)->momentum().pz();
      scattcenterp = sqrt(scattcenterpx*scattcenterpx+scattcenterpy*scattcenterpy+scattcenterpz*scattcenterpz);
      scattcenterenergy = (*fiter)->momentum().e();

      TLorentzVector vec;
      vec.SetPxPyPzE(scattcenterpx,scattcenterpy,scattcenterpz,scattcenterenergy);      
      scattcenterpt = sqrt(scattcenterpx*scattcenterpx+scattcenterpy*scattcenterpy);
      scattcenterphi = vec.Phi();
      scattcentereta = vec.Eta();
      
      scattcenter->Fill();
      
    }
  }

  JetRecoEval* recoeval = _jetevalstack->get_reco_eval();

  //! ******** Truth particles *********  
  for( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter!=range.second; ++iter){

    PHG4Particle *truth = iter->second;
    
    truthpx = truth->get_px();
    truthpy = truth->get_py();
    truthpz = truth->get_pz();
    truthp = sqrt(truthpx*truthpx+truthpy*truthpy+truthpz*truthpz);
    truthenergy = truth->get_e();
    
    TLorentzVector vec;
    vec.SetPxPyPzE(truthpx,truthpy,truthpz,truthenergy);
    
    truthpt = sqrt(truthpx*truthpx+truthpy*truthpy);    
    truthphi = vec.Phi();
    trutheta = vec.Eta();
    truthpid = truth->get_pid();
    
    truth_g4particles->Fill();

  }

  //! ******** EMCal clusters *********  
  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator clusiter;
  
  for(clusiter = begin_end.first; clusiter!=begin_end.second; ++clusiter){

    RawCluster *cluster = clusiter->second;

    clus_energy = cluster->get_energy();
    clus_eta = cluster->get_eta();
    clus_theta = 2.*TMath::ATan((TMath::Exp(-1.*clus_eta)));
    clus_pt = clus_energy*TMath::Sin(clus_theta);
    clus_phi = cluster->get_phi();
    
    if(clus_pt<0.5)
      continue;

    TLorentzVector *clus = new TLorentzVector();
    clus->SetPtEtaPhiE(clus_pt,clus_eta,clus_phi,clus_energy);
    
    float dumarray[4];
    clus->GetXYZT(dumarray);
    clus_x = dumarray[0];
    clus_y = dumarray[1];
    clus_z = dumarray[2];
    clus_t = dumarray[3];

    clus_px = clus_pt*TMath::Cos(clus_phi);
    clus_py = clus_pt*TMath::Sin(clus_phi);
    clus_pz = sqrt(clus_energy*clus_energy-clus_px*clus_px-clus_py*clus_py);

    cluster_tree->Fill();
    
  }

  // //! ******** tracks *********  
  // SvtxEvalStack *svtxevalstack = new SvtxEvalStack(topnode);
  // svtxevalstack->next_event(topnode);

  // SvtxTrackEval *trackeval = svtxevalstack->get_track_eval();

  // for(SvtxTrackMap::Iter iter = trackmap->begin(); iter!=trackmap->end(); ++iter){

  //   SvtxTrack *track = iter->second;

  //   //get reco info
  //   tr_px = track->get_px();
  //   tr_py = track->get_py();
  //   tr_pz = track->get_pz();
  //   tr_p = sqrt(tr_px*tr_px+tr_py*tr_py+tr_pz*tr_pz);
    
  //   tr_pt = sqrt(tr_px*tr_px+tr_py*tr_py);
    
  //   if(tr_pt<0.5)
  //     continue;
  
  //   tr_phi = track->get_phi();
  //   tr_eta = track->get_eta();

  //   if(fabs(tr_eta)>1)
  //     continue;

  //   charge = track->get_charge();
  //   chisq = track->get_chisq();
  //   ndf = track->get_ndf();
  //   dca = track->get_dca();
  //   tr_x = track->get_x();
  //   tr_y = track->get_y();
  //   tr_z = track->get_z();
    
   
  //   //get truth info
  //   PHG4Particle *truthtrack = trackeval->max_truth_particle_by_nclusters(track);
  //   truth_is_primary = truthinfo->is_primary(truthtrack);
    
  //   truthtrackpx = truthtrack->get_px();
  //   truthtrackpy = truthtrack->get_py();
  //   truthtrackpz = truthtrack->get_pz();
  //   truthtrackp = sqrt(truthtrackpx*truthtrackpx+truthtrackpy*truthtrackpy+truthtrackpz*truthtrackpz);
    
  //   truthtracke = truthtrack->get_e();
  //   TLorentzVector *Truthtrack = new TLorentzVector();
  //   Truthtrack->SetPxPyPzE(truthtrackpx,truthtrackpy,truthtrackpz,truthtracke);
  //   truthtrackpt = Truthtrack->Pt();
  //   truthtrackphi = Truthtrack->Phi();
  //   truthtracketa = Truthtrack->Eta();
  //   truthtrackpid = truthtrack->get_pid();


  //   tracktree->Fill();

  // }

  //! Do Sub and UnSub jet pT based on the scattering centers
  //! for Gen Jets - we can do 4MomSub at the moment. need to think about GridSub if it makes sense. 
  
  //! ******** GEN Jets *********
  for(JetMap::Iter iter = truth_jets->begin(); iter!=truth_jets->end(); ++iter){
    Jet *jet = iter->second;
    
    truthjetpt = jet->get_pt();
    if(truthjetpt<5.)
      continue;
    
    truthjeteta = jet->get_eta();
    if(fabs(truthjeteta)>1.)
      continue;

    truthjetpx = jet->get_px();
    truthjetpy = jet->get_py();
    truthjetpz = jet->get_pz();
    truthjetphi = jet->get_phi();
    truthjetmass = jet->get_mass();
    truthjetp = jet->get_p();
    truthjetenergy = jet->get_e();

    if(printDebug) std::cout<<"Truth Jet object "<<std::endl;
    if(printDebug) std::cout<<"pT = "<<truthjetpt<<std::endl;
    if(printDebug) std::cout<<"eta = "<<truthjeteta<<std::endl;
    if(printDebug) std::cout<<"phi = "<<truthjetphi<<std::endl;
    
    TLorentzVector jt, bkg, jt_sub;

    if(printDebug) std::cout<<"Jet constituents 4-vector "<<std::endl;
    //! loop over all the constituents in the truthjet
    for (Jet::ConstIter citer = jet->begin_comp(); citer != jet->end_comp(); ++citer) {
      Jet::SRC source = citer->first;
      unsigned int index = citer->second;
      if (source != Jet::PARTICLE) {
	if(printDebug) std::cout << PHWHERE << " truth jet contains something other than particles!" << std::endl;
	exit(-1);
      }

      PHG4Particle* genjet_constituent = truthinfo->GetParticle(index);

      double genjet_constituentpx = genjet_constituent->get_px();
      double genjet_constituentpy = genjet_constituent->get_py();
      double genjet_constituentpz = genjet_constituent->get_pz();
      // double genjet_constituentp = sqrt(genjet_constituentpx*genjet_constituentpx+
      // 					genjet_constituentpy*genjet_constituentpy+
      // 					genjet_constituentpz*genjet_constituentpz);
      double genjet_constituentenergy = genjet_constituent->get_e();

      TLorentzVector vec;
      vec.SetPxPyPzE(genjet_constituentpx,genjet_constituentpy,genjet_constituentpz,genjet_constituentenergy);

      jt+=vec;

      if(printDebug) std::cout<<vec.Px()<<"\t"<<vec.Py()<<"\t"<<vec.Pz()<<"\t"<<vec.E()<<"\t"<<vec.Pt()<<std::endl;
      
    }

    if(printDebug) std::cout<<"Jet 4-vector "<<std::endl;
    if(printDebug) std::cout<<"pT = "<<jt.Pt()<<std::endl;
    if(printDebug) std::cout<<"eta = "<<jt.Eta()<<std::endl;
    if(printDebug) std::cout<<"phi = "<<jt.Phi()<<std::endl;
    
      
    if(doSubtraction){

      std::list<HepMC::GenParticle *>::const_iterator fiter;
      for (fiter = scatcenterparticles.begin(); fiter != scatcenterparticles.end(); ++fiter) {
	if (verbosity > 1) (*fiter)->print();
	scattcenterpx = (*fiter)->momentum().px();
	scattcenterpy = (*fiter)->momentum().py();
	scattcenterpz = (*fiter)->momentum().pz();
	scattcenterp = sqrt(scattcenterpx*scattcenterpx+scattcenterpy*scattcenterpy+scattcenterpz*scattcenterpz);
	scattcenterenergy = (*fiter)->momentum().e();

	TLorentzVector vec;
	vec.SetPxPyPzE(scattcenterpx,scattcenterpy,scattcenterpz,scattcenterenergy);      
	scattcenterpt = sqrt(scattcenterpx*scattcenterpx+scattcenterpy*scattcenterpy);
	scattcenterphi = vec.Phi();
	scattcentereta = vec.Eta();

	bool isinJet = false;	  
	// double _delRSmall = 100;
	// double _delRposition = 0;
	int counter = 0;

	// if(printDebug) std::cout<<"Truth Jet constituents loop "<<std::endl;

	//! loop over all the constituents in the truthjet
	for (Jet::ConstIter citer = jet->begin_comp(); citer != jet->end_comp(); ++citer) {
	  Jet::SRC source = citer->first;
	  unsigned int index = citer->second;
	  if (source != Jet::PARTICLE) {
	    if(printDebug) std::cout << PHWHERE << " truth jet contains something other than particles!" << std::endl;
	    // exit(-1);
	    continue;
	  }

	  
	  PHG4Particle* genjet_constituent = truthinfo->GetParticle(index);

	  double genjet_constituentpx = genjet_constituent->get_px();
	  double genjet_constituentpy = genjet_constituent->get_py();
	  double genjet_constituentpz = genjet_constituent->get_pz();
	  // double genjet_constituentp = sqrt(genjet_constituentpx*genjet_constituentpx+
	  // 					genjet_constituentpy*genjet_constituentpy+
	  // 					genjet_constituentpz*genjet_constituentpz);
	  double genjet_constituentenergy = genjet_constituent->get_e();
    
	  TLorentzVector vecj;
	  vecj.SetPxPyPzE(genjet_constituentpx,genjet_constituentpy,genjet_constituentpz,genjet_constituentenergy);
    
	  // double genjet_constituentpt = sqrt(genjet_constituentpx*genjet_constituentpx+
	  // 				     genjet_constituentpy*genjet_constituentpy);
	  double genjet_constituentphi = vecj.Phi();
	  double genjet_constituenteta = vecj.Eta();

	  const double _delRTruth = deltaR(scattcentereta, genjet_constituenteta,
					   scattcenterphi, genjet_constituentphi);
	  // const double _delRJet = deltaR(truthjeteta, genjet_constituenteta,
	  // 				 truthjetphi, genjet_constituentphi);

	  // if(_delRJet < 0.6){
	  //   if(printDebug) std::cout<<"scattering center = ("<< scattcentereta <<", "<<scattcenterphi<<"), jet constituent = ("<<
	  //     genjet_constituenteta<<", "<<genjet_constituentphi<<")"<<std::endl;
	  //   if(printDebug) std::cout<<"_delRTruth = " << _delRTruth<<std::endl;
	  //   if(printDebug) std::cout<<"********"<<std::endl;
	  // }
	  
	  if(_delRTruth < 1e-5){
	    // _delRSmall = _delRTruth;
	    // _delRposition = counter;
	    isinJet = true;
	  }
	  counter++;
	}// jet constituents loop
	
	// sum the background contribution
	if(isinJet){

	  bkg += vec;
	  
	}

      }//! scattering center loop 

      if(printDebug) std::cout<<"Total BKG in Jet "<<std::endl;
      if(printDebug) std::cout<<"pT = "<<bkg.Pt()<<std::endl;
      if(printDebug) std::cout<<"eta = "<<bkg.Eta()<<std::endl;
      if(printDebug) std::cout<<"phi = "<<bkg.Phi()<<std::endl;
      
      
      jt_sub = jt - bkg;
      
    } else {
      jt_sub = jt;
    }
    
    if(printDebug) std::cout<<"BKG subtracted Jet 4-vector "<<std::endl;
    if(printDebug) std::cout<<"pT = "<<jt_sub.Pt()<<std::endl;
    if(printDebug) std::cout<<"eta = "<<jt_sub.Eta()<<std::endl;
    if(printDebug) std::cout<<"phi = "<<jt_sub.Phi()<<std::endl;

    //! 4MomSub background subtracted jet = jtsub. its a TLorentzVector
    truthjet_bkgsub_px = jt_sub.Px();
    truthjet_bkgsub_py = jt_sub.Py();
    truthjet_bkgsub_pz = jt_sub.Pz();
    truthjet_bkgsub_phi = jt_sub.Phi();
    truthjet_bkgsub_mass = jt_sub.M();
    truthjet_bkgsub_p = jt_sub.P();
    truthjet_bkgsub_pt = jt_sub.Pt();
    truthjet_bkgsub_energy = jt_sub.E();          

    truthjettree->Fill();
        
  }
  if(printDebug) std::cout<<"Filled truth jet tree"<<std::endl;

  
  //! ******** RECO Jets *********
  for(JetMap::Iter iter = reco_jets->begin(); iter!=reco_jets->end(); ++iter){
    Jet *jet = iter->second;
    Jet *truthjet = recoeval->max_truth_jet_by_energy(jet);
    recojetpt = jet->get_pt();
    if(recojetpt<4.)
      continue;
    
    recojeteta = jet->get_eta();
    if(fabs(recojeteta)>1.)
      continue;
    recojetid = jet->get_id();
    recojetpx = jet->get_px();
    recojetpy = jet->get_py();
    recojetpz = jet->get_pz();
    recojetphi = jet->get_phi();
    recojetmass = jet->get_mass();
    recojetp = jet->get_p();
    recojetenergy = jet->get_e();

    if(printDebug) std::cout<<"Reco Jet object "<<std::endl;
    if(printDebug) std::cout<<"pT = "<<recojetpt<<std::endl;
    if(printDebug) std::cout<<"eta = "<<recojeteta<<std::endl;
    if(printDebug) std::cout<<"phi = "<<recojetphi<<std::endl;
    
    TLorentzVector jt;
    //, bkg, jt_sub;

    if(printDebug) std::cout<<" RECO Jet constituents 4-vector "<<std::endl;
    //! loop over all the constituents in the recojet
    for (Jet::ConstIter citer = jet->begin_comp(); citer != jet->end_comp(); ++citer) {
      Jet::SRC source = citer->first;
      if(printDebug) std::cout<<"going into the recojet constituent source" << source<<std::endl;
      unsigned int index = citer->second;
      if(printDebug) std::cout<<"going into the recojet constituent index" << index<<std::endl;

      if (source == Jet::CEMC_TOWER) {
	RawTower* recojet_constituent = cemctowers->getCluster(index);
	if(printDebug) std::cout<<"got the  recojet constituent object "<<std::endl;
      }else if(source == Jet::HCALIN_TOWER){
	RawTower* recojet_constituent = cemctowers->getCluster(index);
	if(printDebug) std::cout<<"got the  recojet constituent object rawcluster"<<std::endl;	
      }else if(source == Jet::HCALIN_TOWER){
	RawTower* recojet_constituent = cemctowers->getCluster(index);
	if(printDebug) std::cout<<"got the  recojet constituent object rawcluster"<<std::endl;	
      }else {
	if(printDebug) std::cout << PHWHERE << " reco jet contains something other than ecal/hcal towers!" << std::endl;
	// exit(-1);
	continue;
      }
      
      RawTowerGeom * tower_geom = geom->get_tower_geometry(recojet_constituent -> get_key());
      assert(tower_geom);
 
      double r = tower_geom->get_center_radius();
      double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
      double z0 = tower_geom->get_center_z();
 
      double z = z0 - vtxz;
 
      double eta = asinh(z/r); // eta after shift from vertex
 
      double pt = tower->get_energy() / cosh(eta);
      double px = pt * cos(phi);
      double py = pt * sin(phi);
      double pz = pt * sinh(eta);
      
      double cluster_energy = recojet_constituent->get_energy();
      if(printDebug) std::cout<<"got the  recojet constituent object energy "<<cluster_energy<<std::endl;
      double cluster_eta = recojet_constituent->get_eta();
      if(printDebug) std::cout<<"got the  recojet constituent object bineta "<<cluster_eta<<std::endl;
      double cluster_theta = 2.*TMath::ATan((TMath::Exp(-1.*cluster_eta)));
      if(printDebug) std::cout<<"got the  recojet constituent object theta "<<cluster_theta<<std::endl;
      double cluster_pt = cluster_energy*TMath::Sin(cluster_theta);
      if(printDebug) std::cout<<"got the  recojet constituent object pt "<<cluster_pt<<std::endl;
      double cluster_phi = recojet_constituent->get_phi();
      if(printDebug) std::cout<<"got the  recojet constituent object phi "<<cluster_phi<<std::endl;
      // double recojet_constituentp = sqrt(recojet_constituentpx*recojet_constituentpx+
      // 					recojet_constituentpy*recojet_constituentpy+
      // 					recojet_constituentpz*recojet_constituentpz);

      TLorentzVector vec;
      vec.SetPtEtaPhiE(cluster_pt,cluster_eta,cluster_phi,cluster_energy);

      jt+=vec;

      if(printDebug) std::cout<<vec.Px()<<"\t"<<vec.Py()<<"\t"<<vec.Pz()<<"\t"<<vec.E()<<"\t"<<vec.Pt()<<std::endl;
      
    }
    /*

    if(printDebug) std::cout<<"Reco Jet 4-vector "<<std::endl;
    if(printDebug) std::cout<<"pT = "<<jt.Pt()<<std::endl;
    if(printDebug) std::cout<<"eta = "<<jt.Eta()<<std::endl;
    if(printDebug) std::cout<<"phi = "<<jt.Phi()<<std::endl;

    if(doSubtraction){

      std::list<HepMC::GenParticle *>::const_iterator fiter;
      for (fiter = scatcenterparticles.begin(); fiter != scatcenterparticles.end(); ++fiter) {
	if (verbosity > 1) (*fiter)->print();
	scattcenterpx = (*fiter)->momentum().px();
	scattcenterpy = (*fiter)->momentum().py();
	scattcenterpz = (*fiter)->momentum().pz();
	scattcenterp = sqrt(scattcenterpx*scattcenterpx+scattcenterpy*scattcenterpy+scattcenterpz*scattcenterpz);
	scattcenterenergy = (*fiter)->momentum().e();

	TLorentzVector vec;
	vec.SetPxPyPzE(scattcenterpx,scattcenterpy,scattcenterpz,scattcenterenergy);      
	scattcenterpt = sqrt(scattcenterpx*scattcenterpx+scattcenterpy*scattcenterpy);
	scattcenterphi = vec.Phi();
	scattcentereta = vec.Eta();

	bool isinJet = false;	  
	// double _delRSmall = 100;
	// double _delRposition = 0;
	int counter = 0;

	// if(printDebug) std::cout<<"Truth Jet constituents loop "<<std::endl;

	//! loop over all the constituents in the truthjet
	for (Jet::ConstIter citer = jet->begin_comp(); citer != jet->end_comp(); ++citer) {
	  Jet::SRC source = citer->first;
	  unsigned int index = citer->second;
	  if (source != Jet::CEMC_TOWER) {
	    if(printDebug) std::cout << PHWHERE << " RECO jet contains something other than towers !" << std::endl;
	    exit(-1);
	  }

	  RawTower* recojet_constituent = towers->getTower(index);
	  double tower_energy = recojet_constituent->get_energy();
	  double tower_eta = recojet_constituent->get_bineta();
	  double tower_theta = 2.*TMath::ATan((TMath::Exp(-1.*tower_eta)));
	  double tower_pt = tower_energy*TMath::Sin(tower_theta);
	  double tower_phi = recojet_constituent->get_binphi();
	  // double recojet_constituentp = sqrt(recojet_constituentpx*recojet_constituentpx+
	  // 					recojet_constituentpy*recojet_constituentpy+
	  // 					recojet_constituentpz*recojet_constituentpz);
	  
	  TLorentzVector vecj;
	  vecj.SetPtEtaPhiE(tower_pt,tower_eta,tower_phi,tower_energy);
	      
	  // double recojet_constituentpt = sqrt(recojet_constituentpx*recojet_constituentpx+
	  // 				     recojet_constituentpy*recojet_constituentpy);
	  double recojet_constituentphi = vecj.Phi();
	  double recojet_constituenteta = vecj.Eta();

	  const double _delRTruth = deltaR(scattcentereta, recojet_constituenteta,
					   scattcenterphi, recojet_constituentphi);
	  // if(printDebug) std::cout<<"_delRTruth = " << _delRTruth<<std::endl;
	  // const double _delRJet = deltaR(recojeteta, recojet_constituenteta,
	  // 				 recojetphi, recojet_constituentphi);

	  // if(_delRJet < 0.6){
	  
	  //   if(printDebug) std::cout<<"scattering center = ("<< scattcentereta <<", "<<scattcenterphi<<"), jet constituent = ("<<
	  //     recojet_constituenteta<<", "<<recojet_constituentphi<<")"<<std::endl;
	  //   if(printDebug) std::cout<<"_delRTruth = " << _delRTruth<<std::endl;
	  //   if(printDebug) std::cout<<"********"<<std::endl;
	  // }
	  
	  if(_delRTruth < 1e-5){
	    // _delRSmall = _delRTruth;
	    // _delRposition = counter;
	    isinJet = true;
	  }
	  counter++;
	}// jet constituents loop
	
	// sum the background contribution
	if(isinJet){

	  bkg += vec;
	  
	}

      }//! scattering center loop 

      if(printDebug) std::cout<<"Total BKG in RECO Jet "<<std::endl;
      if(printDebug) std::cout<<"pT = "<<bkg.Pt()<<std::endl;
      if(printDebug) std::cout<<"eta = "<<bkg.Eta()<<std::endl;
      if(printDebug) std::cout<<"phi = "<<bkg.Phi()<<std::endl;
      
      
      jt_sub = jt - bkg;
      
    } else {
      jt_sub = jt;
    }
    
    if(printDebug) std::cout<<"BKG subtracted RECO Jet 4-vector "<<std::endl;
    if(printDebug) std::cout<<"pT = "<<jt_sub.Pt()<<std::endl;
    if(printDebug) std::cout<<"eta = "<<jt_sub.Eta()<<std::endl;
    if(printDebug) std::cout<<"phi = "<<jt_sub.Phi()<<std::endl;

    */
    
    if(truthjet){
      truthjetid = truthjet->get_id();
      truthjetp = truthjet->get_p();
      truthjetphi = truthjet->get_phi();
      truthjeteta = truthjet->get_eta();
      truthjetpt = truthjet->get_pt();
      truthjetenergy = truthjet->get_e();
      truthjetpx = truthjet->get_px();
      truthjetpy = truthjet->get_py();
      truthjetpz = truthjet->get_pz();
    }
    else{
      truthjetid = 0;
      truthjetp = 0;
      truthjetphi = 0;
      truthjeteta = 0;
      truthjetpt = 0;
      truthjetenergy = 0;
      truthjetpx = 0;
      truthjetpy = 0;
      truthjetpz = 0;
      
    }
    recojettree->Fill();

  }

  if(printDebug) std::cout<<"Filled reco jet tree"<<std::endl;

  nevents++;
  tree->Fill();

  if(printDebug) std::cout<<"End of event analysis "<<std::endl;
  
  return 0;

}


int JEWELTest::End(PHCompositeNode *topnode)
{
  if(printDebug) std::cout<<" DONE PROCESSING "<<std::endl;

  file->Write();
  file->Close();
  return 0;
}


void JEWELTest::Set_Tree_Branches()
{

  cluster_tree = new TTree("clustertree","A tree with EMCal cluster information");
  cluster_tree->Branch("clus_energy",&clus_energy,"clus_energy/F");
  cluster_tree->Branch("clus_eta",&clus_eta,"clus_eta/F");
  cluster_tree->Branch("clus_phi",&clus_phi,"clus_phi/F");
  cluster_tree->Branch("clus_pt",&clus_pt,"clus_pt/F");
  cluster_tree->Branch("clus_theta",&clus_theta,"clus_theta/F");
  cluster_tree->Branch("clus_x",&clus_x,"clus_x/F");
  cluster_tree->Branch("clus_y",&clus_y,"clus_y/F");
  cluster_tree->Branch("clus_z",&clus_z,"clus_z/F");
  cluster_tree->Branch("clus_t",&clus_t,"clus_t/F");
  cluster_tree->Branch("clus_px",&clus_px,"clus_px/F");
  cluster_tree->Branch("clus_py",&clus_py,"clus_py/F");
  cluster_tree->Branch("clus_pz",&clus_pz,"clus_pz/F");
  cluster_tree->Branch("nevents",&nevents,"nevents/I");

  // tracktree = new TTree("tracktree","a tree with svtx tracks");
  // tracktree->Branch("tr_px",&tr_px,"tr_px/F");
  // tracktree->Branch("tr_py",&tr_py,"tr_py/F");
  // tracktree->Branch("tr_pz",&tr_pz,"tr_pz/F");
  // tracktree->Branch("tr_pt",&tr_pt,"tr_pt/F");
  // tracktree->Branch("tr_phi",&tr_phi,"tr_phi/F");
  // tracktree->Branch("tr_eta",&tr_eta,"tr_eta/F");
  // tracktree->Branch("charge",&charge,"charge/I");
  // tracktree->Branch("chisq",&chisq,"chisq/F");
  // tracktree->Branch("ndf",&ndf,"ndf/I");
  // tracktree->Branch("dca",&dca,"dca/F");
  // tracktree->Branch("tr_x",&tr_x,"tr_x/F");
  // tracktree->Branch("tr_y",&tr_y,"tr_y/F");
  // tracktree->Branch("tr_z",&tr_z,"tr_z/F");
  // tracktree->Branch("nevents",&nevents,"nevents/i");
  // tracktree->Branch("truthtrackpx",&truthtrackpx,"truthtrackpx/F");
  // tracktree->Branch("truthtrackpy",&truthtrackpy,"truthtrackpy/F");
  // tracktree->Branch("truthtrackpz",&truthtrackpz,"truthtrackpz/F");
  // tracktree->Branch("truthtrackp",&truthtrackp,"truthtrackp/F");
  // tracktree->Branch("truthtracke",&truthtracke,"truthtracke/F");
  // tracktree->Branch("truthtrackpt",&truthtrackpt,"truthtrackpt/F");
  // tracktree->Branch("truthtrackphi",&truthtrackphi,"truthtrackphi/F");
  // tracktree->Branch("truthtracketa",&truthtracketa,"truthtracketa/F");
  // tracktree->Branch("truthtrackpid",&truthtrackpid,"truthtrackpid/I");
  // tracktree->Branch("truth_is_primary",&truth_is_primary,"truth_is_primary/B");

  truthjettree = new TTree("truthjettree","a tree with truth jets");
  truthjettree->Branch("truthjetpt",&truthjetpt,"truthjetpt/F");
  truthjettree->Branch("truthjetpx",&truthjetpx,"truthjetpx/F");
  truthjettree->Branch("truthjetpy",&truthjetpy,"truthjetpy/F");
  truthjettree->Branch("truthjetpz",&truthjetpz,"truthjetpz/F");
  truthjettree->Branch("truthjetphi",&truthjetphi,"truthjetphi/F");
  truthjettree->Branch("truthjeteta",&truthjeteta,"truthjeteta/F");
  truthjettree->Branch("truthjetmass",&truthjetmass,"truthjetmass/F");
  truthjettree->Branch("truthjetp",&truthjetp,"truthjetp/F");
  truthjettree->Branch("truthjetenergy",&truthjetenergy,"truthjetenergy/F");
  truthjettree->Branch("truthjet_bkgsub_pt",&truthjet_bkgsub_pt,"truthjet_bkgsub_pt/F");
  truthjettree->Branch("truthjet_bkgsub_px",&truthjet_bkgsub_px,"truthjet_bkgsub_px/F");
  truthjettree->Branch("truthjet_bkgsub_py",&truthjet_bkgsub_py,"truthjet_bkgsub_py/F");
  truthjettree->Branch("truthjet_bkgsub_pz",&truthjet_bkgsub_pz,"truthjet_bkgsub_pz/F");
  truthjettree->Branch("truthjet_bkgsub_phi",&truthjet_bkgsub_phi,"truthjet_bkgsub_phi/F");
  truthjettree->Branch("truthjet_bkgsub_eta",&truthjet_bkgsub_eta,"truthjet_bkgsub_eta/F");
  truthjettree->Branch("truthjet_bkgsub_mass",&truthjet_bkgsub_mass,"truthjet_bkgsub_mass/F");
  truthjettree->Branch("truthjet_bkgsub_p",&truthjet_bkgsub_p,"truthjet_bkgsub_p/F");
  truthjettree->Branch("truthjet_bkgsub_energy",&truthjet_bkgsub_energy,"truthjet_bkgsub_energy/F");
  truthjettree->Branch("nevents",&nevents,"nevents/I");

  recojettree = new TTree("recojettree","a tree with reco jets");
  recojettree->Branch("recojetpt",&recojetpt,"recojetpt/F");
  recojettree->Branch("recojetpx",&recojetpx,"recojetpx/F");
  recojettree->Branch("recojetpy",&recojetpy,"recojetpy/F");
  recojettree->Branch("recojetpz",&recojetpz,"recojetpz/F");
  recojettree->Branch("recojetphi",&recojetphi,"recojetphi/F");
  recojettree->Branch("recojeteta",&recojeteta,"recojeteta/F");
  recojettree->Branch("recojetmass",&recojetmass,"recojetmass/F");
  recojettree->Branch("recojetp",&recojetp,"recojetp/F");
  recojettree->Branch("recojetenergy",&recojetenergy,"recojetenergy/F");
  recojettree->Branch("recojet_bkgsub_pt",&recojet_bkgsub_pt,"recojet_bkgsub_pt/F");
  recojettree->Branch("recojet_bkgsub_px",&recojet_bkgsub_px,"recojet_bkgsub_px/F");
  recojettree->Branch("recojet_bkgsub_py",&recojet_bkgsub_py,"recojet_bkgsub_py/F");
  recojettree->Branch("recojet_bkgsub_pz",&recojet_bkgsub_pz,"recojet_bkgsub_pz/F");
  recojettree->Branch("recojet_bkgsub_phi",&recojet_bkgsub_phi,"recojet_bkgsub_phi/F");
  recojettree->Branch("recojet_bkgsub_eta",&recojet_bkgsub_eta,"recojet_bkgsub_eta/F");
  recojettree->Branch("recojet_bkgsub_mass",&recojet_bkgsub_mass,"recojet_bkgsub_mass/F");
  recojettree->Branch("recojet_bkgsub_p",&recojet_bkgsub_p,"recojet_bkgsub_p/F");
  recojettree->Branch("recojet_bkgsub_energy",&recojet_bkgsub_energy,"recojet_bkgsub_energy/F");
  recojettree->Branch("nevents",&nevents,"nevents/I");
  recojettree->Branch("recojetid",&recojetid,"recojetid/F");
  recojettree->Branch("truthjetid",&truthjetid,"truthjetid/F");
  recojettree->Branch("truthjetp",&truthjetp,"truthjetp/F");
  recojettree->Branch("truthjetphi",&truthjetphi,"truthjetphi/F");
  recojettree->Branch("truthjeteta",&truthjeteta,"truthjeteta/F");
  recojettree->Branch("truthjetpt",&truthjetpt,"truthjetpt/F");
  recojettree->Branch("truthjetenergy",&truthjetenergy,"truthjetenergy/F");
  recojettree->Branch("truthjetpx",&truthjetpx,"truthjetpx/F");
  recojettree->Branch("truthjetpy",&truthjetpy,"truthjetpy/F");
  recojettree->Branch("truthjetpz",&truthjetpz,"truthjetpz/F");

  truth_g4particles = new TTree("truthtree","a tree with all truth particles");
  truth_g4particles->Branch("truthpx",&truthpx,"truthpx/F");
  truth_g4particles->Branch("truthpy",&truthpy,"truthpy/F");
  truth_g4particles->Branch("truthpz",&truthpz,"truthpz/F");
  truth_g4particles->Branch("truthp",&truthp,"truthp/F");
  truth_g4particles->Branch("truthenergy",&truthenergy,"truthenergy/F");
  truth_g4particles->Branch("truthphi",&truthphi,"truthphi/F");
  truth_g4particles->Branch("trutheta",&trutheta,"trutheta/F");
  truth_g4particles->Branch("truthpt",&truthpt,"truthpt/F");
  truth_g4particles->Branch("truthpid",&truthpid,"truthpid/I");
  truth_g4particles->Branch("nevents",&nevents,"nevents/I");

  scattcenter = new TTree("scattcentertree","a tree with all scattcenter particles");
  scattcenter->Branch("scattcenterpx",&scattcenterpx,"scattcenterpx/F");
  scattcenter->Branch("scattcenterpy",&scattcenterpy,"scattcenterpy/F");
  scattcenter->Branch("scattcenterpz",&scattcenterpz,"scattcenterpz/F");
  scattcenter->Branch("scattcenterp",&scattcenterp,"scattcenterp/F");
  scattcenter->Branch("scattcenterenergy",&scattcenterenergy,"scattcenterenergy/F");
  scattcenter->Branch("scattcenterphi",&scattcenterphi,"scattcenterphi/F");
  scattcenter->Branch("scattcentereta",&scattcentereta,"scattcentereta/F");
  scattcenter->Branch("scattcenterpt",&scattcenterpt,"scattcenterpt/F");
  scattcenter->Branch("nevents",&nevents,"nevents/I");

  finalstate = new TTree("finalstatetree","a tree with all finalstate particles");
  finalstate->Branch("finalstatepx",&finalstatepx,"finalstatepx/F");
  finalstate->Branch("finalstatepy",&finalstatepy,"finalstatepy/F");
  finalstate->Branch("finalstatepz",&finalstatepz,"finalstatepz/F");
  finalstate->Branch("finalstatep",&finalstatep,"finalstatep/F");
  finalstate->Branch("finalstateenergy",&finalstateenergy,"finalstateenergy/F");
  finalstate->Branch("finalstatephi",&finalstatephi,"finalstatephi/F");
  finalstate->Branch("finalstateeta",&finalstateeta,"finalstateeta/F");
  finalstate->Branch("finalstatept",&finalstatept,"finalstatept/F");
  finalstate->Branch("nevents",&nevents,"nevents/I");
  
}



