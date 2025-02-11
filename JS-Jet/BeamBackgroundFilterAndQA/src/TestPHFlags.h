// Tell emacs that this is a C++ source
//  -*- C++ -*-.
/// ===========================================================================
/*! \file    TestPHFlags.h
 *  \authors Derek Anderson
 *  \date    11.22.2024
 *
 *  A small F4A module to test if certain
 *  flags exist.
 */
/// ===========================================================================

#ifndef TESTPHFLAGS_H
#define TESTPHFLAGS_H

// f4a libraries
#include <fun4all/SubsysReco.h>

// phparameter libraries
#include <phparameter/PHParameters.h>

// c++ utilities
#include <string>

// forward declarations
class PHCompositeNode;



// ============================================================================
//! Test certain PHFlags
// ============================================================================
/*! A small F4A module to test if certain
 *  flags exist.
 */
class TestPHFlags : public SubsysReco
{

  public:

    // ctor/dtor
    TestPHFlags(const std::string& name = "TestPHFlags", const std::string& flags = "HasBeamBackground", const bool debug = false);
    ~TestPHFlags() override;

    // f4a methods
    int process_event(PHCompositeNode *topNode) override;

   private:

     ///! to retrieve flags
     PHParameters m_flags;

     ///! turn on/off extra debugging messages
     bool m_doDebug;

     ///! name of node where flags are stored
     std::string m_flagNode;

};  // end TestPHFlags

#endif

// end ========================================================================
