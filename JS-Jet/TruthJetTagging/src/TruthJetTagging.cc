//____________________________________________________________________________..

#include "TruthJetTagging.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>                    // for PHWHERE
#include <g4main/PHG4Utils.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <TDatabasePDG.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <TMath.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>
#include <g4jets/Jetv1.h>
#include <sstream>

//____________________________________________________________________________..
TruthJetTagging::TruthJetTagging(const std::string &name):
 SubsysReco(name)
 , _algorithms()
 , _radii()
 , _do_photon_tagging()
 , _do_hf_tagging()
 , _embedding_id(1)
{

}

//____________________________________________________________________________..
TruthJetTagging::~TruthJetTagging()
{

}

//____________________________________________________________________________..
int TruthJetTagging::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthJetTagging::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}
  
//____________________________________________________________________________..
int TruthJetTagging::process_event(PHCompositeNode *topNode)
{
PHG4TruthInfoContainer* truthcontainer = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
if (!truthcontainer)
  {
    std::cout
      << "MyJetAnalysis::process_event - Error can not find DST truthcontainer node "
        << std::endl;
exit(-1);
}
  PHHepMCGenEventMap* geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!geneventmap)
  {
    std::cout << PHWHERE << " - Fatal error - missing node PHHepMCGenEventMap" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHHepMCGenEvent* genevt = geneventmap->get(_embedding_id);
  if (!genevt)
  {
    std::cout << PHWHERE << " - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID " << _embedding_id;
    std::cout << ". Print PHHepMCGenEventMap:";
    geneventmap->identify();
    return Fun4AllReturnCodes::ABORTRUN;
  }



HepMC::GenEvent* theEvent = genevt->getEvent();

  int n_algo = _algorithms.size();
  int n_radii = _radii.size();
  if (_do_hf_tagging)
    {
      if (n_radii != n_algo)
	{
	  std::cout << "TruthJetTagging::process_event - errorr unequal number of jet radii and algoirthms specified" << std::endl;
	  exit(-1);
	}
    }
  for (int algoiter = 0;algoiter < n_algo;algoiter++)
    {
      JetMap* tjets = findNode::getClass<JetMap>(topNode, _algorithms.at(algoiter).c_str());
      if (!tjets)
	{
	  std::cout
	    << "MyJetAnalysis::process_event - Error can not find DST JetMap node "
	    << std::endl;
	  exit(-1);
	}
      for (JetMap::Iter iter = tjets->begin(); iter != tjets->end(); ++iter)
	{
          Jet* tjet = iter->second;
	  assert(tjet);
	  
	  if (_do_photon_tagging)
	    {
	      float photon_fraction =  TruthJetTagging::TruthPhotonTagging ( truthcontainer , tjet ); 
	      tjet->set_property(Jet::prop_gamma,photon_fraction);
	    }
	  if (_do_hf_tagging)
	    {
	      
	      float jet_radius = 0.4;
	      int jet_flavor = TruthJetTagging::hadron_tagging(tjet, theEvent, jet_radius);
	      tjet->set_property(Jet::prop_JetHadronFlavor,jet_flavor);
	    }
	 
	}
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

  float TruthJetTagging::TruthPhotonTagging ( PHG4TruthInfoContainer* truthnode, Jet* tjet)
  {
    assert(truthnode);
    assert(tjet);
    float jetpt = tjet->get_pt();
    float max_gamma_pt = 0;
    for (Jet::ConstIter comp = tjet->begin_comp(); comp != tjet->end_comp(); ++comp)
      {
	PHG4Particle* particle = truthnode->GetParticle((*comp).second);     
	if (particle->get_pid() == 22)
	  {
	    float particle_pt = TMath::Sqrt(TMath::Power(particle->get_px(),2) + TMath::Power(particle->get_py(),2));
	    if (particle_pt > max_gamma_pt)
	      {
		max_gamma_pt = particle_pt;
	      }
	  }
      }
    float ratio = max_gamma_pt/jetpt;
    return ratio;
  }






int TruthJetTagging::hadron_tagging(Jet* this_jet, HepMC::GenEvent* theEvent,
                                      const double match_radius)
{
  float this_pt = this_jet->get_pt();
  float this_phi = this_jet->get_phi();
  float this_eta = this_jet->get_eta();

  int jet_flavor = 0;
  double jet_parton_zt = 0;

  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
  {
    float dR = deltaR((*p)->momentum().pseudoRapidity(), this_eta,
                      (*p)->momentum().phi(), this_phi);
    if (dR > match_radius)
      continue;

    int pidabs = 0;
    TParticlePDG* pdg_p = TDatabasePDG::Instance()->GetParticle((*p)->pdg_id());
    if (pdg_p)
    {
      if (TString(pdg_p->ParticleClass()).BeginsWith("B-"))
      {
        pidabs = TDatabasePDG::Instance()->GetParticle("b")->PdgCode();
      }
      else if (TString(pdg_p->ParticleClass()).BeginsWith("Charmed"))
      {
        pidabs = TDatabasePDG::Instance()->GetParticle("c")->PdgCode();
      }
    }

    const double zt = (*p)->momentum().perp() / this_pt;

    if (pidabs == TDatabasePDG::Instance()->GetParticle("c")->PdgCode()      //
        or pidabs == TDatabasePDG::Instance()->GetParticle("b")->PdgCode())  // handle heavy quarks only. All other favor tagged as default 0
    {
      if (pidabs > abs(jet_flavor))  // heavy quark found
      {
        jet_parton_zt = zt;
        jet_flavor = pidabs;
      }
      else if (pidabs == abs(jet_flavor))  // same quark mass. next compare zt
      {
        if (zt > jet_parton_zt)
        {
          jet_parton_zt = zt;
          jet_flavor = pidabs;
        }
      }

      // if (pidabs == TDatabasePDG::Instance()->GetParticle("b")->PdgCode())
      // {
      //   // if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
      //   // {
      //   //   std::cout << __PRETTY_FUNCTION__
      //   //             << " --BOTTOM--> pt / eta / phi = "
      //   //             << (*p)->momentum().perp() << " / "
      //   //             << (*p)->momentum().pseudoRapidity() << " / "
      //   //             << (*p)->momentum().phi() << " with hadron ";
      //   //   pdg_p->Print();
      //   // }
      // }
      // else if (pidabs == TDatabasePDG::Instance()->GetParticle("c")->PdgCode())
      // {
      //   // if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
      //   // {
      //   //   std::cout << __PRETTY_FUNCTION__
      //   //             << " --CHARM --> pt / eta / phi = "
      //   //             << (*p)->momentum().perp() << " / "
      //   //             << (*p)->momentum().pseudoRapidity() << " / "
      //   //             << (*p)->momentum().phi() << " with hadron ";
      //   //   pdg_p->Print();
      //   // }
      // }
    }
  }  //       for (HepMC::GenEvent::particle_const_iterator p =


  this_jet->set_property(Jet::prop_JetHadronZT,
			 jet_parton_zt);
  // this_jet->identify();

  // if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
  //   std::cout << __PRETTY_FUNCTION__ << " jet_flavor = " << jet_flavor
  //             << std::endl;

  return jet_flavor;
}





















//____________________________________________________________________________..
int TruthJetTagging::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthJetTagging::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthJetTagging::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TruthJetTagging::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void TruthJetTagging::Print(const std::string &what) const
{
  std::cout << "TruthJetTagging::Print(const std::string &what) const Printing info for " << what << std::endl;
}
