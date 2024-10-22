// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef LARGERLENC_H
#define LARGERLENC_H

#include <fun4all/SubsysReco.h>
#include <MethodHistorgrams.h>
#include <thread>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TTree.h>

struct DijetQATypePlots{
	DijetQATypePlots(){
		bad_occ_em_oh_rat=new TH2F("h_bad_occupancy_EM_OH_rat", "Occupancy for events that fail the OHCAL ratio cut; #% Towers #geq 70 MeV EMCAL; #% Towers > 500 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h_rat=new TH2F("h_bad_occupancy_EM_H_rat", "Occupancy for events that fail OHCAL ratio cut; #% Towers #geq 70 MeV EMCAL; (#% Towers #geq 500 MeV OHCAL + #% Towers #geq 100 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_oh=new TH2F("h_bad_occupancy_EM_OH", "Occupancy for events that pass  OHCAL ratio cut but otherwise fail; #% Towers #geq 70 MeV EMCAL; #% Towers > 500 MeV OHCAL; N_{Evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
		bad_occ_em_h=new TH2F("h_bad_occupancy_EM_H", "Occupancy for events that pass OHCAL ratio cut but otherwise fail; #% Towers #geq 70 MeV EMCAL; (#% Towers #geq 500 MeV OHCAL + #% Towers #geq 100 MeV IHCAL)/2", 100, -0.005, 0.995, 100, -0.005, 0.995);
		ohcal_bad_hits=new TH2F("h_ohcal_bad_hits", "Energy depositon for events that fail the OHCAL ratio cut; #eta; #varphi; E [GeV]", 24, -1.1, 1.1, 64, -3.1416, 3.1415);
		emcal_occup=new TH1F("h_EMCAL_occupancy", "Occupancy of EMCAL all events; #% Towers #geq 70 MeV EMCAL; N_{evts}", 100, -0.005, 0.995);
		ihcal_occup=new TH1F("h_IHCAL_occupancy", "Occupancy of IHCAL all events; #% Towers #geq 100 MeV IHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_occup=new TH1F("h_OHCAL_occupancy", "Occupancy of OHCAL all events; #% Towers #geq 500 MeV OHCAL; N_{evts}", 100, -0.005, 0.995);
		ohcal_rat_occup=new TH2F("h_OHCAL_rat", "Occupancy of OHCAL as function of OHCAL ratio; #frac{E_{OHCAL}}{E_{ALL CALS}}; #% Towers #geq 500 MeV; N_{evts}", 100, -0.005, 0.995, 100, -0.005, 0.995);
	}
	TH2F* bad_occ_em_oh_rat;
	TH2F* bad_occ_em_h_rat;
	TH2F* bad_occ_em_oh;
	TH2F* bad_occ_em_h;
	TH2F* ohcal_bad_hits;
	TH2F* ohcal_rat_occup;
	TH1F* emcal_occup;
	TH1F* ihcal_occup;
	TH1F* ohcal_occup;
	std::vector<TH2F*> QA_2D_plots {bad_occ_em_oh_rat, bad_occ_em_h_rat, bad_occ_em_oh, bad_occ_em_h, ohcal_bad_hits, ohcal_rat_occup};
	std::vector<TH1F*> QA_1D_plots {emcal_occup, ihcal_occup, ohcal_occup};
	}
class EventSelectionCut{
	//maybe this gets some histos added for easier QA-ing of cut safety
	public:
		EventSelectionCut(float lpt=12., slpt=7., float det=0.7, float dph=2.75,float maxpct=0.9, bool dj=true, bool ne=false, std::string radius="r04" ): leadingpt(lpt), subleadingpt(slpt), etaedge(det), deltaphi(dph), maxOHCAL(maxpct),isdijet(dj), negativeEnergy(ne){
			JetCuts=new TTree(Form("cuts_%s", radius) , Form("Jet Cut Kinematics for QA on the Jet Event for jet with radius %s", radius));
			JetCuts->Branch("N_Jets", &m_nJets, "Number of Jets in Container/I");
			JetCuts->Branch("Lead_pt", &m_lpt, "p_{T} of Leading Jet/F");
			JetCuts->Branch("Subleading_pt", &m_slpt, "p_{T} of subleading jet (pair if dijet pair exists)/F");
			JetCuts->Branch("Lead_eta", &m_etal, "#eta of leading jet center/F");
			JetCuts->Branch("Subleading_eta", &m_etasl, "#eta of subleading jet center/F");
			JetCuts->Branch("delta_phi", &m_deltaphi, "#Delta #varphi between leading and subleading jet/F");
			JetCuts->Branch("ohcal_ratio", &m_ohcalrat, "E_{OHCAL} to E_{allCal}/F");
			JetCuts->Branch("isDijet", &m_isdijet, "Passes Dijet event cuts/B");
			JetCuts->Branch("negE", &m_hasnege, "Has a negative energy jet/B");
			JetCuts->Branch("passes", &passesCuts, "Passes cut/B");
		};
		TTree* JetCuts; //This is a QA testing ttree to allow for immediate QA
		bool passesTheCut(JetContianerv2* eventjets, float hcalratio){
			//check if the particular event passed the cut 
			bool good=true;
			m_ohcalrat=hcalratio;
			if(hcalratio > maxOHCAL) good=false;
			float leadjetpt=0., subleadjetpt=0.;
			bool haspartner=false;
		       	Jet* leadjet, *subleadjet;
		       	for(auto j:jets){
				float pt=j->get_pt();
				if(pt > leadjetpt){
					if(leadjet)subleadjet=leadjet;
					leadjet=j;
				       	subleadjetpt=leadjetpt;
				       	leadjetpt=pt;
				       }
				if(!negativeEnergy){
					if(j.get_e() < 0){
						m_hasnege=true;
				       		good=false;
					}
					}
				}
			if(leadjetpt < leadingpt) good=false;
			float leadeta=leadjet->get_eta(), leadphi=leadjet->get_phi();
			if(abs(leadeta) > deltaeta ) good=false; //getting rid of events that have the leading jet outside of acceptance region
			for(auto j:jets){
				float phi=j->get_phi();
				if(abs(phi-leadphi) > deltaphi){
				       	subleadjet=j;
					subleadjetpt=j->get_pt();
					haspartner=true;
					break;
				}
			}
			if(subleadjet){ 
				float sldeta=subleadjet->get_eta();
				if(abs(sldeta) > deltaeta) good=false;
				m_etasl=sldeta;
				m_detaphi=subleadjet->get_phi()-leadjet->get_phi();

			}
			else good=false;
			if(subleadjetpt < subleadingpt || !haspartner) good=false;
			passesCut=true;
			m_isdijet=haspartner;
			m_nJets=jets->size();
			m_lpt=leadjetpt;
			m_slpt=subleadjetpt;
			JetCuts->Fill();
			return good;
		}
	private:
		float leadingpt;
		float subleadingpt; 
		float etaedge;
		float deltaphi;
		float maxOHCAL;
		bool isdijet;
		bool negativeEnergy;
		bool passesCut=false;
		int m_nJets=0;
		float m_lpt=0.;
		float m_slpt=0.;
		float m_etal=0.;
		float m_etasl=0.;
		float m_deltaphi=0.;
		float m_ohcalrat=0.;
		bool m_isdijet=false;
		bool m_hasnege=false;
};

class PHCompositeNode;

class LargeRLENC : public SubsysReco
{
 public:

  LargeRLENC(const int n_run=0, const int n_segment=0, const float jet_min_pT=1.0, const bool data=false, const std::string &name = "LargeRLENC");

  ~LargeRLENC() override;

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
	bool isRealData;
	int nRun, nSegment;
	float jetMinpT;
	TTree* DijetQA, *EEC, *JetEvtObs;
	MethodHistograms* FullHcal, *TowardRegion, *AwayRegion, *TransverseRegion;
};
#endif // LARGERLENC_H
