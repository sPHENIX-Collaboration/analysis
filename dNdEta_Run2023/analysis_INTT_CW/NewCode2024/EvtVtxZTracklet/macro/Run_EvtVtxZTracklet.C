#include "../EvtVtxZProtoTracklet.h"

R__LOAD_LIBRARY(../libEvtVtxZProtoTracklet.so)

void Run_EvtVtxZTracklet(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  
  bool IsFieldOn = false,
  bool IsDCACutApplied = 1,
  std::pair<std::pair<double,double>,std::pair<double,double>> DeltaPhiCutInDegree = {{-0.6, 0.6},{-1000.,1000.}}, // note : in degree
  std::pair<std::pair<double,double>,std::pair<double,double>> DCAcutIncm = {{-0.1, 0.1},{-1000.,1000.}}, // note : in cm
  int ClusAdcCut = 35,
  int ClusPhiSizeCut = 39,
  
  bool PrintRecoDetails = 1,
  bool DrawEvtVtxZ = 1,

  bool RunInttBcoFullDiff = true,
  bool RunVtxZReco = 1,
  bool RunTrackletPair = false,
  bool RunTrackletPairRotate = false,
  
  bool HaveGeoOffsetTag = false
)
{

  std::pair<double, double> vertexXYIncm_MC = {-0.0217356, 0.223402}; // note : in cm // note : MC
  std::pair<double, double> vertexXYIncm_data = {-0.0230601, 0.223231}; // note : in cm // note : data

  std::pair<double, double> vertexXYIncm = (run_num == -1) ? vertexXYIncm_MC : vertexXYIncm_data;

  std::cout<<"RunNumber: "<<run_num<<std::endl;
  std::cout<<"ProcessID: "<<process_id<<std::endl;
  std::cout<<"InputVtxXY: "<<vertexXYIncm.first<<", "<<vertexXYIncm.second<<std::endl;

  EvtVtxZProtoTracklet * evzpt = new EvtVtxZProtoTracklet(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,
    output_file_name_suffix,

    vertexXYIncm,
    IsFieldOn,
    IsDCACutApplied,
    DeltaPhiCutInDegree,
    DCAcutIncm,
    ClusAdcCut,
    ClusPhiSizeCut,
      
    PrintRecoDetails,
    DrawEvtVtxZ,

    RunInttBcoFullDiff,
    RunVtxZReco,
    RunTrackletPair,
    RunTrackletPairRotate,
      
    HaveGeoOffsetTag
  );

  string final_output_file_name = evzpt->GetOutputFileName();
  string plot_final_output_file_name = "Plot_"+final_output_file_name;
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  
  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), plot_final_output_file_name.c_str(), output_directory.c_str(), plot_final_output_file_name.c_str()));  

  evzpt -> MainProcess();
  evzpt -> EndRun();

  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));
  system(Form("mv %s/%s %s/completed", output_directory.c_str(), plot_final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
