#include "quickHIJING.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <centrality/CentralityInfo.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h> 
#include <HepMC/GenParticle.h>
#pragma GCC diagnostic pop

//____________________________________________________________________________..
quickHIJING::quickHIJING(const std::string &name):
  SubsysReco(name)
  ,Outfile(name)
{
  std::cout << "quickHIJING::quickHIJING(const std::string &name) Calling ctor" << std::endl;
}  
//____________________________________________________________________________..
quickHIJING::~quickHIJING()
{
  std::cout << "quickHIJING::~quickHIJING() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int quickHIJING::Init(PHCompositeNode *topNode)
{
  std::cout << "quickHIJING::Init(PHCompositeNode *topNode) Initializing" << std::endl;

  out = new TFile("qhTest.root","RECREATE");
  
  T = new TTree("T","T");
  T -> Branch("pid",&m_pid);
  T -> Branch("pt",&m_pt);
  T -> Branch("eta",&m_eta);
  T -> Branch("phi",&m_phi);
  T -> Branch("e",&m_e);
  T -> Branch("psi2",&m_psi2);
  T -> Branch("b",&m_b);
  T -> Branch("cent",&m_cent);
  T -> Branch("p",&m_p);


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int quickHIJING::InitRun(PHCompositeNode *topNode)
{
  std::cout << "quickHIJING::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int quickHIJING::process_event(PHCompositeNode *topNode)
{

  CentralityInfo *cent_node = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if(!cent_node)
    {
      std::cout << "No cent node" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  m_cent = cent_node -> get_centile(CentralityInfo::PROP::bimp);
  m_b = cent_node -> get_quantity(CentralityInfo::PROP::bimp);
  
  //truth particle information
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truthinfo)
    {
      std::cout << PHWHERE << "hijingTruthCheck::process_event Could not find node G4TruthInfo"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  
  
  //For pythia ancestory information
  PHHepMCGenEventMap *genEventMap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if(!genEventMap)
    {
      std::cout << PHWHERE << "cemc::process_event Could not find PHHepMCGenEventMap"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  //event level information of the above
  PHHepMCGenEvent *genEvent = genEventMap -> get(0);
  if(!genEvent)
    {
      std::cout << PHWHERE << "cemc::process_event Could not find PHHepMCGenEvent"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  HepMC::GenEvent *evt = genEvent -> getEvent();
  
  HepMC::HeavyIon *hi = evt -> heavy_ion();
  
  m_psi2 = hi -> event_plane_angle();

 
  PHG4TruthInfoContainer::Range truthRange = truthinfo -> GetPrimaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIter;

  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
      PHG4Particle *truthPar = truthIter->second;
      
      if(abs(truthPar -> get_pid()) >= 12 && abs(truthPar -> get_pid()) <= 16) continue;
      
      m_pid.push_back(truthPar -> get_pid());
      
      m_p.push_back(getP(truthPar));
      
      m_e.push_back(truthPar -> get_e());
      
      m_eta.push_back(getEta(truthPar));
      
      m_phi.push_back(getPhi(truthPar));
      
      m_pt.push_back(getpT(truthPar));
      
      //if(getpT(truthPar) < 0 || getpT(truthPar) > 10)std::cout << "Found particle with pt: " << getpT(truthPar) << std::endl;

    }
  //std::cout << "Saw " << npart << " particles" << std::endl;
  T -> Fill();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int quickHIJING::ResetEvent(PHCompositeNode *topNode)
{

  m_pid.clear();
  m_e.clear();
  m_eta.clear();
  m_pt.clear();
  m_phi.clear();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int quickHIJING::EndRun(const int runnumber)
{
  std::cout << "quickHIJING::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int quickHIJING::End(PHCompositeNode *topNode)
{
  out -> cd();
  T -> Write();
  out -> Close();
  
  std::cout << "quickHIJING::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int quickHIJING::Reset(PHCompositeNode *topNode)
{
 std::cout << "quickHIJING::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void quickHIJING::Print(const std::string &what) const
{
  std::cout << "quickHIJING::Print(const std::string &what) const Printing info for " << what << std::endl;
}
//____________________________________________________________________________..
float quickHIJING::getEta(PHG4Particle *particle)
{
  float px = particle -> get_px();
  float py = particle -> get_py();
  float pz = particle -> get_pz();
  float p = sqrt(pow(px,2) + pow(py,2) + pow(pz,2));

  return 0.5*log((p+pz)/(p-pz));
}
//____________________________________________________________________________..
float quickHIJING::getPhi(PHG4Particle *particle)
{
  float phi = atan2(particle -> get_py(),particle -> get_px());
  return phi;
}
//____________________________________________________________________________..
float quickHIJING::getpT(PHG4Particle *particle)
{
  float px = particle -> get_px();
  float py = particle -> get_py();
  
  float pt = sqrt(pow(px,2) + pow(py,2));
  return pt;
}
//____________________________________________________________________________..
float quickHIJING::getP(PHG4Particle *particle)
{
  float px = particle -> get_px();
  float py = particle -> get_py();
  float pz = particle -> get_pz();
  float p = sqrt(pow(px,2) + pow(py,2) + pow(pz,2));

  return p;
}
