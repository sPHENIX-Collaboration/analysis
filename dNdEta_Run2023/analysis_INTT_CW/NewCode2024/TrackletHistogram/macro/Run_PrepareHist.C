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
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run5/EvtVtxZ/TrackHist",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.0220728, 0.222956},

  std::pair<bool, TH1D*> vtxZReweight = {false, nullptr},
  bool BcoFullDiffCut = true,
  bool INTT_vtxZ_QA = true,
  // std::pair<bool, std::pair<double, double>> isClusQA = {true, {35, 350}}, // note : {adc, phi size}

  bool HaveGeoOffsetTag = false,
  std::pair<bool, int> SetRandomHits = {false, 0},
  bool RandInttZ = false,
  bool ColMulMask = true,

  std::string ColMulMask_map_mother_dir = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana467_20250225/Run7/EvtVtxZ/ColumnCheck"

  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/ColumnCheck/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin50_VtxZ-30to30cm_ClusQAAdc35PhiSize500_00054280.root"

  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/ColumnCheck_NoClusQA/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin50_VtxZ-30to30cm_00054280.root"
  
  // std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/ColumnCheck_noPhiCut/completed/MulMap/completed",
  // std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize350_00054280.root"
)
{
  
  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : baseline
  std::string final_output_directory = output_directory + "/baseline";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  std::pair<bool, std::pair<double, double>> isClusQA = {true, {35,40}}; // note : {adc, phi size}

  std::string ColMulMask_map_dir = ColMulMask_map_mother_dir + "/baseline/completed/MulMap/completed";
  std::string ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280.root", isClusQA.second.first, isClusQA.second.second);

  TrackletHistogramNew * TLHN = new TrackletHistogramNew(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

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
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str(), final_output_file_name.c_str()));  

  TLHN -> MainProcess();
  TLHN -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str()));

  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : noAdcCut
  final_output_directory = output_directory + "/noAdcCut";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  isClusQA = {true, {0,40}}; // note : {adc, phi size}

  ColMulMask_map_dir = ColMulMask_map_mother_dir + "/noAdcCut/completed/MulMap/completed";
  ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280.root", isClusQA.second.first, isClusQA.second.second);

  TrackletHistogramNew * TLHN1 = new TrackletHistogramNew(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

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
    TLHN1 -> SetGoodColMap(
      GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN1->GetGoodColMapName())
    );
  }

  string final_output_file_name1 = TLHN1->GetOutputFileName();
  cout<<"final_output_file_name1: "<<final_output_file_name1<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name1.c_str(), final_output_directory.c_str(), final_output_file_name1.c_str()));  

  TLHN1 -> MainProcess();
  TLHN1 -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name1.c_str(), final_output_directory.c_str()));

  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : 50AdcCut
  final_output_directory = output_directory + "/50AdcCut";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  isClusQA = {true, {50,40}}; // note : {adc, phi size}

  ColMulMask_map_dir = ColMulMask_map_mother_dir + "/50AdcCut/completed/MulMap/completed";
  ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280.root", isClusQA.second.first, isClusQA.second.second);

  TrackletHistogramNew * TLHN2 = new TrackletHistogramNew(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

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
    TLHN2 -> SetGoodColMap(
      GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN2->GetGoodColMapName())
    );
  }

  string final_output_file_name2 = TLHN2->GetOutputFileName();
  cout<<"final_output_file_name2: "<<final_output_file_name2<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name2.c_str(), final_output_directory.c_str(), final_output_file_name2.c_str()));  

  TLHN2 -> MainProcess();
  TLHN2 -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name2.c_str(), final_output_directory.c_str()));

  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : noPhiCut
  final_output_directory = output_directory + "/noPhiCut";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  isClusQA = {true, {35,350}}; // note : {adc, phi size}

  ColMulMask_map_dir = ColMulMask_map_mother_dir + "/noPhiCut/completed/MulMap/completed";
  ColMulMask_map_file = Form("MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc%.0fPhiSize%.0f_00054280.root", isClusQA.second.first, isClusQA.second.second);

  TrackletHistogramNew * TLHN3 = new TrackletHistogramNew(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

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
    TLHN3 -> SetGoodColMap(
      GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, TLHN3->GetGoodColMapName())
    );
  }

  string final_output_file_name3 = TLHN3->GetOutputFileName();
  cout<<"final_output_file_name3: "<<final_output_file_name3<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name3.c_str(), final_output_directory.c_str(), final_output_file_name3.c_str()));  

  TLHN3 -> MainProcess();
  TLHN3 -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name3.c_str(), final_output_directory.c_str()));

  return;
}
