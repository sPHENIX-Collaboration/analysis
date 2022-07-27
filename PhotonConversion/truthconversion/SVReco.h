#ifndef __SVReco_H__
#define __SVReco_H__

#include <fun4all/SubsysReco.h>
#include <GenFit/GFRaveVertex.h>
#include <GenFit/Track.h>
#include <string>
#include <vector>
#include <map>

namespace genfit {
	class GFRaveVertexFactory;
} /* namespace genfit */

namespace PHGenFit {
	class Track;
	class Fitter;
} /* namespace PHGenFit */

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;
class SvtxVertex;
class PHCompositeNode;
class PHG4TruthInfoContainer;
class PHG4CylinderGeomContainer;
//class SvtxClusterMap;
class TrkrClusterContainer;
class SvtxEvalStack;
class JetMap;
class TFile;
class TTree;

//! \brief		Find secondary vertex from track pairs and refit SvtxTracks with PHGenFit.
class SVReco {
	public:

		//! default ctor with hardcoded settings
		SVReco(const std::string &name = "SVReco");

		//! dtor
		~SVReco();

		//!Initialization, called for initialization at each event
		int InitEvent(PHCompositeNode *);

		//!Initialization Run, called for initialization of a run
		int InitRun(PHCompositeNode *);

		//!@return a vertex for each track pair
		genfit::GFRaveVertex* findSecondaryVertex(SvtxTrack* track1, SvtxTrack* track2);
		///@return a new SvtxVertex from the rave_vtx ownership is returned
		SvtxVertex* GFRVVtxToSvtxVertex(genfit::GFRaveVertex* rave_vtx)const;

		//Uses \param vtx to refit \param svtxtrk. @return the PHGenFit version of the track.
		PHGenFit::Track* refitTrack(SvtxVertex* vtx, SvtxTrack* svtxtrk);
		//Uses \param vtx to refit \param svtxtrk. @return the PHGenFit version of the track.
    PHGenFit::Track* refitTrack(genfit::GFRaveVertex* vtx, SvtxTrack* svtxtrk);
		//Looks up \param svtxtrk id in {@link _svtxtrk_gftrk_map}. If \param svtxtrk is NULL @return NULL.
		PHGenFit::Track* getPHGFTrack(SvtxTrack* svtxtrk);		
		void reset_eval_variables();

		void set_do_eval(bool doEval){
			_do_eval = doEval;
		}
		//! changes how genfit tracks are calculated and vtx fit
		void set_use_ladder_geom(bool useLadderGeom){
			_use_ladder_geom = useLadderGeom;
		}

		void set_do_evt_display(bool doEvtDisplay){
			_do_evt_display = doEvtDisplay;
		}

		void set_reverse_mag_field(bool reverseMagField){
			_reverse_mag_field = reverseMagField;
		}

		void set_mag_field_re_scaling_factor(float magFieldReScalingFactor){
			_mag_field_re_scaling_factor = magFieldReScalingFactor;
		}
		//! string name for the rave hueristic
		void set_vertexing_method(const std::string& vertexingMethod){
			_vertexing_method = vertexingMethod;
		}

		void set_mag_field_file_name(const std::string& magFieldFileName){
			_mag_field_file_name = magFieldFileName;
		}

		const std::string& get_track_fitting_alg_name() const{
			return _track_fitting_alg_name;
		}

		void set_track_fitting_alg_name(const std::string& trackFittingAlgName){
			_track_fitting_alg_name = trackFittingAlgName;
		}
		//! used for building the tracks
		void set_primary_pid_guess(int primaryPidGuess){
			_primary_pid_guess = primaryPidGuess;
		}

		void set_cut_min_pT(double cutMinPT){
			_cut_min_pT = cutMinPT;
		}

		void set_cut_dca(double cutDCA){
			_cut_dca = cutDCA;
		}

		void set_cut_chi2ndf(double cutChi2Ndf){
			_cut_chi2_ndf = cutChi2Ndf;
		}

		void set_cut_ncluster(bool cutNcluster){
			_cut_Ncluster = cutNcluster;
		}

		void set_n_maps_layer(unsigned int n){
			_n_maps_layer = n;
		}

