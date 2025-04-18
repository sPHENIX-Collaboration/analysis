#include "SVReco.h"
/*#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxHitMap.h>*/
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertex_v1.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterv1.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <mvtx/MvtxDefs.h>
#include <intt/InttDefs.h>

#include <g4jets/JetMap.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

//#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <mvtx/CylinderGeom_Mvtx.h>
#include <intt/CylinderGeomIntt.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/Track.h>
#include <phgenfit/SpacepointMeasurement.h>

#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>

#include <phgeom/PHGeomUtility.h>
#include <phfield/PHFieldUtility.h>

#include <GenFit/FieldManager.h>
#include <GenFit/GFRaveVertex.h>
#include <GenFit/GFRaveVertexFactory.h>
#include <GenFit/MeasuredStateOnPlane.h>
#include <GenFit/RKTrackRep.h>
#include <GenFit/StateOnPlane.h>
#include <GenFit/Track.h>
#include <GenFit/KalmanFitterInfo.h>

//#include <HFJetTruthGeneration/HFJetDefs.h>

#include <TClonesArray.h>
#include <TMatrixDSym.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TRotation.h>

#include <iostream>
#include <utility>
#include <memory>

#define LogDebug(exp)		std::cout<<"DEBUG: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<std::endl
#define LogError(exp)		std::cout<<"ERROR: "  <<__FILE__<<": "<<__LINE__<<": "<< exp <<std::endl
#define LogWarning(exp)	std::cout<<"WARNING: "<<__FILE__<<": "<<__LINE__<<": "<< exp <<std::endl

using namespace std;

/*Rave
#include <rave/Version.h>
//#include <rave/Track.h>
#include <rave/VertexFactory.h>
#include <rave/ConstantMagneticField.h>
*/
//GenFit
//#include <GenFit/GFRaveConverters.h>


/*
 * Constructor
 */
SVReco::SVReco(const string &name) :
	_mag_field_file_name("/phenix/upgrades/decadal/fieldmaps/sPHENIX.2d.root"),
	_mag_field_re_scaling_factor(1.4 / 1.5), //what is this and why?
	_reverse_mag_field(false),
	_fitter(NULL),
	_track_fitting_alg_name("DafRef"),
	_n_maps_layer(3),
	_n_intt_layer(4),
	_primary_pid_guess(11), //for the tracks
	_cut_Ncluster(false),
	_cut_min_pT(0.1),
	_cut_dca(5.0), //probably in mm
	_cut_chi2_ndf(5),
	_use_ladder_geom(false),
	_vertex_finder(NULL),
	_vertexing_method("avf-smoothing:1"), /*need a list of these and their proper domains*/
	_clustermap(NULL),
	_trackmap(NULL),
	_vertexmap(NULL),
	_do_eval(false),
	_verbosity(10),
	_do_evt_display(false)
{

}


int SVReco::InitEvent(PHCompositeNode *topNode) {
  cout<<"getting SVR nodes"<<endl;
	GetNodes(topNode);
	//cout<<"got vertexing nodes"<<endl;
	//! stands for Refit_GenFit_Tracks
	for(auto p : _main_rf_phgf_tracks) delete p;
	_main_rf_phgf_tracks.clear();

	_svtxtrk_gftrk_map.clear();
	_svtxtrk_wt_map.clear();
	_svtxtrk_id.clear();

	//iterate over all tracks to find priary vertex and make rave/genfit objects
  cout<<"start SVR track loop"<<endl;
	for (SvtxTrackMap::Iter iter = _trackmap->begin(); iter != _trackmap->end();
			++iter) {
		SvtxTrack* svtx_track = iter->second;
		// do track cuts
		if (!svtx_track || svtx_track->get_ndf()<40 || svtx_track->get_pt()<_cut_min_pT ||
				(svtx_track->get_chisq()/svtx_track->get_ndf())>_cut_chi2_ndf ||
				fabs(svtx_track->get_dca3d_xy())>_cut_dca || fabs(svtx_track->get_dca3d_z())>_cut_dca)
			continue;

		int n_MVTX = 0, n_INTT = 0, n_TPC = 0;
		//cout<<"Keys:";
		for (SvtxTrack::ConstClusterKeyIter iter2 = svtx_track->begin_cluster_keys(); iter2!=svtx_track->end_cluster_keys(); iter2++) {
			TrkrDefs::cluskey cluster_key = *iter2;
			//cout<<cluster_key<<',';
			//count where the hits are
			float layer = (float) TrkrDefs::getLayer(cluster_key);
			if (layer<_n_maps_layer) n_MVTX++;
			else if (layer<_n_maps_layer+_n_intt_layer) n_INTT++;
			else n_TPC++;
		}//cluster loop
		//cluster cuts
		//cout<<"\n cluster loop with n_MVTX="<<n_MVTX<<" n_INTT="<<n_INTT<<" and nTPC="<<n_TPC<<endl;
		if ( _cut_Ncluster && (n_MVTX<2 || n_INTT<2 || n_TPC<25) ){
			continue;
		}
		//cout << (svtx_track->get_chisq()/svtx_track->get_ndf()) << ", " << n_TPC << ", " << svtx_track->get_pt() << endl;
		//cout << svtx_track->get_ndf() << ", " << svtx_track->size_clusters() << endl;
		//cout<<"making genfit"<<endl;
		PHGenFit::Track* rf_phgf_track = MakeGenFitTrack(svtx_track); //convert SvtxTrack to GenFit Track
		//cout<<"made genfit"<<endl;

		if (rf_phgf_track) {
			//svtx_track->identify();
			//make a map to connect SvtxTracks to their respective GenFit Tracks
			_svtxtrk_id.push_back(svtx_track->get_id());
			_svtxtrk_gftrk_map[svtx_track->get_id()] = _main_rf_phgf_tracks.size();
			_main_rf_phgf_tracks.push_back(rf_phgf_track); //to be used by findSecondaryVerticies
		}
	}
	cout<<"exit track loop ntracks="<<_main_rf_phgf_tracks.size()<<'\n';

	return Fun4AllReturnCodes::EVENT_OK;
}

PHGenFit::Track* SVReco::getPHGFTrack(SvtxTrack* svtxtrk){
 if(svtxtrk)return _main_rf_phgf_tracks[_svtxtrk_gftrk_map[svtxtrk->get_id()]];
 else return NULL;
}


