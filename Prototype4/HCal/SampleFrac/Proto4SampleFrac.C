#include "Proto4SampleFrac.h"

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
           
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
           
#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawClusterEval.h> 
#include <g4eval/CaloRawTowerEval.h>
#include <g4eval/CaloTruthEval.h>
#include <g4eval/SvtxEvalStack.h>
           
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
           
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
           
#include <TString.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TMath.h>
           
#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;

Proto4SampleFrac::Proto4SampleFrac(const std::string &calo_name, const std::string &filename)
  : SubsysReco("Proto4SampleFrac_" + calo_name),
  _is_sim(true),
  _filename(filename),
  _calo_name(calo_name), 
  _calo_hit_container(nullptr), _calo_abs_hit_container(nullptr), _truth_container(nullptr)
{
  Verbosity(1);
}

Proto4SampleFrac::~Proto4SampleFrac()
{
}

Fun4AllHistoManager* Proto4SampleFrac::get_HistoManager()
{
  Fun4AllServer *se = Fun4AllServer::instance();
  Fun4AllHistoManager *hm = se->getHistoManager("Proto4SampleFrac_HISTOS");

  if (not hm)
  {
    cout
        << "Proto4SampleFrac::get_HistoManager - Making Fun4AllHistoManager Proto4SampleFrac_HISTOS"
        << endl;
    hm = new Fun4AllHistoManager("Proto4SampleFrac_HISTOS");
    se->registerHistoManager(hm);
  }

  assert(hm);

  return hm;
}

                            
string Proto4SampleFrac::get_histo_prefix()
{                           
  return "h_QAG4Sim_" + string(_calo_name);
}                           

