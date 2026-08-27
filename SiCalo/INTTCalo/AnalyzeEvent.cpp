#include "AnalyzeEvent.h"
#include "INTT_Calo_trkReco.h"
#include "INTT_Calo_trkUser.h"

TFile *hfile;

void AnalyzeInit(void) {
  //  hfile = new TFile("INTT_Calo_Ana.root","RECREATE");
  hfile = new TFile(rootfile.c_str(),"RECREATE");
  INTT_Calo_trkReco_Init();

  // *** user code are below 
  INTT_Calo_trkUser_Init();
}

void AnalyzeEnd(void) {

  INTT_Calo_trkReco_End();

  // *** user code are below
  INTT_Calo_trkUser_End();
  hfile->Close();
}

void AnalyzeEvent(void) {

  INTT_Calo_trkReco();

  // ***
  INTT_Calo_trkUser();
}

