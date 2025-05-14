#include "UPCTrigStudy.h"

/// Cluster/Calorimeter includes
/*
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calotrigger/CaloTriggerInfo.h>
*/

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

/// Tracking includes
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>

///
#include <mbd/MbdOut.h>
#include <ffarawobjects/Gl1Packet.h>

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <ffaobjects/EventHeader.h>

/// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TTree.h>
#include <Math/Vector4D.h>
#include <Math/VectorUtil.h>


/// C++ includes
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

/**
 * Constructor of module
 */
UPCTrigStudy::UPCTrigStudy(const std::string &name, const std::string &filename)
  : SubsysReco(name)
  , m_outfilename(filename)
  , m_hm(nullptr)
  //, m_mincluspt(0.25)
  //, m_analyzeTracks(true)
  //, m_analyzeClusters(true)
  //, m_analyzeTruth(false)
{
  /// Initialize variables and trees so we don't accidentally access
  /// memory that was never allocated
  resetVariables();
  initializeTrees();
}

/**
 * Destructor of module
 */
UPCTrigStudy::~UPCTrigStudy()
{
  delete m_hm;
  delete _globaltree;
}

/**
 * Initialize the module and prepare looping over events
 */
int UPCTrigStudy::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in UPCTrigStudy" << std::endl;
  }

  m_outfile = new TFile(m_outfilename.c_str(), "RECREATE");

  h_trig = new TH1F("h_trig", "trig", 16, 0, 16);
  h_ntracks = new TH1F("h_ntracks", "num tracks", 2000, 0, 2000);

  h_zdce = new TH1F("h_zdce","ZDC Energy",820,-50,4050);
  h_zdcse = new TH1F("h_zdcse","ZDC.S Energy",500,0,250);
  h_zdcne = new TH1F("h_zdcne","ZDC.N Energy",500,0,250);
  h_zdctimecut = new TH1F("h_zdctimecut", "zdctimecut", 50, -17.5 , 32.5);
 
  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int UPCTrigStudy::GetNodes(PHCompositeNode *topNode)
{
  /// EventHeader node
  evthdr = findNode::getClass<EventHeader>(topNode, "EventHeader");

  // Get the raw gl1 data from event combined DST
  _gl1raw = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if(!_gl1raw && f_evt<4) std::cout << PHWHERE << " GL1Packet node not found on node tree" << std::endl;

  // MbdOut
  _mbdout = findNode::getClass<MbdOut>(topNode, "MbdOut");
  if(!_mbdout && f_evt<4) std::cout << PHWHERE << " MbdOut node not found on node tree" << std::endl;

  _zdctowers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_ZDC");
  if(!_zdctowers && f_evt<4) std::cout << PHWHERE << " TOWERINFO_CALIB_ZDC node not found on node tree" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int UPCTrigStudy::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in UPCTrigStudy" << std::endl;
  }

  resetVariables();

  h_trig->Fill( 0 );  // event counter
  _nprocessed++;

  /// Get all the data nodes
  int status = GetNodes(topNode);
  if ( status != Fun4AllReturnCodes::EVENT_OK )
  {
    return status;
  }

  /// Get the run and eventnumber
  if ( evthdr )
  {
    f_run = evthdr->get_RunNumber();
    f_evt = evthdr->get_EvtSequence();
  }

  // process triggers, skip if not a clock trigger
  status = process_triggers();
  if ( status != Fun4AllReturnCodes::EVENT_OK)
  {
    return status;
  }

  /// Get MBD information
  f_mbdsn = _mbdout->get_npmt(0);
  f_mbdnn = _mbdout->get_npmt(1);

  process_zdc();

  /// Get calorimeter information
  /*
  if (m_analyzeClusters)
  {
    getEMCalClusters(topNode);
  }
  */

  _globaltree->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int UPCTrigStudy::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Ending UPCTrigStudy analysis package" << std::endl;
  }

  /// Change to the outfile
  m_outfile->cd();

  _globaltree->Write();

  /// If we analyzed the clusters, write them out
  /*
  if (m_analyzeClusters)
  {
    m_clustertree->Write();
  }
  */

  /// Write and close the outfile
  m_outfile->Write();
  m_outfile->Close();

  /// Clean up pointers and associated histos/trees in TFile
  delete m_outfile;

  if (Verbosity() > 1)
  {
    std::cout << "Finished UPCTrigStudy analysis package" << std::endl;
  }

  return 0;
}

