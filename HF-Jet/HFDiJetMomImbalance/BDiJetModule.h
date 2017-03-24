/*!
 *  \file		BDiJetModule.h
 *  \brief		Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details	Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author		Dennis V. Perepelitsa
 */

#ifndef __BDiJetModule_H__
#define __BDiJetModule_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>

#include "TMath.h"

#include "TTree.h"
#include "TFile.h"

class PHCompositeNode;

class BDiJetModule: public SubsysReco {

public:

	BDiJetModule(const std::string &name = "BDiJetModule");

	int Init(PHCompositeNode*);
	int process_event(PHCompositeNode*);
	int End(PHCompositeNode*);

private:

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


	std::string _foutname;

};

#endif // __BDiJetModule_H__
