#include "../RestDist.h"

R__LOAD_LIBRARY(../libRestDist.so)

void Run_RestDist(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",
  std::pair<double, double> vertexXYIncm = {-0.0220728, 0.222956},

  bool Apply_cut = true, // note : vtxZQA
  bool ApplyVtxZReWeighting = false,
  std::pair<bool, int> ApplyEvtBcoFullDiffCut = {true, 61},
  std::pair<bool, std::pair<double,double>> RequireVtxZRange = {true, {-10, 10}},
  std::pair<bool, std::pair<double,double>> isClusQA = {true, {35, 40}}, // note : adc, phi size
  bool isRotated = true
)
{

  string final_output_file_name;
  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and no ClusQA, no inner rotated
  RestDist * RDs1 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,
    vertexXYIncm,

    Apply_cut,
    ApplyVtxZReWeighting,
    ApplyEvtBcoFullDiffCut,
    
    RequireVtxZRange,
    {false,{-10,20000}},
    false //note: isRotated
  );

  final_output_file_name = RDs1->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs1->PrepareEvent();
  RDs1->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and w/ ClusQA, no inner rotated
  RestDist * RDs2 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,
    vertexXYIncm,

    Apply_cut,
    ApplyVtxZReWeighting,
    ApplyEvtBcoFullDiffCut,
    
    RequireVtxZRange,
    isClusQA,
    false //note: isRotated
  );

  final_output_file_name = RDs2->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs2->PrepareEvent();
  RDs2->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));


  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : No vtxZ cut, No ClusQA, no inner rotated
  RestDist * RDs3 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,
    vertexXYIncm,

    Apply_cut,
    ApplyVtxZReWeighting,
    ApplyEvtBcoFullDiffCut,
    
    {false, {-1000,1000}},
    {false,{-10,20000}},
    false //note: isRotated
  );

  final_output_file_name = RDs3->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs3->PrepareEvent();
  RDs3->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and w/ ClusQA, with inner rotated
  RestDist * RDs4 = new RestDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,
    vertexXYIncm,

    Apply_cut,
    ApplyVtxZReWeighting,
    ApplyEvtBcoFullDiffCut,
    
    RequireVtxZRange,
    isClusQA,
    isRotated //note: isRotated
  );

  // RDs4 -> SetInnerBarrelRotation(25.); // todo: 

  final_output_file_name = RDs4->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs4->PrepareEvent();
  RDs4->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
