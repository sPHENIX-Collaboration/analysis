//HCAL Calibration
//Abhisek Sen
//
#include "HCalib.h"
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/getClass.h>

#include <g4detectors/PHG4HcalDefs.h>
#include <phparameter/PHParameters.h>
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CellDefs.h>

#include <g4detectors/PHG4ScintillatorSlat.h>
#include <g4detectors/PHG4ScintillatorSlatContainer.h>
#include <g4detectors/PHG4ScintillatorSlatDefs.h>

#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <TH1F.h>

using namespace std;

HCalib::HCalib():
SubsysReco("HCal_Calibration")
{
 towers=0;
 slats = 0;
 threshold=0.000; //GeV
 outfile=0;
 fill_towers = 1;
 fill_slats = 1;
 is_proto = true;
 return;
}

int HCalib::Init(PHCompositeNode *topNode)
{
 outfile = new TFile("out_calib.root","RECREATE");
 return Fun4AllReturnCodes::EVENT_OK;
}

int HCalib::process_event(PHCompositeNode *topNode)
{
 //cout << "Processing event:: HCalib " << endl;
 string detnames[] = {"HCALIN","HCALOUT"};
 for(int seg=0; seg<2; seg++)
 {
  GetNodes(topNode,detnames[seg]);

  if(fill_towers and towers)
  {
   auto twr_range = towers->getTowers();
   for (auto twr_iter = twr_range.first; twr_iter != twr_range.second; ++twr_iter)
    {
      RawTower* tower = twr_iter->second;
      int etabin = tower->get_bineta();
      int phibin = tower->get_binphi();
      int key = genkey(seg, etabin, phibin);
      auto it = tower_hists.find(key);
      if(it==tower_hists.end())
      {
       string hname=detnames[seg]+"_Tower_Energy_ieta_"+to_string(etabin)+"_iphi_"+to_string(phibin);
       TH1F *h = new TH1F(hname.c_str(),hname.c_str(),500,0,50);
       tower_hists[key] = h;
       h->Fill( 1e3*tower->get_energy());
      }
      else {
       tower_hists[key]->Fill( 1e3*tower->get_energy());
     }
    }

   }


  /*
  for(unsigned int ieta=1; ieta<23; ieta++)
  {
   for(unsigned int iphi=1; iphi<63; iphi++)
   {
   RawTower *twr = towers->getTower(ieta,iphi);
   if(!twr) continue;
   

   float twr_e = twr->get_energy();

   //cout << "Energy " << twr->get_energy() << endl;
   if(twr_e<threshold) continue;
     
   bool good_tower = true; //false;
   if(!good_tower)
   {
   // check for ieta-1, ieta+1
   RawTower *twr_p = towers->getTower(ieta-1,iphi);
   RawTower *twr_n = towers->getTower(ieta+1,iphi);
   if(twr_p && twr_n)
   {
    if(twr_p->get_energy()>threshold &&
      twr_n->get_energy()>threshold ) good_tower=true;
   }

   //check for iphi-1, iphi+1
   twr_p = towers->getTower(ieta,iphi-1);
   twr_n = towers->getTower(ieta,iphi-1);
   if(twr_p && twr_n)
   {
    if(twr_p->get_energy()>threshold &&
      twr_n->get_energy()>threshold ) good_tower=true;
   }

   //Diagonal
   twr_p = towers->getTower(ieta-1,iphi-1);
   twr_n = towers->getTower(ieta+1,iphi+1);
   if(twr_p && twr_n)
   {
    if(twr_p->get_energy()>threshold &&
       twr_n->get_energy()>threshold ) good_tower=true;
   }

   //Diagonal
   twr_p = towers->getTower(ieta-1,iphi+1);
   twr_n = towers->getTower(ieta+1,iphi-1);
   if(twr_p && twr_n)
   {
    if(twr_p->get_energy()>threshold &&
       twr_n->get_energy()>threshold ) good_tower=true;
   }
   }

   if(good_tower) Fill(twr,seg);
   
   }
   }
  }
   */
 if(fill_slats)
 {
   if(is_proto && proto_slats) 
   {
    auto cell_range = proto_slats->getScintillatorSlats();
    for (auto cell_iter = cell_range.first; cell_iter != cell_range.second; ++cell_iter)
    {
      PHG4ScintillatorSlat *cell = cell_iter->second;
      int etabin = cell->get_column();
      int phibin = cell->get_row();
      int key = genkey(seg, etabin, phibin);
      auto it = slat_hists.find(key);
      if(it==slat_hists.end())
      {
       string hname=detnames[seg]+"_Slat_Energy_ieta_"+to_string(etabin)+"_iphi_"+to_string(phibin);
       TH1F *h = new TH1F(hname.c_str(),hname.c_str(),500,0,50);
       slat_hists[key] = h;
       h->Fill( 1e3*cell->get_light_yield() );
      }
      else {
       slat_hists[key]->Fill( 1e3*cell->get_light_yield() );
     }
    }
   }
   
   if(slats)
   {
    auto cell_range = slats->getCells();
    for (auto cell_iter = cell_range.first; cell_iter != cell_range.second; ++cell_iter)
    {
      PHG4Cell *cell = cell_iter->second;
      int etabin = PHG4CellDefs::ScintillatorSlatBinning::get_column(cell->get_cellid());
      int phibin = PHG4CellDefs::ScintillatorSlatBinning::get_row(cell->get_cellid());
      int key = genkey(seg, etabin, phibin);
      auto it = slat_hists.find(key);
      if(it==slat_hists.end())
      {
       string hname=detnames[seg]+"_Slat_Energy_ieta_"+to_string(etabin)+"_iphi_"+to_string(phibin);
       TH1F *h = new TH1F(hname.c_str(),hname.c_str(),500,0,50);
       slat_hists[key] = h;
       h->Fill( 1e3*cell->get_light_yield() );
      }
      else {
       slat_hists[key]->Fill( 1e3*cell->get_light_yield() );
     }
    }
   }
 }

 }

 return Fun4AllReturnCodes::EVENT_OK;
}