int SVReco::InitRun(PHCompositeNode *topNode) {

	CreateNodes(topNode);

	TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);
	PHField * field = PHFieldUtility::GetFieldMapNode(nullptr, topNode);

	_fitter = PHGenFit::Fitter::getInstance(tgeo_manager, field,
			_track_fitting_alg_name, "RKTrackRep", _do_evt_display);

	if (!_fitter) {
		cerr << PHWHERE << endl;
		return Fun4AllReturnCodes::ABORTRUN;
	}

	_vertex_finder = new genfit::GFRaveVertexFactory(_verbosity);
	_vertex_finder->setMethod(_vertexing_method.data());

	if (!_vertex_finder) {
		std::cerr<< PHWHERE<<" bad run init no SVR"<<endl;
		return Fun4AllReturnCodes::ABORTRUN;
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

//note that this might only work for conversion like events
genfit::GFRaveVertex* SVReco::findSecondaryVertex(SvtxTrack* track1, SvtxTrack* track2) {
	//_vertex_finder->setMethod("avr-smoothing:1");
	_vertex_finder->setMethod("avr");
	//_vertex_finder->setMethod("avf-smoothing:1");
	//_vertex_finder->setMethod("kalman");
	vector<genfit::GFRaveVertex*> rave_vertices_conversion;
	vector<genfit::Track*> rf_gf_tracks_conversion;

	if (_svtxtrk_gftrk_map.find(track1->get_id())!=_svtxtrk_gftrk_map.end()&&
			_svtxtrk_gftrk_map.find(track2->get_id())!=_svtxtrk_gftrk_map.end())
	{
		unsigned int trk_index = _svtxtrk_gftrk_map[track1->get_id()];
		PHGenFit::Track* rf_phgf_track = _main_rf_phgf_tracks[trk_index];
		rf_gf_tracks_conversion.push_back(rf_phgf_track->getGenFitTrack());
		//check the genfit track is working
		/*cout<<"Track Comparison Original:\n";
			track1->identify();*/
		//printGenFitTrackKinematics(rf_phgf_track);

		trk_index = _svtxtrk_gftrk_map[track2->get_id()];
		rf_phgf_track = _main_rf_phgf_tracks[trk_index];
		//    track2->identify();
		// printGenFitTrackKinematics(rf_phgf_track);
		rf_gf_tracks_conversion.push_back(rf_phgf_track->getGenFitTrack());
	}
	if (rf_gf_tracks_conversion.size()>1){
		try{
			_vertex_finder->findVertices(&rave_vertices_conversion, rf_gf_tracks_conversion);
		}catch (...){
			std::cout << PHWHERE << "GFRaveVertexFactory::findVertices failed!";
		}
		/*      if(TMath::Abs(rave_vertices_conversion[0]->getChi2()-1.)>.3){
						cout<<"PRINTING:\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n";
						rave_vertices_conversion[0]->Print();
						track1->identify();
						track2->identify();
						cout<<"\n\n\n\n\n";
						}*/
		//if a vertex is found return ownership 
    //TODO check rave_verticies_conversion for mem leak
		if(rave_vertices_conversion.size()>0) return rave_vertices_conversion[0];
		else return NULL;
	}//more than 1 track 
	else return NULL;
}

SVReco::~SVReco(){
	cout<<PHWHERE<<"delete"<<endl;
	if(_fitter){
		delete _fitter;
		_fitter=NULL;
	}
	cout<<PHWHERE<<"delete"<<endl;
	if(_vertex_finder){
		delete _vertex_finder;
		_vertex_finder=NULL;
	}
	cout<<PHWHERE<<"delete"<<endl;
	for (std::vector<PHGenFit::Track*>::iterator i = _main_rf_phgf_tracks.begin(); i != _main_rf_phgf_tracks.end(); ++i)
	{
		if(*i)
			delete *i;
		*i=NULL;
	}
}

//prints the track using the trival verex for point extrapolation
void SVReco::printGenFitTrackKinematics(PHGenFit::Track* track){

	TVector3 vertex_position(0, 0, 0);

	std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state_vertex_ca = NULL;
	try {
		gf_state_vertex_ca = std::shared_ptr < genfit::MeasuredStateOnPlane> (track->extrapolateToPoint(vertex_position));
	} catch (...) {
		if (_verbosity >= 2)
			LogWarning("extrapolateToPoint failed!");
	}
	TVector3 mom = gf_state_vertex_ca->getMom();
	TMatrixDSym cov = gf_state_vertex_ca->get6DCov();

	cout << "OUT Ex: " << sqrt(cov[0][0]) << ", Ey: " << sqrt(cov[1][1]) << ", Ez: " << sqrt(cov[2][2]) << endl;
	cout << "OUT Px: " << mom.X() << ", Py: " << mom.Y() << ", Pz: " << mom.Z() << endl; 
}



//should be deprecated
void SVReco::reset_eval_variables(){
	gf_prim_vtx_ntrk = rv_prim_vtx_ntrk = 0;
	for (int i=0; i<3; i++){
		gf_prim_vtx[i] = gf_prim_vtx_err[i] = -999;
		rv_prim_vtx[i] = rv_prim_vtx_err[i] = -999;
	}//i

	rv_sv_njets = 0;

	for (int ijet=0; ijet<10; ijet++){
		rv_sv_jet_id[ijet] = -999;
		rv_sv_jet_prop[ijet][0] = rv_sv_jet_prop[ijet][1] = -999;
		rv_sv_jet_pT[ijet] = -999;
		rv_sv_jet_px[ijet] = rv_sv_jet_py[ijet] = rv_sv_jet_pz[ijet] = -999;

		rv_sv_pT00_nvtx[ijet] = rv_sv_pT05_nvtx[ijet] = rv_sv_pT10_nvtx[ijet] = rv_sv_pT15_nvtx[ijet] = 0;

		for (int ivtx=0; ivtx<30; ivtx++){
			rv_sv_pT00_vtx_ntrk[ijet][ivtx] = rv_sv_pT05_vtx_ntrk[ijet][ivtx] = rv_sv_pT10_vtx_ntrk[ijet][ivtx] = rv_sv_pT15_vtx_ntrk[ijet][ivtx] = 0;
			rv_sv_pT00_vtx_ntrk_good[ijet][ivtx] = rv_sv_pT05_vtx_ntrk_good[ijet][ivtx] = rv_sv_pT10_vtx_ntrk_good[ijet][ivtx] = rv_sv_pT15_vtx_ntrk_good[ijet][ivtx] = 0;
			rv_sv_pT00_vtx_ntrk_good_pv[ijet][ivtx] = rv_sv_pT05_vtx_ntrk_good_pv[ijet][ivtx] = rv_sv_pT10_vtx_ntrk_good_pv[ijet][ivtx] = rv_sv_pT15_vtx_ntrk_good_pv[ijet][ivtx] = 0;

			rv_sv_pT00_vtx_mass[ijet][ivtx] = rv_sv_pT00_vtx_mass_corr[ijet][ivtx] = rv_sv_pT00_vtx_pT[ijet][ivtx] = -999;
			rv_sv_pT05_vtx_mass[ijet][ivtx] = rv_sv_pT05_vtx_mass_corr[ijet][ivtx] = rv_sv_pT05_vtx_pT[ijet][ivtx] = -999;
			rv_sv_pT10_vtx_mass[ijet][ivtx] = rv_sv_pT10_vtx_mass_corr[ijet][ivtx] = rv_sv_pT10_vtx_pT[ijet][ivtx] = -999;
			rv_sv_pT15_vtx_mass[ijet][ivtx] = rv_sv_pT15_vtx_mass_corr[ijet][ivtx] = rv_sv_pT15_vtx_pT[ijet][ivtx] = -999;

			rv_sv_pT00_vtx_x[ijet][ivtx] = rv_sv_pT00_vtx_y[ijet][ivtx] = rv_sv_pT00_vtx_z[ijet][ivtx] = -999;
			rv_sv_pT00_vtx_ex[ijet][ivtx] = rv_sv_pT00_vtx_ey[ijet][ivtx] = rv_sv_pT00_vtx_ez[ijet][ivtx] = -999;
			rv_sv_pT05_vtx_x[ijet][ivtx] = rv_sv_pT05_vtx_y[ijet][ivtx] = rv_sv_pT05_vtx_z[ijet][ivtx] = -999;
			rv_sv_pT05_vtx_ex[ijet][ivtx] = rv_sv_pT05_vtx_ey[ijet][ivtx] = rv_sv_pT05_vtx_ez[ijet][ivtx] = -999;
			rv_sv_pT10_vtx_x[ijet][ivtx] = rv_sv_pT10_vtx_y[ijet][ivtx] = rv_sv_pT10_vtx_z[ijet][ivtx] = -999;
			rv_sv_pT10_vtx_ex[ijet][ivtx] = rv_sv_pT10_vtx_ey[ijet][ivtx] = rv_sv_pT10_vtx_ez[ijet][ivtx] = -999;
			rv_sv_pT15_vtx_x[ijet][ivtx] = rv_sv_pT15_vtx_y[ijet][ivtx] = rv_sv_pT15_vtx_z[ijet][ivtx] = -999;
			rv_sv_pT15_vtx_ex[ijet][ivtx] = rv_sv_pT15_vtx_ey[ijet][ivtx] = rv_sv_pT15_vtx_ez[ijet][ivtx] = -999;

			rv_sv_pT00_vtx_jet_theta[ijet][ivtx] = rv_sv_pT00_vtx_pT[ijet][ivtx] = -999;
			rv_sv_pT05_vtx_jet_theta[ijet][ivtx] = rv_sv_pT05_vtx_pT[ijet][ivtx] = -999;
			rv_sv_pT10_vtx_jet_theta[ijet][ivtx] = rv_sv_pT10_vtx_pT[ijet][ivtx] = -999;
			rv_sv_pT15_vtx_jet_theta[ijet][ivtx] = rv_sv_pT15_vtx_pT[ijet][ivtx] = -999;

			rv_sv_pT00_vtx_chi2[ijet][ivtx] = rv_sv_pT00_vtx_ndf[ijet][ivtx] = -999;
			rv_sv_pT05_vtx_chi2[ijet][ivtx] = rv_sv_pT05_vtx_ndf[ijet][ivtx] = -999;
			rv_sv_pT10_vtx_chi2[ijet][ivtx] = rv_sv_pT10_vtx_ndf[ijet][ivtx] = -999;
			rv_sv_pT15_vtx_chi2[ijet][ivtx] = rv_sv_pT15_vtx_ndf[ijet][ivtx] = -999;

		}//ivtx
	}//ijet

	return;
}

int SVReco::CreateNodes(PHCompositeNode *topNode){

	return Fun4AllReturnCodes::EVENT_OK;
}

/*
 * GetNodes():
 *  Get all the all the required nodes off the node tree
 */
int SVReco::GetNodes(PHCompositeNode * topNode){
	_clustermap = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
	if (!_clustermap){
		cout << PHWHERE << " TRKR_CLUSTERS node not found on node tree"
			<< endl;
		return Fun4AllReturnCodes::ABORTEVENT;
	}
	// Input Svtx Tracks
	_trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
	if (!_trackmap){
		cout << PHWHERE << " SvtxTrackMap node not found on node tree"
			<< endl;
		return Fun4AllReturnCodes::ABORTEVENT;
	}
	// Input Svtx Vertices
	_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
	if (!_vertexmap){
		cout << PHWHERE << " SvtxVertexrMap node not found on node tree"
			<< endl;
		return Fun4AllReturnCodes::ABORTEVENT;
	}

	_geom_container_intt = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_INTT");
	if (!_geom_container_intt)
	{
		cout << PHWHERE << "CYLINDERGEOM_INTT node not found on node tree"
			<< endl;
		return Fun4AllReturnCodes::ABORTEVENT;
	}
	_geom_container_maps = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MVTX");
		if (!_geom_container_maps)
		{
			cout << PHWHERE << "CYLINDERGEOM_MVTX node not found on node tree"
				<< endl;
			return Fun4AllReturnCodes::ABORTEVENT;
		}
	if(!_vertex_finder){
		std::cerr<< PHWHERE<<" bad run init no SVR"<<endl;
		return Fun4AllReturnCodes::ABORTRUN;
	} 


	return Fun4AllReturnCodes::EVENT_OK;
}

//Edited version original from @sh-lim
PHGenFit::Track* SVReco::MakeGenFitTrack(const SvtxTrack* intrack){
	if (!intrack){
		cerr << PHWHERE << " Input SvtxTrack is NULL!" << endl;
		return NULL;
	}

	if (_use_ladder_geom and !_geom_container_intt and !_geom_container_maps) {
		cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << endl;
		return NULL;
	}

	TVector3 seed_pos(intrack->get_x(), intrack->get_y(), intrack->get_z());
	TVector3 seed_mom(intrack->get_px(), intrack->get_py(), intrack->get_pz()); //mom stands for momentum
	TMatrixDSym seed_cov(6);
	for (int i=0; i<6; i++){
		for (int j=0; j<6; j++){
			seed_cov[i][j] = intrack->get_error(i,j);
		}
	}

	// Create measurements
	std::vector<PHGenFit::Measurement*> measurements;

	for (auto iter = intrack->begin_cluster_keys(); iter != intrack->end_cluster_keys(); ++iter){
		//    unsigned int cluster_id = *iter;
		TrkrCluster* cluster = _clustermap->findCluster(*iter);
		if (!cluster) {
			LogError("No cluster Found!");
			continue;
		}
		float x = cluster->getPosition(0);
		float y = cluster->getPosition(1);
		float radius = sqrt(x*x+y*y);
		TVector3 pos(cluster->getPosition(0), cluster->getPosition(1), cluster->getPosition(2));
		seed_mom.SetPhi(pos.Phi());
		seed_mom.SetTheta(pos.Theta());

		TVector3 n(cluster->getPosition(0), cluster->getPosition(1), 0);
		//cout<<"Cluster with {"<<cluster->getPosition(0)<<','<<cluster->getPosition(0)<<"}\n";

		if (_use_ladder_geom){ //I don't understand this bool
			unsigned int trkrid = TrkrDefs::getTrkrId(*iter);
			unsigned int layer = TrkrDefs::getLayer(*iter);
			if (trkrid == TrkrDefs::mvtxId) {
				int stave_index = MvtxDefs::getStaveId(*iter);
				int chip_index = MvtxDefs::getChipId(*iter);

				double ladder_location[3] = { 0.0, 0.0, 0.0 };
				//not exactly sure where the cylinder geoms are currently objectified. check this 
				CylinderGeom_Mvtx *geom = (CylinderGeom_Mvtx*) _geom_container_maps->GetLayerGeom(layer);
				// returns the center of the sensor in world coordinates - used to get the ladder phi location
				geom->find_sensor_center(stave_index, 0, 0, chip_index, ladder_location);//the mvtx module and half stave are 0
				n.SetXYZ(ladder_location[0], ladder_location[1], 0);
				n.RotateZ(geom->get_stave_phi_tilt());
			} 
			else if (trkrid == TrkrDefs::inttId) {
				//this may bug but it looks ok for now
				CylinderGeomIntt* geom = (CylinderGeomIntt*) _geom_container_intt->GetLayerGeom(layer);
				double hit_location[3] = { 0.0, 0.0, 0.0 };
				geom->find_segment_center(InttDefs::getLadderZId(*iter),InttDefs::getLadderPhiId(*iter), hit_location);

				n.SetXYZ(hit_location[0], hit_location[1], 0);
				n.RotateZ(geom->get_strip_phi_tilt());
			}
		}//if use_ladder_geom
		PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,radius*cluster->getPhiError(), cluster->getZError());
		measurements.push_back(meas);
	}//cluster loop
	genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
	PHGenFit::Track* track(new PHGenFit::Track(rep, seed_pos, seed_mom, seed_cov));
	track->addMeasurements(measurements);

	if (_fitter->processTrack(track, false) != 0) {
		if (_verbosity >= 1)
			LogWarning("Track fitting failed");
		return NULL;
	}

	track->getGenFitTrack()->setMcTrackId(intrack->get_id());

	return track;
}

