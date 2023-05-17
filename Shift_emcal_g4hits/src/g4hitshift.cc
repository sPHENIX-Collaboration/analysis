#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>

#include "g4hitshift.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <fun4all/Fun4AllHistoManager.h>

#include <phool/PHCompositeNode.h>

// G4Hits includes
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4HitDefs.h>  // for hit_idbits

// G4Cells includes
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CellDefs.h>  // for genkey, keytype

// Tower includes
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerDefs.h>
// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>


#include "g4detectors/PHG4CylinderGeomContainer.h"
#include "g4detectors/PHG4CylinderGeom_Spacalv1.h"  // for PHG4CylinderGeom_Spaca...
#include "g4detectors/PHG4CylinderGeom_Spacalv3.h"
#include "g4detectors/PHG4CylinderCellGeomContainer.h"
#include "g4detectors/PHG4CylinderCellGeom_Spacalv1.h"


#include <phool/getClass.h>
#include <TProfile.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TMath.h>
#include <cassert>
#include <sstream>
#include <string>
#include <TF1.h>


//____________________________________________________________________________..
g4hitshift::g4hitshift(const std::string &name):
 SubsysReco(name)
{
}

//____________________________________________________________________________..
g4hitshift::~g4hitshift()
{
}

//____________________________________________________________________________..
int g4hitshift::Init(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int g4hitshift::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int g4hitshift::process_event(PHCompositeNode *topNode)
{ 
  PHG4HitContainer* hits = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_CEMC");
  if (hits)
    {
      //-----------------------------------------------------------------------
      //Loop over G4Hits 
      //-----------------------------------------------------------------------
      PHG4HitContainer::ConstRange hit_range = hits->getHits();
      for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
	{
	  int scint_id = hit_iter->second->get_scint_id();
	  
	  PHG4CylinderGeom_Spacalv3::scint_id_coder decoder(scint_id);
	  
	  int sectornumber = decoder.sector_ID;
	  int shiftedsector = sectornumber + 8;
	  if (shiftedsector > 31)
	    {
	      shiftedsector = shiftedsector - 31;
	    }
	  PHG4CylinderGeom_Spacalv3::scint_id_coder encoder(shiftedsector,decoder.tower_ID,decoder.fiber_ID);
	  
	  hit_iter->second->set_scint_id(encoder.scint_ID);
	}
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int g4hitshift::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int g4hitshift::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int g4hitshift::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int g4hitshift::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void g4hitshift::Print(const std::string &what) const
{
}
