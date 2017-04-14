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

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

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

  string recojetname = "AntiKt_Tower_r05";

  JetMap* recojets = findNode::getClass<JetMap>(topNode,recojetname.c_str());
  if (!recojets)
    {
      cerr << PHWHERE << " ERROR: Can't find " << recojetname << endl;
      exit(-1);
    }


     // for every recojet
  for (JetMap::Iter iter = recojets->begin();
       iter != recojets->end();
       ++iter)
    {
      Jet* recojet = iter->second;

      float id    = recojet->get_id();
      float ncomp = recojet->size_comp();
      float eta   = recojet->get_eta();
      float phi   = recojet->get_phi();
      float e     = recojet->get_e();
      float pt    = recojet->get_pt();

      cout << "Found a jet: " << id << " " << ncomp << " " << eta << " " << phi << " " << e << " " << pt << endl;
    }

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
