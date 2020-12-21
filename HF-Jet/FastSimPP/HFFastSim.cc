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

#include <gsl/gsl_const.h>

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
  , m_singleD(nullptr)
  , m_tSingleLc(nullptr)
  , m_singleLc(nullptr)
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
  m_hNorm->GetXaxis()->SetBinLabel(i++, "Lc");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "Lc->pPiK");
  m_hNorm->GetXaxis()->SetBinLabel(i++, "Accepted");

  m_hNorm->GetXaxis()->LabelsOption("v");

  m_DRapidity = new TH2F("hDRapidity",  //
                         "hDRapidity;Rapidity of D0 meson;Accepted", 1000, -5, 5, 2, -.5, 1.5);

  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

  m_tSingleD = new TTree("TSingleD", "TSingleD");

  m_singleD = new D02PiK;
  m_tSingleD->Branch("D02PiK", "HFFastSim::D02PiK", &m_singleD);

  m_tSingleLc = new TTree("TSingleLc", "TSingleLc");

  m_singleLc = new Lc2pPiK;
  m_tSingleLc->Branch("Lc2pPiK", "HFFastSim::Lc2pPiK", &m_singleLc);

  return 0;
}

std::pair<int, TString> HFFastSim::quark_trace(const HepMC::GenParticle* p, HepMC::GenEvent* theEvent)
{
  int max_abs_q_pid = 0;
  TString chain;

  while (p)
  {
    int pidabs = 0;

    TParticlePDG* pdg_p = TDatabasePDG::Instance()->GetParticle((p)->pdg_id());
    if (pdg_p)
    {
      chain = TString(" -> ") + TString(pdg_p->GetName()) + chain;

      if (TString(pdg_p->ParticleClass()).BeginsWith("B-"))
      {
        pidabs = TDatabasePDG::Instance()->GetParticle("b")->PdgCode();
      }
      else if (TString(pdg_p->ParticleClass()).BeginsWith("Charmed"))
      {
        pidabs = TDatabasePDG::Instance()->GetParticle("c")->PdgCode();
      }
    }
    else
    {
      chain = TString(" -> ") + Form("%d", (p)->pdg_id()) + chain;
    }

    if (pidabs > max_abs_q_pid) max_abs_q_pid = pidabs;

    const HepMC::GenVertex* vtx = p->production_vertex();
    if (vtx)
    {
      if (vtx->particles_in_size() == 1)
      {
        //trace parent
        p = *(vtx->particles_in_const_begin());
      }
      else
      {
        // vac, beam, or lund string
        //        vtx->print();

        break;
      }
    }
  }

  return make_pair(max_abs_q_pid, chain);
}

