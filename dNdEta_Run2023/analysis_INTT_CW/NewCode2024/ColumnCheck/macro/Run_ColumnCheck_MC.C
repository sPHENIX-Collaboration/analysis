#include "../ColumnCheck.h"

R__LOAD_LIBRARY(../libColumnCheck.so)

TH2D * GetGoodColMap (std::string ColMulMask_map_dir_in, std::string ColMulMask_map_file_in, std::string map_name_in)
{
  TFile * f = TFile::Open(Form("%s/%s", ColMulMask_map_dir_in.c_str(), ColMulMask_map_file_in.c_str()));
  TH2D * h = (TH2D*)f->Get(map_name_in.c_str());
  return h;
}

void Run_ColumnCheck_MC(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.0217356, 0.223402},
  double SetMbinFloat = 70, // note : 0 - 100
            
  std::pair<double, double> VtxZRange = {-30, 30},
  bool IsZClustering = false,
  bool BcoFullDiffCut = false,
  std::pair<bool, std::pair<double, double>> isClusQA = {true, {35,39}}, // note : {adc, phi size}

  bool ColMulMask = false,
  std::string ColMulMask_map_dir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run3/EvtVtxZ/ColumnCheck/completed/MulMap/completed",
  std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin50_VtxZ-30to30cm_ClusQAAdc35PhiSize500_00054280.root"
)
{

  ColumnCheck * CC = new ColumnCheck(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,
    vertexXYIncm,
    SetMbinFloat,

    VtxZRange,
    IsZClustering,
    BcoFullDiffCut,
    isClusQA, // note : {adc, phi size}

    ColMulMask
  );

  string final_output_file_name = CC->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  if (ColMulMask){
    CC -> SetGoodColMap(
      GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, CC -> GetGoodColMapName())
    );
  }

  CC -> MainProcess();
  CC -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
