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
#include "IEventDisplay.h"
#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

#include "LoadData.cpp"
#include "AnalyzeEvent.cpp"
#include "IEventDisplay.cpp"
#include "INTT_Calo_trkReco.cpp"
//#include "CalcEmcClusShape.cpp"
#include "INTT_Calo_trkUser.cpp"


void INTT_Calo_Ana(string idatafile_,int imode_ = 2,
		   string rootfile_="INTT_Calo_Ana.root",
		   float xBC_ =  -0.018, float yBC_ = 0.126,
		   int istart_=0, int nevent_=100000){
  cout <<"Welcome to INTT_Calo_Ana"<<endl;
  cout <<"This is analysis program of INTT_Calo nDST with event display!"<<endl;

  const int MaxLoop = 100;
  xBC = xBC_;
  yBC = yBC_;

  idatafile = idatafile_;
  cout <<"data file:"<<idatafile.c_str()<<endl;

  LoadData(idatafile);
  
  NmaxEvent = evtTree->GetEntries();  
  cout <<"# of events in the file:"<<NmaxEvent<<endl;
  
  imode = imode_;
  if(imode == 1) { // Data processing mode
    int istart = istart_;
    int nevent = nevent_;
    
    if(istart+nevent > NmaxEvent) {
      cout <<" Exceeds the # of events in the file" <<endl;
      nevent = NmaxEvent - istart;
    }
    cout << "analyze "<< nevent << " events" <<endl;

    rootfile = rootfile_;
    cout <<"rootfile="<<rootfile.c_str()<<endl;

    AnalyzeInit();
    for(int i=istart;i<istart+nevent;i++) {
      i_event = i;
      GetEvent(i_event);
      AnalyzeEvent();

      if(i%1000 == 0) cout << i << endl;
    }//for
    AnalyzeEnd();
  } else if(imode == 2 ) { // Event Display mode
    gStyle->SetCanvasPreferGL();
    AnalyzeInit();
    for(int i=0;i<MaxLoop;i++) {
      EventSelection( i_event);
      if(i_event<0) break;
      GetEvent(i_event);
      AnalyzeEvent();  // do additional analysis
      IEventDisplay(0);
      IEventDisplay(1);
    }//for
  }//if
}
