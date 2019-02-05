#include <trackbase_historic/SvtxCluster.h>
#include <trackbase_historic/SvtxClusterMap.h>
#include <trackbase_historic/SvtxHitMap.h>
#include <trackbase_historic/SvtxHit_v1.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackMap_v1.h>
#include <trackbase_historic/SvtxTrackState_v1.h>
#include <trackbase_historic/SvtxTrack_v1.h>
#include <trackbase_historic/SvtxVertexMap_v1.h>
#include <trackbase_historic/SvtxVertex_v1.h>
#include "RaveVertexingAux.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/SpacepointMeasurement.h>
#include <phgenfit/Track.h>

#include <phfield/PHFieldUtility.h>
#include <phgeom/PHGeomUtility.h>

#include <GenFit/FieldManager.h>
#include <GenFit/KalmanFitterInfo.h>
#include <GenFit/MeasuredStateOnPlane.h>
#include <GenFit/RKTrackRep.h>
#include <GenFit/StateOnPlane.h>
#include <GenFit/Track.h>

//Rave
/*#include <rave/ConstantMagneticField.h>
#include <rave/Track.h>
#include <rave/Version.h>
#include <rave/VertexFactory.h>
#include <GenFit/GFRaveConverters.h>
*/

//GenFit

#include <TClonesArray.h>
#include <TMatrixDSym.h>
#include <TRandom3.h>
#include <TRotation.h>
#include <TTree.h>
#include <TVector3.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#define LogDebug(exp) std::cout << "DEBUG: " << __FILE__ << ": " << __LINE__ << ": " << exp << std::endl
#define LogError(exp) std::cout << "ERROR: " << __FILE__ << ": " << __LINE__ << ": " << exp << std::endl
#define LogWarning(exp) std::cout << "WARNING: " << __FILE__ << ": " << __LINE__ << ": " << exp << std::endl

//#define _DEBUG_

using namespace std;

/*
 * Constructor
 */
  RaveVertexingAux::RaveVertexingAux(PHCompositeNode* topNode)
  : _fitter(NULL)
  , _primary_pid_guess(22)
  , _vertex_min_ndf(20)
    , _vertex_finder(NULL)
    , _vertexing_method("avf-smoothing:1")
{
	TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);
	PHField* field = PHFieldUtility::GetFieldMapNode(nullptr, topNode);

	_fitter = PHGenFit::Fitter::getInstance(tgeo_manager,
			field, "DafRef",
			"RKTrackRep", false);
	_fitter->set_verbosity(0);
	if (!_fitter)
	{
		cerr << "critical error: RaveVertexingAux has no fitter" << endl;
    _noErrors=false;
	}
	_vertex_finder = new genfit::GFRaveVertexFactory(10);
	_vertex_finder->setMethod(_vertexing_method.data());
	if (!_vertex_finder)
	{
		cerr << "critical error: RaveVertexingAux has no vertexer" << endl;
    _noErrors=false;
	}
}

/*TVector3 RaveVertexingAux::getHelix(SvtxTrack *t1){
  auto genfit_track1 = TranslateSvtxToGenFitTrack(t1);

}*/

SvtxVertex* RaveVertexingAux::makeVtx(SvtxTrack *t1, SvtxTrack *t2)
{
  SvtxVertex_v1* svtx_vtx=new SvtxVertex_v1();
  vector<genfit::Track*> gf_tracks;
  if (t1->get_ndf() < _vertex_min_ndf||t2->get_ndf() < _vertex_min_ndf){//ndf cut
    cout<<"ndf cut \n";
    delete svtx_vtx;
    return NULL; 
  } 

  auto genfit_track1 = TranslateSvtxToGenFitTrack(t1);
  auto genfit_track2 = TranslateSvtxToGenFitTrack(t2);
  if (!genfit_track1 || !genfit_track2){//geomentry cut
    cout<<"geo cut \n";
    delete svtx_vtx;
    return NULL; 
  } 
  gf_tracks.push_back(const_cast<genfit::Track*>(genfit_track1));
  gf_tracks.push_back(const_cast<genfit::Track*>(genfit_track2));

  vector<genfit::GFRaveVertex*> rave_vertices;
  if (gf_tracks.size() >= 2)
  {
    try
    {
      _vertex_finder->findVertices(&rave_vertices, gf_tracks);
    }
    catch(const std::exception &exc){
      if (Verbosity() > 1)
        std::cerr << exc.what();
      delete svtx_vtx;
      return NULL;
    }
    catch (...)
    {
      if (Verbosity() > 1)
        std::cout << "RaveVertexingAux: GFRaveVertexFactory::findVertices failed!\n";
      delete svtx_vtx;
      return NULL;
    }
  }

  for (genfit::GFRaveVertex* rave_vtx : rave_vertices)
  {
    if (!rave_vtx)
    {
      std::cout << "RaveVertexingAux: GFRaveVertexFactory::findVertices failed!\n";
      delete svtx_vtx;
      return NULL;
    }

    svtx_vtx->set_chisq(rave_vtx->getChi2());
    svtx_vtx->set_ndof(rave_vtx->getNdf());
    svtx_vtx->set_position(0, rave_vtx->getPos().X());
    svtx_vtx->set_position(1, rave_vtx->getPos().Y());
    svtx_vtx->set_position(2, rave_vtx->getPos().Z());

    //fill the error matrix
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
        svtx_vtx->set_error(i, j, rave_vtx->getCov()[i][j]);

  }  //loop over RAVE vertices
  return svtx_vtx;
}

