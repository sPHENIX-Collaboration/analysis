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
	_ntp_leptoquark = new TNtuple("ntp_leptoquark","max energy jet from LQ events",
		"event:jetid:isMaxEnergyJet:clusterid:towerid:calorimeterid:towereta:towerphi:towerbineta:towerbinphi:towerenergy:towerz:isTauTower");

	_truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");

	return 0;
}

int
LeptoquarksReco::process_event(PHCompositeNode *topNode)
{
	_ievent ++;

	cout << endl;
	cout << "Processing event " << _ievent << endl;

	string recojetname = "AntiKt_Tower_r05";

	JetMap* recojets = findNode::getClass<JetMap>(topNode,recojetname.c_str());
	if (!recojets)
	{
	cerr << PHWHERE << " ERROR: Can't find " << recojetname << endl;
	exit(-1);
	}

	float max_energy_id = 0;
	float max_energy = 0;
	float is_max_energy_jet = 0;
//	int jet_count = 0;

	// for every recojet
	for (JetMap::Iter iter = recojets->begin();
	iter != recojets->end();
	++iter)
	{
		Jet* recojet = iter->second;

		float id    = recojet->get_id();
//		float ncomp = recojet->size_comp();
//		float eta   = recojet->get_eta();
//		float phi   = recojet->get_phi();
		float e     = recojet->get_e();
//		float pt    = recojet->get_pt();

		if(e > max_energy) max_energy_id = id;

		//recojet->identify(std::cout);
//		cout << "Found a jet: " << id << " " << e << " "<< ncomp << " "<< eta << " " << phi << endl;
	}

	for (JetMap::Iter iter = recojets->begin();
	iter != recojets->end();
	++iter)
	{
		
	//	cout << "Maximum energy jet is:" << endl;
	//	Jet *max_energy_jet = recojets->get(max_energy_id);
		Jet *max_energy_jet = iter->second;
//		if(max_energy_jet->get_e() < 0.1) continue;
		if((iter->second)->get_id() == max_energy_id) is_max_energy_jet = 1;
		else is_max_energy_jet = 0;
	//	cout << is_max_energy_jet << endl;
	//	max_energy_jet->identify(std::cout);

		RawTowerContainer *towers = NULL;
		towers = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
		RawTowerContainer::ConstRange begin_end = towers->getTowers();
		RawTowerContainer::ConstIterator rtiter;

		RawTowerContainer *towers2 = NULL;
		towers2 = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALIN");
		RawTowerContainer::ConstRange begin_end2 = towers2->getTowers();
		RawTowerContainer::ConstIterator rtiter2;

		RawTowerContainer *towers3 = NULL;
		towers3 = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_HCALOUT");
		RawTowerContainer::ConstRange begin_end3 = towers3->getTowers();
		RawTowerContainer::ConstIterator rtiter3;

		RawTowerGeomContainer *geom = NULL;
		CaloRawTowerEval *towereval = NULL;

		GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
		if (!vertexmap) {
			cout << "ERROR: Vertex map not found" << endl;
		}
		GlobalVertex* vtx = vertexmap->begin()->second;
		float vtxz = NAN;
		if (vtx) vtxz = vtx->get_z();
		else cout << "ERROR: Global vertex not found" << endl;
		double calorimeter = 0;

		for (Jet::ConstIter citer = max_energy_jet->begin_comp(); citer != max_energy_jet->end_comp(); ++citer)
		{
	//		cout << "Jet: " << citer->first << " " << citer->second << endl;
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

			if(tower_found)
			{
				int tau_tower = 0;
//				cout << "Looking for primary particle:" << endl;
				if( (towereval->max_truth_primary_particle_by_energy(tower)) )
				{
					PHG4Particle *particle_i = NULL;
					particle_i = (towereval->max_truth_primary_particle_by_energy(tower));
					if(!particle_i) 
					{
						cout << "*********ERROR: Particle not found" << endl;
						continue;
					}
					else if(particle_i)
//					std::string type = typeid( (towereval->max_truth_primary_particle_by_energy(tower)) ).name();
//					cout << type << endl;
//					if(type.find("PHG4Particle") != std::string::npos)
					{
//						cout << " ding" << endl;
//						particle_i->identify(std::cout);
//						cout 	<< "      Primary particle in tower: " 
// 							<< particle_i->get_pid() << " / "
//							<< particle_i->get_name() << " with energy: " 
//							<< particle_i->get_e() << " GeV" << endl;
						if( particle_i->get_name() == "tau-" ) tau_tower = 1;
						else tau_tower = 2;
//						cout << " ding" << endl << endl;
					}
					else { cout << "ERROR: TypeID is: " << /*type <<*/ " not PHG4Particle." << endl; }
				}
				else
				{
					tau_tower = 0;
//					cout << "NONE found" << endl;
				}

				RawTowerGeom * tower_geom = geom->get_tower_geometry(tower -> get_key());
				assert(tower_geom);

				double r = tower_geom->get_center_radius();
				double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
				double z0 = tower_geom->get_center_z();

				double z = z0 - vtxz;

				double eta = asinh(z/r); // eta after shift from vertex

				float lqjet_data[14] = {(float) _ievent,	//event number
					(float) (iter->second)->get_id(),	//jet id
					(float) is_max_energy_jet,		// is this the maximum energy jet?
					(float) citer->second,
					(float) tower->get_id(),
					(float) calorimeter,
					(float) eta,
					(float) phi,
					(float) tower->get_bineta(),
					(float) tower->get_binphi(),
					(float) tower->get_energy(),
					(float) z,
					(float) tau_tower
				};

				_ntp_leptoquark->Fill(lqjet_data);

			}
			else cout << "******* ERROR: tower not found. Calorimeter may not be defined in LeptoquarksReco. Skipping. " << endl;
			tower_found = false;
		}
		is_max_energy_jet = 0;
	}
	return 0;
}

int
LeptoquarksReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();
  _ntp_leptoquark->Write();
  _tfile->Close();

  return 0;
}
