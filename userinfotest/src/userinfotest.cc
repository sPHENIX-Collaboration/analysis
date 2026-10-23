#include "userinfotest.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
// G4Cells includes

#include <treeuserinfo/TreeUserInfo.h>
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
  _f = new TFile( _foutname.c_str(), "RECREATE");
  
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

  _f->cd();
  
  TreeUserInfo::Add(_tree);
  
  _f->Write();
  _f->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int userinfotest::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}
