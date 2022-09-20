#include "CaloWaveFormSim.h"

// G4Hits includes
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

// G4Cells includes
#include <g4detectors/PHG4Cell.h>
#include <g4detectors/PHG4CellContainer.h>

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






double CaloWaveFormSim::template_function(double *x, double *par)
{
  Double_t v1 = par[0]*h_template->Interpolate(x[0]-par[1])+par[2];
  // Double_t v1 = par[0]*TMath::Power(x[0],par[1])+par[2];
  return v1;
}
CaloWaveFormSim::CaloWaveFormSim(const std::string& name, const std::string& filename)
  : SubsysReco(name)
  , detector("CEMC")
  , outfilename(filename)
  , hm(nullptr)
  , outfile(nullptr)
  , g4hitntuple(nullptr)
  , g4cellntuple(nullptr)
  , towerntuple(nullptr)
  , clusterntuple(nullptr)
{
}

CaloWaveFormSim::~CaloWaveFormSim()
{
  delete hm;
  delete g4hitntuple;
  delete g4cellntuple;
  delete towerntuple;
  delete clusterntuple;
}

int CaloWaveFormSim::Init(PHCompositeNode*)
{
  rnd = new TRandom3(0);
  //----------------------------------------------------------------------------------------------------
  //Read in the noise file, this currently points to a tim local area file, 
  //but a copy of this file is in the git repository.
  //----------------------------------------------------------------------------------------------------
  noise = new TTree("t", "tree from ytt_yt.csv");
  noise->ReadFile("/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/slowneutronsignals/CaloAna/noise.csv", "a1:a2:a3:a4:a5:a6:a7:a8:a9:a10:a11:a12:a13:a14:a15:a16:a17:a18:a19:a20:a21:a22:a23:a24:a25:a26:a27:a28:a29:a30:a31");
  for (int i = 0; i < 31;i++)
    {
      noise->SetBranchAddress(Form("a%d",i+1),&noise_val[i]);
    }
  
  hm = new Fun4AllHistoManager(Name());
  outfile = new TFile(outfilename.c_str(), "RECREATE");
  g4hitntuple = new TTree("hitntup", "hitntuple");
  g4hitntuple->Branch("t0",&m_t0);
  g4hitntuple->Branch("x0",&m_x0);
  g4hitntuple->Branch("y0",&m_y0);
  g4hitntuple->Branch("z0",&m_z0);
  g4hitntuple->Branch("eta",&m_eta);
  g4hitntuple->Branch("phi",&m_phi);
  g4hitntuple->Branch("etabin",&m_etabin);
  g4hitntuple->Branch("phibin",&m_phibin);
  g4hitntuple->Branch("edep",&m_edep);
  g4hitntuple->Branch("g4primtrkid",&m_g4primtrkid);
  g4hitntuple->Branch("g4primpt",&m_g4primpt);
  g4hitntuple->Branch("primid",&m_primid);
  g4hitntuple->Branch("primtrkid",&m_primtrkid);
  g4hitntuple->Branch("primpt",&m_primpt);
  g4hitntuple->Branch("primeta",&m_primeta);
  g4hitntuple->Branch("waveform",&m_waveform,"waveform[24576][16]/I");
  g4hitntuple->Branch("ndep",&m_ndep,"ndep[24576]/I");
  g4hitntuple->Branch("tedep",&m_tedep,"tedep[24576]/I");
  g4hitntuple->Branch("extractedadc",& m_extractedadc,"extractedadc[24576]/F");
  g4hitntuple->Branch("toweradc",& m_toweradc,"toweradc[24576]/F");
  g4hitntuple->Branch("geoetabin",&m_geoetabin);
  g4hitntuple->Branch("geophibin",&m_geophibin);
  
  g4cellntuple = new TNtuple("cellntup", "G4Cells", "phi:eta:edep");
  towerntuple = new TNtuple("towerntup", "Towers", "phi:eta:energy");
  clusterntuple = new TNtuple("clusterntup", "Clusters", "phi:z:energy:towers");
  //----------------------------------------------------------------------------------------------------
  //Read in the template file, this currently points to a tim local area file, 
  //but a copy of this file is in the git repository.
  //----------------------------------------------------------------------------------------------------
  std::string template_input_file = "/gpfs/mnt/gpfs02/sphenix/user/trinn/fitting_algorithm_playing/prdfcode/prototype/offline/packages/Prototype4/templates.root";
  TFile* fin = TFile::Open(template_input_file.c_str());
  assert(fin);
  assert(fin->IsOpen());
  h_template = static_cast<TProfile*>(fin->Get("hp_electrons_fine_emcal_36_8GeV"));


  WaveformProcessing = new CaloWaveformProcessing();
  std::cout << "init 1 " << std::endl;
  WaveformProcessing->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  // WaveformProcessing->set_nthreads(4);
  std::cout << "init 2 " << std::endl;
  WaveformProcessing->initialize_processing();
  std::cout << "init 3 " << std::endl;

  light_collection_model.load_data_file(string(getenv("CALIBRATIONROOT")) + string("/CEMC/LightCollection/Prototype3Module.xml"),
								   "data_grid_light_guide_efficiency", "data_grid_fiber_trans");


  return 0;
}

