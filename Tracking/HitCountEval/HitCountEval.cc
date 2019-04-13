/*!
 *  \file		HitCountEval.cc
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
#include "HitCountEval.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <fun4all/PHTFileServer.h>

#include <TTree.h>
#include <TH1F.h>
#include <iostream>

const char *const HitCountEval::hit_containers[10] {"G4HIT_EGEM_0", "G4HIT_EGEM_1", "G4HIT_EGEM_3", 
	"G4HIT_FGEM_0", "G4HIT_FGEM_1", "G4HIT_FGEM_2", "G4HIT_FGEM_3","G4HIT_FGEM_4", "G4HIT_MAPS", 
	"G4HIT_SVTX"};


#define NELEMS(a) (sizeof(a)/sizeof(a[0]))

HitCountEval::HitCountEval(const std::string &name, const std::string &file_name):
	SubsysReco {name},
	output_file_name {file_name},
	event {0}
{
}


//----------------------------------------------------------------------------//
//-- Init():
//--   Intialize all histograms, trees, and ntuples
//----------------------------------------------------------------------------//
int HitCountEval::Init(PHCompositeNode *topNode) {
	std::cout << PHWHERE << " Openning file " << output_file_name << std::endl;
	PHTFileServer::get().open(output_file_name, "RECREATE");

	// create TTree
	for (size_t i {0}; i < NELEMS(hit_containers); ++i) {
		hit_count[i] = new TH1F {(std::string(hit_containers[i]) + "-hits").c_str(), 
			"Hit Count", 100, -4.5, 4.5};
		eta_count[i] = new TH1F {(std::string(hit_containers[i]) + "-etas").c_str(), 
			"Eta Count", 100, -4.5, 4.5};

		hits[i] = new TTree {hit_containers[i], ""};
		hits[i]->Branch("hit_id", &hit_id, "hit_id/l");
		hits[i]->Branch("x", &hit_x, "x/F");
		hits[i]->Branch("y", &hit_y, "y/F");
		hits[i]->Branch("z", &hit_z, "z/F");
		hits[i]->Branch("px", &hit_px, "px/F");
		hits[i]->Branch("py", &hit_py, "py/F");
		hits[i]->Branch("pz", &hit_pz, "pz/F");
		hits[i]->Branch("particle_initial_px", &particle_initial_px, "particle_initial_px/F");
		hits[i]->Branch("particle_initial_py", &particle_initial_py, "particle_initial_py/F");
		hits[i]->Branch("particle_initial_pz", &particle_initial_pz, "particle_initial_pz/F");
		hits[i]->Branch("layer", &hit_layer, "hit_layer/i");
		hits[i]->Branch("event", &hit_event, "event/i");

		normalized_hits[i] = new TTree {(std::string(hit_containers[i]) + "_normalized").c_str(), ""};
		normalized_hits[i]->Branch("eta", &normalized_hit_eta, "eta/D");
		normalized_hits[i]->Branch("hit_count", &normalized_hit_count, "eta/D");
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- process_event():
//--   Call user instructions for every event.
//--   This function contains the analysis structure.
//----------------------------------------------------------------------------//
int HitCountEval::process_event(PHCompositeNode *const topNode) {
	event++;
	if (verbosity >= 2 and event % 1000 == 0)
		std::cout << PHWHERE << "Events processed: " << event << std::endl;

	const auto truth = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
	const auto particle_range = truth->GetPrimaryParticleRange();
	for (auto it = particle_range.first; it != particle_range.second; ++it) {
		PHG4Particle const* g4particle{it->second};
		if(!g4particle) {
			std::cerr << "Missing PHG4Particle\n";
			continue;
		}

		particle_initial_px =  g4particle->get_px();
		particle_initial_py =  g4particle->get_py();
		particle_initial_pz =  g4particle->get_pz();

		const double mom {std::sqrt(particle_initial_px * particle_initial_px 
				+ particle_initial_py * particle_initial_py 
				+ particle_initial_pz * particle_initial_pz)};
		const double eta {0.5 *std::log((mom + particle_initial_pz) / (mom - particle_initial_pz))};
		for (size_t i {0}; i < NELEMS(hit_containers); ++i) {
			eta_count[i]->Fill(eta);
			const auto h = findNode::getClass<PHG4HitContainer>(topNode, hit_containers[i]);
			if (!h) {
				std::cerr << "Couldn't find " << hit_containers[i] << '\n';
				continue;
			}

			const auto hit_range = h->getHits();
			for (auto hp = hit_range.first; hp != hit_range.second; ++hp) {
				PHG4Hit *const hit {hp->second};
				for (int j {0}; j < 2; ++j) {
					hit_count[i]->Fill(eta);
					hit_id = hit->get_hit_id();
					hit_x = hit->get_x(j);
					hit_y = hit->get_y(j);
					hit_z = hit->get_z(j);
					hit_px = hit->get_px(j);
					hit_py = hit->get_py(j);
					hit_pz = hit->get_pz(j);
					hits[i]->Fill();
				}
			}
		}
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

int HitCountEval::End(PHCompositeNode *topNode) {
	PHTFileServer::get().cd(output_file_name);

	for (size_t i {0}; i < NELEMS(hit_containers); ++i) {
		const Long64_t nbins {hit_count[i]->GetSize() - 2};	/* -2 for underflow and overflow */
		for (Long64_t j {0}; j < nbins; ++j) {
			if (eta_count[i]->GetBinContent(j + 1) != 0) {
				const Double_t n {hit_count[i]->GetBinContent(j + 1)};
				const Double_t N {eta_count[i]->GetBinContent(j + 1)};

				normalized_hit_eta = hit_count[i]->GetBinCenter(j + 1);
				normalized_hit_count = n / N;
				normalized_hits[i]->Fill();

			}
			normalized_hits[i]->Write();
			hits[i]->Write();
		}
	}


	return Fun4AllReturnCodes::EVENT_OK;
}

void HitCountEval::set_filename(const char *const file) { 
	if (file) 
		output_file_name = file; 
}