		void set_n_intt_layer(unsigned int n){
			_n_intt_layer = n;
		}

	private:

		//! Get all the nodes
		int GetNodes(PHCompositeNode *);

		//!Create New nodes
		int CreateNodes(PHCompositeNode *);

		///Uses the tracking clusters exclusively to make a GFTrack. @return ownership.
		PHGenFit::Track* MakeGenFitTrack(const SvtxTrack* intrack);
		///Uses the tracking clusters and a vertex to fit/refit the track. @return ownership.
		PHGenFit::Track* MakeGenFitTrack(const SvtxTrack* intrack, const SvtxVertex* invertex);
		///Uses the tracking clusters and a vertex to fit/refit the track. @return ownership.
    PHGenFit::Track* MakeGenFitTrack(const SvtxTrack* intrack, const genfit::GFRaveVertex* invertex);
    ///Uses the vertex to remake the track
//    PHGenFit::Track* MakeGenFitTrack(const PHGenFit::Track* intrack, const genfit::GFRaveVertex* invertex);
		///From {@link PHG4TrackKalmanFitter} 
		std::shared_ptr<SvtxTrack> MakeSvtxTrack(const SvtxTrack* svtx_track,
				const PHGenFit::Track* phgf_track, const SvtxVertex* vertex);
		//! Fill SvtxVertexMap from GFRaveVertexes and Tracks
		void FillVertexMap(
				const std::vector<genfit::GFRaveVertex*> & rave_vertices,
				const std::vector<genfit::Track*> & gf_tracks);
		void printGenFitTrackKinematics(PHGenFit::Track* track);

		void FillSVMap(
				const std::vector<genfit::GFRaveVertex*> & rave_vertices,
				const std::vector<genfit::Track*> & gf_tracks);

		int GetSVMass_mom(
				const genfit::GFRaveVertex* rave_vtx,
				float & vtx_mass,
				float & vtx_px,
				float & vtx_py,
				float & vtx_pz,
				int & ntrk_good_pv 
				);

		//!
		std::string _mag_field_file_name;

		//! created by InitEvent to associate genfit to Svtx
		std::vector<PHGenFit::Track*> _main_rf_phgf_tracks;

		//! rescale mag field, modify the original mag field read in
		float _mag_field_re_scaling_factor;

		//! Switch to reverse Magnetic field
		bool _reverse_mag_field;

		PHGenFit::Fitter* _fitter;
		std::string _track_fitting_alg_name;

		unsigned int _n_maps_layer;
		unsigned int _n_intt_layer;

		int _primary_pid_guess;
		bool _cut_Ncluster;
		double _cut_min_pT;
		double _cut_dca;
		double _cut_chi2_ndf;

		bool _use_ladder_geom;

		//! https://rave.hepforge.org/trac/wiki/RaveMethods
		genfit::GFRaveVertexFactory* _vertex_finder;
		std::string _vertexing_method;

		//! Input Node pointers
		TrkrClusterContainer* _clustermap;
		SvtxTrackMap* _trackmap;
		SvtxVertexMap* _vertexmap;
		PHG4CylinderGeomContainer* _geom_container_intt;
		PHG4CylinderGeomContainer* _geom_container_maps;

		//! switch eval out
		bool _do_eval;

		int _verbosity;
		/** \defgroup eval Variables
			@{*/
		float gf_prim_vtx[3];
		float gf_prim_vtx_err[3];
		int gf_prim_vtx_ntrk;
		float rv_prim_vtx[3];
		float rv_prim_vtx_err[3];
		int rv_prim_vtx_ntrk;

		int rv_sv_njets;
		int rv_sv_jet_id[10];
		int rv_sv_jet_prop[10][2];
		float rv_sv_jet_pT[10];
		float rv_sv_jet_px[10];
		float rv_sv_jet_py[10];
		float rv_sv_jet_pz[10];

