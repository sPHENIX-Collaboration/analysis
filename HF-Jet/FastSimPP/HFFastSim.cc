#include "HFFastSim.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <phhepmc/PHGenIntegral.h>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TTree.h>
#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenRanges.h>
#include <HepMC/GenVertex.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

using namespace std;

HFFastSim::HFFastSim(std::string filename, int flavor, int maxevent)
  : SubsysReco("HFFastSim")
  , _verbose(0)
  , _ievent(0)
  , _total_pass(0)
  , _f(nullptr)
  , _h2(nullptr)
  , _h2all(nullptr)
  , _h2_b(nullptr)
  , _h2_c(nullptr)
  , m_hNorm(nullptr)
  , m_DRapidity(nullptr)
  , m_tSingleD(nullptr)
  , _embedding_id(1)
{
  _foutname = filename;

  _flavor = flavor;

  _maxevent = maxevent;

  _pt_min = 0;
  _pt_max = 100;

  _eta_min = -1.1;
  _eta_max = +1.1;

  _rejection_action = Fun4AllReturnCodes::DISCARDEVENT;
}

int HFFastSim::Init(PHCompositeNode* topNode)
{
  _verbose = true;

  _ievent = 0;

  _total_pass = 0;

  _f = new TFile(_foutname.c_str(), "RECREATE");

  m_hNorm = new TH1D("hNormalization",  //
                     "Normalization;Items;Summed quantity", 10, .5, 10.5);
  int i = 1;
  m_hNorm->GetXaxis()->SetBinLabel(i++, "IntegratedLumi");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "Event");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "D0");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "D0->PiK");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "D0-Pair");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "D0->PiK-Pair");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "Accepted");

  m_hNorm->GetXaxis()->LabelsOption("v");

  m_DRapidity = new TH2F("hDRapidity",  //
                         "hDRapidity;Rapidity of D0 meson;Accepted", 1000, -5, 5, 2, -.5, 1.5);

  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

  m_tSingleD = new TTree("TSingleD", "TSingleD");
  m_tSingleD->Branch("D02PiK", &m_singleD.pid, "pid/I:y/D");

  return 0;
}

bool HFFastSim::process_D02PiK(HepMC::GenEvent* theEvent)
{
  TDatabasePDG* pdg = TDatabasePDG::Instance();

  int targetPID = std::abs(pdg->GetParticle("D0")->PdgCode());
  int daughter1PID = std::abs(pdg->GetParticle("pi+")->PdgCode());
  int daughter2PID = std::abs(pdg->GetParticle("K+")->PdgCode());

  unsigned int nD0(0);
  unsigned int nD0PiK(0);

  auto range = theEvent->particle_range();
  for (HepMC::GenEvent::particle_const_iterator piter = range.begin(); piter != range.end(); ++piter)
  {
    const HepMC::GenParticle* p = *piter;
    assert(p);

    if (std::abs(p->pdg_id()) == targetPID)
    {
      if (Verbosity())
      {
        cout << "HFMLTriggerHepMCTrigger::process_event - Accept signal particle : ";
        p->print();
        cout << endl;
      }

      m_hNorm->Fill("D0", 1);
      ++nD0;

      assert(m_DRapidity);
      const double rapidity = 0.5 * log((p->momentum().e() + p->momentum().z()) /
                                        (p->momentum().e() - p->momentum().z()));

      m_DRapidity->Fill(rapidity, 0);

      const HepMC::GenVertex* decayVertex = p->end_vertex();

      int hasDecay1(0);
      int hasDecay2(0);
      int hasDecayOther(0);

      if (decayVertex)  // decay
      {
        for (auto diter = decayVertex->particles_out_const_begin();
             diter != decayVertex->particles_out_const_end();
             ++diter)

        {
          const HepMC::GenParticle* pd = *diter;
          assert(pd);

          if (Verbosity())
          {
            cout << "HFMLTriggerHepMCTrigger::process_event - Testing daughter particle: ";
            pd->print();
            cout << endl;
          }

          if (std::abs(pd->pdg_id()) == daughter1PID)
          {
            const double eta = pd->momentum().eta();

            if (eta > _eta_min and eta < _eta_max)
              ++hasDecay1;
          }
          else if (std::abs(pd->pdg_id()) == daughter2PID)
          {
            const double eta = pd->momentum().eta();

            if (eta > _eta_min and eta < _eta_max)
              ++hasDecay2;
          }
          else
            ++hasDecayOther;
        }  // for ...

        // found a D0->piK
        if (hasDecay1 == 1 and hasDecay2 == 1 and hasDecayOther == 0)
        {
          m_hNorm->Fill("D0->PiK", 1);
          ++nD0PiK;

          m_singleD = {p->pdg_id(), rapidity};
          assert(m_tSingleD);
          m_tSingleD->Fill();

          m_DRapidity->Fill(rapidity, 1);
        }

      }  //      if (decayVertex)
      else
      {
        cout << "HFMLTriggerHepMCTrigger::process_event - Warning - target particle did not have decay vertex : ";
        p->print();
        cout << endl;
      }

    }  //    if (std::abs(p-> pdg_id()) == targetPID)
  }    //  for (HepMC::GenEvent::particle_const_iterator piter = range.begin(); piter != range.end(); ++piter)

  if (nD0 >= 2)
  {
    if (Verbosity())
      cout << "HFMLTriggerHepMCTrigger::process_event - D0-Pair with nD0 = " << nD0 << endl;
    m_hNorm->Fill("D0-Pair", nD0 * (nD0 - 1) / 2);
  }
  if (nD0PiK >= 2)
  {
    m_hNorm->Fill("D0->PiK-Pair", nD0PiK * (nD0PiK - 1) / 2);
  }

  return nD0PiK >= 1;
}

