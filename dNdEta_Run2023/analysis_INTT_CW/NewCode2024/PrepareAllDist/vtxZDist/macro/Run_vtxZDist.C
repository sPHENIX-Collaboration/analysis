#include "../vtxZDist.h"

R__LOAD_LIBRARY(../libvtxZDist.so)

void Run_vtxZDist(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "", // note : for data
  // std::string output_file_name_suffix = "_TrueXY", // note : for MC


  bool Apply_cut = false, // note: vtxZQA cut 
  bool ApplyVtxZReWeighting = false,
  std::pair<bool, int> ApplyEvtBcoFullDiffCut = {true, 61},

  bool IsVtxZQACutKnown = true
)
{

  vtxZDist * VZD1 = new vtxZDist(
    process_id,
    run_num,
    nevents,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,

    Apply_cut,
    ApplyVtxZReWeighting,
    ApplyEvtBcoFullDiffCut
  );

  string final_output_file_name1 = VZD1->GetOutputFileName();
  cout<<"final_output_file_name1: "<<final_output_file_name1<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name1.c_str(), output_directory.c_str(), final_output_file_name1.c_str()));  

  VZD1->PrepareEvent();
  VZD1->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name1.c_str(), output_directory.c_str()));

  if (IsVtxZQACutKnown && Apply_cut == false)
  {
    vtxZDist * VZD2 = new vtxZDist(
      process_id,
      run_num,
      nevents,
      input_directory,
      input_filename,
      output_directory,

      output_file_name_suffix,

      true, // note : vtxZ QA
      ApplyVtxZReWeighting,
      ApplyEvtBcoFullDiffCut
    );

    string final_output_file_name2 = VZD2->GetOutputFileName();
    cout<<"final_output_file_name2: "<<final_output_file_name2<<endl;

    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name2.c_str(), output_directory.c_str(), final_output_file_name2.c_str()));  

    VZD2->PrepareEvent();
    VZD2->EndRun();


    system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name2.c_str(), output_directory.c_str()));
  }

  return;
}
