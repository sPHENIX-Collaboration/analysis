#include "userinfotest.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
// G4Cells includes

#include <fun4all/PHTFileServer.h>

#include <iostream>

//____________________________________________________________________________..
userinfotest::userinfotest(const std::string &name, const std::string &outfilename):
  SubsysReco(name)  
{
  _foutname = outfilename;  
}

//____________________________________________________________________________..
userinfotest::~userinfotest()
{

}

//____________________________________________________________________________..
int userinfotest::Init(PHCompositeNode *topNode)
{
  // PHTFileServer shares one TFile per name between modules (e.g. MetaDataTreeWriter writing the
  // provenance tree to the same file); open() also cd's into it, so the tree below belongs to it
  PHTFileServer::open(_foutname, "RECREATE");

  _tree = new TTree("ttree","a persevering date tree");

  b_test = 0;
  _tree->Branch("test", &b_test);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

int userinfotest::process_event(PHCompositeNode *topNode)
{

  
  b_test++;
  // UE for each layer and each eta slice for both iterations

  // each layer
  _tree->Fill();
   
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int userinfotest::End(PHCompositeNode *topNode)
{

  // No _tree->Write() and no Close(): PHTFileServer writes the file (with _tree in it) when its
  // last user calls write(), and closes it at the end of the job. If another module (e.g.
  // MetaDataTreeWriter) still uses the file, this only takes us off its user count.
  PHTFileServer::write(_foutname);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int userinfotest::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}