int CaloWaveFormSim::process_event(PHCompositeNode* topNode)
{

  process_g4hits(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloWaveFormSim::process_g4hits(PHCompositeNode* topNode)
{
  bool truth = false;


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
  float tedep[24576];
  for (int i = 0; i < 24576;i++)
    {
      for (int j = 0; j < 16;j++)
	{
	  m_waveform[i][j] = 0.0;
	  waveform[i][j] = 0.0;
	}     
      m_extractedadc[i] = 0;
      m_toweradc[i] = 0;
      m_ndep[i] = 0.0;
      m_tedep[i] = 0.0;
      tedep[i] = 0.0;
    }
  //---------------------------------------------------------
  //Load in the template function as a TF1
  //for use in waveform generation
  //---------------------------------------------------------

  TF1* f_fit = new TF1("f_fit",template_function,0,31,3);
  f_fit->SetParameters(1,0,0);
  //-----------------------------------------------------
  //Set the timeing in of the prompt 
  //signal peak to be 4 time samples into
  //the waveform
  //------------------------------------------------------
  float _shiftval = 4-f_fit->GetMaximumX();
  f_fit->SetParameters(1,_shiftval,0);
   
  // RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  // if (!geomEM)
  //   {
  //     std::cout << "hey no geometry node this is wrong!" << std::endl;
  //   }

  ostringstream nodename;
  nodename.str("");
  nodename << "G4HIT_" << detector;
  PHG4HitContainer* hits = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());




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
	  
	  float eta = -1*TMath::Log(TMath::Tan(fabs(theta/2)));
	  
	  if (part->get_pz() < 0 )
	    {
	      eta = -1 * eta;
	    }
	  m_primpt.push_back(pt);
	  m_primeta.push_back(eta);
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
      //----------------------------------------------------------------------------------
      //Track the G4hit to its primary particle particle to enable
      //study of energy deposition from an individual particle
      //----------------------------------------------------------------------------------

      PHG4Particle* part =	 truthinfo->GetParticle(hit_iter->second->get_trkid());
      int parentid = part->get_parent_id();
      int previousparentid = hit_iter->second->get_trkid();
      while (parentid != 0)
	{
	  previousparentid = parentid;
	  PHG4Particle* mommypart = truthinfo->GetParticle(parentid);
	  parentid = mommypart->get_parent_id();
	}      
      PHG4Particle* primarypart =truthinfo->GetParticle(previousparentid);
      m_g4primtrkid.push_back(primarypart->get_track_id());      
      float pt =TMath::Sqrt(TMath::Power(primarypart->get_py(),2) +TMath::Power(primarypart->get_px(),2));
      m_g4primpt.push_back(pt);
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
      float t0 = 0.5*(hit_iter->second->get_t(0)+hit_iter->second->get_t(1)) / 16.66667;   //Average of g4hit time downscaled by 16.667 ns/time sample 

      float tmax = 60;
      float tmin = -20;
      // if (hit_iter->second->get_t(0) > tmax || hit_iter->second->get_t(1) < tmin) {continue;}
      f_fit->SetParameters(light_yield*26000,_shiftval+t0,0);            //Set the waveform template to match the expected signal from such a hit
      if (hit_iter->second->get_t(0) < tmax && hit_iter->second->get_t(1) > tmin) {
	tedep[towernumber] += light_yield*26000;    // add g4hit adc deposition to the total deposition (scale 673 maps GeV to ADC from test beam data) 
      }
      //-------------------------------------------------------------------------------------------------------------
      //For each tower add the new waveform contribution to the total waveform
       //-------------------------------------------------------------------------------------------------------------
     if (hit_iter->second->get_edep()*673*50 > 1)
	{
	  for (int j = 0; j < 16;j++) // 16 is the number of time samples
	    {
	      waveform[towernumber][j] +=f_fit->Eval(j);
	    }
	  m_ndep[towernumber] +=1;
	}
      m_phibin.push_back(phibin);
      m_etabin.push_back(etabin);
      m_t0.push_back(hit_iter->second->get_t(0));
      m_x0.push_back(hit_iter->second->get_x(0));
      m_y0.push_back(hit_iter->second->get_y(0));
      m_z0.push_back(hit_iter->second->get_z(0));
      m_edep.push_back(hit_iter->second->get_edep());
    }
  }
  //----------------------------------------------------------------------------------------
  //For each tower loop over add a noise waveform 
  //from cosmic data, gain is too high but is corrected for
  //----------------------------------------------------------------------------------------


  for (int i = 0; i < 24576;i++)
    {
      int maxbin = 0;
      bool redraw = true;
      // std::cout << i << std::endl;
      while (redraw)
	{
	  int noise_waveform  = (int)rnd->Uniform(0,383999);
	  noise->GetEntry(noise_waveform);
	  float maxval = 0;
	  for (int k = 0; k < 16;k++)
	    {
	      if (noise_val[k] > maxval)
		{
		  maxval = noise_val[k]; 
		  maxbin = k;
		}
	    }
	  if (maxbin < 10){redraw = false;}
	}
      m_tedep[i] = tedep[i];
      for (int k = 0; k < 16;k++)
	{
	  // m_waveform[i][k] = waveform[i][k]+(noise_val[k]-1500)/16+1500;
	  m_waveform[i][k] = waveform[i][k]+1500;
	  // m_waveform[i][k] = waveform[i][k]+(noise_val[k]);
	}
    }
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


 std::vector<std::vector<float>> fitresults =  WaveformProcessing->process_waveform(waveforms);

  for (int i = 0; i < 24576;i++)
    {
      m_extractedadc[i] = fitresults.at(i).at(0);

    }
  std::cout << 4 << std::endl;



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


  g4hitntuple->Fill();
  
  m_t0.clear();
  m_edep.clear();
  m_eta.clear();
  m_phi.clear();
  m_z0.clear();
  m_y0.clear();
  m_x0.clear();
  m_primid.clear();
  m_primtrkid.clear();
  m_g4primtrkid.clear();
  m_g4primpt.clear();
  m_primpt.clear();
  m_primeta.clear();
  waveforms.clear();
  fitresults.clear();
  m_etabin.clear();
  m_phibin.clear();
  m_geoetabin.clear();
  m_geophibin.clear();
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
