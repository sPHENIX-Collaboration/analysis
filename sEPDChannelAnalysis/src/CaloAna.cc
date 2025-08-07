#include "CaloAna.h"

// G4Hits includes
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>

// G4Cells includes
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>

// Tower includes
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainerv4.h>
#include <calobase/TowerInfov4.h>

// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>

#include <TFile.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TH2.h>

#include <Event/Event.h>
#include <Event/packet.h>
#include <cassert>
#include <sstream>
#include <string>

using namespace std;

CaloAna::CaloAna(const std::string& name, const std::string& filename,float zero_sup)
  : SubsysReco(name)
  , detector("sEPD")
  , m_outputFileName(filename)
  , min_energy(zero_sup)
  , num_channels(768)
  , m_event(-1)
{
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
  // TH1 *h1 = new TH1F("h1",....)
  // hm->registerHisto(h1);
	m_histoFileName = m_outputFileName + "_histos.root";
        m_outputFileName = m_outputFileName + ".root";

	hm->setOutfileName(m_histoFileName.c_str());

  outfile = new TFile(m_outputFileName.c_str(), "RECREATE");


  towerntuple = new TTree("towerntup", "Towers");
  towerntuple->Branch("energy",&m_energy);
  towerntuple->Branch("etabin",&m_etabin);
  towerntuple->Branch("phibin",&m_phibin);




	for(int i = 0; i < num_channels; i++) {
		std::string histo_name = "channel" + std::to_string(i);

		auto histo = new TH1F(histo_name.c_str(),histo_name.c_str(),1200,0,1200);

		hm->registerHisto(histo);	
	}	 

  // for (int i = 0; i < 31;i++)
  //   {
  //     towerntuple->Branch(Form("waveform_%i",i),&m_waveform[i]);
  //   }

  return 0;
}

int CaloAna::process_event(PHCompositeNode* topNode)
{
	++m_event;
  process_towers(topNode);

	if(m_event % 100 == 0) {
		std::cout << "Event " << m_event << std::endl;
	}

  return Fun4AllReturnCodes::EVENT_OK;
}


int CaloAna::process_towers(PHCompositeNode* topNode)
{
  ostringstream nodename;
  ostringstream geonodename;

  nodename.str("");
  nodename << "TOWERS_SEPD_" << detector;
  geonodename.str("");
 
  TowerInfoContainer* offlinetowers = findNode::getClass<TowerInfoContainerv4>(topNode, "TOWERS_SEPD");


	if(offlinetowers == NULL){
		std::cout << "No towers found!!!" << std::endl;
		return -1;
	}

  int size = offlinetowers->size(); //online towers should be the same!


  for (int channel = 0; channel < size;channel++)
    {
      TowerInfo* offlinetower = offlinetowers->get_tower_at_channel(channel);
      float offlineenergy = offlinetower->get_energy();
      if(offlineenergy < min_energy){
		continue;
	}
	unsigned int towerkey = offlinetowers->encode_key(channel);
      int ieta = offlinetowers->getTowerEtaBin(towerkey);
      int iphi = offlinetowers->getTowerPhiBin(towerkey);
  

    
      m_energy.push_back(offlineenergy);
      m_etabin.push_back(ieta);
      m_phibin.push_back(iphi);

	std::string histo_name = "channel" + std::to_string(channel);

	auto h_energy = dynamic_cast<TH1*>(hm->getHisto(histo_name.c_str()));
	h_energy->Fill(offlineenergy);

      // towerntuple->Fill(offlineenergy,ieta,iphi);
    }

  // cout << 2 << endl;
  
  // cout << 5 << endl;
  towerntuple->Fill();
  
  m_etabin.clear();
  m_phibin.clear();
  m_energy.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloAna::End(PHCompositeNode* /*topNode*/)
{
  	hm->dumpHistos(m_histoFileName.c_str(),"RECREATE");

	outfile->cd();
 
  towerntuple->Write();

  outfile->Write();
	outfile->Close();
	delete outfile;

  return 0;
}

