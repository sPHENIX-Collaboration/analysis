#include "CaloAna.h"


// G4Hits includes
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <TLorentzVector.h>

// G4Cells includes
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>

// Tower includes
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfoDefs.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>

// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>


#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>


#include <TFile.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TH2.h>
#include <TH1.h>

#include <Event/Event.h>
#include <Event/packet.h>
#include <cassert>
#include <sstream>
#include <string>

using namespace std;

CaloAna::CaloAna(const std::string& name, const std::string& filename)
  : SubsysReco(name)
  , detector("HCALIN")
  , outfilename(filename)
{
    _eventcounter = 0;
}

CaloAna::~CaloAna()
{
  delete hm;
  delete g4hitntuple;
  delete g4cellntuple;
  delete towerntuple;
  delete clusterntuple;
}

int CaloAna::Init(PHCompositeNode*)
{
  hm = new Fun4AllHistoManager(Name());
  // create and register your histos (all types) here

  outfile = new TFile(outfilename.c_str(), "RECREATE");
  h_emcal_mbd_correlation = new TH2F("h_emcal_mbd_correlation",";emcal;mbd",100,0,1,100,0,1);
  h_ohcal_mbd_correlation = new TH2F("h_ohcal_mbd_correlation",";ohcal;mbd",100,0,1,100,0,1);
  h_ihcal_mbd_correlation = new TH2F("h_ihcal_mbd_correlation",";ihcal;mbd",100,0,1,100,0,1);
  h_emcal_hcal_correlation = new TH2F("h_emcal_hcal_correlation",";emcal;hcal",100,0,1,100,0,1);
  h_cemc_etaphi = new TH2F("h_cemc_etaphi",";eta;phi",96,0,96,256,0,256);
  h_hcalin_etaphi = new TH2F("h_ihcal_etaphi",";eta;phi",24,0,24,64,0,64);
  h_hcalout_etaphi = new TH2F("h_ohcal_etaphi",";eta;phi",24,0,24,64,0,64);
  h_emcal_zdc_correlation = new TH2F("h_zdc_emcal_correlation",";emcal;zdc",100,0,1,100,0,1);
    
    
  h_InvMass = new TH1F("h_InvMass","Invariant Mass",120,0,1.2);
    
  hvtx_z = new TH1D("hvtx_z", "hvtx_z", 101, -50.5 , 50.5);

  //raw timing information
  hzdctime_cut = new TH1D("hzdctime_cut", "hzdctime_cut", 30, 2.5 , 32.5);
  hmbdtime_cut = new TH1D("hmbdtime_cut", "hmbdtime_cut", 30, 2.5 , 32.5);
  hemcaltime_cut = new TH1D("hemcaltime_cut", "hemcaltime_cut", 30, 2.5 , 32.5);
  hihcaltime_cut = new TH1D("hihcaltime_cut", "hihcaltime_cut", 30, 2.5 , 32.5);
  hohcaltime_cut = new TH1D("hohcaltime_cut", "hohcaltime_cut", 30, 2.5 , 32.5);


  //extracted timing information
  hzdctime = new TH1D("hzdctime", "hzdctime", 34, -1.5 , 32.5);
  hmbdtime = new TH1D("hmbdtime", "hmbdtime", 34, -1.5 , 32.5);
  hemcaltime = new TH1D("hemcaltime", "hemcaltime", 34, -1.5 , 32.5);
  hihcaltime = new TH1D("hihcaltime", "hihcaltime", 34, -1.5 , 32.5);
  hohcaltime = new TH1D("hohcaltime", "hohcaltime", 34, -1.5 , 32.5);

  return 0;
}

int CaloAna::process_event(PHCompositeNode* topNode)
{

  _eventcounter++;

  process_towers(topNode);
  
  return Fun4AllReturnCodes::EVENT_OK;
}


