#include "LeptoquarksReco.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TString.h>
#include <TH2D.h>
#include <TDatabasePDG.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <g4cemc/RawTowerGeomContainer.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeom.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawTowerv1.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <g4main/PHG4Shower.h>
#include <g4main/PHG4Particle.h>
#include "g4main/PHG4TruthInfoContainer.h"
#include "g4eval/CaloRawTowerEval.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <typeinfo>
#include <string>

using namespace std;

LeptoquarksReco::LeptoquarksReco(std::string filename) :
	SubsysReco("LeptoquarksReco" ),
	_ievent(0),
	_filename(filename),
	_tfile(nullptr),
	_ntp_leptoquark(nullptr),
	_ntp_jet(nullptr),
	_truthinfo(nullptr)
{
	_filename = filename;


	//  _ebeam_E = 10;
	//  _pbeam_E = 250;
}

int
LeptoquarksReco::Init(PHCompositeNode *topNode)
{
	_verbose = false;
	_ievent = 0;

	_tfile = new TFile(_filename.c_str(), "RECREATE");
	_ntp_leptoquark = new TNtuple("ntp_leptoquark","all tower information from LQ events",
		"event:jetid:isMaxEnergyJet:jet_eta:jet_phi:towerid:calorimeterid:towereta:towerphi:towerbineta:towerbinphi:towerenergy:towerz:isTauTower:jet_mass");
	_ntp_jet = new TNtuple("ntp_jet","all jet information from LQ events",
		"event:jet_id:isMaxEnergyJet:jet_eta:jet_phi:jet_mass:jet_p:jet_pT:jet_eT:jet_e:jet_px:jet_py:jet_pz");



	return 0;
}

