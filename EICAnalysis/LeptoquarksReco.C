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

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cassert>

using namespace std;

LeptoquarksReco::LeptoquarksReco(std::string filename) :
	SubsysReco("LeptoquarksReco" ),
	_ievent(0),
	_filename(filename),
	_tfile(nullptr),
	_ntp_leptoquark(nullptr)
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
		"event:clusterid:towerid:calorimeterid:towereta:towerphi:towerbineta:towerbinphi:towerenergy:towerz");

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

	// for every recojet
	for (JetMap::Iter iter = recojets->begin();
	iter != recojets->end();
	++iter)
	{
		Jet* recojet = iter->second;

		float id    = recojet->get_id();
		float ncomp = recojet->size_comp();
		float eta   = recojet->get_eta();
		float phi   = recojet->get_phi();
		float e     = recojet->get_e();
		//float pt    = recojet->get_pt();

		if(e > max_energy) max_energy_id = id;

		//recojet->identify(std::cout);
		cout << "Found a jet: " << id << " " << e << " "<< ncomp << " "<< eta << " " << phi << endl;
	}

//	cout << "Maximum energy jet is:" << endl;
	Jet *max_energy_jet = recojets->get(max_energy_id);
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
//		cout << citer->second << endl;
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
					break;
				}
			}
		}

		if(tower_found)
		{
//			cout << "*******" << tower->get_energy() << endl;
//			cout << tower->get_bineta() << "   " << tower->get_binphi() << endl;

			RawTower::ShowerConstRange shower_begin_end = tower->get_g4showers();
			RawTower::ShowerConstIterator shower_iter;
			for (shower_iter = shower_begin_end.first; shower_iter !=  shower_begin_end.second; ++shower_iter) 
			{
				cout << "shower found!!" << endl;
			}
			

			RawTowerGeom * tower_geom = geom->get_tower_geometry(tower -> get_key());
			assert(tower_geom);

			double r = tower_geom->get_center_radius();
			double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
			double z0 = tower_geom->get_center_z();

			double z = z0 - vtxz;

			double eta = asinh(z/r); // eta after shift from vertex

//			cout << eta << "     " << phi << endl << endl;

			float lqjet_data[14] = {(float) _ievent,
				(float) citer->second,
				(float) tower->get_id(),
				(float) calorimeter,
				(float) eta,
				(float) phi,
				(float) tower->get_bineta(),
				(float) tower->get_binphi(),
				(float) tower->get_energy(),
				(float) z
			};

			_ntp_leptoquark->Fill(lqjet_data);

		}
		else cout << "******* ERROR: tower not found " << endl;
		tower_found = false;
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
