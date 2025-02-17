/// ===========================================================================
/*! \file    TestPHFlags.cc
 *  \authors Derek Anderson
 *  \date    11.22.2024
 *
 *  A small F4A module to test if certain
 *  flags exist.
 */
/// ===========================================================================

#define TESTPHFLAGS_CC

// module definition
#include "TestPHFlags.h"

// f4a libraries
#include <fun4all/Fun4AllReturnCodes.h>

// pdbcalbase libraries
#include <pdbcalbase/PdbParameterMap.h>

// phool libraries
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/phool.h>

// c++ utilities
#include <iostream>
#include <vector>



// ctor/dtor ==================================================================

// ----------------------------------------------------------------------------
//! Default module constructor
// ----------------------------------------------------------------------------
TestPHFlags::TestPHFlags(const std::string& name, const std::string& flags, const bool debug)
  : SubsysReco(name)
  , m_flags(name)
  , m_doDebug(debug)
  , m_flagNode(flags)
{

  if (m_doDebug)
  {
    std::cout << "TestPHFlags::TestPHFlags(std::string &name, std::string&, bool) Calling ctor" << std::endl;
  }

}  // end ctor(std::string&, std::string&, bool)



// ----------------------------------------------------------------------------
//! Default module destructor
// ----------------------------------------------------------------------------
TestPHFlags::~TestPHFlags()
{

  if (m_doDebug)
  {
    std::cout << "TestPHFlags::~TestPHFlags() Calling dtor" << std::endl;
  }

}  // end dtor



// fun4all methods ============================================================ 

// ----------------------------------------------------------------------------
//! Process event, i.e. spit out the values of several flags
// ----------------------------------------------------------------------------
int TestPHFlags::process_event(PHCompositeNode* topNode)
{

  if (m_doDebug)
  {
    std::cout << "TestPHFlags::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  }

  // flags are stored under the PAR node
  PHNodeIterator itNode(topNode);
  PHCompositeNode* parNode = dynamic_cast<PHCompositeNode*>(itNode.findFirst("PHCompositeNode", "PAR"));
  if (!parNode)
  {
    std::cerr << PHWHERE << " PANIC: PAR node not found! No flags present, and aborting event!" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  // now retrieve flag node
  PdbParameterMap* flagNode = findNode::getClass<PdbParameterMap>(parNode, m_flagNode);
  if (!flagNode)
  {
    std::cerr << PHWHERE << " PANIC: " << m_flagNode << " node not found! Aborting event!" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
  }

  // now pull all flags from node and print
  // all int flags
  m_flags.FillFrom(flagNode);
  m_flags.printint();

  // and then explicitly look for these flags
  std::vector<std::string> flagsToCheck = {
    "HasBeamBackground_NullFilter",
    "HasBeamBackground_StreakSidebandFilter",
    "HasBeamBackground"
  };

  // if flag exists, print value
  for (const std::string& flag : flagsToCheck)
  {
    std::cout << "[" << flag << "] exists? " << m_flags.exist_int_param(flag);
    if (m_flags.exist_int_param(flag))
    {
      std::cout << " : value = " << m_flags.get_int_param(flag) << std::endl;
    }
    else
    {
      std::cout << std::endl;
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'

// end ========================================================================
