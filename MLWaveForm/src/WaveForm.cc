//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in WaveForm.h.
//
// WaveForm(const std::string &name = "WaveForm")
// everything is keyed to WaveForm, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// WaveForm::~WaveForm()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int WaveForm::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int WaveForm::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int WaveForm::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int WaveForm::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int WaveForm::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int WaveForm::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int WaveForm::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void WaveForm::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "WaveForm.h"

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//____________________________________________________________________________..
WaveForm::WaveForm(const std::string &name):
 SubsysReco(name)
{
  outfile.open("data.txt",std::fstream::out);
  std::cout << "WaveForm::WaveForm(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
WaveForm::~WaveForm()
{
  std::cout << "WaveForm::~WaveForm() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int WaveForm::Init(PHCompositeNode * /*topNode*/)
{
  std::cout << "WaveForm::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveForm::InitRun(PHCompositeNode * /*topNode*/)
{
  std::cout << "WaveForm::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveForm::process_event(PHCompositeNode *topNode)
{
  TowerInfoContainer *towerinfocontainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERS_HCALOUT");
  if (towerinfocontainer)
  {
    unsigned int nchannels = towerinfocontainer->size();
    for (unsigned int channel = 0; channel < nchannels; channel++)
    {
      TowerInfo *rawtwr = towerinfocontainer->get_tower_at_channel(channel);
      std::cout << "energy: " << rawtwr->get_energy() << std::endl;
      std::cout << "time: " << rawtwr->get_time_float() << std::endl;
      for (int j = 0; j < 12; j++)
      {
	std::cout << "waveform_value[" << j << "]: " << rawtwr->get_waveform_value(j) << std::endl;
      }
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveForm::ResetEvent(PHCompositeNode * /*topNode*/)
{
  std::cout << "WaveForm::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveForm::EndRun(const int runnumber)
{
  std::cout << "WaveForm::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveForm::End(PHCompositeNode * /*topNode*/)
{
  std::cout << "WaveForm::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  outfile.close();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveForm::Reset(PHCompositeNode * /*topNode*/)
{
 std::cout << "WaveForm::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void WaveForm::Print(const std::string &what) const
{
  std::cout << "WaveForm::Print(const std::string &what) const Printing info for " << what << std::endl;
}
