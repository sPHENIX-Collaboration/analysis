
#include "sPHAnalysis.h"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "TFile.h"
#include "TNtuple.h"
#include "TH1D.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TRandom2.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <trackbase/TrkrDefs.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

//#include <HepMC/GenEvent.h>              // for GenEvent::particle_const_ite...
//#include <HepMC/GenParticle.h>           // for GenParticle
#include <HepMC/GenVertex.h>             // for GenVertex, GenVertex::partic...
#include <HepMC/IteratorRange.h>         // for ancestors, children, descend...
#include <HepMC/SimpleVector.h>   // for FourVector

#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHRandomSeed.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <gsl/gsl_rng.h>

#include "/gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectron.h"
#include "/gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronv1.h"
#include "/gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPair.h"
#include "/gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPairv1.h"
#include "/gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPairContainer.h"
#include "/gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPairContainerv1.h"

using namespace std;
//using namespace HepMC;

//ofstream ofs;
//std::ofstream ascii_io( "text_hepmc.txt" );

//==============================================================

sPHAnalysis::sPHAnalysis(const std::string &name, const std::string &filename) : SubsysReco(name)
{
  OutputNtupleFile=NULL;
  OutputFileName=filename;
  EventNumber=0;
  ntp1=NULL;
  hdeta=NULL;
  hdphi=NULL;
  hmass=NULL;
  hgmass=NULL;
  hgmass0=NULL;
  hgmass09=NULL;
  heop=NULL;
  heop3x3=NULL;
  heop5x5=NULL;
  hdca2d=NULL;
  hdca3dxy=NULL;
  hdca3dz=NULL;
  hchi2=NULL;
  hndf=NULL;
  hquality=NULL;

  _whattodo = 2;
  _rng = nullptr;
}

//==============================================================

