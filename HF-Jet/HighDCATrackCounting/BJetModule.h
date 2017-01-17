/*!
 *  \file		BJetModule.h
 *  \brief		Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details	Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author		Dennis V. Perepelitsa
 */

#ifndef __BJETMODULE_H__
#define __BJETMODULE_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>

#include "TMath.h"

#include "TTree.h"
#include "TFile.h"

class PHCompositeNode;

class BJetModule: public SubsysReco {

public:

	BJetModule(const std::string &name = "BJetModule");

	int Init(PHCompositeNode*);
	int process_event(PHCompositeNode*);
	int End(PHCompositeNode*);

private:

	float dR(float eta1, float eta2, float phi1, float phi2) {

		float deta = eta1 - eta2;
		float dphi = phi1 - phi2;
		if (dphi > +3.14159)
			dphi -= 2 * 3.14159;
		if (dphi < -3.14159)
			dphi += 2 * 3.14159;

		return sqrt(pow(deta, 2) + pow(dphi, 2));

	}

	bool _verbose;

	int _ievent;

	TFile *_f;
	TTree *_tree;

	int _b_event;

	int _b_truthjet_n;
	int _b_truthjet_parton_flavor[10];
	int _b_truthjet_hadron_flavor[10];
	float _b_truthjet_pt[10];
	float _b_truthjet_eta[10];
	float _b_truthjet_phi[10];

	int _b_particle_n;
	float _b_particle_pt[1000];
	float _b_particle_eta[1000];
	float _b_particle_phi[1000];
	int _b_particle_pid[1000];
	unsigned int _b_particle_embed[1000];
	float _b_particle_dca[1000];

	int _b_track_n;
	float _b_track_pt[1000];
	float _b_track_eta[1000];
	float _b_track_phi[1000];
	unsigned int _b_track_nclusters[1000];

	float _b_track_dca2d[1000];
	float _b_track_dca2d_error[1000];
	float _b_track_dca2d_calc[1000];
	float _b_track_dca2d_calc_truth[1000];
	float _b_track_dca3d_calc[1000];
	float _b_track_dca3d_calc_truth[1000];

	float _b_track_dca2d_phi[1000];

	float _b_track_quality[1000];
	float _b_track_chisq[1000];
	int _b_track_ndf[1000];

	unsigned int _b_track_best_nclusters[1000];
	unsigned int _b_track_best_embed[1000];
	bool _b_track_best_primary[1000];
	int _b_track_best_pid[1000];

	int _b_track_best_in[1000];
	int _b_track_best_out[1000];
	int _b_track_best_parent_pid[1000];
	float _b_track_best_dca[1000];

	//float _b_track_particle_pt[100];
	//float _b_track_particle_eta[100];
	//float _b_track_particle_phi[100];
	//int _b_track_particle_pid[100];

	std::string _foutname;

};

#endif // __BJETMODULE_H__
