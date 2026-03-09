#include "JetCalibration.h"
#include "FindDijets.h"
//for emc clusters
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TEfficiency.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Shower.h>
#include <g4main/PHG4HitDefs.h>
#include <TMath.h>
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <HepMC/SimpleVector.h> 
//for vetex information
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
JetCalibration::JetCalibration(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  _foutname = outfilename;  

}

//____________________________________________________________________________..
JetCalibration::~JetCalibration()
{

}

//____________________________________________________________________________..
int JetCalibration::Init(PHCompositeNode *topNode)
{
  _i_event = 0;
  hm = new Fun4AllHistoManager("TRIGGER_HISTOGRAMS");

  TH1D *h;
  TH2D *h2;

  h = new TH1D("h_truth_reco", "", 200, 0, 2);
  hm->registerHisto(h);

  h = new TH1D("h_match_dR", "", 100, 0, 0.2);
  hm->registerHisto(h);

  h = new TH1D("h_truth_iso", "", 100, 0, TMath::Pi());
  hm->registerHisto(h);

  TEfficiency *hp = new TEfficiency("h_match_eff", "", 80, 0, 40);
  hm->registerHisto(hp);

  h = new TH1D("h_truth_em", "", 100, 0, 1);
  hm->registerHisto(h);

  h2 = new TH2D("h_truth_em_v_truth", "", 60, 0, 60, 100, 0, 1);
  hm->registerHisto(h2);

  h2 = new TH2D("h_truth_reco_v_truth", "", 60, 0, 60, 200, 0, 2);
  hm->registerHisto(h2);

  h2 = new TH2D("h_truth_reco_v_truth_em", "", 100, 0, 1, 200, 0, 2);
  hm->registerHisto(h2);

  h2 = new TH2D("h_truth_reco_v_truth_em_shower", "", 100, 0, 1, 200, 0, 2);
  hm->registerHisto(h2);

  return 0;
}

