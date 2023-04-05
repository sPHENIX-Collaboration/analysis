#include "CaloWaveFormSim.h"

// G4Hits includes
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4HitDefs.h>  // for hit_idbits

// G4Cells includes
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>
#include <g4detectors/PHG4CellDefs.h>  // for genkey, keytype

// Tower includes
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerDefs.h>
// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>


#include "g4detectors/PHG4CylinderGeomContainer.h"
#include "g4detectors/PHG4CylinderGeom_Spacalv1.h"  // for PHG4CylinderGeom_Spaca...
#include "g4detectors/PHG4CylinderGeom_Spacalv3.h"
#include "g4detectors/PHG4CylinderCellGeomContainer.h"
#include "g4detectors/PHG4CylinderCellGeom_Spacalv1.h"


#include <phool/getClass.h>
#include <TProfile.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TMath.h>
#include <cassert>
#include <sstream>
#include <string>
#include <TF1.h>
#include <phool/onnxlib.h>

using namespace std;
TProfile* h_template = nullptr;
TProfile* h_template_ihcal = nullptr;
TProfile* h_template_ohcal = nullptr;


#define ROWDIM 320
#define COLUMNDIM 27



double CaloWaveFormSim::template_function(double *x, double *par)
{
  Double_t v1 = par[0]*h_template->Interpolate(x[0]-par[1])+par[2];
  return v1;
}

double CaloWaveFormSim::template_function_ihcal(double *x, double *par)
{
  Double_t v1 = par[0]*h_template_ihcal->Interpolate(x[0]-par[1])+par[2];
  return v1;
}

double CaloWaveFormSim::template_function_ohcal(double *x, double *par)
{
  Double_t v1 = par[0]*h_template_ohcal->Interpolate(x[0]-par[1])+par[2];
  return v1;
}



CaloWaveFormSim::CaloWaveFormSim(const std::string& name, const std::string& filename)
  : SubsysReco(name)
  , detector("CEMC")
  , outfilename(filename)
  , hm(nullptr)
  , outfile(nullptr)
  , g4hitntuple(nullptr)

{
}

CaloWaveFormSim::~CaloWaveFormSim()
{
  delete hm;
  delete g4hitntuple;
}

