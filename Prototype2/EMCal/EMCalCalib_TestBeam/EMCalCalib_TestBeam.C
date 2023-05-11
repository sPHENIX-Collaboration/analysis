#include "EMCalCalib_TestBeam.h"
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>
#include <g4hough/SvtxVertexMap.h>
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
#include <TNtuple.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <cmath>


using namespace std;
// TFile *f_TB=new TFile("output_Tower21_UIUC_2ndenergyscan_Production_0510_MIP_set4_Cosmic_16GeV_2280_hodo_tree11_Jul13.root","RECREATE");

//TFile *f_TB=new TFile("output_0degree_45degree_energyscan_minus16GeV_2861_hodo_tree7_Jun17.root","RECREATE");
 TFile *f_TB=new TFile("output_test.root","RECREATE");

EMCalCalib_TestBeam::EMCalCalib_TestBeam(const std::string &filename) :

  SubsysReco("EMCalCalib_TestBeam"),  _T_EMCalTrk(NULL), _filename(filename), _ievent(0)

{

  Emax_t=0;
  verbosity = 0;

  
  test1=new TTree("test1","trial file");
  test1->Branch("Etotal_t",&Etotal_t,"Etotal_t/F");
  test1->Branch("Emax_t",&Emax_t,"Emax_t/F");
  test1->Branch("E3by3_t",&E3by3_t,"E3by3_t/F");
  test1->Branch("E5by5_t",&E5by5_t,"E5by5_t/F");
  test1->Branch("C1_t",&C1_t,"C1_t/F");
  test1->Branch("C2_inner_t",&C2_inner_t,"C2_inner_t/F");
  test1->Branch("C2_outer_t",&C2_outer_t,"C2_outer_t/F");
  test1->Branch("C2_inner_new_t",&C2_inner_new_t,"C2_inner_new_t/F");
  test1->Branch("C2_outer_new_t",&C2_outer_new_t,"C2_outer_new_t/F");

  test1->Branch("Veto1_t",&Veto1_t,"Veto1_t/F");
  test1->Branch("Veto2_t",&Veto2_t,"Veto2_t/F");
  test1->Branch("Veto3_t",&Veto3_t,"Veto3_t/F");
  test1->Branch("Veto4_t",&Veto4_t,"Veto4_t/F");
  test1->Branch("Horz_HODO_R0_t",&Horz_HODO_R0_t,"Horz_HODO_R0_t/F");
  test1->Branch("Horz_HODO_R1_t",&Horz_HODO_R1_t,"Horz_HODO_R1_t/F");
  test1->Branch("Horz_HODO_R2_t",&Horz_HODO_R2_t,"Horz_HODO_R2_t/F");
  test1->Branch("Horz_HODO_R3_t",&Horz_HODO_R3_t,"Horz_HODO_R3_t/F");
  test1->Branch("Horz_HODO_R4_t",&Horz_HODO_R4_t,"Horz_HODO_R4_t/F");
  test1->Branch("Horz_HODO_R5_t",&Horz_HODO_R5_t,"Horz_HODO_R5_t/F");
  test1->Branch("Horz_HODO_R6_t",&Horz_HODO_R6_t,"Horz_HODO_R6_t/F");
  test1->Branch("Horz_HODO_R7_t",&Horz_HODO_R7_t,"Horz_HODO_R7_t/F");
  test1->Branch("Vert_HODO_R0_t",&Vert_HODO_R0_t,"Vert_HODO_R0_t/F");
  test1->Branch("Vert_HODO_R1_t",&Vert_HODO_R1_t,"Vert_HODO_R1_t/F");
  test1->Branch("Vert_HODO_R2_t",&Vert_HODO_R2_t,"Vert_HODO_R2_t/F");
  test1->Branch("Vert_HODO_R3_t",&Vert_HODO_R3_t,"Vert_HODO_R3_t/F");
  test1->Branch("Vert_HODO_R4_t",&Vert_HODO_R4_t,"Vert_HODO_R4_t/F");
  test1->Branch("Vert_HODO_R5_t",&Vert_HODO_R5_t,"Vert_HODO_R5_t/F");
  test1->Branch("Vert_HODO_R6_t",&Vert_HODO_R6_t,"Vert_HODO_R6_t/F");
  test1->Branch("Vert_HODO_R7_t",&Vert_HODO_R7_t,"Vert_HODO_R7_t/F");
  test1->Branch("TowerID_t",&TowerID_t,"TowerID_t/F");
  test1->Branch("Tower_column_t",&Tower_column_t,"Tower_column_t/F");
  test1->Branch("Tower_row_t",&Tower_row_t,"Tower_row_t/F");
  test1->Branch("TowerE_column_0_t",&TowerE_column_0_t,"TowerE_column_0_t/F");
  test1->Branch("TowerE_column_1_t",&TowerE_column_1_t,"TowerE_column_1_t/F");
  test1->Branch("TowerE_column_2_t",&TowerE_column_2_t,"TowerE_column_2_t/F");
  test1->Branch("TowerE_column_3_t",&TowerE_column_3_t,"TowerE_column_3_t/F");
  test1->Branch("TowerE_column_4_t",&TowerE_column_4_t,"TowerE_column_4_t/F");
  test1->Branch("TowerE_column_5_t",&TowerE_column_5_t,"TowerE_column_5_t/F");
  test1->Branch("TowerE_column_6_t",&TowerE_column_6_t,"TowerE_column_6_t/F");
  test1->Branch("TowerE_column_7_t",&TowerE_column_7_t,"TowerE_column_7_t/F");
  test1->Branch("TowerE_row_0_t",&TowerE_row_0_t,"TowerE_row_0_t/F");
  test1->Branch("TowerE_row_1_t",&TowerE_row_1_t,"TowerE_row_1_t/F");
  test1->Branch("TowerE_row_2_t",&TowerE_row_2_t,"TowerE_row_2_t/F");
  test1->Branch("TowerE_row_3_t",&TowerE_row_3_t,"TowerE_row_3_t/F");
  test1->Branch("TowerE_row_4_t",&TowerE_row_4_t,"TowerE_row_4_t/F");
  test1->Branch("TowerE_row_5_t",&TowerE_row_5_t,"TowerE_row_5_t/F");
  test1->Branch("TowerE_row_6_t",&TowerE_row_6_t,"TowerE_row_6_t/F");
  test1->Branch("TowerE_row_7_t",&TowerE_row_7_t,"TowerE_row_7_t/F");
  test1->Branch("HorzTowerID_t",&HorzTowerID_t,"HorzTowerID_t/F");
  test1->Branch("VertTowerID_t",&VertTowerID_t,"VertTowerID_t/F");
  test1->Branch("SaveHoriz_TowerID0_t",&SaveHoriz_TowerID0_t,"SaveHoriz_TowerID0_t/F");
  test1->Branch("SaveHoriz_TowerID1_t",&SaveHoriz_TowerID1_t,"SaveHoriz_TowerID1_t/F");
  test1->Branch("SaveHoriz_TowerID2_t",&SaveHoriz_TowerID2_t,"SaveHoriz_TowerID2_t/F");
  test1->Branch("SaveHoriz_TowerID3_t",&SaveHoriz_TowerID3_t,"SaveHoriz_TowerID3_t/F");
  test1->Branch("SaveHoriz_TowerID4_t",&SaveHoriz_TowerID4_t,"SaveHoriz_TowerID4_t/F");
  test1->Branch("SaveHoriz_TowerID5_t",&SaveHoriz_TowerID5_t,"SaveHoriz_TowerID5_t/F");
  test1->Branch("SaveHoriz_TowerID6_t",&SaveHoriz_TowerID6_t,"SaveHoriz_TowerID6_t/F");
  test1->Branch("SaveHoriz_TowerID7_t",&SaveHoriz_TowerID7_t,"SaveHoriz_TowerID7_t/F");
  test1->Branch("SaveVert_TowerID0_t",&SaveVert_TowerID0_t,"SaveVert_TowerID0_t/F");
  test1->Branch("SaveVert_TowerID1_t",&SaveVert_TowerID1_t,"SaveVert_TowerID1_t/F");
  test1->Branch("SaveVert_TowerID2_t",&SaveVert_TowerID2_t,"SaveVert_TowerID2_t/F");
  test1->Branch("SaveVert_TowerID3_t",&SaveVert_TowerID3_t,"SaveVert_TowerID3_t/F");
  test1->Branch("SaveVert_TowerID4_t",&SaveVert_TowerID4_t,"SaveVert_TowerID4_t/F");
  test1->Branch("SaveVert_TowerID5_t",&SaveVert_TowerID5_t,"SaveVert_TowerID5_t/F");
  test1->Branch("SaveVert_TowerID6_t",&SaveVert_TowerID6_t,"SaveVert_TowerID6_t/F");
  test1->Branch("SaveVert_TowerID7_t",&SaveVert_TowerID7_t,"SaveVert_TowerID7_t/F");

 
}


