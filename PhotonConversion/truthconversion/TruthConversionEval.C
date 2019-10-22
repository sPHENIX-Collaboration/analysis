#include "TruthConversionEval.h"
#include "Conversion.h"
#include "SVReco.h"
#include "VtxRegressor.h"
//#include "../PhotonConversion/RaveVertexingAux.h"

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calotrigger/CaloTriggerInfo.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <g4main/PHG4TruthInfoContainer.h>	
#include <g4main/PHG4Particlev1.h>
#include <g4main/PHG4Particlev2.h>
#include <g4main/PHG4VtxPoint.h>

/*#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>*/
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrCluster.h>

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

#include <phgenfit/Track.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <TFile.h>
#include <TTree.h>

#include <math.h>
#include <utility>
#include <list>
#include <iostream>

TruthConversionEval::TruthConversionEval(const std::string &name, unsigned int runnumber, 
		int particleEmbed,  int pythiaEmbed,bool makeTTree=true,string TMVAName="",string TMVAPath="") : SubsysReco("TruthConversionEval"),
	_kRunNumber(runnumber),_kParticleEmbed(particleEmbed), _kPythiaEmbed(pythiaEmbed), _kMakeTTree(makeTTree)
{
	_foutname = name;
	_regressor = new VtxRegressor(TMVAName,TMVAPath);
}

TruthConversionEval::~TruthConversionEval(){
	cout<<"destruct"<<endl;
	if (_f) delete _f;
	if (_vertexer) delete _vertexer;
	if(_regressor) delete _regressor;
}

