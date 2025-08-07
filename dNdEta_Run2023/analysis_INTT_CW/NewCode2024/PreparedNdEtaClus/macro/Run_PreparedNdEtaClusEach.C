#include "../PreparedNdEtaClusEach.h"

R__LOAD_LIBRARY(../libPreparedNdEtaClusEach.so)

void Run_PreparedNdEtaClusEach(
  int process_id = 1,
  int run_num = -1,
  string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_ClusHist_noClusQA/completed",
  string input_filename = "MC_ClusHist_vtxZReweight_VtxZQA_SecondRun_merged_001.root",
  string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_ClusHist_noClusQA/completed/dNdEtaClus",
  
  // todo : modify here
  std::string output_file_name_suffix = "_SecondRun",

  bool ApplyAlphaCorr = false,
  bool isTypeA = true,
  std::pair<double,double> cut_INTTvtxZ = {-5, 5},
  int SelectedMbin = 70
)
{

  PreparedNdEtaClusEach * PNEE = new PreparedNdEtaClusEach(
    process_id,
    run_num,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,

    ApplyAlphaCorr,
    isTypeA,
    cut_INTTvtxZ,
    SelectedMbin
  );
  

  std::string final_output_file_name = PNEE->GetOutputFileName();
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  
  

  PNEE -> PrepareStacks();
  std::cout<<111<<endl;
  PNEE -> PreparedNdEtaHist();
  std::cout<<222<<endl;
  PNEE -> DeriveAlphaCorrection();
  std::cout<<333<<endl;
  PNEE -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
