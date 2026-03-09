/// ---------------------------------------------------------------------------
/*! \file   TypesLinkDef.h
 *  \author Derek Anderson
 *  \date   11.16.2023
 *
 *  LinkDef file for the utilities namespace of the
 *  sPHENIX Cold QCD Correlator Analysis.
 */
/// ---------------------------------------------------------------------------

// stl collection
#include <vector>
#include <utility>
// analysis types
#include "ClustInfo.h"
#include "CstInfo.h"
#include "FlowInfo.h"
#include "GEvtInfo.h"
#include "JetInfo.h"
#include "ParInfo.h"
#include "REvtInfo.h"
#include "TrkInfo.h"
#include "TwrInfo.h"

#ifdef __CINT__

#pragma link C++ nestedclasses;

// base utility types
#pragma link C++ class ClustInfo+;
#pragma link C++ class CstInfo+;
#pragma link C++ class FlowInfo+;
#pragma link C++ class GEvtInfo+;
#pragma link C++ class JetInfo+;
#pragma link C++ class ParInfo+;
#pragma link C++ class REvtInfo+;
#pragma link C++ class TrkInfo+;
#pragma link C++ class TwrInfo+;

// stl collections of utility classes
#pragma link C++ class std::vector<ClustInfo>+;
#pragma link C++ class std::vector<CstInfo>+;
#pragma link C++ class std::vector<FlowInfo>+;
#pragma link C++ class std::vector<JetInfo>+;
#pragma link C++ class std::vector<ParInfo>+;
#pragma link C++ class std::vector<TrkInfo>+;
#pragma link C++ class std::vector<TwrInfo>+;
#pragma link C++ class std::vector<vector<CstInfo> >+;
#pragma link C++ class std::vector<std::vector<ParInfo> >+;
#pragma link C++ class std::pair<ParInfo, ParInfo>+;

#endif

// end ------------------------------------------------------------------------

