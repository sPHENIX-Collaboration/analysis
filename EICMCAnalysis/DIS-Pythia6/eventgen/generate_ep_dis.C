/*======Electron-Proton Collision Mark V========
 *Project: Electron Ion Collider Simulations
 *Author: Thomas Krahulik (Stony Brook University)
 *Date Updated: February 25, 2017
 *Purpose: To simulate electron proton collisions
 *To run: root -l generate_ep_dis.C
 ===============================================*/


#ifndef __CINT__
#include "TPythia6.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TStyle.h"
#include "TCanvas.h"
using namespace std;
#endif

void loadLibraries()
{
#ifdef __CINT__
  // Load the Event Generator abstraction library, Pythia 6
  // library, and the Pythia 6 interface library.
  gSystem->Load("libEG");
  gSystem->Load("libPythia6");
  gSystem->Load("libEGPythia6");
#endif
}

/**
 * eELEC = Electron Energy
 * ePROT = Proton Energy
 * nEvents = Number of Events
 */
int generate_ep_dis(   double eELEC = 10.,
		       double ePROT = 250.,
		       int nEvents = 1000 )
{
  loadLibraries();

  /* build output file name */
  TString filename("pythia6.ep.dis.e");
  filename += eELEC;
  filename += "xp";
  filename += ePROT;
  filename += ".";
  filename += nEvents;
  filename += ".root";

  /* create output file */
  TFile *f = new TFile(filename, "recreate");

  TPythia6* pythia = new TPythia6;

  //Set Process
  pythia->SetMSEL(2);

  //Switches
  pythia->SetMSTP(14, 30);
  pythia->SetMSTP(15, 0);
  pythia->SetMSTP(16, 1);
  pythia->SetMSTP(17, 4);
  pythia->SetMSTP(18, 3);
  pythia->SetMSTP(19, 1);
  pythia->SetMSTP(20, 0);
  pythia->SetMSTP(32, 8);
  pythia->SetMSTP(38, 4);
  // pythia->SetMSTP(51, 10050);
  // pythia->SetMSTP(52, 2);
  pythia->SetMSTP(53, 3);
  pythia->SetMSTP(54, 1);
  pythia->SetMSTP(55, 5);
  pythia->SetMSTP(56, 1);
  pythia->SetMSTP(57, 1);
  pythia->SetMSTP(58, 5);
  pythia->SetMSTP(59, 1);
  pythia->SetMSTP(60, 7);
  pythia->SetMSTP(61, 2);
  pythia->SetMSTP(71, 1);
  pythia->SetMSTP(81, 0);
  pythia->SetMSTP(82, 0);
  pythia->SetMSTP(91, 1);
  pythia->SetMSTP(92, 3);
  pythia->SetMSTP(93, 1);
  pythia->SetMSTP(101, 3);
  pythia->SetMSTP(102, 1);
  pythia->SetMSTP(111, 1);
  pythia->SetMSTP(121, 0);
  pythia->SetMSTP(127, 1);

  //Parameters
  pythia->SetPARP(13, 1);
  pythia->SetPARP(18, 0.40);
  pythia->SetPARP(81, 1.9);
  pythia->SetPARP(89, 1800);
  pythia->SetPARP(90, 0.16);
  pythia->SetPARP(91, 0.40);
  pythia->SetPARP(93, 5.);
  pythia->SetPARP(99, 0.40);
  pythia->SetPARP(100 ,5);
  pythia->SetPARP(102, 0.28);
  pythia->SetPARP(103, 1.0);
  pythia->SetPARP(104, 0.8);
  pythia->SetPARP(111, 2.);
  pythia->SetPARP(161, 3.00);
  pythia->SetPARP(162, 24.6);
  pythia->SetPARP(163, 18.8);
  pythia->SetPARP(164, 11.5);
  pythia->SetPARP(165, 0.47679);
  pythia->SetPARP(166, 0.67597);

  //Switches for JetSet
  pythia->SetMSTJ(1, 1);
  pythia->SetMSTJ(12, 1);
  pythia->SetMSTJ(45, 5);

  pythia->SetMSTU(16, 2);
  pythia->SetMSTU(112, 5);
  pythia->SetMSTU(113, 5);
  pythia->SetMSTU(114, 5);

  //Parameters for JetSet
  pythia->SetPARJ(1, 0.100);
  pythia->SetPARJ(2, 0.300);
  pythia->SetPARJ(11, 0.5);
  pythia->SetPARJ(12, 0.6);
  pythia->SetPARJ(21, 0.40);
  pythia->SetPARJ(32, 1.0);
  pythia->SetPARJ(33, 0.80);
  pythia->SetPARJ(41, 0.30);
  pythia->SetPARJ(42, 0.58);
  pythia->SetPARJ(45, 0.5);

  //Kinematic Variables
  pythia->SetCKIN(1, 1.);
  pythia->SetCKIN(2, -1.);
  pythia->SetCKIN(3, 0.);
  pythia->SetCKIN(4, -1.);
  pythia->SetCKIN(5, 1.00);
  pythia->SetCKIN(6, 1.00);
  pythia->SetCKIN(7, -10.);
  pythia->SetCKIN(8, 10.);
  pythia->SetCKIN(9, -40.);
  pythia->SetCKIN(10, 40.);
  pythia->SetCKIN(11, -40.);
  pythia->SetCKIN(12, 40.);
  pythia->SetCKIN(13, -40.);
  pythia->SetCKIN(14, 40.);
  pythia->SetCKIN(15, -40.);
  pythia->SetCKIN(16, 40.);
  pythia->SetCKIN(17, -1.);
  pythia->SetCKIN(18, 1.);
  pythia->SetCKIN(19, -1.);
  pythia->SetCKIN(20, 1.);
  pythia->SetCKIN(21, 0.);
  pythia->SetCKIN(22, 1.);
  pythia->SetCKIN(23, 0.);
  pythia->SetCKIN(24, 1.);
  pythia->SetCKIN(25, -1.);
  pythia->SetCKIN(26, 1.);
  pythia->SetCKIN(27, -1.);
  pythia->SetCKIN(28, 1.);
  pythia->SetCKIN(31, 2.);
  pythia->SetCKIN(32, -1.);
  pythia->SetCKIN(35, 0.);
  pythia->SetCKIN(36, -1.);
  pythia->SetCKIN(37, 0.);
  pythia->SetCKIN(38, -1.);
  pythia->SetCKIN(39, 4.);
  pythia->SetCKIN(40, -1.);
  pythia->SetCKIN(65, 1.e-09);
  pythia->SetCKIN(66, -1.);
  pythia->SetCKIN(67, 0.);
  pythia->SetCKIN(68, -1.);
  pythia->SetCKIN(77, 2.0);
  pythia->SetCKIN(78, -1.);

  //Initialize
  pythia->SetP(1, 1, 0.); // x Momentum of Electron
  pythia->SetP(1, 2, 0.); // y Momentum of Electron
  pythia->SetP(1, 3, -eELEC); // z Momentum of Electron
  pythia->SetP(2, 1, 0.); // x Momentum of Proton
  pythia->SetP(2, 2, 0.); // y Momentum of Proton
  pythia->SetP(2, 3, ePROT); // z Momentum of Proton
  pythia->Initialize("3MOM", "e-", "p", 0.);

  TTree* tree = new TTree("tree", "Pythia 6 tree");

  Int_t nPart;
  Double_t Q2, y, x;
  TClonesArray* p = (TClonesArray*)pythia->GetListOfParticles();
  tree->Branch("p", &p);
  TBranch *b_nPart = tree->Branch("nPart", &nPart, "nPart/I");
  TBranch *b_Q2 = tree->Branch("Q2", &Q2, "Q2/D");
  TBranch *b_y = tree->Branch("y", &y, "y/D");
  TBranch *b_x = tree->Branch("x", &x, "x/D");
  // TBranch *b_W2 = tree->Branch("W2", &W2, "W2/D");
  // TBranch *b_nu = tree->Branch("nu", &nu, "nu/D");

  // Now we generate some events
  for (Int_t i = 0; i < nEvents; i++) {
    // Show how far we got every 100'th event.
    if (i % 1000 == 0) cout << "Event # " << i << endl;
    // Make one event and fill the tree
    pythia->GenerateEvent();

    nPart = pythia->GetN();
    Q2 = pythia->GetPARI(22);
    x = pythia->GetPARI(32);
    y = Q2 / (4.*x*eELEC*ePROT);

    tree->Fill();
    // nParticles->Fill();
  }

  // Show tree structure
  tree->Print();

  f->Write();

  return 0;
}
