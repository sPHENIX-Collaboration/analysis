
#include "JetEnergies.h"

#include <cassert>

#include <g4eval/JetEvalStack.h>
#include <g4eval/JetRecoEval.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <fun4all/SubsysReco.h>
#include <phool/PHCompositeNode.h>
#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <g4main/PHG4HitContainer.h>

#include <TNtuple.h>
#include <TFile.h>

#include <iostream>
#include <cmath>

using namespace std;

JetEnergies::JetEnergies(const string &name,
			   const string &recojetname,
			   const string &truthjetname,
			   const string &filename) 
  : SubsysReco(name),
    _recojetname(recojetname),
    _truthjetname(truthjetname),
    _ievent(0),
    _jetevalstack(NULL),
    _strict(false),
    _errors(0),
    _do_recojet_eval(true),
    _do_truthjet_eval(true),
    _ntp_recojet(NULL),
    _ntp_truthjet(NULL),
    _filename(filename),
    _tfile(NULL),
    _FluxReturn_plus_hit_container(NULL),
    _FluxReturn_minus_hit_container(NULL),
    _BH_1_hit_container(NULL),
    _BH_Forward_hit_container(NULL),
    _BH_Negative_hit_container(NULL)
{
  verbosity = 0;
}

int JetEnergies::Init(PHCompositeNode *topNode) {
  
  _ievent = 0;

  _tfile = new TFile(_filename.c_str(), "RECREATE");
 
  if (_do_recojet_eval) _ntp_recojet = new TNtuple("ntp_recojet","reco jet => max truth jet",
						   "event:id:ncomp:eta:phi:e:pt:"
						   "gid:gncomp:geta:gphi:ge:gpt:"
						   "efromtruth");

  if (_do_truthjet_eval) _ntp_truthjet = new TNtuple("ntp_truthjet","truth jet => best reco jet",
						     "event:gid:gncomp:geta:gphi:ge:gpt:"
						     "id:ncomp:eta:phi:e:pt:"
						     "efromtruth:e_FR_plus:e_FR_minus:e_BH1:e_BH_plus:e_BH_minus");

  
  return Fun4AllReturnCodes::EVENT_OK;
}

//Added 'InitRun(...) since the hit container for the Flux Return was not being intialized in 'Init(...){...}'
int JetEnergies::InitRun(PHCompositeNode *topNode)
{
  //cout << "In InitRun" << endl;
  //Here I add the container for the Plug Door hits
  _FluxReturn_plus_hit_container = findNode::getClass<PHG4HitContainer>(topNode,      "G4HIT_FLUXRET_ETA_PLUS");
  _FluxReturn_minus_hit_container = findNode::getClass<PHG4HitContainer>(topNode,      "G4HIT_FLUXRET_ETA_MINUS");

  _BH_1_hit_container = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_BH_1");
  _BH_Forward_hit_container = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_BH_FORWARD_PLUS");
  _BH_Negative_hit_container = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_BH_FORWARD_NEG");


  //Used for debugging
  /*  
  if( _FluxReturn_hit_container )
    {
      cout << "Correct name" << endl;
    }
  */

  return Fun4AllReturnCodes::EVENT_OK;
  
}

int JetEnergies::process_event(PHCompositeNode *topNode) {

  //cout << "In process Event" << endl;

  if (!_jetevalstack) {
    _jetevalstack = new JetEvalStack(topNode,_recojetname,_truthjetname); 
    _jetevalstack->set_strict(_strict);
    _jetevalstack->set_verbosity(verbosity+1);
  } else {
    _jetevalstack->next_event(topNode);
  }

  //-----------------------------------
  // print what is coming into the code
  //-----------------------------------
  
  printInputInfo(topNode);
  
  //---------------------------
  // fill the Evaluator NTuples
  //---------------------------

  fillOutputNtuples(topNode);

  //--------------------------------------------------
  // Print out the ancestry information for this event
  //--------------------------------------------------
  
  printOutputInfo(topNode);
  
  ++_ievent;

  return Fun4AllReturnCodes::EVENT_OK;
}

int JetEnergies::End(PHCompositeNode *topNode) {
  
  _tfile->cd();

  if (_do_recojet_eval) _ntp_recojet->Write();
  if (_do_truthjet_eval) _ntp_truthjet->Write();
  
  _tfile->Close();

  delete _tfile;

  if (verbosity > 0) {
    cout << "========================== JetEnergies::End() ============================" << endl;
    cout << " " << _ievent << " events of output written to: " << _filename << endl;
    cout << "===========================================================================" << endl;
  }

  delete _jetevalstack;
  
  return Fun4AllReturnCodes::EVENT_OK;
}

void JetEnergies::printInputInfo(PHCompositeNode *topNode) {
  // to be implemented later if needed
  return;
}

void JetEnergies::printOutputInfo(PHCompositeNode *topNode) {
  // to be implemented later if needed
  return;
}

