
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
#include <trackbase_historic/TrackSeed.h>
#include <trackbase_historic/TrackSeed_v1.h>
#include <trackbase_historic/SvtxTrackSeed_v1.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrDefs.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>
#include <calobase/RawTowerv2.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <calobase/RawTowerGeomContainer.h>

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

  ntppid = new  TNtuple("ntppid","","charge:pt:eta:mom:nmvtx:ntpc:chi2:ndf:cemc_ecore:cemc_e3x3:cemc_prob:cemc_chi2:cemc_dphi:cemc_deta:quality:dca2d:hcalin_e3x3:hcalout_e3x3:gdist:cemc-dphi_3x3:cemc_deta_3x3:hcalin_dphi:hcalin_deta:hcalout_dphi:hcalout_deta:hcalin_clusdphi:hcalin_clusdeta:hcalin_cluse:hcalout_clusdphi:hcalout_clusdeta:hcalout_cluse");

  ntp1 = new  TNtuple("ntp1","","type:mass:pt:eta:pt1:pt2:e3x31:e3x32:emce1:emce2:p1:p2:chisq1:chisq2:dca2d1:dca2d2:dca3dxy1:dca3dxy2:dca3dz1:dcz3dz2:mult:rap:nmvtx1:nmvtx2:ntpc1:ntpc2");

  ntpmc1 = new TNtuple("ntpmc1","","charge:pt:eta");
  ntpmc2 = new TNtuple("ntpmc2","","type:mass:pt:eta:rap:pt1:pt2:eta1:eta2");

  ntp2   = new TNtuple("ntp2","",  "type:mass:pt:eta:rap:pt1:pt2:eta1:eta2:mult:emult:pmult:chisq1:chisq2:dca2d1:dca2d2:eop3x3_1:eop3x3_2:dretaphi:nmvtx1:nmvtx2:nintt1:nintt2:ntpc1:ntpc2:b");

  ntpb = new TNtuple("ntpb","","bimp:mult:truemult:cemcmult:evtno:evtno5:ginacc:ngood");

  ntp_notracking = new TNtuple("ntp_notracking","","mass:pt:pt1:pt2:hoe1:hoe2");
  h_notracking_etabins_em =  new TH1D("h_notracking_etabins_em","",200,-0.5,199.5);
  h_notracking_phibins_em =  new TH1D("h_notracking_phibins_em","",400,-0.5,399.5);
  h_notracking_etabins =  new TH1D("h_notracking_etabins","",100,-0.5,99.5);
  h_notracking_phibins =  new TH1D("h_notracking_phibins","",100,-0.5,99.5);
  h_notracking_etabinsout =  new TH1D("h_notracking_etabinsout","",100,-0.5,99.5);
  h_notracking_phibinsout =  new TH1D("h_notracking_phibinsout","",100,-0.5,99.5);
  h_notracking_eoh =  new TH1D("h_notracking_eoh","",10000,0.0,1.0);

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

  test = new TH1D("test","",1000,0.,50.);

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
  } else if(_whattodo==3) {
        return process_event_upsilons(topNode);
  } else if(_whattodo==4) {
      return process_event_notracking(topNode);
  } else if(_whattodo==5) {
      return process_event_filtered(topNode);
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
    test->Fill(npart);
    
    bool einacc = false;
    bool pinacc = false;
    bool einacc2 = false;
    bool pinacc2 = false;
    double jpsipt = 0.;
    double jpsieta = 0.;
    //double jpsimass = 0.;
    double jpsirap = 0.;
    double pt1 = 0.;
    double pt2 = 0.;
    double eta1 = 0.;
    double eta2 = 0.;
    HepMC::GenVertex* jpsidecay_vtx = NULL;

// Find Upsilon or J/psi
    for (HepMC::GenEvent::particle_const_iterator p = event->particles_begin(); p != event->particles_end(); ++p)
    {
      int pid = (*p)->pdg_id();
      int status = (*p)->status();
      double pt = ((*p)->momentum()).perp();
      double pz = ((*p)->momentum()).pz();
      double mass  = ((*p)->momentum()).m();
      double eta  = ((*p)->momentum()).eta();
      double ee = ((*p)->momentum()).e();
      double rap = 0.5*TMath::Log((ee+pz)/(ee-pz));
      
      if(abs(pid)==553 && status==2) {
        cout << "Upsilon: " << pid << " " << status << " " << pt << " " << mass << " " << eta << endl;
        HepMC::GenVertex* upsvtx = (*p)->production_vertex();
        cout << "   Upsilon production Z vertex = " << upsvtx->point3d().z() << endl; 
      }

      if(abs(pid)==443 && status==2) {
        cout << "J/psi: " << pid << " " << status << " " << pt << " " << mass << " " << eta << endl;
        jpsidecay_vtx = (*p)->end_vertex();
        jpsipt = pt;
        jpsieta = eta;
        jpsirap = rap;
      }

    }

// Find decay electrons
    for (HepMC::GenEvent::particle_const_iterator p = event->particles_begin(); p != event->particles_end(); ++p)
    {
      int pid = (*p)->pdg_id();
      int status = (*p)->status();
      double pt = ((*p)->momentum()).perp();
//      double pz = ((*p)->momentum()).pz();
      double mass  = ((*p)->momentum()).m();
      double eta  = ((*p)->momentum()).eta();
//      double ee = ((*p)->momentum()).e();
//      double rap = 0.5*ln((ee+pz)/(ee-pz));

      if(pid==-11  && status==1 && pt>0.0) {
        //HepMC::GenParticle* parent = GetParent(*p, event);
        //int parentid = 0; if(parent) {parentid = parent->pdg_id();}
        //cout << "      parent id = " << parentid << endl;
        HepMC::GenVertex* elevtx = (*p)->production_vertex();
        if(elevtx == jpsidecay_vtx) {
          cout << "electron from J/psi: " << pid << " " << status << " " << pt << " " << mass << " " << eta << endl;
          if(fabs(eta)<1.0) einacc = true;
          if(fabs(eta)<1.0 && pt>2.0) einacc2 = true;
          pt1 = pt;
          eta1 = eta;
        }
      }
      if(pid==11  && status==1 && pt>0.0) {
        HepMC::GenVertex* posvtx = (*p)->production_vertex();
        if(posvtx == jpsidecay_vtx) {
          cout << "positroni form J/psi: " << pid << " " << status << " " << pt << " " << mass << " " << eta << endl;
          if(fabs(eta)<1.0) pinacc = true;
          if(fabs(eta)<1.0 && pt>2.0) pinacc2 = true;
          pt2 = pt;
          eta2 = eta;
        }
      }
    }

    float type = 0.;
    if(einacc && pinacc) { type = 1.; }
    if(einacc2 && pinacc2) { type = 2.; }

//  ntpmc2 = new TNtuple("ntpmc2","","type:mass:pt:eta:rap:pt1:pt2:eta1:eta2");
    float tmp[99];
    tmp[0] = type;
    tmp[1] = 0.;
    tmp[2] = jpsipt;
    tmp[3] = jpsieta;
    tmp[4] = jpsirap;
    tmp[5] = pt1;
    tmp[6] = pt2;
    tmp[7] = eta1;
    tmp[8] = eta2;
       ntpmc2->Fill(tmp);


  return Fun4AllReturnCodes::EVENT_OK;
}