int TruthConversionEval::InitRun(PHCompositeNode *topNode)
{
	_vertexer = new SVReco();
	_vertexer->InitRun(topNode);
	if(_kMakeTTree){
		_runNumber=_kRunNumber;
		_f = new TFile( _foutname.c_str(), "RECREATE");
		_observTree = new TTree("observTree","per event observables");
		_observTree->SetAutoSave(300);
		_observTree->Branch("nMatched", &_b_nMatched);
		_observTree->Branch("nUnmatched", &_b_nUnmatched);
		_observTree->Branch("truth_pT", &_b_truth_pT);
		_observTree->Branch("reco_pT", &_b_reco_pT);
		_observTree->Branch("track_pT",&_b_alltrack_pT) ;
		_observTree->Branch("photon_pT",&_b_allphoton_pT) ;

		_vtxingTree = new TTree("vtxingTree","data predicting vtx from track pair");
		_vtxingTree->SetAutoSave(300);
		_vtxingTree->Branch("vtx_radius", &_b_vtx_radius);
		_vtxingTree->Branch("tvtx_radius", &_b_tvtx_radius);
		_vtxingTree->Branch("vtx_phi", &_b_vtx_phi);
		_vtxingTree->Branch("vtx_eta", &_b_vtx_eta);
		_vtxingTree->Branch("vtx_x", &_b_vtx_x);
		_vtxingTree->Branch("vtx_y", &_b_vtx_y);
		_vtxingTree->Branch("vtx_z", &_b_vtx_z);
		_vtxingTree->Branch("tvtx_eta", &_b_tvtx_eta);
		_vtxingTree->Branch("tvtx_x", &_b_tvtx_x);
		_vtxingTree->Branch("tvtx_y", &_b_tvtx_y);
		_vtxingTree->Branch("tvtx_z", &_b_tvtx_z);
		_vtxingTree->Branch("tvtx_phi", &_b_tvtx_phi);
		_vtxingTree->Branch("vtx_chi2", &_b_vtx_chi2);
		_vtxingTree->Branch("track1_pt", &_b_track1_pt);
		_vtxingTree->Branch("track1_eta",& _b_track1_eta);
		_vtxingTree->Branch("track1_phi",& _b_track1_phi);
		_vtxingTree->Branch("track2_pt", &_b_track2_pt);
		_vtxingTree->Branch("track2_eta",& _b_track2_eta);
		_vtxingTree->Branch("track2_phi",& _b_track2_phi);
		_vtxingTree->Branch("track_layer",& _b_track_layer);

		_trackBackTree = new TTree("trackBackTree","track background all single tracks");
		_trackBackTree->SetAutoSave(300);
		_trackBackTree->Branch("track_dca", &_bb_track_dca);
		_trackBackTree->Branch("track_pT",  &_bb_track_pT);
		_trackBackTree->Branch("track_layer", &_bb_track_layer);
		_trackBackTree->Branch("cluster_prob", &_bb_cluster_prob);

		_pairBackTree = new TTree("pairBackTree","pair background all possible combinations");
		_pairBackTree->SetAutoSave(300);
		_pairBackTree->Branch("track_deta", &_bb_track_deta);
		_pairBackTree->Branch("track2_pid", &_bb_track2_pid);
		_pairBackTree->Branch("track1_pid", &_bb_track1_pid);
		_pairBackTree->Branch("parent_pid", &_bb_parent_pid);
		_pairBackTree->Branch("track_dphi", &_bb_track_dphi);
		_pairBackTree->Branch("track_dlayer",&_bb_track_dlayer);
		_pairBackTree->Branch("approach_dist", &_bb_approach);
		_pairBackTree->Branch("track_dca", &_bb_track_dca);
		_pairBackTree->Branch("track_pT",  &_bb_track_pT);
		_pairBackTree->Branch("track_layer", &_bb_track_layer);
		_pairBackTree->Branch("cluster_prob", &_bb_cluster_prob);
		//_pairBackTree->Branch("nCluster", &_bb_nCluster);
		_pairBackTree->Branch("cluster_dphi", &_bb_cluster_dphi);
		_pairBackTree->Branch("cluster_deta", &_bb_cluster_deta);

		_vtxBackTree = new TTree("vtxBackTree","events that pass track pair cuts");
		_vtxBackTree->SetAutoSave(300);
		_vtxBackTree->Branch("track_deta", &_bb_track_deta);
		_vtxBackTree->Branch("track1_pid", &_bb_track1_pid);
		_vtxBackTree->Branch("track2_pid", &_bb_track2_pid);
		_vtxBackTree->Branch("track_dphi", &_bb_track_dphi);
		_vtxBackTree->Branch("track_dlayer",&_bb_track_dlayer);
		_vtxBackTree->Branch("approach_dist", &_bb_approach);
		_vtxBackTree->Branch("track_dca", &_bb_track_dca);
		_vtxBackTree->Branch("track_pT",  &_bb_track_pT);
		_vtxBackTree->Branch("track_layer", &_bb_track_layer);
		_vtxBackTree->Branch("cluster_prob", &_bb_cluster_prob);
		_vtxBackTree->Branch("vtx_radius",&_bb_vtx_radius);
		_vtxBackTree->Branch("photon_m",&_bb_photon_m);
		_vtxBackTree->Branch("photon_pT",&_bb_photon_pT);
		//_vtxBackTree->Branch("nCluster", &_bb_nCluster);
		_vtxBackTree->Branch("cluster_dphi", &_bb_cluster_dphi);
		_vtxBackTree->Branch("cluster_deta", &_bb_cluster_deta);

		_signalCutTree = new TTree("cutTreeSignal","signal data for making track pair cuts");
		_signalCutTree->SetAutoSave(100);
		_signalCutTree->Branch("track_deta", &_b_track_deta);
		_signalCutTree->Branch("track_dca", &_b_track_dca);
		_signalCutTree->Branch("track_dphi", &_b_track_dphi);
		_signalCutTree->Branch("track_dlayer",&_b_track_dlayer);
		_signalCutTree->Branch("track_layer", &_b_track_layer);
		_signalCutTree->Branch("track_pT", &_b_track_pT);
		//_signalCutTree->Branch("ttrack_pT", &_b_ttrack_pT);
		_signalCutTree->Branch("approach_dist", &_b_approach);
		_signalCutTree->Branch("vtx_radius", &_b_vtx_radius);
		_signalCutTree->Branch("vtx_phi", &_b_vtx_phi);
		_signalCutTree->Branch("tvtx_phi", &_b_tvtx_phi);
		_signalCutTree->Branch("tvtx_radius", &_b_tvtx_radius);
		_signalCutTree->Branch("vtx_chi2", &_b_vtx_chi2);
		//_signalCutTree->Branch("vtxTrackRZ_dist", &_b_vtxTrackRZ_dist);
		//_signalCutTree->Branch("vtxTrackRPhi_dist", &_b_vtxTrackRPhi_dist);
		_signalCutTree->Branch("photon_m", &_b_photon_m);
		_signalCutTree->Branch("tphoton_m", &_b_tphoton_m);
		_signalCutTree->Branch("photon_pT", &_b_photon_pT);
		_signalCutTree->Branch("tphoton_pT", &_b_tphoton_pT);
		_signalCutTree->Branch("cluster_prob", &_b_cluster_prob);
		//_signalCutTree->Branch("nCluster", &_b_nCluster);
		_signalCutTree->Branch("cluster_dphi", &_b_cluster_dphi);
		_signalCutTree->Branch("cluster_deta", &_b_cluster_deta);
	}
	return 0;
}

bool TruthConversionEval::doNodePointers(PHCompositeNode* topNode){
	bool goodPointers=true;
	_mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
	_truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
	_clusterMap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
	_allTracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	//  _hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");
	//if(!_mainClusterContainer||!_truthinfo||!_clusterMap||!_hitMap){
	if(!_mainClusterContainer||!_truthinfo||!_clusterMap||!_allTracks){
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
		if(!_allTracks){
			cerr<<"\t SvtxTrackMap is bad";
		}
		cerr<<endl;
		goodPointers=false;
	}
	return goodPointers;
}

SvtxVertex* TruthConversionEval::get_primary_vertex(PHCompositeNode *topNode)const{
	SvtxVertexMap *vertexMap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
	return vertexMap->get(0);
}

