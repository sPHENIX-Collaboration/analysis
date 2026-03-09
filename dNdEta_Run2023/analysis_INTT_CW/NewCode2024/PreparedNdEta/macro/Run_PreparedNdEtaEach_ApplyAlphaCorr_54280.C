#include "../PreparedNdEtaEach.h"

R__LOAD_LIBRARY(../libPreparedNdEtaEach.so)

std::map<std::string, TH1D*> GetAlphaCorrectionH1DMap(std::string alpha_correction_input_directory, std::vector<std::string> map_name_in){
  
  TFile * file_in = TFile::Open(alpha_correction_input_directory.c_str());
  std::map<std::string, TH1D*> h1D_alpha_correction_map; h1D_alpha_correction_map.clear();
  
  
  for (TObject* keyAsObj : *file_in->GetListOfKeys()){
    auto key = dynamic_cast<TKey*>(keyAsObj);
    std::string hist_name  = key->GetName();
    std::string class_name = key->GetClassName();

    if (class_name != "TH1D") {continue;}
    if (std::find(map_name_in.begin(), map_name_in.end(), hist_name) == map_name_in.end()) {continue;}

    h1D_alpha_correction_map.insert(
      std::make_pair(
        hist_name,
        (TH1D*)file_in->Get(hist_name.c_str())
      )
    );
  }

  for (auto &pair : h1D_alpha_correction_map){
    // pair.second->SetDirectory(0);
    std::cout << "alpha correction name : " << pair.first << std::endl;
  }

  return h1D_alpha_correction_map;
}

void Run_PreparedNdEtaEach_ApplyAlphaCorr_54280(
  int process_id = 0,
  int run_num = 54280,
  string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run24NewCode_TrackHist/completed",
  string input_filename = "Data_TrackletHistogram_BcoFullDiffCut_INTT_vtxZ_QA_SecondRun_00054280_merged.root",
  string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run24NewCode_TrackHist/completed/Run24NewCode_dNdEta",
  
  // todo : modify here
  std::string output_file_name_suffix = "_SecondRun", 

  bool ApplyAlphaCorr = true,
  bool isTypeA = false,
  std::pair<double,double> cut_INTTvtxZ = {-10, 10},
  int SelectedMbin = 70,
  std::pair<bool, std::pair<double,double>> setEtaRange = {true, {-1.9, 1.9}},

  string alpha_correction_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_dNdEta/completed/MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin70_SecondRun_00001_dNdEta.root"
)
{

  PreparedNdEtaEach * PNEE = new PreparedNdEtaEach(
    process_id,
    run_num,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,

    ApplyAlphaCorr,
    isTypeA,
    cut_INTTvtxZ,
    SelectedMbin
  );

  PNEE -> SetAlphaCorrectionH1DMap(
    GetAlphaCorrectionH1DMap(
      alpha_correction_input_directory,
      PNEE->GetAlphaCorrectionNameMap()
    )
  );

  if (setEtaRange.first) {PNEE -> SetSelectedEtaRange(setEtaRange.second);}

  std::vector<std::string> final_output_file_name = PNEE->GetOutputFileName();
  for (auto filename : final_output_file_name){
    cout<<"final_output_file_name: "<<filename<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), filename.c_str(), output_directory.c_str(), filename.c_str()));  
  }
  

  PNEE -> PrepareStacks();
  std::cout<<111<<endl;
  PNEE -> DoFittings();
  std::cout<<222<<endl;
  PNEE -> PrepareMultiplicity();
  std::cout<<333<<endl;
  PNEE -> PreparedNdEtaHist();
  std::cout<<444<<endl;
  PNEE -> DeriveAlphaCorrection();
  std::cout<<555<<endl;
  PNEE -> EndRun();


  for (auto filename : final_output_file_name){
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), filename.c_str(), output_directory.c_str()));
  }

  return;
}
