#include "../TrackletHistogramNew.h"

R__LOAD_LIBRARY(../libTrackletHistogramNew.so)

TH2D * GetGoodColMap (std::string ColMulMask_map_dir_in, std::string ColMulMask_map_file_in, std::string map_name_in)
{
  TFile * f = TFile::Open(Form("%s/%s", ColMulMask_map_dir_in.c_str(), ColMulMask_map_file_in.c_str()));
  TH2D * h = (TH2D*)f->Get(map_name_in.c_str());
  return h;
}

void Run_PrepareHist(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.0230601, 0.223231},

  std::pair<bool, TH1D*> vtxZReweight = {false, nullptr},
  bool BcoFullDiffCut = true,
  bool INTT_vtxZ_QA = true,
  std::pair<bool, std::pair<double, double>> isClusQA = {true, {35, 350}}, // note : {adc, phi size}

  bool HaveGeoOffsetTag = false,
  std::pair<bool, int> SetRandomHits = {false, 0},
  bool RandInttZ = false,
  bool ColMulMask = true,

  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/ColumnCheck/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin50_VtxZ-30to30cm_ClusQAAdc35PhiSize500_00054280.root"

  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/ColumnCheck_NoClusQA/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin50_VtxZ-30to30cm_00054280.root"

  std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/ColumnCheck_noPhiCut/completed/MulMap/completed",
  std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize350_00054280.root"
)
{

  TrackletHistogramNew * TLHN = new TrackletHistogramNew(
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
    isClusQA, // note : {adc, phi size}
    HaveGeoOffsetTag,
    SetRandomHits,
    RandInttZ,
    ColMulMask
  );

  if (ColMulMask){
    TLHN -> SetGoodColMap(
      GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN->GetGoodColMapName())
    );
  }

  string final_output_file_name = TLHN->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  TLHN -> MainProcess();
  TLHN -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