//inspired by PHG4TrackKalmanFitter
PHGenFit::Track* SVReco::MakeGenFitTrack(const SvtxTrack* intrack, const SvtxVertex* invertex){
  if (!intrack){
    cerr << PHWHERE << " Input SvtxTrack is NULL!" << endl;
    return NULL;
  }

  if (_use_ladder_geom and !_geom_container_intt and !_geom_container_maps) {
    cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << endl;
    return NULL;
  }

  // Create measurements
  std::vector<PHGenFit::Measurement*> measurements;

  //create space point measurement from vtx
  if (invertex and invertex->size_tracks() > 1) {
    TVector3 pos(invertex->get_x(), invertex->get_y(), invertex->get_z());
    TMatrixDSym cov(3);
    cov.Zero();
    bool is_vertex_cov_sane = true;
    cout<<"Making covarience for vtx measurement"<<endl;
    for (unsigned int i = 0; i < 3; i++)
      for (unsigned int j = 0; j < 3; j++) {
        cov(i, j) = invertex->get_error(i, j);
      }

    cout<<"Made covarience"<<endl;
    if (is_vertex_cov_sane) {
      PHGenFit::Measurement* meas = new PHGenFit::SpacepointMeasurement(
          pos, cov);
      measurements.push_back(meas);
    }

    //convert SvtxTrack to matricies
    TVector3 seed_pos(intrack->get_x(), intrack->get_y(), intrack->get_z());
    TVector3 seed_mom(intrack->get_px(), intrack->get_py(), intrack->get_pz()); //mom stands for momentum
    TMatrixDSym seed_cov(6);
    for (int i=0; i<6; i++){
      for (int j=0; j<6; j++){
        seed_cov[i][j] = intrack->get_error(i,j);
      }
    } 
    cout<<"Making track cluster measurments"<<endl;
    //make measurements from the track clusters
    for (auto iter = intrack->begin_cluster_keys(); iter != intrack->end_cluster_keys(); ++iter){
      //    unsigned int cluster_id = *iter;
      TrkrCluster* cluster = _clustermap->findCluster(*iter);
      if (!cluster) {
        LogError("No cluster Found!");
        continue;
      }
      float x = cluster->getPosition(0);
      float y = cluster->getPosition(1);
      float radius = sqrt(x*x+y*y);
      TVector3 pos(cluster->getPosition(0), cluster->getPosition(1), cluster->getPosition(2));
      seed_mom.SetPhi(pos.Phi());
      seed_mom.SetTheta(pos.Theta());

      TVector3 n(cluster->getPosition(0), cluster->getPosition(1), 0);
      //cout<<"Cluster with {"<<cluster->getPosition(0)<<','<<cluster->getPosition(0)<<"}\n";

      if (_use_ladder_geom){ //I don't understand this bool
        unsigned int trkrid = TrkrDefs::getTrkrId(*iter);
        unsigned int layer = TrkrDefs::getLayer(*iter);
        if (trkrid == TrkrDefs::mvtxId) {
          int stave_index = MvtxDefs::getStaveId(*iter);
          int chip_index = MvtxDefs::getChipId(*iter);

          double ladder_location[3] = { 0.0, 0.0, 0.0 };
          //not exactly sure where the cylinder geoms are currently objectified. check this 
          CylinderGeom_Mvtx *geom = (CylinderGeom_Mvtx*) _geom_container_maps->GetLayerGeom(layer);
          // returns the center of the sensor in world coordinates - used to get the ladder phi location
          geom->find_sensor_center(stave_index, 0, 0, chip_index, ladder_location);//the mvtx module and half stave are 0
          n.SetXYZ(ladder_location[0], ladder_location[1], 0);
          n.RotateZ(geom->get_stave_phi_tilt());
        } 
        else if (trkrid == TrkrDefs::inttId) {
          //this may bug but it looks ok for now
          CylinderGeomIntt* geom = (CylinderGeomIntt*) _geom_container_intt->GetLayerGeom(layer);
          double hit_location[3] = { 0.0, 0.0, 0.0 };
          geom->find_segment_center(InttDefs::getLadderZId(*iter),InttDefs::getLadderPhiId(*iter), hit_location);

          n.SetXYZ(hit_location[0], hit_location[1], 0);
          n.RotateZ(geom->get_strip_phi_tilt());
        }
      }//if use_ladder_geom
      PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,radius*cluster->getPhiError(), cluster->getZError());
      measurements.push_back(meas);
    }//cluster loop
    genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
    PHGenFit::Track* track(new PHGenFit::Track(rep, seed_pos, seed_mom, seed_cov));
    track->addMeasurements(measurements);

    if (_fitter->processTrack(track, false) != 0) {
      if (_verbosity >= 1)
        LogWarning("Track fitting failed");
      return NULL;
    }
    track->getGenFitTrack()->setMcTrackId(intrack->get_id());
    return track;
  }//valid vtx 
  else{
    cerr<<PHWHERE<<": invalid vertex"<<endl;
    return NULL;
  }

}