//____________________________________________________________________________..
int JetCalibration::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int JetCalibration::process_event(PHCompositeNode *topNode)
{

  _i_event++;

  std::string recoJetName = "AntiKt_Tower_r04_Sub1";

  JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, recoJetName);

  recoJetName = "AntiKt_Truth_r04";

  JetContainer *jets_truth_4 = findNode::getClass<JetContainer>(topNode, recoJetName);
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  PHG4TruthInfoContainer::ShowerRange range = truthinfo->GetPrimaryShowerRange();


  PHG4HitContainer *hitinfo;
  hitinfo = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_CEMC");
  if (!hitinfo)
    {
      return Fun4AllReturnCodes::ABORTRUN;
    }

  m_HitContainerMap[hitinfo->GetID()] = hitinfo;

  hitinfo = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_HCALOUT");
  m_HitContainerMap[hitinfo->GetID()] = hitinfo;

  hitinfo = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_HCALIN");
  m_HitContainerMap[hitinfo->GetID()] = hitinfo;
  
  std::vector<struct jetty> truth_jets;
  std::vector<struct jetty> reco_jets;
  for (auto jet : *jets_truth_4)
    {
      if (jet->get_pt() < 4) continue;
      if (fabs(jet->get_eta()) > .7) continue;
      struct jetty myjet;
      myjet.pt = jet->get_pt();
      myjet.et = 0;
      myjet.eta = jet->get_eta();
      myjet.phi = jet->get_phi();
      myjet.emcal = 0;
      myjet.ohcal = 0;
      float jet_em = 0;
      float jet_h = 0;
      float jet_edep = 0;
      for (const auto& comp : jet->get_comp_vec())
	{
	  unsigned int index = comp.second;
	  PHG4Particle* truth_particle = truthinfo->GetParticle(index);

	  assert(truth_particle);

	  PHG4Shower* shower = nullptr;
	  for (PHG4TruthInfoContainer::ShowerIterator iter = range.first;
	       iter != range.second;
	       ++iter)
	    {
	      shower = iter->second;
	      if (shower->get_parent_particle_id() == truth_particle->get_track_id())
		{
		  break;
		}
	    }

	  if (!shower) 
	    {
	      continue;
	    }


	  float edep = 0.0;
	  float edep_e = 0.0;
	  float edep_h = 0.0;
	  
	  // iterate through hits
	  for (std::map<int, std::set<PHG4HitDefs::keytype> >::iterator iter = shower->begin_g4hit_id();
	       iter != shower->end_g4hit_id();
	       ++iter)
	    {
	      int g4hitmap_id = iter->first;
	      std::map<int, PHG4HitContainer*>::iterator mapiter = m_HitContainerMap.find(g4hitmap_id);
	      if (mapiter == m_HitContainerMap.end())
	  	{
	  	  continue;
	  	}

	      PHG4HitContainer* hits = mapiter->second;

	      // get the g4hits from this particle in this volume
	      for (unsigned long long g4hit_id : iter->second)
	  	{
	  	  PHG4Hit* g4hit = hits->findHit(g4hit_id);
	  	  if (!g4hit)
	  	    {
	  	      continue;
	  	    }

	  	  PHG4Particle* particle = truthinfo->GetParticle(g4hit->get_trkid());
	  	  if (!particle)
	  	    {
	  	      continue;
	  	    }

		  if (!isnan(g4hit->get_edep()))
		    {
		      edep += g4hit->get_edep();
		      if (abs(particle->get_pid()) == 11)
			{
			  edep_e += g4hit->get_edep();
			}
		      else
			{
			  edep_h += g4hit->get_edep();
			}
		    }
		  
	  	}

	    }
	  int pid = truth_particle->get_pid();
	  double energy = sqrt(TMath::Power(truth_particle->get_px(), 2) + TMath::Power(truth_particle->get_py(), 2));
	  // if em add energe to emcal
	  if (isEM(pid))
	    {
	      myjet.emcal += energy;
	    }
	  else 
	    {
	      myjet.ohcal += energy;
	    }
	  myjet.et += energy;
	  
	  jet_em += edep_e;
	  jet_h += edep_h;
	  jet_edep += edep;
	}
      if (jet_edep == 0.0) continue;
      myjet.edep = jet_edep;
      myjet.edep_em = jet_em;
      myjet.edep_hd = jet_h;
      truth_jets.push_back(myjet);

    }
  int size = truth_jets.size();
  for (int ijet = 0 ; ijet < size ; ijet++)
    {
      struct jetty jet1 = truth_jets.at(ijet);
      float min_dR = 99;
      for (int jjet = 0; jjet < size; jjet++)
	{
	  if (jjet == ijet) continue;
	  struct jetty jet2 = truth_jets.at(jjet);
	  double dR = getDr(jet1, jet2);
	  if (dR < min_dR)
	    {
	      min_dR = dR;
	    }
	}
      truth_jets.at(ijet).iso = min_dR;
    }
  auto h_truth_reco = dynamic_cast<TH1*>(hm->getHisto("h_truth_reco"));
  auto h_truth_em = dynamic_cast<TH1*>(hm->getHisto("h_truth_em"));
  auto h_match_dR = dynamic_cast<TH1*>(hm->getHisto("h_match_dR"));
  auto h_match_eff = dynamic_cast<TEfficiency*>(hm->getHisto("h_match_eff"));
  auto h_truth_em_v_truth = dynamic_cast<TH2*>(hm->getHisto("h_truth_em_v_truth"));
  auto h_truth_reco_v_truth_em = dynamic_cast<TH2*>(hm->getHisto("h_truth_reco_v_truth_em"));
  auto h_truth_reco_v_truth_em_shower = dynamic_cast<TH2*>(hm->getHisto("h_truth_reco_v_truth_em_shower"));
  auto h_truth_reco_v_truth = dynamic_cast<TH2*>(hm->getHisto("h_truth_reco_v_truth"));
  auto h_truth_iso = dynamic_cast<TH1*>(hm->getHisto("h_truth_iso"));
  for (auto jet : *jets_4)
    {

      struct jetty myjet;
      if (jet->get_pt() < 1) continue;
      myjet.pt = jet->get_pt();
      myjet.eta = jet->get_eta();
      myjet.phi = jet->get_phi();

      reco_jets.push_back(myjet);
    }

  std::sort(reco_jets.begin(), reco_jets.end(), [] (auto a, auto b) { return a.pt > b.pt; });
  std::sort(truth_jets.begin(), truth_jets.end(), [] (auto a, auto b) { return a.pt > b.pt; });

  for (auto &jet : truth_jets)
    {
      h_truth_iso->Fill(jet.iso);
      if (jet.iso < 1.0) continue;
      bool matched = false;
      for (auto &jetreco : reco_jets)
	{
	  double dR = getDr(jet, jetreco);
	  if (dR < 0.2)
	    {
	      matched = true;
	      h_truth_reco->Fill(jetreco.pt/jet.pt);
	      h_truth_em->Fill(jet.emcal/jet.pt);
	      h_match_dR->Fill(dR);
	      h_truth_em_v_truth->Fill(jet.pt, jet.emcal/jet.pt);
	      if (jet.pt > 20 && jet.pt < 30) h_truth_reco_v_truth_em->Fill(jet.emcal/jet.pt, jetreco.pt/jet.pt);
	      if (jet.pt > 20 && jet.pt < 30) h_truth_reco_v_truth_em_shower->Fill(jet.edep_em/jet.edep, jetreco.pt/jet.pt);
	      h_truth_reco_v_truth->Fill(jet.pt, jetreco.pt/jet.pt);
	      break;
	    }
	}
      h_match_eff->Fill(matched, jet.pt);
    } 


  return Fun4AllReturnCodes::EVENT_OK;
}

double JetCalibration::getDr(struct jetty jet1, struct jetty jet2)
{
  float dphi = jet1.phi - jet2.phi;

  if (dphi > TMath::Pi())
    {
      dphi = dphi - 2.*TMath::Pi();
    }
  if (dphi <= -1*TMath::Pi())
    {
      dphi = dphi + 2. * TMath::Pi();
    }

  return sqrt(TMath::Power(dphi, 2) + TMath::Power(jet1.eta - jet2.eta, 2));
}
bool JetCalibration::isEM(int pid)
{
  if (pid == 11 || pid == -11 || pid == 22 || pid == 111 || pid == 221)
    {
      return true;
    }
  return false;
}

void JetCalibration::GetNodes(PHCompositeNode* topNode)
{


}

int JetCalibration::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetCalibration::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetCalibration::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "JetCalibration::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;
  hm->dumpHistos(_foutname.c_str());
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetCalibration::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "JetCalibration::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}
