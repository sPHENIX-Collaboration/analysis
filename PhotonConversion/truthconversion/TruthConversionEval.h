
/**
* @TruthConversionEval.h
* @author Francesco Vassalli <Francesco.Vassalli@colorado.edu>
* @version 1.0
*
* @section Uses truth particle information to find photon conversions. 
* Infomation about the conversions is recored in a TTree.
* Finally they are associated with clusters for latter analysis
*/
#ifndef TRUTHCONVERSIONEVAL_H__
#define TRUTHCONVERSIONEVAL_H__

#include <fun4all/SubsysReco.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <calobase/RawClusterContainer.h>

#include <TTree.h>
#include <TFile.h>

#include "TLorentzVector.h"
#include "Conversion.h"

#include <vector>
#include <queue>
#include <set>


class PHCompositeNode;
class Conversion;
class SvtxTrackEval;

class TruthConversionEval: public SubsysReco
{

	public:
	/**
 *
 * @param name name of the output file
 * @param runnumber printed in TTree for condor jobs
 * @param particleEmbed the embedID of particles embeded by Fun4All
 * @param pythiaEmbed the embedID for an embeded pythia event
 * @param makeTTree true-writes the TTree false-won't make TTree will still find clusters
 */
		TruthConversionEval(const std::string &name,unsigned int runnumber, 
				int particleEmbed, int pythiaEmbed,bool makeTTree);
		~TruthConversionEval();
		int InitRun(PHCompositeNode*);
		/**
 		* Find the conversions pass them to numUnique.
 		* Fill the TTree
 		* @return event status */
		int process_event(PHCompositeNode*);
		int End(PHCompositeNode*);
		/** get the clusters associated with converions*/
		inline const RawClusterContainer* getClusters()const {return &_conversionClusters;} 

	private:
		inline float deltaR( float eta1, float eta2, float phi1, float phi2 ) {
			float deta = eta1 - eta2;
			float dphi = phi1 - phi2;
			if ( dphi > 3.14159 ) dphi -= 2 * 3.14159;
			if ( dphi < -3.14159 ) dphi += 2 * 3.14159;
			return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
		}
		/** helper function for process_event
		* fills the member fields with information from the conversions 
		* finds the clusters associated with the conversions
		* @return currently will return nothing 
		* but can easily be changed to return a structure for the converions with only 1 truth associated track*/
		std::queue<std::pair<int,int>> numUnique(std::map<int,Conversion>* map,SvtxTrackEval* trackEval,RawClusterContainer* mainClusterContainer);
		/** attempts to find other truth associated tracks for conversions with only 1 truth associated track*/
		void findChildren(std::queue<std::pair<int,int>> missing,PHG4TruthInfoContainer* truthinfo);

		const static int s_kMAXParticles=1000; //< increase this number if arrays go out of bounds
		const unsigned int _kRunNumber;
		const int _kParticleEmbed;
		const int _kPythiaEmbed;
		const bool _kMakeTTree;
		int _runNumber; ///<for the TTree do not change
		TFile *_f=NULL;
		TTree *_tree=NULL; ///< stores most of the data about the conversions
		TTree *_signalCutTree=NULL; //<signal data for making track pair cuts
		std::string _foutname;
		int _b_event;
		int _b_nVtx;  ///<total conversions
		int _b_Tpair; ///<count acceptance e pairs in truth
		int _b_Rpair; ///<count acceptance e pairs in reco
		float _b_rVtx[s_kMAXParticles];  ///<truth conversion radius
		bool _b_pythia[s_kMAXParticles];  ///<record if the conversion is from pythia or G4 particle
		float _b_electron_pt[s_kMAXParticles];
		float _b_positron_pt[s_kMAXParticles];
		float _b_parent_pt  [s_kMAXParticles];
		float _b_parent_eta [s_kMAXParticles];
		float _b_parent_phi [s_kMAXParticles];
    float _b_track_deta [s_kMAXParticles];
    bool _b_track_silicon [s_kMAXParticles];
    int _b_track_dlayer [s_kMAXParticles];
		/** RawClusters associated with truth conversions
		* processed by other modules*/
		RawClusterContainer _conversionClusters;
		
		const static int s_kTPCRADIUS=21; //in cm there is a way to get this from the simulation I should implement?
		///<TPC radius currently hardcoded
		float _kRAPIDITYACCEPT=1; //<acceptance rapidity currently hard coded to |1|
};

inline int get_embed(PHG4Particle* particle, PHG4TruthInfoContainer* truthinfo){
	return truthinfo->isEmbeded(particle->get_track_id());
}
inline float vtoR(PHG4VtxPoint* vtx){
	return (float) sqrt(vtx->get_x()*vtx->get_x()+vtx->get_y()*vtx->get_y());
}

#endif // __TRUTHCONVERSIONEVAL_H__