int CaloWaveFormSim::Init(PHCompositeNode*)
{
  rnd = new TRandom3(0);
  //----------------------------------------------------------------------------------------------------
  //Read in the noise file, this currently points to a tim local area file, 
  //but a copy of this file is in the git repository.
  //----------------------------------------------------------------------------------------------------
  noise_midrad = new TTree("noise_midrad", "tree");
  // noise->ReadFile("/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/slowneutronsignals/CaloAna/noise.csv", "a1:a2:a3:a4:a5:a6:a7:a8:a9:a10:a11:a12:a13:a14:a15:a16:a17:a18:a19:a20:a21:a22:a23:a24:a25:a26:a27:a28:a29:a30:a31");
  noise_midrad->ReadFile("/gpfs/mnt/gpfs02/sphenix/user/trinn/sPHENIX_emcal_cosmics_sector0/noise_waveforms/medium_raddmgnoise.csv", "a1:a2:a3:a4:a5:a6:a7:a8:a9:a10:a11:a12:a13:a14:a15:a16:a17:a18:a19:a20:a21:a22:a23:a24:a25:a26:a27:a28:a29:a30:a31");

  noise_lowrad = new TTree("noise_lowrad", "tree");
  noise_lowrad->ReadFile("/gpfs/mnt/gpfs02/sphenix/user/trinn/sPHENIX_emcal_cosmics_sector0/noise_waveforms/low_raddmgnoise.csv", "a1:a2:a3:a4:a5:a6:a7:a8:a9:a10:a11:a12:a13:a14:a15:a16:a17:a18:a19:a20:a21:a22:a23:a24:a25:a26:a27:a28:a29:a30:a31");


  noise_norad = new TTree("noise_norad", "tree");
  noise_norad->ReadFile("/gpfs/mnt/gpfs02/sphenix/user/trinn/sPHENIX_emcal_cosmics_sector0/noise_waveforms/no_raddmgnoise.csv", "a1:a2:a3:a4:a5:a6:a7:a8:a9:a10:a11:a12:a13:a14:a15:a16:a17:a18:a19:a20:a21:a22:a23:a24:a25:a26:a27:a28:a29:a30:a31");

  for (int i = 0; i < 31;i++)
    {
      noise_midrad->SetBranchAddress(Form("a%d",i+1),&noise_val_midrad[i]);
      noise_lowrad->SetBranchAddress(Form("a%d",i+1),&noise_val_lowrad[i]);
      noise_norad->SetBranchAddress(Form("a%d",i+1),&noise_val_norad[i]);
    }
  
  hm = new Fun4AllHistoManager(Name());
  outfile = new TFile(outfilename.c_str(), "RECREATE");
  g4hitntuple = new TTree("tree", "tree");

  g4hitntuple->Branch("primpt",&m_primpt);
  g4hitntuple->Branch("primeta",&m_primeta);
  g4hitntuple->Branch("primphi",&m_primphi);
  g4hitntuple->Branch("tedep_emcal",&m_tedep,"tedep_emcal[24576]/F");
  g4hitntuple->Branch("tedep_ihcal",&m_tedep_ihcal,"tedep_ihcal[1536]/F");
  g4hitntuple->Branch("tedep_ohcal",&m_tedep_ohcal,"tedep_ohcal[1536]/F");
  g4hitntuple->Branch("extractedadc_emcal",& m_extractedadc,"extractedadc_emcal[24576]/F");
  g4hitntuple->Branch("extractedtime_emcal",& m_extractedtime,"extractedtime_emcal[24576]/F");
  g4hitntuple->Branch("extractedadc_ihcal",& m_extractedadc_ihcal,"extractedadc_ihcal[1536]/F");
  g4hitntuple->Branch("extractedtime_ihcal",& m_extractedtime_ihcal,"extractedtime_ihcal[1536]/F");
  g4hitntuple->Branch("extractedadc_ohcal",& m_extractedadc_ohcal,"extractedadc_ohcal[1536]/F");
  g4hitntuple->Branch("extractedtime_ohcal",& m_extractedtime_ohcal,"extractedtime_ohcal[1536]/F");
  g4hitntuple->Branch("toweradc_emcal",& m_toweradc,"toweradc_emcal[24576]/F");
  g4hitntuple->Branch("toweradc_ihcal",& m_toweradc_ihcal,"toweradc_ihcal[1536]/F");
  g4hitntuple->Branch("toweradc_ohcal",& m_toweradc_ohcal,"toweradc_ohcal[1536]/F");
  // g4hitntuple->Branch("waveform_ohcal",& m_waveform_ohcal,"waveform_ohcal[1536][16]/I");

  // g4hitntuple->Branch("npeaks_ihcal",& m_npeaks_ihcal,"npeaks_ihcall[1536]/I");
  // g4hitntuple->Branch("npeaks_ohcal",& m_npeaks_ohcal,"npeaks_ohcal[1536]/I");

  //----------------------------------------------------------------------------------------------------
  //Read in the template file, this currently points to a tim local area file, 
  //but a copy of this file is in the git repository.
  //----------------------------------------------------------------------------------------------------
  // std::string template_input_file = "/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/prdfcode/prototype/offline/packages/Prototype4/templates.root";
  std::string cemc_template_input_file = "/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/waveform_simulation/calibrations/WaveformProcessing/templates/testbeam_cemc_template.root";
  std::string ihcal_template_input_file = "/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/waveform_simulation/calibrations/WaveformProcessing/templates/testbeam_ihcal_template.root";
  std::string ohcal_template_input_file = "/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/waveform_simulation/calibrations/WaveformProcessing/templates/testbeam_ohcal_template.root";

  TFile* fin1 = TFile::Open(cemc_template_input_file.c_str());
  assert(fin1);
  assert(fin1->IsOpen());
  h_template = static_cast<TProfile*>(fin1->Get("waveform_template"));

  TFile* fin2 = TFile::Open(ihcal_template_input_file.c_str());
  assert(fin2);
  assert(fin2->IsOpen());

  h_template_ihcal = static_cast<TProfile*>(fin2->Get("waveform_template"));

  TFile* fin3 = TFile::Open(ohcal_template_input_file.c_str());
  assert(fin3);
  assert(fin3->IsOpen());

  h_template_ohcal = static_cast<TProfile*>(fin3->Get("waveform_template"));


  WaveformProcessing = new CaloWaveformProcessing();
  WaveformProcessing->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  WaveformProcessing->set_template_file("testbeam_cemc_template.root");
  WaveformProcessing->initialize_processing();



  WaveformProcessing_ihcal = new CaloWaveformProcessing();
  WaveformProcessing_ihcal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  WaveformProcessing_ihcal->set_template_file("testbeam_ihcal_template.root");
  WaveformProcessing_ihcal->initialize_processing();


  WaveformProcessing_ohcal = new CaloWaveformProcessing();
  WaveformProcessing_ohcal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  WaveformProcessing_ohcal->set_template_file("testbeam_ohcal_template.root");
  WaveformProcessing_ohcal->initialize_processing();


  light_collection_model.load_data_file(string(getenv("CALIBRATIONROOT")) + string("/CEMC/LightCollection/Prototype3Module.xml"),
								   "data_grid_light_guide_efficiency", "data_grid_fiber_trans");


  std::cout << " hey do you happen? " << std::endl;

  return 0;
}

