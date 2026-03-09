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

string Run_PreparedNdEtaEach(
  int process_id = 1,
  int run_num = -1,
  string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_TrackHist/completed",
  string input_filename = "MC_TrackletHistogram_vtxZReweight_INTT_vtxZ_QA_SecondRun_merged_001.root",
  string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_dNdEta",
  
  // todo : modify here
  std::string output_file_name_suffix = "_SecondRun",

  bool ApplyAlphaCorr = false,
  bool isTypeA = false,
  std::pair<double,double> cut_INTTvtxZ = {-10, 10},
  int SelectedMbin = 70
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

    string dNdEta_file_out;

  for (auto filename : final_output_file_name){
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), filename.c_str(), output_directory.c_str()));

    if (filename.find("_dNdEta.root") != std::string::npos){
      dNdEta_file_out = output_directory + "/completed/" + filename;
    }
  }

  std::cout<<"dNdEta_file_out: "<<dNdEta_file_out<<std::endl;

  return dNdEta_file_out;
}


string Run_PreparedNdEtaEach_ApplyAlphaCorr(
  int process_id = 2,
  int run_num = -1,
  string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_TrackHist/completed",
  string input_filename = "MC_TrackletHistogram_vtxZReweight_INTT_vtxZ_QA_SecondRun_merged_002.root",
  string output_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_dNdEta",
  
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

    string dNdEta_file_out;

  for (auto filename : final_output_file_name){
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), filename.c_str(), output_directory.c_str()));

    if (filename.find("_dNdEta.root") != std::string::npos){
      dNdEta_file_out = output_directory + "/completed/" + filename;
    }
  }

  std::cout<<"dNdEta_file_out: "<<dNdEta_file_out<<std::endl;

  return dNdEta_file_out;
}



string Run_PreparedNdEtaEach_ApplyAlphaCorr_54280(
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


  string dNdEta_file_out;

  for (auto filename : final_output_file_name){
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), filename.c_str(), output_directory.c_str()));

    if (filename.find("_dNdEta.root") != std::string::npos){
      dNdEta_file_out = output_directory + "/completed/" + filename;
    }
  }

  std::cout<<"dNdEta_file_out: "<<dNdEta_file_out<<std::endl;

  return dNdEta_file_out;
}

