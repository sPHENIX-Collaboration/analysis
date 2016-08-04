#include "HFJetTruthTrigger.h"

#include "HFJetDefs.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>

#include <TLorentzVector.h>
#include <iostream>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

HFJetTruthTrigger::HFJetTruthTrigger(std::string filename, int flavor, int maxevent) :
    SubsysReco("HFJetTagger")
{

  _foutname = filename;

  _flavor = flavor;

  _maxevent = maxevent;

}

int
HFJetTruthTrigger::Init(PHCompositeNode *topNode)
{

  _verbose = true;

  _ievent = 0;

  _total_pass = 0;

  //_f = new TFile( _foutname.c_str(), "RECREATE");

  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

  return 0;

}

int
HFJetTruthTrigger::process_event(PHCompositeNode *topNode)
{

  if ((_ievent % 1000) == 0)
    std::cout << "DVP : at process_event #" << _ievent << std::endl;

  PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topNode,
      "PHHepMCGenEvent");
  HepMC::GenEvent* theEvent = genevt->getEvent();
  //theEvent->print();

  JetMap* truth_jets = findNode::getClass<JetMap>(topNode, "AntiKt_Truth_r04");

  //std::cout << "truth jets has size " << truth_jets->size() << std::endl;

  int ijet_t = 0;
  bool pass_event = false;
  for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end();
      ++iter)
    {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_pt < 10 || fabs(this_eta) > 5)
        continue;

      if (this_pt > 25 && this_pt < 30 && fabs(this_eta) < 0.6)
        {
          //pass_event = true;
          _h2->Fill(this_jet->get_pt(), this_eta);
        }
      else
        {
          continue;
        }

      int jet_flavor = 0;

      _h2all->Fill(this_jet->get_pt(), this_eta);

      //std::cout << " truth jet #" << ijet_t << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << ", checking flavor" << std::endl;
        {
          for (HepMC::GenEvent::particle_const_iterator p =
              theEvent->particles_begin(); p != theEvent->particles_end(); ++p)
            {

              float dR = deltaR((*p)->momentum().pseudoRapidity(), this_eta,
                  (*p)->momentum().phi(), this_phi);
              if (dR > 0.4)
                continue;

              int pidabs = abs((*p)->pdg_id());
              // b-jet overrides everything
              if (pidabs == 5)
                {
                  //std::cout << " --BOTTOM--> pt / eta / phi = " <<  (*p)->momentum().perp() << " / " << (*p)->momentum().pseudoRapidity() << " / " << (*p)->momentum().phi() << std::endl;
                  //(*p)->print();
                  jet_flavor = (*p)->pdg_id();

                }
              else if (pidabs == 4 && pidabs > abs(jet_flavor))
                {
                  //std::cout << " --CHARM --> pt / eta / phi = " <<  (*p)->momentum().perp() << " / " << (*p)->momentum().pseudoRapidity() << " / " << (*p)->momentum().phi() << std::endl;
                  //(*p)->print();
                  jet_flavor = (*p)->pdg_id();

                }
            }


          this_jet->set_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor), jet_flavor);
//          this_jet->identify();

        }

      if (abs(jet_flavor) == _flavor)
        {
          pass_event = true;
          //std::cout << " --> this is flavor " << jet_flavor << " like I want " << std::endl;
        }
      else
        {
          //std::cout << " --> this is flavor " << jet_flavor << " which I do NOT want " << std::endl;
        }

      ijet_t++;
    }

  if (pass_event && _total_pass >= _maxevent)
    {
      std::cout << " --> FAIL due to max events = " << _total_pass << std::endl;
      _ievent++;
      return -1;
    }
  else if (pass_event)
    {
      _total_pass++;
      std::cout << " --> PASS, total = " << _total_pass << std::endl;
      _ievent++;
      return 0;
    }
  else
    {
      //std::cout << " --> FAIL " << std::endl;
      _ievent++;
      return -1;
    }
}

int
HFJetTruthTrigger::End(PHCompositeNode *topNode)
{

  std::cout << " DVP PASSED " << _total_pass << " events" << std::endl;

  //_f->Write();
  //_f->Close();

  return 0;
}

