#include "HFJetTruthTrigger.h"

#include "HFJetDefs.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH2D.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TTree.h>
#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#pragma GCC diagnostic pop
#include <cmath>
#include <cstddef>
#include <iostream>

HFJetTruthTrigger::HFJetTruthTrigger(std::string filename, int flavor,
                                     std::string jet_node, int maxevent)
  : SubsysReco("HFJetTagger_" + jet_node)
  , _verbose(0)
  , _ievent(0)
  , _total_pass(0)
  , _f(nullptr)
  , _h2(nullptr)
  , _h2all(nullptr)
  , _h2_b(nullptr)
  , _h2_c(nullptr)
  , _embedding_id(1)
{
  _foutname = filename;

  _flavor = flavor;

  _maxevent = maxevent;

  _pt_min = 20;
  _pt_max = 100;

  _eta_min = -.7;
  _eta_max = +.7;

  _jet_name = jet_node;

  _rejection_action = Fun4AllReturnCodes::DISCARDEVENT;
}

int HFJetTruthTrigger::Init(PHCompositeNode* topNode)
{
  _verbose = true;

  _ievent = 0;

  _total_pass = 0;

  _f = new TFile(_foutname.c_str(), "RECREATE");

  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

  return 0;
}

int HFJetTruthTrigger::process_event(PHCompositeNode* topNode)
{
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
  //theEvent->print();

  JetMap* truth_jets = findNode::getClass<JetMap>(topNode, _jet_name);
  if (!truth_jets)
  {
    std::cout << PHWHERE << " - Fatal error - node " << _jet_name << " JetMap missing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  const double jet_radius = truth_jets->get_par();

  if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
    std::cout << __PRETTY_FUNCTION__ << ": truth jets has size "
              << truth_jets->size() << " and R = " << jet_radius << std::endl;

  int ijet_t = 0;
  bool pass_event = false;
  for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end();
       ++iter)
  {
    Jet* this_jet = iter->second;

    float this_pt = this_jet->get_pt();
    float this_eta = this_jet->get_eta();

    if (this_pt < 10 || fabs(this_eta) > 5)
      continue;

    _h2all->Fill(this_jet->get_pt(), this_eta);

    if (this_pt > _pt_min && this_pt < _pt_max && (this_eta) > _eta_min && (this_eta) < _eta_max)
    {
      //pass_event = true;
      _h2->Fill(this_jet->get_pt(), this_eta);
      if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
        this_jet->identify();
    }
    else
    {
      continue;
    }

    const int jet_flavor = parton_tagging(this_jet, theEvent, jet_radius);

    if (abs(jet_flavor) == _flavor)
    {
      pass_event = true;
      if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
      {
        this_jet->identify();
        std::cout << " --> this is flavor " << jet_flavor
                  << " like I want " << std::endl;
      }
    }
    else
    {
      if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
      {
        this_jet->identify();
        std::cout << " --> this is flavor " << jet_flavor
                  << " which I do NOT want " << std::endl;
      }
    }

    hadron_tagging(this_jet, theEvent, jet_radius);

    ijet_t++;
  }

  if (pass_event && _total_pass >= _maxevent)
  {
    if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_SOME)
      std::cout << __PRETTY_FUNCTION__ << " --> FAIL due to max events = "
                << _total_pass << std::endl;
    _ievent++;
    return _rejection_action;
  }
  else if (pass_event)
  {
    _total_pass++;
    if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_SOME)
      std::cout << __PRETTY_FUNCTION__ << " --> PASS, total = " << _total_pass
                << std::endl;
    _ievent++;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  else
  {
    if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_SOME)
      std::cout << __PRETTY_FUNCTION__ << " --> FAIL " << std::endl;
    _ievent++;
    return _rejection_action;
  }
}

int HFJetTruthTrigger::End(PHCompositeNode* topNode)
{
  if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_SOME)
    std::cout << __PRETTY_FUNCTION__ << " DVP PASSED " << _total_pass
              << " events" << std::endl;

  _f->cd();
  _f->Write();
  //_f->Close();

  return 0;
}

