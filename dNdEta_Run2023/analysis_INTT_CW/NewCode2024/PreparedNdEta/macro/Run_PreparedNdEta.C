#include "../PreparedNdEta.h"

R__LOAD_LIBRARY(../libPreparedNdEta.so)

void Run_PreparedNdEta(
  int process_id = 1,
  int run_num = -1,
  string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_TrackHist/completed",
  string input_filename = "MC_TrackletHistogram_INTT_vtxZ_QA_test20241212_merged_half_001.root",
  string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_dNdEta",
  
  // todo : modify here
  std::string output_file_name_suffix = "_test20241217WideEtaBin",

  bool ApplyAlphaCorr = false
)
{

  PreparedNdEta * PNE = new PreparedNdEta(
    process_id,
    run_num,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,

    ApplyAlphaCorr
  );
  

  string final_output_file_name = PNE->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  PNE -> PrepareStacks();
  std::cout<<111<<endl;
  PNE -> DoFittings();
  std::cout<<222<<endl;
  PNE -> PrepareMultiplicity();
  std::cout<<333<<endl;
  PNE -> PreparedNdEtaHist();
  std::cout<<444<<endl;
  PNE -> DeriveAlphaCorrection();
  std::cout<<555<<endl;
  PNE -> EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
