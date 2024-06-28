// ----------------------------------------------------------------------------
// 'TypesLinkDef.h'
// Derek Anderson
// 11.16.2023
//
// LinkDef file for the utilities namespace of the
// sPHENIX Cold QCD Correlator Analysis.
// ----------------------------------------------------------------------------

// stl collection
#include <vector>
#include <utility>
// analysis types
#include "ClustInfo.h"
#include "CstInfo.h"
#include "FlowInfo.h"
#include "GenInfo.h"
#include "JetInfo.h"
#include "ParInfo.h"
#include "RecoInfo.h"
#include "TrkInfo.h"

#ifdef __CINT__

#pragma link C++ nestedclasses;

// base utility types
#pragma link C++ class ClustInfo+;
#pragma link C++ class CstInfo+;
#pragma link C++ class FlowInfo+;
#pragma link C++ class GenInfo+;
#pragma link C++ class JetInfo+;
#pragma link C++ class ParInfo+;
#pragma link C++ class RecoInfo+;
#pragma link C++ class TrkInfo+;

// stl collections of utility classes
#pragma link C++ class std::vector<ClustInfo>+;
#pragma link C++ class std::vector<CstInfo>+;
#pragma link C++ class std::vector<FlowInfo>+;
#pragma link C++ class std::vector<JetInfo>+;
#pragma link C++ class std::vector<ParInfo>+;
#pragma link C++ class std::vector<TrkInfo>+;
#pragma link C++ class std::vector<vector<CstInfo> >+;
#pragma link C++ class std::vector<std::vector<ParInfo> >+;
#pragma link C++ class std::pair<ParInfo, ParInfo>+;

#endif

// end ------------------------------------------------------------------------

