/*!
 *  \file		HitCountEval.h
 *  \brief		Evaluation module for PHG4TrackFastSim output
 *  \details	 input: PHG4TruthInfoContainer G4TruthInfo
 *  			PHG4HitContainer G4HIT_EGEM_0
 *  			PHG4HitContainer G4HIT_EGEM_1 
 *  			PHG4HitContainer G4HIT_EGEM_3
 *			PHG4HitContainer G4HIT_FGEM_0
 *			PHG4HitContainer G4HIT_FGEM_1
 *			PHG4HitContainer G4HIT_FGEM_2
 *		        PHG4HitContainer G4HIT_FGEM_3
 *			PHG4HitContainer G4HIT_FGEM_4
 *			PHG4HitContainer G4HIT_MAPS
 *			PHG4HitContainer G4HIT_SVTX
 *  \author		Giorgian Borca-Tasciuc <giorgian.borca-tasciuc@stonybrook.edu>
 */

#ifndef __HitCountEval_H__
#define __HitCountEval_H__

#include <string>
#include <TTree.h>
#include <TH1F.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <fun4all/SubsysReco.h>

#define NHITCONTAINERS 10

#define NELEMS(a) (sizeof(a)/sizeof(a[0]))
class HitCountEval: public SubsysReco
{
	public: 
		HitCountEval(const std::string &name = "HitCountEval", 
			     const std::string &file_name = "g4eval.root");
		int Init(PHCompositeNode *const);
		int process_event(PHCompositeNode *const);
		int End(PHCompositeNode *const);
		void set_filename(const char *const file);
	private:
		std::string output_file_name;

		unsigned event;
		static const char *const hit_containers[NHITCONTAINERS]; 

		TTree *hits[NELEMS(HitCountEval::hit_containers)];
		TTree *normalized_hits[NELEMS(HitCountEval::hit_containers)];
		TH1F *hit_count[NELEMS(HitCountEval::hit_containers)];
		TH1F *eta_count[NELEMS(HitCountEval::hit_containers)];

		/* For Trees */
		Float_t hit_x;
		Float_t hit_y;
		Float_t hit_z;
		Float_t hit_px;
		Float_t hit_py;
		Float_t hit_pz;
		Float_t particle_initial_px;
		Float_t particle_initial_py;
		Float_t particle_initial_pz;

		Int_t hit_id;
		Int_t hit_layer;
		Int_t hit_event;

		Double_t normalized_hit_eta;
		Double_t normalized_hit_count;
};

#endif //* __HitCountEval_H__ *//