bool HFFastSim::process_D02PiK(HepMC::GenEvent* theEvent)
{
  assert(theEvent);

  const double mom_factor = HepMC::Units::conversion_factor(theEvent->momentum_unit(), HepMC::Units::GEV);
  const double length_factor = HepMC::Units::conversion_factor(theEvent->length_unit(), HepMC::Units::CM);
  //  const double time_factor = HepMC::Units::conversion_factor(theEvent->length_unit(), HepMC::Units::CM) / GSL_CONST_CGS_SPEED_OF_LIGHT * 1e9;  // from length_unit()/c to ns

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
      if (Verbosity() >= VERBOSITY_MORE)
      {
        cout << "process_D02PiK - Accept signal particle : ";
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

          if (Verbosity() >= VERBOSITY_MORE)
          {
            cout << "process_D02PiK - Testing daughter particle: ";
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

          assert(m_singleD);
          m_singleD->pid = p->pdg_id();
          m_singleD->y = rapidity;
          m_singleD->pt = p->momentum().perp() * mom_factor;

          m_singleD->decayL = decayVertex->point3d().r() * length_factor;
          m_singleD->prodL = p->production_vertex()->point3d().r() * length_factor;

          auto trace = quark_trace(p, theEvent);
          m_singleD->hadron_origion_qpid = trace.first;
          m_singleD->decayChain = trace.second;

          if (Verbosity())
          {
            cout << "process_D02PiK - Found D0->piK:";
            cout << "m_singleD->pid = " << m_singleD->pid << ", ";
            cout << "m_singleD->y = " << m_singleD->y << ", ";
            cout << "m_singleD->pt = " << m_singleD->pt;
            cout << " origin " << m_singleD->decayChain << endl;
          }

          assert(m_tSingleD);
          m_tSingleD->Fill();
          m_singleD->Clear();

          m_DRapidity->Fill(rapidity, 1);
        }

      }  //      if (decayVertex)
      else
      {
        cout << "process_D02PiK - Warning - target particle did not have decay vertex : ";
        p->print();
        cout << endl;
      }

    }  //    if (std::abs(p-> pdg_id()) == targetPID)
  }    //  for (HepMC::GenEvent::particle_const_iterator piter = range.begin(); piter != range.end(); ++piter)

  if (nD0 >= 2)
  {
    if (Verbosity() >= VERBOSITY_MORE)
      cout << "process_D02PiK - D0-Pair with nD0 = " << nD0 << endl;
    m_hNorm->Fill("D0-Pair", nD0 * (nD0 - 1) / 2);
  }
  if (nD0PiK >= 2)
  {
    m_hNorm->Fill("D0->PiK-Pair", nD0PiK * (nD0PiK - 1) / 2);
  }

  return nD0PiK >= 1;
}

