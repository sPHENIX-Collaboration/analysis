/*!
 *  \file		GenFitTrackProp.h
 *  \brief		Example module to extrapolate SvtxTrack with GenFit swiming
 *  \author		Haiwang Yu <yuhw@nmsu.edu>
 */

#ifndef __GenFitTrackProp_H__
#define __GenFitTrackProp_H__



#include <fun4all/SubsysReco.h>

//#include <g4eval/SvtxEvalStack.h>

#include <string>
//#include <memory>

//Forward declerations

class PHCompositeNode;

class PHG4TruthInfoContainer;

class SvtxEvalStack;

class SvtxClusterMap;
class SvtxTrackMap;

class TFile;
class TTree;
class TH2D;

namespace PHGenFit {
	class Fitter;
}


//Brief: basic ntuple and histogram creation for sim evaluation
class GenFitTrackProp: public SubsysReco
{
 public: 
  //!Default constructor
  GenFitTrackProp(const std::string &name="GenFitTrackProp", const int pid_guess = 211);

  //!Initialization, called for initialization
  int Init(PHCompositeNode *);

  int InitRun(PHCompositeNode *);

  //!Process Event, called for each event
  int process_event(PHCompositeNode *);

  //!End, write and close files
  int End(PHCompositeNode *);

  //! Change output filename
  void set_filename(const char* file)
  { if(file) _outfile_name = file; }

	int get_pid_guess() const {
		return _pid_guess;
	}

	void set_pid_guess(int pidGuess) {
		_pid_guess = pidGuess;
	}

 private:

  //!Get all the nodes
  int GetNodes(PHCompositeNode *);

  void fill_tree(PHCompositeNode*);
  void reset_variables();

 private:

  /*!
   *  Input
   */
  
  //! Node pointers
  PHG4TruthInfoContainer* _truth_container;
  SvtxTrackMap* _trackmap;

  /*!
   *  Processing
   */

  //! event processed
  int _event;

  //!

  SvtxEvalStack* _svtxevalstack;

  //! Needed to initialize genfit::FieldManager and genfit::MaterialEffect
  PHGenFit::Fitter* _fitter;

  std::string _track_fitting_alg_name;

  bool _do_evt_display;

  //!
  int _pid_guess;

  /*!
   *  Output
   */

  //!output filename
  std::string _outfile_name;

  //! Eval TTrees
  TTree* _eval_tree_tracks;

  int event;

  //! truth track info
  int gtrackID;
  int gflavor;

  double gpx;
  double gpy;
  double gpz;

  double gpt;
  double geta;

  double gvx;
  double gvy;
  double gvz;

  //! reco info
  int trackID;
  int charge;
  int nhits;

  double px;
  double py;
  double pz;
  double pt;

  double dca2d;

  double radius80;
  double pathlength80;
  double pathlength85;

};

#endif //* __GenFitTrackProp_H__ *//
