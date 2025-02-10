#include "../SemiChipClustering.h"

R__LOAD_LIBRARY(../libSemiChipClustering.so)

void Run_SemiChipClustering(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",

  bool BcoFullDiffCut = true,
  bool INTT_vtxZ_QA = true,

  std::pair<bool, std::string> BadChMask = {true, "/cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/INTT_HotMap/ad/cf/adcf08dc730a3fac2e2db7a4d66370f3_hotmap_cdb_54280_100000_DST_1114.root"},
  bool ApplyHitQA = true,
  bool clone_hit_remove_BCO_tag = true,
  std::pair<bool, int> cut_HitBcoDiff = {true, 55},
  std::vector<int> adc_conversion_vec = {35, 45, 60, 90, 120, 150, 180, 210}
)
{

  SemiChipClustering * SCC = new SemiChipClustering(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,

    BcoFullDiffCut,
    INTT_vtxZ_QA,
    
    BadChMask,
    ApplyHitQA,
    clone_hit_remove_BCO_tag,
    cut_HitBcoDiff,
    adc_conversion_vec
  );

  string final_output_file_name = SCC->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  SCC -> MainProcess();
  SCC -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
