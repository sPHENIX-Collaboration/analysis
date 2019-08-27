#include "VtxTest.h"
#include "Conversion.h"
#include "SVReco.h"
//#include "../PhotonConversion/RaveVertexingAux.h"

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calotrigger/CaloTriggerInfo.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Particlev1.h>
#include <g4main/PHG4Particlev2.h>
#include <g4main/PHG4VtxPoint.h>

/*#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>*/
#include <trackbase/TrkrClusterContainer.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>

//#include <GenFit/GFRaveConverters.h>
#include <GenFit/FieldManager.h>
#include <GenFit/GFRaveVertex.h>
#include <GenFit/GFRaveVertexFactory.h>
#include <GenFit/MeasuredStateOnPlane.h>
#include <GenFit/RKTrackRep.h>
#include <GenFit/StateOnPlane.h>
#include <GenFit/Track.h>

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>

#include <utility>
#include <iostream>
#include <math.h>

VtxTest::VtxTest(const std::string &name, unsigned int runnumber, 
    int particleEmbed,  int pythiaEmbed,bool makeTTree=true) : SubsysReco("VtxTest"),
  _kRunNumber(runnumber),_kParticleEmbed(particleEmbed), _kPythiaEmbed(pythiaEmbed), _kMakeTTree(makeTTree)
{
  _foutname = name;
}

VtxTest::~VtxTest(){
  if (_f) delete _f;
  if (_vertexer) delete _vertexer;
}