		int rv_sv_pT00_nvtx[10];
		float rv_sv_pT00_vtx_x[10][30];
		float rv_sv_pT00_vtx_y[10][30];
		float rv_sv_pT00_vtx_z[10][30];
		float rv_sv_pT00_vtx_ex[10][30];
		float rv_sv_pT00_vtx_ey[10][30];
		float rv_sv_pT00_vtx_ez[10][30];
		int rv_sv_pT00_vtx_ntrk[10][30];
		int rv_sv_pT00_vtx_ntrk_good[10][30];
		int rv_sv_pT00_vtx_ntrk_good_pv[10][30];
		float rv_sv_pT00_vtx_mass[10][30];
		float rv_sv_pT00_vtx_mass_corr[10][30];
		float rv_sv_pT00_vtx_pT[10][30];
		float rv_sv_pT00_vtx_jet_theta[10][30];
		float rv_sv_pT00_vtx_chi2[10][30];
		float rv_sv_pT00_vtx_ndf[10][30];

		int rv_sv_pT05_nvtx[10];
		float rv_sv_pT05_vtx_x[10][30];
		float rv_sv_pT05_vtx_y[10][30];
		float rv_sv_pT05_vtx_z[10][30];
		float rv_sv_pT05_vtx_ex[10][30];
		float rv_sv_pT05_vtx_ey[10][30];
		float rv_sv_pT05_vtx_ez[10][30];
		int rv_sv_pT05_vtx_ntrk[10][30];
		int rv_sv_pT05_vtx_ntrk_good[10][30];
		int rv_sv_pT05_vtx_ntrk_good_pv[10][30];
		float rv_sv_pT05_vtx_mass[10][30];
		float rv_sv_pT05_vtx_mass_corr[10][30];
		float rv_sv_pT05_vtx_pT[10][30];
		float rv_sv_pT05_vtx_jet_theta[10][30];
		float rv_sv_pT05_vtx_chi2[10][30];
		float rv_sv_pT05_vtx_ndf[10][30];

		int rv_sv_pT10_nvtx[10];
		float rv_sv_pT10_vtx_x[10][30];
		float rv_sv_pT10_vtx_y[10][30];
		float rv_sv_pT10_vtx_z[10][30];
		float rv_sv_pT10_vtx_ex[10][30];
		float rv_sv_pT10_vtx_ey[10][30];
		float rv_sv_pT10_vtx_ez[10][30];
		int rv_sv_pT10_vtx_ntrk[10][30];
		int rv_sv_pT10_vtx_ntrk_good[10][30];
		int rv_sv_pT10_vtx_ntrk_good_pv[10][30];
		float rv_sv_pT10_vtx_mass[10][30];
		float rv_sv_pT10_vtx_mass_corr[10][30];
		float rv_sv_pT10_vtx_pT[10][30];
		float rv_sv_pT10_vtx_jet_theta[10][30];
		float rv_sv_pT10_vtx_chi2[10][30];
		float rv_sv_pT10_vtx_ndf[10][30];

		int rv_sv_pT15_nvtx[10];
		float rv_sv_pT15_vtx_x[10][30];
		float rv_sv_pT15_vtx_y[10][30];
		float rv_sv_pT15_vtx_z[10][30];
		float rv_sv_pT15_vtx_ex[10][30];
		float rv_sv_pT15_vtx_ey[10][30];
		float rv_sv_pT15_vtx_ez[10][30];
		int rv_sv_pT15_vtx_ntrk[10][30];
		int rv_sv_pT15_vtx_ntrk_good[10][30];
		int rv_sv_pT15_vtx_ntrk_good_pv[10][30];
		float rv_sv_pT15_vtx_mass[10][30];
		float rv_sv_pT15_vtx_mass_corr[10][30];
		float rv_sv_pT15_vtx_pT[10][30];
		float rv_sv_pT15_vtx_jet_theta[10][30];
		float rv_sv_pT15_vtx_chi2[10][30];
		float rv_sv_pT15_vtx_ndf[10][30];
		/**@}*/

		bool _do_evt_display;

		std::map<unsigned int, unsigned int> _svtxtrk_gftrk_map;
		std::map<unsigned int, float> _svtxtrk_wt_map;
		std::map<unsigned int, float> _svtxtrk_pt_map;
		std::vector<unsigned int> _svtxtrk_id;

};

#endif //* __SVReco_H__ *//
