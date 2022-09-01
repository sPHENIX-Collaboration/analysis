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

// Cluster includes
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <TProfile.h>
#include <TFile.h>
#include <TNtuple.h>
#include <TMath.h>
#include <cassert>
#include <sstream>
#include <string>
#include <TF1.h>

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
  , detector("HCALIN")
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

  return 0;
}

int CaloWaveFormSim::process_event(PHCompositeNode* topNode)
{

  process_g4hits(topNode);

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloWaveFormSim::process_g4hits(PHCompositeNode* topNode)
{
  float waveform[24576][16];
  float tedep[24576];
  for (int i = 0; i < 24576;i++)
    {
      for (int j = 0; j < 16;j++)
	{
	  m_waveform[i][j] = 0.0;
	  waveform[i][j] = 0.0;
	}      
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
   
  RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!geomEM)
    {
      std::cout << "hey no geometry node this is wrong!" << std::endl;
    }

  ostringstream nodename;
  nodename.str("");
  nodename << "G4HIT_" << detector;
  PHG4HitContainer* hits = findNode::getClass<PHG4HitContainer>(topNode, nodename.str().c_str());

  //-----------------------------------------------------------------------------------------
  //Loop over truth primary particles and record their kinematics
  //-----------------------------------------------------------------------------------------
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
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

      float x0 = hit_iter->second->get_x(0);
      float y0 = hit_iter->second->get_y(0);
      float z0 = hit_iter->second->get_z(0);
      float r0 = TMath::Sqrt(x0*x0+y0*y0);
      float phi = TMath::ATan2(y0,x0);
      float theta = TMath::ATan(r0/z0);
      float eta = -1*TMath::Log(TMath::Tan(fabs(theta/2)));
      if (part->get_pz() < 0 )
	{
	  eta = -1 * eta;
	}
      //-------------------------------------------------------------------------
      //Map the G4hits to the corresponding CEMC tower
      //-------------------------------------------------------------------------
      int etabin = geomEM->get_etabin(eta);
      int phibin = geomEM->get_phibin(phi);
      int towernumber = etabin + 96*phibin;
      //---------------------------------------------------------------------
      //Convert the G4hit into a waveform contribution
      //---------------------------------------------------------------------
      float t0 = 0.5*(hit_iter->second->get_t(0)+hit_iter->second->get_t(1)) / 16.66667;   //Average of g4hit time downscaled by 16.667 ns/time sample 
      f_fit->SetParameters(hit_iter->second->get_edep()*673,_shiftval+t0,0);            //Set the waveform template to match the expected signal from such a hit
      tedep[towernumber] += hit_iter->second->get_edep()*673;    // add g4hit adc deposition to the total deposition (scale 673 maps GeV to ADC from test beam data) 
      //-------------------------------------------------------------------------------------------------------------
      //For each tower add the new waveform contribution to the total waveform
       //-------------------------------------------------------------------------------------------------------------
     if (hit_iter->second->get_edep()*673 > 1)
	{
	  for (int j = 0; j < 16;j++) // 16 is the number of time samples
	    {
	      waveform[towernumber][j] +=f_fit->Eval(j);
	    }
	  m_ndep[towernumber] +=1;
	}
      m_eta.push_back(eta);
      m_phi.push_back(phi);
      m_t0.push_back(hit_iter->second->get_t(0));
      m_x0.push_back(hit_iter->second->get_x(0));
      m_y0.push_back(hit_iter->second->get_y(0));
      m_z0.push_back(hit_iter->second->get_z(0));
      m_edep.push_back(hit_iter->second->get_edep());
    }
  }
  //----------------------------------------------------------------------------------------
  //For each tower loop over add a noise waveform 
  //(from cosmic data, gain is too high but is a source of noise)
  //----------------------------------------------------------------------------------------
  for (int i = 0; i < 24576;i++)
    {
      int noise_waveform  = (int)rnd->Uniform(0,383999);
      noise->GetEntry(noise_waveform);
      m_tedep[i] = tedep[i];
      for (int k = 0; k < 16;k++)
	{
	  m_waveform[i][k] = waveform[i][k]+noise_val[k];
	}
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