//inspired by PHG4TrackKalmanFitter
PHGenFit::Track* SVReco::MakeGenFitTrack(const SvtxTrack* intrack, const genfit::GFRaveVertex* invertex){
  if (!intrack){
    cerr << PHWHERE << " Input SvtxTrack is NULL!" << endl;
    return NULL;
  }

  if (_use_ladder_geom and !_geom_container_intt and !_geom_container_maps) {
    cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << endl;
    return NULL;
  }

  // Create measurements
  std::vector<PHGenFit::Measurement*> measurements;

  //create space point measurement from vtx
  //TODO check that getNTracks is properly initialized 
  if (invertex and invertex->getNTracks() > 1) {
    TVector3 pos=invertex->getPos();
    TMatrixDSym cov = invertex->getCov();
    PHGenFit::Measurement* meas = new PHGenFit::SpacepointMeasurement(
          pos, cov);
    measurements.push_back(meas);

    //convert SvtxTrack to matricies
    TVector3 seed_pos(intrack->get_x(), intrack->get_y(), intrack->get_z());
    TVector3 seed_mom(intrack->get_px(), intrack->get_py(), intrack->get_pz()); //mom stands for momentum
    TMatrixDSym seed_cov(6);
    for (int i=0; i<6; i++){
      for (int j=0; j<6; j++){
        seed_cov[i][j] = intrack->get_error(i,j);
      }
    } 
    cout<<"Making track cluster measurments"<<endl;
    //make measurements from the track clusters
    for (auto iter = intrack->begin_cluster_keys(); iter != intrack->end_cluster_keys(); ++iter){
      //    unsigned int cluster_id = *iter;
      TrkrCluster* cluster = _clustermap->findCluster(*iter);
      if (!cluster) {
        LogError("No cluster Found!");
        continue;
      }
      float x = cluster->getPosition(0);
      float y = cluster->getPosition(1);
      float radius = sqrt(x*x+y*y);
      TVector3 pos(cluster->getPosition(0), cluster->getPosition(1), cluster->getPosition(2));
      seed_mom.SetPhi(pos.Phi());
      seed_mom.SetTheta(pos.Theta());

      TVector3 n(cluster->getPosition(0), cluster->getPosition(1), 0);
      //cout<<"Cluster with {"<<cluster->getPosition(0)<<','<<cluster->getPosition(0)<<"}\n";

      if (_use_ladder_geom){ //I don't understand this bool
        unsigned int trkrid = TrkrDefs::getTrkrId(*iter);
        unsigned int layer = TrkrDefs::getLayer(*iter);
        if (trkrid == TrkrDefs::mvtxId) {
          int stave_index = MvtxDefs::getStaveId(*iter);
          int chip_index = MvtxDefs::getChipId(*iter);

          double ladder_location[3] = { 0.0, 0.0, 0.0 };
          //not exactly sure where the cylinder geoms are currently objectified. check this 
          CylinderGeom_Mvtx *geom = (CylinderGeom_Mvtx*) _geom_container_maps->GetLayerGeom(layer);
          // returns the center of the sensor in world coordinates - used to get the ladder phi location
          geom->find_sensor_center(stave_index, 0, 0, chip_index, ladder_location);//the mvtx module and half stave are 0
          n.SetXYZ(ladder_location[0], ladder_location[1], 0);
          n.RotateZ(geom->get_stave_phi_tilt());
        } 
        else if (trkrid == TrkrDefs::inttId) {
          //this may bug but it looks ok for now
          CylinderGeomIntt* geom = (CylinderGeomIntt*) _geom_container_intt->GetLayerGeom(layer);
          double hit_location[3] = { 0.0, 0.0, 0.0 };
          geom->find_segment_center(InttDefs::getLadderZId(*iter),InttDefs::getLadderPhiId(*iter), hit_location);

          n.SetXYZ(hit_location[0], hit_location[1], 0);
          n.RotateZ(geom->get_strip_phi_tilt());
        }
      }//if use_ladder_geom
      PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,radius*cluster->getPhiError(), cluster->getZError());
      measurements.push_back(meas);
    }//cluster loop
    genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
    PHGenFit::Track* track(new PHGenFit::Track(rep, seed_pos, seed_mom, seed_cov));
    track->addMeasurements(measurements);

    if (_fitter->processTrack(track, false) != 0) {
      if (_verbosity >= 1)
        LogWarning("Track fitting failed");
      return NULL;
    }
    track->getGenFitTrack()->setMcTrackId(intrack->get_id());
    return track;
  }//valid vtx 
  else{
    cerr<<PHWHERE<<": invalid vertex"<<endl;
    return NULL;
  }

}

