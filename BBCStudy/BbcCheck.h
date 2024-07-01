#ifndef __BBCCHECK_H__
#define __BBCCHECK_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <map>
#include <TFile.h>

//Forward declerations
class PHCompositeNode;
class EventHeader;
class MbdOut;
class MbdPmtContainer;
class Gl1Packet;
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
class BbcCheck: public SubsysReco
{
public: 

  //Default constructor
  BbcCheck(const std::string &name="BbcCheck");

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
  void CheckDST(PHCompositeNode *topNode);

  int  Getpeaktime(TH1 *h);
  void process_gl1( PHCompositeNode *topNode );
  void process_zdc( PHCompositeNode *topNode );
  void process_emcal( PHCompositeNode *topNode );
  void process_ohcal( PHCompositeNode *topNode );
  void process_ihcal( PHCompositeNode *topNode );

  //output filename
  std::string _savefname;
  TFile* _savefile;
   
  int nprocessed{0};     // num events processed

  //Output
  TTree* _tree{ nullptr };
  TTree* _tree2{ nullptr };  // for event by event info
  Int_t    f_run;
  Int_t    f_evt;
  Short_t  f_cross;
  Int_t    f_ch;
  Float_t  f_qmean;
  Float_t  f_qmerr;

  ULong64_t f_rtrig{0};
  ULong64_t f_ltrig{0};
  ULong64_t f_strig{0};

  Float_t  f_bimp;    // impact parameter
  Int_t    f_ncoll;   // number n-n collisions
  Int_t    f_npart;   // number participants
  Float_t  f_vx;      // true x vertex of collision point
  Float_t  f_vy;      // true y vertex
  Float_t  f_vz;      // true z vertex
  Float_t  f_vt;       // true start time
  Short_t  f_bbcn[2];  // num hits for each arm (north and south)
  Float_t  f_bbcq[2];  // total charge (currently npe) in each arm
  Float_t  f_bbct[2];  // time in arm
  Float_t  f_bbcte[2]; // earliest hit time in arm
  Float_t  f_bz;       // z-vertex
  Float_t  f_bbct0;    // start time

  TH1 *h_bbcq[128];   // q in each tube
  TGraphErrors *g_bbcq[128];   // q in each tube
  TH1 *h_bbcqtot[2];  // total q in bbc arms
  TH1 *h_bbcqsum;     // total q in bbc arms
  TH2 *h2_bbcqsum;    // north q vs south q

  TH1 *h2_bbcqtot_bz[2];  // total q in bbc arms, vs bz
  TH2 *h2_bbcqsum_bz;    // north q vs south q, vs bz

  TH1 *h_emcale;
  TH1 *h_emcaltimecut;
  TH1 *h_ohcale;
  TH1 *h_ohcaltimecut;
  TH1 *h_ihcale;
  TH1 *h_ihcaltimecut;
  TH1 *h_zdce;
  TH1 *h_zdcse{nullptr};
  TH1 *h_zdcne{nullptr};
  TH1 *h_zdctimecut;
  TH1 *h_bz;
  TH1 *h_bztrig[5]; // 5 MBDNS triggers
  TH2 *h2_tt;
  TH2 *h2_tq;
  TH2 *h2_slew[128];
  TH1 *h_cross;

  std::vector<uint64_t> mbdtrigbits;

  TCanvas *c_bbct;    // Canvas to 
  TH1 *hevt_bbct[2];  // time in each bbc, per event
  TF1 *gaussian;

  TH1 *h_bpmt_bad;

  std::map<int,int> _pids;  // PIDs of tracks in the BBC

  //
  TRandom3*     _rndm;
  Float_t       _tres;    // time resolution of one channel

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //Node pointers
  EventHeader* _evtheader;
  MbdOut* _mbdout;
  MbdPmtContainer* _mbdpmts;
  Gl1Packet* _gl1raw;

};

#endif //* __BBCCHECK_H__ *//
