#include "../PreparedNdEtaEach.h"

R__LOAD_LIBRARY(../libPreparedNdEtaEach.so)

void Run_PreparedNdEtaEach(
  int process_id = 1,
  int run_num = -1,
  string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/TrackHist_ForGeoOffset/completed",
  string input_filename = "MC_TrackHist_VtxZQA_ClusQAAdc35PhiSize500_ForGeoOffset_merged.root",
  string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run3/TrackHist_ForGeoOffset/completed/dNdEta",
  
  // todo : modify here
  std::string output_file_name_suffix = "_ForGeoOffset",

  bool ApplyAlphaCorr = false,
  bool isTypeA = false,
  std::pair<double,double> cut_INTTvtxZ = {-10, 10},
  int SelectedMbin = 70
)
{

  PreparedNdEtaEach * PNEE = new PreparedNdEtaEach(
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
  

  std::vector<std::string> final_output_file_name = PNEE->GetOutputFileName();
  for (auto filename : final_output_file_name){
    cout<<"final_output_file_name: "<<filename<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), filename.c_str(), output_directory.c_str(), filename.c_str()));  
  }
  

  PNEE -> PrepareStacks();
  std::cout<<111<<endl;
  PNEE -> DoFittings();
  std::cout<<222<<endl;
  PNEE -> PrepareMultiplicity();
  std::cout<<333<<endl;
  PNEE -> PreparedNdEtaHist();
  std::cout<<444<<endl;
  PNEE -> DeriveAlphaCorrection();
  std::cout<<555<<endl;
  PNEE -> EndRun();


  for (auto filename : final_output_file_name){
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), filename.c_str(), output_directory.c_str()));
  }

  return;
}
