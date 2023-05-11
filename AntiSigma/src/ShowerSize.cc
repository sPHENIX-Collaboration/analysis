#include "ShowerSize.h"

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

#include<sstream>

using namespace std;

ShowerSize::ShowerSize(const std::string &name, const std::string &filename):
  SubsysReco( name ),
  nblocks(0),
  hm(nullptr),
  _filename(filename),
  ntups(nullptr),
  ntupe(nullptr),
  ntup(nullptr),
  outfile(nullptr)
{}

ShowerSize::~ShowerSize()
{
  //  delete ntup;
  delete hm;
} 


int
ShowerSize::Init( PHCompositeNode* )
{
  ostringstream hname, htit;
  hm = new  Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");
  ntups = new TNtuple("sz"," Shower size","rad:em:hi:ho:mag:bh");
  ntupe = new TNtuple("truth", "The Absolute Truth", "phi:theta:eta:e:p");
  ntup = new TNtuple("de", "Change in Angles", "ID:dphi:dtheta:dtotal:edep");
  return 0;
}

int
ShowerSize::process_event( PHCompositeNode* topNode )
{
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
  ntupe->Fill(ntvars);
  PHG4HitContainer *hits = nullptr;
  double emc[10] = {0};
  double ih[10] = {0};
  double oh[10] = {0};
  double mag[10] = {0};
  double bh[10] = {0};
  double eall[5] = {0};
  float detid[5] = {0};
  string nodename[2] = {"G4HIT_CEMC","G4HIT_ABSORBER_CEMC"};
  for (int j=0; j<2;j++)
  {
    hits = findNode::getClass<PHG4HitContainer>(topNode,nodename[j]);
    if (hits)
    {
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )
      {
	// double radius = sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
	// 	  	     hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y() +
	// 		     hit_iter->second->get_avg_z() * hit_iter->second->get_avg_z());
	double phi =  atan2(hit_iter->second->get_avg_y(),hit_iter->second->get_avg_x());
	double theta = atan(sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
				 hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y()) /
			    hit_iter->second->get_avg_z());

// handle rollover from pi to -pi
	double diffphi = phi-ntvars[0];
	if (diffphi > M_PI)
	{
	  diffphi -= 2*M_PI;
	}
	else if (diffphi < - M_PI)
	{
	  diffphi += 2*M_PI;
	}
	//	double difftheta = theta-ntvars[1];
	if (theta < 0)
	  {
	    theta += M_PI;
	  }
	double difftheta = theta-ntvars[1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 0;
	detid[1] = diffphi;
	detid[2] = theta-ntvars[1];
	detid[3] = deltasqrt;
	detid[4] = edep;
	ntup->Fill(detid);
	for (int i=0; i<10; i++)
	{
	  if (deltasqrt < (i+1)*0.025)
	  {
	    emc[i]+=edep;
	  }
	}
      }
    }
  }
  nodename[0] = {"G4HIT_HCALIN"};
  nodename[1] = {"G4HIT_ABSORBER_HCALIN"};
  for (int j=0; j<2;j++)
  {
    hits = findNode::getClass<PHG4HitContainer>(topNode,nodename[j]);
    if (hits)
    {
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )
      {
	// double radius = sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
	// 	  	     hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y() +
	// 		     hit_iter->second->get_avg_z() * hit_iter->second->get_avg_z());
	double phi =  atan2(hit_iter->second->get_avg_y(),hit_iter->second->get_avg_x());
	double theta = atan(sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
				 hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y()) /
			    hit_iter->second->get_avg_z());
 // handle rollover from pi to -pi
	double diffphi = phi-ntvars[0];
	if (diffphi > M_PI)
	{
	  diffphi -= 2*M_PI;
	}
	else if (diffphi < - M_PI)
	{
	  diffphi += 2*M_PI;
	}
	//	double difftheta = theta-ntvars[1];
	if (theta < 0)
	  {
	    theta += M_PI;
	  }
	double difftheta = theta-ntvars[1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 1;
	detid[1] = diffphi;
	detid[2] = theta-ntvars[1];
	detid[3] = deltasqrt;
	detid[4] = edep;
	ntup->Fill(detid);
	for (int i=0; i<10; i++)
	{
	  if (deltasqrt < (i+1)*0.025)
	  {
	    ih[i]+=edep;
	  }
	}
      }
    }
  }
  nodename[0] = {"G4HIT_HCALOUT"};
  nodename[1] = {"G4HIT_ABSORBER_HCALOUT"};
  for (int j=0; j<2;j++)
  {
    hits = findNode::getClass<PHG4HitContainer>(topNode,nodename[j]);
    if (hits)
    {
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )
      {
	// double radius = sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
	// 	  	     hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y() +
	// 		     hit_iter->second->get_avg_z() * hit_iter->second->get_avg_z());
	double phi =  atan2(hit_iter->second->get_avg_y(),hit_iter->second->get_avg_x());
	double theta = atan(sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
				 hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y()) /
			    hit_iter->second->get_avg_z());
// handle rollover from pi to -pi
	double diffphi = phi-ntvars[0];
	if (diffphi > M_PI)
	{
	  diffphi -= 2*M_PI;
	}
	else if (diffphi < - M_PI)
	{
	  diffphi += 2*M_PI;
	}
	//	double difftheta = theta-ntvars[1];
	if (theta < 0)
	  {
	    theta += M_PI;
	  }
	double difftheta = theta-ntvars[1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 2;
	detid[1] = diffphi;
	detid[2] = theta-ntvars[1];
	detid[3] = deltasqrt;
	detid[4] = edep;
	ntup->Fill(detid);
	for (int i=0; i<10; i++)
	{
	  if (deltasqrt < (i+1)*0.025)
	  {
	    oh[i]+=edep;
	  }
	}
      }
    }
  }
  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_MAGNET");
  if (hits)
  {
    PHG4HitContainer::ConstRange hit_range = hits->getHits();
    for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )
    {
      // double radius = sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
      // 	  	     hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y() +
      // 		     hit_iter->second->get_avg_z() * hit_iter->second->get_avg_z());
      double phi =  atan2(hit_iter->second->get_avg_y(),hit_iter->second->get_avg_x());
      double theta = atan(sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
			       hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y()) /
			  hit_iter->second->get_avg_z());