int DataMcComp(string data_directory_in, string MC_directory_in, string output_directory_in, string output_filename_in)
{
    TFile * file_in_data = TFile::Open(data_directory_in.c_str());
    TFile * file_in_mc = TFile::Open(MC_directory_in.c_str());

    TH1D * data_h1D_BestPair_RecoTrackletEtaPerEvt = (TH1D*)file_in_data->Get("h1D_BestPair_RecoTrackletEtaPerEvt");
    TH1D * data_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_data->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_data->Get("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    TH1D * data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_data->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    data_h1D_BestPair_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    data_h1D_BestPair_RecoTrackletEtaPerEvt -> SetLineColor(1);

    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(1);

    data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);

    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);



    TH1D * MC_h1D_BestPair_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc->Get("h1D_BestPair_RecoTrackletEtaPerEvt");
    TH1D * MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc->Get("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    TH1D * MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    MC_h1D_BestPair_RecoTrackletEtaPerEvt -> SetMarkerColor(2); // note : red
    MC_h1D_BestPair_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(2);
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    

    TH1D * h1D_TruedNdEta = (TH1D*)file_in_mc->Get("h1D_TruedNdEta");
    h1D_TruedNdEta -> SetLineColor(3); // note : green 


    TFile * file_out = new TFile(Form("%s/%s",output_directory_in.c_str(), output_filename_in.c_str()), "RECREATE");
    TCanvas * c1 = new TCanvas("c1", "c1", 800, 600);

    c1 -> cd();
    data_h1D_BestPair_RecoTrackletEtaPerEvt -> Draw("ep");
    MC_h1D_BestPair_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_BestPair_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("ep");
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> Draw("ep");
    MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> Draw("hist same");
    h1D_TruedNdEta -> SetFillColorAlpha(2,0);
    h1D_TruedNdEta -> Draw("hist same");
    c1 -> Write("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> Draw("ep");
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> Draw("hist same");
    h1D_TruedNdEta -> Draw("hist same");
    
    c1 -> Write("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");
    c1 -> Clear();

    file_out -> Close();

    

    return 0;
}

int McMcComp(string MC1_directory_in, string MC2_directory_in, string output_directory_in, string output_filename_in)
{
    TFile * file_in_data = TFile::Open(MC1_directory_in.c_str());
    TFile * file_in_mc = TFile::Open(MC2_directory_in.c_str());

    TH1D * data_h1D_BestPair_RecoTrackletEtaPerEvt = (TH1D*)file_in_data->Get("h1D_BestPair_RecoTrackletEtaPerEvt");
    TH1D * data_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_data->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_data->Get("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    TH1D * data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_data->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    data_h1D_BestPair_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    data_h1D_BestPair_RecoTrackletEtaPerEvt -> SetLineColor(1);

    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(1);

    data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);

    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);

    TH1D * data_h1D_TruedNdEta = (TH1D*)file_in_data->Get("h1D_TruedNdEta");
    data_h1D_TruedNdEta -> SetFillColorAlpha(1,0);
    data_h1D_TruedNdEta -> SetLineColor(4); // note : blue



    TH1D * MC_h1D_BestPair_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc->Get("h1D_BestPair_RecoTrackletEtaPerEvt");
    TH1D * MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc->Get("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    TH1D * MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    MC_h1D_BestPair_RecoTrackletEtaPerEvt -> SetMarkerColor(2);
    MC_h1D_BestPair_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(2);
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    

    TH1D * MC_h1D_TruedNdEta = (TH1D*)file_in_mc->Get("h1D_TruedNdEta");
    MC_h1D_TruedNdEta -> SetFillColorAlpha(2,0);
    MC_h1D_TruedNdEta -> SetLineColor(3); // note : green


    TFile * file_out = new TFile((output_directory_in+"/"+output_filename_in).c_str(), "RECREATE");
    TCanvas * c1 = new TCanvas("c1", "c1", 800, 600);

    c1 -> cd();
    data_h1D_BestPair_RecoTrackletEtaPerEvt -> Draw("ep");
    MC_h1D_BestPair_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_BestPair_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("ep");
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> Draw("ep");
    MC_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> Draw("hist same");
    MC_h1D_TruedNdEta -> Draw("hist same");
    data_h1D_TruedNdEta -> Draw("hist same");
    c1 -> Write("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> Draw("ep");
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> Draw("hist same");
    MC_h1D_TruedNdEta -> Draw("hist same");
    data_h1D_TruedNdEta -> Draw("hist same");
    c1 -> Write("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");
    c1 -> Clear();

    c1 -> cd();
    MC_h1D_TruedNdEta -> Draw("hist");
    data_h1D_TruedNdEta -> Draw("ep same");
    c1 -> Write("h1D_TruedNdEta");
    c1 -> Clear();

    file_out -> Close();

    

    return 0;
}

int CombinedMacro()
{   
    // note : for all common
    std::string output_file_name_suffix = "_SecondRun"; 

    bool isTypeA = false;
    std::pair<double,double> cut_INTTvtxZ = {-10, 10};
    int SelectedMbin = 70;
    std::pair<double,double> cut_EtaRange = {-1.9, 1.9};
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/TrackHistNew_WithClusQA/completed";

    string output_suffix = "dNdEta";
    output_suffix += (isTypeA) ? "_TypeA" : "_AllSensor";
    output_suffix += Form("_VtxZ%d_Mbin%d", (int)cut_INTTvtxZ.second, SelectedMbin);

    string final_output_directory = output_directory + "/" + output_suffix;
    
    // string alpha_correction_input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_dNdEta/completed/MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin70_SecondRun_00001_dNdEta.root";    
    
    // note : MC common
    string MC_input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/TrackHistNew_WithClusQA/completed";
    string MC_output_directory = final_output_directory;

    // note : MC1 deriving the alpha corrections
    string MC1_input_filename = "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize500_SecondRun_merged_001.root";
    int    MC1_process_id = 1;
    int    MC1_run_num = -1;
    bool   MC1_ApplyAlphaCorr = false;
    std::pair<bool, std::pair<double,double>> MC1_setEtaRange = {false, cut_EtaRange};

    // note : MC2 applying the alpha corrections
    string MC2_input_filename = "MC_TrackHist_vtxZReweight_VtxZQA_ClusQAAdc35PhiSize500_SecondRun_merged_002.root";
    int    MC2_process_id = 2;
    int    MC2_run_num = -1;
    bool   MC2_ApplyAlphaCorr = true;
    std::pair<bool, std::pair<double,double>> MC2_setEtaRange = {true, cut_EtaRange};
    
    // note : for data
    string data_input_directory  = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/TrackHistNew_WithClusQA/completed";
    string data_input_filename   = "Data_TrackHist_BcoFullDiffCut_VtxZQA_ClusQAAdc35PhiSize500_SecondRun_00054280_merged.root";
    string data_output_directory = final_output_directory;
    int    data_process_id = 0;
    int    data_run_num = 54280;
    bool   data_ApplyAlphaCorr = true;
    std::pair<bool, std::pair<double,double>> data_setEtaRange = {true, cut_EtaRange};

    system(Form("if [ ! -d %s/completed ]; then mkdir -p %s/completed; fi;", final_output_directory.c_str(), final_output_directory.c_str()));

    string MC1_dNdeta_file = Run_PreparedNdEtaEach(
        MC1_process_id,
        MC1_run_num,
        MC_input_directory,
        MC1_input_filename,
        MC_output_directory,

        output_file_name_suffix,

        MC1_ApplyAlphaCorr,
        isTypeA,
        cut_INTTvtxZ,
        SelectedMbin
    );

    string alpha_correction_input_directory = MC1_dNdeta_file;


    string MC2_dNdeta_file = Run_PreparedNdEtaEach_ApplyAlphaCorr(
        MC2_process_id,
        MC2_run_num,
        MC_input_directory,
        MC2_input_filename,
        MC_output_directory,

        output_file_name_suffix,

        MC2_ApplyAlphaCorr,
        isTypeA,
        cut_INTTvtxZ,
        SelectedMbin,
        MC2_setEtaRange,

        alpha_correction_input_directory
    );


    string data_dNdeta_file = Run_PreparedNdEtaEach_ApplyAlphaCorr_54280(
        data_process_id,
        data_run_num,
        data_input_directory,
        data_input_filename,
        data_output_directory,

        output_file_name_suffix,

        data_ApplyAlphaCorr,
        isTypeA,
        cut_INTTvtxZ,
        SelectedMbin,
        data_setEtaRange,

        alpha_correction_input_directory
    );

    DataMcComp(data_dNdeta_file, MC1_dNdeta_file, final_output_directory, "DataMc1Comp.root");
    DataMcComp(data_dNdeta_file, MC2_dNdeta_file, final_output_directory, "DataMc2Comp.root");
    McMcComp(MC1_dNdeta_file, MC2_dNdeta_file, final_output_directory, "McMcComp.root");
    

    return 0;
}