/*inspired by PHG4TrackKalmanFitter
PHGenFit::Track* SVReco::MakeGenFitTrack(const PHGenFit::Track* intrack, const genfit::GFRaveVertex* invertex){
  if (!intrack){
    cerr << PHWHERE << " Input PHGenFit::Track is NULL!" << endl;
    return NULL;
  }

  if (_use_ladder_geom and !_geom_container_intt and !_geom_container_maps) {
    cout << PHWHERE << "No PHG4CylinderGeomContainer found!" << endl;
    return NULL;
  }

  // Create measurements
  std::vector<PHGenFit::Measurement*> measurements;

  //create space point measurement from vtx
  //TODO check that getNTracks is properly initialized 
  if (invertex and invertex->getNTracks() > 1) {
    TVector3 pos=invertex->getPos();
    TMatrixDSym cov = invertex->getCov();
    PHGenFit::Measurement* meas = new PHGenFit::SpacepointMeasurement(
          pos, cov);
    measurements.push_back(meas);

    //convert SvtxTrack to matricies
    TVector3 seed_pos = intrack->getGenFitTrack()->getPos();
    TVector3 seed_mom = intrack->get_mom(); //mom stands for momentum
    TMatrixDSym seed_cov = intrack->getGenFitTrack()->getCov();
    
    cout<<"Making track cluster measurments"<<endl;
    //make measurements from the track clusters
    for (auto iter = intrack->get_cluster_keys().begin(); iter != intrack->get_cluster_keys().end(); ++iter){
      //    unsigned int cluster_id = *iter;
      TrkrCluster* cluster = _clustermap->findCluster(*iter);
      if (!cluster) {
        LogError("No cluster Found!");
        continue;
      }
      float x = cluster->getPosition(0);
      float y = cluster->getPosition(1);
      float radius = sqrt(x*x+y*y);
      TVector3 pos(cluster->getPosition(0), cluster->getPosition(1), cluster->getPosition(2));
      seed_mom.SetPhi(pos.Phi());
      seed_mom.SetTheta(pos.Theta());

      TVector3 n(cluster->getPosition(0), cluster->getPosition(1), 0);
      //cout<<"Cluster with {"<<cluster->getPosition(0)<<','<<cluster->getPosition(0)<<"}\n";

      if (_use_ladder_geom){ //I don't understand this bool
        unsigned int trkrid = TrkrDefs::getTrkrId(*iter);
        unsigned int layer = TrkrDefs::getLayer(*iter);
        if (trkrid == TrkrDefs::mvtxId) {
          int stave_index = MvtxDefs::getStaveId(*iter);
          int chip_index = MvtxDefs::getChipId(*iter);

          double ladder_location[3] = { 0.0, 0.0, 0.0 };
          //not exactly sure where the cylinder geoms are currently objectified. check this 
          CylinderGeom_Mvtx *geom = (CylinderGeom_Mvtx*) _geom_container_maps->GetLayerGeom(layer);
          // returns the center of the sensor in world coordinates - used to get the ladder phi location
          geom->find_sensor_center(stave_index, 0, 0, chip_index, ladder_location);//the mvtx module and half stave are 0
          n.SetXYZ(ladder_location[0], ladder_location[1], 0);
          n.RotateZ(geom->get_stave_phi_tilt());
        } 
        else if (trkrid == TrkrDefs::inttId) {
          //this may bug but it looks ok for now
          CylinderGeomIntt* geom = (CylinderGeomIntt*) _geom_container_intt->GetLayerGeom(layer);
          double hit_location[3] = { 0.0, 0.0, 0.0 };
          geom->find_segment_center(InttDefs::getLadderZId(*iter),InttDefs::getLadderPhiId(*iter), hit_location);

          n.SetXYZ(hit_location[0], hit_location[1], 0);
          n.RotateZ(geom->get_strip_phi_tilt());
        }
      }//if use_ladder_geom
      PHGenFit::Measurement* meas = new PHGenFit::PlanarMeasurement(pos, n,radius*cluster->getPhiError(), cluster->getZError());
      measurements.push_back(meas);
    }//cluster loop
    genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
    PHGenFit::Track* track(new PHGenFit::Track(rep, seed_pos, seed_mom, seed_cov));
    track->addMeasurements(measurements);

    if (_fitter->processTrack(track, false) != 0) {
      if (_verbosity >= 1)
        LogWarning("Track fitting failed");
      return NULL;
    }
    track->getGenFitTrack()->setMcTrackId(intrack->get_id());
    return track;
  }//valid vtx 
  else{
    cerr<<PHWHERE<<": invalid vertex"<<endl;
    return NULL;
  }

}*/

//From PHG4TrackKalmanFitter
SvtxVertex* SVReco::GFRVVtxToSvtxVertex(genfit::GFRaveVertex* rave_vtx)const{
  SvtxVertex* svtx_vtx= new SvtxVertex_v1();

    svtx_vtx->set_chisq(rave_vtx->getChi2());
    svtx_vtx->set_ndof(rave_vtx->getNdf());
    svtx_vtx->set_position(0, rave_vtx->getPos().X());
    svtx_vtx->set_position(1, rave_vtx->getPos().Y());
    svtx_vtx->set_position(2, rave_vtx->getPos().Z());

    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        svtx_vtx->set_error(i, j, rave_vtx->getCov()[i][j]);

    for (unsigned int i = 0; i < rave_vtx->getNTracks(); i++) {
      //TODO Assume id's are sync'ed between _trackmap_refit and gf_tracks, need to change?
      const genfit::Track* rave_track =
          rave_vtx->getParameters(i)->getTrack();
      for (unsigned int j = 0; j < _main_rf_phgf_tracks.size(); j++) {
        if (rave_track == _main_rf_phgf_tracks[j]->getGenFitTrack())
          svtx_vtx->insert_track(j);
      }
    }
    return svtx_vtx;
}

