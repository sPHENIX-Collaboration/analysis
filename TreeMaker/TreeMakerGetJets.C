#include <TreeMaker.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <TLorentzVector.h>
#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>
#include <jetbackground/TowerBackground.h>



int TreeMaker::GetTruthJets(PHCompositeNode *topNode)
{

  // ------------------------------------------------------------------------------
  // --- get truth jet information
  // ------------------------------------------------------------------------------

  JetMap* truth2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r02");
  //cout << "R = 0.2 truth jets has size " << truth2_jets->size() << endl;
  for (JetMap::Iter iter = truth2_jets->begin(); iter != truth2_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet2_e[ b_truthjet2_n ] = this_e;
      b_truthjet2_pt[ b_truthjet2_n ] = this_pt;
      b_truthjet2_eta[ b_truthjet2_n ] = this_eta;
      b_truthjet2_phi[ b_truthjet2_n ] = this_phi;
      //cout << " truth R=0.2 jet # " << b_truthjet2_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet2_n++;
    } // loop over R = 0.2 truth jets

  JetMap* truth3_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r03");
  //cout << "R = 0.3 truth jets has size " << truth3_jets->size() << endl;
  for (JetMap::Iter iter = truth3_jets->begin(); iter != truth3_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet3_e[ b_truthjet3_n ] = this_e;
      b_truthjet3_pt[ b_truthjet3_n ] = this_pt;
      b_truthjet3_eta[ b_truthjet3_n ] = this_eta;
      b_truthjet3_phi[ b_truthjet3_n ] = this_phi;
      //cout << " truth R=0.3 jet # " << b_truthjet3_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet3_n++;
    } // loop over R = 0.3 truth jets

  JetMap* truth4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r04");
  //cout << "R = 0.4 truth jets has size " << truth4_jets->size() << endl;
  for (JetMap::Iter iter = truth4_jets->begin(); iter != truth4_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet4_e[ b_truthjet4_n ] = this_e;
      b_truthjet4_pt[ b_truthjet4_n ] = this_pt;
      b_truthjet4_eta[ b_truthjet4_n ] = this_eta;
      b_truthjet4_phi[ b_truthjet4_n ] = this_phi;
      //cout << " truth R=0.4 jet # " << b_truthjet4_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet4_n++;
    } // loop over R = 0.4 truth jets

  JetMap* truth5_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r05");
  //cout << "R = 0.5 truth jets has size " << truth5_jets->size() << endl;
  for (JetMap::Iter iter = truth5_jets->begin(); iter != truth5_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet5_e[ b_truthjet5_n ] = this_e;
      b_truthjet5_pt[ b_truthjet5_n ] = this_pt;
      b_truthjet5_eta[ b_truthjet5_n ] = this_eta;
      b_truthjet5_phi[ b_truthjet5_n ] = this_phi;
      //cout << " truth R=0.5 jet # " << b_truthjet5_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet5_n++;
    } // loop over R = 0.5 truth jets

  return 0;

}



int TreeMaker::GetRecoJets(PHCompositeNode *topNode)
{

  // ------------------------------------------------------------------------------
  // --- get reconstructed jet information
  // ------------------------------------------------------------------------------

  JetMap* nsreco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r02");
  //cout << "reco jets R=0.2 has size " << nsreco2_jets->size() << endl;
  for (JetMap::Iter iter = nsreco2_jets->begin(); iter != nsreco2_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet2_e[ b_jet2_n ] = this_e;
      b_jet2_pt[ b_jet2_n ] = this_pt;
      b_jet2_eta[ b_jet2_n ] = this_eta;
      b_jet2_phi[ b_jet2_n ] = this_phi;
      //cout << " pp reco R=0.2 jet #" << b_jet2_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet2_n++;
    } // loop over R=0.2 jets

  JetMap* nsreco3_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r03");
  //cout << "reco jets R=0.3 has size " << nsreco3_jets->size() << endl;
  for (JetMap::Iter iter = nsreco3_jets->begin(); iter != nsreco3_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet3_e[ b_jet3_n ] = this_e;
      b_jet3_pt[ b_jet3_n ] = this_pt;
      b_jet3_eta[ b_jet3_n ] = this_eta;
      b_jet3_phi[ b_jet3_n ] = this_phi;
      //cout << " pp reco R=0.3 jet #" << b_jet3_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet3_n++;
    } // loop over R=0.3 jets

  JetMap* nsreco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r04");
  //cout << "reco jets R=0.4 has size " << nsreco4_jets->size() << endl;
  for (JetMap::Iter iter = nsreco4_jets->begin(); iter != nsreco4_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet4_e[ b_jet4_n ] = this_e;
      b_jet4_pt[ b_jet4_n ] = this_pt;
      b_jet4_eta[ b_jet4_n ] = this_eta;
      b_jet4_phi[ b_jet4_n ] = this_phi;
      //cout << " pp reco R=0.4 jet #" << b_jet4_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet4_n++;
    } // loop over R=0.4 jets

  JetMap* nsreco5_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r05");
  //cout << "reco jets R=0.5 has size " << nsreco5_jets->size() << endl;
  for (JetMap::Iter iter = nsreco5_jets->begin(); iter != nsreco5_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet5_e[ b_jet5_n ] = this_e;
      b_jet5_pt[ b_jet5_n ] = this_pt;
      b_jet5_eta[ b_jet5_n ] = this_eta;
      b_jet5_phi[ b_jet5_n ] = this_phi;
      //cout << " pp reco R=0.2 jet #" << b_jet5_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet5_n++;
    } // loop over R=0.5 jets



  // ------------------------------------------------------------------------------
  // --- get reconstructed jet information (modified)
  // ------------------------------------------------------------------------------

  JetMap* modnsreco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_Mod_r02");
  //cout << "reco jets R=0.2 has size " << modnsreco2_jets->size() << endl;
  for (JetMap::Iter iter = modnsreco2_jets->begin(); iter != modnsreco2_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_modjet2_e[ b_modjet2_n ] = this_e;
      b_modjet2_pt[ b_modjet2_n ] = this_pt;
      b_modjet2_eta[ b_modjet2_n ] = this_eta;
      b_modjet2_phi[ b_modjet2_n ] = this_phi;
      //cout << " pp reco R=0.2 jet #" << b_jet2_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_modjet2_n++;
    } // loop over R=0.2 jets



  return 0;

}
