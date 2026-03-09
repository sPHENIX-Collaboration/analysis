/* 
JetMultSub SubsysReco module.

  Description: 
    This is a module that takes in reconstructed jets and subtracts the background using the multiplicity method.
    Estimates rho using the median of the average pt in a kT jet with radius 0.4.
    Corrects the multiplicity using the average truth multiplicity of a a jet with a given pt_reco.
    Subtracts the background from the jet pt using the formula:
      pt_sub = pt_reco - rho*(N_reco - <N_signal>(pt_reco))  

  Author: Tanner Mengel

  Date: 10/04/2023

*/

//____________________________________________________________________________..
// JetMultSub.h
#include "JetMultSub.h"
//____________________________________________________________________________..
// Fun4All includes
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>
//____________________________________________________________________________..
// PHOOL includes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/phool.h>
//____________________________________________________________________________..
// Jet includes
#include <jetbase/Jet.h>
#include <jetbase/JetMap.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/Jetv1.h>
//____________________________________________________________________________..
// standard includes
#include <cmath>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
//____________________________________________________________________________..
// Starting function definitions...


//____________________________________________________________________________..
// constructor. Sets default values
JetMultSub::JetMultSub(const std::string &name):
  SubsysReco(name)
  , m_reco_input("AntiKt_Tower_r04")
  , m_kt_input("Kt_Tower_r04")
  , m_subtracted_output("AntiKt_Tower_r04_sub") 
  , m_etaRange(-1.0, 1.0)
  , m_ptRange(5.0, 100.0)
{
  std::cout << "JetMultSub::JetMultSub(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
// destructor. Does nothing
JetMultSub::~JetMultSub()
{
  std::cout << "JetMultSub::~JetMultSub() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
// init. Does nothing
int JetMultSub::Init(PHCompositeNode *topNode)
{
  std::cout << "JetMultSub::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
// init run. Very important! Creates output node if it doesn't exist
int JetMultSub::InitRun(PHCompositeNode *topNode)
{
  CreateNode(topNode);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
// process event. This is where the magic happens
int JetMultSub::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() > 0){
    std::cout << "JetMultSub::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  }

  // get unsubtracted and subtracted jet collections
  JetMap *reco_jets = findNode::getClass<JetMap>(topNode, m_reco_input);
  JetMap *sub_jets = findNode::getClass<JetMap>(topNode, m_subtracted_output);
  if(!reco_jets){
    std::cout << "JetMultSub::process_event(PHCompositeNode *topNode) Missing input reco jet collection" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // loop over reco jets and subtract background
  int ijet = 0;
  for (JetMap::Iter iter = reco_jets->begin(); iter != reco_jets->end(); ++iter)
  {
    Jet *this_jet = iter->second;
    
    float this_pt = this_jet->get_pt();
    float this_phi = this_jet->get_phi();
    float this_eta = this_jet->get_eta();

    Jet *new_jet = new Jetv1();
    
    // subtract background
    int nConstituents =this_jet->size_comp();
    float rho = EstimateRho(topNode);
    float multiplicity_correction = GetMultiplicityCorrection(this_pt);
    float nConstituents_minus_multiplicity_correction = nConstituents - multiplicity_correction;
    float new_pt = this_pt - rho*nConstituents_minus_multiplicity_correction;

    // calculate new px, py, pz, e
    float new_et = new_pt*cosh(this_eta);
    float new_e = sqrt(pow(new_et,2) + pow(this_jet->get_pz(),2));
    float new_px = new_pt*cos(this_phi);
    float new_py = new_pt*sin(this_phi);
    float new_pz = this_jet->get_pz();


    // set new jet properties
    new_jet->set_px(new_px);
    new_jet->set_py(new_py);
    new_jet->set_pz(new_pz);
    new_jet->set_e(new_e);

    // add new jet to subtracted jet collection
    sub_jets->insert(new_jet);

    // debug info
    if (Verbosity() > 1 && this_pt > 5)
	  {
	    std::cout << "JetMultSub::process_event: old jet #" << ijet << ", old px / py / pz / e = " << this_jet->get_px() << " / " << this_jet->get_py() << " / " << this_jet->get_pz() << " / " << this_jet->get_e() << std::endl;
	    std::cout << "JetMultSub::process_event: new jet #" << ijet << ", new px / py / pz / e = " << new_jet->get_px() << " / " << new_jet->get_py() << " / " << new_jet->get_pz() << " / " << new_jet->get_e() << std::endl;
	  }

    ijet++;

  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
// reset event. Does nothing
int JetMultSub::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
// end run. You guessed it! does nothing
int JetMultSub::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..'
// this is the end. Does nothing
int JetMultSub::End(PHCompositeNode *topNode)
{
  std::cout << "JetMultSub::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
// reset. Does nothing
int JetMultSub::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
  {
    std::cout << "JetMultSub::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
// print. Prints meaningless message to screen
void JetMultSub::Print(const std::string &what) const
{
  if (Verbosity() > 0){
    std::cout << "JetMultSub::Print(const std::string &what) const Printing info for " << what << std::endl;
    std::cout << "Current status: 50% working" << std::endl;
    std::cout << "Errors codes: 49512323, 304, b.ss3, sos" << std::endl;
  }
}

//____________________________________________________________________________..
// create node. This is actually important. Creates the node if it doesn't exist
int JetMultSub::CreateNode(PHCompositeNode *topNode)
{
  // iterate to the DST node and add the jet collection if necessary
  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // Looking for the ANTIKT node
  PHCompositeNode *antiktNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "ANTIKT"));
  if (!antiktNode)
  {
    std::cout << PHWHERE << "ANTIKT node not found, doing nothing." << std::endl;
  }

  // store the new jet collection if it doesn't exist already
  JetMap *test_jets;
  test_jets = findNode::getClass<JetMap>(topNode, m_subtracted_output);
  if(!test_jets)
  {
    JetMap *sub_jets = new JetMapv1();
    PHIODataNode<PHObject> *subjetNode ;
    if (Verbosity() > 0)
    {
      std::cout << "JetMultSub::CreateNode : creating " << m_subtracted_output << std::endl;
    }
    subjetNode = new PHIODataNode<PHObject>(sub_jets, m_subtracted_output, "PHObject");
    antiktNode->addNode(subjetNode);
  }
  else
  {
    std::cout << "JetMultSub::CreateNode : " << m_subtracted_output << " already exists" << std::endl;
  }

  // all done!

  return Fun4AllReturnCodes::EVENT_OK;

}

//____________________________________________________________________________..
// estimate rho. Gets the event rho value from r=0.4 kT jets
float JetMultSub::EstimateRho(PHCompositeNode *topNode)
{
  
  if (Verbosity() > 0){
    std::cout << "JetMultSub::EstimateRho(PHCompositeNode *topNode) Estimating rho" << std::endl;
  }

  // get the kT jet collection
  JetMap *kt_jets = findNode::getClass<JetMap>(topNode, m_kt_input);
  if(!kt_jets){
    std::cout << "JetMultSub::EstimateRho(PHCompositeNode *topNode) Missing input kT jet collection" << std::endl;
    return -1.0;
  }

  // debug info
  if (Verbosity() > 0)
  {
    std::cout << "JetMultSub::EstimateRho(PHCompositeNode *topNode) Found input kT jet collection" << std::endl;
    std::cout << "JetMultSub::EstimateRho(PHCompositeNode *topNode) Number of kT jets: " << kt_jets->size() << std::endl;
  }

  // loop over kT jets and find the 2 hardest jets in the eta range
  int hardest_kT_jet_indices[2] = {-1,-1};
  float hardest_kT_jet_pts[2] = {-1.0,-1.0};
  for (JetMap::Iter iter = kt_jets->begin(); iter != kt_jets->end(); ++iter)
  {
    Jet *jet = iter->second;
    float pt = jet->get_pt();
    float eta = jet->get_eta();
    // float et = jet->get_et();

    if (eta > m_etaRange.first && eta < m_etaRange.second)
    {
      if (pt > hardest_kT_jet_pts[0])
      {
        hardest_kT_jet_pts[1] = hardest_kT_jet_pts[0];
        hardest_kT_jet_indices[1] = hardest_kT_jet_indices[0];
        hardest_kT_jet_pts[0] = pt;
        hardest_kT_jet_indices[0] = jet->get_id();
      }
      else if (pt > hardest_kT_jet_pts[1])
      {
        hardest_kT_jet_pts[1] = pt;
        hardest_kT_jet_indices[1] = jet->get_id();
      }
    }
  }


  // debug info
  if(Verbosity() > 0)
  {
    std::cout << "JetMultSub::EstimateRho(PHCompositeNode *topNode) Hardest kT jet indices: " << hardest_kT_jet_indices[0] << ", " << hardest_kT_jet_indices[1] << std::endl;
    std::cout << "JetMultSub::EstimateRho(PHCompositeNode *topNode) Hardest kT jet et: " << hardest_kT_jet_pts[0] << ", " << hardest_kT_jet_pts[1] << std::endl;
  }

  // get median value of the average et of constituents in all but the 2 hardest jets
  std::vector<float> et_over_nConstituents;
  for (JetMap::Iter iter = kt_jets->begin(); iter != kt_jets->end(); ++iter)
  {
    Jet *jet = iter->second;
    float pt = jet->get_pt();
    float eta = jet->get_eta();
    // float et = jet->get_et();
    int nConstituents = jet->size_comp();

    if (eta > m_etaRange.first && eta < m_etaRange.second)
    {
      int id = jet->get_id();
      if (id != hardest_kT_jet_indices[0] && id != hardest_kT_jet_indices[1])
      {
        et_over_nConstituents.push_back(pt/nConstituents);
      }
    }
  }

  // get median value
  float median_et_over_nConstituents = 0.0;
  if (et_over_nConstituents.size() > 0)
  {
    std::sort(et_over_nConstituents.begin(), et_over_nConstituents.end());
    median_et_over_nConstituents = et_over_nConstituents[et_over_nConstituents.size()/2];
  }

  // debug info
  if (Verbosity() > 0)
  {
    std::cout << "JetMultSub::EstimateRho(PHCompositeNode *topNode) Median et over nConstituents: " << median_et_over_nConstituents << std::endl;
  }

  // return median et over nConstituents
  return median_et_over_nConstituents;
}

//____________________________________________________________________________..
// get multiplicity correction. Gets the multiplicity correction for a given pt
float JetMultSub::GetMultiplicityCorrection(float pt)
{
  if (Verbosity() > 0)
  {
    std::cout << "JetMultSub::GetMultiplicityCorrection(float pt) Getting multiplicity correction" << std::endl;
  }
  
  
  // to do! Match jets and get the values for this function!
  std::vector<float> pt_reco_bins = {0.0,100.0};
  std::vector<float> multiplicity_corrections = {0.0};
  // get pt  bin
  int pt_bin = -1;
  int pt_size = pt_reco_bins.size();
  for (int i = 0; i < pt_size; i++){
    if (pt > pt_reco_bins[i]){
      pt_bin = i;
    }
  }
  float correction;
  if(pt_bin == -1){
    correction = 0.0;
  }
  else{
    correction = multiplicity_corrections[pt_bin];
  }
  return correction;
}
