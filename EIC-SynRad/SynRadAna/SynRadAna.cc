

#include "SynRadAna.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHDataNode.h>
#include <phool/PHNode.h>  // for PHNode
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/PHRandomSeed.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/recoConsts.h>

#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h>
#include <HepMC/Units.h>

#include <gsl/gsl_const.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>

#include <TAxis.h>
#include <TDatabasePDG.h>
#include <TH1.h>
#include <TH2.h>
#include <TNamed.h>
#include <TString.h>
#include <TVector3.h>

#include <phool/PHCompositeNode.h>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <list>
#include <utility>

using namespace std;

//____________________________________________________________________________..
SynRadAna::SynRadAna(const std::string &name)
  : SubsysReco(name)
  , _embedding_id(0)
{
  if (Verbosity())
    cout << "SynRadAna::SynRadAna(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
SynRadAna::~SynRadAna()
{
  if (Verbosity())
    cout << "SynRadAna::~SynRadAna() Calling dtor" << endl;
}

//____________________________________________________________________________..
int SynRadAna::Init(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::Init(PHCompositeNode *topNode) Initializing" << endl;

  Fun4AllHistoManager *hm = getHistoManager();
  assert(hm);

  TH1 *h(nullptr);

  // n events and n tracks histogram
  h = new TH1F(TString(get_histo_prefix()) + "Normalization",
               TString(get_histo_prefix()) + " Normalization;Items;Count", 10, .5, 10.5);
  int i = 1;
  h->GetXaxis()->SetBinLabel(i++, "Event");
  h->GetXaxis()->SetBinLabel(i++, "Photon");
  h->GetXaxis()->SetBinLabel(i++, "Flux");
  h->GetXaxis()->LabelsOption("v");
  hm->registerHisto(h);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::process_event(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::process_event(PHCompositeNode *topNode) Processing Event" << endl;

  // histogram manager
  Fun4AllHistoManager *hm = getHistoManager();
  assert(hm);
  // n events and n tracks histogram
  TH1 *h_norm = dynamic_cast<TH1 *>(hm->getHisto(get_histo_prefix() + "Normalization"));
  assert(h_norm);
  h_norm->Fill("Event", 1);

  // For pile-up simulation: define GenEventMap
  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  assert(genevtmap);
  PHHepMCGenEvent *genev = genevtmap->get(_embedding_id);
  assert(genev);
  HepMC::GenEvent *hepmc_evt = genev->getEvent();
  assert(hepmc_evt);

  if (Verbosity() >= 2)
  {
    hepmc_evt->print();
  }

  const auto &weightcontainer = hepmc_evt->weights();
  assert(weightcontainer.size() >= 2);

  h_norm->Fill("Flux", weightcontainer[0]);
  h_norm->Fill("Photon", weightcontainer[1]);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::EndRun(const int runnumber)
{
  if (Verbosity())
    cout << "SynRadAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::End(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::End(PHCompositeNode *topNode) This is the End..." << endl;

  getHistoManager()->dumpHistos(string(Name()) + ".root");

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SynRadAna::Reset(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "SynRadAna::Reset(PHCompositeNode *topNode) being Reset" << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void SynRadAna::Print(const std::string &what) const
{
  cout << "SynRadAna::Print(const std::string &what) const Printing info for " << what << endl;
}

//! Get a pointer to the default hist manager for QA modules
Fun4AllHistoManager *SynRadAna::
    getHistoManager()
{
  static const string HistoManagerName("HistoManager_SynRadAna");

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager(HistoManagerName);

  if (not hm)
  {
    //        cout
    //            << "QAHistManagerDef::get_HistoManager - Making Fun4AllHistoManager EMCalAna_HISTOS"
    //            << endl;
    hm = new Fun4AllHistoManager(HistoManagerName);
    se->registerHistoManager(hm);
  }

  assert(hm);

  return hm;
}

string
SynRadAna::get_histo_prefix()
{
  return string("h_") + Name() + string("_");
}
