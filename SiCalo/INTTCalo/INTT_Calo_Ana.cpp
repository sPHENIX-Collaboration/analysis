#include <iostream>
#include <vector>
#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TStyle.h>

#include "INTT_Calo_Ana.h"
#include "LoadData.h"
#include "AnalyzeEvent.h"
#include "EventDisplay.h"
#include "INTT_Calo_trkReco.h"

#include "LoadData.cpp"
#include "AnalyzeEvent.cpp"
#include "EventDisplay.cpp"
#include "INTT_Calo_trkReco.cpp"


void INTT_Calo_Ana(void) {

  cout <<"Welcome to INTT_Calo_Ana"<<endl;
  cout <<"This is analysis program of INTT_Calo nDST with event display!"<<endl;

  int ievent = 0;
  const int MaxLoop = 100;


  // open data file and load the data
  //input data file
  //  idatafile = "run2pp/53876/ana_53876_00001.root";
  //  idatafile = "../../2605/ana_53879_1kevt_4evtdisplay.root";
  //  idatafile = "53876_100evt/ana_53876_00000.root";
  idatafile = "53876_100evt/merge53876.root";

  LoadData(idatafile);
  NmaxEvent = evtTree->GetEntries();
  
  cout <<"Data file:"<<idatafile.c_str()<<endl;
  cout <<"# of events in the file:"<<NmaxEvent<<endl;
  
  cout << "Select Mode: " <<endl;
  cout << " 1: run AnalyzeEvent" << endl;
  cout << " 2: event display" <<endl;
  cin >> imode;
  if(imode == 1) {
    int istart = 0;
    int nevent=0;
    
    cout << "enter istart (first event)"<<endl;
    cin >> istart;
    if(istart<0) istart=0;
    
    cout << "how many event:"<<endl;
    cin >> nevent;
    if(nevent<0) nevent=0;
    if(istart+nevent > NmaxEvent) {
      cout <<" Exceeds the # of events in the file" <<endl;
      nevent = NmaxEvent - istart;
    }
    cout << "analyze "<< nevent << " events" <<endl;

    AnalyzeInit();
    for(int i=istart;i<istart+nevent;i++) {
      GetEvent(i);
      AnalyzeEvent();
      /*
      if(vEmcINTT.size()>0) {
	cout <<"ievent="<<i<<" # of triplet="<<vEmcINTT.size()<<endl<<endl;
      }
      */
      if(i%1000 == 0) cout << i << endl;
    }//for
    AnalyzeEnd();
  } else if(imode == 2 ) { // Event Display
    gStyle->SetCanvasPreferGL();
    AnalyzeInit();
    for(int i=0;i<MaxLoop;i++) {
      EventSelection( ievent);
      if(ievent<0) break;
      GetEvent(ievent);
      AnalyzeEvent();  // do additional analysis
      EventDisplay();
    }//for
  }//if
}