//=====================================================================

int sPHAnalysis::process_event_notracking(PHCompositeNode *topNode) {

  EventNumber++;
  int howoften = 1;
  if((EventNumber-1)%howoften==0) {
    cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  }
  float tmp1[99];

  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  //cout << "Number of GlobalVertexMap entries = " << global_vtxmap->size() << endl;
  double Zvtx = 0.;
  for (GlobalVertexMap::Iter iter = global_vtxmap->begin(); iter != global_vtxmap->end(); ++iter)
  {
    GlobalVertex *vtx = iter->second;
    if(vtx->get_id()==1) Zvtx = vtx->get_z(); // BBC vertex
    //cout << "Global vertex: " << vtx->get_id() << " " << vtx->get_z() << " " << vtx->get_t() << endl;
  }
  cout << "Global vertex Z = " << Zvtx << endl;

  RawClusterContainer* cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!cemc_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_CEMC node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "   Number of CEMC clusters = " << cemc_clusters->size() << endl;

  RawTowerGeomContainer* _geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if(!_geomEM) std::cerr<<"No TOWERGEOM_CEMC"<<std::endl;
  RawTowerGeomContainer* _geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  if(!_geomIH) std::cerr<<"No TOWERGEOM_HCALIN"<<std::endl;
  RawTowerGeomContainer* _geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!_geomIH) std::cerr<<"No TOWERGEOM_HCALIN"<<std::endl;

  RawTowerContainer* _towersRawEM = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  if (!_towersRawEM) std::cerr<<"No TOWER_CALIB_CEMC Node"<<std::endl;
  RawTowerContainer* _towersRawIH = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  if (!_towersRawIH) std::cerr<<"No TOWER_CALIB_HCALIN Node"<<std::endl;
  RawTowerContainer* _towersRawOH = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  if (!_towersRawOH) std::cerr<<"No TOWER_CALIB_HCALOUT Node"<<std::endl;

  vector<TLorentzVector> electrons;
  vector<double> vhoe;

    // loop over CEMC clusters with E > 2 GeV 
    RawClusterContainer::Range begin_end = cemc_clusters->getClusters();
    RawClusterContainer::Iterator iter;
    for (iter = begin_end.first; iter != begin_end.second; ++iter)
    {
      RawCluster* cluster = iter->second;
//      if(!cluster) { cout << "ERROR: bad cluster pointer = " << cluster << endl; continue; }
//      else {
        double cemc_ecore = cluster->get_ecore();
        if(cemc_ecore<2.0) continue;
        double cemc_x = cluster->get_x();
        double cemc_y = cluster->get_y();
        double cemc_z = cluster->get_z() - Zvtx; // correct for event vertex position
        //double cemc_r = cluster->get_r();
        double cemc_r = sqrt(pow(cemc_x,2)+pow(cemc_y,2));
        double cemc_rr = sqrt(pow(cemc_r,2)+pow(cemc_z,2));
        double cemc_eta = asinh( cemc_z / cemc_r );
        double cemc_phi = atan2( cemc_y, cemc_x );
        double cemc_px = cemc_ecore * (cemc_x/cemc_rr);
        double cemc_py = cemc_ecore * (cemc_y/cemc_rr);
        double cemc_pz = cemc_ecore * (cemc_z/cemc_rr);
        //cout << "CEMC cluster: " << cemc_ecore << " " << cemc_eta << " " << cemc_phi << endl;

        // find closest CEMC tower in eta-phi space
          double distem = 99999.;
          RawTower* thetowerem = nullptr;
          RawTowerContainer::ConstRange begin_end_rawEM = _towersRawEM->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end_rawEM.first; rtiter != begin_end_rawEM.second; ++rtiter) {
            RawTower *tower = rtiter->second;
            RawTowerGeom *tower_geom = _geomEM->get_tower_geometry(tower->get_key());
            double cemc_tower_phi  = tower_geom->get_phi();
            double cemc_tower_x = tower_geom->get_center_x();
            double cemc_tower_y = tower_geom->get_center_y();
            double cemc_tower_z = tower_geom->get_center_z() - Zvtx; // correct for event vertex
            double cemc_tower_r = sqrt(pow(cemc_tower_x,2)+pow(cemc_tower_y,2));
            double cemc_tower_eta = asinh( cemc_tower_z / cemc_tower_r );
            double tmpdist = sqrt(pow(cemc_eta-cemc_tower_eta,2)+pow(cemc_phi-cemc_tower_phi,2));
            if(tmpdist<distem) { distem = tmpdist; thetowerem = tower; }
          }
          RawTowerGeom *thetower_geom_em = _geomEM->get_tower_geometry(thetowerem->get_key());
          unsigned int ietaem = thetower_geom_em->get_bineta();
          unsigned int jphiem = thetower_geom_em->get_binphi();
          h_notracking_etabins_em->Fill(double(ietaem));
          h_notracking_phibins_em->Fill(double(jphiem));



          // find closest HCALIN tower in eta-phi space
          double distin = 99999.;
          RawTower* thetowerin = nullptr;
          RawTowerContainer::ConstRange begin_end_rawIN = _towersRawIH->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end_rawIN.first; rtiter != begin_end_rawIN.second; ++rtiter) {
            RawTower *tower = rtiter->second;
            RawTowerGeom *tower_geom = _geomIH->get_tower_geometry(tower->get_key());
            double hcalin_tower_phi  = tower_geom->get_phi();
            double hcalin_tower_x = tower_geom->get_center_x();
            double hcalin_tower_y = tower_geom->get_center_y();
            double hcalin_tower_z = tower_geom->get_center_z() - Zvtx; // correct for event vertex
            double hcalin_tower_r = sqrt(pow(hcalin_tower_x,2)+pow(hcalin_tower_y,2));
            double hcalin_tower_eta = asinh( hcalin_tower_z / hcalin_tower_r );
            double tmpdist = sqrt(pow(cemc_eta-hcalin_tower_eta,2)+pow(cemc_phi-hcalin_tower_phi,2));
            if(tmpdist<distin) { distin = tmpdist; thetowerin = tower; }
          }
          RawTowerGeom *thetower_geom = _geomIH->get_tower_geometry(thetowerin->get_key());
          unsigned int ieta = thetower_geom->get_bineta();
          unsigned int jphi = thetower_geom->get_binphi();
          h_notracking_etabins->Fill(double(ieta));
          h_notracking_phibins->Fill(double(jphi));

          // Calcuate 3x3 energy deposit in HCALIN. Inner HCAL has 24 bins in eta and 64 bins in phi
          double e3x3in = 0.;
          if(ieta<1 || ieta>22) continue; // ignore clusters in edge towers
          for(unsigned int i=0; i<=2; i++) {
            for(unsigned int j=0; j<=2; j++) {
              unsigned int itmp = ieta-1+i;
              unsigned int jtmp = 0;  
              if(jphi==0 && j==0) { jtmp = 63; } // wrap around
              else if(jphi==63 && j==2) { jtmp = 0; } // wrap around
              else { jtmp = jphi-1+j; } 
              RawTower* tmptower = _towersRawIH->getTower(itmp,jtmp);
              if(tmptower) { e3x3in += tmptower->get_energy(); }
            }
          }
          h_notracking_eoh->Fill(e3x3in/cemc_ecore);

          // find closest HCALOUT tower in eta-phi space
          double distout = 99999.;
          RawTower* thetowerout = nullptr;
          RawTowerContainer::ConstRange begin_end_rawON = _towersRawOH->getTowers();
          for (RawTowerContainer::ConstIterator rtiter = begin_end_rawON.first; rtiter != begin_end_rawON.second; ++rtiter) {
            RawTower *tower = rtiter->second;
            RawTowerGeom *tower_geom = _geomOH->get_tower_geometry(tower->get_key());
            double hcalout_tower_phi  = tower_geom->get_phi();
            double hcalout_tower_x = tower_geom->get_center_x();
            double hcalout_tower_y = tower_geom->get_center_y();
            double hcalout_tower_z = tower_geom->get_center_z() - Zvtx; // correct for event vertex
            double hcalout_tower_r = sqrt(pow(hcalout_tower_x,2)+pow(hcalout_tower_y,2));
            double hcalout_tower_eta = asinh( hcalout_tower_z / hcalout_tower_r );
            double tmpdist = sqrt(pow(cemc_eta-hcalout_tower_eta,2)+pow(cemc_phi-hcalout_tower_phi,2));
            if(tmpdist<distout) { distout = tmpdist; thetowerout = tower; }
          }
          RawTowerGeom *thetower_geomout = _geomOH->get_tower_geometry(thetowerout->get_key());
          unsigned int ietaout = thetower_geomout->get_bineta();
          unsigned int jphiout = thetower_geomout->get_binphi();
          h_notracking_etabinsout->Fill(double(ietaout));
          h_notracking_phibinsout->Fill(double(jphiout));

          double e3x3out = 0.;
          if(ietaout<1 || ietaout>22) continue; // ignore clusters in edge towers
          for(unsigned int i=0; i<=2; i++) {
            for(unsigned int j=0; j<=2; j++) {
              unsigned int itmp = ietaout-1+i;
              unsigned int jtmp = 0;
              if(jphiout==0 && j==0) { jtmp = 63; } // wrap around
              else if(jphiout==63 && j==2) { jtmp = 0; } // wrap around
              else { jtmp = jphiout-1+j; }
              RawTower* tmptower = _towersRawOH->getTower(itmp,jtmp);
              if(tmptower) { e3x3out += tmptower->get_energy(); }
            }
          }


          if(e3x3in/cemc_ecore>0.06) continue; // reject hadrons, 90% eID efficiency is with 0.058 cut

          TLorentzVector tmp = TLorentzVector(cemc_px,cemc_py,cemc_pz,cemc_ecore);
          electrons.push_back(tmp);
          vhoe.push_back(e3x3in/cemc_ecore);