int TruthConversionEval::process_event(PHCompositeNode *topNode)
{
	if(!doNodePointers(topNode)) return Fun4AllReturnCodes::ABORTEVENT;
	cout<<"init vertexer event"<<endl;
	_vertexer->InitEvent(topNode);
	_conversionClusters.Reset(); //clear the list of conversion clusters
	PHG4TruthInfoContainer::Range range = _truthinfo->GetParticleRange(); //look at all truth particles
	SvtxEvalStack *stack = new SvtxEvalStack(topNode); //truth tracking info
	SvtxTrackEval* trackeval = stack->get_track_eval();
	if (!trackeval)
	{
		cerr<<"NULL track eval in " <<Name()<<" fatal error"<<endl;
		return 1;
	}
	//make a map of the conversions
	std::map<int,Conversion> mapConversions;
	//h is for hadronic e is for EM
	std::vector<SvtxTrack*> backgroundTracks;
	std::vector<std::pair<SvtxTrack*,SvtxTrack*>> tightbackgroundTrackPairs; //used to find the pair for unmatched conversion tracks
	std::vector<PHG4Particle*> truthPhotons;
	std::list<int> signalTracks;
	//reset obervation variables
	_b_nMatched=0;
	_b_nUnmatched=0;
	_b_truth_pT.clear();
	_b_reco_pT.clear();
	_b_alltrack_pT.clear();
	_b_allphoton_pT.clear();
	cout<<"init truth loop"<<endl;

	for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
		PHG4Particle* g4particle = iter->second;
		if(g4particle->get_pid()==22&&g4particle->get_track_id()==g4particle->get_primary_id()){
			_b_allphoton_pT.push_back(sqrt(g4particle->get_px()*g4particle->get_px()+g4particle->get_py()*g4particle->get_py()));
			truthPhotons.push_back(g4particle);
		}
		PHG4Particle* parent =_truthinfo->GetParticle(g4particle->get_parent_id());
		PHG4VtxPoint* vtx=_truthinfo->GetVtx(g4particle->get_vtx_id()); //get the vertex
		if(!vtx){
			cerr<<"null vtx primaryid="<<g4particle->get_primary_id()<<'\n';
			g4particle->identify();
			cout<<endl;
			continue;
		}
		float radius=sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
		//if outside the tracker skip this
		if(radius>s_kTPCRADIUS) continue;
		int embedID;
		if (parent)//if the particle is not primary
		{
			embedID=get_embed(parent,_truthinfo);
			float truthpT = sqrt(g4particle->get_px()*g4particle->get_px()+g4particle->get_py()*g4particle->get_py());
			if(parent->get_pid()==22&&TMath::Abs(g4particle->get_pid())==11&&truthpT>2.5){ //conversion check
				if (Verbosity()==10)
				{
					std::cout<<"Conversion with radius [cm]:"<<radius<<'\n';
				}
				//initialize the conversion object -don't use constructor b/c setters have error handling
				(mapConversions[vtx->get_id()]).setElectron(g4particle);
				(mapConversions[vtx->get_id()]).setVtx(vtx);
				(mapConversions[vtx->get_id()]).setPrimaryPhoton(parent,_truthinfo);
				(mapConversions[vtx->get_id()]).setEmbed(embedID);
				PHG4Particle* grand =_truthinfo->GetParticle(parent->get_parent_id()); //grandparent
				if (grand) (mapConversions[vtx->get_id()]).setSourceId(grand->get_pid());//record pid of the photon's source
				else (mapConversions[vtx->get_id()]).setSourceId(0);//or it is from the G4 generator
				_b_truth_pT.push_back(truthpT);
				//build a list of the ids
				SvtxTrack* recoTrack = trackeval->best_track_from(g4particle);
				if(recoTrack){
					signalTracks.push_back(recoTrack->get_id());
					_b_reco_pT.push_back(recoTrack->get_pt());
					cout<<"matched truth track"<<endl;
					_b_nMatched++;
				}
				else{
					_b_nUnmatched++; 
					cerr<<"WARNING no matching track for conversion"<<endl;
				}
			}
		}// not primary 
	}//truth particle loop
	signalTracks.sort();
	cout<<"intit track loop"<<endl;
	//build the current backgroundSample
	for ( SvtxTrackMap::Iter iter = _allTracks->begin(); iter != _allTracks->end(); ++iter) {
		_b_alltrack_pT.push_back(iter->second->get_pt());
		auto inCheck = std::find(signalTracks.begin(),signalTracks.end(),iter->first);
		//if the track is not in the list of signal tracks
		if (inCheck==signalTracks.end())
		{
			TLorentzVector track_tlv = tracktoTLV(iter->second);
			bool addTrack=true;
			//make sure the track is not too close to a photon
			for (std::vector<PHG4Particle*>::iterator iPhoton = truthPhotons.begin(); iPhoton != truthPhotons.end()&&addTrack; ++iPhoton)
			{
				TLorentzVector photon_tlv = particletoTLV(*iPhoton);
				if (photon_tlv.DeltaR(track_tlv)<.2)
				{
					addTrack=false;
				}
			}
			if(addTrack){
				backgroundTracks.push_back(iter->second);
				//see if it is a good tight background cannidate
				for ( SvtxTrackMap::Iter jter = std::next(iter,1); jter != _allTracks->end()&&iter->second->get_pt()>_kTightPtMin; ++jter){
					if(jter->second->get_pt()>_kTightPtMin&&TMath::Abs(jter->second->get_eta()-iter->second->get_eta())<_kTightDetaMax&&
							iter->second->get_charge()==jter->second->get_charge()*-1){
						tightbackgroundTrackPairs.push_back(std::pair<SvtxTrack*,SvtxTrack*>(iter->second,jter->second));
					}
				} 

			}
		}
	}
	//pass the map to this helper method which fills the fields for the signal ttree 
	numUnique(&mapConversions,trackeval,_mainClusterContainer,&tightbackgroundTrackPairs);
	
	if (_kMakeTTree)
	{
		cout<<"intit background process"<<endl;
		if(_b_truth_pT.size()!=0) _observTree->Fill();
		processTrackBackground(&backgroundTracks,trackeval);
		cout<<"finish back"<<endl;
	}
	delete stack;
	cout<<"finish process"<<endl;
	return 0;
}

