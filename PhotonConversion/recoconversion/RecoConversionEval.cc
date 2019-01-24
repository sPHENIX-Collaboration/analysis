#include "RecoConversionEval.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxCluster.h>
#include <g4eval/SvtxEvalStack.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <TVector3.h>
#include <TTree.h>
#include <TFile.h>

#include <iostream>
#include <cmath> 
#include <algorithm>
#include <sstream>

using namespace std;

RecoConversionEval::RecoConversionEval(const std::string &name) :
	SubsysReco("RecoConversionEval"), _fname(name) 
{}

int RecoConversionEval::Init(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

int RecoConversionEval::InitRun(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

void RecoConversionEval::doNodePointers(PHCompositeNode *topNode){
	_allTracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
	_mainClusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
	_svtxClusterMap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap");
	_hitMap = findNode::getClass<SvtxHitMap>(topNode,"SvtxHitMap");
	_auxVertexer = new RaveVertexingAux(topNode);
}

bool RecoConversionEval::hasNodePointers()const{
	return _allTracks &&_mainClusterContainer && _svtxClusterMap&&_hitMap
		&&_auxVertexer&&!_auxVertexer->hasErrors();
}

int RecoConversionEval::process_event(PHCompositeNode *topNode) {
	doNodePointers(topNode);
	cout<<"Did pointers: \n \n";
	int bigLoopCount=0;
	/*the is not optimized but is just a nlogn process*/
	for ( SvtxTrackMap::Iter iter = _allTracks->begin(); iter != _allTracks->end(); ++bigLoopCount) {
		//I want to now only check e tracks so check the clusters of the |charge|=1 tracks
		if (abs(iter->second->get_charge())==1&&iter->second->get_pt()>_kTrackPtCut)
		{
			SvtxTrack* thisTrack = iter->second;
			RawCluster* bestCluster= _mainClusterContainer->getCluster(thisTrack->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
			//what if no cluster is found?
			if(bestCluster&&bestCluster->get_prob()>_kEMProbCut){
				//loop over the following tracks
				for (SvtxTrackMap::Iter jter = iter; jter != _allTracks->end(); ++jter)
				{
					//check that the next track is an opposite charge electron
					if (thisTrack->get_charge()*-1==jter->second->get_charge()&&jter->second->get_pt()>_kTrackPtCut)
					{
						RawCluster* nextCluster= _mainClusterContainer->getCluster(jter->second->get_cal_cluster_id(SvtxTrack::CAL_LAYER(1)));
						//what if no cluster is found?
						if(nextCluster&&nextCluster->get_prob()>_kEMProbCut&&pairCuts(thisTrack,jter->second)){
							SvtxVertex* vtxCan= _auxVertexer->makeVtx(thisTrack,jter->second);
							if (vtxCan&&vtxCuts(vtxCan))
							{
								/* code */
							}
						}
					}
				}
			}
		}
		++iter;
	}
	return Fun4AllReturnCodes::EVENT_OK;
}

bool RecoConversionEval::pairCuts(SvtxTrack* t1, SvtxTrack* t2)const{
	return detaCut(t1->get_eta(),t2->get_eta()) && hitCuts(t1,t2);
}

bool RecoConversionEval::hitCuts(SvtxTrack* t1, SvtxTrack* t2)const {
	SvtxCluster *c1 = _svtxClusterMap->get(*(t1->begin_clusters()));
	SvtxCluster *c2 = _svtxClusterMap->get(*(t2->begin_clusters()));
	SvtxHit *h1 = _hitMap->get(*(c1->begin_hits()));
	SvtxHit *h2 = _hitMap->get(*(c2->begin_hits()));
	//check that the first hits are close enough
	if (c1->get_layer()>_kNSiliconLayer&&c2->get_layer()>_kNSiliconLayer)
	{
		if (abs(h1->get_layer()-h2->get_layer())>_kFirstHitStrict)
		{
			return false;
		}
	}
	else{
		if (abs(h1->get_layer()-h2->get_layer())>_kFirstHit)
		{
			return false;
		}
	}
	return true;
}

bool RecoConversionEval::vtxCuts(SvtxVertex *vtx){
	return true;
}

int RecoConversionEval::End(PHCompositeNode *topNode) {
	return Fun4AllReturnCodes::EVENT_OK;
}

bool RecoConversionEval::approachDistance(SvtxTrack *t1,SvtxTrack* t2)const{
	static const double eps = 0.000001;
	TVector3 u(t1->get_px(),t1->get_py(),t1->get_pz());
	TVector3 v(t2->get_px(),t2->get_py(),t2->get_pz());
	TVector3 w(t1->get_x()-t2->get_x(),t1->get_x()-t2->get_y(),t1->get_x()-t2->get_z());

	double a = u.Dot(u);
	double b = u.Dot(v);
	double c = v.Dot(v);
	double d = u.Dot(w);
	double e = v.Dot(w);

	double D = a*c - b*b;
	double sc, tc;
	// compute the line parameters of the two closest points
	if (D < eps) {         // the lines are almost parallel
		sc = 0.0;
		tc = (b>c ? d/b : e/c);   // use the largest denominator
	}
	else {
		sc = (b*e - c*d) / D;
		tc = (a*e - b*d) / D;
	}
	// get the difference of the two closest points
	u*=sc;
	v*=tc;
	w+=u;
	w-=v;
	return w.Mag()<=_kApprochCut;   // return the closest distance 
}

void RecoConversionEval::process_recoTracks(PHCompositeNode *topNode){

}