//! tag jet flavor by parton matching, like PRL 113, 132301 (2014)
int HFJetTruthTrigger::parton_tagging(Jet* this_jet, HepMC::GenEvent* theEvent,
                                      const double match_radius)
{
  float this_pt = this_jet->get_pt();
  float this_phi = this_jet->get_phi();
  float this_eta = this_jet->get_eta();

  int jet_flavor = 0;
  double jet_parton_zt = 0;

  //std::cout << " truth jet #" << ijet_t << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << ", checking flavor" << std::endl;

  //TODO: lack taggign scheme of gluon splitting -> QQ_bar
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
  {
    float dR = deltaR((*p)->momentum().pseudoRapidity(), this_eta,
                      (*p)->momentum().phi(), this_phi);
    if (dR > match_radius)
      continue;

    int pidabs = abs((*p)->pdg_id());
    const double zt = (*p)->momentum().perp() / this_pt;

    if (pidabs == TDatabasePDG::Instance()->GetParticle("c")->PdgCode()      //
        or pidabs == TDatabasePDG::Instance()->GetParticle("b")->PdgCode()   //
        or pidabs == TDatabasePDG::Instance()->GetParticle("t")->PdgCode())  // handle heavy quarks only. All other favor tagged as default 0
    {
      if (pidabs > abs(jet_flavor))  // heavy quark found
      {
        jet_parton_zt = zt;
        jet_flavor = (*p)->pdg_id();
      }
      else if (pidabs == abs(jet_flavor))  // same quark mass. next compare zt
      {
        if (zt > jet_parton_zt)
        {
          jet_parton_zt = zt;
          jet_flavor = (*p)->pdg_id();
        }
      }

      if (pidabs == TDatabasePDG::Instance()->GetParticle("b")->PdgCode())
      {
        if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
          std::cout << __PRETTY_FUNCTION__
                    << " --BOTTOM--> pt / eta / phi = "
                    << (*p)->momentum().perp() << " / "
                    << (*p)->momentum().pseudoRapidity() << " / "
                    << (*p)->momentum().phi() << std::endl;
      }
      else if (pidabs == TDatabasePDG::Instance()->GetParticle("c")->PdgCode())
      {
        if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
          std::cout << __PRETTY_FUNCTION__
                    << " --CHARM --> pt / eta / phi = "
                    << (*p)->momentum().perp() << " / "
                    << (*p)->momentum().pseudoRapidity() << " / "
                    << (*p)->momentum().phi() << std::endl;
      }
    }
  }  //       for (HepMC::GenEvent::particle_const_iterator p =

  if (abs(jet_flavor) == TDatabasePDG::Instance()->GetParticle("b")->PdgCode())
  {
    _h2_b->Fill(this_jet->get_pt(), this_eta);
  }
  else if (abs(jet_flavor) == TDatabasePDG::Instance()->GetParticle("c")->PdgCode())
  {
    _h2_c->Fill(this_jet->get_pt(), this_eta);
  }

  this_jet->set_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor),
                         jet_flavor);
  this_jet->set_property(static_cast<Jet::PROPERTY>(prop_JetPartonZT),
                         jet_parton_zt);
  //          this_jet->identify();

  return jet_flavor;
}

int HFJetTruthTrigger::hadron_tagging(Jet* this_jet, HepMC::GenEvent* theEvent,
                                      const double match_radius)
{
  float this_pt = this_jet->get_pt();
  float this_phi = this_jet->get_phi();
  float this_eta = this_jet->get_eta();

  int jet_flavor = 0;
  double jet_parton_zt = 0;

  //std::cout << " truth jet #" << ijet_t << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << ", checking flavor" << std::endl;

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

      if (pidabs == TDatabasePDG::Instance()->GetParticle("b")->PdgCode())
      {
        if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
        {
          std::cout << __PRETTY_FUNCTION__
                    << " --BOTTOM--> pt / eta / phi = "
                    << (*p)->momentum().perp() << " / "
                    << (*p)->momentum().pseudoRapidity() << " / "
                    << (*p)->momentum().phi() << " with hadron ";
          pdg_p->Print();
        }
      }
      else if (pidabs == TDatabasePDG::Instance()->GetParticle("c")->PdgCode())
      {
        if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
        {
          std::cout << __PRETTY_FUNCTION__
                    << " --CHARM --> pt / eta / phi = "
                    << (*p)->momentum().perp() << " / "
                    << (*p)->momentum().pseudoRapidity() << " / "
                    << (*p)->momentum().phi() << " with hadron ";
          pdg_p->Print();
        }
      }
    }
  }  //       for (HepMC::GenEvent::particle_const_iterator p =

  this_jet->set_property(static_cast<Jet::PROPERTY>(prop_JetHadronFlavor),
                         jet_flavor);
  this_jet->set_property(static_cast<Jet::PROPERTY>(prop_JetHadronZT),
                         jet_parton_zt);
  //          this_jet->identify();

  if (Verbosity() >= HFJetTruthTrigger::VERBOSITY_MORE)
    std::cout << __PRETTY_FUNCTION__ << " jet_flavor = " << jet_flavor
              << std::endl;

  return jet_flavor;
}
