// -- c++ includes --
#include <string>
#include <iostream>

// -- root includes --
#include <TSystem.h>
#include <TFile.h>

using std::string;
using std::cout;
using std::endl;

#include <calib_emc_pi0/pi0EtaByEta.h>
R__LOAD_LIBRARY(libcalibCaloEmc_pi0.so)

void doFitAndCalibUpdate(const std::string &hist_fname = "base/combine_out/out2.root", const std::string &calib_fname = "base/local_calib_copy.root" ,int iter = 4, const string &fieldname = "CEMC_calib_ADC_to_ETower")
{

  string m_fieldname = fieldname;

  pi0EtaByEta *caFit = new pi0EtaByEta("calomodulename", "bla.root");
  caFit->set_massTargetHistFile("/sphenix/user/egm2153/calib_study/emcal_calib_year1/hijing_run14_fb_wupdatedsmear/hijing_run14_uncorr_cls_mass_towerThreshold070MeV.root"); // HIJING RUN14 w/ updated smearing pi0 mass w/ 70MeV cluster tower threshold
  //caFit->set_massTargetHistFile("/sphenix/user/egm2153/calib_study/emcal_calib_year1/hijing_run14_fixed_build/hijing_run14_uncorr_cls_mass_towerThreshold070MeV.root"); // HIJING RUN14 pi0 mass w/ 70MeV cluster tower threshold
  //caFit->set_massTargetHistFile("/sphenix/u/bseidlitz/work/macros/calibrations/calo/emcal_calib_year1/run10_uncorr_cls_mass.root");// HIJING pi0 mass  w/ 30MeV 
  //caFit->set_massTargetHistFile("/sphenix/u/bseidlitz/work/macros/calibrations/calo/emcal_calib_year1/run10_uncorr_cls_mass_towerThreshold0p06.root");// HIJING pi0 mass 
  //caFit->set_scaleAdjFac(152.0/149.0); // effect of worse resolution in data
  caFit->set_calib_fieldname(m_fieldname);
  caFit->fitEtaSlices(hist_fname.c_str(), Form("fitout_iter%d.root", iter), calib_fname.c_str());

  size_t pos = calib_fname.find_last_of('.'); 
  string f_calib_save_name = calib_fname;
  f_calib_save_name.insert(pos,Form("_iter%d",iter));

  TFile* f_calib_mod = new TFile(calib_fname.c_str());
  f_calib_mod->Cp(f_calib_save_name.c_str());

  gSystem->Exit(0);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 3 || argc > 5){
        cout << "usage: ./" << argv[0] << " hist_fname calib_fname [iter] [m_fieldname]" << endl;
        return 1;
    }

    Int_t iter = 4;
    string m_fieldname = "CEMC_calib_ADC_to_ETower";

    if(argc >= 4) {
        iter = std::atoi(argv[3]);
    }
    if(argc >= 5) {
        m_fieldname = argv[4];
    }

    doFitAndCalibUpdate(argv[1], argv[2], iter, m_fieldname);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