int VtxTest::InitRun(PHCompositeNode *topNode)
{
  _vertexer = new SVReco();
  _vertexer->InitRun(topNode);
  if(_kMakeTTree){
    _b_event=0;
    _runNumber=_kRunNumber;
    _f = new TFile( _foutname.c_str(), "RECREATE");
    _tree = new TTree("ttree","conversion data");
    _tree->SetAutoSave(300);
    _tree->Branch("runNumber",&_runNumber);
    _tree->Branch("event",&_b_event); 
    _tree->Branch("nVtx", &_b_nVtx);
    _tree->Branch("nTpair", &_b_Tpair);
    _tree->Branch("nRpair", &_b_Rpair);
    _tree->Branch("rVtx", _b_rVtx,"rVtx[nVtx]/D");
    _tree->Branch("pythia", _b_pythia,"pythia[nVtx]/B");
    _tree->Branch("electron_pt", _b_electron_pt,"electron_pt[nVtx]/F");
    _tree->Branch("positron_reco_pt", _b_positron_reco_pt,"positron_reco_pt[nTpair]/F");
    _tree->Branch("electron_reco_pt", _b_electron_reco_pt,"electron_reco_pt[nTpair]/F");
    _tree->Branch("positron_pt", _b_positron_pt,"positron_pt[nVtx]/F");
    _tree->Branch("photon_pt",   _b_parent_pt    ,"photon_pt[nVtx]/F");
    _tree->Branch("photon_eta",  _b_parent_eta  ,"photon_eta[nVtx]/F");
    _tree->Branch("photon_phi",  _b_parent_phi  ,"photon_phi[nVtx]/F");
    _tree->Branch("e_deta",  _b_e_deta  ,"e_deta[nTpair]/F");
    _tree->Branch("e_dphi",  _b_e_dphi  ,"e_dphi[nTpair]/F");
    _tree->Branch("fLayer",_b_fLayer,"fLayer[nTpair]/I");
    _tree->Branch("photon_source_id",  _b_grandparent_id  ,"photon_source_id[nVtx]/I");
    _tree->Branch("nCluster",_b_nCluster,"nCluster[nRpair]/I");
    _tree->Branch("clus_dphi",_b_cluster_dphi,"clus_dphi[nRpair]/F");
    _tree->Branch("clus_deta",_b_cluster_deta,"clus_deta[nRpair]/F");
    _tree->Branch("Scluster_prob", &_b_Scluster_prob,"Scluster_prob[nRpair]/F");
    _tree->Branch("Mcluster_prob", &_b_Mcluster_prob,"Mcluster_prob[nRpair]/F");

    _signalCutTree = new TTree("cutTreeSignal","signal data for making track pair cuts");
    _signalCutTree->SetAutoSave(300);
    _signalCutTree->Branch("track_deta", &_b_track_deta);
    _signalCutTree->Branch("track_dphi", &_b_track_dphi);
    _signalCutTree->Branch("track_dlayer",&_b_track_dlayer);
    _signalCutTree->Branch("track_layer", &_b_track_layer);
    _signalCutTree->Branch("track_pT", &_b_track_pT);
    _signalCutTree->Branch("ttrack_pT", &_b_ttrack_pT);
    _signalCutTree->Branch("approach_dist", &_b_approach);
    _signalCutTree->Branch("vtx_radius", &_b_vtx_radius);
    _signalCutTree->Branch("tvtx_radius", &_b_tvtx_radius);
    _signalCutTree->Branch("vtx_phi", &_b_vtx_phi);
    _signalCutTree->Branch("vtx_eta", &_b_vtx_eta);
    _signalCutTree->Branch("vtx_x", &_b_vtx_x);
    _signalCutTree->Branch("vtx_y", &_b_vtx_y);
    _signalCutTree->Branch("vtx_z", &_b_vtx_z);
    _signalCutTree->Branch("tvtx_eta", &_b_tvtx_eta);
    _signalCutTree->Branch("tvtx_x", &_b_tvtx_x);
    _signalCutTree->Branch("tvtx_y", &_b_tvtx_y);
    _signalCutTree->Branch("tvtx_z", &_b_tvtx_z);
    _signalCutTree->Branch("tvtx_phi", &_b_tvtx_phi);
    _signalCutTree->Branch("vtx_chi2", &_b_vtx_chi2);
    _signalCutTree->Branch("vtxTrack_dist", &_b_vtxTrack_dist);
    _signalCutTree->Branch("photon_m", &_b_photon_m);
    _signalCutTree->Branch("photon_pT", &_b_photon_pT);
    _signalCutTree->Branch("cluster_prob", &_b_cluster_prob);

    _h_backgroundCutTree = new TTree("cutTreeBackh","background data for making track pair cuts");
    _h_backgroundCutTree->SetAutoSave(300);
    _h_backgroundCutTree->Branch("track_deta", &_bb_track_deta);
    _h_backgroundCutTree->Branch("track_dphi", &_bb_track_dphi);
    _h_backgroundCutTree->Branch("track_dlayer", &_bb_track_dlayer);
    _h_backgroundCutTree->Branch("track_layer", &_bb_track_layer);
    _h_backgroundCutTree->Branch("track_pT", &_bb_track_pT);
    _h_backgroundCutTree->Branch("vtx_radius", &_bb_vtx_radius);
    _h_backgroundCutTree->Branch("vtx_chi2", &_bb_vtx_chi2);
    _h_backgroundCutTree->Branch("approach_dist", &_bb_approach);
    _h_backgroundCutTree->Branch("vtxTrack_dist", &_bb_vtxTrack_dist);
    _h_backgroundCutTree->Branch("photon_m", &_bb_photon_m);
    _h_backgroundCutTree->Branch("photon_pT", &_bb_photon_pT);
    _h_backgroundCutTree->Branch("cluster_prob", &_bb_cluster_prob);
    _h_backgroundCutTree->Branch("pid", &_bb_pid);

    _e_backgroundCutTree = new TTree("cutTreeBacke","background data for making track pair cuts");
    _e_backgroundCutTree->SetAutoSave(300);
    _e_backgroundCutTree->Branch("track_deta", &_bb_track_deta);
    _e_backgroundCutTree->Branch("track_dphi", &_bb_track_dphi);
    _e_backgroundCutTree->Branch("track_dlayer", &_bb_track_dlayer);
    _e_backgroundCutTree->Branch("track_layer", &_bb_track_layer);
    _e_backgroundCutTree->Branch("track_pT", &_bb_track_pT);
    _e_backgroundCutTree->Branch("vtx_radius", &_bb_vtx_radius);
    _e_backgroundCutTree->Branch("vtx_chi2", &_bb_vtx_chi2);
    _e_backgroundCutTree->Branch("approach_dist", &_bb_approach);
    _e_backgroundCutTree->Branch("vtxTrack_dist", &_bb_vtxTrack_dist);
    _e_backgroundCutTree->Branch("photon_m", &_bb_photon_m);
    _e_backgroundCutTree->Branch("photon_pT", &_bb_photon_pT);
    _e_backgroundCutTree->Branch("cluster_prob", &_bb_cluster_prob);
  }
  return 0;
}

