#include "HijingShowerSize.h"

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <fun4all/Fun4AllHistoManager.h>

#include <phool/getClass.h>
#include <phool/PHRandomSeed.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include <gsl/gsl_rng.h>

#include<sstream>

using namespace std;

HijingShowerSize::HijingShowerSize(const std::string &name, const std::string &filename):
  SubsysReco( name ),
  nblocks(0),
  hm(nullptr),
  _filename(filename),
  ntups(nullptr),
  ntupe(nullptr),
  ntup(nullptr),
  outfile(nullptr)
{
  RandomGenerator = gsl_rng_alloc(gsl_rng_mt19937);
  seed = PHRandomSeed(); // fixed seed is handled in this funtcion
  cout << Name() << " random seed: " << seed << endl;
  gsl_rng_set(RandomGenerator,seed);
  return;
}

HijingShowerSize::~HijingShowerSize()
{
  //  delete ntup;
  gsl_rng_free (RandomGenerator);
  delete hm;
} 


int
HijingShowerSize::Init( PHCompositeNode* )
{
  ostringstream hname, htit;
  hm = new  Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");
  ntups = new TNtuple("sz"," Shower size","rad:em:hi:ho:mag:bh");
  ntupe = new TNtuple("truth", "The Absolute Truth", "phi:theta");
  ntup = new TNtuple("de", "Change in Angles", "ID:dphi:dtheta:dtotal:edep");
  return 0;
}

int
HijingShowerSize::process_event( PHCompositeNode* topNode )
{
  float ntvars[5][2] = {0};
  float tupvars[2];
  for (int i=0; i<5; i++)
  {
    ntvars[i][0] = 4*M_PI*gsl_rng_uniform_pos(RandomGenerator) - 2*M_PI;
    ntvars[i][1] = (1.62-1.52)*gsl_rng_uniform_pos(RandomGenerator) + 1.52;
    if (ntvars[i][1] < 0)
      {
	ntvars[i][1]+=M_PI;
      }
    tupvars[0] = ntvars[i][0];
    tupvars[1] = ntvars[i][1];
  ntupe->Fill(tupvars);
  }
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
	for (int i = 0; i<5; i++)
	{
	double diffphi = phi-ntvars[i][0];
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
	double difftheta = theta-ntvars[i][1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	if (deltasqrt>0.5)
	{
	  continue;
	}
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 0;
	detid[1] = diffphi;
	detid[2] = difftheta;
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
	for (int i=0; i<5; i++)
	{
	double diffphi = phi-ntvars[i][0];
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
	double difftheta = theta-ntvars[i][1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	if (deltasqrt>0.5)
	{
	  continue;
	}
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 1;
	detid[1] = diffphi;
	detid[2] = difftheta;
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
	for (int i=0; i<5; i++)
	{
	double diffphi = phi-ntvars[i][0];
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
	double difftheta = theta-ntvars[i][1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	if (deltasqrt>0.5)
	{
	  continue;
	}
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 2;
	detid[1] = diffphi;
	detid[2] = difftheta;
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
      for (int i=0; i<5; i++)
      {
	double diffphi = phi-ntvars[i][0];
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
	double difftheta = theta-ntvars[i][1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	if (deltasqrt>0.5)
	{
	  continue;
	}
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 3;
	detid[1] = diffphi;
	detid[2] = difftheta;
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
      for (int i=0; i<5; i++)
      {
	double diffphi = phi-ntvars[i][0];
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
	double difftheta = theta-ntvars[i][1];
// theta goes from 0-PI --> no rollover problem
	double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
	if (deltasqrt>0.5)
	{
	  continue;
	}
	double edep = hit_iter->second->get_edep();
	eall[0] += edep;
	detid[0] = 4;
	detid[1] = diffphi;
	detid[2] = difftheta;
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
HijingShowerSize::End(PHCompositeNode * topNode)
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
HijingShowerSize::AddNode(const std::string &name, const int detid)
{
  _node_postfix.insert(name);
  _detid[name] = detid;
  return;
}
