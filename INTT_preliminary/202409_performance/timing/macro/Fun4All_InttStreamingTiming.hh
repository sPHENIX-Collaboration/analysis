#pragma once

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <intt/InttMapping.h>
#include <phool/recoConsts.h>
#include <G4Setup_sPHENIX.C>

// headers and library for analysis modules
#include "InttStreamingTiming.h"
R__LOAD_LIBRARY( libInttTiming.so )
