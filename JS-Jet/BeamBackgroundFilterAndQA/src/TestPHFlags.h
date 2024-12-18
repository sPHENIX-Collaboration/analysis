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

// c++ utilities
#include <string>

// f4a libraries
#include <fun4all/SubsysReco.h>

// forward declarations
class FlagSavev1;
class PHCompositeNode;
class recoConsts;



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
    TestPHFlags(const std::string &name = "TestPHFlags", const bool debug = false);
    ~TestPHFlags() override;

    // f4a methods
    int process_event(PHCompositeNode *topNode) override;

   private:

     ///! reco consts (for flags)
     recoConsts* m_consts;

     ///! turn on/off extra debugging messages
     bool m_doDebug;

};  // end TestPHFlags

#endif

// end ========================================================================
