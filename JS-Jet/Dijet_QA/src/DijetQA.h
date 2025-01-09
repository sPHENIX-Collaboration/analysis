// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef DIJETQA_H
#define DIJETQA_H

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllBase.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <jetbase/JetMap.h>
#include <jetbase/JetContainer.h>
#include <jetbase/Jetv2.h>
#include <jetbase/Jetv1.h>

#include <centrality/CentralityInfo.h>
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <string>
#include <vector>
#include <unordered_set>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h" 
#include "TH2.h" 

class PHCompositeNode;

class DijetQA : public SubsysReco
{
 public:

  DijetQA(const std::string run_number="0", const std::string segment_number="0", const std::string &name = "DijetQA", int phismear=0);

  ~DijetQA() override;

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
	void FindPairs(JetContainer*);
	
	float XJ, AJ; 
	//These are the interesting variables, definitions for them are 
	//////////////////////////////////////////////////////////////
	//							    //
	//        X_j = (p_(T, 1))/(p_(T,2))			    //
	//        A_j = (p_(T, 1) -p_(T,2))/P_T			    //
	//							    //
	//////////////////////////////////////////////////////////////
 private:
	float DeltaPhiOne=3.141529694/32.; //cut on the opening angle of phi for the identified jets
				//Should set to integer multilple of hcal phi tower size ->Pi/32 
	int ntowers_opening=1;
	float DeltaPhi=ntowers_opening*DeltaPhiOne; 
	std::string m_run, m_seg; 
	std::pair<float, float> m_etaRange, m_ptRange;
	TTree* m_T;
	int m_event, m_nJet, m_nJetPair;
	float m_centrality, m_zvtx, m_impactparam, m_Ajj, m_xj, m_ptl, m_ptsl;
	float m_phil, m_phisl, m_dphil, m_dphi, m_etal, m_etasl, m_deltaeta;
	TH1F* h_Ajj, *h_xj, *h_pt, *h_dphi;
	TH2F* h_Ajj_pt, *h_xj_pt, *h_dphi_pt, *h_dphi_Ajj;
	TH1F* h_Ajj_l, *h_xj_l, *h_pt_l, *h_dphi_l;
	TH2F* h_Ajj_pt_l, *h_xj_pt_l, *h_dphi_pt_l, *h_dphi_Ajj_l;
	std::string m_recoJetName="AntiKT_Truth_r04";
		//use the above single tower width and tunable parameter to keep things more safely defined
};

#endif // DIJETQA_H