/*
 * dtor
 */
RaveVertexingAux::~RaveVertexingAux()
{
  if(_fitter) delete _fitter;
  if(_vertex_finder) delete _vertex_finder;
}

genfit::Track* RaveVertexingAux::TranslateSvtxToGenFitTrack(SvtxTrack* svtx_track)
{
  try
  {
    // The first state is extracted to PCA, second one is the one with measurement
    SvtxTrackState* svtx_state = (++(svtx_track->begin_states()))->second;
    //SvtxTrackState* svtx_state = (svtx_track->begin_states())->second;

    TVector3 pos(svtx_state->get_x(), svtx_state->get_y(), svtx_state->get_z());
    TVector3 mom(svtx_state->get_px(), svtx_state->get_py(), svtx_state->get_pz());
    TMatrixDSym cov(6);
    for (int i = 0; i < 6; ++i)
    {
      for (int j = 0; j < 6; ++j)
      {
        cov[i][j] = svtx_state->get_error(i, j);
      }
    }

#ifdef _DEBUG_
    {
      cout << "DEBUG" << __LINE__ << endl;
      cout << "path length:      " << svtx_state->get_pathlength() << endl;
      cout << "radius:           " << pos.Perp() << endl;
      cout << "DEBUG: " << __LINE__ << endl;
      for (int i = 0; i < 6; ++i)
      {
        for (int j = 0; j < 6; ++j)
        {
          cout << svtx_state->get_error(i, j) << "\t";
        }
        cout << endl;
      }

      cov.Print();
    }

#endif

    genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_primary_pid_guess);
    genfit::Track* genfit_track = new genfit::Track(rep, TVector3(0, 0, 0), TVector3(0, 0, 0));

    genfit::FitStatus* fs = new genfit::FitStatus();
    fs->setCharge(svtx_track->get_charge());
    fs->setChi2(svtx_track->get_chisq());
    fs->setNdf(svtx_track->get_ndf());
    fs->setIsFitted(true);
    fs->setIsFitConvergedFully(true);

    genfit_track->setFitStatus(fs, rep);

    genfit::TrackPoint* tp = new genfit::TrackPoint(genfit_track);

    genfit::KalmanFitterInfo* fi = new genfit::KalmanFitterInfo(tp, rep);
    tp->setFitterInfo(fi);

    genfit::MeasuredStateOnPlane* ms = new genfit::MeasuredStateOnPlane(rep);
    ms->setPosMomCov(pos, mom, cov);
#ifdef _DEBUG_
    {
      cout << "DEBUG: " << __LINE__ << endl;
      ms->Print();
      cout << "Orig: " << __LINE__ << endl;
      cov.Print();
      cout << "Translate: " << __LINE__ << endl;
      ms->get6DCov().Print();
    }
#endif
    genfit::KalmanFittedStateOnPlane* kfs = new genfit::KalmanFittedStateOnPlane(*ms, 1., 1.);

    //< Acording to the special order of using the stored states
    fi->setForwardUpdate(kfs);

    genfit_track->insertPoint(tp);

#ifdef _DEBUG_
    //		{
    //			cout << "DEBUG" << __LINE__ << endl;
    //			TVector3 pos, mom;
    //			TMatrixDSym cov;
    //			genfit_track->getFittedState().getPosMomCov(pos, mom, cov);
    //			pos.Print();
    //			mom.Print();
    //			cov.Print();
    //		}
#endif

    return genfit_track;
  } //try block
  catch (...)
  {
    LogDebug("TranslateSvtxToGenFitTrack failed!");
  }

  return nullptr;
}