EMCalCalib_TestBeam::~EMCalCalib_TestBeam()

{

 

}


int
EMCalCalib_TestBeam::InitRun(PHCompositeNode *topNode)
{

  _ievent = 0;
  Emax_t=0;
  Etotal_t=0;
  E3by3_t=0;
  E5by5_t=0;
  C1_t=0;
  C2_inner_t=0;
  C2_outer_t=0;
  C2_inner_new_t=0;
  C2_outer_new_t=0;

  Veto1_t=0;
  Veto2_t=0;
  Veto3_t=0;
  Veto4_t=0;
  Horz_HODO_R0_t=0;
  Horz_HODO_R1_t=0;
  Horz_HODO_R2_t=0;
  Horz_HODO_R3_t=0;
  Horz_HODO_R4_t=0;
  Horz_HODO_R5_t=0;
  Horz_HODO_R6_t=0;
  Horz_HODO_R7_t=0;

  Vert_HODO_R0_t=0;
  Vert_HODO_R1_t=0;
  Vert_HODO_R2_t=0;
  Vert_HODO_R3_t=0;
  Vert_HODO_R4_t=0;
  Vert_HODO_R5_t=0;
  Vert_HODO_R6_t=0;
  Vert_HODO_R7_t=0;
  TowerID_t=0;
  Tower_column_t=0;
  Tower_row_t=0;
  TowerE_column_0_t=0;
  TowerE_column_1_t=0;
  TowerE_column_2_t=0;
  TowerE_column_3_t=0;
  TowerE_column_4_t=0;
  TowerE_column_5_t=0;
  TowerE_column_6_t=0;
  TowerE_column_7_t=0;
  TowerE_row_0_t=0;
  TowerE_row_1_t=0;
  TowerE_row_2_t=0;
  TowerE_row_3_t=0;
  TowerE_row_4_t=0;
  TowerE_row_5_t=0;
  TowerE_row_6_t=0;
  TowerE_row_7_t=0;
  HorzTowerID_t=0;
  VertTowerID_t=0;
  SaveHoriz_TowerID0_t=0;
  SaveHoriz_TowerID1_t=0;
  SaveHoriz_TowerID2_t=0;
  SaveHoriz_TowerID3_t=0;
  SaveHoriz_TowerID4_t=0;
  SaveHoriz_TowerID5_t=0;
  SaveHoriz_TowerID6_t=0;
  SaveHoriz_TowerID7_t=0;
  SaveVert_TowerID0_t=0;
  SaveVert_TowerID1_t=0;
  SaveVert_TowerID2_t=0;
  SaveVert_TowerID3_t=0;
  SaveVert_TowerID4_t=0;
  SaveVert_TowerID5_t=0;
  SaveVert_TowerID6_t=0;
  SaveVert_TowerID7_t=0;
  

  PHNodeIterator iter(topNode);

  PHCompositeNode *dstNode = static_cast<PHCompositeNode*>(iter.findFirst(

      "PHCompositeNode", "DST"));

  if (!dstNode)

    {

      std::cerr << PHWHERE << "DST Node missing, doing nothing." << std::endl;

      throw runtime_error(

          "Failed to find DST node in EmcRawTowerBuilder::CreateNodes");
    }

  return Fun4AllReturnCodes::EVENT_OK;

}