void JetEnergies::fillOutputNtuples(PHCompositeNode *topNode) {
  
  if (verbosity > 2) cout << "JetEnergies::fillOutputNtuples() entered" << endl;

  float e_FR_plus = GetTotalEnergy( _FluxReturn_plus_hit_container );
  float e_FR_minus = GetTotalEnergy( _FluxReturn_minus_hit_container );
  float e_BH1 = GetTotalEnergy( _BH_1_hit_container );
  float e_BH_plus = GetTotalEnergy( _BH_Forward_hit_container );
  float e_BH_minus = GetTotalEnergy( _BH_Negative_hit_container );

  JetRecoEval*   recoeval = _jetevalstack->get_reco_eval();
  //JetTruthEval* trutheval = _jetevalstack->get_truth_eval();
 
  //-------------------------
  // fill the reco jet ntuple
  //-------------------------

  if (_do_recojet_eval) {
    if (verbosity > 1) cout << "JetEnergies::filling recojet ntuple..." << endl;

    JetMap* recojets = findNode::getClass<JetMap>(topNode,_recojetname.c_str());
    if (!recojets) {
      cerr << PHWHERE << " ERROR: Can't find " << _recojetname << endl;
      exit(-1);
    }
  
    // for every recojet
    for (JetMap::Iter iter = recojets->begin();
	 iter != recojets->end();
	 ++iter) {
      Jet* recojet = iter->second;
      Jet* truthjet = recoeval->max_truth_jet_by_energy(recojet);

      float id    = recojet->get_id();
      float ncomp = recojet->size_comp();
      float eta   = recojet->get_eta();
      float phi   = recojet->get_phi();
      float e     = recojet->get_e();
      float pt    = recojet->get_pt();

      float gid        = NAN;
      float gncomp     = NAN;
      float geta       = NAN;
      float gphi       = NAN;
      float ge         = NAN;
      float gpt        = NAN;
      float efromtruth = NAN;

      if (truthjet) {
	gid    = truthjet->get_id();
	gncomp = truthjet->size_comp();
	geta   = truthjet->get_eta();
	gphi   = truthjet->get_phi();
	ge     = truthjet->get_e();
	gpt    = truthjet->get_pt();
	efromtruth = recoeval->get_energy_contribution(recojet,truthjet);
      }
      
      float recojet_data[14] = {(float) _ievent,
				id,
				ncomp,
				eta,
				phi,
				e,
				pt,
				gid,
				gncomp,
				geta,
				gphi,
				ge,
				gpt,
				efromtruth
      };

      _ntp_recojet->Fill(recojet_data);
    }
  }

  //-------------------------
  // fill the truth jet ntuple
  //-------------------------

  if (_do_truthjet_eval) {
    if (verbosity > 1) cout << "JetEnergies::filling truthjet ntuple..." << endl;

    JetMap* truthjets = findNode::getClass<JetMap>(topNode,_truthjetname.c_str());
    if (!truthjets) {
      cerr << PHWHERE << " ERROR: Can't find " << _truthjetname << endl;
      exit(-1);
    }
  
    // for every truthjet
    for (JetMap::Iter iter = truthjets->begin();
	 iter != truthjets->end();
	 ++iter) {
      Jet* truthjet = iter->second;
      Jet* recojet = recoeval->best_jet_from(truthjet);

      float gid    = truthjet->get_id();
      float gncomp = truthjet->size_comp();
      float geta   = truthjet->get_eta();
      float gphi   = truthjet->get_phi();
      float ge     = truthjet->get_e();
      float gpt    = truthjet->get_pt();

      float id         = NAN;
      float ncomp      = NAN;
      float eta        = NAN;
      float phi        = NAN;
      float e          = NAN;
      float pt         = NAN;
      float efromtruth = NAN;

      if (recojet) {
	id         = recojet->get_id();
	ncomp      = recojet->size_comp();
	eta        = recojet->get_eta();
	phi        = recojet->get_phi();
	e          = recojet->get_e();
	pt         = recojet->get_pt();
	efromtruth = recoeval->get_energy_contribution(recojet,truthjet);
      }
      
      float truthjet_data[19] = {(float) _ievent,
				 gid,
				 gncomp,
				 geta,
				 gphi,
				 ge,
				 gpt,
				 id,
				 ncomp,
				 eta,
				 phi,
				 e,
				 pt,
				 efromtruth,
				 e_FR_plus,
				 e_FR_minus,
				 e_BH1,
				 e_BH_plus,
				 e_BH_minus
      };

      _ntp_truthjet->Fill(truthjet_data);
    }
  }

  return;
}


//Function that iterates over all the hits for a given object and returns the total energy deposited in that object requires that the object is set as active in the G4_Setup file otherwise may not work
float JetEnergies::GetTotalEnergy(PHG4HitContainer *hit_object)
{
  //cout << "Executing Modified part of code" << endl;
  float e_object = 0;//Holds Energy deposited in object of interest
  //Check if hit container exists mostly used for debugging
  if( hit_object )
    {
      //cout << "Execute my code" << endl;      
      PHG4HitContainer::ConstRange object_hit_range = hit_object->getHits();
      //For loop that will store the total energy deposited in the object in 'e_object'
      //cout << "Starting for loop" << endl;
      for (PHG4HitContainer::ConstIterator hit_iter = object_hit_range.first; hit_iter != object_hit_range.second; hit_iter++)
	{
	  PHG4Hit *this_hit = hit_iter->second;
	  assert(this_hit);
	  e_object += this_hit->get_edep();
	}
      
      //cout << "End of for loop" << endl;
      //cout << "Value of e_FR: " << e_FR << endl;
      
    }
  //cout << "Executing Rest of original code" << endl;
  return e_object;

}
