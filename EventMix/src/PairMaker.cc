
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

//#include <g4cemc/RawClusterContainer.h>
//#include <g4cemc/RawCluster.h>
//#include <g4cemc/RawTowerContainer.h>
//#include <g4cemc/RawTower.h>
//#include "g4cemc/RawTowerGeomContainer_Cylinderv1.h"

#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHRandomSeed.h>
#include <fun4all/Fun4AllReturnCodes.h>

//#include <g4main/PHG4TruthInfoContainer.h>
//#include <g4main/PHG4Particle.h>
//#include <g4main/PHG4VtxPoint.h>

//#include <Geant4/G4ParticleTable.hh>
//#include <Geant4/G4ParticleDefinition.hh>

#include "sPHElectronv1.h"
#include "sPHElectronPairv1.h"

#include <gsl/gsl_rng.h>

using namespace std;

//==============================================================

PairMaker::PairMaker(const std::string &name, const std::string &filename) : SubsysReco(name)
{
  OutputNtupleFile=NULL;
  OutputFileName=filename;
  EventNumber=0;
  htest=NULL;
}

//==============================================================

int PairMaker::Init(PHCompositeNode *topNode) 
{

  OutputNtupleFile = new TFile(OutputFileName.c_str(),"RECREATE");
  std::cout << "PairMaker::Init: output file " << OutputFileName.c_str() << " opened." << endl;

  htest =  new TH1D("htest","",160,4.,12.);

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

  int howoften = 1; 
  if((EventNumber-1)%howoften==0) { 
    cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  }
  
  if(EventNumber==1) topNode->print();

  vector<sPHElectronv1> electrons;
  vector<sPHElectronv1> positrons;

  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) {
    cerr << PHWHERE << " ERROR: Can not find SvtxTrackMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "   Number of tracks = " << trackmap->size() << endl;

  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;
    double px = track->get_px();
    double py = track->get_py();
    double pt = sqrt(px*px + py*py);
    int charge = track->get_charge();
    cout << "   track: " << pt << endl;

    sPHElectronv1 tmpel = sPHElectronv1(track);
    if(charge==-1) electrons.push_back(tmpel);
    if(charge==1)  positrons.push_back(tmpel);

/*
    double m_tr_pz = track->get_pz();
    double m_tr_p = sqrt(m_tr_px * m_tr_px + m_tr_py * m_tr_py + m_tr_pz * m_tr_pz);
    double m_tr_e = sqrt(m_tr_p * m_tr_p + 0.000511 * 0.000511);
//    double m_tr_phi = track->get_phi();
    double m_tr_eta = track->get_eta();
    double m_charge = track->get_charge();
//    double m_chisq = track->get_chisq();
//    double m_ndf = track->get_ndf();
//    double m_dca = track->get_dca();
    double emce = track->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);
    double dphi = track->get_cal_dphi(SvtxTrack::CAL_LAYER::CEMC);
    double deta = track->get_cal_deta(SvtxTrack::CAL_LAYER::CEMC);
    double e3x3 = track->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
    double e5x5 = track->get_cal_energy_5x5(SvtxTrack::CAL_LAYER::CEMC);
      if(m_tr_pt<2.0) continue;
      std::cout << "track: " << m_tr_pt << " " << m_tr_eta << " " << m_charge << " " << emce << " " << e3x3  << std::endl;
      if(!isnan(e3x3) && e3x3!=0.) heop3x3->Fill(e3x3/m_tr_p);
      if(!isnan(e5x5) && e5x5!=0.) heop5x5->Fill(e5x5/m_tr_p);
      if(!isnan(emce) && emce!=0.) heop->Fill(emce/m_tr_p);
      if(isnan(e3x3) || e3x3/m_tr_p < 0.5) continue; // electron
      hdphi->Fill(dphi);
      hdeta->Fill(deta);
      TLorentzVector tmp = TLorentzVector(m_tr_px,m_tr_py,m_tr_pz,m_tr_e);
      if(m_charge>0) positrons.push_back(tmp);
      if(m_charge<0) electrons.push_back(tmp);
*/
  }


  for(long unsigned int i=0; i<electrons.size(); i++) {
  for(long unsigned int j=0; j<positrons.size(); j++) {
    sPHElectronPairv1 pair = sPHElectronPairv1(&electrons[i],&positrons[j]);
    double mass = pair.get_mass();
    cout << "MASS = " << mass << endl;
  }}


  return Fun4AllReturnCodes::EVENT_OK;
} 

//======================================================================

int PairMaker::End(PHCompositeNode *topNode) 
{
  OutputNtupleFile->Write();
  OutputNtupleFile->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}


