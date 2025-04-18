#include "../ClusHistogram.h"

R__LOAD_LIBRARY(../libClusHistogram.so)

void Run_PrepareHist(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.0232717, 0.223173}, // note : in cm // note : data

  std::pair<bool, TH1D*> vtxZReweight = {false, nullptr},
  bool BcoFullDiffCut = true,
  bool INTT_vtxZ_QA = true,
  std::pair<bool, std::pair<double, double>> isClusQA = {true, {35, 500}}, // note : {adc, phi size}
  bool HaveGeoOffsetTag = false,
  std::pair<bool, int> SetRandomHits = {false, 0},
  bool RandInttZ = false
)
{

  ClusHistogram * CSH = new ClusHistogram(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,
    vertexXYIncm,

    vtxZReweight,
    BcoFullDiffCut,
    INTT_vtxZ_QA,
    isClusQA,
    HaveGeoOffsetTag,
    SetRandomHits,
    RandInttZ
  );

  string final_output_file_name = CSH->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  CSH -> MainProcess();
  CSH -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
