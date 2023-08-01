#include "caloTreeGen.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <ffaobjects/EventHeader.h>

//ROOT stuff
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>

//For clusters and geometry
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>

//Tower stuff
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

//thingy
#include <CLHEP/Geometry/Point3D.h>


//____________________________________________________________________________..
caloTreeGen::caloTreeGen(const std::string &name,
                         const std::string &eventFile,
                         const int eventNumber,
                         const int runid,
                         const float tow_cemc_min,
                         const float tow_hcalin_min,
                         const float tow_hcalout_min):
  SubsysReco(name)
  ,T(nullptr)
  ,Outfile(name)
  ,eventFile(eventFile)
  ,totalCaloE(0)
  ,iEvent(0)
  ,eventNumber(eventNumber)
  ,runid(runid)
  ,tow_cemc_min(tow_cemc_min)
  ,tow_hcalin_min(tow_hcalin_min)
  ,tow_hcalout_min(tow_hcalout_min) {

  std::cout << "caloTreeGen::caloTreeGen(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
caloTreeGen::~caloTreeGen() {
  std::cout << "caloTreeGen::~caloTreeGen() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int caloTreeGen::Init(PHCompositeNode *topNode) {

  eventOutput.open(eventFile.c_str());
  
  out = new TFile(Outfile.c_str(),"RECREATE");

  T = new TTree("T","T");

  //emc
  T -> Branch("emcTowE",&m_emcTowE);
  T -> Branch("emcTowEta",&m_emcTowEta);
  T -> Branch("emcTowPhi",&m_emcTowPhi);
  T -> Branch("emcTiming",&m_emcTiming);
  T -> Branch("emciEta",&m_emciEta);
  T -> Branch("emciPhi",&m_emciPhi);
  
  T -> Branch("ohcTowE",&m_ohcTowE);
  T -> Branch("ohcTowEta",&m_ohciTowEta);
  T -> Branch("ohcTowPhi",&m_ohciTowPhi);
  
  T -> Branch("ihcTowE",&m_ihcTowE);
  T -> Branch("ihcTowEta",&m_ihciTowEta);
  T -> Branch("ihcTowPhi",&m_ihciTowPhi);
  
  T -> Branch("totalCaloE",&totalCaloE);

  //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE");

  eventOutput << "{\n \
      \"EVENT\": {\n \
      \"runid\": " << runid << ",\n \
      \"evtid\": " << eventNumber << ",\n \
       \"time\": 0,\n \
       \"type\": \"Collision\",\n \
       \"s_nn\": 200,\n \
          \"B\": 3.0,\n \
         \"pv\": [0,0,0]\n \
      },\n \
      \"META\": {\n \
        \"HITS\": {\n \
          \"INNERTRACKER\": {\n \
            \"type\": \"3D\",\n \
            \"options\": {\n \
              \"size\": 5,\n \
              \"color\": 16777215\n \
            }\n \
          },\n \
          \"CEMC\": {\n \
            \"type\": \"PROJECTIVE\",\n \
              \"options\": {\n \
                \"rmin\": 90,\n \
                \"rmax\": 116.1,\n \
                \"deta\": 0.025,\n \
                \"dphi\": 0.025,\n \
                \"color\": 16766464,\n \
                \"transparent\": 0.6,\n \
                \"scaleminmax\": false\n \
              }\n \
          },\n \
          \"HCALIN\": {\n \
            \"type\": \"PROJECTIVE\",\n \
              \"options\": {\n \
                \"rmin\": 117.27,\n \
                \"rmax\": 165.0,\n \
                \"deta\": 0.025,\n \
                \"dphi\": 0.025,\n \
                \"color\": 4290445312,\n \
                \"transparent\": 0.6,\n \
                \"scaleminmax\": false\n \
              }\n \
          },\n \
          \"HCALOUT\": {\n \
            \"type\": \"PROJECTIVE\",\n \
              \"options\": {\n \
                \"rmin\": 183.3,\n \
                \"rmax\": 274.317,\n \
                \"deta\": 0.025,\n \
                \"dphi\": 0.025,\n \
                \"color\": 24773,\n \
                \"transparent\": 0.6,\n \
                \"scaleminmax\": false\n \
              }\n \
          }\n \
        }\n \
      },\n \
  \"HITS\": {\n";
  
  std::cout << "caloTreeGen::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::InitRun(PHCompositeNode *topNode) {
  std::cout << "caloTreeGen::InitRun(PHCompositeNode *topNode) Initializing for Run " << runid << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::process_event(PHCompositeNode *topNode) {

  // skip events until event number is reached
  if(iEvent != eventNumber) {
    ++iEvent;
    return 0;
  }
  else std::cout << "Processing Event: " << iEvent << std::endl;

  //tower information
  TowerInfoContainer *emcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC");
  // TowerInfoContainer *emcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC_RETOWER");

  if(!emcTowerContainer) {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERS_CEMC"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  
  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  // RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");

  if (!towergeom) {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  float calib = 1.;

  //grab all the towers and fill their energies. 
  unsigned int tower_range = emcTowerContainer->size();
  totalCaloE = 0;

  bool first_entry = true;

  for(unsigned int iter = 0; iter < tower_range; iter++) {
    unsigned int towerkey = emcTowerContainer->encode_key(iter);
    unsigned int ieta = getCaloTowerEtaBin(towerkey);
    unsigned int iphi = getCaloTowerPhiBin(towerkey);
    m_emciEta.push_back(ieta);
    m_emciPhi.push_back(iphi);
      
    double energy = emcTowerContainer -> get_tower_at_channel(iter) -> get_energy()/calib;
    totalCaloE += energy;
    double time = emcTowerContainer -> get_tower_at_channel(iter) -> get_time();

    double eta = towergeom -> get_etacenter(ieta);
    double phi = towergeom -> get_phicenter(iphi);
    m_emcTowEta.push_back(eta);
    m_emcTowPhi.push_back(phi);
    m_emcTowE.push_back(energy);
    m_emcTiming.push_back(time);

    if(energy < tow_cemc_min) continue;

    if(first_entry) {
        eventOutput << "\"CEMC\": [{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << energy << "}\n";
        first_entry = false;
    }
    else eventOutput << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << energy << "}\n";
  }
  if(tower_range == 0) eventOutput << "\"CEMC\": [{}],\n";
  else          eventOutput << "],\n";

  first_entry = true;
  
  //tower information
  TowerInfoContainer *ohcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_HCALOUT");
  if(!ohcTowerContainer) {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERINFO_CALIB_OHCAL"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //Tower geometry node for location information
  towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  if (!towergeom) {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_HCALOUT"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  tower_range = ohcTowerContainer->size();

  for(unsigned int iter = 0; iter < tower_range; iter++) {
    unsigned int towerkey = ohcTowerContainer->encode_key(iter);
    unsigned int ieta = getCaloTowerEtaBin(towerkey);
    unsigned int iphi = getCaloTowerPhiBin(towerkey);
    m_ohciTowEta.push_back(ieta);
    m_ohciTowPhi.push_back(iphi);

    // double eta = eta_map[ieta];
    // double phi = phi_map[iphi];
    double eta = towergeom -> get_etacenter(ieta);
    double phi = towergeom -> get_phicenter(iphi);

    double energy = ohcTowerContainer -> get_tower_at_channel(iter) -> get_energy();
    m_ohcTowE.push_back(energy);

    if(energy < tow_hcalout_min) continue;

    if(first_entry) {
      eventOutput << "\"HCALOUT\": [{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << energy << "}\n";
      first_entry = false;
    }
    else eventOutput << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << energy << "}\n";
  }

  if(tower_range == 0) eventOutput << "\"HCALOUT\": [{}],\n";
  else          eventOutput << "],\n";

  first_entry = true;

  TowerInfoContainer *ihcTowerContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_HCALIN");
  if(!ihcTowerContainer) {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERINFO_CALIB_HCALIN"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  //Tower geometry node for location information
  towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");

  if (!towergeom) {
    std::cout << PHWHERE << "caloTreeGen::process_event Could not find node TOWERGEOM_HCALIN"  << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  tower_range = ihcTowerContainer->size();

  for(unsigned int iter = 0; iter < tower_range; iter++) {
    unsigned int towerkey = ihcTowerContainer->encode_key(iter);
    unsigned int ieta = getCaloTowerEtaBin(towerkey);
    unsigned int iphi = getCaloTowerPhiBin(towerkey);
    m_ihciTowEta.push_back(ieta);
    m_ihciTowPhi.push_back(iphi);

    // double eta = eta_map[ieta];
    // double phi = phi_map[iphi];
    double eta = towergeom -> get_etacenter(ieta);
    double phi = towergeom -> get_phicenter(iphi);
      
    double energy = ihcTowerContainer -> get_tower_at_channel(iter) -> get_energy();
    m_ihcTowE.push_back(energy);

    if(energy < tow_hcalin_min) continue;

    if(first_entry) {
      eventOutput << "\"HCALIN\": [{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << energy << "}\n";
      first_entry = false;
    }
    else eventOutput << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << energy << "}\n";
  }

  if(tower_range == 0) eventOutput << "\"HCALIN\": [{}]\n";
  else          eventOutput << "]\n";

  T -> Fill();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::ResetEvent(PHCompositeNode *topNode)
{
  m_emcTowPhi.clear();
  m_emcTowEta.clear();
  m_emcTowE.clear();
  m_emcTiming.clear();
  m_emciEta.clear();
  m_emciPhi.clear();
  
  m_ihcTowE.clear();
  m_ihciTowEta.clear();
  m_ihciTowPhi.clear();

  m_ohcTowE.clear();
  m_ohciTowEta.clear();
  m_ohciTowPhi.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::EndRun(const int runnumber)
{
  std::cout << "caloTreeGen::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::End(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  out -> cd();
  T -> Write();
  out -> Close();
  delete out;

  eventOutput << "},\n \
  \"TRACKS\": {\n \
  \"INNERTRACKER\": []\n \
  }}";
  eventOutput.close();
  //hm -> dumpHistos(Outfile.c_str(), "UPDATE");
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int caloTreeGen::Reset(PHCompositeNode *topNode)
{
  std::cout << "caloTreeGen::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void caloTreeGen::Print(const std::string &what) const
{
  std::cout << "caloTreeGen::Print(const std::string &what) const Printing info for " << what << std::endl;
}
// convert from calorimeter key to phi bin
//____________________________________________________________________________..
unsigned int caloTreeGen::getCaloTowerPhiBin(const unsigned int key)
{
  unsigned int etabin = key >> 16U;
  unsigned int phibin = key - (etabin << 16U);
  return phibin;
}

//____________________________________________________________________________..
// convert from calorimeter key to eta bin
unsigned int caloTreeGen::getCaloTowerEtaBin(const unsigned int key)
{
  unsigned int etabin = key >> 16U;
  return etabin;
}
//____________________________________________________________________________..
float caloTreeGen::getMaxTowerE(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  float maxEnergy = 0;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++)
  {
    float towE = toweriter -> second;
   
    if( towE > maxEnergy)  maxEnergy = towE;
  }
  return maxEnergy;
}
//____________________________________________________________________________..
std::vector<int> caloTreeGen::returnClusterTowEta(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<int> towerIDsEta;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerIDsEta.push_back(RawTowerDefs::decode_index1(toweriter -> first));

  return towerIDsEta;
}
//____________________________________________________________________________..
std::vector<int> caloTreeGen::returnClusterTowPhi(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<int> towerIDsPhi;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerIDsPhi.push_back(RawTowerDefs::decode_index2(toweriter -> first));
  return towerIDsPhi;
}
//____________________________________________________________________________..
std::vector<float> caloTreeGen::returnClusterTowE(RawCluster *cluster, TowerInfoContainer *towerContainer)
{
  RawCluster::TowerConstRange towers = cluster -> get_towers();
  RawCluster::TowerConstIterator toweriter;
  
  std::vector<float> towerE;
  for(toweriter = towers.first; toweriter != towers.second; toweriter++) towerE.push_back(toweriter -> second);
  
  return towerE;
}
