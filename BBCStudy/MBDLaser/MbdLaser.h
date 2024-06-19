#ifndef __MBDLASER_H__
#define __MBDLASER_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <map>
#include <TFile.h>

//Forward declerations
class PHCompositeNode;
class EventHeader;
class MbdOut;
class MbdPmtContainer;
class TFile;
class TTree;
class TDatabasePDG;
class TRandom3;
class TH1;
class TH2;
class TGraphErrors;
class TF1;
class TCanvas;


//Brief: basic TTree and histogram creation for sim evaluation
class MbdLaser: public SubsysReco
{
public: 

  //Default constructor
  MbdLaser(const std::string &name="MbdLaser");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  //Initialization at start of every run
  int InitRun(PHCompositeNode *);

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int End(PHCompositeNode *);

  //Change output filename
  void set_savefile(const char *f) { _savefname = f; }

  void set_run(const int r) { f_run = r; }

private:

  //
  void LaserDST(PHCompositeNode *topNode);

  int  Getpeaktime(TH1 *h);

  //output filename
  std::string _savefname;
  TFile* _savefile{ nullptr };
   
  //input file (abdul)
 // std::string _file;
 // TFile* _file;
  int nprocessed{0};     // num events processed

  //Output
  TTree* _tree{ nullptr };
  Int_t    f_run;
  Int_t    f_evt{ 0 };
  Int_t    f_ch;
  Float_t  f_qmean;
  Float_t  f_qmerr;
  Float_t  f_tmean;
  Float_t  f_tmerr;


  Float_t  f_mbdq[2]; // total charge (currently npe) in each arm
  Float_t  f_mbdt[2]; // time in arm
  Float_t  f_mbdte[2]; // earliest hit time in arm
  Float_t  f_mbdt0;   // start time

  TH1 *h_mbdq[128];   // q in each tube
  TH1* h_mbdt[128];  //  t in ecah tube 

  TH2* h2_mbdbtt;    // Timne vs ch
  TH2* h2_mbdbq;    // charge vs ch 

  TGraphErrors *g_mbdq[128];   // q in each tube
  TGraphErrors *g_mbdt[128];   // t in each tube

 
  TCanvas *c_mbdt;    // Canvas to 
  TH1 *hevt_mbdt[2];  // time in each mbd, per event
 
  //
  TRandom3*     _rndm;

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //Node pointers
  EventHeader* _evtheader;
  MbdOut* _mbdout;
  MbdPmtContainer* _mbdpmts;
 // MbdPmtHit*  _mbdpmthit;

};

#endif //* __MBDLASER_H__ *//


