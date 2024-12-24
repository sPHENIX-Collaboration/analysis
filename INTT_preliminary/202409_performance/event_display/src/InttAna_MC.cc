//for MC
#include "InttAna.h"

#include <math.h>

/// Cluster/Calorimeter includes

/// Fun4All includes
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// #include <trackbase/TrkrClusterv4.h>
// #include <trackbase/TrkrClusterContainerv3.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase/InttDefs.h>

/// ROOT includes
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>
#include <TTree.h>

/// C++ includes
#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

#include "InttEvent.h"
#include "InttRawData.h"

// To get vertex
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>

// TODO:
/// HEPMC truth includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

#include <g4main/PHG4InEvent.h>
#include <g4main/PHG4VtxPoint.h> // for PHG4Particle
#include <g4main/PHG4Particle.h> // for PHG4Particle

using namespace std;

/**
 * This class demonstrates the construction and use of an analysis module
 * within the sPHENIX Fun4All framework. It is intended to show how to
 * obtain physics objects from the analysis tree, and then write them out
 * to a ROOT tree and file for further offline analysis.
 */

/**
 * Constructor of module
 */
InttAna::InttAna(const std::string &name, const std::string &filename)
    : SubsysReco(name), _rawModule(nullptr), fname_(filename), anafile_(nullptr), h_zvtxseed_(nullptr)
{
  xbeam_ = ybeam_ = 0.0;
}

/**
 * Destructor of module
 */
InttAna::~InttAna()
{
}

/**
 * Initialize the module and prepare looping over events
 */
int InttAna::Init(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 5)
  {
    std::cout << "Beginning Init in InttAna" << std::endl;
  }

  anafile_ = new TFile(fname_.c_str(), "recreate");
  h_dca2d_zero = new TH1F("h_dca2d_zero", "DCA2D to 0", 500, -10, 10);
  h2_dca2d_zero = new TH2F("h2_dca2d_zero", "DCA2D to 0 vs nclus", 500, -10, 10, 100, 0, 10000);
  h2_dca2d_len = new TH2F("h2_dca2d_len", "DCA2D to 0 vs len", 500, -10, 10, 500, -10, 10);
  h_zvtx = new TH1F("h_zvtx", "Zvertex", 400, -40, 40);
  h_eta = new TH1F("h_eta", "h_eta", 100, -6, 6);
  h_phi = new TH1F("h_phi", "h_phi", 100, -6, 6);
  h_theta = new TH1F("h_theta", "h_theta", 100, -4, 4);

  h_ntp_clus = new TNtuple("ntp_clus", "Cluster Ntuple", "nclus:nclus2:bco_full:evt:size:adc:x:y:z:lay:lad:sen:x_vtx:y_vtx:z_vtx");
  h_ntp_cluspair = new TNtuple("ntp_cluspair", "Cluster Pair Ntuple", "nclus:nclus2:bco_full:evt:ang1:ang2:z1:z2:dca2d:len:unorm:l1:l2:vx:vy:vz");

  h_zvtxseed_ = new TH1F("h_zvtxseed", "Zvertex Seed histogram", 200, -50, 50);

  // TODO:
  m_hepmctree = new TTree("hepmctree", "A tree with hepmc truth particles");
  m_hepmctree->Branch("m_evt", &m_evt, "m_evt/I");
  m_hepmctree->Branch("m_xvtx", &m_xvtx, "m_xvtx/D");
  m_hepmctree->Branch("m_yvtx", &m_yvtx, "m_yvtx/D");
  m_hepmctree->Branch("m_zvtx", &m_zvtx, "m_zvtx/D");
  m_hepmctree->Branch("m_partid1", &m_partid1, "m_partid1/I");
  m_hepmctree->Branch("m_partid2", &m_partid2, "m_partid2/I");
  m_hepmctree->Branch("m_x1", &m_x1, "m_x1/D");
  m_hepmctree->Branch("m_x2", &m_x2, "m_x2/D");
  m_hepmctree->Branch("m_mpi", &m_mpi, "m_mpi/I");
  m_hepmctree->Branch("m_process_id", &m_process_id, "m_process_id/I");
  m_hepmctree->Branch("m_truthenergy", &m_truthenergy, "m_truthenergy/D");
  m_hepmctree->Branch("m_trutheta", &m_trutheta, "m_trutheta/D");
  m_hepmctree->Branch("m_truththeta", &m_truththeta, "m_truththeta/D");
  m_hepmctree->Branch("m_truthphi", &m_truthphi, "m_truthphi/D");
  m_hepmctree->Branch("m_status", &m_status, "m_status/I");
  m_hepmctree->Branch("m_truthpx", &m_truthpx, "m_truthpx/D");
  m_hepmctree->Branch("m_truthpy", &m_truthpy, "m_truthpy/D");
  m_hepmctree->Branch("m_truthpz", &m_truthpz, "m_truthpz/D");
  m_hepmctree->Branch("m_truthpt", &m_truthpt, "m_truthpt/D");
  m_hepmctree->Branch("m_numparticlesinevent", &m_numparticlesinevent, "m_numparticlesinevent/I");
  m_hepmctree->Branch("m_truthpid", &m_truthpid, "m_truthpid/I");

  return 0;
}

