#include "../ColumnCheck.h"

R__LOAD_LIBRARY(../libColumnCheck.so)

void Run_ColumnCheck(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run5/EvtVtxZ/ColumnCheck",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  double SetMbinFloat = 70, // note : 0 - 100
            
  std::pair<double, double> VtxZRange = {-30, 30}, // note: MBD vtx Z
  bool IsZClustering = false,
  // std::pair<bool, std::pair<double, double>> isClusQA = {true, {35,40}}, // note : {adc, phi size}
 
  bool ColMulMask = false
)
{
  std::pair<double, double> vertexXYIncm_data = {-0.0220728, 0.222956};

  // std::pair<double, double> vertexXYIncm_MC = {-0.0218978, 0.223183}; // note : HIJING
  // std::pair<double, double> vertexXYIncm_MC = {-0.0216964, 0.223331}; // note : HIJING + strangeness increase
  std::pair<double, double> vertexXYIncm_MC = {-0.0218667, 0.223296}; // note : AMPT
  // std::pair<double, double> vertexXYIncm_MC = {-0.021907, 0.223293}; // note : EPOS

  std::pair<double, double> vertexXYIncm = (run_num == -1) ? vertexXYIncm_MC : vertexXYIncm_data;
  bool BcoFullDiffCut = (run_num == -1) ? false : true;

  std::cout<<"vertexXYIncm: "<<vertexXYIncm.first<<" "<<vertexXYIncm.second<<std::endl;

  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : baseline
  std::string final_output_directory = output_directory + "/baseline";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  std::pair<bool, std::pair<double, double>> isClusQA = {true, {35,40}}; // note : {adc, phi size}

  ColumnCheck * CC = new ColumnCheck(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

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
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str(), final_output_file_name.c_str()));  

  CC -> MainProcess();
  CC -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name.c_str(), final_output_directory.c_str()));


  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : noAdcCut
  final_output_directory = output_directory + "/noAdcCut";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  isClusQA = {true, {0,40}}; // note : {adc, phi size}

  ColumnCheck * CC1 = new ColumnCheck(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

    output_file_name_suffix,
    vertexXYIncm,
    SetMbinFloat,

    VtxZRange,
    IsZClustering,
    BcoFullDiffCut,
    isClusQA, // note : {adc, phi size}

    ColMulMask
  );

  string final_output_file_name1 = CC1->GetOutputFileName();
  cout<<"final_output_file_name1: "<<final_output_file_name1<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name1.c_str(), final_output_directory.c_str(), final_output_file_name1.c_str()));  

  CC1 -> MainProcess();
  CC1 -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name1.c_str(), final_output_directory.c_str()));


  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : 50AdcCut
  final_output_directory = output_directory + "/50AdcCut";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  isClusQA = {true, {50,40}}; // note : {adc, phi size}

  ColumnCheck * CC2 = new ColumnCheck(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

    output_file_name_suffix,
    vertexXYIncm,
    SetMbinFloat,

    VtxZRange,
    IsZClustering,
    BcoFullDiffCut,
    isClusQA, // note : {adc, phi size}

    ColMulMask
  );

  string final_output_file_name2 = CC2->GetOutputFileName();
  cout<<"final_output_file_name2: "<<final_output_file_name2<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name2.c_str(), final_output_directory.c_str(), final_output_file_name2.c_str()));  

  CC2 -> MainProcess();
  CC2 -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name2.c_str(), final_output_directory.c_str()));
  

  // Division : -------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : noPhiCut
  final_output_directory = output_directory + "/noPhiCut";
  system(Form("mkdir -p %s/completed", final_output_directory.c_str()));

  isClusQA = {true, {35,350}}; // note : {adc, phi size}

  ColumnCheck * CC3 = new ColumnCheck(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    final_output_directory,

    output_file_name_suffix,
    vertexXYIncm,
    SetMbinFloat,

    VtxZRange,
    IsZClustering,
    BcoFullDiffCut,
    isClusQA, // note : {adc, phi size}

    ColMulMask
  );

  string final_output_file_name3 = CC3->GetOutputFileName();
  cout<<"final_output_file_name3: "<<final_output_file_name3<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", final_output_directory.c_str(), final_output_file_name3.c_str(), final_output_directory.c_str(), final_output_file_name3.c_str()));  

  CC3 -> MainProcess();
  CC3 -> EndRun();

  system(Form("mv %s/%s %s/completed", final_output_directory.c_str(), final_output_file_name3.c_str(), final_output_directory.c_str()));

  return;
}
