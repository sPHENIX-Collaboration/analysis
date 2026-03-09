#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <GlobalVariables.C>
#include <G4_Global.C>

//#include <G4_Centrality.C>
//#include <G4_Input.C>
#include <Trkr_Clustering.C>
#include <Trkr_RecoInit.C>

#include <phool/recoConsts.h>
//#include <anatutorial/AnaTutorial.h>
//#include <anatutorial/InttAna.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)

#include <globalvertex/GlobalVertexReco.h>
#include <MbdReco.h>
R__LOAD_LIBRARY(libmbd.so)

#include <inttread/InttAna.h>
// #include <inttxyvertexfinder/InttZVertexFinder.h>
// #include <inttxyvertexfinder/InttXYVertexFinder.h>
#include <InttZVertexFinder.h>
#include <InttXYVertexFinder.h>

R__LOAD_LIBRARY(libinttread.so)
//R__LOAD_LIBRARY(libanatutorial.so)

#include "functions.hh"