int InttAna::InitRun(PHCompositeNode * /*topNode*/)
{
  cout << "InttAna::InitRun beamcenter " << xbeam_ << " " << ybeam_ << endl;

  return 0;
}

/**
 * Main workhorse function where each event is looped over and
 * data from each event is collected from the node tree for analysis
 */
int InttAna::process_event(PHCompositeNode *topNode)
{
  static int ievt = 0;
  cout << "InttEvt::process evt : " << ievt++ << endl;

  if (Verbosity() > 5)
  {
    std::cout << "Beginning process_event in AnaTutorial" << std::endl;
  }
  // TODO:
  // getHEPMCTruth(topNode);

  // readRawHit(topNode);

  ActsGeometry *m_tGeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
  if (!m_tGeometry)
  {
    std::cout << PHWHERE << "No ActsGeometry on node tree. Bailing." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  // TODO:
  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!hepmceventmap)
  {
    cout << PHWHERE << "hepmceventmap node is missing." << endl;
    // return Fun4AllReturnCodes::ABORTEVENT;
  }

  // TODO:
  PHG4InEvent *phg4inevent = findNode::getClass<PHG4InEvent>(topNode, "PHG4INEVENT");
  if (!phg4inevent)
  {
    cout << PHWHERE << "PHG4INEVENT node is missing." << endl;
    // return Fun4AllReturnCodes::ABORTEVENT;
  }

  TrkrClusterContainer *m_clusterMap =
      findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!m_clusterMap)
  {
    cout << PHWHERE << "TrkrClusterContainer node is missing." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertexMap *vertexs =
      findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexs)
  {
    cout << PHWHERE << "GlobalVertexMap node is missing." << endl;
    // return Fun4AllReturnCodes::ABORTEVENT;
  }

  /////////////
  //  InttEvent from RawModule (not standard way)
  InttEvent *inttEvt = nullptr;
  if (_rawModule)
  {
    inttEvt = _rawModule->getInttEvent();
  }
  uint64_t bco = 0;
  //-- int evtseq = -1;
  if (inttEvt != NULL)
  {
    bco = inttEvt->bco;
    //-- evtseq = inttEvt->evtSeq;
  }

  // xvtx_sim = 0; //vertexs->find(GlobalVertex::TRUTH)->second->get_x();
  // yvtx_sim = 0; //vertexs->find(GlobalVertex::TRUTH)->second->get_y();
  // zvtx_sim = 0; //vertexs->find(GlobalVertex::TRUTH)->second->get_z();
  xvtx_sim = vertexs->find(GlobalVertex::TRUTH)->second->get_x();
  yvtx_sim = vertexs->find(GlobalVertex::TRUTH)->second->get_y();
  zvtx_sim = vertexs->find(GlobalVertex::TRUTH)->second->get_z();

  // TODO:
  if (hepmceventmap != nullptr)
    getHEPMCTruth(topNode);
  else if (phg4inevent != nullptr)
    getPHG4Particle(topNode);
  else
  {
    cout << "no inputinfo available. hepmctree is empty" << endl;
  }

  /////////////
  int nclusadd = 0, nclusadd2 = 0;
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range = m_clusterMap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
        nclusadd++;
        const auto cluster = clusIter->second;
        if (cluster->getAdc() > 40)
          nclusadd2++;
      }
    }
  }
  /////////////

  static int evtCount = 0;

  // cout << "evtCount : " << evtCount << " " << evtseq << " " << hex << bco << dec << endl;
  cout << "nclus : " << nclusadd << " " << nclusadd2 << endl;

  struct ClustInfo
  {
    int layer;
    Acts::Vector3 pos;
  };

  float ntpval[20];
  int nCluster = 0;
  bool exceedNwrite = false;
  // std::vector<Acts::Vector3> clusters[2]; // inner=0; outer=1
  std::vector<ClustInfo> clusters[2]; // inner=0; outer=1
  for (unsigned int inttlayer = 0; inttlayer < 4; inttlayer++)
  {
    int layer = (inttlayer < 2 ? 0 : 1);
    for (const auto &hitsetkey : m_clusterMap->getHitSetKeys(TrkrDefs::TrkrId::inttId, inttlayer + 3))
    {
      auto range = m_clusterMap->getClusters(hitsetkey);

      for (auto clusIter = range.first; clusIter != range.second; ++clusIter)
      {
        const auto cluskey = clusIter->first;
        const auto cluster = clusIter->second;
        const auto globalPos = m_tGeometry->getGlobalPosition(cluskey, cluster);
        int ladder_z = InttDefs::getLadderZId(cluskey);
        int ladder_phi = InttDefs::getLadderPhiId(cluskey);
        int size = cluster->getSize();

        if (nCluster < 5)
        {
          cout << "xyz : " << globalPos.x() << " " << globalPos.y() << " " << globalPos.z() << " :  "
               << cluster->getAdc() << " " << size << " " << inttlayer << " " << ladder_z << " " << ladder_phi << endl;
        }
        else
        {
          if (!exceedNwrite)
          {
            cout << " exceed : ncluster limit.  no more cluster xyz printed" << endl;
            exceedNwrite = true;
          }
        }

        ClustInfo info;
        info.layer = inttlayer;
        info.pos = globalPos;

        // clusters[layer].push_back(globalPos);
        clusters[layer].push_back(info);
        nCluster++;

        ntpval[0] = nclusadd;  // bco_full
        ntpval[1] = nclusadd2; // bco_full
        ntpval[2] = bco;       // bco_full
        // ntpval[3] = evtseq;            // evtCount;
        ntpval[3] = evtCount;          // evtCount;
        ntpval[4] = size;              // size
        ntpval[5] = cluster->getAdc(); // size
        ntpval[6] = globalPos.x();
        ntpval[7] = globalPos.y();
        ntpval[8] = globalPos.z();
        ntpval[9] = inttlayer;
        ntpval[10] = ladder_phi;
        ntpval[11] = ladder_z;
        ntpval[12] = xvtx_sim;
        ntpval[13] = yvtx_sim;
        ntpval[14] = zvtx_sim;
        h_ntp_clus->Fill(ntpval);

        // cout << "track event = " << evtCount << endl;

        // = new TNtuple("ntp_clus", "Cluster Ntuple", "bco_full:evt:size:adc:x:y:z:lay:lad:sen");
      }
    }
  }
  cout << "nCluster : " << nCluster << "-----" << endl;

  /////////////////////////////////
  /////////////////////////////////

  /////////////////////////////////
  /////////////////////////////////

  h_zvtxseed_->Reset();

  if (nCluster < 300)
  // if(nCluster<500)
  {
    float ntpval2[20];
    Acts::Vector3 beamspot = Acts::Vector3(xbeam_, ybeam_, 0);
    vector<double> vz_array;
    for (auto c1 = clusters[0].begin(); c1 != clusters[0].end(); ++c1) // inner
    {
      for (auto c2 = clusters[1].begin(); c2 != clusters[1].end(); ++c2) // outer
      {
        Acts::Vector3 p1 = c1->pos - beamspot;
        Acts::Vector3 p2 = c2->pos - beamspot;
        Acts::Vector3 u = p2 - p1;
        double unorm = sqrt(u.x() * u.x() + u.y() * u.y());
        if (unorm < 0.00001)
          continue;

        // skip bad compbination
        double p1_ang = atan2(p1.y(), p1.x());
        double p2_ang = atan2(p2.y(), p2.x());
        double d_ang = p2_ang - p1_ang;

        // unit vector in 2D
        double ux = u.x() / unorm;
        double uy = u.y() / unorm;
        double uz = u.z() / unorm; // same normalization factor(2D) is multiplied

        Acts::Vector3 p0 = beamspot - p1;

        double dca_p0 = p0.x() * uy - p0.y() * ux; // cross product of p0 x u
        double len_p0 = p0.x() * ux + p0.y() * uy; // dot product of p0 . u

        // beam center in X-Y plane
        double vx = len_p0 * ux + p1.x();
        double vy = len_p0 * uy + p1.y();

        double vz = len_p0 * uz + p1.z();

        // if(unorm>4.5||d_ang<-0.005*3.1415||0.005*3.1415<d_ang)
        // if(unorm>4.5||d_ang<-0.25*3.1415||0.25*3.1415<d_ang)
        if (fabs(d_ang) < 0.1 && fabs(dca_p0) < 0.5)
        {
          h_zvtxseed_->Fill(vz);
          vz_array.push_back(vz);
        }

        h_dca2d_zero->Fill(dca_p0);
        h2_dca2d_zero->Fill(dca_p0, nCluster);
        h2_dca2d_len->Fill(dca_p0, len_p0);
        // cout<<"dca : "<<dca_p0<<endl;
        //
        ntpval2[0] = nclusadd;
        ntpval2[1] = nclusadd2;
        ntpval2[2] = 0;
        ntpval2[3] = evtCount;
        ntpval2[4] = p1_ang;
        ntpval2[5] = p2_ang;
        ntpval2[6] = p1.z();
        ntpval2[7] = p2.z();
        ntpval2[8] = dca_p0;
        ntpval2[9] = len_p0;
        ntpval2[10] = unorm;
        ntpval2[11] = c1->layer;
        ntpval2[12] = c2->layer;
        ntpval2[13] = vx;
        ntpval2[14] = vy;
        ntpval2[15] = vz;
        h_ntp_cluspair->Fill(ntpval2);
        // h_ntp_cluspair = new TNtuple("ntp_cluspair", "Cluster Pair Ntuple", "nclus:bco_full:evt:ang1:ang2:dca2d:len:unorm");
      }
    }

    // calculate trancated mean of DCA~Z histogram as Z-vertex position
    double zvtx = -9999.;

    if (vz_array.size() > 3)
    {
      double zbin = h_zvtxseed_->GetMaximumBin();
      double zcenter = h_zvtxseed_->GetBinCenter(zbin);
      //-- double zmean = h_zvtxseed_->GetMean();
      double zrms = h_zvtxseed_->GetRMS();
      if (zrms < 20)
        zrms = 20;

      double zmax = zcenter + zrms; // 1 sigma
      double zmin = zcenter - zrms; // 1 sigma

      double zsum = 0.;
      int zcount = 0;
      for (auto iz = vz_array.begin(); iz != vz_array.end(); ++iz)
      {
        double vz = (*iz);
        if (zmin < vz && vz < zmax)
        {
          zsum += vz;
          zcount++;
        }
      }
      if (zcount > 0)
        zvtx = zsum / zcount;

      // cout << "ZVTX: " << zvtx << " " << zcenter << " " << zmean << " " << zrms << " " << zbin << endl;
    }

    h_zvtx->Fill(zvtx);
  }

  evtCount++;

  return Fun4AllReturnCodes::EVENT_OK;
}