void TruthConversionEval::numUnique(std::map<int,Conversion> *mymap=NULL,SvtxTrackEval* trackeval=NULL,RawClusterContainer *mainClusterContainer=NULL,
		std::vector<std::pair<SvtxTrack*,SvtxTrack*>>* backgroundTracks=NULL){
	cout<<"start conversion analysis loop"<<endl;
	for (std::map<int,Conversion>::iterator i = mymap->begin(); i != mymap->end(); ++i) {
		TLorentzVector tlv_photon,tlv_electron,tlv_positron; //make tlv for each particle 
		PHG4Particle *photon = i->second.getPrimaryPhoton(); //set the photon

		if(photon)tlv_photon.SetPxPyPzE(photon->get_px(),photon->get_py(),photon->get_pz(),photon->get_e()); //intialize
		else cerr<<"No truth photon for conversion"<<endl;
		PHG4Particle *e1=i->second.getElectron(); //set the first child 
		if(e1){
			tlv_electron.SetPxPyPzE(e1->get_px(),e1->get_py(),e1->get_pz(),e1->get_e());
		}
		else cerr<<"No truth electron for conversion"<<endl;
		PHG4Particle *e2=i->second.getPositron();
		if(e2){ //this will be false for conversions with 1 truth track
			tlv_positron.SetPxPyPzE(e2->get_px(),e2->get_py(),e2->get_pz(),e2->get_e()); //init the tlv
			if (TMath::Abs(tlv_electron.Eta())<_kRAPIDITYACCEPT&&TMath::Abs(tlv_positron.Eta())<_kRAPIDITYACCEPT)
			{
				unsigned int nRecoTracks = i->second.setRecoTracks(trackeval); //find the reco tracks for this conversion
				switch(nRecoTracks)
				{
					case 2: //there are 2 reco tracks
						{
							recordConversion(&(i->second),&tlv_photon,&tlv_electron,&tlv_positron);
							break;
						}
					case 1: //there's one reco track try to find the other
						{
							PHG4Particle *truthe;
							//get the truth particle that is missing a reco track
							if(!i->second.getRecoTrack(e1->get_track_id()))truthe = e1;
							else truthe=e2;
							if(!truthe) cerr<<"critical error"<<endl;
							//look through the background for the missing pair
							for(auto pair : *backgroundTracks){
								if(!pair.first||!pair.second) cerr<<"critical error2"<<endl;
								if ((pair.first->get_charge()>0&&truthe->get_pid()<0)||(pair.first->get_charge()<0&&truthe->get_pid()>0))
								{
									TVector3 truth_tlv(truthe->get_px(),truthe->get_py(),truthe->get_pz());
									TVector3 reco_tlv(pair.first->get_px(),pair.first->get_py(),pair.first->get_pz());
									if (reco_tlv.DeltaR(truth_tlv)<.1)
									{
										i->second.setRecoTrack(truthe->get_track_id(),pair.first);
									}
								}
								else if ((pair.second->get_charge()>0&&truthe->get_pid()<0)||(pair.second->get_charge()<0&&truthe->get_pid()>0))
								{
									TVector3 truth_tlv(truthe->get_px(),truthe->get_py(),truthe->get_pz());
									TVector3 reco_tlv(pair.second->get_px(),pair.second->get_py(),pair.second->get_pz());
									if (reco_tlv.DeltaR(truth_tlv)<.1)
									{
										i->second.setRecoTrack(truthe->get_track_id(),pair.second);
									}
								}
							}
							recordConversion(&(i->second),&tlv_photon,&tlv_electron,&tlv_positron);
							break;
						}
					case 0: //no reco tracks
						//TODO maybe try to find the reco tracks? for now just record the bad info
						recordConversion(&(i->second),&tlv_photon,&tlv_electron,&tlv_positron);
						break;
					default:
						if (Verbosity()>1)
						{
							cerr<<Name()<<" error setting reco tracks"<<endl;
						}
						break;
				}//switch
			}//rapidity cut
		}// has 2 truth tracks
		cout<<"map loop"<<endl;
	}//map loop
	cout<<"done num"<<endl;
}