int
LeptoquarksReco::process_event(PHCompositeNode *topNode)
{
	_ievent ++;

	cout << endl;
	cout << "LeptoquarksReco: Processing event " << _ievent << endl;

	_truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");

	if (!_truthinfo) {
          cerr << PHWHERE << " ERROR: Can't find G4TruthInfo" << endl;
          exit(-1);
        }


	string recojetname = "AntiKt_Tower_r05";

	JetMap* recojets = findNode::getClass<JetMap>(topNode,recojetname.c_str());
	if (!recojets)
	{
		cerr << PHWHERE << " ERROR: Can't find " << recojetname << endl;
		exit(-1);
	}

//	float max_energy_id = 0;
//	float max_energy = 0;
	float is_max_energy_jet = 0;

	std::vector<float> energy_list;

	//loop over every recojet in the event to determine the maximum energy jet.
	for (JetMap::Iter iter = recojets->begin();
	iter != recojets->end();
	++iter)
	{
		Jet* recojet = iter->second;

//		float id    = recojet->get_id();
		float e     = recojet->get_e();
		energy_list.push_back(e);

//		if(e > max_energy) max_energy_id = id;
	}

	//Rank the entries in the energy_list by energy (highest energyy = 1, second highest = 2, etc.)
	vector<float> energy_list_sorted = energy_list;
	std::sort(energy_list_sorted.begin(), energy_list_sorted.end(), std::greater<float>());

	map<float, int> energyRankMap;
	for(int i = 0; (unsigned)i < energy_list_sorted.size(); i++)
	{
		energyRankMap.insert(make_pair(energy_list_sorted[i],i));
	}

	//loop over every jet identified in the event, this time we will be able to record which is the max energy jet and store this info.
	for (JetMap::Iter iter = recojets->begin();
	iter != recojets->end();
	++iter)
	{
		
//		cout << "Maximum energy jet is:" << endl;
//		Jet *max_energy_jet = recojets->get(max_energy_id);
		Jet *max_energy_jet = iter->second;	//Not actually the max energy jet anymore, just every jet.
//		if(max_energy_jet->get_e() < 0.1) continue;
//		if((iter->second)->get_id() == max_energy_id) is_max_energy_jet = 1;
//		else is_max_energy_jet = 0;

		float jet_eta = max_energy_jet->get_eta();
		float jet_phi = max_energy_jet->get_phi();
		float jet_mass = max_energy_jet->get_mass();
		float jet_momentum = max_energy_jet->get_p();
		float jet_trans_momentum = max_energy_jet->get_pt();
		float jet_trans_e = max_energy_jet->get_et();
		float jet_e = max_energy_jet->get_e();
		float jet_px = max_energy_jet->get_px();
		float jet_py = max_energy_jet->get_py();
		float jet_pz = max_energy_jet->get_pz();

		auto it = energyRankMap.find(max_energy_jet->get_e());
		is_max_energy_jet = it->second + 1;
//		cout << it->second + 1 << " " << max_energy_jet->get_e() << endl;

		GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
		if (!vertexmap) {
			cout << "ERROR: Vertex map not found" << endl;
		}
		GlobalVertex* vtx = vertexmap->begin()->second;
		float vtxz = NAN;
		if (vtx) vtxz = vtx->get_z();
		else cout << "ERROR: Global vertex not found" << endl;
		double calorimeter = 0;

		//Loop over all of the towers in a jet
		for (Jet::ConstIter citer = max_energy_jet->begin_comp(); citer != max_energy_jet->end_comp(); ++citer)
		{
			RawTowerContainer *towers = NULL;
			towers = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
			RawTowerContainer::ConstRange begin_end = towers->getTowers();
			RawTowerContainer::ConstIterator rtiter;

			RawTowerGeomContainer *geom = NULL;
			CaloRawTowerEval *towereval = NULL;

			//Look for each tower in the calorimeters
			RawTower *tower = NULL;
			bool tower_found = false;
			for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) 
			{
				RawTower *tower_i = rtiter->second;
				if(tower_i->get_id() == citer->second)
				{
					calorimeter = 1;
					geom = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_CEMC");
					tower = tower_i;
					tower_found = true;

					if ( _map_towereval.find("CEMC") == _map_towereval.end() )
					{
						_map_towereval.insert( make_pair( "CEMC", new CaloRawTowerEval(topNode, "CEMC") ) );
					}
					towereval = _map_towereval.find("CEMC")->second;

					break;
				}
			}
			if (tower_found == false)
			{
				RawTowerContainer *towers2 = NULL;
				towers2 = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALIN");
				RawTowerContainer::ConstRange begin_end2 = towers2->getTowers();
				RawTowerContainer::ConstIterator rtiter2;

				for (rtiter2 = begin_end2.first; rtiter2 !=  begin_end2.second; ++rtiter2) 
				{
					RawTower *tower_i = rtiter2->second;
					if(tower_i->get_id() == citer->second)
					{
						calorimeter = 2;
						geom = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_HCALIN");
						tower = tower_i;
						tower_found = true;

						if ( _map_towereval.find("HCALIN") == _map_towereval.end() )
						{
							_map_towereval.insert( make_pair( "HCALIN", new CaloRawTowerEval(topNode, "HCALIN") ) );
						}
						towereval = _map_towereval.find("HCALIN")->second;

						break;
					}
				}
			}
			if(tower_found == false)
			{
				RawTowerContainer *towers3 = NULL;
				towers3 = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALOUT");
				RawTowerContainer::ConstRange begin_end3 = towers3->getTowers();
				RawTowerContainer::ConstIterator rtiter3;

				for (rtiter3 = begin_end3.first; rtiter3 !=  begin_end3.second; ++rtiter3) 
				{
					RawTower *tower_i = rtiter3->second;
					if(tower_i->get_id() == citer->second)
					{
						calorimeter = 3;
						geom = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_HCALOUT");
						tower = tower_i;
						tower_found = true;

						if ( _map_towereval.find("HCALOUT") == _map_towereval.end() )
						{
							_map_towereval.insert( make_pair( "HCALOUT", new CaloRawTowerEval(topNode, "HCALOUT") ) );
						}
						towereval = _map_towereval.find("HCALOUT")->second;

						break;
					}
				}
			}
			if(tower_found == false)
			{
				RawTowerContainer *towers3 = NULL;
				towers3 = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_FEMC");
				RawTowerContainer::ConstRange begin_end3 = towers3->getTowers();
				RawTowerContainer::ConstIterator rtiter3;

				for (rtiter3 = begin_end3.first; rtiter3 !=  begin_end3.second; ++rtiter3) 
				{
					RawTower *tower_i = rtiter3->second;
					if(tower_i->get_id() == citer->second)
					{
						calorimeter = 11;
						geom = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_FEMC");
						tower = tower_i;
						tower_found = true;

						if ( _map_towereval.find("FEMC") == _map_towereval.end() )
						{
							_map_towereval.insert( make_pair( "FEMC", new CaloRawTowerEval(topNode, "FEMC") ) );
						}
						towereval = _map_towereval.find("FEMC")->second;

						break;
					}
				}
			}
			if(tower_found == false)
			{
				RawTowerContainer *towers3 = NULL;
				towers3 = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_FHCAL");
				RawTowerContainer::ConstRange begin_end3 = towers3->getTowers();
				RawTowerContainer::ConstIterator rtiter3;

				for (rtiter3 = begin_end3.first; rtiter3 !=  begin_end3.second; ++rtiter3) 
				{
					RawTower *tower_i = rtiter3->second;
					if(tower_i->get_id() == citer->second)
					{
						calorimeter = 12;
						geom = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_FHCAL");
						tower = tower_i;
						tower_found = true;

						if ( _map_towereval.find("FHCAL") == _map_towereval.end() )
						{
							_map_towereval.insert( make_pair( "FHCAL", new CaloRawTowerEval(topNode, "FHCAL") ) );
						}
						towereval = _map_towereval.find("FHCAL")->second;

						break;
					}
				}
			}
			if(tower_found == false)
			{
				RawTowerContainer *towers3 = NULL;
				towers3 = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_EEMC");
				RawTowerContainer::ConstRange begin_end3 = towers3->getTowers();
				RawTowerContainer::ConstIterator rtiter3;

				for (rtiter3 = begin_end3.first; rtiter3 !=  begin_end3.second; ++rtiter3) 
				{
					RawTower *tower_i = rtiter3->second;
					if(tower_i->get_id() == citer->second)
					{
						calorimeter = 21;
						geom = findNode::getClass<RawTowerGeomContainer>(topNode,"TOWERGEOM_EEMC");
						tower = tower_i;
						tower_found = true;

						if ( _map_towereval.find("EEMC") == _map_towereval.end() )
						{
							_map_towereval.insert( make_pair( "EEMC", new CaloRawTowerEval(topNode, "EEMC") ) );
						}
						towereval = _map_towereval.find("EEMC")->second;

						break;
					}
				}
			}

			// If the tower is found, get the PHG4Particle which contributes the most energy to that tower.
			if(tower_found)
			{
				int tau_tower = 0;
//				cout << endl <<  "Looking for primary particle in tower " << tower->get_id() << endl;

				PHG4Particle *particle_i = NULL;
				particle_i = (towereval->max_truth_primary_particle_by_energy(tower));

				if(!particle_i) 
				{
					cout << "*********Warning in LeptoquarksReco: Particle not found in tower " << tower->get_id() << ". May be noise." << endl;
//					continue;
				}
				else if(particle_i)
				{
//						cout 	<< "      Primary particle in tower: " 
// 							<< particle_i->get_pid() << " / "
//							<< particle_i->get_name() << " with energy: " 
//							<< particle_i->get_e() << " GeV" << endl;
//					if( particle_i->get_name() == "tau-" ) tau_tower = 1;
					tau_tower = 2;
				}
				else 
				{
					cout << "********ERROR in LeptoquarksReco: Condition should not be possible." << endl; 
				}

				RawTowerGeom * tower_geom = geom->get_tower_geometry(tower -> get_key());
				assert(tower_geom);

				double r = tower_geom->get_center_radius();
				double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
				double z0 = tower_geom->get_center_z();

				double z = z0 - vtxz;

				double eta = asinh(z/r); // eta after shift from vertex

				float lqjet_data[15] = {(float) _ievent,	//event number
					(float) (iter->second)->get_id(),	//jet id
					(float) is_max_energy_jet,		//is this the maximum energy jet?
					(float) jet_eta,			//eta of the jet
					(float) jet_phi,			//phi of the jet
					(float) tower->get_id(),		//tower id
					(float) calorimeter,			//calorimeter id. 1 = CEMC, 2 = HCALIN, 3 = HCALOUT
					(float) eta,				//eta of tower calculated from bin/calorimeter information
					(float) phi,				//phi of tower calculated from bin/calorimeter information
					(float) tower->get_bineta(),		//eta bin of tower
					(float) tower->get_binphi(),		//phi bin of tower
					(float) tower->get_energy(),		//energy deposited in tower
					(float) z,				//z position of calorimeter relative to interaction point
					(float) tau_tower,			//is the primary source of energy in this tower a tau?
					(float) jet_mass			//invarient mass of the jet
				};

				_ntp_leptoquark->Fill(lqjet_data);

			}
			else cout << "******* ERROR in LeptoquarksReco: tower not found. Calorimeter may not be defined in LeptoquarksReco. Skipping. " << endl;
			tower_found = false;
		}

		float lqjet_data[14] = {(float) _ievent,	//event number
			(float) (max_energy_jet)->get_id(),	//jet id
			(float) is_max_energy_jet,		//is this the maximum energy jet?
			(float) jet_eta,			//
			(float) jet_phi,
			(float) jet_mass,
			(float) jet_momentum,
			(float) jet_trans_momentum,
			(float) jet_trans_e,
			(float) jet_e,
			(float) jet_px,
			(float) jet_py,
			(float) jet_pz
			};

		_ntp_jet->Fill(lqjet_data);

		is_max_energy_jet = 0;
	}
	return 0;
}

int
LeptoquarksReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();
  _ntp_leptoquark->Write();
  _ntp_jet->Write();
  _tfile->Close();

  return 0;
}