/**
 * End the module and finish any data collection. Clean up any remaining
 * loose ends
 */
int InttAna::End(PHCompositeNode * /*topNode*/)
{
  if (Verbosity() > 1)
  {
    std::cout << "Ending InttAna analysis package" << std::endl;
  }
  anafile_->cd();
  // TODO:
  m_hepmctree->Write();
  if (anafile_ != nullptr)
  {
    anafile_->Write();
    anafile_->Close();
  }

  return 0;
}
// TODO:

void InttAna::readRawHit(PHCompositeNode *topNode)
{
  TrkrHitSetContainer *m_hits = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
  if (!m_hits)
  {
    cout << PHWHERE << "ERROR: Can't find node TRKR_HITSET" << endl;
    return;
  }

  // uint8_t getLadderZId(TrkrDefs::hitsetkey key);
  // uint8_t getLadderPhiId(TrkrDefs::hitsetkey key);
  // int getTimeBucketId(TrkrDefs::hitsetkey key);
  //
  // uint16_t getCol(TrkrDefs::hitkey key); // z-strip = offline chip
  // uint16_t getRow(TrkrDefs::hitkey key); // y-strip = offline channel

  // loop over the InttHitSet objects
  TrkrHitSetContainer::ConstRange hitsetrange =
      m_hits->getHitSets(TrkrDefs::TrkrId::inttId);
  for (TrkrHitSetContainer::ConstIterator hitsetitr = hitsetrange.first;
       hitsetitr != hitsetrange.second;
       ++hitsetitr)
  {
    // Each hitset contains only hits that are clusterizable - i.e. belong to a single sensor
    TrkrHitSet *hitset = hitsetitr->second;

    if (Verbosity() > 1)
      cout << "InttClusterizer found hitsetkey " << hitsetitr->first << endl;
    if (Verbosity() > 2)
      hitset->identify();

    // we have a single hitset, get the info that identifies the sensor
    // int layer = TrkrDefs::getLayer(hitsetitr->first);
    // int ladder_z_index = InttDefs::getLadderZId(hitsetitr->first);
    // int ladder_phi_index = InttDefs::getLadderPhiId(hitsetitr->first);
    // int ladder_bco_index = InttDefs::getTimeBucketId(hitsetitr->first);

    // cout << "layer " << layer << " " << ladder_z_index << " " << ladder_phi_index << " " << ladder_bco_index << endl;

    //// we will need the geometry object for this layer to get the global position
    // CylinderGeomIntt* geom = dynamic_cast<CylinderGeomIntt*>(geom_container->GetLayerGeom(layer));
    // float pitch = geom->get_strip_y_spacing();
    // float length = geom->get_strip_z_spacing();

    // fill a vector of hits to make things easier - gets every hit in the hitset
    std::vector<std::pair<TrkrDefs::hitkey, TrkrHit *>> hitvec;
    TrkrHitSet::ConstRange hitrangei = hitset->getHits();
    for (TrkrHitSet::ConstIterator hitr = hitrangei.first;
         hitr != hitrangei.second;
         ++hitr)
    {
      hitvec.push_back(make_pair(hitr->first, hitr->second));
      // int chip = InttDefs::getCol(hitr->first);
      // int chan = InttDefs::getRow(hitr->first);
      // int adc = (hitr->second)->getAdc();
      // cout << "     hit : " << chip << " " << chan << " " << adc << endl;
    }
    // cout << "hitvec.size(): " << hitvec.size() << endl;
  }
}

