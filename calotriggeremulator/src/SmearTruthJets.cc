#include "SmearTruthJets.h"
//for emc clusters
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TVector3.h>
#include <TH2D.h>
#include <TF1.h>
#include <TH1D.h>
#include <TMath.h>
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>
#include <vector>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
// G4Cells includes

#include <iostream>

#include <map>

//____________________________________________________________________________..
SmearTruthJets::SmearTruthJets(const std::string &name):
  SubsysReco(name)
  
{
}

//____________________________________________________________________________..
SmearTruthJets::~SmearTruthJets()
{

}

//____________________________________________________________________________..
int SmearTruthJets::Init(PHCompositeNode *topNode)
{
  TFile *f = new TFile("/sphenix/user/dlis/Projects/CaloTriggerEmulator/analysis/calotriggeremulator/src/jesjer.root","r");
  h_jes = (TH1D*) f->Get("h_jes")->Clone();
  h_jer = (TH1D*) f->Get("h_jer")->Clone();
  if (!h_jer)
    {
      std::cout << "NOTHING" << std::endl;      
    }
  f->Close();

  smearfunction = new TF1("smearfunction","gaus", 0, 2);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmearTruthJets::InitRun(PHCompositeNode *topNode)
{

  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
    {
      std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  // Create the AntiKt node if required
  PHCompositeNode *AlgoNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "ANTIKT"));
  if (!AlgoNode)
    {
      AlgoNode = new PHCompositeNode("ANTIKT");
      dstNode->addNode(AlgoNode);
    }

  // Create the Input node if required
  PHCompositeNode *InputNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "TRUTH"));
  if (!InputNode)
    {
      InputNode = new PHCompositeNode("TRUTH");
      AlgoNode->addNode(InputNode);
    }

  JetContainer *jetconn = findNode::getClass<JetContainer>(topNode, "AntiKt_TruthSmear_r04");
  if (!jetconn)
    {
      jetconn = new JetContainerv1();
      PHIODataNode<PHObject> *JetContainerNode = new PHIODataNode<PHObject>(jetconn, "AntiKt_TruthSmear_r04", "PHObject");
      InputNode->addNode(JetContainerNode);
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

int SmearTruthJets::process_event(PHCompositeNode *topNode)
{
  std::cout << " here"<<std::endl;

  std::string recoJetName = "AntiKt_Truth_r04";
  JetContainer *jets_truth_4 = findNode::getClass<JetContainer>(topNode, recoJetName);
  std::cout << " here"<<std::endl;
  recoJetName = "AntiKt_TruthSmear_r04";
  JetContainer *jets_truth_smear_4 = findNode::getClass<JetContainer>(topNode, recoJetName);
  std::cout << " here"<<std::endl;
  jets_truth_smear_4 = jets_truth_4;
  std::cout << " here"<<std::endl;
  for (auto jet : *jets_truth_smear_4)
    {
      std::cout << "JET"<<std::endl;
      std::cout << "Start: " << jet->get_pt() << " " << jet->get_eta() << " " << jet->get_phi() << std::endl;
      float pt = jet->get_pt();
      int ptbin = floor(pt/5) - 2;
      if (ptbin < 1) ptbin = 1;
      if (ptbin > 9) ptbin = 9;
      float jes = h_jes->GetBinContent(ptbin);
      float jer = h_jer->GetBinContent(ptbin);

      smearfunction->SetParameters(1, jes, jer);
      
      double x = smearfunction->GetRandom();
      std::cout << jes << " " << jer << " -- " << x<<std::endl;      
      pt *= x;
      TVector3 v;
      v.SetPtEtaPhi(pt, jet->get_eta(), jet->get_phi());
      jet->set_px(v.X());
      jet->set_py(v.Y());
      jet->set_pz(v.Z());
      std::cout << "End : " << jet->get_pt() << " " << jet->get_eta() << " " << jet->get_phi() << std::endl;
    }	 

  return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int SmearTruthJets::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "SmearTruthJets::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  delete smearfunction;
  delete h_jer;
  delete h_jes;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int SmearTruthJets::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "SmearTruthJets::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}
