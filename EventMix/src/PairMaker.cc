
#include "PairMaker.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "TFile.h"
#include "TH1D.h"
#include "TLorentzVector.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

//#include <g4cemc/RawClusterContainer.h>
//#include <g4cemc/RawCluster.h>
//#include <g4cemc/RawTowerContainer.h>
//#include <g4cemc/RawTower.h>
//#include "g4cemc/RawTowerGeomContainer_Cylinderv1.h"

#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHRandomSeed.h>
#include <fun4all/Fun4AllReturnCodes.h>

//#include <g4main/PHG4TruthInfoContainer.h>
//#include <g4main/PHG4Particle.h>
//#include <g4main/PHG4VtxPoint.h>

//#include <Geant4/G4ParticleTable.hh>
//#include <Geant4/G4ParticleDefinition.hh>

#include "sPHElectron.h"
#include "sPHElectronv1.h"
#include "sPHElectronPair.h"
#include "sPHElectronPairv1.h"
#include "sPHElectronPairContainer.h"
#include "sPHElectronPairContainerv1.h"

#include <gsl/gsl_rng.h>

typedef PHIODataNode<PHObject> PHObjectNode_t;

using namespace std;

//==============================================================

PairMaker::PairMaker(const std::string &name, const std::string &filename) : SubsysReco(name)
{
//  OutputNtupleFile=NULL;
//  htest=NULL;
//  OutputFileName=filename;
  outnodename = "ElectronPairs";
  EventNumber=0;
}

//==============================================================

int PairMaker::Init(PHCompositeNode *topNode) 
{

//  OutputNtupleFile = new TFile(OutputFileName.c_str(),"RECREATE");
//  std::cout << "PairMaker::Init: output file " << OutputFileName.c_str() << " opened." << endl;
//  htest =  new TH1D("htest","",160,4.,12.);

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    cerr << PHWHERE << " ERROR: Can not find DST node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  sPHElectronPairContainerv1* eePairs = new sPHElectronPairContainerv1();

  PHCompositeNode *PairNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", outnodename));
  if (!PairNode)
  {
    PHObjectNode_t *PairNode = new PHIODataNode<PHObject>(eePairs,outnodename.c_str(),"PHObject");
    dstNode->addNode(PairNode);
  }
  else { cout << PHWHERE << " INFO: " << outnodename << " already exists." << endl; }

  std::cout << "PairMaker::Init ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================
  
int PairMaker::InitRun(PHCompositeNode *topNode) 
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================

int PairMaker::process_event(PHCompositeNode *topNode) 
{
  return process_event_test(topNode);
}

//======================================================================

int PairMaker::process_event_test(PHCompositeNode *topNode) {
  EventNumber++;

//  int howoften = 1; 
//  if((EventNumber-1)%howoften==0) { 
    cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
//  }
  
  if(EventNumber==1) topNode->print();

  sPHElectronPairContainerv1 *eePairs = findNode::getClass<sPHElectronPairContainerv1>(topNode,outnodename.c_str());
  if(!eePairs) { cerr << outnodename << " not found!" << endl; } 
    else { cout << "Found " << outnodename << " node." << endl; }

  vector<sPHElectronv1> electrons;
  vector<sPHElectronv1> positrons;

  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!global_vtxmap) { 
    cerr << PHWHERE << " ERROR: Can not find GlobalVertexMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "Number of GlobalVertexMap entries = " << global_vtxmap->size() << endl;

  SvtxVertexMap *vtxmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(vtxmap) {cout << "Number of SvtxVertexMap entries = " << vtxmap->size() << endl;}
    else {cout << "SvtxVertexMap node not found!" << endl;}

  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) {
    cerr << PHWHERE << " ERROR: Can not find SvtxTrackMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "   Number of tracks = " << trackmap->size() << endl;

  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;
    if(!isElectron(track)) continue;
    double px = track->get_px();
    double py = track->get_py();
    double pt = sqrt(px*px + py*py);
    int charge = track->get_charge();
    double x = track->get_x();
    double y = track->get_y();
    double z = track->get_z();
    unsigned int vtxid = track->get_vertex_id();
    cout << "   track: " << pt << " " << x << " " << y << " " << z << " " << vtxid << endl;
    GlobalVertex* gvtx = global_vtxmap->get(vtxid); 
    cout << "   global vertex: " << gvtx->get_x() << " " << gvtx->get_y() << " " << gvtx->get_z() << endl;    
    sPHElectronv1 tmpel = sPHElectronv1(track);
    if(charge==-1) electrons.push_back(tmpel);
    if(charge==1)  positrons.push_back(tmpel);
    //double emce = track->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);
    //double e3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
  }

  for(long unsigned int i=0; i<electrons.size(); i++) {
  for(long unsigned int j=0; j<positrons.size(); j++) {
    sPHElectronPairv1 pair = sPHElectronPairv1(&electrons[i],&positrons[j]);
    pair.set_type(1);
    double mass = pair.get_mass();
    cout << "MASS = " << mass << endl;
    eePairs->insert(&pair);
  }}

  return Fun4AllReturnCodes::EVENT_OK;
} 

//======================================================================

bool PairMaker::isElectron(SvtxTrack* trk) 
{
  double px = trk->get_px();
  double py = trk->get_py();
  double pz = trk->get_pz();
  double pt = sqrt(px*px+py*py);
  double pp = sqrt(pt*pt+pz*pz);
  double e3x3 = trk->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
  if(pp==0.) return false;
  if(pt<2.0 || e3x3/pp<0.5) return false;
  return true;
}

int PairMaker::End(PHCompositeNode *topNode) 
{
//  OutputNtupleFile->Write();
//  OutputNtupleFile->Close();
  cout << "END: ====================================" << endl;
  sPHElectronPairContainerv1 *eePairs = findNode::getClass<sPHElectronPairContainerv1>(topNode,outnodename.c_str());
  cout << "Total number of pairs = " << eePairs->size() << endl;
  topNode->print();
  return Fun4AllReturnCodes::EVENT_OK;
}


