// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef HERWIGPRODUCTIONQAMODULE_H
#define HERWIGPRODUCTIONQAMODULE_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <math.h>
#include <array>
#include <vector>
#include <TH1.h>
#include <TH2.h>

class PHCompositeNode;

class HerwigProductionQAModule : public SubsysReco
{
 public:

  HerwigProductionQAModule(const std::string &name = "HerwigProductionQAModule");

  ~HerwigProductionQAModule() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;

 private:
	//jet specific QA plots
  	std::vector<TH1F*> h_all_jets_pt {};
	std::vector<TH1F*> h_all_jets_eta {};
	std::vector<TH1F*> h_all_jets_phi {};
	std::vector<TH1F*> h_all_jets_e {};
	std::vector<TH1I*> h_all_jets_n_comp {};
	
	std::vector<TH1F*> h_lead_jets_pt {};
	std::vector<TH1F*> h_lead_jets_eta {};
	std::vector<TH1F*> h_lead_jets_phi {};
	std::vector<TH1F*> h_lead_jets_e {};
	std::vector<TH1I*> h_lead_jets_n_comp {};

	//photon specific QA plots 
	TH1F* h_all_photons_pt;	
	TH1F* h_all_photons_eta;	
	TH1F* h_all_photons_phi;	
	TH1F* h_all_photons_e;

	TH1F* h_lead_photons_pt;	
	TH1F* h_lead_photons_eta;	
	TH1F* h_lead_photons_phi;	
	TH1F* h_lead_photons_e;
		
	TH1F* h_direct_photons_pt;	
	TH1F* h_direct_photons_eta;	
	TH1F* h_direct_photons_phi;	
	TH1F* h_direct_photons_e;

	TH1F* h_frag_photons_pt;	
	TH1F* h_frag_photons_eta;	
	TH1F* h_frag_photons_phi;	
	TH1F* h_frag_photons_e;

	TH1I* h_n_photons;
	TH1I* h_n_frag;
	TH1I* h_n_direct;
	std::vector<TH1I*> h_n_jets {};
	//Photon-Jet comparisons
	std::vector<TH2F*> h_photon_jet_pt {};
	std::vector<TH2F*> h_photon_jet_eta {};
	std::vector<TH2F*> h_photon_jet_phi {};
	std::vector<TH2F*> h_photon_jet_e {};
	std::vector<TH1F*> h_photon_jet_dphi {};

	//Event categorization in final states
	TH1F* h_particle_eta;
	TH1F* h_particle_phi;
	TH1F* h_particle_e;
	TH1F* h_particle_pt;
	TH1F* h_particle_et;

	TH2F* h_particle_et_eta;
	TH2F* h_particle_et_phi;
	TH2F* h_particle_pt_eta;
	TH2F* h_particle_pt_phi;
	TH2F* h_particle_e_phi;

	TH2F* h_electron_phi_eta;
	TH1F* h_electron_pt;
		
	TH2F* h_protron_phi_eta;
	TH1F* h_protron_pt;

	TH2F* h_neutron_phi_eta;
	TH1F* h_neutron_pt;

	TH2F* h_pion_phi_eta;
	TH1F* h_pion_pt;

	TH1F* h_electron_n;
	TH1F* h_proton_n;
	TH1F* h_neutron_n;
	TH1F* h_pion_n;
	TH1F* h_total_E;


};

#endif // HERWIGPRODUCTIONQAMODULE_H