bool HFFastSim::process_Lc2pPiK(HepMC::GenEvent* theEvent)
{
  assert(theEvent);

  const double mom_factor = HepMC::Units::conversion_factor(theEvent->momentum_unit(), HepMC::Units::GEV);
  const double length_factor = HepMC::Units::conversion_factor(theEvent->length_unit(), HepMC::Units::CM);
  //  const double time_factor = HepMC::Units::conversion_factor(theEvent->length_unit(), HepMC::Units::CM) / GSL_CONST_CGS_SPEED_OF_LIGHT * 1e9;  // from length_unit()/c to ns

  TDatabasePDG* pdg = TDatabasePDG::Instance();

  const int targetPID = std::abs(pdg->GetParticle("Lambda_c+")->PdgCode());
  assert(targetPID == 4122);
  const int daughter1PID = std::abs(pdg->GetParticle("pi+")->PdgCode());
  assert(daughter1PID == 211);
  const int daughter2PID = std::abs(pdg->GetParticle("K+")->PdgCode());
  assert(daughter2PID == 321);
  const int daughter3PID = std::abs(pdg->GetParticle("proton")->PdgCode());
  assert(daughter3PID == 2212);

  unsigned int nLc(0);
  unsigned int nLc2pPiK(0);

  auto range = theEvent->particle_range();
  for (HepMC::GenEvent::particle_const_iterator piter = range.begin(); piter != range.end(); ++piter)
  {
    const HepMC::GenParticle* p = *piter;
    assert(p);

    if (std::abs(p->pdg_id()) == targetPID)
    {
      if (Verbosity() >= VERBOSITY_MORE)
      {
        cout << "HFFastSim::process_Lc2pPiK - Accept signal particle : ";
        p->print();
        cout << endl;
      }

      m_hNorm->Fill("Lc", 1);
      ++nLc;

      const double rapidity = 0.5 * log((p->momentum().e() + p->momentum().z()) /
                                        (p->momentum().e() - p->momentum().z()));

      const HepMC::GenVertex* decayVertex = p->end_vertex();

      int hasDecay1(0);
      int hasDecay2(0);
      int hasDecay3(0);
      int hasDecayOther(0);

      if (decayVertex)  // decay
      {
        for (auto diter = decayVertex->particles_out_const_begin();
             diter != decayVertex->particles_out_const_end();
             ++diter)

        {
          const HepMC::GenParticle* pd = *diter;
          assert(pd);

          if (Verbosity() >= VERBOSITY_MORE)
          {
            cout << "HFFastSim::process_Lc2pPiK - Testing daughter particle: ";
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
          else if (std::abs(pd->pdg_id()) == daughter3PID)
          {
            const double eta = pd->momentum().eta();

            if (eta > _eta_min and eta < _eta_max)
              ++hasDecay3;
          }
          else
            ++hasDecayOther;
        }  // for ...

        // found a D0->piK
        if (hasDecay1 == 1 and hasDecay2 == 1 and hasDecay3 == 1 and hasDecayOther == 0)
        {
          m_hNorm->Fill("Lc->pPiK", 1);
          ++nLc2pPiK;

          assert(m_singleLc);
          m_singleLc->pid = p->pdg_id();
          m_singleLc->y = rapidity;
          m_singleLc->pt = p->momentum().perp() * mom_factor;

          m_singleLc->decayL = decayVertex->point3d().r() * length_factor;
          m_singleLc->prodL = p->production_vertex()->point3d().r() * length_factor;

          auto trace = quark_trace(p, theEvent);
          m_singleLc->hadron_origion_qpid = trace.first;
          m_singleLc->decayChain = trace.second;

          if (Verbosity())
          {
            cout << "HFFastSim::process_Lc2pPiK - Found D0->piK:";
            cout << "m_singleLc->pid = " << m_singleLc->pid << ", ";
            cout << "m_singleLc->y = " << m_singleLc->y << ", ";
            cout << "m_singleLc->pt = " << m_singleLc->pt;
            cout << " origin " << m_singleLc->decayChain << endl;
          }

          assert(m_tSingleLc);
          m_tSingleLc->Fill();
          m_singleLc->Clear();

          m_DRapidity->Fill(rapidity, 1);
        }

      }  //      if (decayVertex)
      else
      {
        cout << "HFFastSim::process_Lc2pPiK - Warning - target particle did not have decay vertex : ";
        p->print();
        cout << endl;
      }

    }  //    if (std::abs(p-> pdg_id()) == targetPID)
  }    //  for (HepMC::GenEvent::particle_const_iterator piter = range.begin(); piter != range.end(); ++piter)

  return nLc2pPiK >= 1;
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
    cout << "HFFastSim::process_event - process HepMC::GenEvent with signal_process_id = "
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
  process_Lc2pPiK(theEvent);

  if (pass_event && _total_pass >= _maxevent)
  {
    if (Verbosity() >= HFFastSim::VERBOSITY_MORE)
      std::cout << __PRETTY_FUNCTION__ << " --> FAIL due to max events = "
                << _total_pass << std::endl;
    _ievent++;
    return _rejection_action;
  }
  else if (pass_event)
  {
    _total_pass++;
    if (Verbosity() >= HFFastSim::VERBOSITY_MORE)
      std::cout << __PRETTY_FUNCTION__ << " --> PASS, total = " << _total_pass
                << std::endl;
    _ievent++;
    m_hNorm->Fill("Accepted", 1);
    return Fun4AllReturnCodes::EVENT_OK;
  }
  else
  {
    if (Verbosity() >= HFFastSim::VERBOSITY_MORE)
      std::cout << __PRETTY_FUNCTION__ << " --> FAIL " << std::endl;
    _ievent++;
    return _rejection_action;
  }
}

int HFFastSim::End(PHCompositeNode* topNode)
{
  if (Verbosity() >= HFFastSim::VERBOSITY_SOME)
    std::cout << __PRETTY_FUNCTION__ << " PASSED " << _total_pass
              << " events" << std::endl;

  PHGenIntegral* integral_node = findNode::getClass<PHGenIntegral>(topNode, "PHGenIntegral");
  if (integral_node)
  {
    integral_node->identify();
    assert(m_hNorm);
    m_hNorm->Fill("IntegratedLumi", integral_node->get_Integrated_Lumi());
  }

  _f->cd();
  _f->Write();
  //_f->Close();

  return 0;
}
