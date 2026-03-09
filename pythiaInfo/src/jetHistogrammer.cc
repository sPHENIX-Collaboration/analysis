//____________________________________________________________________________..
// This module is intended to look at truth level and pseudo jet level jets
// and measure the spectrum from min bias pythia. The output will be used
// to determine the point of 100% efficiency between two independent data sets
// e.g. the 10 and 30GeV triggered data sets
//____________________________________________________________________________..

#include "jetHistogrammer.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

//fast jet stuff
#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>

//jet stuff
#include <g4jets/JetMap.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>


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
jetHistogrammer::jetHistogrammer(const std::string &name, const std::string &fileout):
  SubsysReco(name),
  m_outputFilename(fileout)
{
  std::cout << "jetHistogrammer::jetHistogrammer(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
jetHistogrammer::~jetHistogrammer()
{

  for(int i = 0; i < nEtaBins; i++)ptGJet[i] = 0;
  ptPJet = 0;
  std::cout << "jetHistogrammer::~jetHistogrammer() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int jetHistogrammer::Init(PHCompositeNode *topNode)
{
  
  std::cout << "jetHistogrammer::Init(PHCompositeNode *topNode) Initializing" << std::endl;

    
  int nBins = 100;
  Float_t bins[nBins+1];
  for(int i = 0; i < nBins+1; i++) bins[i] = 80./nBins*i;
  
  for(int i = 0; i < nEtaBins; i++) ptGJet[i] = new TH1F(Form("geantJets_%dEta",i),"jets reco'd in GEANT world",nBins,bins);
  //0.7,1.5,inclusive
  
  ptPJet = new TH1F("pythiaJets","jets reco'd in pythia world",nBins,bins);
  
  

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetHistogrammer::InitRun(PHCompositeNode *topNode)
{
  std::cout << "jetHistogrammer::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetHistogrammer::process_event(PHCompositeNode *topNode)
{
  
  JetMap* jetsMC = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");
  if(!jetsMC)
    {
      std::cout << "jetHistogrammer::process_event Cannot find truth node!" << std::endl;
      exit(-1);
    }
  
  PHHepMCGenEventMap *genEventMap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if(!genEventMap)
    {
      std::cout << "jetHistogrammer::process_event Cannot find genEventMap!" << std::endl;
      exit(-1);
    }

  PHHepMCGenEvent *genEvent = genEventMap -> get(1);
  if(!genEvent)
    {
      std::cout << "jetHistogrammer::process_event Cannot find genEvent!" << std::endl;
      exit(-1);
    }
  
  HepMC::GenEvent *theEvent = genEvent -> getEvent();
  

  float maxJetPt = 0;
  float maxEta = -1;
  for(JetMap::Iter iter = jetsMC -> begin(); iter != jetsMC -> end(); ++iter)
    {
      Jet *truthjet = iter -> second;
      
      if(truthjet -> get_pt() > maxJetPt) 
	{
	  maxJetPt = truthjet -> get_pt();
	  maxEta = truthjet -> get_eta();
	}
    }


  if(getEtaBin(maxEta) != -1)
    {
      ptGJet[3] -> Fill(maxJetPt);
      for(int i = (int)getEtaBin(maxEta);  i < nEtaBins - 1; i++) ptGJet[i] -> Fill(maxJetPt);
    }
  else 
    {
      ptGJet[3] -> Fill(maxJetPt);
    }

  std::vector<fastjet::PseudoJet> pseudojets;
  unsigned int i = 0;
  for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p)
    {
      if(!(*p) -> status()) continue; //only stable particles

      // remove some particles (muons, taus, neutrinos)...
      // 12 == nu_e
      // 13 == muons
      // 14 == nu_mu
      // 15 == taus
      // 16 == nu_tau
      if (abs((*p) -> pdg_id() >= 12) && abs((*p) -> pdg_id() <= 16))  continue;

      // remove acceptance... _etamin,_etamax
      if (((*p) -> momentum().px() == 0.0) && ((*p) -> momentum().py() == 0.0)) continue;  // avoid pt=0

      if (abs((*p) -> momentum().pseudoRapidity()) > 1.5) continue;

      fastjet::PseudoJet pseudojet((*p)->momentum().px(),
                                   (*p)->momentum().py(),
                                   (*p)->momentum().pz(),
                                   (*p)->momentum().e());
      pseudojet.set_user_index(i);
      pseudojets.push_back(pseudojet);
      i++;
    }
  //call fastjet
  fastjet::JetDefinition *jetDef = new fastjet::JetDefinition(fastjet::antikt_algorithm, 0.4, fastjet::E_scheme, fastjet::Best);
  fastjet::ClusterSequence jetFinder(pseudojets, *jetDef);
  std::vector<fastjet::PseudoJet> fastjets = jetFinder.inclusive_jets();
  delete jetDef;
  
  maxJetPt = 0;
  for(unsigned int ijet = 0; ijet < fastjets.size(); ++ijet)
    {
      const double pt = sqrt(fastjets[ijet].px() * fastjets[ijet].px() + fastjets[ijet].py() * fastjets[ijet].py()); 
      if(pt > maxJetPt) maxJetPt = pt;

    }
  ptPJet -> Fill(maxJetPt);


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetHistogrammer::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetHistogrammer::EndRun(const int runnumber)
{
  std::cout << "jetHistogrammer::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetHistogrammer::End(PHCompositeNode *topNode)
{
  
  std::cout << "jetHistogrammer opening output file: " << m_outputFilename << std::endl;
  PHTFileServer::get().open(m_outputFilename,"RECREATE");
  for(int i = 0; i < nEtaBins; i++)ptGJet[i] -> Write();
  ptPJet -> Write();
    
  std::cout << "jetHistogrammer::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int jetHistogrammer::Reset(PHCompositeNode *topNode)
{
  std::cout << "jetHistogrammer::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void jetHistogrammer::Print(const std::string &what) const 
{
  std::cout << "jetHistogrammer::Print(const std::string &what) const Printing info for " << what << std::endl;
}
//____________________________________________________________________________..
int jetHistogrammer::getEtaBin(float eta)
{
  int etaBin = -1;
  for(int i = 0; i < nEtaBins-1; i++)
    {
      if(sqrt(pow(eta,2)) >= etaBins[i] && sqrt(pow(eta,2)) < etaBins[i+1]) etaBin = i;
    }
  return etaBin;

}