int CaloWaveFormSim::process_event(PHCompositeNode* topNode)
{
  process_g4hits(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloWaveFormSim::process_g4hits(PHCompositeNode* topNode)
{
  bool truth = true;


 PHG4CylinderGeomContainer *layergeo = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_CEMC");
  if (!layergeo)
  {
    std::cout << "PHG4FullProjSpacalCellReco::process_event - Fatal Error - Could not locate sim geometry node "
              <<  std::endl;
    exit(1);
  }

  const PHG4CylinderGeom *layergeom_raw = layergeo->GetFirstLayerGeom();
  assert(layergeom_raw);

  const PHG4CylinderGeom_Spacalv3 *layergeom =
      dynamic_cast<const PHG4CylinderGeom_Spacalv3 *>(layergeom_raw);
  assert(layergeom);

  PHG4CylinderCellGeomContainer *seggeo = findNode::getClass<PHG4CylinderCellGeomContainer>(topNode, "CYLINDERCELLGEOM_CEMC");
  PHG4CylinderCellGeom *geo_raw = seggeo->GetFirstLayerCellGeom();
  PHG4CylinderCellGeom_Spacalv1 *geo = dynamic_cast<PHG4CylinderCellGeom_Spacalv1 *>(geo_raw);

  float waveform[24576][16];
  float waveform_ihcal[1536][16];
  float waveform_ohcal[1536][16];
  float tedep[24576];
  float tedep_ihcal[1536];
  float tedep_ohcal[1536];
  for (int i = 0; i < 24576;i++)
    {
      for (int j = 0; j < 16;j++)
	{
	  m_waveform[i][j] = 0.0;
	  waveform[i][j] = 0.0;
	}     
      m_extractedadc[i] = 0;
      m_extractedtime[i] = 0;
      m_toweradc[i] = 0;
      m_ndep[i] = 0.0;
      m_tedep[i] = 0.0;
      tedep[i] = 0.0;
    }


  for (int i = 0; i < 1536;i++)
    {
      tedep_ihcal[i] = 0.0;  
      m_extractedadc_ihcal[i] = 0;
      m_extractedtime_ihcal[i] = 0;
      tedep_ohcal[i] = 0.0;  
      m_extractedadc_ohcal[i] = 0;
      m_extractedtime_ohcal[i] = 0; 
      m_toweradc_ihcal[i] = 0.0;
      m_toweradc_ohcal[i] = 0.0;
      for (int j = 0; j < 16;j++)
	{
	  m_waveform_ihcal[i][j] = 0;
	  waveform_ihcal[i][j] = 0;
	  m_waveform_ohcal[i][j] = 0;
	  waveform_ohcal[i][j] = 0;
	}
    }

  //---------------------------------------------------------
  //Load in the template function as a TF1
  //for use in waveform generation
  //---------------------------------------------------------

  TF1* f_fit = new TF1("f_fit",template_function,0,31,3);
  f_fit->SetParameters(1,0,0);

  TF1* f_fit_ihcal = new TF1("f_fit_ihcal",template_function_ihcal,0,31,3);
  f_fit_ihcal->SetParameters(1,0,0);

  TF1* f_fit_ohcal = new TF1("f_fit_ohcal",template_function_ohcal,0,31,3);
  f_fit_ohcal->SetParameters(1,0,0);

  //-----------------------------------------------------
  //Set the timeing in of the prompt 
  //signal peak to be 4 time samples into
  //the waveform
  //------------------------------------------------------
  float _shiftval = 4-f_fit->GetMaximumX();
  f_fit->SetParameters(1,_shiftval,0);

  float _shiftval_ihcal = 4-f_fit_ihcal->GetMaximumX();
  f_fit_ihcal->SetParameters(1,_shiftval_ihcal,0);

  float _shiftval_ohcal = 4-f_fit_ohcal->GetMaximumX();
  f_fit_ohcal->SetParameters(1,_shiftval_ohcal,0);


  ostringstream nodename;
  nodename.str("");
  nodename << "G4HIT_" << detector;
  PHG4HitContainer* hits = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());
  PHG4HitContainer* hits_ihcal = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_HCALIN");
  PHG4HitContainer* hits_ohcal = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_HCALOUT");

  //-----------------------------------------------------------------------------------------
  //Loop over truth primary particles and record their kinematics
  //-----------------------------------------------------------------------------------------
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (truth)
    {
      PHG4TruthInfoContainer::ConstRange truth_range = truthinfo->GetPrimaryParticleRange();
      for (PHG4TruthInfoContainer::ConstIterator truth_iter = truth_range.first; truth_iter != truth_range.second; truth_iter++)
	{
	  PHG4Particle*part = truth_iter->second;
	  m_primid.push_back(part->get_pid());  
	  float pt =TMath::Sqrt(TMath::Power(part->get_py(),2) +TMath::Power(part->get_px(),2));
	  float theta = TMath::ATan(pt/part->get_pz());
	  float phi = TMath::ATan2(part->get_py(),part->get_px());
	  float eta = -1*TMath::Log(TMath::Tan(fabs(theta/2)));  
	  if (part->get_pz() < 0 )
	    {
	      eta = -1 * eta;
	    }
	  m_primpt.push_back(part->get_e());
	  m_primeta.push_back(eta);
	  m_primphi.push_back(phi);
	  m_primtrkid.push_back(part->get_track_id());  
	}
    }

  if (hits)
  {
    //-----------------------------------------------------------------------
    //Loop over G4Hits to build a waveform simulation
    //-----------------------------------------------------------------------
    PHG4HitContainer::ConstRange hit_range = hits->getHits();
    for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
    {
      //-----------------------------------------------------------------
      //Extract position information for each G4hit 
      //information for each G4hit in the calorimeter
      //-----------------------------------------------------------------
      int scint_id = hit_iter->second->get_scint_id();
      PHG4CylinderGeom_Spacalv3::scint_id_coder decoder(scint_id);
      std::pair<int, int> tower_z_phi_ID = layergeom->get_tower_z_phi_ID(decoder.tower_ID, decoder.sector_ID);
      const int &tower_ID_z = tower_z_phi_ID.first;
      const int &tower_ID_phi = tower_z_phi_ID.second;

      PHG4CylinderGeom_Spacalv3::tower_map_t::const_iterator it_tower =
        layergeom->get_sector_tower_map().find(decoder.tower_ID);
      assert(it_tower != layergeom->get_sector_tower_map().end());
      
      const int etabin_cell = geo->get_etabin_block(tower_ID_z);  // block eta bin
      const int sub_tower_ID_x = it_tower->second.get_sub_tower_ID_x(decoder.fiber_ID);
      const int sub_tower_ID_y = it_tower->second.get_sub_tower_ID_y(decoder.fiber_ID);
      unsigned short etabinshort = etabin_cell * layergeom->get_n_subtower_eta() + sub_tower_ID_y;
      unsigned short phibin_cell = tower_ID_phi * layergeom->get_n_subtower_phi() + sub_tower_ID_x;
    
      //----------------------------------------------------------------------------------------------------
      //Extract light yield from g4hit and correct for light collection efficiency
      //----------------------------------------------------------------------------------------------------
      double light_yield = hit_iter->second->get_light_yield();
      {
	const double z = 0.5 * (hit_iter->second->get_local_z(0) + hit_iter->second->get_local_z(1));
	assert(not std::isnan(z));
	light_yield *= light_collection_model.get_fiber_transmission(z);
      }
      {
	const double x = it_tower->second.get_position_fraction_x_in_sub_tower(decoder.fiber_ID);
	const double y = it_tower->second.get_position_fraction_y_in_sub_tower(decoder.fiber_ID);
	light_yield *= light_collection_model.get_light_guide_efficiency(x, y);
      }
      //-------------------------------------------------------------------------
      //Map the G4hits to the corresponding CEMC tower
      //-------------------------------------------------------------------------
      int etabin = etabinshort;
      int phibin = phibin_cell;
      int towernumber = etabin + 96*phibin;
      //---------------------------------------------------------------------
      //Convert the G4hit into a waveform contribution
      //---------------------------------------------------------------------
      // float t0 = 0.5*(hit_iter->second->get_t(0)+hit_iter->second->get_t(1)) / 16.66667;   //Average of g4hit time downscaled by 16.667 ns/time sample 
      float t0 = (hit_iter->second->get_t(0)) / 16.66667;   //Place waveform at the starting time of the G4hit, avoids issues caused by excessively long lived g4hits
      float tmax = 16.667*16;
      float tmin = -20;
      f_fit->SetParameters(light_yield*26000,_shiftval+t0,0);            //Set the waveform template to match the expected signal from such a hit
      if (hit_iter->second->get_t(1) >= tmin && hit_iter->second->get_t(0) <= tmax) {
	tedep[towernumber] += light_yield*26000;    // add g4hit adc deposition to the total deposition  
      }
      //-------------------------------------------------------------------------------------------------------------
      //For each tower add the new waveform contribution to the total waveform
       //-------------------------------------------------------------------------------------------------------------
     if (hit_iter->second->get_edep()*26000 > 1 && hit_iter->second->get_t(1) >= tmin && hit_iter->second->get_t(0) <= tmax)
	{
	  for (int j = 0; j < 16;j++) // 16 is the number of time samples
	    {
	      waveform[towernumber][j] +=f_fit->Eval(j);
	    }
	  m_ndep[towernumber] +=1;
	}
      m_phibin.push_back(phibin);
      m_etabin.push_back(etabin);
    }
  }



  //--------------
  // do IHCAL
  //--------------

  if (hits_ihcal)
  {
    //-----------------------------------------------------------------------
    //Loop over G4Hits to build a waveform simulation
    //-----------------------------------------------------------------------
    PHG4HitContainer::ConstRange hit_range = hits_ihcal->getHits();
    for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
    {
      //-----------------------------------------------------------------
      //Extract position information for each G4hit 
      //information for each G4hit in the calorimeter
      //-----------------------------------------------------------------
      short icolumn = hit_iter->second->get_scint_id();
      int introw = (hit_iter->second->get_hit_id() >> PHG4HitDefs::hit_idbits);

      if (introw >= ROWDIM || introw < 0)
	{
	  std::cout << __PRETTY_FUNCTION__ << " row " << introw
		    << " exceed array size: " << ROWDIM
		    << " adjust ROWDIM and recompile" << std::endl;
	  exit(1);
	}
      int towerphi = introw/4;
      int towereta = icolumn;

      //----------------------------------------------------------------------------------------------------
      //Extract light yield from g4hit and correct for light collection efficiency
      //----------------------------------------------------------------------------------------------------
      double light_yield = hit_iter->second->get_light_yield();  //raw_light_yield has no MEPHI maps applied, light_yield aoppplies the maps change at some point     
      //-------------------------------------------------------------------------
      //Map the G4hits to the corresponding CEMC tower
      //-------------------------------------------------------------------------
      
      int etabin = towereta;
      int phibin =towerphi;
      int towernumber = etabin + 24*phibin;
     
      //---------------------------------------------------------------------
      //Convert the G4hit into a waveform contribution
      //---------------------------------------------------------------------
      // float t0 = 0.5*(hit_iter->second->get_t(0)+hit_iter->second->get_t(1)) / 16.66667;   //Average of g4hit time downscaled by 16.667 ns/time sample 
      float t0 = (hit_iter->second->get_t(0)) / 16.66667;   //Place waveform at the starting time of the G4hit, avoids issues caused by excessively long lived g4hits
      float tmax = 16.667*16;
      float tmin = -20;
      f_fit_ihcal->SetParameters(light_yield*2600,_shiftval_ihcal+t0,0);            //Set the waveform template to match the expected signal from such a hit
      if (hit_iter->second->get_t(1) >= tmin && hit_iter->second->get_t(0) <= tmax) {
	tedep_ihcal[towernumber] += light_yield*2600;    // add g4hit adc deposition to the total deposition
      }
      //-------------------------------------------------------------------------------------------------------------
      //For each tower add the new waveform contribution to the total waveform
       //-------------------------------------------------------------------------------------------------------------
     if (hit_iter->second->get_edep()*2600 > 1 &&hit_iter->second->get_t(1) >= tmin && hit_iter->second->get_t(0) <= tmax )
	{
	  for (int j = 0; j < 16;j++) // 16 is the number of time samples
	    {
	      waveform_ihcal[towernumber][j] +=f_fit_ihcal->Eval(j);
	    }
	}
    }
  }



  //--------------
  // do OHCAL
  //--------------

  if (hits_ohcal)
  {
    //-----------------------------------------------------------------------
    //Loop over G4Hits to build a waveform simulation
    //-----------------------------------------------------------------------
    PHG4HitContainer::ConstRange hit_range = hits_ohcal->getHits();
    for (PHG4HitContainer::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; hit_iter++)
    {
      //-----------------------------------------------------------------
      //Extract position information for each G4hit 
      //information for each G4hit in the calorimeter
      //-----------------------------------------------------------------
      short icolumn = hit_iter->second->get_scint_id();
      int introw = (hit_iter->second->get_hit_id() >> PHG4HitDefs::hit_idbits);


      if (introw >= ROWDIM || introw < 0)
	{
	  std::cout << __PRETTY_FUNCTION__ << " row " << introw
		    << " exceed array size: " << ROWDIM
		    << " adjust ROWDIM and recompile" << std::endl;
	  exit(1);
	}


      int towerphi = introw/5;
      int towereta = icolumn;

      //----------------------------------------------------------------------------------------------------
      //Extract light yield from g4hit and correct for light collection efficiency
      //----------------------------------------------------------------------------------------------------

      double light_yield = hit_iter->second->get_light_yield();  //raw_light_yield has no MEPHI maps applied, light_yield aoppplies the maps change at some point
      //-------------------------------------------------------------------------
      //Map the G4hits to the corresponding CEMC tower
      //-------------------------------------------------------------------------
      
      int etabin = towereta;
      int phibin =towerphi;
      int towernumber = etabin + 24*phibin;
      
      //---------------------------------------------------------------------
      //Convert the G4hit into a waveform contribution
      //---------------------------------------------------------------------
      // float t0 = 0.5*(hit_iter->second->get_t(0)+hit_iter->second->get_t(1)) / 16.66667;   //Average of g4hit time downscaled by 16.667 ns/time sample 
      float t0 = (hit_iter->second->get_t(0)) / 16.66667;   //Place waveform at the starting time of the G4hit, avoids issues caused by excessively long lived g4hits
      float tmax =16.667*16 ;
      float tmin = -20;
      f_fit_ohcal->SetParameters(light_yield*5000,_shiftval_ohcal+t0,0);            //Set the waveform template to match the expected signal from such a hit
      if (hit_iter->second->get_t(0) < tmax && hit_iter->second->get_t(1) > tmin) {
	  {
	    tedep_ohcal[towernumber] += light_yield*5000;    // add g4hit adc deposition to the total deposition 
	  }
      }
      //-------------------------------------------------------------------------------------------------------------
      //For each tower add the new waveform contribution to the total waveform
      //-------------------------------------------------------------------------------------------------------------

     if (hit_iter->second->get_edep()*5000 > 1 && hit_iter->second->get_t(1) >= tmin && hit_iter->second->get_t(0) <= tmax)
	{
	  for (int j = 0; j < 16;j++) // 16 is the number of time samples
	    {
	      waveform_ohcal[towernumber][j] +=f_fit_ohcal->Eval(j);
	    }
	}
    }
  }


  //----------------------------------------------------------------------------------------
  //For each tower loop over add a noise waveform 
  //from cosmic data, gain is too high but is corrected for
  //----------------------------------------------------------------------------------------

  //-----------------------------
  // do noise for EMCal:
  //-----------------------------
  for (int i = 0; i < 24576;i++)
    {
      int noise_waveform  = (int)rnd->Uniform(0,1990);
      noise_midrad->GetEntry(noise_waveform);
      m_tedep[i] = tedep[i];
      for (int k = 0; k < 16;k++)
	{
	  m_waveform[i][k] = waveform[i][k]+(noise_val_midrad[k]-1500)/16.0+1500;
	  // m_waveform[i][k] = waveform[i][k]+1500;
	}
    }
  //---------------------------
  // do noise for ihcal:
  //---------------------------
  for (int i = 0; i < 1536;i++)
    {
      int noise_waveform  = (int)rnd->Uniform(0,1990);
      noise_lowrad->GetEntry(noise_waveform);     
      m_tedep_ihcal[i] = tedep_ihcal[i];
      for (int k = 0; k < 16;k++)
	{
	   m_waveform_ihcal[i][k] = waveform_ihcal[i][k]+(noise_val_lowrad[k]-1500)/16.0+1500;
	  // m_waveform_ihcal[i][k] = waveform_ihcal[i][k]+1500;
	}
    }
  //---------------------------
  // do noise for ohcal:
  //---------------------------
  for (int i = 0; i < 1536;i++)
    {
      int noise_waveform  = (int)rnd->Uniform(0,1990);
      noise_norad->GetEntry(noise_waveform);
      
      m_tedep_ohcal[i] = tedep_ohcal[i];
      for (int k = 0; k < 16;k++)
	{
	  m_waveform_ohcal[i][k] = waveform_ohcal[i][k]+(noise_val_norad[k]-1500)/16.0+1500;
	  // m_waveform_ohcal[i][k] = waveform_ohcal[i][k]+1500;
	}
    }
  std::vector<std::vector<float>> fitresults;
  std::vector<std::vector<float>> fitresults_ihcal;
  std::vector<std::vector<float>> fitresults_ohcal;
  //------------------------------------------
  //Process Waveforms:  EMCal
  //------------------------------------------
  {
    std::vector<std::vector<float>> waveforms;
    for (int i = 0; i < 24576;i++)
      {
	std::vector<float>tmp;
	for (int j = 0; j < 16;j++)
	  {
	    tmp.push_back(m_waveform[i][j]);
	  }
	waveforms.push_back(tmp);
	tmp.clear();
      }
    fitresults =  WaveformProcessing->process_waveform(waveforms);
    for (int i = 0; i < 24576;i++)
      {
	m_extractedadc[i] = fitresults.at(i).at(0);
	m_extractedtime[i] = fitresults.at(i).at(1) - _shiftval;
      }
    waveforms.clear();
  }



  //------------------------------------------
  //Process Waveforms:  IHCal
  //------------------------------------------
  {
    std::vector<std::vector<float>> waveforms;
    for (int i = 0; i < 1536;i++)
      {
	std::vector<float>tmp;
	for (int j = 0; j < 16;j++)
	  {
	    tmp.push_back(m_waveform_ihcal[i][j]);
	  }
	waveforms.push_back(tmp);

	// int size2 = tmp.size();
	// int n_peak = 0;
	// for (int j = 2; j < size2-1;j++)
	//   {
	//     if (tmp.at(j) > 1.01*tmp.at(j-2) && tmp.at(j) > 1.01 * tmp.at(j+1))
	//       {
	// 	n_peak++;
	//       }
	//   }
	// m_npeaks_ihcal[i] = n_peak;



	tmp.clear();
      }
    fitresults_ihcal =  WaveformProcessing_ihcal->process_waveform(waveforms);
    for (int i = 0; i < 1536;i++)
      {
	m_extractedadc_ihcal[i] = fitresults_ihcal.at(i).at(0);
	m_extractedtime_ihcal[i] = fitresults_ihcal.at(i).at(1) - _shiftval_ihcal;
      }
    waveforms.clear();
  }



  //------------------------------------------
  //Process Waveforms:  OHCal
  //------------------------------------------
  {
    std::vector<std::vector<float>> waveforms;
    for (int i = 0; i < 1536;i++)
      {
	std::vector<float>tmp;
	for (int j = 0; j < 16;j++)
	  {
	    tmp.push_back(m_waveform_ohcal[i][j]);
	  }
	waveforms.push_back(tmp);

	
	// int size2 = tmp.size();
	// int n_peak = 0;
	// for (int j = 2; j < size2-1;j++)
	//   {
	//     if (tmp.at(j) - tmp.at(j-2) > 15 && tmp.at(j) - tmp.at(j+1) > 15)
	//       {
	// 	n_peak++;
	//       }
	//   }

	// m_npeaks_ohcal[i] = n_peak;
	tmp.clear();
      }




    fitresults_ohcal =  WaveformProcessing_ohcal->process_waveform(waveforms);
    for (int i = 0; i < 1536;i++)
      {
	m_extractedadc_ohcal[i] = fitresults_ohcal.at(i).at(0);
	m_extractedtime_ohcal[i] = fitresults_ohcal.at(i).at(1) - _shiftval_ohcal;
      }
    waveforms.clear();
  }



  std::cout << 4 << std::endl;


  {
    RawTowerContainer *towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_CEMC");
   
    RawTowerContainer::ConstRange tower_range = towers->getTowers();
    for (RawTowerContainer::ConstIterator tower_iter = tower_range.first; tower_iter != tower_range.second; tower_iter++)
      {
	int phibin = tower_iter->second->get_binphi();
	int etabin = tower_iter->second->get_bineta();
	float energy = tower_iter->second->get_energy();
	int towernumber = etabin + 96*phibin;
	m_toweradc[towernumber] =  energy;
      }
  }

  {
    RawTowerContainer *towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_HCALIN");
    
    RawTowerContainer::ConstRange tower_range = towers->getTowers();
    for (RawTowerContainer::ConstIterator tower_iter = tower_range.first; tower_iter != tower_range.second; tower_iter++)
      {
	int phibin = tower_iter->second->get_binphi();
	int etabin = tower_iter->second->get_bineta();
	float energy = tower_iter->second->get_energy();
	int towernumber = etabin + 24*phibin;
	m_toweradc_ihcal[towernumber] =  energy;
      }
  }
  
  {
    RawTowerContainer *towers = findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_HCALOUT");
    RawTowerContainer::ConstRange tower_range = towers->getTowers();
    for (RawTowerContainer::ConstIterator tower_iter = tower_range.first; tower_iter != tower_range.second; tower_iter++)
      {
	int phibin = tower_iter->second->get_binphi();
	int etabin = tower_iter->second->get_bineta();
	float energy = tower_iter->second->get_energy();
	int towernumber = etabin + 24*phibin;
	m_toweradc_ohcal[towernumber] =  energy;
      }
  }
  
  g4hitntuple->Fill();
  

  //------------------------------
  //Clear vector content
  //------------------------------
 
  fitresults.clear();
  fitresults_ihcal.clear();
  fitresults_ohcal.clear();
  m_primid.clear();
  m_primtrkid.clear();
  m_primpt.clear();
  m_primeta.clear();
  m_primphi.clear();
  m_etabin.clear();
  m_phibin.clear();




  return Fun4AllReturnCodes::EVENT_OK;
}



int CaloWaveFormSim::End(PHCompositeNode* topNode)
{
  outfile->cd();
  g4hitntuple->Write();
  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(outfilename, "UPDATE");
  return 0;
}
