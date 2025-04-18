#ifndef __LARGERLENC_LEDPEDESTALSCAN_H__
#define __LARGERLENC_LEDPEDESTALSCAN_H__

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

#include "LargeRLENC.h"
#include <string>
#include <map>
#include <array>

class PHCompositeNode;
class LargeRLENC;

class LEDPedestalScan: public SubsysReco
{
	public:

	LEDPedestalScan(const int n_run, const int n_segment, const bool doE2C=false, const bool act_loc=true, const std::string& name="LEDPedestalScan");
	~LEDPedestalScan() override {} 
	int Init(PHCompositeNode *topNode/*, bool* has_retower, bool *has_jets*/) override 	  {
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
	int n_evts=0;
	JetContainerv1* getJets(float radius, std::array<float, 3> vertex, std::map<std::array<float, 3>, float> towers);
	
	private:
		LargeRLENC* encCalc;
		TH2F* jet_loc;
		TH2F* lead_jet_loc;
		TH2F* jet_loc_center;
		TH2F* lead_jet_loc_center;
		std::vector<TH2F*> ohcal_energy_i, ihcal_energy_i, emcal_energy_i;
		std::vector<TH2F*> ohcal_energy_node, ihcal_energy_node, emcal_energy_node;
		std::vector<std::array<float, 5>> energy_thresholds;
		TH1F* ohcal_energy, *ihcal_energy, *emcal_energy, *allcal_energy;
		TH1F* ohcal_energy_total, *ihcal_energy_total, *emcal_energy_total, *allcal_energy_total;
		TH1F* ohcal_hit_plot, *ihcal_hit_plot, *emcal_hit_plot, *allcal_hit_plot;
		int run;
		int segement;
		bool run_e2c;
		bool local; 
};
#endif
