// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef LARGERLENC_H
#define LARGERLENC_H
//fun4all
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>

//phool 
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//Calo towers 
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfo.h>
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeomContainer_Cylinderv1.h>

//G4 opbjects
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4TruthInfoContainer.h>

//jetbase objetcts 
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jet.h> 
#include <jetbase/TowerJetInput.h>
#include <jetbase/ClusterJetInput.h>

//fastjet
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

//vertex stuff
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

//c++
#include <thread>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <math.h>
#include <array>
#include <iostream>
#include <fstream>
//root
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TInterpreter.h>
//Homebrews 
#include <calorimetertowerenc/MethodHistograms.h> 
#include <DijetEventCuts.h>

#define PI 3.14159265358979323464
class PHCompositeNode;
class Jet;
class JetContainer;
class PHG4Hit;
class PHG4Particle;
class PHG4TruthInfoContainer;

struct DijetQATypePlots{
	DijetQATypePlots(){
		bad_occ_em_oh_rat=new TH2F("h_bad_occupancy_EM_OH_rat", "Occupancy for events that fail the OHCAL ratio cut; #% Towers #geq 10 MeV EMCAL; #% Towers > 10 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h_rat=new TH2F("h_bad_occupancy_EM_H_rat", "Occupancy for events that fail OHCAL ratio cut; #% Towers #geq 10 MeV EMCAL; (#% Towers #geq 10 MeV OHCAL + #% Towers #geq 10 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_oh=new TH2F("h_bad_occupancy_EM_OH", "Occupancy for events that pass  OHCAL ratio cut but otherwise fail; #% Towers #geq 10 MeV EMCAL; #% Towers > 10 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h=new TH2F("h_bad_occupancy_EM_H", "Occupancy for events that pass OHCAL ratio cut but otherwise fail; #% Towers #geq 10 MeV EMCAL; (#% Towers #geq 10 MeV OHCAL + #% Towers #geq 10 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		ohcal_bad_hits=new TH2F("h_ohcal_bad_hits", "Energy depositon for events that fail the OHCAL ratio cut; #eta; #varphi; E [GeV]", 24, -1.1, 1.1, 64, -PI, PI);
		emcal_occup=new TH1F("h_EMCAL_occupancy", "Occupancy of EMCAL all events; #% Towers #geq 10 MeV EMCAL; N_{evts}", 100, -0.005, 0.995);
		ihcal_occup=new TH1F("h_IHCAL_occupancy", "Occupancy of IHCAL all events; #% Towers #geq 10 MeV IHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_occup=new TH1F("h_OHCAL_occupancy", "Occupancy of OHCAL all events; #% Towers #geq 10 MeV OHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_rat_occup=new TH2F("h_OHCAL_rat", "Occupancy of OHCAL as function of OHCAL ratio; #frac{E_{OHCAL}}{E_{ALL CALS}}; #% Towers #geq 500 MeV; N_{evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
	}
	TH2F* bad_occ_em_oh_rat=nullptr;
	TH2F* bad_occ_em_h_rat=nullptr;
	TH2F* bad_occ_em_oh=nullptr;
	TH2F* bad_occ_em_h=nullptr;
	TH2F* ohcal_bad_hits=nullptr;
	TH2F* ohcal_rat_occup=nullptr;
	TH1F* emcal_occup=nullptr;
	TH1F* ihcal_occup=nullptr;
	TH1F* ohcal_occup=nullptr;
	TH1F* ohcal_rat_h=nullptr;
	std::vector<TH2F*> QA_2D_plots {bad_occ_em_oh_rat, bad_occ_em_h_rat, bad_occ_em_oh, bad_occ_em_h, ohcal_bad_hits, ohcal_rat_occup};
	std::vector<TH1F*> QA_1D_plots {emcal_occup, ihcal_occup, ohcal_occup};
	};

class LargeRLENC : public SubsysReco
{

 public:
	enum Regions{
		Full, 
		Towards, 
		Away, 
		TransverseMax,
		TransverseMin
	};
	enum Calorimeter{
		All, 
		EMCAL,
		IHCAL,
		OHCAL
	};
  	LargeRLENC(const int n_run=0, const int n_segment=0, const float jet_min_pT=1.0, const bool data=false, std::fstream* ofs=nullptr, const std::string vari="E", const std::string &name = "LargeRLENC");

  	~LargeRLENC() override {};

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
	int Init(PHCompositeNode *topNode/*, bool* has_retower, bool *has_jets*/) override {
	// this first section is a backup way to get the status of the towers or jets
	/*	*has_retower=true;
		*has_jets=true;
		if(data){
			auto jets = findNode::getClass<JetContainer>(topNode, "AntiKt_Towers_r04");
			auto retower = findNode::getClass<TowerInfoContainer(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
			if(!jets) (*has_jets)=false;
			if(!retower) (*has_retower)=false;
		}*/
		return Fun4AllReturnCodes::EVENT_OK;
	}

  	/** Called for first event when run number is known.
      	Typically this is where you may want to fetch data from
      	database, because you know the run number. A place
      	to book histograms which have to know the run number.
   	*/
  	int InitRun(PHCompositeNode *topNode) override {return Fun4AllReturnCodes::EVENT_OK;}

  	/** Called for each event.
      	This is where you do the real work.
   	*/
 	int process_event(PHCompositeNode *topNode) override;
	
	/// Clean up internals after each event.
	int ResetEvent(PHCompositeNode *topNode) override {return Fun4AllReturnCodes::EVENT_OK;}

	/// Called at the end of each run.
	int EndRun(const int runnumber) override {return Fun4AllReturnCodes::EVENT_OK;}

	/// Called at the end of all processing.
	int End(PHCompositeNode *topNode) override {
//		if(write_to_file) this->text_out_file->close();
		return Fun4AllReturnCodes::EVENT_OK;
	};

 	/// Reset
  	int Reset(PHCompositeNode * /*topNode*/) override {return Fun4AllReturnCodes::EVENT_OK;};

  	void Print(const std::string &what = "ALL") const override;

	//and now for the unique stuff
	void addTower(int, TowerInfoContainer*, RawTowerGeomContainer_Cylinderv1*, std::map<std::array<float, 3>, float>*, RawTowerDefs::CalorimeterId);
	
	JetContainer* getJets(std::string, std::string, std::array<float, 3>, float ohcal_rat, PHCompositeNode*);

	void CaloRegion(std::map<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, float, std::string, std::array<float, 3>, float);

	void SingleCaloENC(std::map<std::array<float, 3>, float>, float, std::array<float, 3>, bool, bool, std::map<int, std::pair<float, float>>, int, float*);
	
	void CalculateENC(std::pair<std::array<float, 3>, float>, std::pair<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, std::pair<float, std::pair<float, float>>*, std::pair<float, std::vector< std::pair< std::pair<float, float>, float > > >  *, bool, bool);

	void JetEventObservablesBuilding(std::array<float, 3>, std::map<std::array<float, 3>, float>, std::map<float, float>*);
	float getR(float, float, float, float, bool print=false);


 private:

	std::string algo, radius, output_file_name;
	DijetEventCuts* eventCut;	
  	bool isRealData;
	int nRun, nSegment, m_Njets, n_evts;
	float jetMinpT, MinpTComp;
	float ptoE=1.; //need to actually do some studies into this in order to get a meaningful conversion factor
	int m_region, m_calo; 
	std::vector<std::pair<int, std::vector< std::pair<float, float> > > > t_e2c, t_e3c; //make this an indexable object
	float m_rl, m_rm, m_ri, m_e2c, m_e3c, m_Et, m_vtx, m_vty, m_vtz, m_philead, m_etalead;
	std::vector<std::pair< int, std::vector< std::pair< std::array<float, 3>, float> > > > t_e3c_full;
	std::map< std::string, std::array< std::map< std::pair< float, float >, float >, 3 > > t_pt_evt;
	float m_phi, m_eta; 
	std::string which_variable; //Which varaible are we caluclating the EEC over (E, E_T, p, p_T)
	TTree* DijetQA, *EEC, *JetEvtObs;
	std::vector<MethodHistograms*> FullCal, TowardRegion, AwayRegion, TransverseRegion;
	std::vector<std::vector<MethodHistograms*>> Region_vector;
	float m_etotal, m_eemcal, m_eihcal, m_eohcal;
	std::array<float, 3> m_vertex;
	std::vector<std::array<float, 3>> m_dijets;
//	std::map<int, std::map<std::array<float, 3>, float> > m_pt;
//	std::vector<float> m_pt_evts;
	float m_xjl, m_Ajjl;
	//std::fstream* text_out_file;
//	bool write_to_file;
	TH1F* emcal_occup, *ihcal_occup, *ohcal_occup, *ohcal_rat_h;
	TH2F* ohcal_rat_occup, *ohcal_bad_hits, *bad_occ_em_oh, *bad_occ_em_h;
	std::vector<std::array<float, 3>> m_emcal, m_ihcal, m_ohcal; //3 points, eta, phi, et

};
#endif // LARGERLENC_H
