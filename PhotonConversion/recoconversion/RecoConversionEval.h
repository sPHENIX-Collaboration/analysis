#ifndef RecoConversionEval_H__
#define RecoConversionEval_H__

//===============================================
/// \file RecoConversionEval.h
/// \brief Use reco info to tag photon conversions
/// \author Francesco Vassalli
//===============================================
#include "RaveVertexingAux.h"
#include <fun4all/SubsysReco.h>
#include <string>
#include <cmath>
class PHCompositeNode;
class SvtxEvalStack;
class SvtxVertex;
class SvtxTrack;
class SvtxTrackMap;
class SvtxHitMap;
class SvtxClusterMap;
class RawClusterContainer;
class TTree;
class TFile;

class RecoConversionEval : public SubsysReco {

	public:

		RecoConversionEval(const std::string &name);
		~RecoConversionEval(){
			if (_auxVertexer) delete _auxVertexer;
		}	
		int Init(PHCompositeNode *topNode);
		int InitRun(PHCompositeNode *topNode);
		int process_event(PHCompositeNode *topNode);
		int End(PHCompositeNode *topNode);

	private:
		SvtxTrackMap* _allTracks;
		RawClusterContainer* _mainClusterContainer;
		SvtxClusterMap* _svtxClusterMap;
		SvtxHitMap *_hitMap;
		RaveVertexingAux *_auxVertexer;
		std::string _fname;
		TFile *_file;
		TTree *_tree;

		void doNodePointers(PHCompositeNode *topNode);
		bool hasNodePointers()const;
		void process_recoTracks(PHCompositeNode *topNode);

		inline bool detaCut(float eta1, float eta2) const{
			return eta1>eta2?eta1-eta2:eta2-eta1<_kPolarCut;
		}

		bool pairCuts(SvtxTrack* t1, SvtxTrack* t2)const;
		/* Check that the radial distance between the first hit of both tracks is less the cut 
		 * cut should be stricter for pairs with no silicone hits
		 * also need to check the approach distance
		 */
		bool hitCuts(SvtxTrack* t1, SvtxTrack* t2)const ;
		bool vtxCuts(SvtxVertex* vtx);
		/* cut on the distance between the closest point between the two tracks*/
		bool approachDistance(SvtxTrack *t1,SvtxTrack* t2)const;
		// I want these to be static constexpr
		unsigned int _kNSiliconLayer=7;
		float _kEMProbCut=.5;
		float _kPolarCut=.1;
		float _kFirstHit=3;
		float _kFirstHitStrict=1;
		float _kTrackPtCut=.4; //MeV
		double _kApprochCut=.2;


};

#endif // __RecoConversionEval_H__