void InttAna::getHEPMCTruth(PHCompositeNode *topNode)
{
  cout << "getHEPMCTruth!!!" << endl;
  /// Get the node from the node tree
  PHHepMCGenEventMap *hepmceventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");

  /// If the node was not properly put on the tree, return
  if (!hepmceventmap)
  {
    std::cout << PHWHERE
              << "HEPMC event map node is missing, can't collected HEPMC truth particles"
              << std::endl;
    return;
  }

  /// Could have some print statements for debugging with verbosity
  if (Verbosity() > 1)
  {
    std::cout << "Getting HEPMC truth particles " << std::endl;
  }

  /// You can iterate over the number of events in a hepmc event
  /// for pile up events where you have multiple hard scatterings per bunch crossing
  for (PHHepMCGenEventMap::ConstIter eventIter = hepmceventmap->begin();
       eventIter != hepmceventmap->end();
       ++eventIter)
  {
    /// Get the event
    PHHepMCGenEvent *hepmcevent = eventIter->second;

    if (hepmcevent)
    {
      /// Get the event characteristics, inherited from HepMC classes
      HepMC::GenEvent *truthevent = hepmcevent->getEvent();
      if (!truthevent)
      {
        std::cout << PHWHERE
                  << "no evt pointer under phhepmvgeneventmap found "
                  << std::endl;
        return;
      }

      /// Get the parton info
      HepMC::PdfInfo *pdfinfo = truthevent->pdf_info();

      /// Get the parton info as determined from HEPMC
      m_partid1 = pdfinfo->id1();
      m_partid2 = pdfinfo->id2();
      m_x1 = pdfinfo->x1();
      m_x2 = pdfinfo->x2();

      /// Are there multiple partonic intercations in a p+p event
      m_mpi = truthevent->mpi();

      /// Get the PYTHIA signal process id identifying the 2-to-2 hard process
      m_process_id = truthevent->signal_process_id();

      if (Verbosity() > 2)
      {
        std::cout << " Iterating over an event" << std::endl;
      }

      /// Loop over all the truth particles and get their information
      for (HepMC::GenEvent::particle_const_iterator iter = truthevent->particles_begin();
           iter != truthevent->particles_end();
           ++iter)
      {
        /// Get each pythia particle characteristics
        m_truthenergy = (*iter)->momentum().e();
        m_truthpid = (*iter)->pdg_id();

        // TODO: truth information
        m_xvtx = xvtx_sim;
        m_yvtx = yvtx_sim;
        m_zvtx = zvtx_sim;
        m_trutheta = (*iter)->momentum().pseudoRapidity();
        m_truththeta = 2 * atan(exp(-m_trutheta));
        // h_eta->Fill(m_trutheta);
        m_truthphi = (*iter)->momentum().phi();
        m_truthpx = (*iter)->momentum().px();
        m_truthpy = (*iter)->momentum().py();
        m_truthpz = (*iter)->momentum().pz();

        m_truthpt = sqrt(m_truthpx * m_truthpx + m_truthpy * m_truthpy);
        m_status = (*iter)->status();

        // m_truthphi = atan2(m_truthpy,m_truthpx);

        // cout << "status: " << (*iter)->status() << " " << m_truthpid << endl;

        h_phi->Fill(m_truthphi);
        h_theta->Fill(m_truththeta);

        /*std::cout << "m_trutheta = " << m_trutheta << "  m_truthphi = " << m_truthphi << std::endl;*/

        /// Fill the truth tree
        m_hepmctree->Fill();
        m_numparticlesinevent++;
      }

      // for (HepMC::GenEventVertexRange::vertex_const_iterator viter = truthevent->vertices_begin();
      //      viter != truthevent->vertices_end();
      //      ++viter)
      // {
      //   /// Get each pythia particle characteristics
      //   m_vertex = (*viter)->vertex_range();
      //   cout<<m_vertex<<endl;

      //   /// Fill the truth tree
      //   m_hepmctree->Fill();
      // }
    }
    // cout << "truth event = " << m_evt << endl;
    m_evt++;
  }
}