/*
 * Fill SvtxVertexMap from GFRaveVertexes and Tracks
 */
void SVReco::FillVertexMap(
		const std::vector<genfit::GFRaveVertex*>& rave_vertices,
		const std::vector<genfit::Track*>& gf_tracks){

	for (unsigned int ivtx=0; ivtx<rave_vertices.size(); ++ivtx){
		genfit::GFRaveVertex* rave_vtx = rave_vertices[ivtx];
		rv_prim_vtx_ntrk = rave_vtx->getNTracks();

		//TVector3 vertex_position(rv_prim_vtx[0], rv_prim_vtx[1], rv_prim_vtx[2]);

		//cout << "N TRK gf: " << gf_tracks.size() << ", rv: " << rv_prim_vtx_ntrk << endl;

		for (int itrk=0; itrk< rv_prim_vtx_ntrk; itrk++){

			TVector3 rvtrk_mom = rave_vtx->getParameters(itrk)->getMom();
			float rvtrk_w = rave_vtx->getParameters(itrk)->getWeight();

			unsigned int rvtrk_mc_id = rave_vtx->getParameters(itrk)->getTrack()->getMcTrackId();
			_svtxtrk_wt_map[rvtrk_mc_id] = rvtrk_w;

			//cout << "w: " << rvtrk_w << ", mc id: " << rvtrk_mc_id << endl;
			/*
				 SvtxTrack* svtx_track = _trackmap->get(rvtrk_mc_id);

				 cout << "rave trk, px: " << rvtrk_mom.Px() << ", py: " << rvtrk_mom.Py() << ", pz: " << rvtrk_mom.Pz() << endl;
				 cout << "svtx trk, px: " << svtx_track->get_px() << ", py: " << svtx_track->get_py() << ", pz: " << svtx_track->get_pz() << endl;
				 */

			/*
				 for (SvtxTrackMap::ConstIter iter3=_trackmap->begin(); iter3!=_trackmap->end(); iter3++)
				 {
				 SvtxTrack* svtx_track = iter3->second;

				 if ( fabs((svtx_track->get_px()-rvtrk_mom.Px())/svtx_track->get_px())<0.10
				 && fabs((svtx_track->get_py()-rvtrk_mom.Py())/svtx_track->get_py())<0.10
				 && fabs((svtx_track->get_pz()-rvtrk_mom.Pz())/svtx_track->get_pz())<0.10 ){
				 cout << "rave trk, px: " << rvtrk_mom.Px() << ", py: " << rvtrk_mom.Py() << ", pz: " << rvtrk_mom.Pz() << endl;
			//cout << "ggff trk, px: " << gftrk_mom.Px() << ", py: " << gftrk_mom.Py() << ", pz: " << gftrk_mom.Pz() << endl;
			cout << "svtx trk, px: " << svtx_track->get_px() << ", py: " << svtx_track->get_py() << ", pz: " << svtx_track->get_pz() << endl;
			}
			}//iter3
			*/

			//unsigned int trk_id = svtxtrk_id[itrk];

			/*
				 cout << "W: " << w_trk 
				 << ", id: " << rave_vtx->getParameters(itrk)->GetUniqueID()
				 << ", px: " << rave_vtx->getParameters(itrk)->getMom().Px() 
				 << ", py: " << rave_vtx->getParameters(itrk)->getMom().Py() 
				 << ", pz: " << rave_vtx->getParameters(itrk)->getMom().Pz() 
				 << endl;
				 */

			//if (svtxtrk_gftrk_map.find(svtx_track->get_id())!=svtxtrk_gftrk_map.end()){
			//}

			//TVector3 mom_trk = rave_vtx->getParameters(itrk)->getMom();
		}//primvtx_tracks loop
	}//rave verticies loop
	return;
}

int SVReco::GetSVMass_mom(
		const genfit::GFRaveVertex* rave_vtx,
		float & vtx_mass,
		float & vtx_px,
		float & vtx_py,
		float & vtx_pz,
		int & ntrk_good_pv
		){

	float sum_E = 0, sum_px = 0, sum_py = 0, sum_pz = 0;

	int N_good = 0, N_good_pv = 0;

	for (unsigned int itrk=0; itrk<rave_vtx->getNTracks(); itrk++){
		TVector3 mom_trk = rave_vtx->getParameters(itrk)->getMom(); 

		double w_trk = rave_vtx->getParameters(itrk)->getWeight();

		sum_px += mom_trk.X();
		sum_py += mom_trk.Y();
		sum_pz += mom_trk.Z();
		sum_E += sqrt(mom_trk.Mag2() + 0.140*0.140);

		//cout << "W: " << w_trk << endl;
		if ( w_trk>0.7 ){
			N_good++;

			unsigned int rvtrk_mc_id = rave_vtx->getParameters(itrk)->getTrack()->getMcTrackId();
			//cout << "mc_id: " << rvtrk_mc_id << ", wt: " << svtxtrk_wt_map[rvtrk_mc_id] << endl;
			if ( _svtxtrk_wt_map[rvtrk_mc_id]>0.7 ){
				N_good_pv++;
			}
		}//

	}//itrk

	vtx_mass =  sqrt(sum_E*sum_E - sum_px*sum_px - sum_py*sum_py - sum_pz*sum_pz);
	vtx_px = sum_px;
	vtx_py = sum_py;
	vtx_pz = sum_pz;

	ntrk_good_pv = N_good_pv;

	//cout << "Mass: " << vtx_mass << ", pT: " << vtx_pT << endl;
	return N_good;
}

//Seems deprecated
void SVReco::FillSVMap(
		const std::vector<genfit::GFRaveVertex*>& rave_vertices,
		const std::vector<genfit::Track*>& gf_tracks){

	return;
}

PHGenFit::Track*  SVReco::refitTrack(SvtxVertex* vtx, SvtxTrack* svtxtrk){
  PHGenFit::Track* gftrk = MakeGenFitTrack(svtxtrk,vtx);
  if(gftrk) {
    cout<<"good genfit refit"<<endl;
    //MakeSvtxTrack(svtxtrk,gftrk,vtx);
    return gftrk;
  }
  else {
    cout<<"No refit possible"<<endl;
    return NULL;
  }
}

PHGenFit::Track*  SVReco::refitTrack(genfit::GFRaveVertex* vtx, SvtxTrack* svtxtrk){
  PHGenFit::Track* gftrk = MakeGenFitTrack(svtxtrk,vtx);
  if(gftrk) {
    cout<<"good genfit refit"<<endl;
    //MakeSvtxTrack(svtxtrk,gftrk,vtx);
    return gftrk;
  }
  else {
    cout<<"No refit possible"<<endl;
    return NULL;
  }
}

/*PHGenFit::Track*  SVReco::refitTrack(genfit::GFRaveVertex* vtx, PHGenFit::Track* phgf_track){
  PHGenFit::Track* gftrk = MakeGenFitTrack(phgf_track,vtx);
  if(gftrk) {
    cout<<"good genfit refit"<<endl;
    //MakeSvtxTrack(svtxtrk,gftrk,vtx);
    return gftrk;
  }
  else {
    cout<<"No refit possible"<<endl;
    return NULL;
  }
}*/


