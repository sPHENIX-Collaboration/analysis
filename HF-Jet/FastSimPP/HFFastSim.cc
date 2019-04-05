#include "HFFastSim.h"

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

#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <cmath>
#include <cstddef>
#include <iostream>

HFFastSim::HFFastSim(std::string filename, int flavor,
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

  _pt_min = 25;
  _pt_max = 100;

  _eta_min = -.6;
  _eta_max = +.6;

  _jet_name = jet_node;

  _rejection_action = Fun4AllReturnCodes::DISCARDEVENT;
}

int HFFastSim::Init(PHCompositeNode* topNode)
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

int HFFastSim::process_event(PHCompositeNode* topNode)
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

  if (Verbosity() >= HFFastSim::VERBOSITY_MORE)
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
      if (Verbosity() >= HFFastSim::VERBOSITY_MORE)
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
      if (Verbosity() >= HFFastSim::VERBOSITY_MORE)
      {
        this_jet->identify();
        std::cout << " --> this is flavor " << jet_flavor
                  << " like I want " << std::endl;
      }
    }
    else
    {
      if (Verbosity() >= HFFastSim::VERBOSITY_MORE)
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
    if (Verbosity() >= HFFastSim::VERBOSITY_SOME)
      std::cout << __PRETTY_FUNCTION__ << " --> FAIL due to max events = "
                << _total_pass << std::endl;
    _ievent++;
    return _rejection_action;
  }
  else if (pass_event)
  {
    _total_pass++;
    if (Verbosity() >= HFFastSim::VERBOSITY_SOME)
      std::cout << __PRETTY_FUNCTION__ << " --> PASS, total = " << _total_pass
                << std::endl;
    _ievent++;
    return Fun4AllReturnCodes::EVENT_OK;
  }
  else
  {
    if (Verbosity() >= HFFastSim::VERBOSITY_SOME)
      std::cout << __PRETTY_FUNCTION__ << " --> FAIL " << std::endl;
    _ievent++;
    return _rejection_action;
  }
}

int HFFastSim::End(PHCompositeNode* topNode)
{
  if (Verbosity() >= HFFastSim::VERBOSITY_SOME)
    std::cout << __PRETTY_FUNCTION__ << " DVP PASSED " << _total_pass
              << " events" << std::endl;

  _f->cd();
  _f->Write();
  //_f->Close();

  return 0;
}

