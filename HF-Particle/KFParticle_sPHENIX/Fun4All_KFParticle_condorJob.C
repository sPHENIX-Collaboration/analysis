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
#include <decayfinder/DecayFinder.h>
#include <antitrigger/AntiTrigger.h>

#include <stdlib.h>

R__LOAD_LIBRARY(libdecayfinder.so)
R__LOAD_LIBRARY(libantitrigger.so)
R__LOAD_LIBRARY(libkfparticle_sphenix.so)
R__LOAD_LIBRARY(libfun4all.so)

#endif

using namespace std;

int Fun4All_KFParticle_condorJob(string fileList = "dst_hf_bottom_test.list", const int nEvents = 0){

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
  reconstructionChannel["D02K-pi+"] = 0;
  reconstructionChannel["D02K+pi-"] = 0;
  reconstructionChannel["Dstar2D0pi"] = 1;
  reconstructionChannel["Lc2pK-pi+"] = 0;
  reconstructionChannel["lowMassPiPi"] = 0;
  reconstructionChannel["Jpsi2ll"] = 0;
  reconstructionChannel["Bs2Jpsiphi"] = 0;
  reconstructionChannel["Bd2D-pi+"] = 0;
  reconstructionChannel["Bs2Ds-pi+"] = 0;
  reconstructionChannel["B+2D0pi+"] = 0;
  reconstructionChannel["Lb2Lcpi+"] = 0;
  reconstructionChannel["Upsilon"] = 0;
  reconstructionChannel["testSpace"] = 0;

  bool use_acts_vertexing = false;
  bool use_decay_finder = false;
  bool use_anti_trigger = false;
  bool use_decay_descriptor = true && (reconstructionChannel["D02K-pi+"] || reconstructionChannel["D02K+pi-"] || reconstructionChannel["Dstar2D0pi"] || reconstructionChannel["Bs2Jpsiphi"]);

  string decayDescriptor;
  if (reconstructionChannel["D02K-pi+"] || reconstructionChannel["D02K+pi-"]) decayDescriptor = "[D0 -> K^- pi^+]cc";
  if (reconstructionChannel["Dstar2D0pi"]) decayDescriptor = "[D*+ -> {D0 -> K^- pi^+} pi^+]cc";
  if (reconstructionChannel["Bs2Jpsiphi"]) decayDescriptor = "B_s0 -> {J/psi -> mu^+ mu^-} {phi -> K^+ K^-}";

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
  string makeDirectory = "mkdir -p " + outputDirectory + reconstructionName;
  system(makeDirectory.c_str());

  //--------------
  // IO management
  //--------------
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->AddListFile(fileList);
  se->registerInputManager(hitsin);

  string fileNumber = fileList.substr(fileList.size() - 10, 5);

  if (use_decay_finder)
  {
    DecayFinder* myFinder = new DecayFinder("MyDecayFinder");
    myFinder->Verbosity(verbosity);
    myFinder->setDecayDescriptor(decayDescriptor);
    myFinder->allowPi0(true);
    myFinder->allowPhotons(true);
    myFinder->triggerOnDecay(true);
    se->registerSubsystem(myFinder);
  }

  if (use_anti_trigger)
  {
    AntiTrigger* myFinder = new AntiTrigger("myTestAntiTrigger");
    myFinder->Verbosity(verbosity);
    std::vector<std::string> particleList = {"D0", "D+", "Ds+", "Lambdac+", "B+", "B0", "Bs0", "Lambdab0"};
    myFinder->setParticleList(particleList);
    se->registerSubsystem(myFinder);
  }

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

  bool fixToPV = true; 
  kfparticle->setMinimumTrackPT(0.2);
  kfparticle->setMinimumTrackIPchi2(0);
  kfparticle->setMinimumTrackIP(0.00);
  kfparticle->setMaximumTrackchi2nDOF(4);
  kfparticle->setMaximumVertexchi2nDOF(4);
  kfparticle->setMaximumDaughterDCA(0.05);
  kfparticle->setFlightDistancechi2(0.8);
  kfparticle->setMinDIRA(0.9);
  kfparticle->setMotherPT(0.0);
  kfparticle->setMotherIPchi2(50); 

  kfparticle->saveDST(0);
  kfparticle->saveOutput(1);
  kfparticle->doTruthMatching(1);
  kfparticle->getDetectorInfo(0);
  kfparticle->getCaloInfo(1);
  kfparticle->getAllPVInfo(0);

  std::vector<std::pair<std::string, int>> daughterList;
  std::vector<std::pair<std::string, int>> intermediateList;
  std::vector<std::pair<float, float>> intermediateMassRange;
  std::vector<std::pair<float, float>> intermediateIPRange;
  std::vector<std::pair<float, float>> intermediateIPchi2Range;
  std::vector<int> nIntTracks;
  std::vector<float> intPt, intIP, intIPchi2, intDIRA, intFDchi2;

  //D2Kpi reco
  if (reconstructionChannel["D02K-pi+"]
  or  reconstructionChannel["D02K+pi-"])
  {
    kfparticle->setMinimumMass(1.7);
    kfparticle->setMaximumMass(2.0);
    kfparticle->constrainToPrimaryVertex(fixToPV);
    //kfparticle->setTrackMapNodeName("D0_SvtxTrackMap"); 
 
    if (use_decay_descriptor)
    {
      kfparticle->setDecayDescriptor(decayDescriptor);
    }
    else
    {
      kfparticle->setMotherName("D0");  
      kfparticle->setNumberOfTracks(2);
    
      kfparticle->hasIntermediateStates(false);
      kfparticle->getChargeConjugate(false);

      if (reconstructionChannel["D02K-pi+"])
      {
        daughterList.push_back(make_pair("kaon", -1));
        daughterList.push_back(make_pair("pion", +1));
      }
      else
      {
        daughterList.push_back(make_pair("kaon", +1));
        daughterList.push_back(make_pair("pion", -1));
      }
    }
  }

  //D*->D0(->Kpi)pi
  if (reconstructionChannel["Dstar2D0pi"])
  {
    kfparticle->setMinimumMass(1.9);
    kfparticle->setMaximumMass(2.1);
    kfparticle->constrainToPrimaryVertex(true);

    if (use_decay_descriptor)
    {
      kfparticle->setDecayDescriptor(decayDescriptor);
    }
    else
    {
      kfparticle->setMotherName("Dstar");
      kfparticle->setNumberOfTracks(3);  
      kfparticle->hasIntermediateStates(true);
      kfparticle->getChargeConjugate(true);

      kfparticle->setNumberOfIntermediateStates(1);
      intermediateList.push_back(make_pair("D0", 0));
      nIntTracks.push_back(2);
      daughterList.push_back(make_pair("kaon", -1));
      daughterList.push_back(make_pair("pion", +1));
      daughterList.push_back(make_pair("pion", +1));
    }

    intermediateMassRange.push_back(make_pair(1.75, 1.95));
    intPt.push_back(0.2);
    intIP.push_back(0);
    intermediateIPRange.push_back(make_pair(0., 10.));
    intermediateIPchi2Range.push_back(make_pair(0., 100.));
    intDIRA.push_back(0.98);
    intFDchi2.push_back(0);
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

      daughterList.push_back(make_pair("proton", +1));
      daughterList.push_back(make_pair("kaon", -1));
      daughterList.push_back(make_pair("pion", +1));
  }

   //Low mass pipi
   if (reconstructionChannel["lowMassPiPi"])
  {
     kfparticle->setDecayDescriptor("mother -> pi^+ pi^-");
     kfparticle->setMinimumMass(0.2);
     kfparticle->setMaximumMass(2);

     kfparticle->constrainToPrimaryVertex(true);

     kfparticle->setFlightDistancechi2(0);
     kfparticle->setMinDIRA(0.0);
     kfparticle->setMotherIPchi2(20);
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
     kfparticle->setMaximumTrackchi2nDOF(10);
     kfparticle->setMaximumVertexchi2nDOF(2);
     kfparticle->setMaximumDaughterDCA(0.03);
     kfparticle->setFlightDistancechi2(0);
     kfparticle->setMinDIRA(-1);
     kfparticle->setMotherPT(0);
     kfparticle->setMotherIPchi2(1e5); 

     daughterList.push_back(make_pair("muon", -1));
     daughterList.push_back(make_pair("muon", +1));
  }

  //Bs2Jpsiphi reco
  if (reconstructionChannel["Bs2Jpsiphi"])
  {
      kfparticle->setMotherName("Bs0");  
      kfparticle->setMinimumMass(4.8);
      kfparticle->setMaximumMass(6.0);
      kfparticle->constrainToPrimaryVertex(true);

      if (use_decay_descriptor)
      {
        kfparticle->setDecayDescriptor(decayDescriptor);
      }
      else
      {
        kfparticle->setNumberOfTracks(4);
        kfparticle->hasIntermediateStates(true);
        kfparticle->setNumberOfIntermediateStates(2);

        intermediateList.push_back(make_pair("J/psi", 0));
        nIntTracks.push_back(2);
        daughterList.push_back(make_pair("electron", -1));
        daughterList.push_back(make_pair("electron", +1));

        intermediateList.push_back(make_pair("phi", 0));
        nIntTracks.push_back(2);
        daughterList.push_back(make_pair("kaon", -1));
        daughterList.push_back(make_pair("kaon", +1));
      }

      kfparticle->constrainIntermediateMasses(true);
    
      intermediateMassRange.push_back(make_pair(2.5, 3.2));
      intPt.push_back(0);
      intermediateIPRange.push_back(make_pair(0., 2000.));
      intermediateIPchi2Range.push_back(make_pair(0., 1000.));
      intDIRA.push_back(0);
      intFDchi2.push_back(0.);
    
      intermediateMassRange.push_back(make_pair(0.9, 1.1));
      intPt.push_back(0);
      intermediateIPRange.push_back(make_pair(0., 1000.));
      intermediateIPchi2Range.push_back(make_pair(0., 1000.));
      intDIRA.push_back(0);
      intFDchi2.push_back(0);
  }

  //Bd2D-pi+ reco
  if (reconstructionChannel["Bd2D-pi+"])
  {
      kfparticle->setMotherName("B0");  
      kfparticle->constrainToPrimaryVertex(true);
      kfparticle->setMinimumMass(4.8);
      kfparticle->setMaximumMass(6.0);
      kfparticle->setNumberOfTracks(4);
      kfparticle->getChargeConjugate(true);

      kfparticle->hasIntermediateStates(true);
      kfparticle->setNumberOfIntermediateStates(1);

      intermediateList.push_back(make_pair("D-", -1));
      daughterList.push_back(make_pair("kaon", +1));
      daughterList.push_back(make_pair("pion", -1));
      daughterList.push_back(make_pair("pion", -1));
      intermediateMassRange.push_back(make_pair(1.7, 2.1));
      nIntTracks.push_back(3);
      intPt.push_back(0.5);
      intIP.push_back(0.01);
      intIPchi2.push_back(10);
      intDIRA.push_back(0.95);
      intFDchi2.push_back(2);

      daughterList.push_back(make_pair("pion", +1));
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

      intermediateList.push_back(make_pair("Ds-", -1));
      daughterList.push_back(make_pair("kaon", +1));
      daughterList.push_back(make_pair("kaon", -1));
      daughterList.push_back(make_pair("pion", -1));
      intermediateMassRange.push_back(make_pair(1.0, 3.0));
      nIntTracks.push_back(3);
      intPt.push_back(0.);

      daughterList.push_back(make_pair("pion", +1));
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

      intermediateList.push_back(make_pair("D0", 0));
      daughterList.push_back(make_pair("kaon", -1));
      daughterList.push_back(make_pair("pion", +1));
      intermediateMassRange.push_back(make_pair(1.7, 2.0));
      nIntTracks.push_back(2);
      intPt.push_back(0.);

      daughterList.push_back(make_pair("pion", +1));
  }

    //Lb2Lcpi reco
   if (reconstructionChannel["Lb2Lcpi+"])
   {
      kfparticle->setMotherName("Lambdab0");  
      kfparticle->setMinimumMass(5.0);
      kfparticle->setMaximumMass(6.0);
      kfparticle->setNumberOfTracks(4);

      kfparticle->constrainToPrimaryVertex(true);
      kfparticle->getChargeConjugate(true);

      kfparticle->hasIntermediateStates(true);
      kfparticle->setNumberOfIntermediateStates(1);

      intermediateList.push_back(make_pair("Lambdac", +1));
      daughterList.push_back(make_pair("proton", +1));
      daughterList.push_back(make_pair("kaon", -1));
      daughterList.push_back(make_pair("pion", +1));
      intermediateMassRange.push_back(make_pair(2.15, 2.40));
      nIntTracks.push_back(3);
      intPt.push_back(0.5);
      intIP.push_back(0.01);
      intIPchi2.push_back(-1);
      intDIRA.push_back(0.95);
      intFDchi2.push_back(2);

      daughterList.push_back(make_pair("pion", -1));
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
     
      intermediateList.push_back(make_pair("B0", 0));
      daughterList.push_back(make_pair("kaon", +1));
      daughterList.push_back(make_pair("pion", -1));
      daughterList.push_back(make_pair("pion", -1));
      daughterList.push_back(make_pair("pion", +1));
      intermediateMassRange.push_back(make_pair(4.8, 6));
      nIntTracks.push_back(4);
      intPt.push_back(0);
     
      intermediateList.push_back(make_pair("B0", 0));
      daughterList.push_back(daughterList[0]);
      daughterList.push_back(daughterList[1]);
      daughterList.push_back(daughterList[2]);
      daughterList.push_back(daughterList[3]);
      intermediateMassRange.push_back(make_pair(4.8, 6));
      nIntTracks.push_back(4);
      intPt.push_back(0);
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
      daughterList.push_back(make_pair("electron", +1));
      daughterList.push_back(make_pair("electron", -1));
  }


  //More general setup
  if (!use_decay_descriptor)
  {
    kfparticle->setDaughters( daughterList );
    kfparticle->setIntermediateStates( intermediateList );
    kfparticle->setNumberTracksFromIntermeditateState( nIntTracks );
  }
  kfparticle->setIntermediateMassRange( intermediateMassRange );
  kfparticle->setIntermediateMinPT( intPt );
  kfparticle->setIntermediateMinIP( intIP );
  kfparticle->setIntermediateMinIPchi2( intIPchi2 );
  kfparticle->setIntermediateMinDIRA( intDIRA );
  kfparticle->setIntermediateMinFDchi2( intFDchi2 );
  if (reconstructionChannel["Dstar2D0pi"] || reconstructionChannel["Bs2Jpsiphi"]) 
  {  
    kfparticle->setIntermediateIPRange(intermediateIPRange);
    kfparticle->setIntermediateIPchi2Range(intermediateIPchi2Range);
  }

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
