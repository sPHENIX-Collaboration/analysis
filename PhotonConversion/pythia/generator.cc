#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h" //added plugin for HepMC, think we will need some new library in pythia for this
#include <TTree.h>
#include <TFile.h>
using namespace Pythia8;
using namespace std;

void generator(std::string filename, long nEvents, bool signalOnly=false){
/*  using namespace HepMC;
  string hepName = filename+".dat";    //filenames
  HepMC::Pythia8ToHepMC ToHepMC;    // Interface for conversion from Pythia8::Event to HepMC event.
  HepMC::IO_GenEvent ascii_io(hepName, std::ios::out);*/ //file where HepMC events will be stored.

  /*pythia set up*/
  Pythia pythiaengine;
  pythiaengine.readString("Beams:eCM = 200.");
  pythiaengine.readString("SoftQCD:nonDiffractive = on");
  pythiaengine.readString("SoftQCD:singleDiffractive = on");
  pythiaengine.readString("SoftQCD:doubleDiffractive = on");
  pythiaengine.readString("PhaseSpace:pTHatMin = 0");
  pythiaengine.readString("Random::setSeed = on");
  pythiaengine.readString("Random::seed =0");
  //pythiaengine.readString("111:onMode = off"); ///pi0 won't decay
  pythiaengine.init();

  string tfilename = filename+"_analysis.root";
  TFile *outFile = new TFile(tfilename.c_str(),"RECREATE");
  TTree *photonTree = new TTree("photonTree","phat phirn tree");
  photonTree->SetAutoSave(300);
  vector<float> photon_pT;
  photonTree->Branch("photon_pT",&photon_pT);
  TTree *nophotonTree = new TTree("nophotonTree","phat phirn tree");
  unsigned noPhotonEvents=0;
  nophotonTree->Branch("nNoPhoton",&noPhotonEvents);

  for (int iEvent = 0; iEvent < nEvents; ++iEvent)
  {
    if (!pythiaengine.next()){
      cout<<"pythia.next() failed"<<"\n";
      iEvent--;
      continue;
    } 
    photon_pT.clear();
    for(unsigned ipart=0; ipart!=pythiaengine.event.size(); ipart++){
      if(pythiaengine.event[ipart].id()==22&&pythiaengine.event[ipart].isFinal()&&pythiaengine.event[ipart].pT()>5
          &&TMath::Abs(pythiaengine.event[ipart].eta()))photon_pT.push_back(pythiaengine.event[ipart].pT());
    }
    if (photon_pT.size()>0)photonTree->Fill();
    else{
      noPhotonEvents++;
    }
/*    if(!signalOnly||photon_pT.size()>0){
      HepMC::GenEvent* hepmcevtfrag = new HepMC::GenEvent(); //create HepMC event
      ToHepMC.fill_next_event( pythiaengine, hepmcevtfrag ); //convert event from pythia to HepMC
      ascii_io << hepmcevtfrag;//write event to file
      delete hepmcevtfrag; //delete event so it can be redeclared next time
    }*/
  }
  nophotonTree->Fill();
  outFile->Write();
  outFile->Close();
  pythiaengine.stat();
}

int main(int argc, char const *argv[] )
{
  string fileOut = string(argv[1]);
  bool signalOnly=false;
  if(argv[2][0]=='1'||argv[2][0]=='t')signalOnly=true;
  long nEvents =strtol(argv[2],NULL,10);  // 5000000;
  generator(fileOut,nEvents,signalOnly);
  cout<<"All done"<<endl;
  return 0;
}
