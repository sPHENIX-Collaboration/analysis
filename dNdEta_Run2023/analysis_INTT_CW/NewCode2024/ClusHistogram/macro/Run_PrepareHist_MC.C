#include "../ClusHistogram.h"

R__LOAD_LIBRARY(../libClusHistogram.so)

TH1D * GetReweighting_hist(string input_map_directory, string map_name)
{
  TFile * file_in = TFile::Open(Form("%s", input_map_directory.c_str()));
  TH1D * h1D_INTT_vtxZ_reweighting = (TH1D*)file_in->Get(map_name.c_str()); // todo : the map of the vtxZ reweighting
  return h1D_INTT_vtxZ_reweighting;
}

void Run_PrepareHist_MC(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.0214921, 0.223299}, // note : in cm // note : MC

  std::pair<bool, TH1D*> vtxZReweight = {true, nullptr},
  bool BcoFullDiffCut = false,
  bool INTT_vtxZ_QA = true,
  std::pair<bool, std::pair<double, double>> isClusQA = {true, {35, 500}}, // note : {adc, phi size}
  bool HaveGeoOffsetTag = false,
  std::pair<bool, int> SetRandomHits = {false, 0},
  bool RandInttZ = false,

  std::string vtxZReWeighting_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/completed/VtxZDist/completed/vtxZ_comp_withVtxZQA/INTTvtxZReWeight.root",
  std::string map_name = "HIJING_noZWeight_VtxZQA_Inclusive70"
)
{

  if (vtxZReweight.first) {vtxZReweight.second = (TH1D*)(GetReweighting_hist(vtxZReWeighting_input_directory, map_name))->Clone();}

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
    SetRandomHits
  );

  string final_output_file_name = CSH->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  CSH -> MainProcess();
  CSH -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
