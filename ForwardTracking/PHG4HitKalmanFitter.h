/*!
 *  \file		PHG4HitKalmanFitter.h
 *  \brief		Kalman Filter based on smeared truth PHG4Hit
 *  \details	Kalman Filter based on smeared truth PHG4Hit
 *  \author		Haiwang Yu <yuhw@nmsu.edu>
 */

#ifndef __PHG4HitKalmanFitter_H__
#define __PHG4HitKalmanFitter_H__

#include <iostream>
#include <string>
#include <vector>

#include <fun4all/SubsysReco.h>

#include <TVector3.h>

#include <g4main/PHG4HitContainer.h>
#include <phgenfit/Measurement.h>

class PHG4Particle;
namespace PHGenFit
{
  class PlanarMeasurement;
} /* namespace PHGenFit */

namespace PHGenFit
{
  class Track;
} /* namespace PHGenFit */

namespace genfit
{
  class GFRaveVertexFactory;
} /* namespace genfit */

class SvtxTrack;
namespace PHGenFit
{
  class Fitter;
} /* namespace PHGenFit */

class SvtxTrackMap;
class SvtxVertexMap;
class SvtxVertex;
class PHCompositeNode;
class PHG4TruthInfoContainer;
class SvtxClusterMap;
class SvtxEvalStack;
class TFile;
class TTree;

class PHG4TrackFastSim : public SubsysReco
{
 public:
  //! Default constructor
  PHG4TrackFastSim(const std::string& name = "PHG4HitKalmanFitter");

  //! dtor
  ~PHG4TrackFastSim();

  //!Initialization, called for initialization
  int Init(PHCompositeNode*);

  //!Initialization Run, called for initialization of a run
  int InitRun(PHCompositeNode*);

  //!Process Event, called for each event
  int process_event(PHCompositeNode*);

  //!End, write and close files
  int End(PHCompositeNode*);

  /// set verbosity
  void Verbosity(int verb)
  {
    verbosity = verb;  // SubsysReco verbosity
  }

  bool is_do_evt_display() const
  {
    return _do_evt_display;
  }

  void set_do_evt_display(bool doEvtDisplay)
  {
    _do_evt_display = doEvtDisplay;
  }

  double get_FGEM_phi_resolution() const
  {
    return _phi_resolution;
  }

  void set_FGEM_phi_resolution(double fgemPhiResolution)
  {
    _phi_resolution = fgemPhiResolution;
  }

  double get_FGEM_r_resolution() const
  {
    return _r_resolution;
  }

  void set_FGEM_r_resolution(double fgemRResolution)
  {
    _r_resolution = fgemRResolution;
  }

  const std::string& get_fit_alg_name() const
  {
    return _fit_alg_name;
  }

  void set_fit_alg_name(const std::string& fitAlgName)
  {
    _fit_alg_name = fitAlgName;
  }

  double get_pat_rec_hit_finding_eff() const
  {
    return _pat_rec_hit_finding_eff;
  }

  void set_pat_rec_hit_finding_eff(double patRecHitFindingEff)
  {
    if (!(patRecHitFindingEff >= 0 && patRecHitFindingEff <= 1))
    {
      std::cout << "ERROR: _pat_rec_hit_finding_eff out of range! \n";
    }
    _pat_rec_hit_finding_eff = patRecHitFindingEff;
  }

  double get_pat_rec_nosise_prob() const
  {
    return _pat_rec_nosise_prob;
  }

  void set_pat_rec_nosise_prob(double patRecNosiseProb)
  {
    if (!(patRecNosiseProb <= 1. && patRecNosiseProb >= 0))
    {
      std::cout << "ERROR: _pat_rec_nosise_prob out of range! \n";
      return;
    }
    _pat_rec_nosise_prob = patRecNosiseProb;
  }

 private:
  /*!
	 * Create needed nodes.
	 */
  int CreateNodes(PHCompositeNode*);

  /*!
	 * Get all the all the required nodes off the node tree.
	 */
  int GetNodes(PHCompositeNode*);

  /*!
	 *
	 */
  int PseudoPatternRecognition(const PHG4Particle* particle,
                               std::vector<PHGenFit::Measurement*>& meas_out, TVector3& seed_pos,
                               TVector3& seed_mom, TMatrixDSym& seed_cov, const bool do_smearing = true);

  PHGenFit::PlanarMeasurement* PHG4HitToMeasurementVerticalPlane(const PHG4Hit* g4hit);

  PHGenFit::PlanarMeasurement* VertexMeasurement(const TVector3& vtx, const double dr,
                                                 const double dphi);

  /*!
	 * Make SvtxTrack from PHGenFit::Track
	 */
  SvtxTrack* MakeSvtxTrack(const PHGenFit::Track* phgf_track_in);

  //! Event counter
  int _event;

  //! Input Node pointers
  PHG4TruthInfoContainer* _truth_container;

  std::vector<PHG4HitContainer*> _phg4hits;
  std::vector<std::string> _phg4hits_names;

  //! Output Node pointers
  SvtxTrackMap* _trackmap_out;

  /*!
	 *	GenFit fitter interface
	 */
  PHGenFit::Fitter* _fitter;

  //!
  std::string _fit_alg_name;

  //!
  bool _do_evt_display;

  /*!
	 * For PseudoPatternRecognition function.
	 */

  double _phi_resolution;

  double _r_resolution;

  //!
  double _pat_rec_hit_finding_eff;

  //!
  double _pat_rec_nosise_prob;
};

#endif /*__PHG4HitKalmanFitter_H__*/
