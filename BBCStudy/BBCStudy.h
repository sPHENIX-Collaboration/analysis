#ifndef __BBCSTUDY_H__
#define __BBCSTUDY_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <map>
#include <TFile.h>

//Forward declerations
class PHCompositeNode;
class PHG4HitContainer;
class PHG4TruthInfoContainer;
class EventHeader;
class BbcOut;
class TFile;
class TTree;
class TDatabasePDG;
class TRandom3;
class TH1;
class TH2;
class TF1;
class TCanvas;


//Brief: basic ntuple and histogram creation for sim evaluation
class BBCStudy: public SubsysReco
{
public: 

  //Default constructor
  BBCStudy(const std::string &name="BBCStudy");

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

  void set_tres(const Float_t tr) { _tres = tr; }

private:

  //
  void CheckDST(PHCompositeNode *topNode);

  //output filename
  std::string _savefname;
  TFile* _savefile;
   
  int nprocessed{0};     // num events processed

  //Output
  TTree* _tree;
  Int_t    f_evt;
  Float_t  f_bimp;    // impact parameter
  Int_t    f_ncoll;   // number n-n collisions
  Int_t    f_npart;   // number participants
  Float_t  f_vx;      // true x vertex of collision point
  Float_t  f_vy;      // true y vertex
  Float_t  f_vz;      // true z vertex
  Float_t  f_vt;       // true start time
  Short_t  f_bbcn[2]; // num hits for each arm (north and south)
  Float_t  f_bbcq[2]; // total charge (currently npe) in each arm
  Float_t  f_bbct[2]; // time in arm
  Float_t  f_bbcte[2]; // earliest hit time in arm
  Float_t  f_bbcz;    // z-vertex
  Float_t  f_bbct0;   // start time

  TH1* h_bbcq[128];   // q in each tube
  TH1* h_bbcqtot[2];  // total q in bbc arms
  TH2* h2_bbcqtot;    // north q vs south q
  TH1* h_ztrue;       // true z-vertex
  TH1* h_tdiff;       // time diff between estimated and real time
  TH2* h2_tdiff_ch;   // time diff by channel

  TCanvas *c_bbct;    // Canvas to 
  TH1 *hevt_bbct[2];  // time in each bbc, per event
  TF1 *gaussian;

  std::map<int,int> _pids;  // PIDs of tracks in the BBC

  //
  TDatabasePDG* _pdg;
  TRandom3*     _rndm;
  Float_t       _tres;    // time resolution of one channel

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //Node pointers
  PHG4TruthInfoContainer* _truth_container;
  PHG4HitContainer* _bbchits;
  EventHeader* _evtheader;

};

#endif //* __BBCSTUDY_H__ *//
