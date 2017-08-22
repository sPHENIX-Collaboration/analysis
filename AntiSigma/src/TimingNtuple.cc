#include "TimingNtuple.h"

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <fun4all/Fun4AllHistoManager.h>

#include <phool/getClass.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include <boost/foreach.hpp>

#include<sstream>

using namespace std;

TimingNtuple::TimingNtuple(const std::string &name, const std::string &filename):
  SubsysReco( name ),
  nblocks(0),
  hm(NULL),
  _filename(filename),
  ntup(NULL),
  outfile(NULL)
{}

TimingNtuple::~TimingNtuple()
{
  //  delete ntup;
  delete hm;
} 


int
TimingNtuple::Init( PHCompositeNode* )
{
  ostringstream hname, htit;
  hm = new  Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");
  //ntup = new TNtuple("hitntup", "G4Hits", "detid:layer:x0:y0:z0:x1:y1:z1:edep");
  ntupt = new TNtuple("tnt", "G4Hits", "ID:t:dtotal");
//  ntupavet = new TNtuple("time", "G4Hits", "ID:avt");
 //  ntup->SetDirectory(0);
  TH1 *h1 = new TH1F("edep1GeV","edep 0-1GeV",1000,0,1);
  eloss.push_back(h1);
  h1 = new TH1F("edep100GeV","edep 0-100GeV",1000,0,100);
  eloss.push_back(h1);
  return 0;
}

int
TimingNtuple::process_event( PHCompositeNode* topNode )
{
  ostringstream nodename;

  map<int, PHG4Particle*>::const_iterator particle_iter;
  PHG4TruthInfoContainer *_truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  PHG4TruthInfoContainer::ConstRange primary_range =
    _truth_container->GetPrimaryParticleRange();
  float ntvars[5] = {0};
  for (PHG4TruthInfoContainer::ConstIterator particle_iter = primary_range.first;
       particle_iter != primary_range.second; ++particle_iter)
  {
    PHG4Particle *particle = particle_iter->second;
    ntvars[0] = atan2(particle->get_py(), particle->get_px());
    ntvars[1] = atan(sqrt(particle->get_py()*particle->get_py() + 
			  particle->get_px()*particle->get_px()) /
		     particle->get_pz());
    if (ntvars[1] < 0)
      {
	ntvars[1]+=M_PI;
      }
    ntvars[2] = 0.5*log((particle->get_e()+particle->get_pz())/
			(particle->get_e()-particle->get_pz()));
    ntvars[3] = particle->get_e();
    ntvars[4] = particle->get_pz();
  }

  set<string>::const_iterator iter;
  vector<TH1 *>::const_iterator eiter;

  PHG4HitContainer *hits = nullptr;

  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_CEMC");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  double dtotal = get_dtotal(hit_iter->second, ntvars[0],ntvars[1]);
	  ntupt->Fill(0,hit_iter->second->get_avg_t(),dtotal);
	}
    }


  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_HCALIN");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  double dtotal = get_dtotal(hit_iter->second, ntvars[0],ntvars[1]);
	  ntupt->Fill(1,hit_iter->second->get_avg_t(),dtotal);
	}
    }

  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_HCALOUT");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  double dtotal = get_dtotal(hit_iter->second, ntvars[0],ntvars[1]);
	  ntupt->Fill(2,hit_iter->second->get_avg_t(),dtotal);
	}
    }

  return 0;
}

int
TimingNtuple::End(PHCompositeNode * topNode)
{
  outfile->cd();
  //  ntup->Write();
  ntupt->Write();
  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(_filename, "UPDATE");
  return 0;
}

void
TimingNtuple::AddNode(const std::string &name, const int detid)
{
 _node_postfix.insert(name);
 _detid[name] = detid;
 return;
}

double
TimingNtuple::get_dtotal(const PHG4Hit *hit, const double phi, const double theta)
{
  double phi_hit =  atan2(hit->get_avg_y(),hit->get_avg_x());
  double theta_hit = atan(sqrt(hit->get_avg_x()*hit->get_avg_x()+
			       hit->get_avg_y()*hit->get_avg_y()) /
			  hit->get_avg_z());
  double diffphi =  phi_hit - phi;
  if (diffphi > M_PI)
  {
    diffphi -= 2*M_PI;
  }
  else if (diffphi < - M_PI)
  {
    diffphi += 2*M_PI;
  }
  if (theta_hit < 0)
  {
    theta_hit += M_PI;
  }
  double difftheta = theta_hit-theta;
  double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
  return deltasqrt;
}
