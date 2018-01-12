#include "TowerTiming.h"

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h>

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

TowerTiming::TowerTiming(const std::string &name, const std::string &filename):
  SubsysReco( name ),
  hm(nullptr),
  _filename(filename),
  ntuptwr(nullptr),
  ntupe(nullptr),
  outfile(nullptr)
{}

TowerTiming::~TowerTiming()
{
  //  delete ntup;
  delete hm;
} 


int
TowerTiming::Init( PHCompositeNode* )
{
  ostringstream hname, htit;
  hm = new  Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");
  ntuptwr = new TNtuple("twr","Towers","detid:phi:eta:edep:t");
  ntupe = new TNtuple("truth", "The Absolute Truth", "phi:theta:eta:e:p");
  return 0;
}

int
TowerTiming::process_event( PHCompositeNode* topNode )
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
    ntvars[2] = 0.5*log((particle->get_e()+particle->get_pz())/
			(particle->get_e()-particle->get_pz()));
    ntvars[3] = particle->get_e();
    ntvars[4] = particle->get_pz();
  }
  ntupe->Fill(ntvars);
  RawTowerContainer  *towers = nullptr;
  // double emc[10] = {0};
  // double ih[10] = {0};
  // double oh[10] = {0};
  // double mag[10] = {0};
  // double bh[10] = {0};
  // double eall[5] = {0};
  string nodename[3] = {"TOWER_SIM_CEMC", "TOWER_SIM_HCALIN", "TOWER_SIM_HCALOUT"};
  string geonodename[3] = {"TOWERGEOM_CEMC", "TOWERGEOM_HCALIN", "TOWERGEOM_HCALOUT"};
  for (int j=0; j<3;j++)
  {
    towers = findNode::getClass<RawTowerContainer>(topNode,nodename[j]);
    if (towers)
    {
      RawTowerGeomContainer* towergeom = findNode::getClass<RawTowerGeomContainer>(topNode,geonodename[j]);
      if (!towergeom)
	{
	  cout << "no geometry node " << geonodename << endl;
	  continue;
	}
      RawTowerContainer::ConstRange twr_range = towers->getTowers();
      for ( RawTowerContainer::ConstIterator twr_iter = twr_range.first ; twr_iter !=  twr_range.second; ++twr_iter )
      {
        double energy = twr_iter->second->get_energy();
	      int phibin = twr_iter->second->get_binphi();
	      int etabin = twr_iter->second->get_bineta();
	      double phi = towergeom->get_phicenter(phibin);
	      double eta = towergeom->get_etacenter(etabin);
               ntuptwr->Fill(j,
			  phi,
			  eta,
                          energy,
			     twr_iter->second->get_time());
	// double radius = sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
	// 	  	     hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y() +
	// 		     hit_iter->second->get_avg_z() * hit_iter->second->get_avg_z());
// 	double phi =  atan2(hit_iter->second->get_avg_y(),hit_iter->second->get_avg_x());
// 	double theta = atan(sqrt(hit_iter->second->get_avg_x() * hit_iter->second->get_avg_x() +
// 				 hit_iter->second->get_avg_y() * hit_iter->second->get_avg_y()) /
// 			    hit_iter->second->get_avg_z());
// // handle rollover from pi to -pi
// 	double diffphi = phi-ntvars[0];
// 	if (diffphi > M_PI)
// 	{
// 	  diffphi -= 2*M_PI;
// 	}
// 	else if (diffphi < - M_PI)
// 	{
// 	  diffphi += 2*M_PI;
// 	}

// 	double deltasqrt = sqrt(diffphi*diffphi+(theta-ntvars[1])*(theta-ntvars[1]));
// 	double edep = hit_iter->second->get_edep();
// 	eall[0] += edep;
// 	for (int i=0; i<10; i++)
// 	{
// 	  if (deltasqrt < i*0.1)
// 	  {
// 	    emc[i]+=edep;
// 	  }
// 	}
//       }
      }
    }
  }
  return 0;
}

int
TowerTiming::End(PHCompositeNode * topNode)
{
  outfile->cd();
  //  ntup->Write();
  ntuptwr->Write();
  ntupe->Write();
  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(_filename, "UPDATE");
  return 0;
}

void
TowerTiming::AddNode(const std::string &name, const int detid)
{
  _node_postfix.insert(name);
  _detid[name] = detid;
  return;
}
