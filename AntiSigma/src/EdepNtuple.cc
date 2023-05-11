#include "EdepNtuple.h"

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>

#include <fun4all/Fun4AllHistoManager.h>

#include <phool/getClass.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include <boost/foreach.hpp>

#include<sstream>

using namespace std;

EdepNtuple::EdepNtuple(const std::string &name, const std::string &filename):
  SubsysReco( name ),
  nblocks(0),
  hm(NULL),
  _filename(filename),
  ntup(NULL),
  outfile(NULL)
{}

EdepNtuple::~EdepNtuple()
{
  //  delete ntup;
  delete hm;
} 


int
EdepNtuple::Init( PHCompositeNode* )
{
  ostringstream hname, htit;
  hm = new  Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");
  //ntup = new TNtuple("hitntup", "G4Hits", "detid:layer:x0:y0:z0:x1:y1:z1:edep");
  ntupe = new TNtuple("ed", "G4Hits", "ES:EA:HIS:HIA:HOS:HOA:BH:MAG");
 //  ntup->SetDirectory(0);
  TH1 *h1 = new TH1F("edep1GeV","edep 0-1GeV",1000,0,1);
  eloss.push_back(h1);
  h1 = new TH1F("edep100GeV","edep 0-100GeV",1000,0,100);
  eloss.push_back(h1);
  return 0;
}

int
EdepNtuple::process_event( PHCompositeNode* topNode )
{
  ostringstream nodename;
  set<string>::const_iterator iter;
  vector<TH1 *>::const_iterator eiter;
  float ntvar[8]={0};
  PHG4HitContainer *hits = nullptr;

  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_CEMC");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  ntvar[0] += hit_iter->second->get_edep();
	}
    }

  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_ABSORBER_CEMC");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  ntvar[1] += hit_iter->second->get_edep();
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
	  ntvar[2] += hit_iter->second->get_edep();
	}
    }

  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_ABSORBER_HCALIN");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  ntvar[3] += hit_iter->second->get_edep();
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
	  ntvar[4] += hit_iter->second->get_edep();
	}
    }

  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_ABSORBER_HCALOUT");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  ntvar[5] += hit_iter->second->get_edep();
	}
    }

 hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_BH_1");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  ntvar[6] += hit_iter->second->get_edep();
	}
    }

 hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_MAGNET");

  if (hits)
    {
      //          double numhits = hits->size();
      //          nhits[i]->Fill(numhits);
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

	{
	  ntvar[7] += hit_iter->second->get_edep();
	}
    }

  ntupe->Fill(ntvar);
  return 0;
}

int
EdepNtuple::End(PHCompositeNode * topNode)
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
EdepNtuple::AddNode(const std::string &name, const int detid)
{
 _node_postfix.insert(name);
 _detid[name] = detid;
 return;
}
