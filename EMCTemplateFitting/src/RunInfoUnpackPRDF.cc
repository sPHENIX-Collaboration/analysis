#include "RunInfoUnpackPRDF.h"

#include "PROTOTYPE4_FEM.h"

#include <ffaobjects/EventHeaderv1.h>

#include <phparameter/PHParameters.h>

#include <pdbcalbase/PdbParameterMap.h>

#include <fun4all/Fun4AllBase.h>  // for Fun4AllBase::VERBOSITY_SOME
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>  // for SubsysReco

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <Event/Event.h>
#include <Event/EventTypes.h>
#include <Event/packet.h>

#include <cmath>  // for NAN
#include <iostream>
#include <string>
#include <utility>  // for pair, make_pair

using namespace std;

//____________________________________
RunInfoUnpackPRDF::RunInfoUnpackPRDF()
  : SubsysReco("RunInfoUnpackPRDF")
  , runinfo_node_name("RUN_INFO")
{
}

//_____________________________________
int RunInfoUnpackPRDF::InitRun(PHCompositeNode *topNode)
{
  CreateNodeTree(topNode);
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________
int RunInfoUnpackPRDF::process_event(PHCompositeNode *topNode)
{
  Event *event = findNode::getClass<Event>(topNode, "PRDF");
  if (!event)
  {
    if (Verbosity() >= VERBOSITY_SOME)
      cout << "RunInfoUnpackPRDF::Process_Event - Event not found" << endl;
    return Fun4AllReturnCodes::DISCARDEVENT;
  }

  // construct event info
  EventHeaderv1 *eventheader =
      findNode::getClass<EventHeaderv1>(topNode, "EventHeader");
  if (eventheader)
  {
    eventheader->set_RunNumber(event->getRunNumber());
    eventheader->set_EvtSequence(event->getEvtSequence());
    eventheader->set_EvtType(event->getEvtType());
    eventheader->set_TimeStamp(event->getTime());
    if (Verbosity())
    {
      eventheader->identify();
    }
  }

  // search for run info
  if (event->getEvtType() != BEGRUNEVENT)
    return Fun4AllReturnCodes::EVENT_OK;
  else
  {
    if (Verbosity() >= VERBOSITY_SOME)
    {
      cout << "RunInfoUnpackPRDF::process_event - with BEGRUNEVENT events ";
      event->identify();
    }

    map<int, Packet *> packet_list;

    PHParameters Params("RunInfo");

    // special treatment for EMCal tagging packet
    // https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test#What_is_new_in_the_data_structures_in_2017
    {
      int has_new_EMCal = 0;

      if (event->existPacket(PROTOTYPE4_FEM::PACKET_EMCAL_HIGHETA_FLAG))
      {
        // react properly - new emcal!
        has_new_EMCal = 1;
      }

      Params.set_double_param("EMCAL_Is_HighEta", has_new_EMCal);
      Params.set_int_param("EMCAL_Is_HighEta", has_new_EMCal);
    }

    // generic packets
    for (typ_channel_map::const_iterator it = channel_map.begin();
         it != channel_map.end(); ++it)
    {
      const string &name = it->first;
      const channel_info &info = it->second;

      if (packet_list.find(info.packet_id) == packet_list.end())
      {
        packet_list[info.packet_id] = event->getPacket(info.packet_id);
      }

      Packet *packet = packet_list[info.packet_id];

      if (!packet)
      {
        //          if (Verbosity() >= VERBOSITY_SOME)
        cout << "RunInfoUnpackPRDF::process_event - failed to locate packet "
             << info.packet_id << " from ";
        event->identify();

        Params.set_double_param(name, NAN);
        continue;
      }

      const int ivalue = packet->iValue(info.offset);

      const double dvalue = ivalue * info.calibration_const;

      if (Verbosity() >= VERBOSITY_SOME)
      {
        cout << "RunInfoUnpackPRDF::process_event - " << name << " = " << dvalue
             << ", raw = " << ivalue << " @ packet " << info.packet_id
             << ", offset " << info.offset << endl;
      }

      Params.set_double_param(name, dvalue);
    }

    for (map<int, Packet *>::iterator it = packet_list.begin();
         it != packet_list.end(); ++it)
    {
      if (it->second)
        delete it->second;
    }

    Params.SaveToNodeTree(topNode, runinfo_node_name);

    if (Verbosity() >= VERBOSITY_SOME)
      Params.Print();
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//_______________________________________
void RunInfoUnpackPRDF::CreateNodeTree(PHCompositeNode *topNode)
{
  PHNodeIterator nodeItr(topNode);
  // DST node
  PHCompositeNode *run_node = static_cast<PHCompositeNode *>(
      nodeItr.findFirst("PHCompositeNode", "RUN"));
  if (!run_node)
  {
    cout << "PHComposite node created: RUN" << endl;
    run_node = new PHCompositeNode("RUN");
    topNode->addNode(run_node);
  }

  PdbParameterMap *nodeparams =
      findNode::getClass<PdbParameterMap>(run_node, runinfo_node_name);
  if (not nodeparams)
  {
    run_node->addNode(new PHIODataNode<PdbParameterMap>(new PdbParameterMap(),
                                                        runinfo_node_name));
  }

  // DST node
  PHCompositeNode *dst_node = static_cast<PHCompositeNode *>(
      nodeItr.findFirst("PHCompositeNode", "DST"));
  if (!dst_node)
  {
    cout << "PHComposite node created: DST" << endl;
    dst_node = new PHCompositeNode("DST");
    topNode->addNode(dst_node);
  }

  EventHeaderv1 *eventheader = new EventHeaderv1();
  PHIODataNode<PHObject> *EventHeaderNode = new PHIODataNode<PHObject>(
      eventheader, "EventHeader", "PHObject");  // contain PHObject
  dst_node->addNode(EventHeaderNode);
}

void RunInfoUnpackPRDF::add_channel(
    const std::string &name,        //! name of the channel
    const int packet_id,            //! packet id
    const unsigned int offset,      //! offset in packet data
    const double calibration_const  //! conversion constant from integer to
                                    //! meaningful value
)
{
  channel_map.insert(
      make_pair(name, channel_info(packet_id, offset, calibration_const)));
}