int HFFastSim::process_event(PHCompositeNode* topNode)
{
  assert(m_hNorm);
  m_hNorm->Fill("Event", 1);

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
  assert(theEvent);

  if (Verbosity() >= VERBOSITY_MORE)
  {
    cout << "HFMLTriggerHepMCTrigger::process_event - process HepMC::GenEvent with signal_process_id = "
         << theEvent->signal_process_id();
    if (theEvent->signal_process_vertex())
    {
      cout << " and signal_process_vertex : ";
      theEvent->signal_process_vertex()->print();
    }
    cout << "  - Event record:" << endl;
    theEvent->print();
  }

  // process quarks
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
  {
    int pidabs = abs((*p)->pdg_id());
    if (pidabs == TDatabasePDG::Instance()->GetParticle("c")->PdgCode()      //
        or pidabs == TDatabasePDG::Instance()->GetParticle("b")->PdgCode()   //
        or pidabs == TDatabasePDG::Instance()->GetParticle("t")->PdgCode())  // handle heavy quarks only. All other favor tagged as default 0
    {
      if (pidabs == TDatabasePDG::Instance()->GetParticle("b")->PdgCode())
      {
        if (Verbosity() >= VERBOSITY_MORE)
          std::cout << __PRETTY_FUNCTION__
                    << " --BOTTOM--> pt / eta / phi = "
                    << ((*p)->momentum().perp()) << " / "
                    << (*p)->momentum().pseudoRapidity() << " / "
                    << (*p)->momentum().phi() << std::endl;

        _h2_b->Fill((*p)->momentum().perp(), (*p)->momentum().pseudoRapidity());
      }
      else if (pidabs == TDatabasePDG::Instance()->GetParticle("c")->PdgCode())
      {
        if (Verbosity() >= VERBOSITY_MORE)
          std::cout << __PRETTY_FUNCTION__
                    << " --CHARM --> pt / eta / phi = "
                    << (*p)->momentum().perp() << " / "
                    << (*p)->momentum().pseudoRapidity() << " / "
                    << (*p)->momentum().phi() << std::endl;

        _h2_c->Fill((*p)->momentum().perp(), (*p)->momentum().pseudoRapidity());
      }
    }

  }  //       for (HepMC::GenEvent::particle_const_iterator p =

  bool pass_event = process_D02PiK(theEvent);

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
    m_hNorm->Fill("Accepted", 1);
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

  PHGenIntegral* integral_node = findNode::getClass<PHGenIntegral>(topNode, "PHGenIntegral");
  if (integral_node)
  {
    assert(m_hNorm);
    m_hNorm->Fill("IntegratedLumi", integral_node->get_Integrated_Lumi());
  }

  _f->cd();
  _f->Write();
  //_f->Close();

  return 0;
}
