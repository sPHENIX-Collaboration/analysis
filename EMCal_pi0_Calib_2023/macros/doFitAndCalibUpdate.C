#ifndef FUN4ALL_YEAR1_C
#define FUN4ALL_YEAR1_C

#include <caloreco/CaloTowerCalib.h>
#include <caloreco/RawClusterBuilderTemplate.h>
#include <caloreco/RawClusterDeadHotMask.h>
#include <caloreco/RawClusterPositionCorrection.h>
#include <caloreco/TowerInfoDeadHotMask.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <globalvertex/GlobalVertexReco.h>
#include <mbd/MbdReco.h>

#include <phool/recoConsts.h>

#include <cdbobjects/CDBTTree.h>  // for CDBTTree
#include <ffamodules/CDBInterface.h>
#include <GlobalVariables.C>

R__LOAD_LIBRARY(libcdbobjects)

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libglobalvertex.so)

#include <caloana/CaloAna.h>
R__LOAD_LIBRARY(libcaloana.so)


void doFitAndCalibUpdate(const std::string &hist_fname = "parallel_test/combine_out/out1.root", const std::string &calib_fname = "parallel_test/local_cdb_copy.root" ,int iter = 3)
{


  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  CaloAna *caFit = new CaloAna("calomodulename", "bla.root");
  caFit->fitEtaSlices(hist_fname.c_str(), Form("fitout_iter%d.root", iter), calib_fname.c_str());

  size_t pos = calib_fname.find_last_of('.'); 
  string f_calib_save_name = calib_fname;
  f_calib_save_name.insert(pos,Form("_iter%d",iter));

  TFile* f_calib_mod = new TFile(calib_fname.c_str());
  f_calib_mod->Cp(f_calib_save_name.c_str());

  gSystem->Exit(0);
}


#endif