/*FIXME this code is broken I have made zero attempt to find out why
* I decided not to use the SvtxTrack after they are refit
* may need to make the phgf_track pointer shared 
*/
std::shared_ptr<SvtxTrack> SVReco::MakeSvtxTrack(const SvtxTrack* svtx_track,
		const PHGenFit::Track* phgf_track, const SvtxVertex* vertex) {

	double chi2 = phgf_track->get_chi2();
	double ndf = phgf_track->get_ndf();

	TVector3 vertex_position(0, 0, 0);
	TMatrixF vertex_cov(3,3);
	double dvr2 = 0;
	double dvz2 = 0;

	if (vertex) {
		vertex_position.SetXYZ(vertex->get_x(), vertex->get_y(),
				vertex->get_z());
		dvr2 = vertex->get_error(0, 0) + vertex->get_error(1, 1);
		dvz2 = vertex->get_error(2, 2);

		for(int i=0;i<3;i++)
			for(int j=0;j<3;j++)
				vertex_cov[i][j] = vertex->get_error(i,j);
	}
	else{
		cerr<<PHWHERE<<": No vertex to make SvtxTrack"<<endl;
	}

	//genfit::MeasuredStateOnPlane* gf_state_beam_line_ca = nullptr;
	std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state_beam_line_ca = nullptr;
	try {
		gf_state_beam_line_ca = std::shared_ptr<genfit::MeasuredStateOnPlane>(phgf_track->extrapolateToLine(vertex_position,
					TVector3(0., 0., 1.)));
	} catch (...) {
		if (_verbosity >= 2)
			LogWarning("extrapolateToLine failed!");
	}
	if(!gf_state_beam_line_ca) return nullptr;

	/*!
	 *  1/p, u'/z', v'/z', u, v
	 *  u is defined as momentum X beam line at POCA of the beam line
	 *  v is alone the beam line
	 *  so u is the dca2d direction
	 */

	double u = gf_state_beam_line_ca->getState()[3];
	double v = gf_state_beam_line_ca->getState()[4];

	double du2 = gf_state_beam_line_ca->getCov()[3][3];
	double dv2 = gf_state_beam_line_ca->getCov()[4][4];

	//delete gf_state_beam_line_ca;

	//const SvtxTrack_v1* temp_track = static_cast<const SvtxTrack_v1*> (svtx_track);
	//  SvtxTrack_v1* out_track = new SvtxTrack_v1(
	//      *static_cast<const SvtxTrack_v1*>(svtx_track));
	std::shared_ptr < SvtxTrack_v1 > out_track = std::shared_ptr < SvtxTrack_v1
		> (new SvtxTrack_v1(*static_cast<const SvtxTrack_v1*>(svtx_track)));

	out_track->set_dca2d(u);
	out_track->set_dca2d_error(sqrt(du2 + dvr2));

	std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state_vertex_ca = nullptr;
	try {
		gf_state_vertex_ca = std::shared_ptr < genfit::MeasuredStateOnPlane
			> (phgf_track->extrapolateToPoint(vertex_position));
	} catch (...) {
		if (_verbosity >= 2)
			LogWarning("extrapolateToPoint failed!");
	}
	if (!gf_state_vertex_ca) {
		//delete out_track;
		return nullptr;
	}

	TVector3 mom = gf_state_vertex_ca->getMom();
	TVector3 pos = gf_state_vertex_ca->getPos();
	TMatrixDSym cov = gf_state_vertex_ca->get6DCov();

	//  genfit::MeasuredStateOnPlane* gf_state_vertex_ca =
	//      phgf_track->extrapolateToLine(vertex_position,
	//          TVector3(0., 0., 1.));

	u = gf_state_vertex_ca->getState()[3];
	v = gf_state_vertex_ca->getState()[4];

	du2 = gf_state_vertex_ca->getCov()[3][3];
	dv2 = gf_state_vertex_ca->getCov()[4][4];


	double dca3d = sqrt(u * u + v * v);
	double dca3d_error = sqrt(du2 + dv2 + dvr2 + dvz2);

	out_track->set_dca(dca3d);
	out_track->set_dca_error(dca3d_error);

	/*!
	 * dca3d_xy, dca3d_z
	 */


	/*
	// Rotate from u,v,n to r: n X Z, Z': n X r, n using 5D state/cov
	// commented on 2017-10-09
	TMatrixF pos_in(3,1);
	TMatrixF cov_in(3,3);
	pos_in[0][0] = gf_state_vertex_ca->getState()[3];
	pos_in[1][0] = gf_state_vertex_ca->getState()[4];
	pos_in[2][0] = 0.;
	cov_in[0][0] = gf_state_vertex_ca->getCov()[3][3];
	cov_in[0][1] = gf_state_vertex_ca->getCov()[3][4];
	cov_in[0][2] = 0.;
	cov_in[1][0] = gf_state_vertex_ca->getCov()[4][3];
	cov_in[1][1] = gf_state_vertex_ca->getCov()[4][4];
	cov_in[1][2] = 0.;
	cov_in[2][0] = 0.;
	cov_in[2][1] = 0.;
	cov_in[2][2] = 0.;
	TMatrixF pos_out(3,1);
	TMatrixF cov_out(3,3);
	TVector3 vu = gf_state_vertex_ca->getPlane().get()->getU();
	TVector3 vv = gf_state_vertex_ca->getPlane().get()->getV();
	TVector3 vn = vu.Cross(vv);
	pos_cov_uvn_to_rz(vu, vv, vn, pos_in, cov_in, pos_out, cov_out);
	//! vertex cov in (u',v',n')
	TMatrixF vertex_cov_out(3,3);
	get_vertex_error_uvn(vu,vv,vn, vertex_cov, vertex_cov_out);
	float dca3d_xy = pos_out[0][0];
	float dca3d_z  = pos_out[1][0];
	float dca3d_xy_error = sqrt(cov_out[0][0] + vertex_cov_out[0][0]);
	float dca3d_z_error  = sqrt(cov_out[1][1] + vertex_cov_out[1][1]);
	//Begin DEBUG
	//  LogDebug("rotation debug---------- ");
	//  gf_state_vertex_ca->Print();
	//  LogDebug("dca rotation---------- ");
	//  pos_out = pos_in;
	//  cov_out = cov_in;
	//  pos_in.Print();
	//  cov_in.Print();
	//  pos_out.Print();
	//  cov_out.Print();
	//  cout
	//    <<"dca3d_xy: "<<dca3d_xy <<" +- "<<dca3d_xy_error*dca3d_xy_error
	//    <<"; dca3d_z: "<<dca3d_z<<" +- "<< dca3d_z_error*dca3d_z_error
	//    <<"\n";
	//  gf_state_vertex_ca->get6DCov().Print();
	//  LogDebug("vertex rotation---------- ");
	//  vertex_position.Print();
	//  vertex_cov.Print();
	//  vertex_cov_out.Print();
	//End DEBUG
	*/

	//
	// in: X, Y, Z; out; r: n X Z, Z X r, Z

	float dca3d_xy = NAN;
	float dca3d_z  = NAN;
	float dca3d_xy_error = NAN;
	float dca3d_z_error  = NAN;

	try{
		TMatrixF pos_in(3,1);
		TMatrixF cov_in(3,3);
		TMatrixF pos_out(3,1);
		TMatrixF cov_out(3,3);

		TVectorD state6(6); // pos(3), mom(3)
		TMatrixDSym cov6(6,6); //

		gf_state_vertex_ca->get6DStateCov(state6, cov6);

		TVector3 vn(state6[3], state6[4], state6[5]);

		// mean of two multivariate gaussians Pos - Vertex
		pos_in[0][0] = state6[0] - vertex_position.X();
		pos_in[1][0] = state6[1] - vertex_position.Y();
		pos_in[2][0] = state6[2] - vertex_position.Z();


		for(int i=0;i<3;++i){
			for(int j=0;j<3;++j){
				cov_in[i][j] = cov6[i][j] + vertex_cov[i][j];
			}
		}

		dca3d_xy = pos_out[0][0];
		dca3d_z  = pos_out[2][0];
		dca3d_xy_error = sqrt(cov_out[0][0]);
		dca3d_z_error  = sqrt(cov_out[2][2]);

#ifdef _DEBUG_
		cout<<__LINE__<<": Vertex: ----------------"<<endl;
		vertex_position.Print();
		vertex_cov.Print();

		cout<<__LINE__<<": State: ----------------"<<endl;
		state6.Print();
		cov6.Print();

		cout<<__LINE__<<": Mean: ----------------"<<endl;
		pos_in.Print();
		cout<<"===>"<<endl;
		pos_out.Print();

		cout<<__LINE__<<": Cov: ----------------"<<endl;
		cov_in.Print();
		cout<<"===>"<<endl;
		cov_out.Print();

		cout<<endl;
#endif

	} catch (...) {
		if (_verbosity > 0)
			LogWarning("DCA calculationfailed!");
	}

	out_track->set_dca3d_xy(dca3d_xy);
	out_track->set_dca3d_z(dca3d_z);
	out_track->set_dca3d_xy_error(dca3d_xy_error);
	out_track->set_dca3d_z_error(dca3d_z_error);

	//if(gf_state_vertex_ca) delete gf_state_vertex_ca;

	out_track->set_chisq(chi2);
	out_track->set_ndf(ndf);
	out_track->set_charge(phgf_track->get_charge());

	out_track->set_px(mom.Px());
	out_track->set_py(mom.Py());
	out_track->set_pz(mom.Pz());

	out_track->set_x(pos.X());
	out_track->set_y(pos.Y());
	out_track->set_z(pos.Z());

	for (int i = 0; i < 6; i++) {
		for (int j = i; j < 6; j++) {
			out_track->set_error(i, j, cov[i][j]);
		}
	}

	//  for (SvtxTrack::ConstClusterIter iter = svtx_track->begin_clusters();
	//      iter != svtx_track->end_clusters(); ++iter) {
	//    unsigned int cluster_id = *iter;
	//    SvtxCluster* cluster = _clustermap->get(cluster_id);
	//    if (!cluster) {
	//      LogError("No cluster Found!");
	//      continue;
	//    }
	//    //cluster->identify(); //DEBUG
	//
	//    //unsigned int l = cluster->get_layer();
	//
	//    TVector3 pos(cluster->get_x(), cluster->get_y(), cluster->get_z());
	//
	//    double radius = pos.Pt();
	//
	//    std::shared_ptr<genfit::MeasuredStateOnPlane> gf_state = nullptr;
	//    try {
	//      gf_state = std::shared_ptr < genfit::MeasuredStateOnPlane
	//          > (phgf_track->extrapolateToCylinder(radius,
	//              TVector3(0, 0, 0), TVector3(0, 0, 1), 0));
	//    } catch (...) {
	//      if (Verbosity() >= 2)
	//        LogWarning("Exrapolation failed!");
	//    }
	//    if (!gf_state) {
	//      if (Verbosity() > 1)
	//        LogWarning("Exrapolation failed!");
	//      continue;
	//    }
	//
	//    //SvtxTrackState* state = new SvtxTrackState_v1(radius);
	//    std::shared_ptr<SvtxTrackState> state = std::shared_ptr<SvtxTrackState> (new SvtxTrackState_v1(radius));
	//    state->set_x(gf_state->getPos().x());
	//    state->set_y(gf_state->getPos().y());
	//    state->set_z(gf_state->getPos().z());
	//
	//    state->set_px(gf_state->getMom().x());
	//    state->set_py(gf_state->getMom().y());
	//    state->set_pz(gf_state->getMom().z());
	//
	//    //gf_state->getCov().Print();
	//
	//    for (int i = 0; i < 6; i++) {
	//      for (int j = i; j < 6; j++) {
	//        state->set_error(i, j, gf_state->get6DCov()[i][j]);
	//      }
	//    }
	//
	//    out_track->insert_state(state.get());
	//
	//#ifdef _DEBUG_
	//    cout
	//    <<__LINE__
	//    <<": " << radius <<" => "
	//    <<sqrt(state->get_x()*state->get_x() + state->get_y()*state->get_y())
	//    <<endl;
	//#endif
	//  }

#ifdef _DEBUG_
	cout << __LINE__ << endl;
#endif

	const genfit::Track *gftrack = phgf_track->getGenFitTrack();
	const genfit::AbsTrackRep *rep = gftrack->getCardinalRep();
	for(unsigned int id = 0; id< gftrack->getNumPointsWithMeasurement();++id) {
		genfit::TrackPoint *trpoint = gftrack->getPointWithMeasurementAndFitterInfo(id, gftrack->getCardinalRep());

		if(!trpoint) {
			if (_verbosity > 1)
				LogWarning("!trpoint");
			continue;
		}

		genfit::KalmanFitterInfo* kfi = static_cast<genfit::KalmanFitterInfo*>( trpoint->getFitterInfo(rep) );
		if(!kfi) {
			if (_verbosity > 1)
				LogWarning("!kfi");
			continue;
		}

		std::shared_ptr<const genfit::MeasuredStateOnPlane> gf_state = nullptr;
		try {
			//gf_state = std::shared_ptr <genfit::MeasuredStateOnPlane> (const_cast<genfit::MeasuredStateOnPlane*> (&(kfi->getFittedState(true))));
			const genfit::MeasuredStateOnPlane* temp_state = &(kfi->getFittedState(true));
			gf_state = std::shared_ptr <genfit::MeasuredStateOnPlane> (new genfit::MeasuredStateOnPlane(*temp_state));
		} catch (...) {
			if (_verbosity > 1)
				LogWarning("Exrapolation failed!");
		}
		if (!gf_state) {
			if (_verbosity > 1)
				LogWarning("Exrapolation failed!");
			continue;
		}
		genfit::MeasuredStateOnPlane temp;
		float pathlength = -phgf_track->extrapolateToPoint(temp,vertex_position,id);

		std::shared_ptr<SvtxTrackState> state = std::shared_ptr<SvtxTrackState> (new SvtxTrackState_v1(pathlength));
		state->set_x(gf_state->getPos().x());
		state->set_y(gf_state->getPos().y());
		state->set_z(gf_state->getPos().z());

		state->set_px(gf_state->getMom().x());
		state->set_py(gf_state->getMom().y());
		state->set_pz(gf_state->getMom().z());

		//gf_state->getCov().Print();

		for (int i = 0; i < 6; i++) {
			for (int j = i; j < 6; j++) {
				state->set_error(i, j, gf_state->get6DCov()[i][j]);
			}
		}

		out_track->insert_state(state.get());

#ifdef _DEBUG_
		cout
			<<__LINE__
			<<": " << id
			<<": " << pathlength <<" => "
			<<sqrt(state->get_x()*state->get_x() + state->get_y()*state->get_y())
			<<endl;
#endif
	}

	return out_track;
}
