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

#include "functions.hh"
#include "constant_values.hh"

#include <InttCalib.h>
R__LOAD_LIBRARY( libinttcalib.so )
