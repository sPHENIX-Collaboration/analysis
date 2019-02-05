#include "ConvertedPhotonReconstructor.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <phool/PHDataNode.h>

#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <g4eval/SvtxEvalStack.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <iostream>
#include <cmath> 
#include <algorithm>
#include <sstream>

using namespace std;

ConvertedPhotonReconstructor::ConvertedPhotonReconstructor(const std::string &name) :
	SubsysReco("ConvertedPhotonReconstructor")
{
	this->name=name+"recovered.root";;
	event=0;
	_file = new TFile( this->name.c_str(), "RECREATE");
	_tree = new TTree("convertedphotontree","tracks reconstructed to converted photons");
	_tree->SetAutoSave(300);
	b_recovec1 = new TLorentzVector();
	b_recovec1 = new TLorentzVector();
	b_truthvec2 = new TLorentzVector();
	b_truthvec2 = new TLorentzVector();
	b_truthVertex = new TVector3();
	b_recoVertex = new  TVector3();
	_tree->Branch("charge",&b_goodCharge);
	_tree->Branch("silicone",&b_hasSilicone);
	_tree->Branch("hash",&hash);
	_tree->Branch("reco_tlv1",    "TLorentzVector",  &b_recovec1);
	_tree->Branch("reco_tlv2",    "TLorentzVector",  &b_recovec2);
	_tree->Branch("truth_tlv1",   "TLorentzVector", &b_truthvec1);
	_tree->Branch("truth_tlv2",   "TLorentzVector", &b_truthvec2);
	_tree->Branch("truth_vertex","TVector3",        &b_truthVertex);
	_tree->Branch("reco_vertex", "TVector3",        &b_recoVertex);
}

int ConvertedPhotonReconstructor::Init(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::InitRun(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int ConvertedPhotonReconstructor::process_event(PHCompositeNode *topNode) {
	/*if (((verbosity > 0)&&(event%100==0))) {  
		cout << "ConvertedPhotonReconstructor::process_event - Event = " << event << endl;
		}*/
	//  ReconstructedConvertedPhoton* recovered=reconstruct(topNode);
	/*if(recovered){
		cout<<"recovered"<<endl;
	//recoveredPhotonVec.push_back(recovered);
	cout<<"pushed"<<endl;
	}
	else{
	cout<<"no recovery"<<endl;
	}*/

	std::stringstream ss;
	ss<<"-"<<event;             //this is where the file number is 
	hash=name.substr(name.length()-24,5)+ss.str();
	reconstruct(topNode);
	event++;
	cout<<"return event::ok"<<endl;
	return Fun4AllReturnCodes::EVENT_OK;
}

ConvertedPhotonReconstructor::~ConvertedPhotonReconstructor(){

	_file->Write();
	_file->Close();
	delete _file;
}

int ConvertedPhotonReconstructor::End(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

void ConvertedPhotonReconstructor::process_recoTracks(PHCompositeNode *topNode){

}

ReconstructedConvertedPhoton* ConvertedPhotonReconstructor::reconstruct(PHCompositeNode *topNode){
	//let the stack get the info from the node
	SvtxEvalStack *stack = new SvtxEvalStack(topNode);
	if(!stack){
		cout<<"Evaluator is null quiting photon recovery\n";
		return nullptr;
	}
	stack->set_strict(false); //no idea what this does 
	//stack->set_verbosity(verbosity+1); //might be able to lower this 
	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
	SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	SvtxClusterMap* clustermap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap"); 
	SvtxVertexEval* vertexeval = stack->get_vertex_eval();
	SvtxTrackEval* trackeval =   stack->get_track_eval();
	//use the bools to check the event is good
	if(!vertexeval||!trackeval||!clustermap){
		cout<<"Evaluator is null quiting photon recovery\n";
		return nullptr;
	}
  if(vertexmap->size()!=1){
    cout<<"Vertex count != 1 not reconstructing event\n";
    return nullptr;
  }
	for (SvtxVertexMap::Iter iter = vertexmap->begin(); iter != vertexmap->end(); ++iter) {
		SvtxVertex* vertex = iter->second;
		//only take 2 track events
		if(vertex&&vertex->size_tracks()==2){
			SvtxVertex::TrackIter titer = vertex->begin_tracks(); 
			SvtxTrack* track1 = trackmap->get(*titer);
			PHG4Particle* truth1 = trackeval->max_truth_particle_by_nclusters(track1); 
			++titer;
			SvtxTrack* track2= trackmap->get(*titer);
			PHG4Particle* truth2 = trackeval->max_truth_particle_by_nclusters(track2);
      cout<<"Layers: maps:"<<n_intt_layer<<" intt:"<<n_maps_layer<<'\n';
			if (!b_hasSilicone&&clustermap->get(*track1->begin_clusters())->get_layer()<n_intt_layer+n_maps_layer){
				b_hasSilicone=true;
			}
			//both the truth particles must come from the same vertex 
			if (!truth1||!truth2||truth1->get_vtx_id()!=truth2->get_vtx_id())
			{
				cout<<"Skipping photon recovery tracks do not match \n";
				continue;
			}
			//record the vertex position
			float vx,vy,vz;
			vx = vertex->get_x();
			vy = vertex->get_y();
			vz = vertex->get_z();
			cout<<"Vertex:"<<vx<<", "<<vy<<", "<<vz<<'\n';
			//not sure what I want to do with these charges
			float charge1 = track1->get_charge();
			if(abs(charge1)!=1){
				cout<<"Quiting photon recovery due to charge="<<charge1<<'\n';
				continue; //only considering electron positron conversion 
			}
			//record the track momentum and charge
			float t1x,t1y,t1z,t2x,t2y,t2z,charge2;
			t1x = track1->get_px();
			t1y = track1->get_py();
			t1z = track1->get_pz();
			charge2 = track2->get_charge();
			if(charge1!= -1*charge2){
				b_goodCharge=false;
			}
			t2x = track2->get_px();
			t2y = track2->get_py();
			t2z = track2->get_pz();


			//convert to TObjects
			TVector3 Ttrack1(t1x,t1y,t1z), Ttrack2(t2x,t2y,t2z);
			PHG4VtxPoint* point = vertexeval->max_truth_point_by_ntracks(vertex); //not entirely sure what this does
			//double check these give the right values 
			b_recovec1= new TLorentzVector(Ttrack1,pToE(Ttrack1,kEmass));
			b_recovec2= new TLorentzVector(Ttrack2,pToE(Ttrack2,kEmass)); 
			b_recoVertex= new TVector3(vx,vy,vz);
			TVector3 tTrack1(truth1->get_px(),truth1->get_py(),truth1->get_pz()),
							 tTrack2(truth2->get_px(),truth2->get_py(),truth2->get_pz());
			b_truthVertex= new TVector3(point->get_x(),point->get_y(),point->get_z());
			b_truthvec1= new TLorentzVector(tTrack1,pToE(tTrack1,kEmass));
			b_truthvec2= new TLorentzVector( tTrack2,pToE(tTrack2,kEmass));
			_tree->Fill();
		}
	}
	delete stack;
	return nullptr;
}