int Proto4SampleFrac::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity())
    cout << "Proto4SampleFrac::InitRun" << endl;

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = static_cast<PHCompositeNode *>(iter.findFirst(
      "PHCompositeNode", "DST"));
  assert(dstNode);

  if (!dstNode)
  {
    std::cerr << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    throw runtime_error(
        "Failed to find DST node in EmcRawTowerBuilder::CreateNodes");
  }

  _calo_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_" + _calo_name);
  if (!_calo_hit_container)
  {
    cout << "Proto4SampleFrac::InitRun - Fatal Error - "
      << "unable to find DST node " << "G4HIT_" + _calo_name << endl;
    assert(_calo_hit_container);
  }

  _calo_abs_hit_container = findNode::getClass<PHG4HitContainer>(topNode,
      "G4HIT_ABSORBER_" + _calo_name);
  if (!_calo_abs_hit_container)
  {
    cout << "Proto4SampleFrac::InitRun - Fatal Error - "
      << "unable to find DST node " << "G4HIT_ABSORBER_" + _calo_name
      << endl;
    assert(_calo_abs_hit_container);
  }
             
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode,
      "G4TruthInfo");
  if (!_truth_container)
  {      
    cout << "Proto4SampleFrac::InitRun - Fatal Error - "
      << "unable to find DST node " << "G4TruthInfo" << endl;
    assert(_truth_container);
  }     

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4SampleFrac::End(PHCompositeNode *topNode)
{
  cout << "Proto4SampleFrac::End - write to " << _filename << endl;
  PHTFileServer::get().cd(_filename);

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);
  for (unsigned int i = 0; i < hm->nHistos(); i++)
    hm->getHisto(i)->Write();

  //  if (_T_EMCalTrk)
  //    _T_EMCalTrk->Write();

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4SampleFrac::Init(PHCompositeNode *topNode)
{
  cout << "Proto4SampleFrac::get_HistoManager - Making PHTFileServer "
       << _filename << endl;
  PHTFileServer::get().open(_filename, "RECREATE");

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  TH1D * h_info = new TH1D(TString(get_histo_prefix()) + "_Normalization", //
      TString(_calo_name) + " Normalization;Items;Count", 10, .5, 10.5);
  int i = 1;
  h_info->GetXaxis()->SetBinLabel(i++, "Event");
  h_info->GetXaxis()->SetBinLabel(i++, "G4Hit Active");
  h_info->GetXaxis()->SetBinLabel(i++, "G4Hit Absor.");
  h_info->GetXaxis()->LabelsOption("v");
  hm->registerHisto(h_info);

  hm->registerHisto(                           
      new TH2F(TString(get_histo_prefix()) + "_G4Hit_RZ", //
	TString(_calo_name) + " RZ projection;Z (cm);R (cm)", 1200, -300, 300,
	600, 0, 300));                    

  hm->registerHisto(                           
      new TH2F(TString(get_histo_prefix()) + "_G4Hit_XY_cal", //
	TString(_calo_name) + " XY projection;X (cm);Y (cm)", 3000, 50, 350,
	2000, -100, 100));
	// TString(_calo_name) + " XY projection;X (cm);Y (cm)", 1200, -300, 300,
	// 1200, -300, 300));

  hm->registerHisto(                           
      new TH2F(TString(get_histo_prefix()) + "_G4Hit_XY_abs", //
	TString(_calo_name) + " XY projection;X (cm);Y (cm)", 3000, 50, 350,
	2000, -100, 100));

  hm->registerHisto(                           
      new TH2F(TString(get_histo_prefix()) + "_G4Hit_Mat_XY_cal", //
	TString(_calo_name) + " XY projection;X (cm);Y (cm)", 3000, 50, 350,
	2000, -100, 100));

  hm->registerHisto(                           
      new TH2F(TString(get_histo_prefix()) + "_G4Hit_Mat_XY_abs", //
	TString(_calo_name) + " XY projection;X (cm);Y (cm)", 3000, 50, 350,
	2000, -100, 100));

  hm->registerHisto(                           
      new TH2F(TString(get_histo_prefix()) + "_G4Hit_LateralTruthProjection", //
	TString(_calo_name)                  
	+ " shower lateral projection (last primary);Polar direction (cm);Azimuthal direction (cm)",
	200, -30, 30, 200, -30, 30));        

  hm->registerHisto(new TH1F(TString(get_histo_prefix()) + "_G4Hit_SF", //
	TString(_calo_name) + " sampling fraction;Sampling fraction", 1000, 0, 1.0));

  hm->registerHisto(                                                                                     
      new TH1F(TString(get_histo_prefix()) + "_G4Hit_VSF", //
	TString(_calo_name)
	+ " visible sampling fraction;Visible sampling fraction", 1000, 0, 1.0));              

  TH1F * h =                 
    new TH1F(TString(get_histo_prefix()) + "_G4Hit_HitTime", //
	TString(_calo_name)
	+ " hit time (edep weighting);Hit time - T0 (ns);Geant4 energy density",
	1000, 0.5, 10000);
  hm->registerHisto(h);

  hm->registerHisto(
      new TH1F(TString(get_histo_prefix()) + "_G4Hit_FractionTruthEnergy", //
	TString(_calo_name)
	+ " fraction truth energy ;G4 energy (active + absorber) / total truth energy",
	1000, 0, 1));

  hm->registerHisto(
      new TH1F(TString(get_histo_prefix()) + "_G4Hit_FractionEMVisibleEnergy", //
	TString(_calo_name)
	+ " fraction visible energy from EM; visible energy from e^{#pm} / total visible energy",
	100, 0, 1));

  return Fun4AllReturnCodes::EVENT_OK;
}

