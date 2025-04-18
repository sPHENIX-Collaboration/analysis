
#include "PairMaker.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "TFile.h"
#include "TLorentzVector.h"
#include "TRandom2.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase/TrkrDefs.h>

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHRandomSeed.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include "sPHElectron.h"
#include "sPHElectronv1.h"
#include "sPHElectronPair.h"
#include "sPHElectronPairv1.h"
#include "sPHElectronPairContainer.h"
#include "sPHElectronPairContainerv1.h"

#include "trackpidassoc/TrackPidAssoc.h"

//#include <gsl/gsl_rng.h>

typedef PHIODataNode<PHObject> PHObjectNode_t;

using namespace std;

//==============================================================

PairMaker::PairMaker(const std::string &name, const std::string &filename) : SubsysReco(name)
{
  _ZMIN = -15.;
  _ZMAX = 15.;
  _multbins.push_back(0.);
//  _multbins.push_back(75.);
//  _multbins.push_back(250.);
//  _multbins.push_back(900.);
  _multbins.push_back(3000.);
  _multbins.push_back(9999.);
  _min_buffer_depth = 10;
  _max_buffer_depth = 50;
  outnodename = "ElectronPairs";
  _rng = nullptr;
  EventNumber=0;
}

//==============================================================

int PairMaker::Init(PHCompositeNode *topNode) 
{

  _rng = new TRandom2();
  _rng->SetSeed(0);

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    cerr << PHWHERE << " ERROR: Can not find DST node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  sPHElectronPairContainerv1* eePairs = new sPHElectronPairContainerv1();

  PHCompositeNode *PairNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", outnodename));
  if (!PairNode)
  {
    PHObjectNode_t *PairNode = new PHIODataNode<PHObject>(eePairs,outnodename.c_str(),"PHObject");
    dstNode->addNode(PairNode);
    cout << PHWHERE << " INFO: added " << outnodename << endl;
  }
  else { cout << PHWHERE << " INFO: " << outnodename << " already exists." << endl; }

  std::cout << "PairMaker::Init ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================
  
int PairMaker::InitRun(PHCompositeNode *topNode) 
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//==============================================================

int PairMaker::process_event(PHCompositeNode *topNode) 
{
  return process_event_test(topNode);
}

//======================================================================

int PairMaker::process_event_test(PHCompositeNode *topNode) {
  EventNumber++;

//  int howoften = 1; 
//  if((EventNumber-1)%howoften==0) { 
    cout<<"--------------------------- EventNumber = " << EventNumber-1 << endl;
//  }
  
  if(EventNumber==1) topNode->print();

  sPHElectronPairContainerv1 *eePairs = findNode::getClass<sPHElectronPairContainerv1>(topNode,outnodename.c_str());
  if(!eePairs) { cerr << outnodename << " not found!" << endl; } 
    else { cout << "Found " << outnodename << " node." << endl; }

  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!global_vtxmap) { 
    cerr << PHWHERE << " ERROR: Can not find GlobalVertexMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  //cout << "Number of GlobalVertexMap entries = " << global_vtxmap->size() << endl;

  SvtxVertexMap *vtxmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!vtxmap) {
      cout << "SvtxVertexMap node not found!" << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }
  //cout << "Number of SvtxVertexMap entries = " << vtxmap->size() << endl;

  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) {
    cerr << PHWHERE << " ERROR: Can not find SvtxTrackMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  RawClusterContainer* cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!cemc_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_CEMC node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  else { cout << "FOUND CLUSTER_CEMC node." << endl; }

  int mycount = 0;
  RawClusterContainer::ConstRange cluster_range = cemc_clusters->getClusters();
  for (RawClusterContainer::ConstIterator cluster_iter = cluster_range.first; cluster_iter != cluster_range.second; cluster_iter++)
  {
    //RawCluster *cluster = cluster_iter->second;
    //double phi = cluster->get_phi();
    //double z = cluster->get_z();
    //double ee = cluster->get_energy();
    //int ntowers = cluster->getNTowers();
    //if(ee>2.) cout << "cluster: " << ee << " " << ntowers << " " << phi << " " << z << endl;
    mycount++;
  }
  cout << "Number of CEMC clusters = " << mycount << endl;

  TrackPidAssoc *track_pid_assoc =  findNode::getClass<TrackPidAssoc>(topNode, "TrackPidAssoc");
  if(!track_pid_assoc) {
    cerr << PHWHERE << "ERROR: CAN NOT FIND TrackPidAssoc Node!" << endl; 
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  auto electrons = track_pid_assoc->getTracks(TrackPidAssoc::electron);

//    for(auto it = electrons.first; it != electrons.second; ++it)
//    {
//      SvtxTrack *tr = trackmap->get(it->second);
//      double p = tr->get_p();
//      std::cout << " pid " << it->first << " track ID " << it->second << " mom " << p << std::endl;
//    }

  double mult = (double)trackmap->size();
  cout << "   Number of tracks = " << trackmap->size() << endl;
  unsigned int centbin = 99999;
  for(unsigned int i=0; i<NCENT; i++) { if(mult>_multbins[i] && mult <=_multbins[i+1]) { centbin = i; } }
  if(centbin<0 || centbin>=NCENT) {cout << "BAD MULTIPLICITY = " << mult << endl; return Fun4AllReturnCodes::ABORTEVENT; }
  cout << "Centrality bin = " << centbin << endl;

  double _vtxbinsize  = (_ZMAX - _ZMIN)/double(NZ);
  //cout << " _vtxbinsize = " << _vtxbinsize << endl;

  vector<sPHElectronv1> elepos;
//  vector<sPHElectronv1> electrons;
//  vector<sPHElectronv1> positrons;

// My own electron ID
/*
  for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;
    if(!isElectron(track)) continue;
    double px = track->get_px();
    double py = track->get_py();
    double pt = sqrt(px*px + py*py);
    int charge = track->get_charge();
    double x = track->get_x();
    double y = track->get_y();
    double z = track->get_z();
    unsigned int vtxbin = (z - _ZMIN)/_vtxbinsize;
    if(vtxbin<0 || vtxbin>=NZ) continue;
    unsigned int vtxid = track->get_vertex_id();
    if(vtxid<0 || vtxid>=global_vtxmap->size()) continue;
    cout << "electron: "<<charge<<" "<<pt<<" "<<x<<" "<<y<<" "<<z<<" "<<vtxid<<" "<<vtxbin<< endl;
    GlobalVertex* gvtx = global_vtxmap->get(vtxid);
    cout << "global vertex: "<<gvtx->get_x()<<" "<<gvtx->get_y()<<" "<<gvtx->get_z()<<endl;
    sPHElectronv1 tmpel = sPHElectronv1(track);
    tmpel.set_zvtx(gvtx->get_z());
    elepos.push_back(tmpel);
    (_buffer[vtxbin][centbin]).push_back(tmpel);
  } // end loop over tracks
*/

    for(auto it = electrons.first; it != electrons.second; ++it)
    {
      SvtxTrack *track = trackmap->get(it->second);
      unsigned int cemc_clusid = track->get_cal_cluster_id(SvtxTrack::CAL_LAYER::CEMC); 
      TrkrDefs::cluskey cemc_cluskey = track->get_cal_cluster_key(SvtxTrack::CAL_LAYER::CEMC);
      float cemc_cluse = track->get_cal_cluster_e(SvtxTrack::CAL_LAYER::CEMC);
        cout << "CEMC match: " << cemc_clusid << " " << cemc_cluskey << " " << cemc_cluse << endl;
        RawCluster* cluster = cemc_clusters->getCluster(cemc_clusid);
        double ee = cluster->get_energy();
        double ecore = cluster->get_ecore();
        double prob = cluster->get_prob();
        double cemc_chi2 = cluster->get_chi2();
        cout << "cluster: " << ee << " " << ecore << " " << prob << " " << cemc_chi2 << endl;

      double px = track->get_px();
      double py = track->get_py();
      double pt = sqrt(px*px + py*py);
      int charge = track->get_charge();
      double x = track->get_x();
      double y = track->get_y();
      double z = track->get_z();
      unsigned int vtxbin = (z - _ZMIN)/_vtxbinsize;
      if(vtxbin<0 || vtxbin>=NZ) continue;
      unsigned int vtxid = track->get_vertex_id();
      if(vtxid<0 || vtxid>=global_vtxmap->size()) continue;
      cout << "electron: "<<charge<<" "<<pt<<" "<<x<<" "<<y<<" "<<z<<" "<<vtxid<<" "<<vtxbin<< endl;
      GlobalVertex* gvtx = global_vtxmap->get(vtxid);
      cout << "global vertex: "<<gvtx->get_x()<<" "<<gvtx->get_y()<<" "<<gvtx->get_z()<<endl;

      sPHElectronv1 tmpel = sPHElectronv1(track);
      tmpel.set_zvtx(gvtx->get_z());
      tmpel.set_cemc_ecore(ecore);
      tmpel.set_cemc_prob(prob);
      tmpel.set_cemc_chi2(cemc_chi2);

      elepos.push_back(tmpel);
      (_buffer[vtxbin][centbin]).push_back(tmpel);

    }

  cout << "# of electrons/positrons = " << elepos.size() << endl;

  if(elepos.size()>1) {
    for(long unsigned int i=0; i<elepos.size()-1; i++) {
      for(long unsigned int j=i+1; j<elepos.size(); j++) {
        sPHElectronPairv1 pair = sPHElectronPairv1(&elepos[i],&elepos[j]);
        double mass = pair.get_mass();
        int charge1 = (elepos[i]).get_charge();
        int charge2 = (elepos[j]).get_charge();
        int type = 0;
        if(charge1*charge2<0) {type = 1;}
          else if (charge1>0 && charge2>0) {type=2;}
            else if (charge1<0 && charge2<0) {type=3;}
              else {cout << "ERROR: wrong charge!" << endl;}
        cout << "MASS = " << type << " " << mass << endl;
        pair.set_type(type);
        eePairs->insert(&pair);
      }
    }
  }

/*
  for(long unsigned int i=0; i<electrons.size(); i++) {
  for(long unsigned int j=0; j<positrons.size(); j++) {
    sPHElectronPairv1 pair = sPHElectronPairv1(&electrons[i],&positrons[j]);
    pair.set_type(1);
    double mass = pair.get_mass();
    cout << "MASS = " << mass << endl;
    eePairs->insert(&pair);
  }}
*/

  int nmix = MakeMixedPairs(elepos, eePairs, centbin);
  cout << "number of mixed pairs = " << nmix << endl;

  return Fun4AllReturnCodes::EVENT_OK;
} 

//======================================================================

int PairMaker::MakeMixedPairs(std::vector<sPHElectronv1> elepos, sPHElectronPairContainerv1* eePairs, unsigned int centbin) {
  int count=0;
  double _vtxbinsize  = (_ZMAX - _ZMIN)/double(NZ);

  for(unsigned int k=0; k<elepos.size(); k++) {

    sPHElectronv1 thisel = elepos[k];
    double z = thisel.get_zvtx();
    unsigned int vtxbin = (z - _ZMIN)/_vtxbinsize;
    if(vtxbin<0 || vtxbin>=NZ) continue;

    unsigned int _num_mixes = 3;
    unsigned int buffsize = (_buffer[vtxbin][centbin]).size();

    if(buffsize >= _min_buffer_depth) {
      for(unsigned int i=0; i<_num_mixes; i++) {
        double rnd = _rng->Uniform();
        unsigned int irnd = rnd * buffsize;
        sPHElectronv1 mixel = (_buffer[vtxbin][centbin]).at(irnd);

        sPHElectronPairv1 pair = sPHElectronPairv1(&thisel,&mixel);
        int charge1 = thisel.get_charge();
        int charge2 = mixel.get_charge();
        int type = 0;
        if(charge1*charge2<0) {type = 4;}
          else if (charge1>0 && charge2>0) {type=5;}
            else if (charge1<0 && charge2<0) {type=6;}
              else {cout << "ERROR: wrong charge!" << endl;}
        pair.set_type(type);
        eePairs->insert(&pair);
        cout << "Inserted MIXED pair with mass = " << type << " " << pair.get_mass() << endl;
        count++;
      } // end i loop
      (_buffer[vtxbin][centbin]).push_back(elepos[k]);  // keep filling buffer
    }
    else {
      (_buffer[vtxbin][centbin]).push_back(elepos[k]);  // keep filling buffer
    }

  } //end k loop over elepos entries

  return count;
}

//======================================================================

bool PairMaker::isElectron(SvtxTrack* trk) 
{
  double px = trk->get_px();
  double py = trk->get_py();
  double pz = trk->get_pz();
  double pt = sqrt(px*px+py*py);
  double pp = sqrt(pt*pt+pz*pz);
  double e3x3 = trk->get_cal_energy_3x3(SvtxTrack::CAL_LAYER::CEMC);
  if(pt<2.0) return false;
  if(pp==0.) return false;
  if(isnan(e3x3)) return false;
  if(e3x3/pp<0.7) return false;
  double chisq = trk->get_chisq();
  double ndf = trk->get_ndf();
  if((chisq/ndf)>10.) return false;
  int nmvtx = 0;
  int ntpc = 0;
  for (SvtxTrack::ConstClusterKeyIter iter = trk->begin_cluster_keys();
     iter != trk->end_cluster_keys();
     ++iter)
  {
    TrkrDefs::cluskey cluster_key = *iter;
    int trackerid = TrkrDefs::getTrkrId(cluster_key);
    if(trackerid==0) nmvtx++;
    if(trackerid==2) ntpc++;
  }
  if(nmvtx<1) return false;
  if(ntpc<20) return false;

  return true;
}

//==============================================================================

int PairMaker::End(PHCompositeNode *topNode) 
{
  cout << "END: ====================================" << endl;
  cout << "mixing buffers: " << endl;
  for(unsigned int i=0; i<NZ; i++) {
    for(unsigned int j=0; j<NCENT; j++) {
      cout << i << " " << j << "    " << (_buffer[i][j]).size() << endl;
      (_buffer[i][j]).clear();
    }
  }
  cout << "=========================================" << endl;

  return Fun4AllReturnCodes::EVENT_OK;
}