int

EMCalCalib_TestBeam::End(PHCompositeNode *topNode)

{
  // test1->Write();
  f_TB->Write();

  cout << "EMCalCalib::End - write to " << _filename << endl;

  PHTFileServer::get().cd(_filename);


  Fun4AllHistoManager *hm = get_HistoManager();

  assert(hm);

 
  

  // help index files with TChain

  TTree * T_Index = new TTree("T_Index", "T_Index");

  assert(T_Index);

  T_Index->Write();

  return Fun4AllReturnCodes::EVENT_OK;

}


int

EMCalCalib_TestBeam::Init(PHCompositeNode *topNode)

{

  _ievent = 0;
  Emax_t=0;
  Etotal_t=0;
  E3by3_t=0;
  E5by5_t=0;
  C1_t=0;
  C2_inner_t=0;
  C2_outer_t=0;

  C2_inner_new_t=0;
  C2_outer_new_t=0;
  
  Veto1_t=0;
  Veto2_t=0;
  Veto3_t=0;
  Veto4_t=0;

  Horz_HODO_R0_t=0;
  Horz_HODO_R1_t=0;
  Horz_HODO_R2_t=0;
  Horz_HODO_R3_t=0;
  Horz_HODO_R4_t=0;
  Horz_HODO_R5_t=0;
  Horz_HODO_R6_t=0;
  Horz_HODO_R7_t=0;

  Vert_HODO_R0_t=0;
  Vert_HODO_R1_t=0;
  Vert_HODO_R2_t=0;
  Vert_HODO_R3_t=0;
  Vert_HODO_R4_t=0;
  Vert_HODO_R5_t=0;
  Vert_HODO_R6_t=0;
  Vert_HODO_R7_t=0;
  TowerID_t=0;
  Tower_column_t=0;
  Tower_row_t=0;
  TowerE_column_0_t=0;
  TowerE_column_1_t=0;
  TowerE_column_2_t=0;
  TowerE_column_3_t=0;
  TowerE_column_4_t=0;
  TowerE_column_5_t=0;
  TowerE_column_6_t=0;
  TowerE_column_7_t=0;
  TowerE_row_0_t=0;
  TowerE_row_1_t=0;
  TowerE_row_2_t=0;
  TowerE_row_3_t=0;
  TowerE_row_4_t=0;
  TowerE_row_5_t=0;
  TowerE_row_6_t=0;
  TowerE_row_7_t=0;
  HorzTowerID_t=0;
  VertTowerID_t=0;
  SaveHoriz_TowerID0_t=0;
  SaveHoriz_TowerID1_t=0;
  SaveHoriz_TowerID2_t=0;
  SaveHoriz_TowerID3_t=0;
  SaveHoriz_TowerID4_t=0;
  SaveHoriz_TowerID5_t=0;
  SaveHoriz_TowerID6_t=0;
  SaveHoriz_TowerID7_t=0;
  SaveVert_TowerID0_t=0;
  SaveVert_TowerID1_t=0;
  SaveVert_TowerID2_t=0;
  SaveVert_TowerID3_t=0;
  SaveVert_TowerID4_t=0;
  SaveVert_TowerID5_t=0;
  SaveVert_TowerID6_t=0;
  SaveVert_TowerID7_t=0;




  cout << "EMCalCalib_TestBeam::get_HistoManager - Making PHTFileServer " << _filename

      << endl;

  PHTFileServer::get().open(_filename, "RECREATE");




      cout << "EMCalCalib_TestBeam::Init - Process tower occupancies" << endl;

      Init_Tower(topNode); //goes here 4-14-2016



 


  return Fun4AllReturnCodes::EVENT_OK;

}