// handle rollover from pi to -pi
	double diffphi = phi-ntvars[0];
	if (diffphi > M_PI)
	{
	  diffphi -= 2*M_PI;
	}
	else if (diffphi < - M_PI)
	{
	  diffphi += 2*M_PI;
	}
	//double difftheta = theta-ntvars[1];
	if (theta < 0)
	  {
	    theta += M_PI;
	  }
	double difftheta = theta-ntvars[1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 3;
	detid[1] = diffphi;
	detid[2] = theta-ntvars[1];
	detid[3] = deltasqrt;
	detid[4] = edep;
	ntup->Fill(detid);
      for (int i=0; i<10; i++)
      {
	if (deltasqrt < (i+1)*0.025)
	{
	  mag[i]+=edep;
	}
      }
    }
  }
  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_BH_1");
  if (hits)
  {
    PHG4HitContainer::ConstRange hit_range = hits->getHits();
    for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )
    {
      // double radius = sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
      // 	  	     hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y() +
      // 		     hit_iter->second->get_avg_z() * hit_iter->second->get_avg_z());
      double phi =  atan2(hit_iter->second->get_avg_y(),hit_iter->second->get_avg_x());
      double theta = atan(sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
			       hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y()) /
			  hit_iter->second->get_avg_z());
// handle rollover from pi to -pi
	double diffphi = phi-ntvars[0];
	if (diffphi > M_PI)
	{
	  diffphi -= 2*M_PI;
	}
	else if (diffphi < - M_PI)
	{
	  diffphi += 2*M_PI;
	}
	//	double difftheta = theta-ntvars[1];
	if (theta < 0)
	  {
	    theta += M_PI;
	  }
	double difftheta = theta-ntvars[1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 4;
	detid[1] = diffphi;
	detid[2] = theta-ntvars[1];
	detid[3] = deltasqrt;
	detid[4] = edep;
	ntup->Fill(detid);
      for (int i=0; i<10; i++)
      {
	if (deltasqrt < (i+1)*0.025)
	{
	  bh[i]+=edep;
	}
      }
    }
  }
  for (int i=0; i<10;i++)
  {
    float nte[6] = {0};
    nte[0] = i;
    nte[1] = emc[i];
    nte[2] = ih[i];
    nte[3] = oh[i];
    nte[4] = mag[i];
    nte[5] = bh[i];
    ntups->Fill(nte);
  }
  float nte[6] = {0};
  nte[0] = 100;
  nte[1] = eall[0];
  nte[2] = eall[1];
  nte[3] = eall[2];
  nte[4] = eall[3];
  nte[5] = eall[4];
  ntups->Fill(nte);

  return 0;
}

int
ShowerSize::End(PHCompositeNode * topNode)
{
  outfile->cd();
  //  ntup->Write();
  ntups->Write();
  ntupe->Write();
  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(_filename, "UPDATE");
  return 0;
}

void
ShowerSize::AddNode(const std::string &name, const int detid)
{
  _node_postfix.insert(name);
  _detid[name] = detid;
  return;
}
