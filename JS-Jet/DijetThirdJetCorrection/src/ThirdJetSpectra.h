// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef THIRDJETSPECTRA_H
#define THIRDJETSPECTRA_H
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <ffaobjects/EventHeaderv1.h>
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <phool/phool.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHObject.h>

//pythia 
#include <phpythia8/PHPythia8.h>
#include <phpythia8/PHPy8JetTrigger.h>

//HEPMC nneded files
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>

//g4 packages 
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
//
//fastJet 
#include <jetbase/JetContainer.h>
#include <jetbase/Jet.h>
#include <jetbase/JetAlgo.h>
#include <jetbase/FastJetAlgo.h>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>

////c++
#include <string>
#include <vector>
#include <map>
#include <math.h>
#include <algorithm>

//Root
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
//homebrew 
#include <largerlenc/DijetEventCuts.h> //prebuilt dijet cut object 

//#define PI 3.14159265; //defined this in the dijet event cuts
class PHCompositeNode;
class Jet;
class JetContainer;

class ThirdJetSpectra : public SubsysReco
{
   public:

  	ThirdJetSpectra(std::string numb = "0", const std::string &name = "ThirdJetSpectra");
	~ThirdJetSpectra() override;

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
	std::vector<fastjet::PseudoJet> findAllJets(HepMC::GenEvent* e1);
	void getJetTripplet(JetContainerv1*, std::vector<Jetv2*>*, bool);
	void getJetPair(JetContainerv1*, std::vector<Jetv2*>*, bool);
	
	TH1F* xj, *xj_strict, *xj_onl, *xj_strict_onl;
	TH1F* first_jet_pt, *second_jet_pt, *third_jet_pt;
	TH1F* first_jet_phi, *second_jet_phi, *third_jet_phi;
	TH1F* first_jet_eta, *second_jet_eta, *third_jet_eta;
	TH1F* first_jet_E, *second_jet_E, *third_jet_E;
	TH1F* dphi_12, *dphi_13, *dphi_23;
	TH1F* xj_12, *xj_13, *xj_23;
	TH1F* xj_strict_12, *xj_strict_13, *xj_strict_23;
	TH1F* third_jet_pt_dec, *third_jet_pt_dec_strict;
	TH1F* decorr, *decorr_strict;
	TH2F* third_jet_pt_dec_n, *third_jet_pt_dec_strict_n;
	TH3F* dphi_123, *pt_123;
	DijetEventCuts* DiJEC, *DiJEC_strict; 
	int n_evts=0, n_three=0, n_try=0;
	std::string segment_numb;
};

	#endif // THIRDJETSPECTRA_H
