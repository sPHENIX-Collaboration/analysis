// $Id: $                                                                                             

/*!
 * \file Fun4All_EMCalLikelihood.C
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#include <cassert>

void
Fun4All_EMCalLikelihood(const int nEvents = 1000,
//Fun4All_EMCalLikelihood(const int nEvents = 100000000, // original
    TString base_dir =
        "../..//sPHENIX_work/production_analysis_cemc2x2/emcstudies/pidstudies/spacal2d/fieldmap/",
    TString pid = "e-", TString kine_config = "eta0_8GeV", TString ll_config =
        "Edep_Distribution_ll_sample")
{
  const TString inputFile = base_dir + "/G4Hits_sPHENIX_" + pid + "_"
      + kine_config + "-ALL.root_Ana.root.lst";
  //                "/phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_2GeV-ALL.root_Ana.root.lst")

  TString s_outputFile = inputFile;
  s_outputFile += "_EMCalLikelihood.root";
  const char * outputFile = s_outputFile.Data();

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4vertex.so");
  gSystem->Load("libemcal_ana.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
//    se->Verbosity(10);
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  rc->set_IntFlag("",0);

  double center_cemc_iphi = 1000;
  double center_cemc_ieta = 1000;
  double center_hcalin_iphi = 1000;
  double center_hcalin_ieta = 1000;

  double width = 0;
  double width_emcal_eta = 0;

  int charge = 0;

  if (pid == "e+" || pid == "pi+" || pid == "kaon+" || pid == "proton")
    charge = +1;
  else if (pid == "e-" || pid == "pi-" || pid == "kaon-"
      || pid == "anti_proton")
    charge = -1;
  if (charge == 0)
    {
      cout
          << "Fun4All_EMCalLikelihood - Fatal Error - invalid input particle type: "
          << pid << endl;
      assert(charge != 0);
    }

  if (base_dir.Contains("spacal") && kine_config.Contains("eta0_"))
    {
      ///////////////////////////////////////////////
      // Projection center based on /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
      ///////////////////////////////////////////////
      center_cemc_iphi = 0.0952638; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
      center_cemc_ieta = 0.00397331; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0_4GeV-ALL.root_Ana.root
      center_hcalin_iphi = 0.354109; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.000273002; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0_4GeV-ALL.root_Ana.root

//      width = 1.4;
//      width_emcal_eta = 1.4;
      width = 1.6;
      width_emcal_eta = 1.6;
    }
  else if (base_dir.Contains("spacal2d") && charge <0
      && kine_config.Contains("eta0.90_2GeV"))
    {

      center_cemc_iphi = 0.249273; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
      center_cemc_ieta = 0.0146006; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root

      center_hcalin_iphi = 0.447927; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.101503; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root

      width = 1.6;
      width_emcal_eta = 1.6;
    }
  else if (base_dir.Contains("spacal2d") && charge <0
      && kine_config.Contains("eta0.90_4GeV"))
    {
      center_cemc_iphi = 0.129035; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
      center_cemc_ieta = 0.0222246; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root

      center_hcalin_iphi = 0.385379; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.0596968; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root

      width = 1.6;
      width_emcal_eta = 1.6;
    }
  else if (base_dir.Contains("spacal2d") && charge <0
      && kine_config.Contains("eta0.90_8GeV"))
    {
      center_cemc_iphi = 0.0829824; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
      center_cemc_ieta = 0.0275653; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root

      center_hcalin_iphi = 0.348779; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.0348952; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal2d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_8GeV-ALL.root_Ana.root

      width = 1.6;
      width_emcal_eta = 1.6;
    }
  else if (base_dir.Contains("spacal1d") && charge <0
      && kine_config.Contains("eta0.90_2GeV"))
    {
      center_cemc_iphi = 0.53236; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
      center_cemc_ieta = 1.15917; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_2GeV-ALL.root_Ana.root
      center_hcalin_iphi = 0.445272; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.180098; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_2GeV-ALL.root_Ana.root

      width = 1.8;
      width_emcal_eta = 2.6;
    }
  else if (base_dir.Contains("spacal1d") && charge <0
      && kine_config.Contains("eta0.90_4GeV"))
    {
      center_cemc_iphi = 0.251162; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
      center_cemc_ieta = 1.35067; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_4GeV-ALL.root_Ana.root
      center_hcalin_iphi = 0.393433; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.116632; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_pi-_eta0.90_4GeV-ALL.root_Ana.root

      width = 1.8;
      width_emcal_eta = 2.6;
    }
  else if (base_dir.Contains("spacal1d") && charge <0
      && kine_config.Contains("eta0.90_8GeV"))
    {
      center_cemc_iphi = 0.122055; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
      center_cemc_ieta = 1.52588; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
      center_hcalin_iphi = 0.644177; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.799621; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0.90_8GeV-ALL.root_Ana.root

      width = 1.8;
      width_emcal_eta = 2.6;
    }
  else if (base_dir.Contains("spacal2d") && charge >0
      && kine_config.Contains("eta0.90_2GeV"))
    {
      center_cemc_iphi = -0.16237; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
      center_cemc_ieta = 0.00993428; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
      center_hcalin_iphi = 0.338006; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.0939571; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root

      width = 1.6;
      width_emcal_eta = 1.6;
    }
  else if (base_dir.Contains("spacal2d") && charge >0
      && kine_config.Contains("eta0.90_4GeV"))
    {
      center_cemc_iphi = -0.0455698; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
      center_cemc_ieta = 0.0181539; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
      center_hcalin_iphi = 0.340425; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.0670486; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root


      width = 1.6;
      width_emcal_eta = 1.6;
    }
  else if (base_dir.Contains("spacal2d") && charge >0
      && kine_config.Contains("eta0.90_8GeV"))
    {
      center_cemc_iphi = 0.00278605; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
      center_cemc_ieta = 0.0237387; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
      center_hcalin_iphi = 0.336144; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.0484305; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal2d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root


      width = 1.6;
      width_emcal_eta = 1.6;
    }
  else if (base_dir.Contains("spacal1d") && charge >0
      && kine_config.Contains("eta0.90_2GeV"))
    {
      center_cemc_iphi = -0.530751; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root
      center_cemc_ieta = 1.15259; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_2GeV-ALL.root_Ana.root

      center_hcalin_iphi = 0.317738; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.119789; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_2GeV-ALL.root_Ana.root

      width = 1.8;
      width_emcal_eta = 2.6;
    }
  else if (base_dir.Contains("spacal1d") && charge >0
      && kine_config.Contains("eta0.90_4GeV"))
    {
      center_cemc_iphi = -0.252243; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root
      center_cemc_ieta = 1.34503; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_4GeV-ALL.root_Ana.root

      center_hcalin_iphi = 0.334954; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root
       center_hcalin_ieta = -0.0992204; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_4GeV-ALL.root_Ana.root

      width = 1.8;
      width_emcal_eta = 2.6;
    }
  else if (base_dir.Contains("spacal1d") && charge >0
      && kine_config.Contains("eta0.90_8GeV"))
    {
      center_cemc_iphi = -0.122248; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root
      center_cemc_ieta = 1.52555; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_e+_eta0.90_8GeV-ALL.root_Ana.root

      center_hcalin_iphi = 0.33193; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root
      center_hcalin_ieta = -0.0749197; // /phenix/sim02/phnxreco/ePHENIX/jinhuang/sPHENIX_work/production_analysis/emcstudies/pidstudies/spacal1d/fieldmap/G4Hits_sPHENIX_pi+_eta0.90_8GeV-ALL.root_Ana.root

      width = 1.8;
      width_emcal_eta = 2.6;
    }
  else
    {
      cout << "Error !!!!!! Unknown configuraiton";

      return;
    }

  TH2F * h2_Edep_Distribution_e = NULL;
  TH2F * h2_Edep_Distribution_pi = NULL;

  // load edep ll input distribution
  if (charge <0)
    {
      TFile * f =
          new TFile(
              base_dir + "/G4Hits_sPHENIX_e-_" + kine_config
                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
                  + ll_config + ".root");

      if (f->IsOpen())
        {
          f->ls();
          h2_Edep_Distribution_e = (TH2F *) f->GetObjectChecked(
              "h2_Edep_Distribution", "TH2F");
        }

//      TFile * f =
//          new TFile(
//              base_dir + "/G4Hits_sPHENIX_pi-_" + kine_config
//                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
//                  + ll_config + ".root");
      TFile * f =
          new TFile(
              base_dir + "/G4Hits_sPHENIX_anti_proton_" + kine_config
                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
                  + ll_config + ".root");

      if (f->IsOpen())
        {
          f->ls();
          h2_Edep_Distribution_pi = (TH2F *) f->GetObjectChecked(
              "h2_Edep_Distribution", "TH2F");
        }
//      assert(h2_Edep_Distribution_e);
//      assert(h2_Edep_Distribution_pi);
    }

  else if (charge > 0)
    {
      TFile * f =
          new TFile(
              base_dir + "/G4Hits_sPHENIX_e+_" + kine_config
                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
                  + ll_config + ".root");

      if (f->IsOpen())
        {
          f->ls();
          h2_Edep_Distribution_e = (TH2F *) f->GetObjectChecked(
              "h2_Edep_Distribution", "TH2F");
        }

//      TFile * f =
//          new TFile(
//              base_dir + "/G4Hits_sPHENIX_pi-_" + kine_config
//                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
//                  + ll_config + ".root");
      TFile * f =
          new TFile(
              base_dir + "/G4Hits_sPHENIX_pi+_" + kine_config
                  + "-ALL.root_Ana.root.lst_EMCalLikelihood.root_DrawEcal_Likelihood_"
                  + ll_config + ".root");

      if (f->IsOpen())
        {
          f->ls();
          h2_Edep_Distribution_pi = (TH2F *) f->GetObjectChecked(
              "h2_Edep_Distribution", "TH2F");
        }
//      assert(h2_Edep_Distribution_e);
//      assert(h2_Edep_Distribution_pi);
    }

  EMCalLikelihood * emcal_ana = new EMCalLikelihood(
      string(inputFile.Data()) + string("_hist.root"));
  emcal_ana->Verbosity(5);

  emcal_ana->set_center_cemc_ieta(center_cemc_ieta);
  emcal_ana->set_center_cemc_iphi(center_cemc_iphi);
  emcal_ana->set_center_hcalin_ieta(center_hcalin_ieta);
  emcal_ana->set_center_hcalin_iphi(center_hcalin_iphi);

  // -------------------------------------------------------------------
  // No CEMC ganging readout
  // -------------------------------------------------------------------
//  emcal_ana->set_width_cemc_ieta(width_emcal_eta);
//  emcal_ana->set_width_cemc_iphi(width);
//  emcal_ana->set_width_hcalin_ieta(width);
//  emcal_ana->set_width_hcalin_iphi(width);

  // -------------------------------------------------------------------
  // 2x2 CEMC ganging readout, cluster size x (2x2)
  // -------------------------------------------------------------------
//  emcal_ana->do_ganging(2,2);
//  emcal_ana->set_width_cemc_ieta(width_emcal_eta*2);
//  emcal_ana->set_width_cemc_iphi(width*2);
//  emcal_ana->set_width_hcalin_ieta(width*2);
//  emcal_ana->set_width_hcalin_iphi(width*2);

  // -------------------------------------------------------------------
  // 2x2 CEMC ganging readout, cluster size x (1.4x1.4)
  // -------------------------------------------------------------------
//  emcal_ana->do_ganging(2,2);
//  emcal_ana->set_width_cemc_ieta(width_emcal_eta*1.4);
//  emcal_ana->set_width_cemc_iphi(width*1.4);
//  emcal_ana->set_width_hcalin_ieta(width*1.4);
//  emcal_ana->set_width_hcalin_iphi(width*1.4);

  // -------------------------------------------------------------------
  // 2x2 CEMC ganging readout, cluster size x (1.2x1.2)
  // -------------------------------------------------------------------
  emcal_ana->do_ganging(2,2);
  emcal_ana->set_width_cemc_ieta(width_emcal_eta*1.2);
  emcal_ana->set_width_cemc_iphi(width*1.2);
  emcal_ana->set_width_hcalin_ieta(width);
  emcal_ana->set_width_hcalin_iphi(width);

  if (h2_Edep_Distribution_e)
    emcal_ana->set_h2_Edep_Distribution_e(h2_Edep_Distribution_e);
  if (h2_Edep_Distribution_pi)
    emcal_ana->set_h2_Edep_Distribution_pi(h2_Edep_Distribution_pi);


  se->registerSubsystem(emcal_ana);

  //--------------
  // IO management
  //--------------

  // Hits file
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
//  hitsin->fileopen(inputFile);
  hitsin->AddListFile(inputFile.Data());
  se->registerInputManager(hitsin);

  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT",
      outputFile);
  out->AddNode("Sync");
  out->AddNode("UpsilonPair");
  out->AddNode("EMCalTrk");
  out->AddNode("GlobalVertexMap");
  se->registerOutputManager(out);

  gSystem->ListLibraries();

//  return;

  se->run(nEvents);
//  se->dumpHistos(string(inputFile) + string("_hist.root"), "recreate");

  //-----
  // Exit
  //-----
  gSystem->Exec("ps -o sid,ppid,pid,user,comm,vsize,rssize,time");

  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}

