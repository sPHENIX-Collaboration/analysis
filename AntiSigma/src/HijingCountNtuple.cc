#include "HijingCountNtuple.h"

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

HijingCountNtuple::HijingCountNtuple(const std::string &name, const std::string &filename):
  SubsysReco( name ),
  nblocks(0),
  hm(NULL),
  _filename(filename),
  ntup(NULL),
  outfile(NULL)
{}

HijingCountNtuple::~HijingCountNtuple()
{
  //  delete ntup;
  delete hm;
} 


int
HijingCountNtuple::Init( PHCompositeNode* )
{
  ostringstream hname, htit;
  hm = new  Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");
  //ntup = new TNtuple("hitntup", "G4Hits", "detid:layer:x0:y0:z0:x1:y1:z1:edep");
  ntupe = new TNtuple("hijingcnt", "NHits", "cemc:cemc_abs:ihcal:ihcal_abs:ohcal:ohcal_abs:prim");
 //  ntup->SetDirectory(0);
  return 0;
}

int
HijingCountNtuple::process_event( PHCompositeNode* topNode )
{
  map<int, PHG4Particle*>::const_iterator particle_iter;
  PHG4TruthInfoContainer *_truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  PHG4TruthInfoContainer::ConstRange primary_range =
    _truth_container->GetPrimaryParticleRange();
  float ntvars[7] = {0};
  for (PHG4TruthInfoContainer::ConstIterator particle_iter = primary_range.first;
       particle_iter != primary_range.second; ++particle_iter)
  {
    ntvars[6] += 1.;
  }
  PHG4HitContainer *hits = nullptr;
  string nodename[6] = {"G4HIT_CEMC", "G4HIT_ABSORBER_CEMC",
			"G4HIT_HCALIN", "G4HIT_ABSORBER_HCALIN",
			"G4HIT_HCALOUT", "G4HIT_ABSORBER_HCALOUT"};

  for (int i=0; i<6; i++)
  {
  hits = findNode::getClass<PHG4HitContainer>(topNode,nodename[i]);

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
	  ntvars[i] = hits->size();
    }
  }
 
  ntupe->Fill(ntvars);
  return 0;
}

int
HijingCountNtuple::End(PHCompositeNode * topNode)
{
  outfile->cd();
  //  ntup->Write();
  ntupe->Write();
  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(_filename, "UPDATE");
  return 0;
}

void
HijingCountNtuple::AddNode(const std::string &name, const int detid)
{
 _node_postfix.insert(name);
 _detid[name] = detid;
 return;
}
