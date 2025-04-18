#include "../RestDist.h"

R__LOAD_LIBRARY(../libRestDist.so)

TH1D * GetReweighting_hist(string input_map_directory, string map_name)
{
  TFile * file_in = TFile::Open(Form("%s", input_map_directory.c_str()));
  TH1D * h1D_INTT_vtxZ_reweighting = (TH1D*)file_in->Get(map_name.c_str()); // todo : the map of the vtxZ reweighting
  return h1D_INTT_vtxZ_reweighting;
}

void Run_RestDist_MC(
  int process_id = 0,
  int run_num = 54280,
  int nevents = -1,
  string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro",
  string input_filename = "file_list_54280_intt.txt",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
  
  // todo : modify here
  std::string output_file_name_suffix = "",

  std::pair<double, double> vertexXYIncm = {-0.0218978, 0.223183}, // note : HIJING
  // std::pair<double, double> vertexXYIncm = {}, // note : HIJING + strangeness increase
  // std::pair<double, double> vertexXYIncm = {}, // note : AMPT
  // std::pair<double, double> vertexXYIncm = {}, // note : EPOS

  bool Apply_cut = true, // note : vtxZQA
  bool ApplyVtxZReWeighting = true,
  std::pair<bool, int> ApplyEvtBcoFullDiffCut = {false, 61},
  std::pair<bool, std::pair<double,double>> RequireVtxZRange = {true, {-10, 10}},
  std::pair<bool, std::pair<double,double>> isClusQA = {true, {35, 40}}, // note : adc, phi size
  bool isRotated = true,

  std::string vtxZReWeighting_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/completed/VtxZDist/completed/vtxZ_comp_WithVtxZQA_001/INTTvtxZReWeight.root",
  std::string map_name = "HIJING_noZWeight_WithVtxZQA_Inclusive70"
)
{

  string final_output_file_name;

  TH1D * h1D_INTT_vtxZ_reweighting = GetReweighting_hist(vtxZReWeighting_input_directory, map_name);

  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and no ClusQA, no rotated
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

    false // note : isRotated
  );
  RDs1->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);

  final_output_file_name = RDs1->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs1->PrepareEvent();
  RDs1->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and w/ ClusQA, no rotated
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

    false // note : isRotated
  );
  RDs2->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);

  final_output_file_name = RDs2->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs2->PrepareEvent();
  RDs2->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));


  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // note : No vtxZ cut, No ClusQA, no rotated
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

    false // note : isRotated
  );
  RDs3->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);

  final_output_file_name = RDs3->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs3->PrepareEvent();
  RDs3->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));


  // Division : ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  // note : with vtxZ range cut, and w/ ClusQA, with rotate
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

    isRotated // note : isRotated
  );
  RDs4->SetINTTvtxZReweighting(h1D_INTT_vtxZ_reweighting);

  final_output_file_name = RDs4->GetOutputFileName();
  cout<<"final_output_file_name: "<<final_output_file_name<<endl;

  system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str(), final_output_file_name.c_str()));  

  RDs4->PrepareEvent();
  RDs4->EndRun();


  system(Form("mv %s/%s %s/completed", output_directory.c_str(), final_output_file_name.c_str(), output_directory.c_str()));

  return;
}