int Proto4SampleFrac::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 2) cout << "Proto4SampleFrac::process_event() entered" << endl;

  TH1F *h = nullptr;

  Fun4AllHistoManager *hm = get_HistoManager();
  assert(hm);

  TH1D* h_info = dynamic_cast<TH1D*>(hm->getHisto(get_histo_prefix() + "_Normalization"));
  assert(h_info);

  // get primary
  assert(_truth_container); 
  PHG4TruthInfoContainer::ConstRange primary_range =
    _truth_container->GetPrimaryParticleRange();
  double total_primary_energy = 1e-9; //make it zero energy epsilon samll so it can be used for denominator
  for (PHG4TruthInfoContainer::ConstIterator particle_iter = primary_range.first;
      particle_iter != primary_range.second; ++particle_iter)
  {           
    const PHG4Particle *particle = particle_iter->second;
    assert(particle);     
    total_primary_energy += particle->get_e();
  }           

  assert(not _truth_container->GetMap().empty());
  const PHG4Particle * last_primary = _truth_container->GetMap().rbegin()->second;
  assert(last_primary);
                                                                                                         
  if (Verbosity() > 2)
  {    
    cout
      << "QAG4SimulationCalorimeter::process_event_G4Hit() handle this truth particle"
      << endl;
    last_primary->identify();
  }    
  const PHG4VtxPoint* primary_vtx = //
    _truth_container->GetPrimaryVtx(last_primary->get_vtx_id());
  assert(primary_vtx);
  if (Verbosity() > 2)
  {    
    cout
      << "QAG4SimulationCalorimeter::process_event_G4Hit() handle this vertex"
      << endl;
    primary_vtx->identify();
  }

  const double t0 = primary_vtx->get_t();
  const TVector3 vertex(primary_vtx->get_x(), primary_vtx->get_y(),
      primary_vtx->get_z());

  // projection axis
  TVector3 axis_proj(last_primary->get_px(), last_primary->get_py(),
      last_primary->get_pz());
  if (axis_proj.Mag() == 0)
    axis_proj.SetXYZ(0, 0, 1);
  axis_proj = axis_proj.Unit();

  // azimuthal direction axis
  TVector3 axis_azimuth = axis_proj.Cross(TVector3(0, 0, 1));
  if (axis_azimuth.Mag() == 0)
    axis_azimuth.SetXYZ(1, 0, 0);
  axis_azimuth = axis_azimuth.Unit();

  // polar direction axis
  TVector3 axis_polar = axis_proj.Cross(axis_azimuth);
  assert(axis_polar.Mag() > 0);
  axis_polar = axis_polar.Unit();

  double e_calo = 0.0; // active energy deposition
  double ev_calo = 0.0; // visible energy
  double ea_calo = 0.0; // absorber energy
  double ev_calo_em = 0.0; // EM visible energy

  if (_calo_hit_container)
  {
    TH2F * hrz = dynamic_cast<TH2F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_RZ"));
    assert(hrz);

    TH2F * hxy_cal = dynamic_cast<TH2F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_XY_cal"));
    assert(hxy_cal);

    TH2F * hmat_xy_cal = dynamic_cast<TH2F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_Mat_XY_cal"));
    assert(hmat_xy_cal);

    TH1F * ht = dynamic_cast<TH1F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_HitTime"));
    assert(ht);

    TH2F * hlat = dynamic_cast<TH2F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_LateralTruthProjection"));
    assert(hlat);

    h_info->Fill("G4Hit Active", _calo_hit_container->size());
    PHG4HitContainer::ConstRange calo_hit_range =
      _calo_hit_container->getHits();
    for (PHG4HitContainer::ConstIterator hit_iter = calo_hit_range.first;
	hit_iter != calo_hit_range.second; hit_iter++)
    {

      PHG4Hit *this_hit = hit_iter->second;
      assert(this_hit);

      e_calo += this_hit->get_edep();
      ev_calo += this_hit->get_light_yield();

      // EM visible energy that is only associated with electron energy deposition
      PHG4Particle* particle = _truth_container->GetParticle(
	  this_hit->get_trkid());
      if (!particle)
      {
	cout <<__PRETTY_FUNCTION__<<" - Error - this PHG4hit missing particle: "; this_hit -> identify();
      }
      assert(particle);
      if (abs(particle->get_pid()) == 11)
	ev_calo_em += this_hit->get_light_yield();

      const TVector3 hit(this_hit->get_avg_x(), this_hit->get_avg_y(),
	  this_hit->get_avg_z());

      hrz->Fill(hit.Z(), hit.Perp(), this_hit->get_edep());
      hxy_cal->Fill(hit.X(), hit.Y(), this_hit->get_edep());
      ht->Fill(this_hit->get_avg_t() - t0, this_hit->get_edep());

      const double hit_azimuth = axis_azimuth.Dot(hit - vertex);
      const double hit_polar = axis_polar.Dot(hit - vertex);
      hlat->Fill(hit_polar, hit_azimuth, this_hit->get_edep());

      const TVector3 hit_entry(this_hit->get_x(0), this_hit->get_y(0), this_hit->get_z(0));
      const TVector3 hit_exit(this_hit->get_x(1), this_hit->get_y(1), this_hit->get_z(1));
      hmat_xy_cal->Fill(hit_entry.X(),hit_entry.Y());
      hmat_xy_cal->Fill(hit_exit.X(),hit_exit.Y());
    }
  }

  if (_calo_abs_hit_container)
  {
    h_info->Fill("G4Hit Absor.", _calo_abs_hit_container->size());

    TH2F * hxy_abs = dynamic_cast<TH2F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_XY_abs"));
    assert(hxy_abs);

    TH2F * hmat_xy_abs = dynamic_cast<TH2F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_Mat_XY_abs"));
    assert(hmat_xy_abs);

    PHG4HitContainer::ConstRange calo_abs_hit_range =
      _calo_abs_hit_container->getHits();
    for (PHG4HitContainer::ConstIterator hit_iter = calo_abs_hit_range.first;
	hit_iter != calo_abs_hit_range.second; hit_iter++)
    {

      PHG4Hit *this_hit = hit_iter->second;
      assert(this_hit);

      ea_calo += this_hit->get_edep();

      const TVector3 hit(this_hit->get_avg_x(), this_hit->get_avg_y(),
	  this_hit->get_avg_z());

      hxy_abs->Fill(hit.X(), hit.Y(), this_hit->get_edep());

      const TVector3 hit_entry(this_hit->get_x(0), this_hit->get_y(0), this_hit->get_z(0));
      const TVector3 hit_exit(this_hit->get_x(1), this_hit->get_y(1), this_hit->get_z(1));
      hmat_xy_abs->Fill(hit_entry.X(),hit_entry.Y());
      hmat_xy_abs->Fill(hit_exit.X(),hit_exit.Y());
    }
  }

  if (Verbosity() > 3)
    cout << "QAG4SimulationCalorimeter::process_event_G4Hit::" << _calo_name
      << " - SF = " << e_calo / (e_calo + ea_calo + 1e-9) << ", VSF = "
      << ev_calo / (e_calo + ea_calo + 1e-9) << endl;

  if (e_calo + ea_calo > 0)
  {       
    h = dynamic_cast<TH1F*>(hm->getHisto(get_histo_prefix() + "_G4Hit_SF"));
    assert(h);
    h->Fill(e_calo / (e_calo + ea_calo));

    h = dynamic_cast<TH1F*>(hm->getHisto(get_histo_prefix() + "_G4Hit_VSF"));
    assert(h);
    h->Fill(ev_calo / (e_calo + ea_calo));
  }       

  h = dynamic_cast<TH1F*>(hm->getHisto(
	get_histo_prefix() + "_G4Hit_FractionTruthEnergy"));
  assert(h);
  h->Fill((e_calo + ea_calo) / total_primary_energy);

  if (ev_calo > 0)
  {       
    h = dynamic_cast<TH1F*>(hm->getHisto(
	  get_histo_prefix() + "_G4Hit_FractionEMVisibleEnergy"));
    assert(h);
    h->Fill(ev_calo_em / (ev_calo));
  }

  if (Verbosity() > 3)
    cout << "QAG4SimulationCalorimeter::process_event_G4Hit::" << _calo_name
      << " - histogram " << h->GetName() << " Get Sum = " << h->GetSum()
      << endl;


  // at the end, count success events
  h_info->Fill("Event", 1); 

  return Fun4AllReturnCodes::EVENT_OK;
}

pair<int, int>
Proto4SampleFrac::find_max(RawTowerContainer *towers, int cluster_size)
{
  const int clus_edge_size = (cluster_size - 1) / 2;
  assert(clus_edge_size >= 0);

  pair<int, int> max(-1000, -1000);
  double max_e = 0;

  for (int col = 0; col < n_size; ++col)
    for (int row = 0; row < n_size; ++row)
    {
      double energy = 0;

      for (int dcol = col - clus_edge_size; dcol <= col + clus_edge_size;
           ++dcol)
        for (int drow = row - clus_edge_size; drow <= row + clus_edge_size;
             ++drow)
        {
          if (dcol < 0 or drow < 0)
            continue;

          RawTower *t = towers->getTower(dcol, drow);
          if (t)
            energy += t->get_energy();
        }

      if (energy > max_e)
      {
        max = make_pair(col, row);
        max_e = energy;
      }
    }

  return max;
}
