

#include "EMCalCalib_TestBeam.h"

#include "PhotonPair.h"


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


#include <g4cemc/RawTowerContainer.h>

#include <g4cemc/RawTowerGeomContainer.h>

#include <g4cemc/RawTower.h>

#include <g4cemc/RawClusterContainer.h>

#include <g4cemc/RawCluster.h>


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
 TFile *f_TB=new TFile("output.root","RECREATE");
  TNtuple *ntupleTruth_TB2=new TNtuple("ntuple","event ntuple","Etotal:Emax:E3by3:E5by5:C1:C2"); 


EMCalCalib_TestBeam::EMCalCalib_TestBeam(const std::string &filename, EMCalCalib_TestBeam::enu_flags flags) :

    SubsysReco("EMCalCalib_TestBeam"), _eval_stack(NULL), _T_EMCalTrk(NULL), _filename(filename), _flags(flags), _ievent(0)

{


  verbosity = 0;

 
}


EMCalCalib_TestBeam::~EMCalCalib_TestBeam()

{

  if (_eval_stack)

    {

      delete _eval_stack;

    }

}


int
EMCalCalib_TestBeam::InitRun(PHCompositeNode *topNode)
{

  _ievent = 0;


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


  cout << "EMCalCalib_TestBeam::get_HistoManager - Making PHTFileServer " << _filename

      << endl;

  PHTFileServer::get().open(_filename, "RECREATE");



 

  if (flag(kProcessTower))

    {

      cout << "EMCalCalib_TestBeam::Init - Process tower occupancies" << endl;

      Init_Tower(topNode); //goes here 4-14-2016

    }

 


  return Fun4AllReturnCodes::EVENT_OK;

}


int

EMCalCalib_TestBeam::process_event(PHCompositeNode *topNode)

{


  if (verbosity > 0)

    cout << "EMCalCalib_TestBeam::process_event() entered" << endl;


  if (_eval_stack)

    _eval_stack->next_event(topNode);


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



    cout << "EMCalCalib::process_event_Tower() entered !!" << endl;



    string towernodename = "TOWER_CALIB_" + detector;
    string towernodename1="TOWER_CALIB_" + chrenkov1;
     string towernodename2="TOWER_CALIB_" + chrenkov2;


  // Grab the towers

    RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(topNode, towernodename.c_str());
    RawTowerContainer* towers_sub1 = findNode::getClass<RawTowerContainer>(topNode, towernodename1.c_str());
     RawTowerContainer* towers_sub2 = findNode::getClass<RawTowerContainer>(topNode, towernodename2.c_str());


 
Float_t Emax=0;
Float_t eta_max=0;
Float_t phi_max=0;
Float_t energy_tower=0;
 Float_t sumE=0;

 Float_t energy_chrenk1=0;
 Float_t energy_chrenk2=0;
 

  energy_chrenk1=towers_sub1->getTower(0,0)->get_energy();
 energy_chrenk2=towers_sub2->getTower(0,0)->get_energy();
 	cout<<" energy_c1 is : "<<energy_chrenk1<<endl;
	cout<<" energy_c2 is : "<<energy_chrenk2<<endl;



	//loop through to find Emax
for (int i=0; i<8;i++){//eta (x)
  for (int j=0; j<8;j++){//phi(y)
           energy_tower=towers->getTower(i,j)->get_energy();
	

   if (energy_tower>Emax&& energy_tower!=0)
      {
	Emax=energy_tower;//stores the max energy value here
	cout<<" energy_tower is : "<<energy_tower<<endl;

	eta_max=i;
	phi_max=j;
      }
   sumE=sumE+energy_tower;
     
  }//end i
 }//end j

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

      E_3by3=tower_x_1_y_1+tower_x_1_y+tower_x_1_yplus1+tower_x_y_1+tower_x_yplus1+ tower_xplus1_y_1+ tower_xplus1_y+ tower_xplus1_yplus1+Emax;
      E_5by5=tower_x_1_y_1+tower_x_1_y+tower_x_1_yplus1+tower_x_y_1+tower_x_yplus1+ tower_xplus1_y_1+ tower_xplus1_y+ tower_xplus1_yplus1+tower_x_2_y_2+tower_x_2_y_1+tower_x_2_y+ tower_x_2_yplus1+tower_x_2_yplus2+tower_x_1_y_2+tower_x_1_yplus2+ tower_x_y_2+ tower_x_yplus2+tower_xplus1_y_2+tower_xplus1_yplus2+tower_xplus2_y_2+tower_xplus2_y_1+tower_xplus2_y+tower_xplus2_yplus1+tower_xplus2_yplus2+Emax;

      cout<<"E  MAX   : "<<Emax<<endl;
      cout<<"E 3 by 3  : "<<E_3by3<<endl;
      cout<<"E 5 by 5  : "<<E_5by5<<endl; 
      }



  }
 }
 // if (sumE>0.5&&E_3by3>0.5)
 ntupleTruth_TB2->Fill(sumE,Emax,E_3by3,E_5by5,energy_chrenk1,energy_chrenk2);

 cout<<"HERE IS sum E :"<<sumE<<"Here is the eta max : "<<eta_max<<" Here is phi max : "<<phi_max<<endl;
 cout<<"HERE IS tower E :"<<energy_tower<<"E max :"<<Emax<<endl;

 
  return Fun4AllReturnCodes::EVENT_OK;
 

}