int CaloAna::process_towers(PHCompositeNode* topNode)
{
    
    std::cout<<_eventcounter<<std::endl;
    
    float totalcemc = 0.;
    float totalihcal = 0.;
    float totalohcal = 0.;
    float totalmbd = 0.;
    float totalzdc = 0.;
    
    float emcaldownscale = 1000000/800;
    float ihcaldownscale = 40000/300;
    float ohcaldownscale = 250000/600;
    float mbddownscale = 250000.0;
    float zdcdownscale  = 2e4;
    
    
    int max_zdc_t = -1;
    int max_emcal_t = -1;
    int max_mbd_t = -1;
    int max_ihcal_t = -1;
    int max_ohcal_t = -1;
    
    
    
    //----------------------------------get vertex------------------------------------------------------//
    
    GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
    if (!vertexmap)
    {
        std::cout << PHWHERE << " Fatal Error - GlobalVertexMap node is missing"
        << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }
    
    GlobalVertex *vtx = vertexmap->begin()->second;
    float vtx_z = NAN;
    if (vtx)
    {
        vtx_z = vtx->get_z();
    }
   
    if (abs(vtx_z) < _vz)
    {
        
     hvtx_z->Fill(vtx_z);
    
    //----------------------------------------------timing-----------------------------------------------//
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_CEMC");
        if (offlinetowers)
        {
            int size = offlinetowers->size();
            for (int channel = 0; channel < size;channel++)
            {
                hemcaltime_cut->Fill(offlinetowers->get_tower_at_channel(channel)->get_time());
            }
        }
    }
    
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_HCALIN");
        if (offlinetowers)
        {
            int size = offlinetowers->size();
            for (int channel = 0; channel < size;channel++)
            {
                hihcaltime_cut->Fill(offlinetowers->get_tower_at_channel(channel)->get_time());
            }
        }
    }
    
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_HCALOUT");
        if (offlinetowers)
        {
            int size = offlinetowers->size();
            for (int channel = 0; channel < size;channel++)
            {
                hohcaltime_cut->Fill(offlinetowers->get_tower_at_channel(channel)->get_time());
            }
        }
    }
    
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_MBD");
        if (offlinetowers)
        {
            int size = offlinetowers->size();
            for (int channel = 0; channel < size;channel++)
            {
                unsigned int towerkey = TowerInfoDefs::encode_mbd(channel);
                int type = TowerInfoDefs::get_mbd_type(towerkey);
                if (type == 1) hmbdtime_cut->Fill(offlinetowers->get_tower_at_channel(channel)->get_time());
            }
        }
    }
    
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_ZDC");
        if (offlinetowers)
        {
            int size = offlinetowers->size();
            for (int channel = 0; channel < size;channel++)
            {
                hzdctime_cut->Fill(offlinetowers->get_tower_at_channel(channel)->get_time());
            }
        }
    }
    
    
    //get time estimate
    max_zdc_t = Getpeaktime(hzdctime_cut);
    max_mbd_t = Getpeaktime(hmbdtime_cut);
    max_emcal_t = Getpeaktime(hemcaltime_cut);
    max_ihcal_t = Getpeaktime(hihcaltime_cut);
    max_ohcal_t = Getpeaktime(hohcaltime_cut);
    
    //----------------------------------------------tower energies -----------------------------------------------//
    
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_CEMC");
        if (offlinetowers)
        {
            int size = offlinetowers->size(); //online towers should be the same!
            for (int channel = 0; channel < size;channel++)
            {
                TowerInfo* offlinetower = offlinetowers->get_tower_at_channel(channel);
                float offlineenergy = offlinetower->get_energy();
                unsigned int towerkey = offlinetowers->encode_key(channel);
                int ieta = offlinetowers->getTowerEtaBin(towerkey);
                int iphi = offlinetowers->getTowerPhiBin(towerkey);
                int _time = offlinetowers->get_tower_at_channel(channel)->get_time();
                
                if(_time > (max_emcal_t - _range) && _time < (max_emcal_t + _range))
                {
                    totalcemc += offlineenergy;
                    hemcaltime->Fill(_time);
                    
                    if (offlineenergy > 1)
                    {
                        h_cemc_etaphi->Fill(ieta,iphi,offlineenergy);
                    }
                }
            }
        }
    }
    
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_HCALIN");
        if (offlinetowers)
        {
            int size = offlinetowers->size(); //online towers should be the same!
            for (int channel = 0; channel < size;channel++)
            {
                TowerInfo* offlinetower = offlinetowers->get_tower_at_channel(channel);
                float offlineenergy = offlinetower->get_energy();
                unsigned int towerkey = offlinetowers->encode_key(channel);
                int ieta = offlinetowers->getTowerEtaBin(towerkey);
                int iphi = offlinetowers->getTowerPhiBin(towerkey);
                int _time = offlinetowers->get_tower_at_channel(channel)->get_time();
                
                if(_time > (max_ihcal_t - _range) && _time < (max_ihcal_t + _range))
                {
                    totalihcal += offlineenergy;
                    hihcaltime->Fill(_time);
                    
                    if (offlineenergy > 1)
                    {
                        h_hcalin_etaphi->Fill(ieta,iphi,offlineenergy);
                    }
                }
                
            }
        }
    }
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_HCALOUT");
        if (offlinetowers)
        {
            int size = offlinetowers->size(); //online towers should be the same!
            for (int channel = 0; channel < size;channel++)
            {
                TowerInfo* offlinetower = offlinetowers->get_tower_at_channel(channel);
                float offlineenergy = offlinetower->get_energy();
                unsigned int towerkey = offlinetowers->encode_key(channel);
                int ieta = offlinetowers->getTowerEtaBin(towerkey);
                int iphi = offlinetowers->getTowerPhiBin(towerkey);
                int _time = offlinetowers->get_tower_at_channel(channel)->get_time();
                
                if(_time > (max_ohcal_t - _range) && _time < (max_ohcal_t + _range))
                {
                    totalohcal += offlineenergy;
                    hohcaltime->Fill(_time);
                    
                    if (offlineenergy > 1)
                    {
                        h_hcalout_etaphi->Fill(ieta,iphi,offlineenergy);
                    }
                }
                
            }
        }
    }
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_MBD");
        if (offlinetowers)
        {
            int size = offlinetowers->size(); //online towers should be the same!
            for (int channel = 0; channel < size;channel++)
            {
                TowerInfo* offlinetower = offlinetowers->get_tower_at_channel(channel);
                float offlineenergy = offlinetower->get_energy();
                unsigned int towerkey = TowerInfoDefs::encode_mbd(channel);
                int type = TowerInfoDefs::get_mbd_type(towerkey);
                int _time = offlinetowers->get_tower_at_channel(channel)->get_time();
                
                if (type == 1)
                {
                    if(_time > (max_mbd_t - _range) && _time < (max_mbd_t + _range))
                    {
                        totalmbd += offlineenergy; hmbdtime->Fill(_time);
                    }
                }
            }
        }
    }
    
    {
        TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_ZDC");
        if (offlinetowers)
        {
            int size = offlinetowers->size(); //online towers should be the same!
            for (int channel = 0; channel < size;channel++)
            {
                TowerInfo* offlinetower = offlinetowers->get_tower_at_channel(channel);
                float offlineenergy = offlinetower->get_energy();
                int _time = offlinetowers->get_tower_at_channel(channel)->get_time();
                
                if(channel == 0 || channel == 2 || channel == 4 || channel == 8 || channel == 10 || channel == 12)
                {
                    if(_time > (max_zdc_t - _range) && _time < (max_zdc_t + _range))
                    {
                        totalzdc += offlineenergy;
                        hzdctime->Fill(_time);
                    }
                }
            }
        }
    }
    
    h_emcal_mbd_correlation->Fill(totalcemc/emcaldownscale,totalmbd/mbddownscale);
    h_ihcal_mbd_correlation->Fill(totalihcal/ihcaldownscale,totalmbd/mbddownscale);
    h_ohcal_mbd_correlation->Fill(totalohcal/ohcaldownscale,totalmbd/mbddownscale);
    h_emcal_hcal_correlation->Fill(totalcemc/emcaldownscale,totalohcal/ohcaldownscale);
    h_emcal_zdc_correlation->Fill(totalcemc/emcaldownscale,totalzdc/zdcdownscale);
    
}
 //------------------------------------------------- pi 0 --------------------------------------------------------//

  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_POS_COR_CEMC");
  if(!clusterContainer)
    {
      std::cout << PHWHERE << "funkyCaloStuff::process_event - Fatal Error - CLUSTER_CEMC node is missing. " << std::endl;
      return 0;
    }

  float emcMinClusE1 = 1.5;//0.5;
  float emcMinClusE2 = 0.8;//0.5;
  float emcMaxClusE = 32;
  float minDr = 0.08;
  float maxAlpha = 0.8;

  if (totalmbd < 0.1*mbddownscale)
    {

      RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
      RawClusterContainer::ConstIterator clusterIter;
      RawClusterContainer::ConstIterator clusterIter2;

      for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
	{
	  RawCluster *recoCluster = clusterIter -> second;
	  
	  CLHEP::Hep3Vector vertex(0,0,0);
	  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
	  
	  float clusE = E_vec_cluster.mag();
	  float clus_eta = E_vec_cluster.pseudoRapidity();
	  float clus_phi = E_vec_cluster.phi();
	  float clus_pt = E_vec_cluster.perp();
	  float clus_chisq = recoCluster->get_chi2();
	  
	  if (clusE < emcMinClusE1 || clusE > emcMaxClusE){continue;}
	  if (abs(clus_eta) > 0.7){continue;}
	  if (clus_chisq > 4){continue;}
	  
	  TLorentzVector photon1;
	  photon1.SetPtEtaPhiE(clus_pt, clus_eta, clus_phi, clusE);
	  

	  
	  for(clusterIter2 = clusterEnd.first; clusterIter2 != clusterEnd.second; clusterIter2++)
	    {
	      if (clusterIter ==clusterIter2){continue;}
	      RawCluster *recoCluster2 = clusterIter2 -> second;
	  
	      CLHEP::Hep3Vector vertex2(0,0,0);
	      CLHEP::Hep3Vector E_vec_cluster2 = RawClusterUtility::GetECoreVec(*recoCluster2, vertex2);
	  

	      float clus2E = E_vec_cluster2.mag();
	      float clus2_eta = E_vec_cluster2.pseudoRapidity();
	      float clus2_phi = E_vec_cluster2.phi();
	      float clus2_pt = E_vec_cluster2.perp();
	      float clus2_chisq = recoCluster2->get_chi2();
	      
	      if (clus2E < emcMinClusE2 || clus2E > emcMaxClusE){continue;}
	      if (abs(clus2_eta) > 0.7){continue;}
	      if (clus2_chisq > 4){continue;}
	      
	      TLorentzVector photon2;
	      photon2.SetPtEtaPhiE(clus2_pt, clus2_eta, clus2_phi, clus2E);
	      

	      if(sqrt(pow(clusE - clus2E,2))/(clusE + clus2E) > maxAlpha) continue; 

	      if(photon1.DeltaR(photon2) < minDr) continue;
	      TLorentzVector pi0 = photon1 + photon2;
	      h_InvMass -> Fill(pi0.M());
	    }
	  
	}
      
      
    }
  
  return Fun4AllReturnCodes::EVENT_OK;

}

int CaloAna::End(PHCompositeNode* /*topNode*/)
{
  outfile->cd();
 
  h_emcal_mbd_correlation->Write();
  h_ihcal_mbd_correlation->Write();
  h_ohcal_mbd_correlation->Write();
  h_emcal_hcal_correlation->Write();
  h_InvMass->Write();
  h_cemc_etaphi->Write();
  h_hcalin_etaphi->Write();
  h_hcalout_etaphi->Write();

  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(outfilename, "UPDATE");
  return 0;
}

int CaloAna::Getpeaktime(TH1 * h)
{
    int getmaxtime, tcut = -1;

    for(int bin = 1; bin < h->GetNbinsX(); bin++)
    {
      double c = h->GetBinContent(bin);
      double max = h->GetMaximum();
      int bincenter = h->GetBinCenter(bin);
      if(max == c)
      {
        getmaxtime = bincenter;
        if(getmaxtime != -1) tcut = getmaxtime;
      }
    }

    return tcut;

}
