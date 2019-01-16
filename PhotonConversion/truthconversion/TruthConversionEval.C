#include "TruthConversionEval.h"


#include <calotrigger/CaloTriggerInfo.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>


#include <utility>
#include <iostream>
#include <math.h>

TruthConversionEval::TruthConversionEval(const std::string &name, unsigned int runnumber, 
    int particleEmbed,  int pythiaEmbed,bool makeTTree=true) : SubsysReco("TruthConversionEval"),
  _kRunNumber(runnumber),_kParticleEmbed(particleEmbed), _kPythiaEmbed(pythiaEmbed), _kMakeTTree(makeTTree)
{
  _foutname = name;
}

TruthConversionEval::~TruthConversionEval(){
  if (_f) delete _f;
}

int TruthConversionEval::InitRun(PHCompositeNode *topNode)
{
  _b_event=0;
  _runNumber=_kRunNumber;
  if(_kMakeTTree){
    _f = new TFile( _foutname.c_str(), "RECREATE");
    _tree = new TTree("ttree","conversion data");
    _tree->SetAutoSave(300);
    _tree->Branch("runNumber",&_runNumber);
    _tree->Branch("event",&_b_event); 
    _tree->Branch("nVtx", &_b_nVtx);
    _tree->Branch("nTpair", &_b_Tpair);
    _tree->Branch("nRpair", &_b_Rpair);
    _tree->Branch("rVtx", _b_rVtx,"rVtx[nVtx]/F");
    _tree->Branch("pythia", _b_pythia,"pythia[nVtx]/B");
    _tree->Branch("electron_pt", _b_electron_pt,"electron_pt[nVtx]/F");
    _tree->Branch("positron_pt", _b_positron_pt,"positron_pt[nVtx]/F");
    _tree->Branch("photon_pt",   _b_parent_pt    ,"photon_pt[nVtx]/F");
    _tree->Branch("photon_eta",  _b_parent_eta  ,"photon_eta[nVtx]/F");
    _tree->Branch("photon_phi",  _b_parent_phi  ,"photon_phi[nVtx]/F");
    _tree->Branch("photon_source_id",  _b_grandparent_id  ,"photon_source_id[nVtx]/I");
    _signalCutTree = new TTree("cutTreeSignal","signal data for making track pair cuts");
    _signalCutTree->SetAutoSave(300);
    _signalCutTree->Branch("track_deta", &_b_track_deta);
    _signalCutTree->Branch("track_dlayer",&_b_track_dlayer);
    _signalCutTree->Branch("track_layer", &_b_track_layer);
    _signalCutTree->Branch("approach_dist", &_b_approach);
    _signalCutTree->Branch("vtx_radius", &_b_vtx_radius);
    _signalCutTree->Branch("vtx_chi2", &_b_vtx_chi2);
    _signalCutTree->Branch("vtxTrack_dist", &_b_vtxTrack_dist);
    _signalCutTree->Branch("photon_m", &_b_photon_m);
    _signalCutTree->Branch("photon_pT", &_b_photon_pT);
    _backgroundCutTree = new TTree("cutTreeBack","background data for making track pair cuts");
    _backgroundCutTree->SetAutoSave(300);
    _backgroundCutTree->Branch("track_deta", &_bb_track_deta);
    _backgroundCutTree->Branch("track_dlayer", &_bb_track_dlayer);
    _backgroundCutTree->Branch("track_layer", &_bb_track_layer);
    _backgroundCutTree->Branch("vtx_radius", &_bb_vtx_radius);
    _backgroundCutTree->Branch("vtx_chi2", &_bb_vtx_chi2);
    _backgroundCutTree->Branch("approach_dist", &_bb_approach);
    _backgroundCutTree->Branch("vtxTrack_dist", &_bb_vtxTrack_dist);
    _backgroundCutTree->Branch("photon_m", &_bb_photon_m);
    _backgroundCutTree->Branch("photon_pT", &_bb_photon_pT);
  }
  return 0;
}

