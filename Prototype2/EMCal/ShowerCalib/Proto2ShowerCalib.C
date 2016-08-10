#include "Proto2ShowerCalib.h"

#include <prototype2/RawTower_Temperature.h>
#include <prototype2/RawTower_Prototype2.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>

#include <TNtuple.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TLorentzVector.h>

#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

Proto2ShowerCalib::Proto2ShowerCalib(const std::string &filename) :
    SubsysReco("Proto2ShowerCalib"), _filename(filename), _ievent(0)
{

  verbosity = 1;

}

Proto2ShowerCalib::~Proto2ShowerCalib()
{
}

int
Proto2ShowerCalib::InitRun(PHCompositeNode *topNode)
{
  if (verbosity)
    cout << "Proto2ShowerCalib::InitRun" << endl;

  _ievent = 0;

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = static_cast<PHCompositeNode*>(iter.findFirst(
      "PHCompositeNode", "DST"));
  if (!dstNode)
    {
      std::cerr << PHWHERE << "DST Node missing, doing nothing." << std::endl;
      throw runtime_error(
          "Failed to find DST node in EmcRawTowerBuilder::CreateNodes");
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto2ShowerCalib::End(PHCompositeNode *topNode)
{
  cout << "Proto2ShowerCalib::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

//  if (_T_EMCalTrk)
//    _T_EMCalTrk->Write();

// help index files with TChain
  TTree * T_Index = new TTree("T_Index", "T_Index");
  assert(T_Index);
  T_Index->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto2ShowerCalib::Init(PHCompositeNode *topNode)
{

  _ievent = 0;

  cout << "Proto2ShowerCalib::get_HistoManager - Making PHTFileServer "
      << _filename << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  return Fun4AllReturnCodes::EVENT_OK;
}

int
Proto2ShowerCalib::process_event(PHCompositeNode *topNode)
{

  if (verbosity > 2)
    cout << "Proto2ShowerCalib::process_event() entered" << endl;

  RawTower_Prototype2* TOWER_CALIB_TRIGGER_VETO = findNode::getClass<
      RawTower_Prototype2>(topNode, "TOWER_CALIB_TRIGGER_VETO");
  assert(TOWER_CALIB_TRIGGER_VETO);

  RawTower_Temperature* TOWER_TEMPERATURE_EMCAL = findNode::getClass<
      RawTower_Temperature>(topNode, "TOWER_TEMPERATURE_EMCAL");
  assert(TOWER_TEMPERATURE_EMCAL);

  return Fun4AllReturnCodes::EVENT_OK;
}

Fun4AllHistoManager *
Proto2ShowerCalib::get_HistoManager()
{

  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("Proto2ShowerCalib_HISTOS");

  if (not hm)
    {
      cout
          << "Proto2ShowerCalib::get_HistoManager - Making Fun4AllHistoManager Proto2ShowerCalib_HISTOS"
          << endl;
      hm = new Fun4AllHistoManager("Proto2ShowerCalib_HISTOS");
      se->registerHistoManager(hm);
    }

  assert(hm);

  return hm;
}