void InttAna::getPHG4Particle(PHCompositeNode *topNode)
{
  cout << "getPHG4Particle!!!" << endl;
  /// Get the node from the node tree
  PHG4InEvent *phg4inevent = findNode::getClass<PHG4InEvent>(topNode, "PHG4INEVENT");

  /// If the node was not properly put on the tree, return
  if (!phg4inevent)
  {
    std::cout << PHWHERE
              << "PHG4InEvent node is missing, can't collected PHG4 truth particles"
              << std::endl;
    return;
  }

  /// Could have some print statements for debugging with verbosity
  if (Verbosity() > 1)
  {
    std::cout << "Getting PHG4InEvent truth particles " << std::endl;
  }

  /// You can iterate over the number of events in a hepmc event
  /// for pile up events where you have multiple hard scatterings per bunch crossing
  std::map<int, PHG4VtxPoint *>::const_iterator vtxiter;
  std::multimap<int, PHG4Particle *>::const_iterator particle_iter;
  std::pair<std::map<int, PHG4VtxPoint *>::const_iterator, std::map<int, PHG4VtxPoint *>::const_iterator> vtxbegin_end = phg4inevent->GetVertices();

  for (vtxiter = vtxbegin_end.first; vtxiter != vtxbegin_end.second; ++vtxiter)
  {
    //*fout << "vtx number: " << vtxiter->first << std::endl;
    //(*vtxiter->second).identify(*fout);
    std::pair<std::multimap<int, PHG4Particle *>::const_iterator,
              std::multimap<int, PHG4Particle *>::const_iterator>
        particlebegin_end = phg4inevent->GetParticles(vtxiter->first);

    PHG4VtxPoint *vtx = vtxiter->second;
    m_xvtx = vtx->get_x();
    m_yvtx = vtx->get_y();
    m_zvtx = vtx->get_z();
    // virtual double get_t() const { return std::numeric_limits<double>::quiet_NaN(); }

    for (particle_iter = particlebegin_end.first; particle_iter != particlebegin_end.second; ++particle_iter)
    {
      //      (particle_iter->second)->identify(*fout);
      PHG4Particle *part = particle_iter->second;

      m_truthenergy = part->get_e();
      m_truthpid = part->get_pid();

      m_truthpx = part->get_px();
      m_truthpy = part->get_py();
      m_truthpz = part->get_pz();
      m_truthphi = atan2(m_truthpy, m_truthpx);
      m_truthpt = sqrt(m_truthpx * m_truthpx + m_truthpy * m_truthpy);

      m_truththeta = atan2(m_truthpt, m_truthpz);
      m_trutheta = -log(tan(0.5 * m_truththeta));
      m_status = 1;

      // part->get_pid() const override { return fpid; }
      // part->get_name() const override { return fname; }
      // part->get_e() const override { return fe; }
      // part->get_track_id();
      // part->get_vtx_id() const override { return vtxid; }
      // part->get_parent_id() const override { return parentid; }
      // int get_primary_id() const override { return primaryid; }

      /// Fill the truth tree
      h_phi->Fill(m_truthphi);
      h_theta->Fill(m_truththeta);

      m_hepmctree->Fill();
      m_numparticlesinevent++;
    }
  }
  cout << "truth event = " << m_evt << endl;
  m_evt++;

  /*
    for (PHHepMCGenEventMap::ConstIter eventIter = hepmceventmap->begin();
         eventIter != hepmceventmap->end();
         ++eventIter)
    {
      /// Get the event
      PHHepMCGenEvent *hepmcevent = eventIter->second;

      if (hepmcevent)
      {
        /// Get the parton info
        HepMC::PdfInfo *pdfinfo = truthevent->pdf_info();

        /// Get the parton info as determined from HEPMC
        m_partid1 = pdfinfo->id1();
        m_partid2 = pdfinfo->id2();
        m_x1 = pdfinfo->x1();
        m_x2 = pdfinfo->x2();

        /// Are there multiple partonic intercations in a p+p event
        m_mpi = truthevent->mpi();

        /// Get the PYTHIA signal process id identifying the 2-to-2 hard process
        m_process_id = truthevent->signal_process_id();
      }
      // cout << "truth event = " << m_evt << endl;
      m_evt++;
    }
  */
}