int TruthConversionEval::process_event(PHCompositeNode *topNode)
{
  doNodePointers(topNode);
  _conversionClusters.Reset(); //clear the list of conversion clusters
  PHG4TruthInfoContainer::Range range = _truthinfo->GetParticleRange();
  SvtxEvalStack *stack = new SvtxEvalStack(topNode);
  SvtxTrackEval* trackeval = stack->get_track_eval();
  if (!trackeval)
  {
    cerr<<"NULL track eval in " <<Name()<<" fatal error"<<endl;
    return 1;
  }
  //make a map of the conversions
  std::map<int,Conversion> mapConversions;
  std::map<int,Conversion> backgroundMap;
  unsigned int backi=0;
  unsigned int backmod=0;
  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second; 
    PHG4Particle* parent =_truthinfo->GetParticle(g4particle->get_parent_id());
    float radius=0;
    if(parent){ //if the particle is not primary find its vertex 
      //check that the parent is an embeded(2) photon or a pythia(3) photon that converts
      PHG4VtxPoint* vtx=_truthinfo->GetVtx(g4particle->get_vtx_id()); //get the vertex
      int parentEmbedID = get_embed(parent,_truthinfo);
      if(parentEmbedID==_kParticleEmbed||(parentEmbedID==_kPythiaEmbed
            &&parent->get_pid()==22&&TMath::Abs(g4particle->get_pid())==11)){
        radius=sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
        if (radius<s_kTPCRADIUS) //limits to truth conversions within the tpc radius
        { 
          //initialize the conversion object -don't use constructor b/c setters have error handling
          if (Verbosity()==10)
          {
            std::cout<<"Conversion with radius [cm]:"<<radius<<'\n';
          }
          (mapConversions[vtx->get_id()]).setElectron(g4particle);
          (mapConversions[vtx->get_id()]).setVtx(vtx);
          (mapConversions[vtx->get_id()]).setParent(parent);
          (mapConversions[vtx->get_id()]).setEmbed(parentEmbedID);
          PHG4Particle* grand =_truthinfo->GetParticle(parent->get_parent_id());
          if (grand) (mapConversions[vtx->get_id()]).setSourceId(grand->get_pid());
          else (mapConversions[vtx->get_id()]).setSourceId(0);
        }
      }
      else if(trackeval->best_track_from(g4particle)){ //not a conversion but has a track
        (backgroundMap[backi]).setElectron(g4particle);
        (backgroundMap[backi]).setVtx(vtx);
        (backgroundMap[backi]).setParent(parent);
        (backgroundMap[backi]).setEmbed(parentEmbedID);
        if(++backmod%2==0){
          backi++;
        }
      }
    }
  }
  //pass the map to this helper method which fills the fields for the TTree 
  numUnique(&mapConversions,trackeval,_mainClusterContainer);
  //std::queue<std::pair<int,int>> missingChildren= numUnique(&vtxList,&mapConversions,trackeval,mainClusterContainer);
  if (Verbosity()==10)
  {
    cout<<Name()<<"# conversion clusters="<<_conversionClusters.size()<<'\n';
  }
  //findChildren(missingChildren,truthinfo);
  if (_kMakeTTree)
  {
    _tree->Fill();
    processBackground(&backgroundMap,trackeval);
  }
  if (Verbosity()>=8)
  {
    std::cout<<Name()<<" found "<<_b_nVtx<<" truth conversions \n";
  }
  _b_event++;
  delete stack;
  if (_vertexer) delete _vertexer;
  return 0;
}

