#include "LeptoquarksReco.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TString.h>
#include <TH2D.h>
#include <TDatabasePDG.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

using namespace std;

LeptoquarksReco::LeptoquarksReco(std::string filename) :
  SubsysReco("LeptoquarksReco" )
{
  _foutname = filename;
  _ebeam_E = 10;
  _pbeam_E = 250;
}

int
LeptoquarksReco::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  return 0;
}

int
LeptoquarksReco::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  cout << endl;
  cout << "Processing event " << _ievent << endl;

//  JetMap* recojets = findNode::getClass<JetMap>(topNode,_recojetname.c_str());
//  141     if (!recojets) {
//    142       cerr << PHWHERE << " ERROR: Can't find " << _recojetname << endl;
//    143       exit(-1);
//    144     }
//  145   
//    146     // for every recojet
//    147     for (JetMap::Iter iter = recojets->begin();
//		 148          iter != recojets->end();
//		 149          ++iter) {
//    150       Jet* recojet = iter->second;
//


  return 0;
}

int
LeptoquarksReco::End(PHCompositeNode *topNode)
{
  //_fout_root->cd();
  //_tree_lq->Write();
  //_fout_root->Close();

  return 0;
}
