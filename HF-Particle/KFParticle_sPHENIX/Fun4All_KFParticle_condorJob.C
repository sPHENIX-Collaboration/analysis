// $Id: $

/*!
 * \file Fun4All_KFParticle_condorJob.C
 * \brief 
 * \author Cameron Dean <cdean@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include "FROG.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 00, 0)

#include "ACTS_tracking/G4Setup_sPHENIX.C"
#include "ACTS_tracking/G4_Tracking.C"

#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <kfparticle_sphenix/KFParticle_sPHENIX.h>

#include <stdlib.h>

R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libfun4all.so)

#endif

using namespace std;

int Fun4All_KFParticle_condorJob(string fileList = "dst_hf_charm.list", const int nEvents = 0){

  int verbosity = 1;
  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4dst.so");

  // Enabling file finding in dCache
  FROG *fr = new FROG();

  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);

  //---------------
  // Choose reco
  //---------------
  map<string, int> reconstructionChannel;
  reconstructionChannel["D02K-pi+"] = 1;
  reconstructionChannel["D02K+pi-"] = 0;
  reconstructionChannel["Lc2pK-pi+"] = 0;
  reconstructionChannel["Jpsi2ll"] = 0;
  reconstructionChannel["Bs2Jpsiphi"] = 0;
  reconstructionChannel["Bd2D-pi+"] = 0;
  reconstructionChannel["Bs2Ds-pi+"] = 0;
  reconstructionChannel["B+2D0pi+"] = 0;
  reconstructionChannel["Upsilon"] = 0;
  reconstructionChannel["testSpace"] = 0;
  bool testMDC = true;
  bool use_acts_vertexing = true;

  const int numberOfActiveRecos = accumulate( begin(reconstructionChannel), end(reconstructionChannel), 0, 
                                              [](const int previous, const pair<const string, int>& element) 
                                              { return previous + element.second; });

  string reconstructionName = "blank";
  if (numberOfActiveRecos != 1)
  {
    if (numberOfActiveRecos == 0) printf("*\n*\n*\n* You have not enabled any reconstruction, exiting!\n*\n*\n*\n");
    else
    {
        printf("*\n*\n*\n* You have more than one active reconstruction, exiting!\n*\n*\n*\n");
        std::map<string, int>::iterator it;
        for ( it = reconstructionChannel.begin(); it != reconstructionChannel.end(); it++ )
            printf("You have set channel %s to %u\n", it->first.c_str(), it->second);
    }
    delete se;
    gSystem->Exit(0);
    return 1;
  }
  else
  {
    std::map<string, int>::iterator it;
    for ( it = reconstructionChannel.begin(); it != reconstructionChannel.end(); it++ )
      if (it->second == 1) reconstructionName = it->first;
  }

  string outputDirectory = "";
  string makeDirectory = "mkdir " + outputDirectory + reconstructionName;
  system(makeDirectory.c_str());

  //--------------
  // IO management
  //--------------
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->AddListFile(fileList);
  se->registerInputManager(hitsin);

  string fileNumber = fileList.substr(fileList.size() - 10, 5);

  string actsVertexName = "SvtxVertexMap_recoOnly";
  if (use_acts_vertexing)
  {
    G4Init();

    MakeActsGeometry* geom = new MakeActsGeometry();
    geom->Verbosity(verbosity);
    geom->setMagField(G4MAGNET::magfield);
    geom->setMagFieldRescale(G4MAGNET::magfield_rescale);
    se->registerSubsystem(geom);

    PHActsInitialVertexFinder *f = new PHActsInitialVertexFinder("MyVertexFinder");
    f->setSvtxTrackMapName("SvtxTrackMap");
    f->setSvtxVertexMapName(actsVertexName.c_str());
    f->setInitialVertexer(false);
    f->Verbosity(0);
    se->registerSubsystem(f);
  }

  //General configurations
  KFParticle_sPHENIX *kfparticle = new KFParticle_sPHENIX();
  kfparticle->Verbosity(verbosity);
  if (use_acts_vertexing) kfparticle->setVertexMapNodeName(actsVertexName.c_str());

  float minTrackIPchi2 = testMDC ? -1 : 10;
  float maxTrackchi2nDOF = testMDC ? 4 : 2; 
  bool fixToPV = true;
  
  kfparticle->setMinimumTrackPT(0.1);
  kfparticle->setMinimumTrackIPchi2(minTrackIPchi2);
  kfparticle->setMaximumTrackchi2nDOF(maxTrackchi2nDOF);
  kfparticle->setMaximumVertexchi2nDOF(2);
  kfparticle->setMaximumDaughterDCA(0.03);
  kfparticle->setFlightDistancechi2(80);
  kfparticle->setMinDIRA(0.8);
  kfparticle->setMotherPT(0);
  kfparticle->setMotherIPchi2(1e5); 

  kfparticle->saveDST(0);
  kfparticle->saveOutput(1);
  kfparticle->doTruthMatching(1);
  kfparticle->getDetectorInfo(0);

  std::pair<std::string, int> daughterList[99];
  std::pair<std::string, int> intermediateList[99];
  std::pair<float, float> intermediateMassRange[99];
  int nIntTracks[99];
  float intPt[99];

  //D2Kpi reco
  if (reconstructionChannel["D02K-pi+"]
  or  reconstructionChannel["D02K+pi-"])
  {
      kfparticle->setMotherName("D0");  
      //kfparticle->setTrackMapNodeName("D0_SvtxTrackMap"); 
      kfparticle->setMinimumMass(1.7);
      kfparticle->setMaximumMass(2.0);
      kfparticle->setNumberOfTracks(2);
    
      kfparticle->constrainToPrimaryVertex(fixToPV);
      kfparticle->hasIntermediateStates(false);
      kfparticle->getChargeConjugate(false);

      if (reconstructionChannel["D02K-pi+"])
      {
        daughterList[0] = make_pair("kaon", -1);
        daughterList[1] = make_pair("pion", +1);
      }
      else
      {
        daughterList[0] = make_pair("kaon", +1);
        daughterList[1] = make_pair("pion", -1);
      }
  }

  //Lambdac Reco
  if (reconstructionChannel["Lc2pK-pi+"])
  {
      kfparticle->setMotherName("Lambdac");
      kfparticle->setMinimumMass(2.0);
      kfparticle->setMaximumMass(2.4);
      kfparticle->setNumberOfTracks(3);
      kfparticle->setMotherIPchi2(20);

      kfparticle->constrainToPrimaryVertex(fixToPV);
      kfparticle->hasIntermediateStates(false);
      kfparticle->getChargeConjugate(true);

      daughterList[0] = make_pair("proton", +1);
      daughterList[1] = make_pair("kaon", -1);
      daughterList[2] = make_pair("pion", +1);
  }

  //Jpsi2ll
  if (reconstructionChannel["Jpsi2ll"])
  {
     kfparticle->setMotherName("J/psi");
     kfparticle->setMinimumMass(2.5);
     kfparticle->setMaximumMass(4);
     kfparticle->setNumberOfTracks(2);

     kfparticle->constrainToPrimaryVertex(true);
     kfparticle->hasIntermediateStates(false);
      kfparticle->getChargeConjugate(false);
  
     kfparticle->setMinimumTrackPT(0.5);
     kfparticle->setMinimumTrackIPchi2(10);
     kfparticle->setMaximumTrackchi2nDOF(3);
     kfparticle->setMaximumVertexchi2nDOF(2);
     kfparticle->setMaximumDaughterDCA(0.03);
     kfparticle->setFlightDistancechi2(0);
     kfparticle->setMinDIRA(-1);
     kfparticle->setMotherPT(1);
     kfparticle->setMotherIPchi2(1e5); 

     daughterList[0] = make_pair("muon", -1);
     daughterList[1] = make_pair("muon", +1);
  }

  //Bs2Jpsiphi reco
  if (reconstructionChannel["Bs2Jpsiphi"])
  {
      kfparticle->setMotherName("Bs0");  
      kfparticle->setMinimumMass(4.8);
      kfparticle->setMaximumMass(6.0);
      kfparticle->setNumberOfTracks(4);
     
      kfparticle->constrainToPrimaryVertex(true);
      kfparticle->hasIntermediateStates(true);
      kfparticle->constrainIntermediateMasses(true);
      kfparticle->setNumberOfIntermediateStates(2);
    
      intermediateList[0] = make_pair("J/psi", 0);
      daughterList[0]     = make_pair("electron", -1);
      daughterList[1]     = make_pair("electron", +1);
      intermediateMassRange[0] = make_pair(2.9, 3.2);
      nIntTracks[0] = 2;
      intPt[0] = 0;
    
      intermediateList[1] = make_pair("phi", 0);
      daughterList[2]     = make_pair("kaon", -1);
      daughterList[3]     = make_pair("kaon", +1);
      intermediateMassRange[1] = make_pair(0.9, 1.2);
      nIntTracks[1] = 2;
      intPt[1] = 0;
  }


  //Bd2D-pi+ reco
  if (reconstructionChannel["Bd2D-pi+"])
  {
      kfparticle->setMinimumMass(4.8);
      kfparticle->setMaximumMass(6.0);
      kfparticle->setNumberOfTracks(4);

      kfparticle->hasIntermediateStates(true);
      kfparticle->setNumberOfIntermediateStates(1);

      intermediateList[0] = make_pair("D-", -1);
      daughterList[0]     = make_pair("kaon", +1);
      daughterList[1]     = make_pair("pion", -1);
      daughterList[2]     = make_pair("pion", -1);
      intermediateMassRange[0] = make_pair(1.0, 3.0);
      nIntTracks[0] = 3;
      intPt[0] = 0.;

      daughterList[3] = make_pair("pion", +1);
  }

  //Bs2Ds-pi+ reco
  if (reconstructionChannel["Bs2Ds-pi+"])
  {
      kfparticle->setMotherName("Bs0");  
      kfparticle->setMinimumMass(4.8);
      kfparticle->setMaximumMass(6.0);
      kfparticle->setNumberOfTracks(4);

      kfparticle->constrainToPrimaryVertex(fixToPV);
      kfparticle->getChargeConjugate(false);
      kfparticle->hasIntermediateStates(true);
      kfparticle->setNumberOfIntermediateStates(1);

      intermediateList[0] = make_pair("Ds-", -1);
      daughterList[0]     = make_pair("kaon", +1);
      daughterList[1]     = make_pair("kaon", -1);
      daughterList[2]     = make_pair("pion", -1);
      intermediateMassRange[0] = make_pair(1.0, 3.0);
      nIntTracks[0] = 3;
      intPt[0] = 0.;

      daughterList[3] = make_pair("pion", +1);
  }

    //B+2D0pi+ reco
   if (reconstructionChannel["B+2D0pi+"])
   {
      kfparticle->setMotherName("B+");  
      kfparticle->setMinimumMass(4.5);
      kfparticle->setMaximumMass(6.0);
      kfparticle->setNumberOfTracks(3);

      kfparticle->constrainToPrimaryVertex(true);
      kfparticle->getChargeConjugate(true);

      kfparticle->setMinimumTrackPT(0.1);
      kfparticle->setMinimumTrackIPchi2(10); 
      kfparticle->setMaximumTrackchi2nDOF(2);
      kfparticle->setMaximumVertexchi2nDOF(2);
      kfparticle->setMaximumDaughterDCA(0.03);
      kfparticle->setFlightDistancechi2(80);
      kfparticle->setMinDIRA(0.8);
      kfparticle->setMotherPT(0);
      kfparticle->setMotherIPchi2(50); 

      kfparticle->hasIntermediateStates(true);
      kfparticle->setNumberOfIntermediateStates(1);

      intermediateList[0] = make_pair("D0", 0);
      daughterList[0]     = make_pair("kaon", -1);
      daughterList[1]     = make_pair("pion", +1);
      intermediateMassRange[0] = make_pair(1.7, 2.0);
      nIntTracks[0] = 2;
      intPt[0] = 0.;

      daughterList[2] = make_pair("pion", +1);
  }

  //Upsilon reco
  if (reconstructionChannel["Upsilon"])
  {
      kfparticle->setMotherName("Upsilon");  
      kfparticle->setMinimumMass(9);
      kfparticle->setMaximumMass(11);
      kfparticle->setNumberOfTracks(8); 
     
      kfparticle->hasIntermediateStates(true);
      kfparticle->setNumberOfIntermediateStates(2);
     
      intermediateList[0] = make_pair("B0", 0);
      daughterList[0]     = make_pair("kaon", +1);
      daughterList[1]     = make_pair("pion", -1);
      daughterList[2]     = make_pair("pion", -1);
      daughterList[3]     = make_pair("pion", +1);
      intermediateMassRange[0] = make_pair(4.8, 6);
      nIntTracks[0] = 4;
      intPt[0] = 0;
     
      intermediateList[1] = make_pair("B0", 0);
      daughterList[4] = daughterList[0];
      daughterList[5] = daughterList[1];
      daughterList[6] = daughterList[2];
      daughterList[7] = daughterList[3];
      intermediateMassRange[1] = make_pair(4.8, 6);
      nIntTracks[1] = 4;
      intPt[1] = 0;
  }


  //testSpace
  if (reconstructionChannel["testSpace"])
  {
      kfparticle->hasIntermediateStates(false);
      kfparticle->constrainToPrimaryVertex(false);
      kfparticle->getChargeConjugate(false);

      kfparticle->setMotherName("J/psi");  
      kfparticle->setMinimumMass(2.8);
      kfparticle->setMaximumMass(3.4);

      kfparticle->setMinimumTrackIPchi2(0.); //Tracks should point back to the PV
      kfparticle->setMaximumDaughterDCA(0.02);

      kfparticle->setNumberOfTracks(2);
      daughterList[0] = make_pair("electron", +1);
      daughterList[1] = make_pair("electron", -1);
  }


  //More general setup
  kfparticle->setDaughters( daughterList );
  kfparticle->setIntermediateStates( intermediateList );
  kfparticle->setIntermediateMassRange( intermediateMassRange );
  kfparticle->setNumberTracksFromIntermeditateState( nIntTracks );
  kfparticle->setIntermediateMinPT( intPt );

  kfparticle->setOutputName(outputDirectory + reconstructionName + "/outputData_" + reconstructionName + "_" + fileNumber + ".root");

  se->registerSubsystem(kfparticle);
  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    return 0;

  se->run(nEvents);

  //-----
  // Exit
  //-----

  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
  return 0;
}
