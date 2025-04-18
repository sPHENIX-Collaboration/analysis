// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef LARGERLENC_H
#define LARGERLENC_H
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
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

//G4 objects
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <phhepmc/PHHepMCGenEvent.h>  
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>

//jetbase objects 
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

//trigger
#include <ffarawobjects/Gl1Packetv2.h>
#include <ffarawobjects/Gl1Packetv1.h>
#include <calotrigger/TriggerAnalyzer.h>
#include <calotrigger/TriggerRunInfov1.h>

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
#include <set>
//root
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TInterpreter.h>
#include <TDirectory.h>
//Homebrews 
#include <calorimetertowerenc/MethodHistograms.h> 
#include "DijetEventCuts.h"
#include "HelperStructs.h"

#define PI 3.14159265358979323464
class PHCompositeNode;
class Jet;
class JetContainer;
class PHG4Hit;
class PHG4Particle;
class PHG4TruthInfoContainer;
class TriggerAnalyzer; 


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
		OHCAL, 
		TRUTH
	};
  	LargeRLENC(const int n_run=0, const int n_segment=0, const float jet_min_pT=1.0, const bool data=false, const bool pedestal=false, std::fstream* ofs=nullptr, const std::string vari="E", const std::string &name = "LargeRLENC");

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
	
	std::array<float,3> HadronicEnergyBalence(Jet*, float, PHCompositeNode*);
	std::vector<std::array<float,3>> getJetEnergyRatios(JetContainerv1*, float, PHCompositeNode*);	
	JetContainerv1* getJets(std::string, std::string, std::array<float, 3>, float ohcal_rat, PHCompositeNode*);
	void TruthRegion (std::map<std::array<float, 3>, float> , float,  std::string, std::array<float, 3>, float);

	void CaloRegion(std::map<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, std::map<std::array<float, 3>, float>, float, std::string, std::array<float, 3>, float);

	void SingleCaloENC( std::map<std::array<float, 3>, float>, float, std::array<float, 3>, bool, bool, std::map<int, std::pair<float, float>>, LargeRLENC::Calorimeter);
	
	void CalculateENC(StrippedDownTowerWithThreshold*, std::vector<StrippedDownTowerWithThreshold>, bool, bool);

	void JetEventObservablesBuilding(std::array<float, 3>, std::map<std::array<float, 3>, float>, std::map<float, float>*);
	float getR(float, float, float, float, bool print=false);
	bool triggerCut(bool, PHCompositeNode*);
	void Merger(TowerOutput*, std::vector<TowerOutput*>, std::set<float>, std::set<std::array<float, 3>>);
	DijetEventCuts* eventCut;	
	void MakeEMCALRetowerMap(RawTowerGeomContainer_Cylinderv1* em_geom, TowerInfoContainer* emcal, RawTowerGeomContainer_Cylinderv1* h_geom, TowerInfoContainer* hcal );
	std::vector<std::array<float, 4>> Thresholds;	
	std::map<int, std::pair<float, float>> emcal_lookup_table;
 private:
	std::string algo, radius, output_file_name;
	std::string ohcal_energy_towers="TOWERINFO_CALIB_HCALOUT", ihcal_energy_towers="TOWERINFO_CALIB_HCALIN", emcal_energy_towers="TOWERINFO_CALIB_CEMC";
  	bool isRealData, pedestalData;
	int nRun, nSegment, m_Njets, n_evts, n_with_jets=0;
	float jetMinpT, MinpTComp;
	float ptoE=1.; //need to actually do some studies into this in order to get a meaningful conversion factor
	int m_region, m_calo; 
	std::vector<std::pair<int, std::vector< std::pair<float, float> > > > t_e2c, t_e3c; //make this an indexable object
	float m_rl, m_rm, m_ri, m_e2c, m_e3c, m_Et, m_vtx, m_vty, m_vtz, m_philead, m_etalead;
	std::vector<std::pair< int, std::vector< std::pair< std::array<float, 3>, float> > > > t_e3c_full;
	std::map< std::string, std::array< std::map< std::pair< float, float >, float >, 3 > > t_pt_evt;
	float m_phi, m_eta; 
	std::string which_variable; //Which varaible are we caluclating the EEC over (E, E_T, p, p_T)
	TTree* DijetQA, *EEC/*, *JetEvtObs*/;
	std::vector<std::vector<std::vector<MethodHistograms*>>> Region_vector, Tr_Region_vector;
	float m_etotal, m_eemcal, m_eihcal, m_eohcal;
	std::array<float, 3> m_vertex;
	std::vector<std::array<float, 3>> m_dijets;
	float m_xjl, m_Ajjl;
	TH1F* emcal_occup, *ihcal_occup, *ohcal_occup, *ohcal_rat_h;
	TH2F* ohcal_rat_occup, *ohcal_bad_hits, *bad_occ_em_oh, *bad_occ_em_h;
	TH1F* IE, *badIE, *goodIE;
	TH2F* E_IE, *badE_IE, *goodE_IE;
	std::vector<std::array<float, 3>> m_emcal, m_ihcal, m_ohcal; //3 points, eta, phi, et
	std::array<std::array<TH1F*, 3>, 3> Et_miss_hists;
	std::array<float, 5> thresh_mins;
	float ohcal_min=0.01; //7.5 MeV from jet 30 and 10 study
	float emcal_min=0.01; //50 MeV
	float ihcal_min=0.005; //7.5 MeV
	float all_min=0.065; //65 MeV
	//all these are conservative vals 
	int n_steps=10;
};
#endif // LARGERLENC_H
