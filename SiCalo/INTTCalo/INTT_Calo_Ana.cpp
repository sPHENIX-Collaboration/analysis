#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include <TFile.h>
#include <TTree.h>
#include <TStyle.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TNtuple.h>

#include "INTT_Calo_Ana.h"
#include "LoadData.h"
#include "AnalyzeEvent.h"
#include "EventDisplay.h"
#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

#include "LoadData.cpp"
#include "AnalyzeEvent.cpp"
#include "EventDisplay.cpp"
#include "INTT_Calo_trkReco.cpp"
#include "INTT_Calo_trkUser.cpp"


void INTT_Calo_Ana(void) {

  cout <<"Welcome to INTT_Calo_Ana"<<endl;
  cout <<"This is analysis program of INTT_Calo nDST with event display!"<<endl;

  const int MaxLoop = 100;

  // open data file and load the data
  //input data file
  //  idatafile = "run2pp/53876/ana_53876_00001.root";
  //  idatafile = "../../2605/ana_53879_1kevt_4evtdisplay.root";
  //  idatafile = "53876_100evt/ana_53876_00000.root";
  //  idatafile = "53876_100evt/merge53876.root";
  //
  // current data. 10 files are merged
  //  idatafile = "run2pp_new/ana_53876_0000m.root";
  // simulaiton data
  // electron
  idatafile = "ana_electron/single_e_1M.root";
  // pi minus
  //idatafile = "ana_pionn/single_pionn_1M.root";

  cout << "Enter datafile to analyze" <<endl;
  cin >> idatafile;

  LoadData(idatafile);
  NmaxEvent = evtTree->GetEntries();  
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

    cout << "Enter name of the output ROOT file"<<endl;
    cin >> rootfile;

    AnalyzeInit();
    for(int i=istart;i<istart+nevent;i++) {
      i_event = i;
      GetEvent(i_event);
      AnalyzeEvent();

      if(i%1000 == 0) cout << i << endl;
    }//for
    AnalyzeEnd();
  } else if(imode == 2 ) { // Event Display
    gStyle->SetCanvasPreferGL();
    AnalyzeInit();
    for(int i=0;i<MaxLoop;i++) {
      EventSelection( i_event);
      if(i_event<0) break;
      GetEvent(i_event);
      AnalyzeEvent();  // do additional analysis
      EventDisplay(0);
      EventDisplay(1);
    }//for
  }//if
}