//only call if _kMakeTTree is true
void TruthConversionEval::processTrackBackground(std::vector<SvtxTrack*> *v_tracks,SvtxTrackEval* trackeval){
	Conversion pairMath;
	float lastpT=-1.;
	unsigned nNullTrack=0;
	cout<<"The total possible background track count is "<<v_tracks->size()<<'\n';
	for (std::vector<SvtxTrack*>::iterator iter = v_tracks->begin(); iter != v_tracks->end(); ++iter) {
		cout<<"looping"<<endl;
		SvtxTrack* iTrack = *iter;
		//get the SvtxTrack it must not be NULL
		if(!iTrack){
			nNullTrack++;
			continue;
		}

		if(TMath::Abs(iTrack->get_eta())>1.1||iTrack->get_pt()==lastpT)continue; //TODO this skips duplicate tracks but why are they there in the first place?
		lastpT=iTrack->get_pt();
		cout<<"\t pT="<<lastpT<<'\n';

		auto temp_key_it=iTrack->begin_cluster_keys();//key iterator to first cluster
		if(temp_key_it!=iTrack->end_cluster_keys()){//if the track has clusters
			TrkrCluster* temp_cluster = _clusterMap->findCluster(*temp_key_it);//get the cluster 
			if(temp_cluster) _bb_track_layer = TrkrDefs::getLayer(temp_cluster->getClusKey());//if there is a cluster record its layer
			else _bb_track_layer=-999;
		}
		//record track info
		_bb_track_dca = iTrack->get_dca();
		_bb_track_pT = iTrack->get_pt();
		//record the EMCal cluster info
		auto cluster1 = _mainClusterContainer->getCluster(iTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
		if(cluster1) _bb_cluster_prob= cluster1->get_prob();
		else _bb_cluster_prob=-999;
		//pair with other tracks
		for(std::vector<SvtxTrack*>::iterator jter =std::next(iter,1);jter!=v_tracks->end(); ++jter){//posible bias by filling the track level variables with iTrack instead of min(iTrack,jTrack)
			SvtxTrack* jTrack = *jter;
			if(!jTrack||TMath::Abs(jTrack->get_eta())>1.1)continue;
			//record pair geometry
			cout<<"calculations"<<endl;
			_bb_track_deta = pairMath.trackDEta(iTrack,jTrack);
			_bb_track_dphi = pairMath.trackDPhi(iTrack,jTrack);
			_bb_track_dlayer = pairMath.trackDLayer(_clusterMap,iTrack,jTrack);
			_bb_approach = pairMath.approachDistance(iTrack,jTrack);
			//record second EMCal cluster
			auto cluster2 = _mainClusterContainer->getCluster(iTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
			//if the EMCal clusters can be found record their pair geometry 
			if (cluster2&&cluster1)
			{
				//if the two clusters are unique calculate the values 
				if(cluster2->get_id()!=cluster1->get_id())
				{
					_bb_nCluster = 2;
					_bb_cluster_dphi=fabs(cluster1->get_phi()-cluster2->get_phi());
					TVector3 etaCalc(cluster1->get_x(),cluster1->get_y(),cluster1->get_z());
					float eta1 = etaCalc.PseudoRapidity();
					etaCalc.SetXYZ(cluster2->get_x(),cluster2->get_y(),cluster2->get_z());
					_bb_cluster_deta=fabs(eta1-etaCalc.PseudoRapidity());
				}
				else{ //if they are not unique then they are 0
					_bb_nCluster = 1;
					_bb_cluster_dphi=0;
					_bb_cluster_deta=0;
				} 
			}
			else{ //clusters were not found
				_bb_nCluster=0;
				_bb_cluster_deta=-999;
				_bb_cluster_dphi=-999;
			}
			/*_bb_track1_pid = (*iTruthTrack)->get_pid();
				_bb_track2_pid = (*jTruthTrack)->get_pid();
				PHG4Particle* parent  = _truthinfo->GetParticle((*iTruthTrack)->get_parent_id());
				if(parent) _bb_parent_pid = parent->get_pid();
				else _bb_parent_pid=0;*/
			//if the track pairs pass the pair cuts make them part of the vertex background
			if (_bb_track_layer>=0&&_bb_track_pT>_kTightPtMin&&_bb_track_deta<_kTightDetaMax&&TMath::Abs(_bb_track_dlayer)<9)
			{
				//iTrack->identify();
				//jTrack->identify();
				cout<<"here vertex"<<endl;
				//make the vertex
				genfit::GFRaveVertex* recoVert = _vertexer->findSecondaryVertex(iTrack,jTrack);
				cout<<"made vertex"<<endl;
				//if the vertex is made record its info
				if (recoVert)
				{
					recoVert=pairMath.correctSecondaryVertex(_regressor,recoVert,iTrack,jTrack);
					cout<<"corrected vertex"<<endl;
					TVector3 recoVertPos = recoVert->getPos();
					cout<<"deleting"<<endl;
					delete recoVert;
					cout<<"deleted"<<endl;
					//fill the tree values 
					_bb_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
					_bb_vtx_chi2 = recoVert->getChi2();
					_bb_vtxTrackRZ_dist = pairMath.vtxTrackRZ(recoVertPos,iTrack,jTrack);
					_bb_vtxTrackRPhi_dist = pairMath.vtxTrackRPhi(recoVertPos,iTrack,jTrack);
					//then make the photon
					TLorentzVector* recoPhoton= pairMath.getRecoPhoton(iTrack,jTrack);
					if(recoPhoton){
						_bb_photon_m = recoPhoton->Dot(*recoPhoton);
						_bb_photon_pT = recoPhoton->Pt();
						delete recoPhoton;
					}
					else{
						_bb_photon_m=-999;
						_bb_photon_pT=-999;
					}
				}
				else{ // no vtx could be found for thre track pair
					_bb_vtx_radius = -999;
					_bb_vtx_chi2 = -999;
					_bb_vtxTrackRZ_dist =-999;
					_bb_vtxTrackRPhi_dist =-999;
				}
				cout<<"fill vtx"<<endl;
				_vtxBackTree->Fill();
			}//pair cuts
			_pairBackTree->Fill();
			cout<<"filled pair"<<endl;
		}//jTrack loop
		cout<<"filling track"<<endl;
		_trackBackTree->Fill();
		cout<<"filled track"<<endl;
	}//iTrack loop
	cout<<"Null track count ="<<nNullTrack<<endl;
}

void TruthConversionEval::recordConversion(Conversion *conversion,TLorentzVector *tlv_photon,TLorentzVector *tlv_electron, TLorentzVector *tlv_positron){
	cout<<"recording"<<endl;
	if(tlv_photon&&tlv_electron&&tlv_positron&&conversion&&conversion->recoCount()==2){
		_b_track_deta = conversion->trackDEta();
		_b_track_dphi = conversion->trackDPhi();
		_b_track_dlayer = conversion->trackDLayer(_clusterMap);
		_b_track_layer = conversion->firstLayer(_clusterMap);
		_b_approach = conversion->approachDistance();
		_b_track_dca = conversion->minDca();
		//record pT info
		_b_track_pT = conversion->minTrackpT();
		if(tlv_electron->Pt()>tlv_positron->Pt()) _b_ttrack_pT = tlv_positron->Pt();
		else _b_ttrack_pT = tlv_electron->Pt();
		//record initial photon info
		TLorentzVector* recoPhoton = conversion->getRecoPhoton();
		if (recoPhoton)
		{
			_b_photon_m=recoPhoton->Dot(*recoPhoton);
			TLorentzVector truth_added_tlv = *tlv_electron+*tlv_positron;
			_b_tphoton_m= truth_added_tlv.Dot(truth_added_tlv);
			_b_photon_pT=recoPhoton->Pt();
			conversion->PrintPhotonRecoInfo(tlv_photon,tlv_electron,tlv_positron,_b_photon_m);
		}
		else{//photon was not reconstructed
			_b_photon_m =-999;
			_b_photon_pT=-999;
			conversion->PrintPhotonRecoInfo(tlv_photon,tlv_electron,tlv_positron,_b_photon_m);
		}
		_b_tphoton_pT=tlv_photon->Pt();
		cout<<"second"<<endl;
		//truth vertex info
		_b_tvtx_radius = sqrt(conversion->getVtx()->get_x()*conversion->getVtx()->get_x()+conversion->getVtx()->get_y()*conversion->getVtx()->get_y());
		TVector3 tVertPos(conversion->getVtx()->get_x(),conversion->getVtx()->get_y(),conversion->getVtx()->get_z());
		_b_tvtx_phi = tVertPos.Phi();
		_b_tvtx_eta = tVertPos.Eta();
		_b_tvtx_z = tVertPos.Z();
		_b_tvtx_x = tVertPos.X();
		_b_tvtx_y = tVertPos.Y();
		//TODO check Conversion operations for ownership transfer->memleak due to lack of delete
		cout<<"vertexing"<<endl;
		genfit::GFRaveVertex* originalVert, *recoVert;
		originalVert=recoVert = conversion->getSecondaryVertex(_vertexer);
		recoVert = conversion->correctSecondaryVertex(_regressor);
		cout<<"finding gf_tracks"<<endl;
		//std::pair<PHGenFit::Track*,PHGenFit::Track*> ph_gf_tracks = conversion->getPHGFTracks(_vertexer);
		if (recoVert)
		{
			//cout<<"finding refit_gf_tracks"<<endl;
			//std::pair<PHGenFit::Track*,PHGenFit::Track*> refit_phgf_tracks=conversion->refitTracks(_vertexer);
			//TODO check repetive refitting and revterexing this is issue #23
			/*if (ph_gf_tracks.first&&refit_phgf_tracks.first)
				{
				cout<<"Good Track refit with original:\n";ph_gf_tracks.first->get_mom().Print();cout<<"\n\t and refit:\n";
				refit_phgf_tracks.first->get_mom().Print();
				}
				if (ph_gf_tracks.second&&refit_phgf_tracks.second)
				{
				cout<<"Good Track refit with original:\n"; 
				ph_gf_tracks.second->get_mom().Print(); 
				cout<<"\n\t and refit:\n";
				refit_phgf_tracks.second->get_mom().Print();
				}*/
			recoPhoton = conversion->getRefitRecoPhoton();
			if(recoPhoton) _b_photon_pT=recoPhoton->Pt();
			//fill the vtxing tree
			TVector3 recoVertPos = originalVert->getPos();
			_b_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
			_b_vtx_phi = recoVertPos.Phi();
			_b_vtx_eta = recoVertPos.Eta();
			_b_vtx_z = recoVertPos.Z();
			_b_vtx_x = recoVertPos.X();
			_b_vtx_y = recoVertPos.Y();
			_b_vtx_chi2 = recoVert->getChi2();
			//track info
			pair<float,float> pTstemp = conversion->getTrackpTs();
			_b_track1_pt = pTstemp.first;
			_b_track2_pt = pTstemp.second;
			pair<float,float> etasTemp = conversion->getTrackEtas();
			_b_track1_eta = etasTemp.first;
			_b_track2_eta = etasTemp.second;
			pair<float,float> phisTemp = conversion->getTrackPhis();
			_b_track1_phi = phisTemp.first;
			_b_track2_phi = phisTemp.second;
			_vtxingTree->Fill();

			//populate the refit vertex values
			recoVertPos = recoVert->getPos();
			_b_vtx_radius = sqrt(recoVertPos.x()*recoVertPos.x()+recoVertPos.y()*recoVertPos.y());
			_b_vtx_phi = recoVertPos.Phi();
			_b_vtx_eta = recoVertPos.Eta();
			_b_vtx_z = recoVertPos.Z();
			_b_vtx_x = recoVertPos.X();
			_b_vtx_y = recoVertPos.Y();
			_b_vtxTrackRZ_dist = conversion->vtxTrackRZ(recoVertPos);
			_b_vtxTrackRPhi_dist = conversion->vtxTrackRPhi(recoVertPos);
			cout<<"done full vtx values"<<endl;
		}

		else{//vtx not reconstructed
			cout<<"no vtx "<<endl;
			_b_vtx_radius  =-999.;
			_b_vtx_phi = -999.;
			_b_vtx_eta = -999.;
			_b_vtx_z = -999.;
			_b_vtx_x = -999.;
			_b_vtx_y = -999.;
			_b_vtx_chi2 = -999.;
			_b_vtxTrackRZ_dist = -999.;
			_b_vtxTrackRPhi_dist = -999.;
			//track info
			pair<float,float> pTstemp = conversion->getTrackpTs();
			_b_track1_pt = pTstemp.first;
			_b_track2_pt = pTstemp.second;
			pair<float,float> etasTemp = conversion->getTrackEtas();
			_b_track1_eta = etasTemp.first;
			_b_track2_eta = etasTemp.second;
			pair<float,float> phisTemp = conversion->getTrackPhis();
			_b_track1_phi = phisTemp.first;
			_b_track2_phi = phisTemp.second;
			cout<<"done no vtx values"<<endl;
			_vtxingTree->Fill();
		}
		//reset the values
		_b_cluster_prob=-999;
		_b_cluster_deta=-999;
		_b_cluster_dphi=-999;
		_b_nCluster=-999;
		pair<int,int> clusterIds = conversion->get_cluster_ids();
		RawCluster *clustemp;
		if(_mainClusterContainer->getCluster(clusterIds.first)){//if there is matching cluster 
			clustemp =   dynamic_cast<RawCluster*>(_mainClusterContainer->getCluster(clusterIds.first)->CloneMe());
			//this is for cluster subtraction which will not be implented soon
			// _conversionClusters.AddCluster(clustemp); //add the calo cluster to the container
			if (_kMakeTTree)
			{
				_b_cluster_prob=clustemp->get_prob();
				RawCluster *clus2 = _mainClusterContainer->getCluster(clusterIds.second);
				if (clus2)
				{
					_b_cluster_dphi=fabs(clustemp->get_phi()-clus2->get_phi());
					TVector3 etaCalc(clustemp->get_x(),clustemp->get_y(),clustemp->get_z());
					//TODO check cluster_prob distribution for signal
					if (clus2->get_prob()>_b_cluster_prob)
					{
						_b_cluster_prob=clus2->get_prob();
					}
					//calculate deta
					float eta1 = etaCalc.PseudoRapidity();
					etaCalc.SetXYZ(clus2->get_x(),clus2->get_y(),clus2->get_z());
					_b_cluster_deta=fabs(eta1-etaCalc.PseudoRapidity());
					if (clusterIds.first!=clusterIds.second) //if there are two district clusters
					{
						_b_nCluster=2;
					}
					else{
						_b_nCluster=1;
					}
				}
			}
		}
	}

	else{//not a "complete" conversion some of the data is missing 
		cout<<"incomplete conversion"<<endl;
		_b_track_deta   = -999;
		_b_track_dphi   = -999;
		_b_track_dlayer = -999;
		_b_track_layer  = -999;
		_b_approach     = -999;
		_b_track_dca    = -999;
		//record pT info
		_b_track_pT = -999;
		if(!tlv_electron||!tlv_positron) _b_ttrack_pT = -999;
		else if(tlv_electron->Pt()>tlv_positron->Pt()) _b_ttrack_pT = tlv_positron->Pt();
		else _b_ttrack_pT = tlv_electron->Pt();
		//record initial photon info
		TLorentzVector* recoPhoton = conversion->getRecoPhoton();
		if (recoPhoton)
		{
			_b_photon_pT=recoPhoton->Pt();
			_b_photon_m=recoPhoton->Dot(*recoPhoton);
			if (tlv_positron&&tlv_electron)
			{
				TLorentzVector truth_added_tlv = *tlv_electron+*tlv_positron;
				_b_tphoton_m= truth_added_tlv.Dot(truth_added_tlv);
			}
			else{
				_b_tphoton_m=-999;
			}
		}
		else{//photon was not reconstructed
			_b_photon_m =-999;
			_b_photon_pT=-999;
		}
		if(tlv_photon)_b_tphoton_pT=tlv_photon->Pt();
		else _b_tphoton_pT=-999;
		cout<<"here"<<endl;
		//truth vertex info
		_b_tvtx_radius = sqrt(conversion->getVtx()->get_x()*conversion->getVtx()->get_x()+conversion->getVtx()->get_y()*conversion->getVtx()->get_y());
		TVector3 tVertPos(conversion->getVtx()->get_x(),conversion->getVtx()->get_y(),conversion->getVtx()->get_z());
		cout<<"still here"<<endl;
		_b_tvtx_phi = tVertPos.Phi();
		_b_tvtx_eta = tVertPos.Eta();
		_b_tvtx_z = tVertPos.Z();
		_b_tvtx_x = tVertPos.X();
		_b_tvtx_y = tVertPos.Y();
		//TODO check Conversion operations for ownership transfer->memleak due to lack of delete
		//vtx not reconstructed because conversion is incomplete
		_b_vtx_radius = -999.;
		_b_vtx_phi = -999.;
		_b_vtx_eta = -999.;
		_b_vtx_z = -999.;
		_b_vtx_x = -999.;
		_b_vtx_y = -999.;
		_b_vtx_chi2 = -999.;
		_b_vtxTrackRZ_dist = -999.;
		_b_vtxTrackRPhi_dist = -999.;
		//track info not possible to reconstruct both tracks 
		_b_track1_pt  = -999;
		_b_track2_pt  = -999;
		_b_track1_eta = -999;
		_b_track2_eta = -999;
		_b_track1_phi = -999;
		_b_track2_phi = -999;

		//reset the values
		_b_cluster_prob=-1;
		_b_cluster_deta=-1;
		_b_cluster_dphi=-1;
		_b_nCluster=-1;
		pair<int,int> clusterIds = conversion->get_cluster_ids();
		RawCluster *clustemp;
		if(_mainClusterContainer->getCluster(clusterIds.first)){//if there is matching cluster 
			clustemp =   dynamic_cast<RawCluster*>(_mainClusterContainer->getCluster(clusterIds.first)->CloneMe());
			//this is for cluster subtraction which will not be implented soon

			// _conversionClusters.AddCluster(clustemp); //add the calo cluster to the container
			if (_kMakeTTree)
			{
				_b_cluster_prob=clustemp->get_prob();
				RawCluster *clus2 = _mainClusterContainer->getCluster(clusterIds.second);
				if (clus2)
				{
					_b_cluster_dphi=fabs(clustemp->get_phi()-clus2->get_phi());
					TVector3 etaCalc(clustemp->get_x(),clustemp->get_y(),clustemp->get_z());
					//TODO check cluster_prob distribution for signal
					if (clus2->get_prob()>_b_cluster_prob)
					{
						_b_cluster_prob=clus2->get_prob();
					}
					//calculate deta
					float eta1 = etaCalc.PseudoRapidity();
					etaCalc.SetXYZ(clus2->get_x(),clus2->get_y(),clus2->get_z());
					_b_cluster_deta=fabs(eta1-etaCalc.PseudoRapidity());
					if (clusterIds.first!=clusterIds.second) //if there are two district clusters
					{
						_b_nCluster=2;
					}
					else{
						_b_nCluster=1;
					}
				}
			}
		}
	}
	cout<<"Filling"<<endl;
	_signalCutTree->Fill();  
}

const RawClusterContainer* TruthConversionEval::getClusters()const {return &_conversionClusters;} 

int TruthConversionEval::get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo)const{
	return truthinfo->isEmbeded(particle->get_track_id());
}

float TruthConversionEval::vtoR(PHG4VtxPoint* vtx)const{
	return (float) sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
}

int TruthConversionEval::End(PHCompositeNode *topNode)
{
	cout<<"ending"<<endl;
	if(_kMakeTTree){
		cout<<"closing"<<endl;
		//_signalCutTree->Write();
		_f->Write();
		_f->Close();
	}
	return 0;
}
