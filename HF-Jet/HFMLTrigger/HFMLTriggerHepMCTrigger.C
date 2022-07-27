#include "HFMLTriggerHepMCTrigger.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phhepmc/PHGenIntegral.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHTimeServer.h>
#include <phool/PHTimer.h>
#include <phool/getClass.h>

#include <phool/PHCompositeNode.h>

#include <pdbcalbase/PdbParameterMap.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>


#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CylinderCellGeom.h>
#include <g4detectors/PHG4CylinderCellGeomContainer.h>
#include <g4detectors/PHG4CylinderGeom.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>

#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <g4eval/SvtxEvalStack.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenRanges.h>
#include <HepMC/GenVertex.h>
#include <ffaobjects/FlagSavev1.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <TDatabasePDG.h>
#include <TFile.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TLorentzVector.h>
#include <TTree.h>

#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <boost/format.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

using namespace std;

HFMLTriggerHepMCTrigger::HFMLTriggerHepMCTrigger(const std::string& moduleName,
                                                 const std::string& filename)
  : SubsysReco(moduleName)
  , _ievent(0)
  , m_RejectReturnCode(Fun4AllReturnCodes::ABORTEVENT)
  , _f(nullptr)
  , _eta_min(-1)
  , _eta_max(1)
  , _embedding_id(1)
  , m_Geneventmap(nullptr)
  , m_Flags(nullptr)
  , m_hNorm(nullptr)
  , m_DRapidity(nullptr)
{
  _foutname = filename;
}

int HFMLTriggerHepMCTrigger::Init(PHCompositeNode* topNode)
{
  _ievent = 0;

  _f = new TFile((_foutname + string(".root")).c_str(), "RECREATE");

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

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerHepMCTrigger::InitRun(PHCompositeNode* topNode)
{
  m_Geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!m_Geneventmap)
  {
    std::cout << PHWHERE << " - Fatal error - missing node PHHepMCGenEventMap" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHNodeIterator iter(topNode);

  PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    throw std::runtime_error(
        "Failed to find DST node in RawTowerBuilder::CreateNodes");
  }

  m_Flags = findNode::getClass<PdbParameterMap>(dstNode, "HFMLTrigger_HepMCTriggerFlags");
  if (m_Flags == nullptr)
  {
    m_Flags = new PdbParameterMap();

    dstNode->addNode(new PHDataNode<PHObject>(m_Flags, "HFMLTrigger_HepMCTriggerFlags", "PHObject"));
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerHepMCTrigger::process_event(PHCompositeNode* topNode)
{
  assert(m_Geneventmap);

  PHHepMCGenEvent* genevt = m_Geneventmap->get(_embedding_id);
  if (!genevt)
  {
    std::cout << PHWHERE << " - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID " << _embedding_id;
    std::cout << ". Print PHHepMCGenEventMap:";
    m_Geneventmap->identify();
    return Fun4AllReturnCodes::ABORTRUN;
  }

  HepMC::GenEvent* theEvent = genevt->getEvent();
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

  TDatabasePDG* pdg = TDatabasePDG::Instance();

  int targetPID = std::abs(pdg->GetParticle("D0")->PdgCode());
  int daughter1PID = std::abs(pdg->GetParticle("pi+")->PdgCode());
  int daughter2PID = std::abs(pdg->GetParticle("K+")->PdgCode());

  bool acceptEvent = false;

  assert(m_hNorm);
  m_hNorm->Fill("Event", 1);

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

      if (decayVertex)
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
        }

        if (hasDecay1 == 1 and hasDecay2 == 1 and hasDecayOther == 0)
        {
          m_hNorm->Fill("D0->PiK", 1);
          ++nD0PiK;

          acceptEvent = true;

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
    cout <<"HFMLTriggerHepMCTrigger::process_event - D0-Pair with nD0 = "<<nD0<<endl;
    m_hNorm->Fill("D0-Pair", nD0 * (nD0 - 1) / 2);
  }
  if (nD0PiK >= 2)
  {
    m_hNorm->Fill("D0->PiK-Pair", nD0PiK * (nD0PiK - 1) / 2);
  }

  ++_ievent;

  if (Verbosity())
  {
    cout << "HFMLTriggerHepMCTrigger::process_event - acceptEvent = " << acceptEvent;
    cout << endl;

    if (acceptEvent)
    {
      cout << "HFMLTriggerHepMCTrigger::process_event - processed HepMC::GenEvent with signal_process_id = "
           << theEvent->signal_process_id();
      if (theEvent->signal_process_vertex())
      {
        cout << " and signal_process_vertex : ";
        theEvent->signal_process_vertex()->print();
      }
      cout << "  - Event record:" << endl;
      theEvent->print();
    }
  }

  assert(m_Flags);
  m_Flags->set_int_param(Name(), acceptEvent);

  if (acceptEvent)
  {
    m_hNorm->Fill("Accepted", 1);
    return Fun4AllReturnCodes::EVENT_OK;
  }
  else
    return m_RejectReturnCode;
}

int HFMLTriggerHepMCTrigger::End(PHCompositeNode* topNode)
{
  PHGenIntegral* integral_node = findNode::getClass<PHGenIntegral>(topNode, "PHGenIntegral");
  if (integral_node)
  {
    assert(m_hNorm);
    m_hNorm->Fill("IntegratedLumi", integral_node->get_Integrated_Lumi());
  }

  if (_f)
  {
    _f->cd();
    _f->Write();
    //_f->Close();

    //    m_hitLayerMap->Write();
  }

  cout << "HFMLTriggerHepMCTrigger::End - output to " << _foutname << ".*" << endl;

  return Fun4AllReturnCodes::EVENT_OK;
}
