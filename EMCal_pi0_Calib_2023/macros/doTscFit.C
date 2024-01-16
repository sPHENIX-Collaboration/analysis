#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
//#include <rawwaveformtowerbuilder/RawWaveformTowerBuilder.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllUtils.h>

#include <cdbobjects/CDBTTree.h>  // for CDBTTree
#include "TowerInfo.h"
#include "TowerInfoDefs.h"

#include <litecaloeval/LiteCaloEval.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/RawClusterBuilderTemplate.h>

#include <phool/recoConsts.h>

#include "/sphenix/u/bseidlitz/work/macros/calibrations/calo/calib_nov23/towerslope/TSCtoCDBTTree.C"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libLiteCaloEvalTowSlope.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcdbobjects)

void mergeCDBTTrees(const char * infile1, const char * infile2, const char * outputfile);

#endif

//void doTscFit(int nevents = 0, const char *fname = "input.list", const char * outfile =  "defout.root", const char * fitoutfile = "deffitout.root")
void doTscFit(const std::string &hist_fname = "parallel_test/combine_out/out1.root", const std::string &calib_fname = "parallel_test/local_cdb_copy.root")
{
  //Fun4AllServer *se = Fun4AllServer::instance();

  string fitoutfile = "tsc_fitout.root";

  LiteCaloEval modlce;
  modlce.CaloType(LiteCaloEval::CEMC);
  modlce.Get_Histos(hist_fname.c_str(),fitoutfile.c_str());
  modlce.m_myminbin =  8;  
  modlce.m_mymaxbin =  95 + 1 ; 
  modlce.setFitMin(0.12);
  modlce.setFitMax(0.7);
  modlce.FitRelativeShifts(&modlce,110);

  // create the cdbttree from tsc output andd multiply the corrections 
  // into the base calibration to pickup for pi0 first iteration
  TSCtoCDBTTree(fitoutfile.c_str(),"tsc_output_cdb.root");
  mergeCDBTTrees("tsc_output_cdb.root",calib_fname.c_str(),calib_fname.c_str());

  size_t pos = calib_fname.find_last_of('.');
  string f_calib_save_name = calib_fname;
  f_calib_save_name.insert(pos,"_postTSC");

  TFile* f_calib_mod = new TFile(calib_fname.c_str());
  f_calib_mod->Cp(f_calib_save_name.c_str());

  gSystem->Exit(0);
}


void mergeCDBTTrees(const char * infile1, const char * infile2, const char * outputfile)
{

  CDBTTree *cdbttree1 = new CDBTTree(infile1);  
  CDBTTree *cdbttree2 = new CDBTTree(infile2);  
  CDBTTree *cdbttreeOut = new CDBTTree(outputfile);

  string m_fieldname = "Femc_datadriven_qm1_correction";

  for(int i = 0; i < 96 ; i++)
  {
    for(int j = 0; j < 256; j++)
    {
      unsigned int key = TowerInfoDefs::encode_emcal(i,j);
      float val1 = cdbttree1->GetFloatValue(key, m_fieldname);
      float val2 = cdbttree2->GetFloatValue(key, m_fieldname);
      cdbttreeOut->SetFloatValue(key,m_fieldname,val1*val2);
    }
  }

  cdbttreeOut->Commit();
  cdbttreeOut->WriteCDBTTree();
  delete cdbttreeOut;
  delete cdbttree1;
  delete cdbttree2;

}//end macro