void VtxTest::doNodePointers(PHCompositeNode* topNode){
  _mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  _truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  _clusterMap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  //  _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");
  //if(!_mainClusterContainer||!_truthinfo||!_clusterMap||!_hitMap){
  if(!_mainClusterContainer||!_truthinfo||!_clusterMap){
    cerr<<Name()<<": critical error-bad nodes \n";
    if(!_mainClusterContainer){
      cerr<<"\t RawClusterContainer is bad";
    }
    if(!_truthinfo){
      cerr<<"\t TruthInfoContainer is bad";
    }
    if(!_clusterMap){
      cerr<<"\t TrkrClusterMap is bad";
    }
    /*if(!_hitMap){
      cerr<<"\t SvtxHitMap is bad";
      }*/
    cerr<<endl;
  }
}

int VtxTest::process_event(PHCompositeNode *topNode)
{
  doNodePointers(topNode);
  _vertexer->InitEvent(topNode);
  PHG4TruthInfoContainer::Range range = _truthinfo->GetParticleRange(); //look at all truth particles
  SvtxEvalStack *stack = new SvtxEvalStack(topNode); //truth tracking info
  SvtxTrackEval* trackeval = stack->get_track_eval();
  SvtxTrack *svtxtrack1=NULL;
  SvtxTrack *svtxtrack2=NULL;
  PHG4VtxPoint *truth_vtx=NULL;
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second;
    if(g4particle&&g4particle->get_pid()==211&&!_truthinfo->GetParticle(g4particle->get_parent_id())){
      cout<<"found pion\n";
      TLorentzVector truth;
      truth.SetPxPyPzE(g4particle->get_px(),g4particle->get_py(),g4particle->get_pz(),g4particle->get_e());
      if(!svtxtrack1)svtxtrack1=trackeval->best_track_from(g4particle);
      else svtxtrack2=trackeval->best_track_from(g4particle);
      if(truth_vtx==NULL) truth_vtx=_truthinfo->GetVtx(g4particle->get_vtx_id()); //get the vertex
      else{
        PHG4VtxPoint *vtemp = truth_vtx;
        truth_vtx=_truthinfo->GetVtx(g4particle->get_vtx_id());
        if(!(*truth_vtx==*vtemp)) cout<<"vtx does not agree"<<endl;
      }
    }
  }
  if(!truth_vtx){
    cout<<"did not find truth_vtx"<<endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if(!svtxtrack1||!svtxtrack2){
    cout<<"did not find tracks"<<endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  genfit::GFRaveVertex* recoVert=_vertexer->findSecondaryVertex(svtxtrack1,svtxtrack2);
  cout<<"here"<<endl;
  if (recoVert)
  {
    TVector3 recoVertPos = recoVert->getPos();
    _b_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
    _b_tvtx_radius = sqrt(truth_vtx->get_x()*truth_vtx->get_x()+truth_vtx->get_y()*truth_vtx->get_y());
    _b_vtx_phi = recoVertPos.Phi();
    _b_vtx_eta = recoVertPos.Eta();
    _b_vtx_z = recoVertPos.Z();
    _b_vtx_x = recoVertPos.X();
    _b_vtx_y = recoVertPos.Y();
    TVector3 tVertPos(truth_vtx->get_x(),truth_vtx->get_y(),truth_vtx->get_z());
    _b_tvtx_phi = tVertPos.Phi();
    _b_tvtx_eta = tVertPos.Eta();
    _b_tvtx_z = tVertPos.Z();
    _b_tvtx_x = tVertPos.X();
    _b_tvtx_y = tVertPos.Y();
    _b_vtx_chi2 = recoVert->getChi2();
  }
  else{
    _b_vtx_radius = 0;
    _b_vtx_phi = 0;
    _b_vtx_eta = 0;
    _b_vtx_z = 0;
    _b_vtx_x = 0;
    _b_vtx_y = 0;
    _b_tvtx_radius = sqrt(truth_vtx->get_x()*truth_vtx->get_x()+truth_vtx->get_y()*truth_vtx->get_y());
    TVector3 tVertPos(truth_vtx->get_x(),truth_vtx->get_y(),truth_vtx->get_z());
    _b_tvtx_phi = tVertPos.Phi();
    _b_tvtx_eta = tVertPos.Eta();
    _b_tvtx_z = tVertPos.Z();
    _b_tvtx_x = tVertPos.X();
    _b_tvtx_y = tVertPos.Y();
  }
  delete stack;
  _signalCutTree->Fill();  
  return 0;
}

std::queue<std::pair<int,int>> VtxTest::numUnique(std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL,RawClusterContainer *mainClusterContainer=NULL){
  _b_nVtx = 0;
  _b_Tpair=0;
  _b_Rpair=0;
  std::queue<std::pair<int,int>> missingChildren;//this is deprecated
  for (std::map<int,Conversion>::iterator i = mymap->begin(); i != mymap->end(); ++i) {
    PHG4VtxPoint *vtx =i->second.getVtx(); //get the vtx
    PHG4Particle *temp = i->second.getPhoton(); //set the photon
    TLorentzVector tlv_photon,tlv_electron,tlv_positron; //make tlv for each particle 
    tlv_photon.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //intialize
    temp=i->second.getElectron(); //set the first child 
    tlv_electron.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
    if(_kMakeTTree){//fill tree values
      cout<<"numUnique filling tree"<<endl;
      _b_rVtx[_b_nVtx] = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y()); //find the radius
      _b_parent_pt[_b_nVtx] =tlv_photon.Pt();
      _b_parent_phi[_b_nVtx]=tlv_photon.Phi();
      _b_parent_eta[_b_nVtx]=tlv_photon.Eta();
      _b_grandparent_id[_b_nVtx]=i->second.getSourceId();
      _b_e_deta[_b_nVtx]=-1.;
      _b_e_dphi[_b_nVtx]=-1.;
      _b_positron_pt[_b_nVtx]=-1.;
      _b_electron_pt[_b_nVtx]=tlv_electron.Pt();
    }
    temp=i->second.getPositron();
    if(temp){ //this will be false for conversions with 1 truth track
      tlv_positron.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //init the tlv
      if(_kMakeTTree) _b_positron_pt[_b_nVtx]=tlv_positron.Pt(); //fill tree
      if (TMath::Abs(tlv_electron.Eta())<_kRAPIDITYACCEPT&&TMath::Abs(tlv_positron.Eta())<_kRAPIDITYACCEPT)
      {
        unsigned int nRecoTracks = i->second.setRecoTracks(trackeval); //find the reco tracks for this conversion
        if(_kMakeTTree){
          _b_e_deta[_b_Tpair]=TMath::Abs(tlv_electron.Eta()-tlv_positron.Eta());
          _b_e_dphi[_b_Tpair]=TMath::Abs(tlv_electron.Phi()-tlv_positron.Phi());
          _b_fLayer[_b_Tpair]=_b_track_layer = i->second.firstLayer(_clusterMap); 
          _b_fLayer[_b_Tpair]=-1;
          _b_Tpair++;
        }//tree
        switch(nRecoTracks)
        {
          case 2: //there are 2 reco tracks
            {
              if(_kMakeTTree){
                _b_track_deta = i->second.trackDEta();
                _b_track_dphi = i->second.trackDPhi();
                _b_track_dlayer = i->second.trackDLayer(_clusterMap);
                _b_track_dlayer=-1;
                _b_track_pT = i->second.minTrackpT();
                if(tlv_electron.Pt()>tlv_positron.Pt()) _b_ttrack_pT = tlv_positron.Pt();
                else _b_ttrack_pT = tlv_electron.Pt();
                _b_approach = i->second.approachDistance();
                pair<SvtxTrack*, SvtxTrack*> reco_tracks=i->second.getRecoTracks();
                genfit::GFRaveVertex* recoVert = _vertexer->findSecondaryVertex(reco_tracks.first,reco_tracks.second);
                if (recoVert)
                {
                  TVector3 recoVertPos = recoVert->getPos();
                  _b_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
                  _b_tvtx_radius = _b_rVtx[_b_nVtx];
                  _b_vtx_phi = recoVertPos.Phi();
                  _b_vtx_eta = recoVertPos.Eta();
                  _b_vtx_z = recoVertPos.Z();
                  _b_vtx_x = recoVertPos.X();
                  _b_vtx_y = recoVertPos.Y();
                  TVector3 tVertPos(vtx->get_x(),vtx->get_y(),vtx->get_z());
                  _b_tvtx_phi = tVertPos.Phi();
                  _b_tvtx_eta = tVertPos.Eta();
                  _b_tvtx_z = tVertPos.Z();
                  _b_tvtx_x = tVertPos.X();
                  _b_tvtx_y = tVertPos.Y();
                  _b_vtx_chi2 = recoVert->getChi2();
                  _b_vtxTrack_dist = i->second.dist(&recoVertPos,_clusterMap);
                }

                TLorentzVector* recoPhoton = i->second.setRecoPhoton();
                if (recoPhoton)
                {
                  _b_photon_m=recoPhoton->Dot(*recoPhoton);
                  _b_photon_pT=recoPhoton->Pt();
                }
                else{ //photon was not reconstructed
                  _b_photon_m =-1;
                  _b_photon_pT=-1;
                }
                //reset the values
                _b_cluster_prob=-1;
                _b_cluster_deta[_b_Rpair]=-1.;
                _b_cluster_dphi[_b_Rpair]=-1.;
                _b_nCluster[_b_Rpair]=0;
                _b_Scluster_prob[_b_Rpair]=-1;
                _b_Mcluster_prob[_b_Rpair]=-1;

              }
              pair<int,int> clusterIds = i->second.get_cluster_ids();
              RawCluster *clustemp;
              if(mainClusterContainer->getCluster(clusterIds.first)){//if there is matching cluster 
                clustemp =   dynamic_cast<RawCluster*>(mainClusterContainer->getCluster(clusterIds.first)->Clone());
                _conversionClusters.AddCluster(clustemp); //add the calo cluster to the container
                if (_kMakeTTree)
                {
                  _b_cluster_prob=clustemp->get_prob();
                  RawCluster *clus2 = mainClusterContainer->getCluster(clusterIds.second);
                  if (clus2)
                  {
                    _b_cluster_dphi[_b_Rpair]=fabs(clustemp->get_phi()-clus2->get_phi());
                    TVector3 etaCalc(clustemp->get_x(),clustemp->get_y(),clustemp->get_z());
                    if (clus2->get_prob()>_b_cluster_prob)
                    {
                      _b_cluster_prob=clus2->get_prob();
                    }
                    //calculate deta
                    float eta1 = etaCalc.PseudoRapidity();
                    etaCalc.SetXYZ(clus2->get_x(),clus2->get_y(),clus2->get_z());
                    _b_cluster_deta[_b_Rpair]=fabs(eta1-etaCalc.PseudoRapidity());
                    /* this may be a logical bug. What are S and M? If this is false the identical cluster prob is recorded twice.*/
                    if (clusterIds.first!=clusterIds.second) //if there are two district clusters
                    {
                      _b_nCluster[_b_Rpair]=2;
                      _b_Scluster_prob[_b_Rpair]=clustemp->get_prob();
                    }
                    else{
                      _b_nCluster[_b_Rpair]=1;
                      _b_Mcluster_prob[_b_Rpair]=clustemp->get_prob();
                    }
                  }
                }
              }
              _signalCutTree->Fill();  
              _b_Rpair++;
              break;
            }
          case 1: //there's one reco track
            {
              int clustidtemp=i->second.get_cluster_id(); //get the cluster id of the current conversion
              if(mainClusterContainer->getCluster(clustidtemp)){//if thre is matching cluster 
                RawCluster *clustemp =   dynamic_cast<RawCluster*>(mainClusterContainer->getCluster(clustidtemp)->Clone());
                _conversionClusters.AddCluster(clustemp); //add the calo cluster to the container
              }
              //cout<<"Matched 1 reco with layer="<<i->second.firstLayer(_clusterMap)<<"pTs:"<<tlv_electron.Pt()<<"-"<<tlv_positron.Pt()<<'\n';
              break;
            }
          case 0: //no reco tracks
            break;
          default:
            if (Verbosity()>1)
            {
              cerr<<Name()<<" error setting reco tracks"<<endl;
            }
            break;
        }//switch
      }//rapidity cut
      if (_kMakeTTree)
      {
        _b_pythia[_b_nVtx]=i->second.getEmbed()==_kPythiaEmbed;
        _b_nVtx++; 
      }
    }// has 2 truth tracks
  }//map loop
  return missingChildren;
}

