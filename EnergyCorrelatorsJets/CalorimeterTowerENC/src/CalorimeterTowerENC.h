// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALORIMETERTOWERENC_H
#define CALORIMETERTOWERENC_H

#include <fun4all/Fun4AllBase.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <g4eval/JetTruthEval.h>

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
//#include <calobase/DeadHotMapLoader.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jet.h> 

#include <TH1.h>
#include <TTree.h>
#include <TH2.h>
#include <TFile.h>

#include <string>
#include <cstdio>
#include <vector>
#include <map>
#include <utility>
#include <unordered_set>
#include <set>
#include <thread>
#include <cfloat>
#include <cmath>

#include "MethodHistograms.h" 

class PHCompositeNode;
class Jet; 
class JetContainer; 
class PHG4Hit;
class PHG4Particle; 
class PHG4Shower;
class PHG4TruthInfoContainer;
class JetTruthEval; 

#define PI 3.1415926535
class CalorimeterTowerENC : public SubsysReco
{
 public:

  	CalorimeterTowerENC(int n_run, int n_segment, float jet_min=1.0, const std::string &name = "CalorimeterTowerENC");

  	~CalorimeterTowerENC() override {};

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
  	int InitRun(PHCompositeNode *topNode) override {return 0;};

  /** Called for each event.
      This is where you do the real work.
   */
  	int process_event(PHCompositeNode *topNode) override;
	std::pair<std::map<float, std::map<float, int>>, std::pair<float, float>> GetTowerMaps(RawTowerGeomContainer_Cylinderv1*, RawTowerDefs::CalorimeterId, TowerInfoContainer*);
	float getPt(PHG4Particle*);
	float getR(std::pair<float, float>, std::pair<float, float>);
	void GetENCCalo(std::map<int, float>, RawTowerGeomContainer_Cylinderv1*, TowerInfoContainer*, MethodHistograms*, float);
	void GetENCCalo(PHCompositeNode*, std::unordered_set<int>, TowerInfoContainer*, RawTowerGeomContainer_Cylinderv1*, RawTowerDefs::CalorimeterId, float,  std::string, int, float, std::map<int, float>*); 
	void GetE2C(PHCompositeNode*, std::unordered_set<int>, std::unordered_set<int>, std::unordered_set<int>, float);
	void GetE2C(PHCompositeNode*, std::map<PHG4Particle*, std::pair<float, float>>);
	void GetE3C(PHCompositeNode*, std::unordered_set<int>, std::unordered_set<int>, std::unordered_set<int>, std::map<int, float>*, bool, float);
	void GetE3C(PHCompositeNode*, std::map<PHG4Particle*, std::pair<float, float>>);
	int GetTowerNumber(std::pair<float, float>, std::map<float, std::map<float, int>>, std::pair<float, float>);
	int RecordHits(PHCompositeNode* topNode, Jet*, std::vector<std::unordered_set<int>>);	
	std::map<std::pair<float, float>, std::vector<std::unordered_set<int>>> FindAntiKTTowers(PHCompositeNode*);
	std::map<Jet*, std::pair<float, float>> MatchKtTowers(std::map<std::pair<float, float>, std::vector<std::unordered_set<int>>>, JetContainer*); 
	//This is returning a map of jet axes with a set of towers that correspond to that jet for each calo, EM, IH, OH
  /// Clean up internals after each event.
  	int ResetEvent(PHCompositeNode *topNode) override {return 0;};

  /// Called at the end of each run.
  	int EndRun(const int runnumber) override {return 0;};

  /// Called at the end of all processing.
  	int End(PHCompositeNode *topNode) override;

  /// Reset
  	int Reset(PHCompositeNode * /*topNode*/) override {return 0;};

  	void Print(const std::string &what = "ALL") const override;
	std::string outfilename="";

 private:
	int n_evts=0, Nj=1;
	float jet_cutoff=1.0; 
	TH2F *jethits, *comptotows, *calojethits; //phi-eta hit map and correlation plots for cross checks
	//JetTruthEval* truth_evaluater;
	MethodHistograms *Particles, *EMCal, *IHCal, *OHCal, *EMCalKT, *IHCalKT, *OHCalKT, *AllCal, *AllCalKT;
	TH1F* number_of_jets, *EM_energy, *OH_energy, *IH_energy;
	std::map<std::string, MethodHistograms*> histogram_map; 
	std::pair< std::map<float, std::map<float, int>>, std::pair<float, float>> EMCALMAP, IHCALMAP, OHCALMAP;
	std::map<int, int> emcal_lookup;
	std::map<int, std::vector<int>> hcal_lookup;
};

#endif // CALORIMETERTOWERENC_H
