// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef EVENTSHAPEQA_H
#define EVENTSHAPEQA_H

//fun4all
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllReturnCodes.h>

//phool 
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/PHNodeOperation.h>

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
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <math.h>
#include <array>
#include <iostream>

//root
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>
#include <TInterpreter.h>

#define PI 3.14159265358979
class PHCompositeNode;
enum CompDataTypes
{
	notComp = -1, 
	G4TruthMap = 0,
	G4TruthPrimaryParticleRange = 1,
	PHHepMCGenEvent = 2,
	AllCalTower = 3,
	EMCALTower = 4,
	IHCALTower = 5,
	OHCALTower = 6, 
	TopoCluster = 7,
	AllCALTowerCluster  = 8,
	EMCALTowerCluster = 9,
	IHCALTowerCluster = 10,
	OHCALTowerCluster = 11
};
enum JetDataTypes
{
	notJet = -1, 
	G4TruthJets = 0,
	TowerJets = 1,
	ClusterJets = 2
};

class HitPlots
{
  public:
	HitPlots(std::string input_type="G4 Particle Truth", bool jet=false, bool shifted=false)
	{
		isJet=jet;
		isShifted=shifted;
		inputType=input_type;
		convertToEnum(inputType, isJet);
		convertToLabel(this->ComponentType, this->JetType, isJet);
		makePlots();
	}
	void convertToEnum(std::string it, bool iJ)
	{
		if(iJ)
		{
			if(it.find("G4") != std::string::npos || it.find("Truth") != std::string::npos) this->JetType = JetDataTypes::G4TruthJets;
			else if(it.find("ower" ) != std::string::npos) this->JetType = JetDataTypes::TowerJets;
			else if(it.find("luster") != std::string::npos) this->JetType = JetDataTypes::ClusterJets;
			else this->JetType = JetDataTypes::notJet;
			this->ComponentType = CompDataTypes::notComp;
		}
		else{
			if(it.find("G4") != std::string::npos && it.find("Primary") == std::string::npos) this->ComponentType = CompDataTypes::G4TruthMap;
			else if( it.find("G4") != std::string::npos) this->ComponentType = CompDataTypes::G4TruthPrimaryParticleRange;
			else if ( it.find("HepMC") != std::string::npos) this->ComponentType = CompDataTypes::PHHepMCGenEvent;
			else if ( it.find("ower") != std::string::npos)
			{
				if( it.find("luster") == std::string::npos) 
				{
					if(       it.find("All")!= std::string::npos || it.find("ALL" != std::string::npos) this->ComponentType = CompDataTypes::AllCalTower;
					else if ( it.find("EM") != std::string::npos || it.find("em") != std::string::npos) this->ComponentType = CompDataTypes::EMCALTower;
					else if ( it.find("IH") != std::string::npos || it.find("ih") != std::string::npos) this->ComponentType = CompDataTypes::IHCALTower;
					else if ( it.find("OH") != std::string::npos || it.find("oh") != std::string::npos) this->ComponentType = CompDataTypes::OHCALTower;
				}
				else
				{
					if(       it.find("All")!= std::string::npos || it.find("ALL" != std::string::npos) this->ComponentType = CompDataTypes::AllCalTower;
					else if ( it.find("EM") != std::string::npos || it.find("em") != std::string::npos) this->ComponentType = CompDataTypes::EMCALTower;
					else if ( it.find("IH") != std::string::npos || it.find("ih") != std::string::npos) this->ComponentType = CompDataTypes::IHCALTower;
					else if ( it.find("OH") != std::string::npos || it.find("oh") != std::string::npos) this->ComponentType = CompDataTypes::OHCALTower;

				}
			}
			else if ( it.find("luster") != std::string::npos ) this->ComponentType = CompDataTypes::TopoCluster;
			else this->ComponentType = CompDataTypes::notComp;
			this->JetType = JetDataTypes::nonJet;
			}
			return;
	}

	void convertToLabel(CompDataTypes ct, JetDataTypes jt, bool iJ=false)
	{
		if(!iJ)
		{
			switch (ct) 
			{
				case CompDataTypes::G4TruthMap :
					this->generic_name_label+="G4truth_map_";
					this->generic_Title_label+="over G4 Truth Map Components ";
					break;
				case CompDataTypes::G4TruthPrimaryParticleRange : 
					this->generic_name_label+="G4Truth_PrimaryParticle_Range_;
					this->generic_Title_label+="over G4 Truth Primary Particle Components ;
					break;
				case CompDataTypes::PHHepMCGenEvent  : 
					this->generic_name_label+="HepMC_map_";
					this->generic_Title_label+="over HepMC map ";
					break;
				case CompDataTypes:: AllCALTower  : 
					this->generic_name_label+="AllCAL_Tower_";
					this->generic_Title_label+="over Combined Calorimeter Towers ";
					break;
				case CompDataTypes:: EMCALTower  : 
					this->generic_name_label+="EMCAL_Tower_";
					this->generic_Title_label+="over recombined EMCAL Towers ";
					break;
				case CompDataTypes:: IHCALTower  : 
					this->generic_name_label+="IHCAL_Tower_";
					this->generic_Title_label+="over IHCAL Towers ";
					break;
				case CompDataTypes:: OHCALTower : 
					this->generic_name_label+="OHCAL_Tower_";
					this->generic_Title_label+="over OHCAL Towers ";
					break;
				case CompDataTypes::TopoCluster: 
					this->generic_name_label+="Cluster_";
					this->generic_Title_label+="over Clusters ";
					break;
				case CompDataTypes::ALLCALTowerCluster : 
					this->generic_name_label+="CAL_Cluster_Towers_";
					this->generic_Title_label+="over combined Calorimeter Towers identified from clusters";
					break;
				case CompDataTypes::EMCALTowerCluster : 
					this->generic_name_label+="EMCAL_Cluster_Towers_";
					this->generic_Title_label+="over retowered EMCAL Towers identified from clusters";
					break;
				case CompDataTypes::IHCALTowerCluster : 
					this->generic_name_label+="IHCAL_Cluster_Towers_";
					this->generic_Title_label+="over IHCAL Towers identified from clusters";
					break;
				case CompDataTypes::OHCALTowerCluster : 
					this->generic_name_label+="OHCAL_Cluster_Towers_";
					this->generic_Title_label+="over OHCAL Towers identified from clusters";
					break;

			}
		}
		else
		{
			switch (jt){
				case JetDataTypes::G4TruthJets :
					this->generic_name_label+="G4truth_jets_";
					this->generic_Title_label+="over G4 Truth Jets ";
					break;
				case JetDataTypes::TowerJets :
					this->generic_name_label+="Tower_jets_";
					this->generic_Title_label+="over Tower Jets ";
					break;
				case JetDataTypes::ClusterJets :
					this->generic_name_label+="Cluster_jets_";
					this->generic_Title_label+="over Cluster Jets "
					break;
			}
		}
		if(this->isShifted){
			this->generic_name_label+="relative_axis_";
			this->generic_Title_label+="relative to Lead axis ";
		}
		else 
		{
			this->generic_name_label+="zero_axis_";
			this->generic_Title_label+="relative to detector axis ";
		}
		return;
	}
	void makePlots()
	{
		float pm=-PI, px=PI;
		float em=-1.2, ex=1.2;
		float Em=-0.5, Ex=99.5;
		float Rm=0, Rx=4.5;
		float xjm=0., xjx=1.;
		float Aj=0., Ajx=1.;
		bool istruth=true;
		xj=new TH1F(Form("%s_xj", this->generic_name_label.c_str()), Form("x_{j} %s ; x_{j}; Counts ", this->generic_Title_label.c_str()), 100, xjm, xjx);
		Aj=new TH1F(Form("%s_Aj", this->generic_name_label.c_str()), Form("A_{j} %s ; A_{j}; Counts ", this->generic_Title_label.c_str()), 100, Ajm, Ajx);
		E=new TH1F(Form("%s_E", this->generic_name_label.c_str()), Form("E %s ; E [GeV]; Counts ", this->generic_Title_label.c_str()), 100, Em, Ex);
		ET=new TH1F(Form("%s_ET", this->generic_name_label.c_str()), Form("E_{T} %s ; E_{T} [GeV]; Counts ", this->generic_Title_label.c_str()), 100, Em, Ex);
		R=new TH1F(Form("%s_R", this->generic_name_label.c_str()), Form("#Delta R from central axis %s ; #Delta R; Counts ", this->generic_Title_label.c_str()), 100, Rm, Rx);
		phi=new TH1F(Form("%s_phi", this->generic_name_label.c_str()), Form("varphi %s ; #varphi; Counts ", this->generic_Title_label.c_str()), 100, pm, px);
		eta=new TH1F(Form("%s_eta", this->generic_name_label.c_str()), Form("#eta %s ; #eta; Counts ", this->generic_Title_label.c_str()), 100, em, ex);
	       	eta_phi_hit = new TH2F(Form("%s_phi_eta_hit", this->generic_name_label.c_str()), Form("Hit map %s; #eta; #varphi", this->generic_Title_label.c_str()), 100, em, ex, 100, pm, px);
	       	eta_phi_E = new TH2F(Form("%s_phi_eta_E", this->generic_name_label.c_str()), Form("Energy Deposition %s; #eta; #varphi", this->generic_Title_label.c_str()), 100, em, ex, 100, pm, px);
	       	E_R_hit = new TH2F(Form("%s_E_R_hit", this->generic_name_label.c_str()), Form("Hit map %s;E [GeV]; #Delta R", this->generic_Title_label.c_str()), 100, Em, Ex, 100, Rm, Rx);
		return;
	}
	TH1F* xj, *Aj, *phi, *eta, *E, *ET, *R;
	TH2F* eta_phi_hit, *eta_phi_E, *E_R_hit;
	CompDataTypes ComponentType = CompDataTypes::notComp;
	JetDataTypes JetType = JetDataTypes::notJet;
	bool isJet = false;
	bool isShifted=false;
	std::string generic_name_label="h_";
	std::string generic_Title_label="";
};


class EventShapeQA : public SubsysReco
{
 public:

	EventShapeQA(int verb = 0, const std::string &name = "EventShapeQA");

	~EventShapeQA() override;

	int Init(PHCompositeNode *topNode) override;

	int InitRun(PHCompositeNode *topNode) override {return Fun4AllReturnCodes::EVENT_OK;}

	int process_event(PHCompositeNode *topNode) override;

	int ResetEvent(PHCompositeNode *topNode) override {return Fun4AllReturnCodes::EVENT_OK;}

	int EndRun(const int runnumber) override {return Fun4AllReturnCodes::EVENT_OK;}

	int End(PHCompositeNode *topNode) override;

	int Reset(PHCompositeNode * /*topNode*/) override {return Fun4AllReturnCodes::EVENT_OK;}

	void Print(const std::string &what = "ALL") const override;
	float PhiWrap(float phi) 
	{
		if(phi > PI) phi = 2*PI - phi;
		else if (phi < - PI ) phi = - 2 *PI - phi;
		return phi;
	}
	float calcR(float phi1, float phi2, float eta1, float eta2)
	{
		float dphi = phi1-phi2;
		dphi = PhiWrap(dphi);
		float deta = eta1 - eta2;
		float R = std::sqrt(std::pow(dphi, 2) + std::pow(deta, 2));
		return R;
	}

 private:
	void doPHG4Analysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>>)
	void doPHG4JetAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>>)
	void doHepMCAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>>)
	void doCaloAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>>)
	void doCaloJetAnalysis(std::pair<std::pair<CompDataTypes, JetDataTypes>, PHCompositeNode*>>)
	int verbosity = 0, n_evt = 0;
	//particles in reference to the proper calorimeter axis
	std::vector<std::pair<std::pair<CompDataTypes, JetDataTypes> , HitPlots*>>* calo_jet_zero_axis=NULL; 
	//jets in reference to the proper calorimeter axis
	std::vector<std::pair<std::pair<CompDataTypes, JetDataTypes> , HitPlots*>>* calo_jet_shift_axis=NULL; 
 	std::vector<std::pair<std::pair<CompDataTypes, JetDataTypes> ,PHCompositeNode*>> nodes {};

};

#endif // EVENTSHAPEQA_H
