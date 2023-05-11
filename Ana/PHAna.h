#ifndef __PHANA_H__
#define __PHANA_H__

#include <fun4all/SubsysReco.h>
#include <string>

//Forward declerations
class PHCompositeNode;
class PHG4HitContainer;
class PHG4TruthInfoContainer;
class TFile;
class TNtuple;


//Brief: basic ntuple and histogram creation for sim evaluation
class PHAna: public SubsysReco
{
 public: 
  //Default constructor
  PHAna(const std::string &name="PHAna");

  //Initialization, called for initialization
  int Init(PHCompositeNode *);

  //Process Event, called for each event
  int process_event(PHCompositeNode *);

  //End, write and close files
  int End(PHCompositeNode *);

  //Change output filename
  void set_filename(const char* file)
  { if(file) _outfile = file; }

  float Square(float x){ return x*x; }

  //Flags of different kinds of outputs
  enum Flag
  {
    //all disabled
    NONE = 0,

    //truth
    TRUTH = (1<<0),

    //histograms
    HIST = (1<<1),

    //Sampling fractions
    SF = (1<<2),

    //Inner HCal Towers
    HCALIN_TOWER = (1<<3),
    
    //All flags ON
    ALL = (1<<4)-1
  };

  //Set the flag
  //Flags should be set like set_flag(PHAna::TRUTH, true) from macro
  void set_flag(const Flag& flag, const bool& value)
  {
   if(value) _flags |= flag;
   else _flags &= (~flag);
  }

  //User modules
  void fill_truth_ntuple(PHCompositeNode*);
  void fill_sf_ntuple(PHCompositeNode*);
  void create_histos();
  void fill_histos(PHCompositeNode*);

 private:
  //output filename
  std::string _outfile;
   
  //Event counter
  int _event;

  //Get all the nodes
  void GetNodes(PHCompositeNode *);
  
  //flags
  unsigned int _flags;

  //TNtuples
  TNtuple* _truth;
  TNtuple* _sf;
  TNtuple* _hcalin_towers;

  //Node pointers
  PHG4TruthInfoContainer* _truth_container;
  PHG4HitContainer* _hcalout_hit_container;
  PHG4HitContainer* _hcalin_hit_container;
  PHG4HitContainer* _cemc_hit_container;
  PHG4HitContainer* _hcalout_abs_hit_container;
  PHG4HitContainer* _hcalin_abs_hit_container;
  PHG4HitContainer* _cemc_abs_hit_container;
  PHG4HitContainer* _cemc_electronics_hit_container;
  PHG4HitContainer* _hcalin_spt_hit_container;


};

#endif //* __PHANA_H__ *//