int HCalib::End(PHCompositeNode *topNode)
{
 outfile->cd();
 for( auto it: tower_hists)
 {
  if(it.second->GetEntries()>0)
   it.second->Write();
 }

 for(auto it : slat_hists)
 {
  if(it.second->GetEntries()>0)
   it.second->Write();
 }

 outfile->Close();
 return Fun4AllReturnCodes::EVENT_OK;
}

void HCalib::GetNodes(PHCompositeNode *topNode, const string &det)
{
    static int ncalls = 0;
    //string towernode = "TOWER_CALIB_" + det;
    string towernode = "TOWER_SIM_" + det;
    towers = findNode::getClass<RawTowerContainer>(topNode,towernode.c_str());
  if (!towers) {
      if(ncalls<5) cerr << PHWHERE << " ERROR: Can't find " << towernode << endl;
      ncalls++;
      //exit(-1);
    }

  //Slats
  std::string cellnodename = "G4CELL_" + det;
  slats = findNode::getClass<PHG4CellContainer>(topNode, cellnodename);
  if(!slats)
  {
    if(ncalls<5) cerr << PHWHERE << " Node missing " << cellnodename << endl;
  }

  //Proto slats
  if( is_proto )
  {
   proto_slats = findNode::getClass<PHG4ScintillatorSlatContainer>(topNode, cellnodename); 
   if(!proto_slats)
  {
    if(ncalls<5) cerr << PHWHERE << " Prototype Node missing " << cellnodename << endl;
  }
  }

}

int HCalib::genkey(const int detid, const int etabin, const int phibin)
{
 //Assuming 
 //At most detid : 2, etabin: 24, phibin:256
 return 30000*detid + 500*etabin + phibin;
}