int sPHAnalysis::Init(PHCompositeNode *topNode) 
{

//  ofs.open("test.txt");

  std::cout << "sPHAnalysis::Init started..." << endl;
  OutputNtupleFile = new TFile(OutputFileName.c_str(),"RECREATE");
  std::cout << "sPHAnalysis::Init: output file " << OutputFileName.c_str() << " opened." << endl;

  ntppid = new  TNtuple("ntppid","","charge:pt:eta:mom:nmvtx:ntpc:chi2:ndf:cemc_ecore:cemc_e3x3:cemc_prob:cemc_chi2:cemc_dphi:cemc_deta:quality:dca2d:hcalin_e3x3:gdist");

  ntp1 = new  TNtuple("ntp1","","type:mass:pt:eta:pt1:pt2:e3x31:e3x32:emce1:emce2:p1:p2:chisq1:chisq2:dca2d1:dca2d2:dca3dxy1:dca3dxy2:dca3dz1:dcz3dz2:mult:rap:nmvtx1:nmvtx2:ntpc1:ntpc2");

  ntpmc1 = new TNtuple("ntpmc1","","charge:pt:eta");
  ntpmc2 = new TNtuple("ntpmc2","","type:mass:pt:eta:rap:pt1:pt2:eta1:eta2");

  ntp2   = new TNtuple("ntp2","",  "type:mass:pt:eta:rap:pt1:pt2:eta1:eta2:mult:emult:pmult:chisq1:chisq2:dca2d1:dca2d2:eop3x3_1:eop3x3_2:dretaphi:nmvtx1:nmvtx2:nintt1:nintt2:ntpc1:ntpc2:b");

  ntpb = new TNtuple("ntpb","","bimp");

  hmult =  new TH1D("hmult","",100,0.,2000.);
  hmass =  new TH1D("hmass","",160,4.,12.);
  hgmass =  new TH1D("hgmass","",160,4.,12.);
  hgmass0 =  new TH1D("hgmass0","",160,4.,12.);
  hgmass09 =  new TH1D("hgmass09","",160,4.,12.);
  heop3x3 =  new TH1D("heop3x3","",180,0.,1.8);
  heop5x5 =  new TH1D("heop5x5","",180,0.,1.8);
  heop =  new TH1D("heop","",180,0.,1.8);
  hdphi =  new TH1D("hdphi","",200,0.25,0.25);
  hdeta = new TH1D("hdeta","",200,0.25,0.25);

  hdca2d = new TH1D("hdca2d","",1000,-0.1,0.1);
  hdca3dxy = new TH1D("hdca3dxy","",1000,-0.1,0.1);
  hdca3dz = new TH1D("hdca3dz","",1000,-0.1,0.1);
  hchi2 = new TH1D("hchi2","",1000,0.,50.);
  hndf = new TH1D("hndf","",1000,0.,50.);
  hquality = new TH1D("hquality","",1000,0.,50.);

//  HepMC::write_HepMC_IO_block_begin( ascii_io );

  _rng = new TRandom2();
  _rng->SetSeed(0);

//  fsin = new TF1("fsin", "sin(x)", 0, M_PI);

  std::cout << "sPHAnalysis::Init ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================
  
int sPHAnalysis::InitRun(PHCompositeNode *topNode) 
{
  std::cout << "sPHAnalysis::InitRun started..." << std::endl;
  std::cout << "sPHAnalysis::InitRun ended." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================

int sPHAnalysis::process_event(PHCompositeNode *topNode) 
{
  //return process_event_bimp(topNode);
  //return process_event_hepmc(topNode);
  if(_whattodo==0) {
//    cout << "MAKING PAIRS." << endl;
    return process_event_pairs(topNode);
  } else if(_whattodo==1) {
//    cout << "DOING ANALYSIS." << endl;
    return process_event_test(topNode);
  } else if(_whattodo==2) {
//    cout << "PYTHIA UPSILONS." << endl;
    return process_event_pythiaupsilon(topNode);
  } else { cerr << "ERROR: wrong choice of what to do." << endl; return Fun4AllReturnCodes::ABORTRUN; }
}

//======================================================================

int sPHAnalysis::process_event_hepmc(PHCompositeNode *topNode) {
  EventNumber++;

  cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  if(EventNumber==1) topNode->print();

  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
    PHHepMCGenEvent *genevt = nullptr;

  for (PHHepMCGenEventMap::ReverseIter iter = genevtmap->rbegin(); iter != genevtmap->rend(); ++iter)
  {
    genevt = iter->second;
    if(!genevt) {cout<<"no phgenevt!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
    cout << "got PHHepMCGenEvent... " << genevt << endl;
  }

    HepMC::GenEvent *event = genevt->getEvent();
    if (!event) { cout << PHWHERE << "no genevent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
    int npart = event->particles_size();
    int nvert = event->vertices_size();
    cout << "Number of particles = " << npart << " " << nvert << endl;

/*
    for (HepMC::GenEvent::particle_const_iterator p = event->particles_begin(); p != event->particles_end(); ++p)
    {
      int pid = (*p)->pdg_id();
      int status = (*p)->status();
      double pt = ((*p)->momentum()).perp();
//      double mass  = ((*p)->momentum()).m();
//      double eta  = ((*p)->momentum()).eta();
      if(pt>2.0) cout << pid << " " << pt << " " << status << endl;
    }
*/

// generate Upsilon

   double pt = _rng->Uniform() * 10.;
   double y  = (_rng->Uniform()*2.) - 1.;
   double phi = (2.0 * M_PI) * _rng->Uniform();
   double mass = 9.3987;
   double mt = sqrt(mass * mass + pt * pt);
   double eta = asinh(sinh(y) * mt / pt);
   TLorentzVector vm;
   vm.SetPtEtaPhiM(pt, eta, phi, mass);

// decay it to electrons

    double m1 = 0.000511;
    double m2 = 0.000511;

// first electroin in Upsilon rest frame
    double E1 = (mass * mass - m2 * m2 + m1 * m1) / (2.0 * mass);
    double p1 = sqrt((mass * mass - (m1 + m2) * (m1 + m2)) * (mass * mass - (m1 - m2) * (m1 - m2))) / (2.0 * mass);
    double phi1 = _rng->Uniform() * 2. * M_PI;
    double th1  = fsin->GetRandom();
    double px1 = p1 * sin(th1) * cos(phi1);
    double py1 = p1 * sin(th1) * sin(phi1);
    double pz1 = p1 * cos(th1);
    TLorentzVector v1;
    v1.SetPxPyPzE(px1, py1, pz1, E1);

// boost to lab frame
    double betax = vm.Px() / vm.E();
    double betay = vm.Py() / vm.E();
    double betaz = vm.Pz() / vm.E();
    v1.Boost(betax, betay, betaz);

// second electron
    TLorentzVector v2 = vm - v1;    
    cout << "decay electrons: " << v1.Pt() << " " << v2.Pt() << endl;

    HepMC::FourVector fv1 = HepMC::FourVector(v1.Px(),v1.Py(),v1.Pz());
    HepMC::GenParticle gp1 = HepMC::GenParticle(fv1,-11);
    HepMC::FourVector fv2 = HepMC::FourVector(v2.Px(),v2.Py(),v2.Pz());
    HepMC::GenParticle gp2 = HepMC::GenParticle(fv2,11);

    HepMC::GenEvent* newevent = new HepMC::GenEvent(*event);

    for (HepMC::GenEvent::vertex_iterator v = newevent->vertices_begin(); v != newevent->vertices_end(); ++v)
    {
       int npin = (*v)->particles_in_size();
       int npout = (*v)->particles_out_size();
       HepMC::ThreeVector pnt = (*v)->point3d();
       cout << "event vertex: " << npin << " " << npout << " " << pnt.x() << " " << pnt.y() << " " << pnt.z() << endl;
       cout << "adding particle with id " << gp1.pdg_id() << endl;
       (*v)->add_particle_out(&gp1);
       cout << "   after adding electron1: " << (*v)->particles_out_size() << endl;
       (*v)->add_particle_out(&gp2);
       cout << "   after adding electron2: " << (*v)->particles_out_size() << endl;
       break;  // add electrons to the first vertex
    }

    PHHepMCGenEvent* newgenevt = new PHHepMCGenEvent();
    newgenevt->addEvent(newevent);

//    genevt->clearEvent();
    PHHepMCGenEvent* tmpptr = genevtmap->insert_event(1, newgenevt);
    cout << "inserted event " << tmpptr << endl;

  cout << "procedd_event_hepmc() ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//======================================================================

int sPHAnalysis::process_event_pairs(PHCompositeNode *topNode) {
  EventNumber++;
  float tmp[99];
  cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
    //if(EventNumber==1) topNode->print();

  sPHElectronPairContainerv1 *eePairs = findNode::getClass<sPHElectronPairContainerv1>(topNode,"ElectronPairs");
  cout << eePairs << endl;
  if(!eePairs) { cout << "ERROR: ElectronPairs node not found!" << endl; }
    else { cout << "Found ElectronPairs node." << endl; }

  float mult = eePairs->size();
  cout << "Number of pairs = " << eePairs->size() << endl;

  for (sPHElectronPairContainerv1::ConstIter iter = eePairs->begin(); iter != eePairs->end(); ++iter)
  {
    sPHElectronPairv1* pair = iter->second;
    sPHElectronv1* electron = (sPHElectronv1*)pair->get_first();
    sPHElectronv1* positron = (sPHElectronv1*)pair->get_second();
    tmp[0] = pair->get_type();
    tmp[1] = pair->get_mass();
    tmp[2] = pair->get_pt();
    tmp[3] = pair->get_eta();
      double px1 = electron->get_px();
      double py1 = electron->get_py();
      double pz1 = electron->get_pz();
      double px2 = positron->get_px();
      double py2 = positron->get_py();
      double pz2 = positron->get_pz();
    tmp[4] = sqrt(px1*px1+py1*py1);
    tmp[5] = sqrt(px2*px2+py2*py2);
    tmp[6] = electron->get_e3x3();
    tmp[7] = positron->get_e3x3();
    tmp[8] = electron->get_emce();
    tmp[9] = positron->get_emce();
    tmp[10] = sqrt(px1*px1+py1*py1+pz1*pz1);
    tmp[11] = sqrt(px2*px2+py2*py2+pz2*pz2);
      double chisq1 = electron->get_chi2();
      double ndf1 = (double)electron->get_ndf();
      if(ndf1!=0.) { chisq1 = chisq1/ndf1;} else {chisq1 = 99999.;}
      double chisq2 = positron->get_chi2();
      double ndf2 = (double)positron->get_ndf();
      if(ndf2!=0.) { chisq2 = chisq2/ndf2;} else {chisq2 = 99999.;}
    tmp[12] = chisq1;
    tmp[13] = chisq2;
    tmp[14] = electron->get_dca2d();
    tmp[15] = positron->get_dca2d();
    tmp[16] = electron->get_dca3d_xy();
    tmp[17] = positron->get_dca3d_xy();
    tmp[18] = electron->get_dca3d_z();
    tmp[19] = positron->get_dca3d_z();
    tmp[20] = mult;
    tmp[21] = 0.; // rapidity
    tmp[22] = positron->get_nmvtx();
    tmp[23] = electron->get_nmvtx();
    tmp[24] = positron->get_ntpc();
    tmp[25] = electron->get_ntpc();

       ntp1->Fill(tmp);
  }


  cout << "process_event_pairs() ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//============================================================================

int sPHAnalysis::process_event_bimp(PHCompositeNode *topNode) {
  EventNumber++;
  if((EventNumber-1)%10==0) {cout<<"------------------ EventNumber = " << EventNumber-1 << endl;}
  float tmp[99];

  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  PHHepMCGenEvent *genevt = nullptr;

  for (PHHepMCGenEventMap::ReverseIter iter = genevtmap->rbegin(); iter != genevtmap->rend(); ++iter)
  {
    genevt = iter->second;
    if(!genevt) {cout<<"ERROR: no PHHepMCGenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
  }

    HepMC::GenEvent *event = genevt->getEvent();
    if (!event) { cout << PHWHERE << "ERROR: no HepMC::GenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}

    HepMC::HeavyIon* hi = event->heavy_ion();
    float impact_parameter = hi->impact_parameter();
    //cout << "HepMC::GenEvent: impact parameter = " << impact_parameter << endl;

    tmp[0] = impact_parameter;
    ntpb->Fill(tmp);

  return Fun4AllReturnCodes::EVENT_OK;
}

//=============================================================================================

HepMC::GenParticle*  sPHAnalysis::GetParent(HepMC::GenParticle* p, HepMC::GenEvent* event)
{

  HepMC::GenParticle* parent = NULL;
//  if(!p->production_vertex()) return parent;

  for ( HepMC::GenVertex::particle_iterator mother = p->production_vertex()-> particles_begin(HepMC::ancestors);
        mother != p->production_vertex()-> particles_end(HepMC::ancestors);
        ++mother )
    {
          //if(abs((*mother)->pdg_id()) == 23 || abs((*mother)->pdg_id()) == 21) // Z0 or gluon
          //cout << "      mother pid = " << (*mother)->pdg_id() << endl;
          if(abs((*mother)->pdg_id()) == 553) // Upsilon
            {
              parent = *mother;
              break;
            }
      if(parent != NULL) break;
    }

  return parent;
}


//======================================================================

int sPHAnalysis::process_event_pythiaupsilon(PHCompositeNode *topNode) {
  EventNumber++;
  int howoften = 1; 
  if((EventNumber-1)%howoften==0) { cout<<"------------ EventNumber = " << EventNumber-1 << endl; }
  if(EventNumber==1) topNode->print();

  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  PHHepMCGenEvent *genevt = nullptr;

  for (PHHepMCGenEventMap::ReverseIter iter = genevtmap->rbegin(); iter != genevtmap->rend(); ++iter)
  {
    genevt = iter->second;
    if(!genevt) {cout<<"ERROR: no PHHepMCGenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
    //cout << "got PHHepMCGenEvent... " << genevt << endl;
  }
    
    HepMC::GenEvent *event = genevt->getEvent();
    if (!event) { cout << PHWHERE << "ERROR: no HepMC::GenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
    
    int npart = event->particles_size();
    int nvert = event->vertices_size();
    cout << "HepMC::GenEvent: Number of particles, vertuces = " << npart << " " << nvert << endl;
    
    for (HepMC::GenEvent::particle_const_iterator p = event->particles_begin(); p != event->particles_end(); ++p)
    {
      int pid = (*p)->pdg_id();
      int status = (*p)->status();
      double pt = ((*p)->momentum()).perp();
      double mass  = ((*p)->momentum()).m();
      double eta  = ((*p)->momentum()).eta();
      if(status==1 && pt>2.0) {cout << pid << " " << status << " " << pt << " " << mass << " " << eta << endl;}
      if(abs(pid)==553 && status==2) cout << "Upsilon: " << pid << " " << status << " " << pt << " " << mass << " " << eta << endl;
      if(abs(pid)==11  && status==1 && pt>2.0) {
        cout << "electron: " << pid << " " << status << " " << pt << " " << mass << " " << eta << endl;
        HepMC::GenParticle* parent = GetParent(*p, event);
        int parentid = 0; if(parent) {parentid = parent->pdg_id();}
        cout << "      parent id = " << parentid << endl;
      }
    }

  return Fun4AllReturnCodes::EVENT_OK;
}



//======================================================================

int sPHAnalysis::process_event_test(PHCompositeNode *topNode) {
  EventNumber++;
  int howoften = 1; 
  if((EventNumber-1)%howoften==0) { 
    cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  }
  if(EventNumber==1) topNode->print();

/*
  float impact_parameter = 0.;

  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  PHHepMCGenEvent *genevt = nullptr;

  for (PHHepMCGenEventMap::ReverseIter iter = genevtmap->rbegin(); iter != genevtmap->rend(); ++iter)
  {
    genevt = iter->second;
    if(!genevt) {cout<<"ERROR: no PHHepMCGenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
    //cout << "got PHHepMCGenEvent... " << genevt << endl;
  }

    HepMC::GenEvent *event = genevt->getEvent();
    if (!event) { cout << PHWHERE << "ERROR: no HepMC::GenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
    //int npart = event->particles_size();
    //int nvert = event->vertices_size();
    //cout << "HepMC::GenEvent: Number of particles, vertuces = " << npart << " " << nvert << endl;
    HepMC::HeavyIon* hi = event->heavy_ion();
    impact_parameter = hi->impact_parameter(); 
    cout << "HepMC::GenEvent: impact parameter = " << impact_parameter << endl;
*/

  float tmp1[99];

  PHG4TruthInfoContainer* truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truth_container) {
    cerr << PHWHERE << " ERROR: Can not find G4TruthInfo node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  PHG4TruthInfoContainer::ConstRange range = truth_container->GetPrimaryParticleRange();

  vector<TLorentzVector> gparticles;

    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter)
    {
      PHG4Particle* g4particle = iter->second;
      int gflavor  = g4particle->get_pid();
      double gmass = 0.;
      if(fabs(gflavor)==11)       { gmass = 0.000511; } 
      else if(fabs(gflavor)==211) { gmass = 0.13957; } 
      else if(fabs(gflavor)==321) { gmass = 0.49368; } 
      else { continue; }
      int trackid = g4particle->get_track_id();
      if(trackid>truth_container->GetNumPrimaryVertexParticles()-50) { //embedded particles are the last ones
      //if(trackid>truth_container->GetNumPrimaryVertexParticles()-2) {
        double gpx = g4particle->get_px();
        double gpy = g4particle->get_py();
        double gpz= g4particle->get_pz();
        double gpt = sqrt(gpx*gpx+gpy*gpy);
        //double phi = atan2(gpy,gpx);
        //double eta = asinh(gpz/gpt);
        double ge = sqrt(gpt*gpt + gpz*gpz + gmass * gmass);
        //int primid =  g4particle->get_primary_id();
        //int parentid = g4particle->get_parent_id();
        TLorentzVector tmp = TLorentzVector(gpx,gpy,gpz,ge);
        gparticles.push_back(tmp);
        //cout << "embedded: " << gflavor << " " << gpt << " " << gmass << endl;
      }
    }

    cout << "number of embedded particles = " << gparticles.size() << endl;

//-------------------------------------------------------------------

  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) {
    cerr << PHWHERE << " ERROR: Can not find SvtxTrackMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "   Number of tracks = " << trackmap->size() << endl;
  double mult = trackmap->size();
  hmult->Fill(mult);

  RawClusterContainer* cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!cemc_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_CEMC node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  else { cout << "FOUND CLUSTER_CEMC node." << endl; }

  int nmvtx = 0; int nintt = 0; int ntpc = 0;

//------------------------------------------------------------------------------------------

//cout << "starting loop over tracks..." << endl;
  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;
    if(!track) {
      cout << "ERROR: bad track pointer = " << track << endl;
      continue;
    }

    double charge = track->get_charge();
      if(charge>0.) continue;
    double px = track->get_px();
    double py = track->get_py();
    double pt = sqrt(px * px + py * py);
      if(pt<2.0) continue;
    double phi = track->get_phi();
    double eta = track->get_eta();
    
    double gdist = 999.;
    for(unsigned int i=0; i<gparticles.size(); i++) {
      double gphi = gparticles[i].Phi();
      double geta = gparticles[i].Eta();
      double tmpdist = sqrt(pow(phi-gphi,2)+pow(eta-geta,2));
      if(tmpdist<gdist) { gdist = tmpdist; }
    }
    if(gdist>0.001) continue;

    //if(!isElectron(track)) continue;

    unsigned int cemc_clusid = track->get_cal_cluster_id(SvtxTrack::CAL_LAYER::CEMC);
    double cemc_ecore = 0.;
    double cemc_prob = 99999.;
    double cemc_chi2 = 99999.;
    if(cemc_clusid<99999) {
      RawCluster* cluster = cemc_clusters->getCluster(cemc_clusid);
      if(!cluster) {
        cout << "ERROR: bad cluster pointer = " << cluster << endl;
        continue;
      }
      //cemc_e = cluster->get_energy();
      cemc_ecore = cluster->get_ecore();
      cemc_prob = cluster->get_prob();
      cemc_chi2 = cluster->get_chi2();
    }

    nmvtx = 0; nintt = 0; ntpc = 0;
    for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys();
                                        iter != track->end_cluster_keys(); ++iter)
    {
      TrkrDefs::cluskey cluster_key = *iter;
      int trackerid = TrkrDefs::getTrkrId(cluster_key);
      if(trackerid==0) nmvtx++;
      if(trackerid==1) nintt++;
      if(trackerid==2) ntpc++;
    }

    double pz = track->get_pz();
    double mom = sqrt(px * px + py * py + pz * pz);
    double chisq = track->get_chisq();
    double ndf = track->get_ndf();
    double chi2 = 999.;
      if(ndf!=0.) chi2 = chisq/ndf;
    //double dca2d = track->get_dca2d();
    double dca3d_xy = track->get_dca3d_xy();
    //double emce = track->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);
    double cemc_dphi = track->get_cal_dphi(SvtxTrack::CAL_LAYER::CEMC);
    double cemc_deta = track->get_cal_deta(SvtxTrack::CAL_LAYER::CEMC);
    double cemc_e3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
    double hcalin_e3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALIN);
    //double cemc_e5x5 = track->get_cal_energy_5x5(SvtxTrack::CAL_LAYER::CEMC);
    //double eop = 0.;
    //  if(mom!=0) eop = cemc_ecore/mom;

    tmp1[0] = charge;
    tmp1[1] = pt;
    tmp1[2] = eta;
    tmp1[3] = mom;
    tmp1[4] = nmvtx;
    tmp1[5] = ntpc;
    tmp1[6] = chisq;
    tmp1[7] = ndf;
    tmp1[8] = cemc_ecore;
    tmp1[9] = cemc_e3x3;
    tmp1[10] = cemc_prob;
    tmp1[11] = cemc_chi2;
    tmp1[12] = cemc_dphi;
    tmp1[13] = cemc_deta;
    tmp1[14] = chi2;
    tmp1[15] = dca3d_xy;
    tmp1[16] = hcalin_e3x3;
    tmp1[17] = gdist;
      ntppid->Fill(tmp1);

  } // end loop over tracks

  return Fun4AllReturnCodes::EVENT_OK;
}

//=================================================================================
//=================================================================================

int sPHAnalysis::process_event_upsilons(PHCompositeNode *topNode) {
  EventNumber++;
  int howoften = 1;
  if((EventNumber-1)%howoften==0) {
    cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  }
  if(EventNumber==1) topNode->print();

  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) {
    cerr << PHWHERE << " ERROR: Can not find SvtxTrackMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "   Number of tracks = " << trackmap->size() << endl;

  RawClusterContainer* cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!cemc_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_CEMC node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  else { cout << "FOUND CLUSTER_CEMC node." << endl; }

  vector<TLorentzVector> electrons;
  vector<TLorentzVector> positrons;
  vector<double> vpchi2;
  vector<double> vmchi2;
  vector<double> vpdca;
  vector<double> vmdca;
  vector<double> vpeop3x3;
  vector<double> vmeop3x3;
  vector<int> vpnmvtx;
  vector<int> vpnintt;
  vector<int> vpntpc;
  vector<int> vmnmvtx;
  vector<int> vmnintt;
  vector<int> vmntpc;

  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;
    if(!track) {
      cout << "ERROR: bad track pointer = " << track << endl;
      continue;
    }

    double charge = track->get_charge();
      if(charge>0.) continue;
    double px = track->get_px();
    double py = track->get_py();
    double pz = track->get_pz();
    double pt = sqrt(px * px + py * py);
    double mom = sqrt(pt*pt+pz*pz);
    double ee = sqrt(mom*mom+0.000511*0.000511);
      if(pt<2.0) continue;
    //double phi = track->get_phi();
    double eta = track->get_eta();
    double chisq = track->get_chisq();
    double ndf = track->get_ndf();
    double chi2 = 999.;
    double dca3d_xy = track->get_dca3d_xy();
      if(ndf!=0.) chi2 = chisq/ndf;
    //double cemc_dphi = track->get_cal_dphi(SvtxTrack::CAL_LAYER::CEMC);
    //double cemc_deta = track->get_cal_deta(SvtxTrack::CAL_LAYER::CEMC);
    double cemc_e3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
    //double hcalin_e3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::HCALIN);

    unsigned int cemc_clusid = track->get_cal_cluster_id(SvtxTrack::CAL_LAYER::CEMC);
    double cemc_ecore = 0.;
    //double cemc_prob = 99999.;
    //double cemc_chi2 = 99999.;
    if(cemc_clusid<99999) {
      RawCluster* cluster = cemc_clusters->getCluster(cemc_clusid);
      if(!cluster) {
        cout << "ERROR: bad cluster pointer = " << cluster << endl;
        continue;
      }
      //cemc_e = cluster->get_energy();
      cemc_ecore = cluster->get_ecore();
      //cemc_prob = cluster->get_prob();
      //cemc_chi2 = cluster->get_chi2();
    }
    if(cemc_ecore/mom<0.7) continue; // not an electron

    int nmvtx = 0; int nintt = 0; int ntpc = 0;
    for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys();
                                        iter != track->end_cluster_keys(); ++iter)
    {
      TrkrDefs::cluskey cluster_key = *iter;
      int trackerid = TrkrDefs::getTrkrId(cluster_key);
      if(trackerid==0) nmvtx++;
      if(trackerid==1) nintt++;
      if(trackerid==2) ntpc++;
    }

      if(charge<0) std::cout << "electron: " << pt << " " << eta << " " << charge << " " << cemc_ecore << " " << cemc_e3x3  << std::endl;
      if(charge>0) std::cout << "positron: " << pt << " " << eta << " " << charge << " " << cemc_ecore << " " << cemc_e3x3  << std::endl;
      TLorentzVector tmp = TLorentzVector(px,py,pz,ee);
      if(charge>0) {
        positrons.push_back(tmp); 
        vpeop3x3.push_back(cemc_ecore); vpchi2.push_back(chi2); vpdca.push_back(dca3d_xy);
        vpnmvtx.push_back(nmvtx); vpnintt.push_back(nintt); vpntpc.push_back(ntpc);
      }
      if(charge<0) {
        electrons.push_back(tmp); 
        vmeop3x3.push_back(cemc_ecore); vmchi2.push_back(chi2); vmdca.push_back(dca3d_xy);
        vmnmvtx.push_back(nmvtx); vmnintt.push_back(nintt); vmntpc.push_back(ntpc);
      }

  } // end loop over tracks

  double emult = electrons.size();
  double pmult = positrons.size();
  float tmp1[99];

  for(long unsigned int i=0; i<electrons.size(); i++) {
  for(long unsigned int j=0; j<positrons.size(); j++) {
    TLorentzVector pair = electrons[i]+positrons[j];
    double mass = pair.M();
    hmass->Fill(mass);
    if(mass<5.0) continue;
      tmp1[0] = 1.;
      tmp1[1] = pair.M();
      tmp1[2] = pair.Pt();
      tmp1[3] = pair.Eta();
      tmp1[4] = pair.Rapidity();
      tmp1[5] = positrons[j].Pt();
      tmp1[6] = electrons[i].Pt();
      tmp1[7] = positrons[j].Eta();
      tmp1[8] = electrons[i].Eta();
      tmp1[9] = trackmap->size();
      tmp1[10] = emult;
      tmp1[11] = pmult;
      tmp1[12] = vpchi2[j];
      tmp1[13] = vmchi2[i];
      tmp1[14] = vpdca[j];
      tmp1[15] = vmdca[i];
      tmp1[16] = vpeop3x3[j];
      tmp1[17] = vmeop3x3[i];
      tmp1[18] = electrons[i].DrEtaPhi(positrons[j]);
      tmp1[19] = vpnmvtx[j];
      tmp1[20] = vmnmvtx[i];
      tmp1[21] = vpnintt[j];
      tmp1[22] = vmnintt[i];
      tmp1[23] = vpntpc[j];
      tmp1[24] = vmntpc[i];
      tmp1[25] = 0.;
       ntp2->Fill(tmp1);
  }}

  if(electrons.size()>1) {
  for(long unsigned int i=0; i<electrons.size()-1; i++) {
  for(long unsigned int j=i+1; j<electrons.size(); j++) {
    TLorentzVector pair = electrons[i]+electrons[j];
    double mass = pair.M();
    hmass->Fill(mass);
    if(mass<5.0) continue;
      tmp1[0] = 3.;
      tmp1[1] = pair.M();
      tmp1[2] = pair.Pt();
      tmp1[3] = pair.Eta();
      tmp1[4] = pair.Rapidity();
      tmp1[5] = electrons[j].Pt();
      tmp1[6] = electrons[i].Pt();
      tmp1[7] = electrons[j].Eta();
      tmp1[8] = electrons[i].Eta();
      tmp1[9] = trackmap->size();
      tmp1[10] = emult;
      tmp1[11] = pmult;
      tmp1[12] = vmchi2[j];
      tmp1[13] = vmchi2[i];
      tmp1[14] = vmdca[j];
      tmp1[15] = vmdca[i];
      tmp1[16] = vmeop3x3[j];
      tmp1[17] = vmeop3x3[i];
      tmp1[18] = electrons[i].DrEtaPhi(electrons[j]);
      tmp1[19] = vmnmvtx[j];
      tmp1[20] = vmnmvtx[i];
      tmp1[21] = vmnintt[j];
      tmp1[22] = vmnintt[i];
      tmp1[23] = vmntpc[j];
      tmp1[24] = vmntpc[i];
      tmp1[25] = 0.;
       ntp2->Fill(tmp1);
  }}}

  if(positrons.size()>1) {
  for(long unsigned int i=0; i<positrons.size()-1; i++) {
  for(long unsigned int j=i+1; j<positrons.size(); j++) {
    TLorentzVector pair = positrons[i]+positrons[j];
    double mass = pair.M();
    hmass->Fill(mass);
    if(mass<5.0) continue;
      tmp1[0] = 2.;
      tmp1[1] = pair.M();
      tmp1[2] = pair.Pt();
      tmp1[3] = pair.Eta();
      tmp1[4] = pair.Rapidity();
      tmp1[5] = positrons[j].Pt();
      tmp1[6] = positrons[i].Pt();
      tmp1[7] = positrons[j].Eta();
      tmp1[8] = positrons[i].Eta();
      tmp1[9] = trackmap->size();
      tmp1[10] = emult;
      tmp1[11] = pmult;
      tmp1[12] = vpchi2[j];
      tmp1[13] = vpchi2[i];
      tmp1[14] = vpdca[j];
      tmp1[15] = vpdca[i];
      tmp1[16] = vpeop3x3[j];
      tmp1[17] = vpeop3x3[i];
      tmp1[18] = positrons[i].DrEtaPhi(positrons[j]);
      tmp1[19] = vpnmvtx[j];
      tmp1[20] = vpnmvtx[i];
      tmp1[21] = vpnintt[j];
      tmp1[22] = vpnintt[i];
      tmp1[23] = vpntpc[j];
      tmp1[24] = vpntpc[i];
      tmp1[25] = 0.;
       ntp2->Fill(tmp1);
  }}}

  return Fun4AllReturnCodes::EVENT_OK;
} 

//======================================================================

bool sPHAnalysis::isElectron(SvtxTrack* trk)
{
  double px = trk->get_px();
  double py = trk->get_py();
  double pz = trk->get_pz();
  double pt = sqrt(px*px+py*py);
  double pp = sqrt(pt*pt+pz*pz);
  double e3x3 = trk->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
  //double eclus = trk->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);
  if(pp==0.) return false;
  //if(pt<2.0) return false;
  if(pt<0.1) return false;
  //cout << e3x3 << " " << eclus << endl;
  if(isnan(e3x3)) return false;
  //if(e3x3/pp<0.70) return false;
  if(e3x3/pp<0.1) return false;
  return true;
}

//======================================================================

int sPHAnalysis::End(PHCompositeNode *topNode) 
{
  std::cout << "sPHAnalysis::End() started, Number of processed events = " << EventNumber << endl;
//   HepMC::write_HepMC_IO_block_end( ascii_io );
  std::cout << "Writing out..." << endl;
  OutputNtupleFile->Write();
  std::cout << "Closing output file..." << endl;
  OutputNtupleFile->Close();
//  ofs.close();
  return Fun4AllReturnCodes::EVENT_OK;
}


