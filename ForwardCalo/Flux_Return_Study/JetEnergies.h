#ifndef __JETENERGIES_H__
#define __JETENERGIES_H__

//===============================================
/// \file JetEnergies.h
/// \brief copy of JetEvaluator.h that will add the energy absorbed by the plug door and the black hole to the list of items in the generated truth jet ntuple

// VERY IMPORTANT: In order to get the flux return energy the Flux Return must be set to active in the G4Setup file otherwise this may not work

/// \Original author Michael P. McCumber
/// \Modifiying author David Kapukchyan
//===============================================

#include <g4eval/JetEvalStack.h>

#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4HitContainer.h>

#include <TNtuple.h>
#include <TFile.h>

#include <string>

//class PHG4HitContainer;

/// \class JetEnergies
///
/// \This is from the old JetEvaluator.h file
/// \brief Compares reconstructed jets to truth jets
///
/// Plan: This module will trace the jet constituents to
/// the greatest contributor Monte Carlo jet and then
/// test one against the other.
///
class JetEnergies : public SubsysReco {

 public:
 
  JetEnergies(const std::string &name = "JETENERGIES",
	       const std::string &recojetname = "AntiKt_Tower_r0.3",
	       const std::string &truthjetname = "AntiKt_Truth_r0.3",
	       const std::string &filename = "g4eval_jets.root");
  virtual ~JetEnergies() {};
		
  int Init(PHCompositeNode *topNode);
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  void set_strict(bool b) {_strict = b;}
  
 private:

  float GetTotalEnergy(PHG4HitContainer* hit_object);

  std::string _recojetname;
  std::string _truthjetname;
  
  unsigned long _ievent;

  JetEvalStack* _jetevalstack;
  
  //----------------------------------
  // evaluator output ntuples

  bool _strict;
  unsigned int _errors;
  
  bool _do_recojet_eval;
  bool _do_truthjet_eval;
  
  TNtuple *_ntp_recojet;
  TNtuple *_ntp_truthjet;

  // evaluator output file
  std::string _filename;
  TFile *_tfile;

  //For some reason this declaration has to go here
  PHG4HitContainer* _FluxReturn_plus_hit_container;
  PHG4HitContainer* _FluxReturn_minus_hit_container;
  PHG4HitContainer* _BH_1_hit_container;
  PHG4HitContainer* _BH_Forward_hit_container;
  PHG4HitContainer* _BH_Negative_hit_container;

  // subroutines
  void printInputInfo(PHCompositeNode *topNode);    ///< print out the input object information (debugging upstream components)
  void fillOutputNtuples(PHCompositeNode *topNode); ///< dump the evaluator information into ntuple for external analysis
  void printOutputInfo(PHCompositeNode *topNode);   ///< print out the ancestry information for detailed diagnosis
};

#endif // __JETENERGIES_H__