//      } // valid CEMC cluster
    } // end loop over CEMC clusters
    cout << "number of selected electrons = " << electrons.size() << " " << vhoe.size() << endl;

// Make Upsilons

  if(electrons.size()>=2) {
    for(long unsigned int i=0; i<electrons.size()-1; i++) {
      for(long unsigned int j=i+1; j<electrons.size(); j++) {
        TLorentzVector pair = electrons[i]+electrons[j];
        cout << i << " " << j << endl;
        tmp1[0] = pair.M();
        tmp1[1] = pair.Pt();
        cout << pair.M() << " " << pair.Pt() << endl;
        tmp1[2] = (electrons[i]).Pt();
        tmp1[3] = (electrons[j]).Pt();
        cout << vhoe[i] << " " << vhoe[j] << endl;
        tmp1[4] = vhoe[i];
        tmp1[5] = vhoe[j];
        cout << "filling..." << endl;
          ntp_notracking->Fill(tmp1);
        cout << "done." << endl;
      }
    }
  }

  cout << "returning..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;

}

//======================================================================

double sPHAnalysis::Get_CAL_e3x3(SvtxTrack* track, RawTowerContainer* _towersRawOH, RawTowerGeomContainer* _geomOH, int what, double Zvtx, double &dphi, double &deta) {

double e3x3 = 0.;
double pathlength = 999.;
vector<double> proj;
for (SvtxTrack::StateIter stateiter = track->begin_states(); stateiter != track->end_states(); ++stateiter)
{
  SvtxTrackState *trackstate = stateiter->second;
  if(trackstate) { proj.push_back(trackstate->get_pathlength()); }
}
if(what==0)      { pathlength = proj[proj.size()-3]; } // CEMC
else if(what==1) { pathlength = proj[proj.size()-2]; } // HCALIN
else if(what==2) { pathlength = proj[proj.size()-1]; } // HCALOUT
else { dphi = 9999.; deta = 9999.; return e3x3;}

  double track_eta = 999.;
  double track_phi = 999.;
  SvtxTrackState* trackstate = track->get_state(pathlength);
  if(trackstate) {
    double track_x = trackstate->get_x();
    double track_y = trackstate->get_y();
    double track_z = trackstate->get_z() - Zvtx;
    double track_r = sqrt(track_x*track_x+track_y*track_y);
    track_eta = asinh( track_z / track_r );
    track_phi = atan2( track_y, track_x );
  } else { cout << "track state not found!" << endl; dphi = 9999.; deta = 9999.; return e3x3; }

  double dist = 9999.;
  RawTower* thetower = nullptr;
  RawTowerContainer::ConstRange begin_end_rawOH = _towersRawOH->getTowers();
  for (RawTowerContainer::ConstIterator rtiter = begin_end_rawOH.first; rtiter != begin_end_rawOH.second; ++rtiter) {
    RawTower *tower = rtiter->second;
    RawTowerGeom *tower_geom = _geomOH->get_tower_geometry(tower->get_key());
    //double tower_phi  = tower_geom->get_phi();
    double tower_x = tower_geom->get_center_x();
    double tower_y = tower_geom->get_center_y();
    double tower_z = tower_geom->get_center_z() - Zvtx; // correct for event vertex
    double tower_r = sqrt(pow(tower_x,2)+pow(tower_y,2));
    double tower_eta = asinh( tower_z / tower_r );
    double tower_phi = atan2( tower_y , tower_x );
    double tmpdist = sqrt(pow(track_eta-tower_eta,2)+pow(track_phi-tower_phi,2));
    if(tmpdist<dist) { dist = tmpdist; thetower = tower; deta = fabs(track_eta-tower_eta); dphi = fabs(track_phi-tower_phi); }
  }
  cout << "dist: " << dist << " " << deta << " " << dphi << endl;

  if(!thetower) { dphi = 9999.; deta = 9999.; return e3x3; }
  RawTowerGeom *thetower_geom = _geomOH->get_tower_geometry(thetower->get_key());
  unsigned int ieta = thetower_geom->get_bineta();
  unsigned int jphi = thetower_geom->get_binphi();

  unsigned int maxbinphi = 63; if(what==0) maxbinphi = 255;
  unsigned int maxbineta = 23; if(what==0) maxbineta = 93;
    // calculate e3x3
    for(unsigned int i=0; i<=2; i++) {
      for(unsigned int j=0; j<=2; j++) {
        unsigned int itmp = ieta-1+i;
        unsigned int jtmp = 0;
        if(jphi==0 && j==0) { jtmp = maxbinphi; }      // wrap around
        else if(jphi==maxbinphi && j==2) { jtmp = 0; } // wrap around
        else { jtmp = jphi-1+j; }
        if(itmp>=0 && itmp<=maxbineta) { 
          RawTower* tmptower = _towersRawOH->getTower(itmp,jtmp);
          if(tmptower) { e3x3 += tmptower->get_energy(); }
        }
      }
    }

  return e3x3;
}

