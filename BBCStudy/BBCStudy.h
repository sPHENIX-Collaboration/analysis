#ifndef __BBCSTUDY_H__
#define __BBCSTUDY_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <TFile.h>

//Forward declerations
class PHCompositeNode;
class PHG4HitContainer;
class PHG4TruthInfoContainer;
class TFile;
class TTree;
class TDatabasePDG;


//Brief: basic ntuple and histogram creation for sim evaluation
class BBCStudy: public SubsysReco
{
public: 

  //Default constructor
  BBCStudy(const std::string &name="BBCStudy");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  //Initialization at start of Run
  int InitRun(PHCompositeNode *);

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int End(PHCompositeNode *);

  //Change output filename
  void set_savefile(const char *f) { _savefname = f; }

private:

  //output filename
  std::string _savefname;
  TFile* _savefile;
   
  //Output
  TTree* _tree;
  Int_t    f_evt;
  Short_t  f_bbcn[2]; // num hits
  Float_t  f_bbcq[2]; // charge (currently npe)
  Float_t  f_bbct[2]; // time
  Float_t  f_bbcz;  // z-vertex
  Float_t  f_bbct0; // start time


  TDatabasePDG* _pdg;

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //Node pointers
  PHG4TruthInfoContainer* _truth_container;
  PHG4HitContainer* _bbchits;

};

#endif //* __BBCSTUDY_H__ *//
