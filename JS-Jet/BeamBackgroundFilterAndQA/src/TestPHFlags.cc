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

// c++ utilities
#include <iostream>
#include <vector>

// f4a libraries
#include <fun4all/Fun4AllReturnCodes.h>
//#include <ffaobjects/FlagSave.h>
#include <ffaobjects/FlagSavev1.h>
#include <phool/recoConsts.h>

// phool libraries
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHFlag.h>



// ctor/dtor ==================================================================

// ----------------------------------------------------------------------------
//! Default module constructor
// ----------------------------------------------------------------------------
TestPHFlags::TestPHFlags(const std::string& name, const bool debug)
  : SubsysReco(name)
  , m_consts(nullptr)
  , m_doDebug(debug)
{

  if (m_doDebug)
  {
    std::cout << "TestPHFlags::TestPHFlags(const std::string &name) Calling ctor" << std::endl;
  }

}  // end ctor(std::string&, bool)



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

  // instantiate flags database
  // and print all int flags
  m_consts = recoConsts::instance();
  m_consts->PrintIntFlags();

  // and then explicitly look for these flags
  std::vector<std::string> flagsToCheck = {
    "HasBeamBackground_NullFilter",
    "HasBeamBackground_StreakSidebandFilter",
    "HasBeamBackground"
  };

  // if flag exists, print value
  for (const std::string& flag : flagsToCheck)
  {
    std::cout << "[" << flag << "] exists? " << m_consts->FlagExist(flag);
    if (m_consts->FlagExist(flag))
    {
      std::cout << " : value = " << m_consts->get_IntFlag(flag) << std::endl;
    }
    else
    {
      std::cout << std::endl;
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;

}  // end 'process_event(PHCompositeNode*)'

// end ========================================================================