std::queue<std::pair<int,int>> TruthConversionEval::numUnique(std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL,RawClusterContainer *mainClusterContainer=NULL){

  _b_nVtx = 0;
  _b_Tpair=0;
  _b_Rpair=0;
  std::queue<std::pair<int,int>> missingChildren;
  for (std::map<int,Conversion>::iterator i = mymap->begin(); i != mymap->end(); ++i) {
    //fill the tree
    PHG4VtxPoint *vtx =i->second.getVtx(); //get the vtx
    _b_rVtx[_b_nVtx] = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y()); //find the radius
    PHG4Particle *temp = i->second.getPhoton(); //set the photon
    TLorentzVector tlv_photon,tlv_electron,tlv_positron; //make tlv for each particle 
    tlv_photon.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //intialize
    //fill tree values
    _b_parent_pt[_b_nVtx] =tlv_photon.Pt();
    _b_parent_phi[_b_nVtx]=tlv_photon.Phi();
    _b_parent_eta[_b_nVtx]=tlv_photon.Eta();
    _b_grandparent_id[_b_nVtx]=i->second.getSourceId();
    temp=i->second.getElectron(); //set the first child 
    tlv_electron.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e());
    _b_electron_pt[_b_nVtx]=tlv_electron.Pt(); //fill tree
    temp=i->second.getPositron();
    if(temp){ //this will be false for conersions with 1 truth track
      tlv_positron.SetPxPyPzE(temp->get_px(),temp->get_py(),temp->get_pz(),temp->get_e()); //init the tlv
      _b_positron_pt[_b_nVtx]=tlv_positron.Pt(); //fill tree
      if (TMath::Abs(tlv_electron.Eta())<_kRAPIDITYACCEPT&&TMath::Abs(tlv_positron.Eta())<_kRAPIDITYACCEPT)
      {
        _b_Tpair++;
        unsigned int nRecoTracks = i->second.setRecoTracks(trackeval); //find the reco tracks for this conversion
        int clustidtemp=-1;
        switch(nRecoTracks){
          case 2: //there are 2 reco tracks
            if(_kMakeTTree){
              _b_track_deta = i->second.trackDEta();
              _b_track_dlayer = i->second.trackDLayer(_svtxClusterMap,_hitMap);
              _b_track_layer = i->second.firstLayer(_svtxClusterMap);
              _b_approach = i->second.approachDistance();
              /*The recoVtx finding doesn't work yet so using truth vtx for now
               * pair<SvtxTrack*,SvtxTrack*> recoTracks = i->second.getRecoTracks();
               pair<SvtxVertex*,SvtxVertex*> recoTracks = i->second.getRecoTracks();
               SvtxVertex* vtx = _vertexer->makeVtx(recoTracks.first,recoTracks.second);
               if(vtx) _b_vtx_radius =sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
               else _b_vtx_radius=-1;
               SvtxVertex *recoVtx = i->second.getRecoVtx();
               if (!recoVtx)
               {
               cout<<"reco vtx is null"<<endl;
               }*/
              _b_vtx_radius = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
              //_b_vtx_chi2 = recoVtx->get_chisq();
              _b_vtxTrack_dist = i->second.dist(vtx,_svtxClusterMap);
              TLorentzVector* recoPhoton = i->second.setRecoPhoton();
              if (recoPhoton)
              {
                _b_photon_m=recoPhoton->Dot(*recoPhoton);
                _b_photon_pT=recoPhoton->Pt();
              }
              else{
                _b_photon_m =0;
                _b_photon_pT=0;
              }
              _signalCutTree->Fill();   
            }
            _b_Rpair++;
            clustidtemp=i->second.get_cluster_id(); //get the cluster id of the current conversion
            break;
          case 1: //there's one reco track
            clustidtemp=i->second.get_cluster_id(); //get the cluster id of the current conversion
            break;
          case 0: //no reco tracks
            break;
          default:
            if (Verbosity()>1)
            {
              cerr<<Name()<<" error setting reco tracks"<<endl;
            }
            break;
        }
        if(mainClusterContainer->getCluster(clustidtemp)){//if thre is matching cluster 
          RawCluster *clustemp =   dynamic_cast<RawCluster*>(mainClusterContainer->getCluster(clustidtemp)->Clone());
          _conversionClusters.AddCluster(clustemp); //add the calo cluster to the container
        }
      }
    }
    /*this code has been turned off because it is not currently useful 
      to analyze the conversions with only one truth track 
      else{ //fails the truth 2 track check
      cout<<"1 track event \n";
      temp=i->second.getElectron(); //go back to the first track 
      _b_positron_pt[_b_nVtx]=-1; //set the second track to null
      missingChildren.push(pair<int, int>(temp->get_parent_id(),temp->get_track_id())); //add the known ids to the list missing a child
      cout<<"No pair for:\n";
      temp->identify();
      cout<<"with parent:\n";
      i->second.getPhoton()->identify();
      }*/
    _b_pythia[_b_nVtx]=i->second.getEmbed()==3;
    _b_nVtx++; 
  }
  return missingChildren;
}

void TruthConversionEval::processBackground(std::map<int,Conversion> *mymap,SvtxTrackEval* trackeval){
  for (std::map<int,Conversion>::iterator i = mymap->begin(); i != mymap->end(); ++i) {
    int nReco=i->second.setRecoTracks(trackeval);
    if (nReco==2)
    {
      _bb_track_deta = i->second.trackDEta();
      _bb_track_dlayer = i->second.trackDLayer(_svtxClusterMap,_hitMap);
      _bb_track_layer = i->second.firstLayer(_svtxClusterMap);
      _bb_approach = i->second.approachDistance();
      /*The recoVtx finding doesn't work yet so using truth vtx for now
       * pair<SvtxTrack*,SvtxTrack*> recoTracks = i->second.getRecoTracks();
       pair<SvtxVertex*,SvtxVertex*> recoTracks = i->second.getRecoTracks();
       SvtxVertex* vtx = _vertexer->makeVtx(recoTracks.first,recoTracks.second);
       if(vtx) _b_vtx_radius =sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
       else _b_vtx_radius=-1;
       SvtxVertex *recoVtx = i->second.getRecoVtx();
       if (!recoVtx)
       {
       cout<<"reco vtx is null"<<endl;
       }*/
      PHG4VtxPoint *vtx = i->second.getVtx();
      _b_vtx_radius = sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
      //_b_vtx_chi2 = recoVtx->get_chisq();
      _b_vtxTrack_dist = i->second.dist(vtx,_svtxClusterMap);
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
      _backgroundCutTree->Fill();
    }
  }
}

void TruthConversionEval::findChildren(std::queue<std::pair<int,int>> missingChildren,PHG4TruthInfoContainer* _truthinfo){
  if (Verbosity()>=6)
  {
    while(!missingChildren.empty()){
      for (PHG4TruthInfoContainer::ConstIterator iter = _truthinfo->GetParticleRange().first; iter != _truthinfo->GetParticleRange().second; ++iter)
      {
        if(iter->second->get_parent_id()==missingChildren.front().first&&iter->second->get_track_id()!=missingChildren.front().second){
          cout<<"Found Child:\n";
          iter->second->identify();
          cout<<"With mother:\n";

        }
      }
      missingChildren.pop();
    }
  }
}


int TruthConversionEval::End(PHCompositeNode *topNode)
{
  _f->Write();
  _f->Close();
  return 0;
}