int

EMCalCalib_TestBeam::process_event(PHCompositeNode *topNode)

{


  if (verbosity > 0)

    cout << "EMCalCalib_TestBeam::process_event() entered" << endl;


  

    process_event_Tower(topNode);

  return Fun4AllReturnCodes::EVENT_OK;

}





Fun4AllHistoManager *

EMCalCalib_TestBeam::get_HistoManager()

{


  Fun4AllServer *se = Fun4AllServer::instance();

  Fun4AllHistoManager *hm = se->getHistoManager("EMCalAna_HISTOS");


  if (not hm)

    {

      cout

          << "EMCalCalib::get_HistoManager - Making Fun4AllHistoManager EMCalAna_HISTOS"

          << endl;

      hm = new Fun4AllHistoManager("EMCalAna_HISTOS");

      se->registerHistoManager(hm);

    }


  assert(hm);


  return hm;

}


int

EMCalCalib_TestBeam::Init_Tower(PHCompositeNode *topNode)

{

  Fun4AllHistoManager *hm = get_HistoManager();

  assert(hm);



  return Fun4AllReturnCodes::EVENT_OK;

}


int

EMCalCalib_TestBeam::process_event_Tower(PHCompositeNode *topNode)

{

   const string detector("CEMC");
   const string chrenkov1("C1"); 
   const string chrenkov2("C2");
   const string HODO_V("HODO_VERTICAL");
   const string HODO_H("HODO_HORIZONTAL");
   const string VETO("TRIGGER_VETO");
   
   cout << "EMCalCalib::process_event_Tower() entered !!" << endl;

   string towernodename = "TOWER_CALIB_" + detector;
   string towernodename1="TOWER_CALIB_" + chrenkov1;
   string towernodename2="TOWER_CALIB_" + chrenkov2;

   string towernodename3="TOWER_CALIB_" + HODO_V;
   string towernodename4="TOWER_CALIB_" + HODO_H;

   string towernodename5="TOWER_CALIB_" + VETO;

   string towernodename6="TOWER_RAW_"+ chrenkov2;

  // Grab the towers

     RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(topNode, towernodename.c_str());
     RawTowerContainer* towers_sub1 = findNode::getClass<RawTowerContainer>(topNode, towernodename1.c_str());
     RawTowerContainer* towers_sub2 = findNode::getClass<RawTowerContainer>(topNode, towernodename2.c_str());
     RawTowerContainer* towers_sub3=  findNode::getClass<RawTowerContainer>(topNode,towernodename3.c_str());
     RawTowerContainer* towers_sub4=  findNode::getClass<RawTowerContainer>(topNode,towernodename4.c_str());
     RawTowerContainer* towers_sub5=  findNode::getClass<RawTowerContainer>(topNode,towernodename5.c_str());


     Float_t Emax=0;
     Float_t eta_max=0;
     Float_t phi_max=0;
     Float_t energy_tower=0;
     Float_t sumE=0;
     Float_t saveHoriz=0;
     Float_t saveVert=0;

     Float_t energy_chrenk1=0;

     Float_t energy_chrenk2_C2inner=0;
     Float_t energy_chrenk2_C2outer=0;
     Float_t energy_chrenk2_C2inner_new=0;//new as of 6-23
     Float_t energy_chrenk2_C2outer_new=0;//new as of 6-23

     Float_t Horizontal_HODO_R0=abs(towers_sub4->getTower(0,0)->get_energy());
     Float_t Horizontal_HODO_R1=abs(towers_sub4->getTower(0,1)->get_energy());
     Float_t Horizontal_HODO_R2=abs(towers_sub4->getTower(0,2)->get_energy());
     Float_t Horizontal_HODO_R3=abs(towers_sub4->getTower(0,3)->get_energy());
     Float_t Horizontal_HODO_R4=abs(towers_sub4->getTower(0,4)->get_energy());
     Float_t Horizontal_HODO_R5=abs(towers_sub4->getTower(0,5)->get_energy());
     Float_t Horizontal_HODO_R6=abs(towers_sub4->getTower(0,6)->get_energy());
     Float_t Horizontal_HODO_R7=abs(towers_sub4->getTower(0,7)->get_energy());

     Float_t Vertical_HODO_R0=abs(towers_sub3->getTower(0,0)->get_energy());
     Float_t Vertical_HODO_R1=abs(towers_sub3->getTower(0,1)->get_energy());
     Float_t Vertical_HODO_R2=abs(towers_sub3->getTower(0,2)->get_energy());
     Float_t Vertical_HODO_R3=abs(towers_sub3->getTower(0,3)->get_energy());
     Float_t Vertical_HODO_R4=abs(towers_sub3->getTower(0,4)->get_energy());
     Float_t Vertical_HODO_R5=abs(towers_sub3->getTower(0,5)->get_energy());
     Float_t Vertical_HODO_R6=abs(towers_sub3->getTower(0,6)->get_energy());
     Float_t Vertical_HODO_R7=abs(towers_sub3->getTower(0,7)->get_energy());

     Float_t Veto_Trig0=abs(towers_sub5->getTower(0,0)->get_energy());
     Float_t Veto_Trig1=abs(towers_sub5->getTower(0,1)->get_energy());
     Float_t Veto_Trig2=abs(towers_sub5->getTower(0,2)->get_energy());
     Float_t Veto_Trig3=abs(towers_sub5->getTower(0,3)->get_energy());

     Float_t SaveHoriz_TowerID0=abs(towers_sub3->getTower(0,0)->get_id());
     Float_t SaveHoriz_TowerID1=abs(towers_sub3->getTower(0,1)->get_id());
     Float_t SaveHoriz_TowerID2=abs(towers_sub3->getTower(0,2)->get_id());
     Float_t SaveHoriz_TowerID3=abs(towers_sub3->getTower(0,3)->get_id());
     Float_t SaveHoriz_TowerID4=abs(towers_sub3->getTower(0,4)->get_id());
     Float_t SaveHoriz_TowerID5=abs(towers_sub3->getTower(0,5)->get_id());
     Float_t SaveHoriz_TowerID6=abs(towers_sub3->getTower(0,6)->get_id());
     Float_t SaveHoriz_TowerID7=abs(towers_sub3->getTower(0,7)->get_id());

     Float_t SaveVert_TowerID0=abs(towers_sub4->getTower(0,0)->get_id());
     Float_t SaveVert_TowerID1=abs(towers_sub4->getTower(0,1)->get_id());
     Float_t SaveVert_TowerID2=abs(towers_sub4->getTower(0,2)->get_id());
     Float_t SaveVert_TowerID3=abs(towers_sub4->getTower(0,3)->get_id());
     Float_t SaveVert_TowerID4=abs(towers_sub4->getTower(0,4)->get_id());
     Float_t SaveVert_TowerID5=abs(towers_sub4->getTower(0,5)->get_id());
     Float_t SaveVert_TowerID6=abs(towers_sub4->getTower(0,6)->get_id());
     Float_t SaveVert_TowerID7=abs(towers_sub4->getTower(0,7)->get_id());


  
  Float_t ColumnE_0=abs(towers->getTower(0,0)->get_energy()+ towers->getTower(0,1)->get_energy() +
			towers->getTower(0,2)->get_energy()+towers->getTower(0,3)->get_energy()+
			towers->getTower(0,4)->get_energy()+towers->getTower(0,5)->get_energy()+
			towers->getTower(0,6)->get_energy()+towers->getTower(0,7)->get_energy());
  
  Float_t ColumnE_1=abs(towers->getTower(1,0)->get_energy()+ towers->getTower(1,1)->get_energy() +
			towers->getTower(1,2)->get_energy()+towers->getTower(1,3)->get_energy()+
			towers->getTower(1,4)->get_energy()+towers->getTower(1,5)->get_energy()+
			towers->getTower(1,6)->get_energy()+towers->getTower(1,7)->get_energy());
  
  Float_t ColumnE_2=abs(towers->getTower(2,0)->get_energy()+ towers->getTower(2,1)->get_energy() +
			towers->getTower(2,2)->get_energy()+towers->getTower(2,3)->get_energy()+
			towers->getTower(2,4)->get_energy()+towers->getTower(2,5)->get_energy()+
			towers->getTower(2,6)->get_energy()+towers->getTower(2,7)->get_energy());
  
  Float_t ColumnE_3=abs(towers->getTower(3,0)->get_energy()+ towers->getTower(3,1)->get_energy() +
			towers->getTower(3,2)->get_energy()+towers->getTower(3,3)->get_energy()+
			towers->getTower(3,4)->get_energy()+towers->getTower(3,5)->get_energy()+
			towers->getTower(3,6)->get_energy()+towers->getTower(3,7)->get_energy());
  
  Float_t ColumnE_4=abs(towers->getTower(4,0)->get_energy()+ towers->getTower(4,1)->get_energy() +
			towers->getTower(4,2)->get_energy()+towers->getTower(4,3)->get_energy()+
			towers->getTower(4,4)->get_energy()+towers->getTower(4,5)->get_energy()+
			towers->getTower(4,6)->get_energy()+towers->getTower(4,7)->get_energy());
  
  Float_t ColumnE_5=abs(towers->getTower(5,0)->get_energy()+ towers->getTower(5,1)->get_energy() +
			towers->getTower(5,2)->get_energy()+towers->getTower(5,3)->get_energy()+
			towers->getTower(5,4)->get_energy()+towers->getTower(5,5)->get_energy()+
			towers->getTower(5,6)->get_energy()+towers->getTower(5,7)->get_energy());
  
  Float_t ColumnE_6=abs(towers->getTower(6,0)->get_energy()+ towers->getTower(6,1)->get_energy() +
			towers->getTower(6,2)->get_energy()+towers->getTower(6,3)->get_energy()+
			towers->getTower(6,4)->get_energy()+towers->getTower(6,5)->get_energy()+
			towers->getTower(6,6)->get_energy()+towers->getTower(6,7)->get_energy());
  
  Float_t ColumnE_7=abs(towers->getTower(7,0)->get_energy()+ towers->getTower(7,1)->get_energy() +
			towers->getTower(7,2)->get_energy()+towers->getTower(7,3)->get_energy()+
			towers->getTower(7,4)->get_energy()+towers->getTower(7,5)->get_energy()+
			towers->getTower(7,6)->get_energy()+towers->getTower(7,7)->get_energy());

  Float_t RowE_0=abs(towers->getTower(0,0)->get_energy()+ towers->getTower(1,0)->get_energy() +
		     towers->getTower(2,0)->get_energy()+towers->getTower(3,0)->get_energy()+
		     towers->getTower(4,0)->get_energy()+towers->getTower(5,0)->get_energy()+
		     towers->getTower(6,0)->get_energy()+towers->getTower(7,0)->get_energy());
  
  Float_t RowE_1=abs(towers->getTower(0,1)->get_energy()+ towers->getTower(1,1)->get_energy() +
		     towers->getTower(2,1)->get_energy()+towers->getTower(3,1)->get_energy()+
		     towers->getTower(4,1)->get_energy()+towers->getTower(5,1)->get_energy()+
		     towers->getTower(6,1)->get_energy()+towers->getTower(7,1)->get_energy());
  
  Float_t RowE_2=abs(towers->getTower(0,2)->get_energy()+ towers->getTower(1,2)->get_energy()+
		     towers->getTower(2,2)->get_energy()+towers->getTower(3,2)->get_energy()+
		     towers->getTower(4,2)->get_energy()+towers->getTower(5,2)->get_energy()+
		     towers->getTower(6,2)->get_energy()+towers->getTower(7,2)->get_energy());
  
  Float_t RowE_3=abs(towers->getTower(0,3)->get_energy()+ towers->getTower(1,3)->get_energy() +
		     towers->getTower(2,3)->get_energy()+towers->getTower(3,3)->get_energy()+
		     towers->getTower(4,3)->get_energy()+towers->getTower(5,3)->get_energy()+
		     towers->getTower(6,3)->get_energy()+towers->getTower(7,3)->get_energy());
  
  Float_t RowE_4=abs(towers->getTower(0,4)->get_energy()+ towers->getTower(1,4)->get_energy() +
		     towers->getTower(2,4)->get_energy()+towers->getTower(3,4)->get_energy()+
		     towers->getTower(4,4)->get_energy()+towers->getTower(5,4)->get_energy()+
		     towers->getTower(6,4)->get_energy()+towers->getTower(7,4)->get_energy());
  
  Float_t RowE_5=abs(towers->getTower(0,5)->get_energy()+ towers->getTower(1,5)->get_energy() +
		     towers->getTower(2,5)->get_energy()+towers->getTower(3,5)->get_energy()+
		     towers->getTower(4,5)->get_energy()+towers->getTower(5,5)->get_energy()+
		     towers->getTower(6,5)->get_energy()+towers->getTower(7,5)->get_energy());
  
  Float_t RowE_6=abs(towers->getTower(0,6)->get_energy()+ towers->getTower(1,6)->get_energy() +
		     towers->getTower(2,6)->get_energy()+towers->getTower(3,6)->get_energy()+
		     towers->getTower(4,6)->get_energy()+towers->getTower(5,6)->get_energy()+
		     towers->getTower(6,6)->get_energy()+towers->getTower(7,6)->get_energy());
  
  Float_t RowE_7=abs(towers->getTower(0,7)->get_energy()+ towers->getTower(1,7)->get_energy() +
		     towers->getTower(2,7)->get_energy()+towers->getTower(3,7)->get_energy()+
		     towers->getTower(4,7)->get_energy()+towers->getTower(5,7)->get_energy()+
		     towers->getTower(6,7)->get_energy()+towers->getTower(7,7)->get_energy());



  Veto1_t=Veto_Trig0;
  Veto2_t=Veto_Trig1;
  Veto3_t=Veto_Trig2;
  Veto4_t=Veto_Trig3;

 


  SaveHoriz_TowerID0_t=SaveHoriz_TowerID0;
  SaveHoriz_TowerID1_t=SaveHoriz_TowerID1;
  SaveHoriz_TowerID2_t=SaveHoriz_TowerID2;
  SaveHoriz_TowerID3_t=SaveHoriz_TowerID3;
  SaveHoriz_TowerID4_t=SaveHoriz_TowerID4;
  SaveHoriz_TowerID5_t=SaveHoriz_TowerID5;
  SaveHoriz_TowerID6_t=SaveHoriz_TowerID6;
  SaveHoriz_TowerID7_t=SaveHoriz_TowerID7;
  SaveVert_TowerID0_t=SaveVert_TowerID0;
  SaveVert_TowerID1_t=SaveVert_TowerID1;
  SaveVert_TowerID2_t=SaveVert_TowerID2;
  SaveVert_TowerID3_t=SaveVert_TowerID3;
  SaveVert_TowerID4_t=SaveVert_TowerID4;
  SaveVert_TowerID5_t=SaveVert_TowerID5;
  SaveVert_TowerID6_t=SaveVert_TowerID6;
  SaveVert_TowerID7_t=SaveVert_TowerID7;
 
  energy_chrenk1=towers_sub1->getTower(0,0)->get_energy();
  cout<<endl;
  energy_chrenk2_C2inner=towers_sub2->getTower(0)->get_energy();
  energy_chrenk2_C2outer=towers_sub2->getTower(1)->get_energy();
    
  //use the following two lines for the joint HCAL/EMCAL scan
    
  // energy_chrenk2_C2inner_new=towers_sub2->getTower(10)->get_energy();
  //energy_chrenk2_C2outer_new=towers_sub2->getTower(11)->get_energy();

    
   

  C1_t=energy_chrenk1;
  C2_inner_t=energy_chrenk2_C2inner;
  C2_outer_t=energy_chrenk2_C2outer;
  C2_inner_new_t=energy_chrenk2_C2inner_new;
  C2_outer_new_t=energy_chrenk2_C2outer_new;


   ///////////////////////////////////
   //conditional energy cuts to HODOSCOPES
   ////////////////////////////////////
  Vert_HODO_R0_t=Vertical_HODO_R0;
  Vert_HODO_R1_t=Vertical_HODO_R1;
  Vert_HODO_R2_t=Vertical_HODO_R2;
  Vert_HODO_R3_t=Vertical_HODO_R3;
  Vert_HODO_R4_t=Vertical_HODO_R4;
  Vert_HODO_R5_t=Vertical_HODO_R5;
  Vert_HODO_R6_t=Vertical_HODO_R6;
  Vert_HODO_R7_t=Vertical_HODO_R7;

  //store the horizontal hodos in tree
  //   if(Horizontal_HODO_R0>30)
  Horz_HODO_R0_t=Horizontal_HODO_R0;
  Horz_HODO_R1_t=Horizontal_HODO_R1;
  Horz_HODO_R2_t=Horizontal_HODO_R2;
  Horz_HODO_R3_t=Horizontal_HODO_R3;
  Horz_HODO_R4_t=Horizontal_HODO_R4;
  Horz_HODO_R5_t=Horizontal_HODO_R5;
  Horz_HODO_R6_t=Horizontal_HODO_R6;
  Horz_HODO_R7_t=Horizontal_HODO_R7;


  //store the column  in tree
  TowerE_column_0_t=ColumnE_0;
  TowerE_column_1_t=ColumnE_1;
  TowerE_column_2_t=ColumnE_2;
  TowerE_column_3_t=ColumnE_3;
  TowerE_column_4_t=ColumnE_4;
  TowerE_column_5_t=ColumnE_5;
  TowerE_column_6_t=ColumnE_6;
  TowerE_column_7_t=ColumnE_7;

  //store the row in tree
  TowerE_row_0_t=RowE_0;
  TowerE_row_1_t=RowE_1;
  TowerE_row_2_t=RowE_2;
  TowerE_row_3_t=RowE_3;
  TowerE_row_4_t=RowE_4;
  TowerE_row_5_t=RowE_5;
  TowerE_row_6_t=RowE_6;
  TowerE_row_7_t=RowE_7;


  // Float_t getTowerID=0;
  Float_t getHorzTowerID=0;
  Float_t getVertTowerID=0;

 	

	//loop through to find Emax
for (int i=0; i<8;i++){//eta (x)
  for (int j=0; j<8;j++){//phi(y)
    energy_tower=abs(towers->getTower(i,j)->get_energy());
    getHorzTowerID=towers_sub4->getTower(0,j)->get_id();
    getVertTowerID=towers_sub3->getTower(0,j)->get_id();

   
 if (energy_tower>Emax &&energy_tower>0 )
      {
	Emax=energy_tower;//stores the max energy value here
	Emax_t=Emax;
	eta_max=i;
	phi_max=j;
      }
   sumE=sumE+energy_tower;
   saveHoriz=getHorzTowerID;
   saveVert=getVertTowerID;
   Etotal_t=sumE;
   HorzTowerID_t=saveHoriz;
   VertTowerID_t=saveVert;
   

   
    
   
 
    
  }//end j

  
 
 }//end i

//for 3by3 array
 Float_t tower_x_1_y_1=0;
 Float_t tower_x_1_y=0;
 Float_t tower_x_1_yplus1=0;

 Float_t tower_x_y_1=0;
 Float_t tower_x_yplus1=0;

 Float_t tower_xplus1_y_1=0;
 Float_t tower_xplus1_y=0;
 Float_t tower_xplus1_yplus1=0;

 //for 3by3 && 5by5 array
  Float_t tower_x_2_y_2=0;
 Float_t tower_x_2_y_1=0;
 Float_t tower_x_2_y=0;
 Float_t tower_x_2_yplus1=0;
  Float_t tower_x_2_yplus2=0;

  Float_t tower_x_1_y_2=0;
 Float_t tower_x_1_yplus2=0;

 Float_t tower_x_y_2=0;
 Float_t tower_x_yplus2=0;
 
 Float_t tower_xplus1_y_2=0;
 Float_t tower_xplus1_yplus2=0;

 Float_t tower_xplus2_y_2=0;
 Float_t tower_xplus2_y_1=0;
 Float_t tower_xplus2_y=0;
 Float_t tower_xplus2_yplus1=0;
 Float_t tower_xplus2_yplus2=0;


 Float_t E_3by3=0;
 Float_t E_5by5=0;
 
 RawTowerContainer* towersmax = findNode::getClass<RawTowerContainer>(topNode, towernodename.c_str());

 //pass through data to grab row info
 for (int i=0;i<8;i++)
   {
     for (int j=0;j<8;j++)
       {
	  if (i==eta_max&&j==phi_max)
	    {	Tower_row_t=towers_sub4->getTower(0,j)->get_id();
	        Tower_column_t=towers_sub3->getTower(0,0)->get_id();
	    }
       }
   }
	 
 
//2nd loop through the data with Emax
 for (int i=2; i<6;i++){//eta (x)
  for (int j=2; j<6;j++){//phi(y)
    if (i==eta_max&&j==phi_max)
      {	    
	//for 3by3 array
      tower_x_1_y_1=towersmax->getTower(i-1,j-1)->get_energy();
      tower_x_1_y=towersmax->getTower(i-1,j)->get_energy();
      tower_x_1_yplus1=towersmax->getTower(i-1,j+1)->get_energy();

      tower_x_y_1=towersmax->getTower(i,j-1)->get_energy();
      tower_x_yplus1=towersmax->getTower(i,j+1)->get_energy();

      tower_xplus1_y_1=towersmax->getTower(i+1,j-1)->get_energy();
      tower_xplus1_y=towersmax->getTower(i+1,j)->get_energy();
      tower_xplus1_yplus1=towersmax->getTower(i+1,j+1)->get_energy();

      //for 5by5 array
       tower_x_2_y_2=towersmax->getTower(i-2,j-2)->get_energy();
       tower_x_2_y_1=towersmax->getTower(i-2,j-1)->get_energy();
       tower_x_2_y=towersmax->getTower(i-2,j)->get_energy();
       tower_x_2_yplus1=towersmax->getTower(i-2,j+1)->get_energy();
       tower_x_2_yplus2=towersmax->getTower(i-2,j+2)->get_energy();
      
       tower_x_1_y_2=towersmax->getTower(i-1,j-2)->get_energy();
       tower_x_1_yplus2=towersmax->getTower(i-1,j+2)->get_energy();

       tower_x_y_2=towersmax->getTower(i,j-2)->get_energy();
       tower_x_yplus2=towersmax->getTower(i,j+2)->get_energy();

       tower_xplus1_y_2=towersmax->getTower(i+1,j-2)->get_energy();
       tower_xplus1_yplus2=towersmax->getTower(i+1,j+2)->get_energy();

       tower_xplus2_y_2=towersmax->getTower(i+2,j-2)->get_energy();
       tower_xplus2_y_1=towersmax->getTower(i+2,j-1)->get_energy();
       tower_xplus2_y=towersmax->getTower(i+2,j)->get_energy();
       tower_xplus2_yplus1=towersmax->getTower(i+2,j+1)->get_energy();
       tower_xplus2_yplus2=towersmax->getTower(i+2,j+2)->get_energy();

       // getTowerID=towersmax->getTower(i,j)->get_id();


      E_3by3=tower_x_1_y_1+tower_x_1_y+tower_x_1_yplus1+tower_x_y_1+
	tower_x_yplus1+ tower_xplus1_y_1+ tower_xplus1_y+ tower_xplus1_yplus1+Emax;
      
      E_5by5=tower_x_1_y_1+tower_x_1_y+tower_x_1_yplus1+tower_x_y_1+
	tower_x_yplus1+tower_xplus1_y_1+ tower_xplus1_y+ tower_xplus1_yplus1+
	tower_x_2_y_2+tower_x_2_y_1+tower_x_2_y+ tower_x_2_yplus1+
	tower_x_2_yplus2+tower_x_1_y_2+tower_x_1_yplus2+ tower_x_y_2+
	tower_x_yplus2+tower_xplus1_y_2+tower_xplus1_yplus2+tower_xplus2_y_2+
	tower_xplus2_y_1+tower_xplus2_y+tower_xplus2_yplus1+tower_xplus2_yplus2+Emax;
      
      E3by3_t=E_3by3;
      E5by5_t=E_5by5;

      }



  }
 }

 // TowerID_t=getTowerID;
 TowerE_column_0_t=ColumnE_0;
 TowerE_column_1_t=ColumnE_1;
 TowerE_column_2_t=ColumnE_2;
 TowerE_column_3_t=ColumnE_3;
 TowerE_column_4_t=ColumnE_4;
 TowerE_column_5_t=ColumnE_5;
 TowerE_column_6_t=ColumnE_6;
 TowerE_column_7_t=ColumnE_7;

 TowerE_row_0_t=RowE_0;
 TowerE_row_1_t=RowE_1;
 TowerE_row_2_t=RowE_2;
 TowerE_row_3_t=RowE_3;
 TowerE_row_4_t=RowE_4;
 TowerE_row_5_t=RowE_5;
 TowerE_row_6_t=RowE_6;
 TowerE_row_7_t=RowE_7;

  HorzTowerID_t=getHorzTowerID;
    
   VertTowerID_t=getVertTowerID;

    cout<<"HERE IS sum E :"<<sumE<<"Here is the eta max : "
    <<eta_max<<" Here is phi max : "<<phi_max<<endl;
 cout<<"HERE IS tower E :"<<energy_tower<<"E max :"<<Emax<<endl;


  test1->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
 

}

