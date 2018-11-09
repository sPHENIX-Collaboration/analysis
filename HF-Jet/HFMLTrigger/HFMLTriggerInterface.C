#include "HFMLTriggerInterface.h"

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

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

// Short alias for this namespace
namespace pt = boost::property_tree;

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

using namespace std;

HFMLTriggerInterface::HFMLTriggerInterface(std::string filename)
  : SubsysReco("HFMLTriggerInterface")
  , _ievent(0)
  , _f(nullptr)
  , _eta_min(-1)
  , _eta_max(1)
  , _embedding_id(0)
{
  _foutname = filename;
}

int HFMLTriggerInterface::Init(PHCompositeNode* topNode)
{
  _ievent = 0;

  _f = new TFile((_foutname + string(".root")).c_str(), "RECREATE");

  m_jsonOut.open((_foutname + string(".json")).c_str(), fstream::out);

  //  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  //  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

  return 0;
}

int HFMLTriggerInterface::process_event(PHCompositeNode* topNode)
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
  assert(theEvent);
  if (Verbosity())
    theEvent->print();

  // Create a root
  pt::ptree json;

  json.put("EventID", _ievent);

  assert(m_jsonOut.is_open());
  pt::write_json(m_jsonOut, json);

  ++_ievent;

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFMLTriggerInterface::End(PHCompositeNode* topNode)
{
  if (_f)
  {
    _f->cd();
    _f->Write();
    //_f->Close();
  }

  if (m_jsonOut.is_open())
    m_jsonOut.close();

  cout << "HFMLTriggerInterface::End - output to " << _foutname << ".*" << endl;

  return Fun4AllReturnCodes::EVENT_OK;
}