//======================================================================

int sPHAnalysis::process_event_filtered(PHCompositeNode *topNode) {
EventNumber++;

  cout << "Event # " << EventNumber << endl;

  SvtxTrackMap* _trackmap = nullptr;
  TrackSeedContainer* _trackseedcontainer_svtx = nullptr;
  TrackSeedContainer* _trackseedcontainer_silicon = nullptr;
  TrackSeedContainer* _trackseedcontainer_tpc = nullptr;
  TrkrClusterContainer* _trkrclusters = nullptr;
  RawClusterContainer* _cemc_clusters = nullptr;

  _trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!_trackmap) { cerr << PHWHERE << "ERROR: SvtxTrackMap node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }
  cout << "   Number of tracks = " << _trackmap->size() << endl;

  _trkrclusters  = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if(!_trkrclusters) { cerr << PHWHERE << "ERROR: TRKR_CLUSTER node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }
  cout << "   Number of TRKR clusters = " << _trkrclusters->size() << endl;

  _trackseedcontainer_svtx = findNode::getClass<TrackSeedContainer>(topNode, "SvtxTrackSeedContainer");
  if(!_trackseedcontainer_svtx) { cerr << PHWHERE << "ERROR: SvtxTrackSeedContainer node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _trackseedcontainer_silicon = findNode::getClass<TrackSeedContainer>(topNode, "SiliconTrackSeedContainer");
  if(!_trackseedcontainer_silicon) { cerr << PHWHERE << "ERROR: SiliconTrackSeedContainer node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _trackseedcontainer_tpc = findNode::getClass<TrackSeedContainer>(topNode, "TpcTrackSeedContainer");
  if(!_trackseedcontainer_tpc) { cerr << PHWHERE << "ERROR: TpcTrackSeedContainer node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!_cemc_clusters) { cerr << PHWHERE << "ERROR: CLUSTER_CEMC node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }
  cout << "   Number of CEMC clusters = " << _cemc_clusters->size() << endl;

  return 0;
}



//======================================================================

int sPHAnalysis::process_event_test(PHCompositeNode *topNode) {

  int evtno = EventNumber;
  int evtno5 = evtno%5;

  EventNumber++;
  int howoften = 1; 
  if((EventNumber-1)%howoften==0) { 
    cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  }
  //if(EventNumber==1) topNode->print();
  float tmp1[99];
  float tmpb[99];

  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  //cout << "Number of GlobalVertexMap entries = " << global_vtxmap->size() << endl;
  double Zvtx = 0.;
  for (GlobalVertexMap::Iter iter = global_vtxmap->begin(); iter != global_vtxmap->end(); ++iter)
  {
    GlobalVertex *vtx = iter->second;
    if(vtx->get_id()==1) Zvtx = vtx->get_z(); // BBC vertex
    //cout << "Global vertex: " << vtx->get_id() << " " << vtx->get_z() << " " << vtx->get_t() << endl;
  }
  cout << "Global BBC vertex Z = " << Zvtx << endl;

// TRUTH ------------------------------------------------------------------------
  float impact_parameter = 999.;
  int npart = 0;
  int ginacc = 0;
/*

  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  PHHepMCGenEvent *genevt = nullptr;
  if(!genevtmap) {cout << "NO PHHepMCGenEventMap node found!" << endl;

  for (PHHepMCGenEventMap::ReverseIter iter = genevtmap->rbegin(); iter != genevtmap->rend(); ++iter)
  {
    genevt = iter->second;
    if(genevt->get_embedding_id()==0) break;
    if(!genevt) {cout<<"ERROR: no PHHepMCGenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
  }

  HepMC::GenEvent *event = genevt->getEvent();
  if (!event) { cout << PHWHERE << "ERROR: no HepMC::GenEvent!" << endl; return Fun4AllReturnCodes::ABORTEVENT;}
  npart = event->particles_size();
  int nvert = event->vertices_size();
  cout << "HepMC::GenEvent: Number of particles, vertuces = " << npart << " " << nvert << endl;

  HepMC::HeavyIon* hi = event->heavy_ion();
  if(hi) { 
    impact_parameter = hi->impact_parameter(); 
    cout << "HepMC::GenEvent: impact parameter = " << impact_parameter << endl;
  }

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
      else if(fabs(gflavor)==2212) { gmass = 0.93827; } 
      else { continue; }
        double gpx = g4particle->get_px();
        double gpy = g4particle->get_py();
        double gpz= g4particle->get_pz();
        double gpt = sqrt(gpx*gpx+gpy*gpy);
        double ge = sqrt(gpt*gpt + gpz*gpz + gmass * gmass);
        TLorentzVector tmp = TLorentzVector(gpx,gpy,gpz,ge);
        if(gpt>0.5 && fabs(tmp.Eta())<1.0) { ginacc++; }

      int trackid = g4particle->get_track_id();
      if(trackid>truth_container->GetNumPrimaryVertexParticles()-50) { //embedded particles are the last ones
      //if(trackid>truth_container->GetNumPrimaryVertexParticles()-2) { // Quarkonia
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

//    TLorentzVector ele = gparticles[0];
//    TLorentzVector pos = gparticles[1];
//    TLorentzVector jpsi = ele + pos;;
//    cout << "Embedded J/psi: " << jpsi.Pt() << " " << jpsi.Eta() << endl; 
*/

// RECO -------------------------------------------------------------------

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
  cout << "   Number of CEMC clusters = " << cemc_clusters->size() << endl;
  int cemcmult = cemc_clusters->size();

  RawClusterContainer* hcalin_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_HCALIN");
  if(!hcalin_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_HCALIN node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  RawClusterContainer* hcalout_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_HCALOUT");
  if(!hcalout_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_HCALOUT node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  RawTowerGeomContainer* _geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if(!_geomEM) std::cerr<<"No TOWERGEOM_CEMC"<<std::endl;
  RawTowerGeomContainer* _geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  if(!_geomIH) std::cerr<<"No TOWERGEOM_HCALIN"<<std::endl;
  RawTowerGeomContainer* _geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!_geomOH) std::cerr<<"No TOWERGEOM_HCALIN"<<std::endl;

  RawTowerContainer* _towersEM = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  if (!_towersEM) std::cerr<<"No TOWER_CALIB_CEMC Node"<<std::endl;
  RawTowerContainer* _towersIH = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  if (!_towersIH) std::cerr<<"No TOWER_CALIB_HCALIN Node"<<std::endl;
  RawTowerContainer* _towersOH = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  if (!_towersOH) std::cerr<<"No TOWER_CALIB_HCALOUT Node"<<std::endl;
  
//------------------------------------------------------------------------------------------
/*
    RawClusterContainer::Range begin_end = cemc_clusters->getClusters();
    RawClusterContainer::Iterator iter;
    for (iter = begin_end.first; iter != begin_end.second; ++iter)
    {
      RawCluster* cluster = iter->second;
      if(!cluster) { cout << "ERROR: bad cluster pointer = " << cluster << endl; continue; }
      else {
        double cemc_ecore = cluster->get_ecore();
        //double cemc_e = cluster->get_energy();
        //double cemc_phi = cluster->get_phi();
        double cemc_x = cluster->get_x();
        double cemc_y = cluster->get_y();
        double cemc_z = cluster->get_z();
        double cemc_r = cluster->get_r();
        double cemc_eta = asinh( cemc_z / cemc_r );
        double cemc_phi = atan2( cemc_y, cemc_x );
      }
    }
*/

//------------------------------------------------------------------------------------------

int ngood = 0;
cout << "starting loop over tracks..." << endl;
  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;
    if(!track) { cout << "ERROR: bad track pointer = " << track << endl; continue; }

    double charge = track->get_charge();
    double px = track->get_px();
    double py = track->get_py();
    double pz = track->get_pz();
    double pt = sqrt(px * px + py * py);
    double mom = sqrt(px * px + py * py + pz * pz);
    double eta = track->get_eta();
    //double phi = track->get_phi();
    double chisq = track->get_chisq();
    double ndf = track->get_ndf();
    double chi2 = 999.;
      if(ndf!=0.) chi2 = chisq/ndf;
    double dca3d_xy = track->get_dca3d_xy();
    if(pt>0.5 && chi2<5.) { ngood++; }
      if(pt<2.0) continue;

// Find matching CEMC cluster
    double cemc_dphi = 99999.;
    double cemc_deta = 99999.;
    RawCluster* clus = MatchClusterCEMC(track,cemc_clusters, cemc_dphi, cemc_deta, Zvtx, 1);
    double cemc_ecore = 0.;
    double cemc_prob = 0.;
    double cemc_chi2 = 0.;
    if(clus) {
      cemc_ecore = clus->get_ecore();
      cemc_prob = clus->get_prob();
    }

// Calculate e3x3 for calorimeters
    double cemc_deta2 = 9999.;
    double cemc_dphi2 = 9999.;
    double cemc_e3x3 = Get_CAL_e3x3(track, _towersEM, _geomEM, 0, Zvtx, cemc_dphi2, cemc_deta2);
    double hcalin_deta = 9999.;
    double hcalin_dphi = 9999.;
    double hcalin_e3x3 = Get_CAL_e3x3(track, _towersIH, _geomIH, 1, Zvtx, hcalin_dphi, hcalin_deta);
    double hcalout_deta = 9999.;
    double hcalout_dphi = 9999.;
    double hcalout_e3x3 = Get_CAL_e3x3(track, _towersOH, _geomOH, 2, Zvtx, hcalout_dphi, hcalout_deta);

    double hcalin_clusdphi = 9999.;
    double hcalin_clusdeta = 9999.;
    double hcalout_clusdphi = 9999.;
    double hcalout_clusdeta = 9999.;
    RawCluster* clus_hcalin = MatchClusterCEMC(track, hcalin_clusters, hcalin_clusdphi, hcalin_clusdeta, Zvtx, 2);
    RawCluster* clus_hcalout = MatchClusterCEMC(track, hcalout_clusters, hcalout_clusdphi, hcalout_clusdeta, Zvtx, 3);
    double hcalin_cluse = clus_hcalin->get_energy();
    double hcalout_cluse = clus_hcalout->get_energy();

    cout << "track: " << charge << " " << pt << "    " << cemc_ecore << " " << cemc_e3x3 << " " << hcalin_e3x3 << " " << hcalout_e3x3 << endl;

// count hits
    int nmvtx = 0; int nintt = 0; int ntpc = 0;
    auto siseed = track->get_silicon_seed();
    if(siseed) {
      for (auto iter = siseed->begin_cluster_keys(); iter != siseed->end_cluster_keys(); ++iter) {
        TrkrDefs::cluskey cluster_key = *iter;
        auto trkrid = TrkrDefs::getTrkrId(cluster_key);
        if(trkrid==TrkrDefs::mvtxId) nmvtx++;
        if(trkrid==TrkrDefs::inttId) nintt++;
        //int layer = TrkrDefs::getLayer(cluster_key);
        //if(0<=layer && layer<=2) nmvtx++;
        //if(3<=layer && layer<=6) nintt++;
      }
    }
    auto tpcseed = track->get_tpc_seed();
    if(tpcseed) {
      for (auto iter = tpcseed->begin_cluster_keys(); iter != tpcseed->end_cluster_keys(); ++iter) {
        TrkrDefs::cluskey cluster_key = *iter;
        int layer = TrkrDefs::getLayer(cluster_key);
        if(layer>6) ntpc++;
      }
    }
    cout << "    ntpc, nmvtx, nitt= " << ntpc << " " << nmvtx << " " << nintt << endl;
      
// print all track states
/*
  vector<double> proj;
  for (SvtxTrack::StateIter stateiter = track->begin_states(); stateiter != track->end_states(); ++stateiter)
  {
    SvtxTrackState *trackstate = stateiter->second;
    if(trackstate) { 
      cout << "state: " << trackstate->get_pathlength() << endl; 
      proj.push_back(trackstate->get_pathlength());
    } 
  }
  //cout << "projection radii = " << proj[proj.size()-3] << " " << proj[proj.size()-2] << " " << proj[proj.size()-1] << endl;
*/
    double gdist = 999.;
/*    
    for(unsigned int i=0; i<gparticles.size(); i++) {
      double gphi = gparticles[i].Phi();
      double geta = gparticles[i].Eta();
      double tmpdist = sqrt(pow(phi-gphi,2)+pow(eta-geta,2));
      if(tmpdist<gdist) { gdist = tmpdist; }
    }
    cout << "gdist: " << gdist << endl;
//    if(gdist>0.001) continue;
*/

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
    tmp1[17] = hcalout_e3x3;
    tmp1[18] = gdist;
    tmp1[19] = cemc_dphi2;
    tmp1[20] = cemc_dphi2;
    tmp1[21] = hcalin_dphi;
    tmp1[22] = hcalin_deta;
    tmp1[23] = hcalout_deta;
    tmp1[24] = hcalout_deta;
    tmp1[25] = hcalin_clusdphi;
    tmp1[26] = hcalin_clusdeta;
    tmp1[27] = hcalin_cluse;
    tmp1[28] = hcalout_clusdphi;
    tmp1[29] = hcalout_clusdeta;
    tmp1[30] = hcalout_cluse;
      ntppid->Fill(tmp1);

  } // end loop over tracks

//  ntpb = new TNtuple("ntpb","","bimp:mult:truemult:cemcmult");
  tmpb[0] = impact_parameter;
  tmpb[1] = mult;
  tmpb[2] = npart;
  tmpb[3] = cemcmult;
  tmpb[4] = float(evtno);
  tmpb[5] = float(evtno5);
  tmpb[6] = float(ginacc);
  tmpb[7] = float(ngood);
    ntpb->Fill(tmpb);

  return Fun4AllReturnCodes::EVENT_OK;
}
//================================================================================

TVector3 sPHAnalysis::GetProjectionToCalorimeter(SvtxTrack* track, int what) {
// what = 1 CEMC
// what = 2 HCALIN
// what = 3 HCALOUT

  TVector3 projection; // 0,0,0

  vector<double> proj;
  for (SvtxTrack::StateIter stateiter = track->begin_states(); stateiter != track->end_states(); ++stateiter)
  {
    SvtxTrackState *trackstate = stateiter->second;
    if(trackstate) { proj.push_back(trackstate->get_pathlength()); }
  }
  double pathlength = proj[proj.size()+4-what]; // CEMC is next next to last, usually 93.5

  SvtxTrackState* trackstate = track->get_state(pathlength); // at CEMC inner face
  if(trackstate) {
    double track_x = trackstate->get_x();
    double track_y = trackstate->get_y();
    double track_z = trackstate->get_z();
    projection.SetX(track_x);
    projection.SetY(track_y);
    projection.SetZ(track_z);
  }

  return projection;
}

//=================================================================================

RawCluster* sPHAnalysis::MatchClusterCEMC(SvtxTrack* track, RawClusterContainer* cemc_clusters, double &dphi, double &deta, double Zvtx, int what) {

  RawCluster* returnCluster = NULL;
  double track_eta = 99999.;
  double track_phi = 99999.;
  dphi = 99999.;
  deta = 99999.;

  vector<double> proj;
  for (SvtxTrack::StateIter stateiter = track->begin_states(); stateiter != track->end_states(); ++stateiter)
  {
    SvtxTrackState *trackstate = stateiter->second;
    if(trackstate) { proj.push_back(trackstate->get_pathlength()); }
  }
  double pathlength = 0.;
  if(what==1) {
    pathlength = proj[proj.size()-3]; // CEMC is next next to last
  } else if(what==2) {
    pathlength = proj[proj.size()-2]; // HCALIN is next to last
  } else if(what==3) {
    pathlength = proj[proj.size()-1]; // HCALOUT is the last
  } else {return returnCluster; }

  SvtxTrackState* trackstate = track->get_state(pathlength); // at CEMC inner face
  if(trackstate) {
    double track_x = trackstate->get_x();
    double track_y = trackstate->get_y();
    double track_z = trackstate->get_z() - Zvtx;
    double track_r = sqrt(track_x*track_x+track_y*track_y);
    track_eta = asinh( track_z / track_r );
    track_phi = atan2( track_y, track_x );
  } else { return returnCluster; }

  if(track_eta == 99999. || track_phi == 99999.) { return returnCluster; }
  double dist = 99999.;

    RawClusterContainer::Range begin_end = cemc_clusters->getClusters();
    RawClusterContainer::Iterator iter;
    for (iter = begin_end.first; iter != begin_end.second; ++iter)
    {
      RawCluster* cluster = iter->second;
      if(!cluster) continue;
      else {
        double cemc_ecore = cluster->get_ecore();
        if(cemc_ecore<0.0) continue;
        double cemc_x = cluster->get_x();
        double cemc_y = cluster->get_y();
        double cemc_z = cluster->get_z() - Zvtx;
        double cemc_r = cluster->get_r();
        double cemc_eta = asinh( cemc_z / cemc_r );
        double cemc_phi = atan2( cemc_y, cemc_x );
        double tmpdist = sqrt(pow((cemc_eta-track_eta),2)+pow((cemc_phi-track_phi),2));
        if(tmpdist<dist) {
          dist = tmpdist; returnCluster = cluster; dphi = fabs(cemc_phi-track_phi); deta = fabs(cemc_eta-track_eta);
        }
      }
    }

  return returnCluster;
}


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

  SvtxVertexMap *vtxmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!vtxmap) {
      cout << "SvtxVertexMap node not found!" << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "Number of SvtxVertexMap entries = " << vtxmap->size() << endl;

  double Zvtx = 0.;
  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!global_vtxmap) {
    cerr << PHWHERE << " ERROR: Can not find GlobalVertexMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  for (GlobalVertexMap::Iter iter = global_vtxmap->begin(); iter != global_vtxmap->end(); ++iter)
  { GlobalVertex *vtx = iter->second; if(vtx->get_id()==1) { Zvtx = vtx->get_z(); } }
  cout << "Global BBC vertex Z = " << Zvtx << endl;

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
    double px = track->get_px();
    double py = track->get_py();
    double pz = track->get_pz();
    double pt = sqrt(px * px + py * py);
    double mom = sqrt(pt*pt+pz*pz);
    double ee = sqrt(mom*mom+0.000511*0.000511);
      //if(pt<0.5) continue;
      if(pt<2.0) continue;
    //double phi = track->get_phi();
    double eta = track->get_eta();
    double chisq = track->get_chisq();
    double ndf = track->get_ndf();
    double chi2 = 999.;
    double dca3d_xy = track->get_dca3d_xy(); if(ndf!=0.) chi2 = chisq/ndf;

    double cemc_dphi = 99999.;
    double cemc_deta = 99999.;
    RawCluster* clus = MatchClusterCEMC(track, cemc_clusters, cemc_dphi, cemc_deta, Zvtx, 1);
    double cemc_ecore = 0.;
    if(clus) cemc_ecore = clus->get_ecore();
    if(cemc_ecore/mom<0.7) continue; // not an electron
    double trk_x = track->get_x();
    double trk_y = track->get_y();
    double trk_z = track->get_z();
    cout << "track: " << charge << " " << pt << " " << cemc_ecore/mom << " " << trk_x << " " << trk_y << " " << trk_z << endl;

    int nmvtx = 0; int nintt = 0; int ntpc = 0;
    auto siseed = track->get_silicon_seed();
    for (auto iter = siseed->begin_cluster_keys(); iter != siseed->end_cluster_keys(); ++iter) {
      TrkrDefs::cluskey cluster_key = *iter;
      //int layer = TrkrDefs::getLayer(cluster_key);
      auto trkrid = TrkrDefs::getTrkrId(cluster_key);
      if(trkrid==TrkrDefs::mvtxId) nmvtx++;
      if(trkrid==TrkrDefs::inttId) nintt++;
      //if(0<=layer && layer<=2) nmvtx++;
      //if(3<=layer && layer<=6) nintt++;
    }
    auto tpcseed = track->get_tpc_seed();
    for (auto iter = tpcseed->begin_cluster_keys(); iter != tpcseed->end_cluster_keys(); ++iter) {
      TrkrDefs::cluskey cluster_key = *iter;
      int layer = TrkrDefs::getLayer(cluster_key);
      if(layer>6) ntpc++;
    }

      if(charge<0) std::cout << "electron: " << pt << " " << eta << " " << nmvtx << " " << ntpc << " " << charge << " " << cemc_ecore << std::endl;
      if(charge>0) std::cout << "positron: " << pt << " " << eta << " " << nmvtx << " " << ntpc << " " << charge << " " << cemc_ecore << std::endl;
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

// make pairs ----------------------------------------------------------------------------------

  for(long unsigned int i=0; i<electrons.size(); i++) {
  for(long unsigned int j=0; j<positrons.size(); j++) {
    TLorentzVector pair = electrons[i]+positrons[j];
    double mass = pair.M();
    hmass->Fill(mass);
    cout << "Upsilon mass = " << pair.M() << endl; 
    //if(mass<5.0) continue;
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
    //if(mass<5.0) continue;
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
    //if(mass<5.0) continue;
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