//only call if _kMakeTTree is true
void VtxTest::processBackground(std::map<int,Conversion> *mymap,SvtxTrackEval* trackeval,TTree* tree){
  for (std::map<int,Conversion>::iterator i = mymap->begin(); i != mymap->end(); ++i) {
    int nReco=i->second.setRecoTracks(trackeval);
    if (nReco==2)
    {
      _bb_track_deta = i->second.trackDEta();
      _bb_track_dphi = i->second.trackDPhi();
      _bb_track_dlayer = i->second.trackDLayer(_clusterMap);
      _bb_track_layer = i->second.firstLayer(_clusterMap);
      _bb_track_layer=-1;
      _bb_track_dlayer=-1;
      _bb_track_pT = i->second.minTrackpT();
      _bb_approach = i->second.approachDistance();
      _bb_pid = i->second.getElectron()->get_pid();
      pair<SvtxTrack*, SvtxTrack*> reco_tracks=i->second.getRecoTracks();
      genfit::GFRaveVertex* recoVert = _vertexer->findSecondaryVertex(reco_tracks.first,reco_tracks.second);
      if (recoVert)
      {
        TVector3 recoVertPos = recoVert->getPos();
        _bb_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
        _bb_vtx_chi2 = recoVert->getChi2();
        _bb_vtxTrack_dist = i->second.dist(&recoVertPos,_clusterMap);
      }
      TLorentzVector* recoPhoton = i->second.setRecoPhoton();
      if (recoPhoton)
      {
        _bb_photon_m=recoPhoton->Dot(*recoPhoton);
        _bb_photon_pT=recoPhoton->Pt();
      }
      else{
        _bb_photon_m =0;
        _bb_photon_pT=0;
      }
      _bb_cluster_prob= _mainClusterContainer->getCluster(i->second.get_cluster_id())->get_prob();
      tree->Fill();
    }
  }
}

void VtxTest::findChildren(std::queue<std::pair<int,int>> missingChildren,PHG4TruthInfoContainer* _truthinfo){
  if (Verbosity()>=6)
  {
    while(!missingChildren.empty()){
      for (PHG4TruthInfoContainer::ConstIterator iter = _truthinfo->GetParticleRange().first; iter != _truthinfo->GetParticleRange().second; ++iter)
      {
        if(Verbosity()>1&& iter->second->get_parent_id()==missingChildren.front().first&&iter->second->get_track_id()!=missingChildren.front().second){
          cout<<"Found Child:\n";
          iter->second->identify();
          cout<<"With mother:\n";

        }
      }
      missingChildren.pop();
    }
  }
}

const RawClusterContainer* VtxTest::getClusters()const {return &_conversionClusters;} 

int VtxTest::get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo)const{
  return truthinfo->isEmbeded(particle->get_track_id());
}

float VtxTest::vtoR(PHG4VtxPoint* vtx)const{
  return (float) sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
}


int VtxTest::End(PHCompositeNode *topNode)
{
  if(_kMakeTTree){
    _f->Write();
    _f->Close();
  }
  return 0;
}
