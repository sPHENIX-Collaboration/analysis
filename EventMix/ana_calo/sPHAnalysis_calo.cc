#include "sPHAnalysis_calo.h"

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "TFile.h"
#include "TNtuple.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "TRandom2.h"

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>
#include <calobase/RawTowerv2.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>
#include <calobase/RawTowerGeomContainer.h>

#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHRandomSeed.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <gsl/gsl_rng.h>

using namespace std;

//==============================================================

sPHAnalysis_calo::sPHAnalysis_calo(const std::string &name, const std::string &filename) : SubsysReco(name)
{
  OutputNtupleFile=nullptr;
  OutputFileName=filename;
  EventNumber=0;
  ntp_notracking=nullptr;
  h_mult=nullptr;
  h_ecore= nullptr;
  _whattodo = 0;
  _rng = nullptr;
}

//==============================================================

int sPHAnalysis_calo::Init(PHCompositeNode *topNode)
{
  std::cout << "sPHAnalysis_calo::Init started..." << endl;
  OutputNtupleFile = new TFile(OutputFileName.c_str(),"RECREATE");
  std::cout << "sPHAnalysis_calo::Init: output file " << OutputFileName.c_str() << " opened." << endl;

  ntp_notracking = new  TNtuple("ntp_notracking","","mass:pt:e1:e2:hoe1:hoe2");
  h_mult =  new TH1D("h_mult","",1000,0.,10000.);
  h_ecore =  new TH1D("h_ecore","",200,0.,20.);

  char hname[99];
  for(int i=0; i<256*96; i++) {
    sprintf(hname,"h_pedestal_%d",i);
    h_pedestal[i] = new TH1F(hname,hname,10000,-0.5,9999.5);
  }

  _rng = new TRandom2();
  _rng->SetSeed(0);

  cout << "sPHAnalysis_calo::Init ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================

int sPHAnalysis_calo::InitRun(PHCompositeNode *topNode)
{
  cout << "sPHAnalysis_calo::InitRun started..." << endl;
  cout << "sPHAnalysis_calo::InitRun ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================

int sPHAnalysis_calo::process_event(PHCompositeNode *topNode)
{
  if(_whattodo==0) {
    return process_event_test(topNode); // simulation
  } else if(_whattodo==1) {
    return process_event_data(topNode); // data
  } else {
    cerr << "ERROR: wrong choice of what to do." << endl; return Fun4AllReturnCodes::ABORTRUN; 
  }
}

//======================================================================

int sPHAnalysis_calo::process_event_data(PHCompositeNode *topNode) {
  EventNumber++;
  float tmp1[99];

  cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  if(EventNumber==1) topNode->print();

  TowerInfoContainer* _towersCEMC = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_CEMC");
  if (!_towersCEMC) { std::cerr<<"No TOWERS_CEMC Node"<<std::endl; return Fun4AllReturnCodes::ABORTEVENT; }
  unsigned int ntowers = _towersCEMC->size();
  cout << " # of towers = " << ntowers << endl;

  for (unsigned int channel = 0; channel < ntowers; channel++)
  {
    TowerInfo *cemcinfo_raw = _towersCEMC->get_tower_at_channel(channel);
    float raw_amplitude = cemcinfo_raw->get_energy();
    float raw_time = cemcinfo_raw->get_time();
    unsigned int key = _towersCEMC->encode_key(channel);
    unsigned int iphi = _towersCEMC->getTowerPhiBin(key);
    unsigned int ieta = _towersCEMC->getTowerEtaBin(key);
    h_pedestal[channel]->Fill(raw_amplitude);
    if(channel>0 && channel<20) cout << channel << " " << iphi << " " << ieta << " " << raw_amplitude << " " << raw_time << endl;
  }

  cout << "sPHAnalysis_calo::procedss_event_data() ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//======================================================================

int sPHAnalysis_calo::process_event_test(PHCompositeNode *topNode) {
  EventNumber++;
  float tmp1[99];

  cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
  if(EventNumber==1) topNode->print();

// Find event vertex
  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  //cout << "Number of GlobalVertexMap entries = " << global_vtxmap->size() << endl;
  double Zvtx = 0.;
  for (GlobalVertexMap::Iter iter = global_vtxmap->begin(); iter != global_vtxmap->end(); ++iter)
  {
    GlobalVertex *vtx = iter->second;
    if(vtx->get_id()==1) Zvtx = vtx->get_z(); // BBC vertex (?)
    //cout << "Global vertex: " << vtx->get_id() << " " << vtx->get_z() << " " << vtx->get_t() << endl;
  }
  cout << "Global vertex Z = " << Zvtx << endl;

// Find node with CEMC clusters
  RawClusterContainer* cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!cemc_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_CEMC node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  cout << "   Number of CEMC clusters = " << cemc_clusters->size() << endl;
  h_mult->Fill(cemc_clusters->size());

// Fetch CEMC geometry
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
      if(!cluster) { cout << "ERROR: bad cluster pointer = " << cluster << endl; continue; }
      else {
        double cemc_ecore = cluster->get_ecore();
        h_ecore->Fill(cemc_ecore);
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
          //h_notracking_eoh->Fill(e3x3in/cemc_ecore);

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

          // Calcuate 3x3 energy deposit in HCALOUT. Outer HCAL has 24 bins in eta and 64 bins in phi
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

      } // valid CEMC cluster
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

  cout << "sPHAnalysis_calo::procedss_event_test() ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//======================================================================

int sPHAnalysis_calo::End(PHCompositeNode *topNode)
{
  std::cout << "sPHAnalysis_calo::End() started, Number of processed events = " << EventNumber << endl;
  OutputNtupleFile->Write();
  OutputNtupleFile->Close();
  return Fun4AllReturnCodes::EVENT_OK;
}