int UPCTrigStudy::process_triggers()
{
  // Only use MBDNS triggered events
  if ( _gl1raw != nullptr )
  {
    const uint64_t CLOCK = 0x1;        // CLOCK trigger bit
    const uint64_t MBDTRIGS = 0x7c00;  // MBDNS trigger bits
    const uint64_t MBDWIDE = 0xc00;    // MBDNS wide bits (no vtx cut)
    //const uint64_t ZDCNS = 0x8;        // ZDCNS trigger bits

    f_cross = _gl1raw->getBunchNumber();
    f_rtrig = _gl1raw->getTriggerVector();
    f_ltrig = _gl1raw->getLiveVector();
    f_strig = _gl1raw->getScaledVector();

    if ( (f_strig&CLOCK) == 0 )
    {
      return Fun4AllReturnCodes::ABORTEVENT;
    }

    if ( f_strig&MBDWIDE )
    {
      _mbdwide = 1;
    }
    else
    {
      _mbdwide = 0;
    }

    if ( _nprocessed<10 )
    {
      std::cout << "Trig " << _nprocessed << std::endl;
      std::cout << std::hex;
      std::cout << "Raw\t" << f_rtrig << std::endl;
      std::cout << "Live\t" << f_ltrig << std::endl;
      std::cout << "Scaled\t" << f_strig << std::endl;
      std::cout << "AND\t" << std::hex << (f_strig&MBDTRIGS) << std::dec << std::endl;
      std::cout << "WIDE\t" << _mbdwide << std::endl;
      std::cout << std::dec;

    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}


void UPCTrigStudy::process_zdc()
{
  int max_zdc_t = Getpeaktime( h_zdctimecut );
  int range = 1;
  float zdc_etot = 0.;

  int size = _zdctowers->size(); //online towers should be the same!
  for (int ich = 0; ich < size; ich++)
  {
    TowerInfo* zdctower = _zdctowers->get_tower_at_channel(ich);
    float zdce = zdctower->get_energy();
    int zdct = _zdctowers->get_tower_at_channel(ich)->get_time();
    h_zdctimecut->Fill( zdct );

    if ( (zdct  < (max_zdc_t - range)) ||  (zdct  > (max_zdc_t + range)) )
    {
      continue;
    }

    //
    if (ich==0||ich==2||ich==4)
    {
      f_zdcse += zdce;
    }
    else if (ich == 8 || ich == 10 || ich == 12)
    {
      f_zdcne += zdce;
    }

  }

  h_zdcse->Fill( f_zdcse );
  h_zdcne->Fill( f_zdcne );
  zdc_etot = f_zdcse + f_zdcne;
  h_zdce->Fill( zdc_etot );
}

int UPCTrigStudy::Getpeaktime(TH1 * h)
{
  int getmaxtime, tcut = -1;

  for(int bin = 1; bin < h->GetNbinsX()+1; bin++)
  {
    double c = h->GetBinContent(bin);
    double max = h->GetMaximum();
    int bincenter = h->GetBinCenter(bin);
    if(max == c)
    {
      getmaxtime = bincenter;
      if(getmaxtime != -1) tcut = getmaxtime;
    }
  }

  return tcut;
}



/**
 * This method gets clusters from the EMCal and stores them in a tree. It
 * also demonstrates how to get trigger emulator information. Clusters from
 * other containers can be obtained in a similar way (e.g. clusters from
 * the IHCal, etc.)
 */
/*
   void UPCTrigStudy::getEMCalClusters(PHCompositeNode *topNode)
   {
/// Get the raw cluster container
/// Note: other cluster containers exist as well. Check out the node tree when
/// you run a simulation
RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");

if (!clusters)
{
std::cout << PHWHERE
<< "EMCal cluster node is missing, can't collect EMCal clusters"
<< std::endl;
return;
}

/// Get the global vertex to determine the appropriate pseudorapidity of the clusters
GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
if (!vertexmap)
{
std::cout << "UPCTrigStudy::getEmcalClusters - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
assert(vertexmap);  // force quit

return;
}

if (vertexmap->empty())
{
std::cout << "UPCTrigStudy::getEmcalClusters - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
return;
}

/// just take a bbc vertex
GlobalVertex *vtx = nullptr;
for(auto iter = vertexmap->begin(); iter!= vertexmap->end(); ++iter)
{
GlobalVertex* vertex = iter->second;
if(vertex->find_vtxids(GlobalVertex::MBD) != vertex->end_vtxids())
{
vtx = vertex;
}
}
if (vtx == nullptr)
{
return;
}

/// Trigger emulator
CaloTriggerInfo *trigger = findNode::getClass<CaloTriggerInfo>(topNode, "CaloTriggerInfo");

/// Can obtain some trigger information if desired
if (trigger)
{
m_E_4x4 = trigger->get_best_EMCal_4x4_E();
}
RawClusterContainer::ConstRange begin_end = clusters->getClusters();
RawClusterContainer::ConstIterator clusIter;

/// Loop over the EMCal clusters
for (clusIter = begin_end.first; clusIter != begin_end.second; ++clusIter)
{
/// Get this cluster
const RawCluster *cluster = clusIter->second;

/// Get cluster characteristics
/// This helper class determines the photon characteristics
/// depending on the vertex position
/// This is important for e.g. eta determination and E_T determination
CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);
m_clusenergy = E_vec_cluster.mag();
m_cluseta = E_vec_cluster.pseudoRapidity();
m_clustheta = E_vec_cluster.getTheta();
m_cluspt = E_vec_cluster.perp();
m_clusphi = E_vec_cluster.getPhi();

if (m_cluspt < m_mincluspt)
{
  continue;
}

m_cluspx = m_cluspt * cos(m_clusphi);
m_cluspy = m_cluspt * sin(m_clusphi);
m_cluspz = sqrt(m_clusenergy * m_clusenergy - m_cluspx * m_cluspx - m_cluspy * m_cluspy);

// fill the cluster tree with all emcal clusters
m_clustertree->Fill();
}
}
*/

/**
 * This function puts all of the tree branch assignments in one place so as to not
 * clutter up the UPCTrigStudy::Init function.
 */
void UPCTrigStudy::initializeTrees()
{
  _globaltree = new TTree("gt", "Global Info");
  _globaltree->Branch("run", &f_run, "run/I");
  _globaltree->Branch("evt", &f_evt, "evt/I");
  _globaltree->Branch("rtrig",&f_rtrig,"rtrig/l");
  _globaltree->Branch("ltrig",&f_ltrig,"ltrig/l");
  _globaltree->Branch("strig",&f_strig,"strig/l");
  _globaltree->Branch("zdcse",&f_zdcse,"zdcse/F");
  _globaltree->Branch("zdcne",&f_zdcne,"zdcne/F");
  _globaltree->Branch("cross",&f_cross,"cross/S");
  _globaltree->Branch("mbdsn",&f_mbdsn,"mbdsn/S");
  _globaltree->Branch("mbdnn",&f_mbdnn,"mbdnn/S");

  //_globaltree->Branch("ntrks", &m_ntracks, "ntrks/I");

  /*
     m_clustertree = new TTree("clustertree", "A tree with emcal clusters");
     m_clustertree->Branch("m_clusenergy", &m_clusenergy, "m_clusenergy/D");
     m_clustertree->Branch("m_cluseta", &m_cluseta, "m_cluseta/D");
     m_clustertree->Branch("m_clustheta", &m_clustheta, "m_clustheta/D");
     m_clustertree->Branch("m_cluspt", &m_cluspt, "m_cluspt/D");
     m_clustertree->Branch("m_clusphi", &m_clusphi, "m_clusphi/D");
     m_clustertree->Branch("m_cluspx", &m_cluspx, "m_cluspx/D");
     m_clustertree->Branch("m_cluspy", &m_cluspy, "m_cluspy/D");
     m_clustertree->Branch("m_cluspz", &m_cluspz, "m_cluspz/D");
     m_clustertree->Branch("m_E_4x4", &m_E_4x4, "m_E_4x4/D");
     */
}

/**
 * This function initializes all of the member variables in this class so that there
 * are no variables that might not be set before e.g. writing them to the output
 * trees.
 */
void UPCTrigStudy::resetVariables()
{
  f_evt = 0;
  f_rtrig = 0UL;
  f_ltrig = 0UL;
  f_strig = 0UL;
  f_zdcse = 0.;
  f_zdcne = 0.;
  f_mbdsn = 0;
  f_mbdnn = 0;
  f_ntracks = 0;
  _mbdwide = 0;
}

