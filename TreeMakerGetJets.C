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



int TreeMaker::GetSeedJets(PHCompositeNode *topNode)
{

  // ------------------------------------------------------------------------------
  // --- get jet seed information
  // ------------------------------------------------------------------------------

  JetMap* seedraw_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_HIRecoSeedsRaw_r02");
  //cout << "reco jets R=0.2 has size " << seedraw_jets->size() << endl;
  for (JetMap::Iter iter = seedraw_jets->begin(); iter != seedraw_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet2seedraw_pt[ b_jet2seedraw_n ] = this_pt;
      b_jet2seedraw_eta[ b_jet2seedraw_n ] = this_eta;
      b_jet2seedraw_phi[ b_jet2seedraw_n ] = this_phi;
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      //cout << " SeedRaw R=0.2 jet #" << b_jet2seedraw_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet2seedraw_n++;
    }

  JetMap* seedsub_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_HIRecoSeedsSub_r02");
  //cout << "reco jets R=0.2 has size " << seedsub_jets->size() << endl;
  for (JetMap::Iter iter = seedsub_jets->begin(); iter != seedsub_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      b_jet2seedsub_pt[ b_jet2seedsub_n ] = this_pt;
      b_jet2seedsub_eta[ b_jet2seedsub_n ] = this_eta;
      b_jet2seedsub_phi[ b_jet2seedsub_n ] = this_phi;
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      //cout << " SeedSub R=0.2 jet #" << b_jet2seedsub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet2seedsub_n++;
    }

  return 0;

}



int TreeMaker::GetRecoJets(PHCompositeNode *topNode)
{

  // ------------------------------------------------------------------------------
  // --- get reconstructed jet information (no subtraction)
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
  // --- get reconstructed jet information (with subtraction)
  // ------------------------------------------------------------------------------

  JetMap* reco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r02_Sub1");
  //cout << "reco jets R=0.4 has size " << reco2_jets->size() << endl;
  for (JetMap::Iter iter = reco2_jets->begin(); iter != reco2_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet2sub_e[ b_jet2sub_n ] = this_e;
      b_jet2sub_pt[ b_jet2sub_n ] = this_pt;
      b_jet2sub_eta[ b_jet2sub_n ] = this_eta;
      b_jet2sub_phi[ b_jet2sub_n ] = this_phi;
      //cout << " reco R=0.2 jet (Sub1) #" << b_jet2sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet2sub_n++;
    } // loop over R=0.2 reco jets

  JetMap* reco3_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r03_Sub1");
  //cout << "reco jets R=0.4 has size " << reco3_jets->size() << endl;
  for (JetMap::Iter iter = reco3_jets->begin(); iter != reco3_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet3sub_e[ b_jet3sub_n ] = this_e;
      b_jet3sub_pt[ b_jet3sub_n ] = this_pt;
      b_jet3sub_eta[ b_jet3sub_n ] = this_eta;
      b_jet3sub_phi[ b_jet3sub_n ] = this_phi;
      //cout << " reco R=0.3 jet (Sub1) #" << b_jet3sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet3sub_n++;
    } // loop over R=0.3 reco jets

  JetMap* reco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r04_Sub1");
  //cout << "reco jets R=0.4 has size " << reco4_jets->size() << endl;
  for (JetMap::Iter iter = reco4_jets->begin(); iter != reco4_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet4sub_e[ b_jet4sub_n ] = this_e;
      b_jet4sub_pt[ b_jet4sub_n ] = this_pt;
      b_jet4sub_eta[ b_jet4sub_n ] = this_eta;
      b_jet4sub_phi[ b_jet4sub_n ] = this_phi;
      //cout << " reco R=0.4 jet (Sub1) #" << b_jet4sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet4sub_n++;
    } // loop over R=0.4 reco jets

  JetMap* reco5_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r05_Sub1");
  //cout << "reco jets R=0.4 has size " << reco4_jets->size() << endl;
  for (JetMap::Iter iter = reco5_jets->begin(); iter != reco5_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet5sub_e[ b_jet5sub_n ] = this_e;
      b_jet5sub_pt[ b_jet5sub_n ] = this_pt;
      b_jet5sub_eta[ b_jet5sub_n ] = this_eta;
      b_jet5sub_phi[ b_jet5sub_n ] = this_phi;
      //cout << " reco R=0.5 jet (Sub1) #" << b_jet5sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet5sub_n++;
    } // loop over R=0.5 jets

  return 0;

}
