#include "GetFinalResult.h"

GetFinalResult::GetFinalResult(
    int runnumber_in,
    int SelectedMbin_in, // note : 0, 1, ---- 10, 70, 100 
    std::pair<double,double> vtxZ_range_in, // note : cm
    bool isTypeA_in,
    bool ApplyGeoAccCorr_in,
    std::pair<bool, std::pair<double,double>> cut_EtaRange_in,

    std::string output_file_name_suffix_in,

    std::string output_directory_in
):
    runnumber(runnumber_in),
    SelectedMbin(SelectedMbin_in),
    vtxZ_range(vtxZ_range_in),
    isTypeA(isTypeA_in),
    ApplyGeoAccCorr(ApplyGeoAccCorr_in),
    cut_EtaRange(cut_EtaRange_in),
    output_file_name_suffix(output_file_name_suffix_in),
    output_directory(output_directory_in)
{
    SetResultRangeFolderName = Form("vtxZ_%.0f_%.0fcm_MBin%d", vtxZ_range.first, vtxZ_range.second, SelectedMbin);
    SetResultRangeFolderName += (isTypeA) ? "_TypeA" : "";
    SetResultRangeFolderName += (ApplyGeoAccCorr) ? "_GeoAccCorr" : "";

    SemiMotherFolderName = output_directory + "/" + SetResultRangeFolderName;

    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi;", SemiMotherFolderName.c_str(), SemiMotherFolderName.c_str()));

    BaseLine_AlphaCorr_directory = no_map;
    BaseLine_dNdEta_directory = no_map;
}

void GetFinalResult::PrepareBaseLine(
    std::string data_input_directory,
    std::string data_input_filename,
    
    std::string MC_input_directory,
    std::string MC1_input_filename,
    std::string MC2_input_filename
)
{
    std::vector<std::string> temp_file_dir = PreparedNdEtaPlain(
        0,
        true,
        true,
        Folder_BaseLine,
        data_input_directory,
        data_input_filename,

        MC_input_directory,
        MC1_input_filename,
        MC2_input_filename
    );

    // todo: if there are more directories in temp_file_dir, check here
    BaseLine_AlphaCorr_directory = temp_file_dir[0];
    BaseLine_dNdEta_directory = temp_file_dir[1];
}

void GetFinalResult::PrepareRunSegment(
    std::string data_input_directory,
    std::vector<std::string> data_input_filename,
    
    std::string MC_input_directory,
    std::string MC1_input_filename,
    std::string MC2_input_filename
) // note : two times
{
    for (int i = 0; i < data_input_filename.size(); i++){
        PreparedNdEtaPlain(
            i,
            true,
            true,
            Folder_RunSegment,
            data_input_directory,
            data_input_filename[i],

            MC_input_directory,
            MC1_input_filename,
            MC2_input_filename
        );
    }
}

void GetFinalResult::PrepareClusAdcCut(
    int run_index,
    std::string data_input_directory,
    std::string data_input_filename,
    
    std::string MC_input_directory,
    std::string MC1_input_filename,
    std::string MC2_input_filename
) // todo : can be twice
{
    PreparedNdEtaPlain(
        run_index,
        true,
        true,
        Folder_ClusAdcCut,
        data_input_directory,
        data_input_filename,

        MC_input_directory,
        MC1_input_filename,
        MC2_input_filename
    );
}

void GetFinalResult::PrepareClusPhiCut(
    std::string data_input_directory,
    std::vector<std::string> data_input_filename,
    
    std::string MC_input_directory,
    std::string MC1_input_filename,
    std::string MC2_input_filename
)
{
    for (int i = 0; i < data_input_filename.size(); i++){
        PreparedNdEtaPlain(
            i,
            true,
            true,
            Folder_ClusPhiCut,
            data_input_directory,
            data_input_filename[i],

            MC_input_directory,
            MC1_input_filename,
            MC2_input_filename
        );
    }
}

void GetFinalResult::PrepareDeltaPhiCut(
    std::vector<double> cut_SigDeltaPhi,    
    std::string data_input_directory,
    std::string data_input_filename,
    
    std::string MC_input_directory,
    std::string MC1_input_filename,
    std::string MC2_input_filename
) // note : two times 
{
    for (int i = 0; i < cut_SigDeltaPhi.size(); i++)
    {
        PreparedNdEtaPlain(
            i,
            true,
            true,
            Folder_DeltaPhiCut,
            data_input_directory,
            data_input_filename,

            MC_input_directory,
            MC1_input_filename,
            MC2_input_filename,

            {true, {-1. * cut_SigDeltaPhi[i], cut_SigDeltaPhi[i]}}
        );
    }
}


// Division : -the macros---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

std::vector<std::string> GetFinalResult::PreparedNdEtaPlain(
    int index,
    bool PrepareAlphaCorr,
    bool RunComparison,
    std::string sub_folder_name,
    std::string data_input_directory,
    std::string data_input_filename,
    
    std::string MC_input_directory,
    std::string MC1_input_filename,
    std::string MC2_input_filename,

    std::pair<bool, std::pair<double,double>> cut_DeltaPhi_Signal_range
)
{
    std::string Plain_output_directory = SemiMotherFolderName + "/" + sub_folder_name + "/Run_" + std::to_string(index);
    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi;", Plain_output_directory.c_str(), Plain_output_directory.c_str()));

    // todo: 
    std::string GeoAccCorr_directory = no_map;

    // Division : ------------------------------------------------------------------------------
    int    MC1_process_id = 1;
    int    MC1_run_num = -1;
    bool   MC1_ApplyAlphaCorr = false;
    std::pair<bool, std::pair<double,double>> MC1_setEtaRange = {false, eta_range_dNdEta_Preparation};

    // note : MC2 applying the alpha corrections
    int    MC2_process_id = 2;
    int    MC2_run_num = -1;
    bool   MC2_ApplyAlphaCorr = true;
    std::pair<bool, std::pair<double,double>> MC2_setEtaRange = {true, eta_range_dNdEta_Preparation};

    int    data_process_id = 0;
    bool   data_ApplyAlphaCorr = true;
    std::pair<bool, std::pair<double,double>> data_setEtaRange = {true, eta_range_dNdEta_Preparation};

    // Division : ------------------------------------------------------------------------------

    std::string alpha_correction_input_directory = no_map;
    std::string MC1_dNdeta_file = no_map;
    std::string MC2_dNdeta_file = no_map;

    if (PrepareAlphaCorr){
        MC1_dNdeta_file = Run_PreparedNdEtaEach(
            MC1_process_id,
            MC1_run_num,
            MC_input_directory,
            MC1_input_filename,
            Plain_output_directory,

            output_file_name_suffix,

            MC1_ApplyAlphaCorr,
            ApplyGeoAccCorr,

            isTypeA,
            vtxZ_range,
            SelectedMbin,

            cut_DeltaPhi_Signal_range,
            {false, {-2.7, 2.7}}, // note : the eta range
            GeoAccCorr_directory,
            no_map
        );

        alpha_correction_input_directory = MC1_dNdeta_file;
    }
    else {
        alpha_correction_input_directory = BaseLine_AlphaCorr_directory;
        if (alpha_correction_input_directory == no_map){
            std::cout<<"Error : alpha_correction_input_directory == \"no_map\""<<std::endl;
            exit(1);
        }
    }

    if (PrepareAlphaCorr){
        MC2_dNdeta_file = Run_PreparedNdEtaEach(
            MC2_process_id,
            MC2_run_num,
            MC_input_directory,
            MC2_input_filename,
            Plain_output_directory,

            output_file_name_suffix,

            MC2_ApplyAlphaCorr,
            ApplyGeoAccCorr,

            isTypeA,
            vtxZ_range,
            SelectedMbin,

            cut_DeltaPhi_Signal_range,
            MC2_setEtaRange,
            GeoAccCorr_directory,
            alpha_correction_input_directory
        );
    }


    std::string data_dNdeta_file = Run_PreparedNdEtaEach(
        data_process_id,
        runnumber,
        data_input_directory,
        data_input_filename,
        Plain_output_directory,

        output_file_name_suffix,

        data_ApplyAlphaCorr,
        ApplyGeoAccCorr,

        isTypeA,
        vtxZ_range,
        SelectedMbin,

        cut_DeltaPhi_Signal_range,
        data_setEtaRange,
        GeoAccCorr_directory, 
        alpha_correction_input_directory
    );

    if (RunComparison && PrepareAlphaCorr)
    {        
        DataMcComp(data_dNdeta_file, MC1_dNdeta_file, Plain_output_directory, "DataMc1Comp.root");
        McMcComp(MC1_dNdeta_file, MC2_dNdeta_file, Plain_output_directory, "McMcComp.root");
        DataMcComp(data_dNdeta_file, MC2_dNdeta_file, Plain_output_directory, "DataMc2Comp.root");
    }

    return {
        alpha_correction_input_directory,
        data_dNdeta_file
    };

}


std::string GetFinalResult::Run_PreparedNdEtaEach(
  int process_id,
  int run_num,
  std::string input_directory,
  std::string input_filename,
  std::string output_directory,
  
  // todo : modify here
  std::string output_file_name_suffix, 

  bool ApplyAlphaCorr,
  bool func_ApplyGeoAccCorr,

  bool isTypeA,
  std::pair<double,double> cut_INTTvtxZ,
  int SelectedMbin,

  std::pair<bool, std::pair<double,double>> setBkgRotated_DeltaPhi_Signal_range,
  std::pair<bool, std::pair<double,double>> setEtaRange,
  std::string GeoAccCorr_input_directory,
  std::string alpha_correction_input_directory
)
{

    system(Form("if [ ! -d %s/completed ]; then mkdir -p %s/completed; fi;", output_directory.c_str(), output_directory.c_str()));  

  PreparedNdEtaEach * PNEE = new PreparedNdEtaEach(
    process_id,
    run_num,
    input_directory,
    input_filename,
    output_directory,

    output_file_name_suffix,

    ApplyAlphaCorr,
    func_ApplyGeoAccCorr,

    isTypeA,
    cut_INTTvtxZ,
    SelectedMbin
  );

  if (func_ApplyGeoAccCorr && GeoAccCorr_input_directory != no_map)
  {
    PNEE -> SetGeoAccCorrH2DMap(
      GetGeoAccCorrH2DMap(
        GeoAccCorr_input_directory,
        PNEE->GetGeoAccCorrNameMap()
      )
    );
  }

  if (ApplyAlphaCorr && alpha_correction_input_directory != no_map)
  {
    PNEE -> SetAlphaCorrectionH1DMap(
      GetAlphaCorrectionH1DMap(
        alpha_correction_input_directory,
        PNEE->GetAlphaCorrectionNameMap()
      )
    );
  }

  if (setEtaRange.first) {PNEE -> SetSelectedEtaRange(setEtaRange.second);}

  if (setBkgRotated_DeltaPhi_Signal_range.first)
  {
    PNEE -> SetBkgRotated_DeltaPhi_Signal_range(setBkgRotated_DeltaPhi_Signal_range.second);
  }

  std::vector<std::string> final_output_file_name = PNEE->GetOutputFileName();
  for (auto filename : final_output_file_name){
    cout<<"final_output_file_name: "<<filename<<endl;
    system(Form("if [ -f %s/completed/%s ]; then rm %s/completed/%s; fi;", output_directory.c_str(), filename.c_str(), output_directory.c_str(), filename.c_str()));  
  }
  
  std::cout<<000<<endl;
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

    std::string dNdEta_file_out;

  for (auto filename : final_output_file_name){
    system(Form("mv %s/%s %s/completed", output_directory.c_str(), filename.c_str(), output_directory.c_str()));

    if (filename.find("_dNdEta.root") != std::string::npos){
      dNdEta_file_out = output_directory + "/completed/" + filename;
    }
  }

  std::cout<<"dNdEta_file_out: "<<dNdEta_file_out<<std::endl;

    // note : testing
    delete PNEE;

  return dNdEta_file_out;
}

int GetFinalResult::DataMcComp(string data_directory_in, string MC_directory_in, string output_directory_in, string output_filename_in)
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

int GetFinalResult::McMcComp(string MC1_directory_in, string MC2_directory_in, string output_directory_in, string output_filename_in)
{
    TFile * file_in_mc1 = TFile::Open(MC1_directory_in.c_str());
    TFile * file_in_mc2 = TFile::Open(MC2_directory_in.c_str());

    TH1D * MC1_h1D_BestPair_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc1->Get("h1D_BestPair_RecoTrackletEtaPerEvt");
    TH1D * MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc1->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * MC1_h1D_BestPair_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc1->Get("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    TH1D * MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc1->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    MC1_h1D_BestPair_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    MC1_h1D_BestPair_RecoTrackletEtaPerEvt -> SetLineColor(1);

    MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(1);

    MC1_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    MC1_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);

    MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);

    TH1D * MC1_h1D_TruedNdEta = (TH1D*)file_in_mc1->Get("h1D_TruedNdEta");
    MC1_h1D_TruedNdEta -> SetFillColorAlpha(1,0);
    MC1_h1D_TruedNdEta -> SetLineColor(4); // note : blue



    TH1D * MC2_h1D_BestPair_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc2->Get("h1D_BestPair_RecoTrackletEtaPerEvt");
    TH1D * MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc2->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * MC2_h1D_BestPair_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc2->Get("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    TH1D * MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc2->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    MC2_h1D_BestPair_RecoTrackletEtaPerEvt -> SetMarkerColor(2);
    MC2_h1D_BestPair_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(2);
    MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC2_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC2_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    

    TH1D * MC2_h1D_TruedNdEta = (TH1D*)file_in_mc2->Get("h1D_TruedNdEta");
    MC2_h1D_TruedNdEta -> SetFillColorAlpha(2,0);
    MC2_h1D_TruedNdEta -> SetLineColor(3); // note : green


    TFile * file_out = new TFile((output_directory_in+"/"+output_filename_in).c_str(), "RECREATE");
    TCanvas * c1 = new TCanvas("c1", "c1", 800, 600);

    c1 -> cd();
    MC1_h1D_BestPair_RecoTrackletEtaPerEvt -> Draw("ep");
    MC2_h1D_BestPair_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_BestPair_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("ep");
    MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    MC1_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> Draw("ep");
    MC2_h1D_BestPair_RecoTrackletEtaPerEvtPostAC -> Draw("hist same");
    MC2_h1D_TruedNdEta -> Draw("hist same");
    MC1_h1D_TruedNdEta -> Draw("hist same");
    c1 -> Write("h1D_BestPair_RecoTrackletEtaPerEvtPostAC");
    c1 -> Clear();

    c1 -> cd();
    MC1_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> Draw("ep");
    MC2_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> Draw("hist same");
    MC2_h1D_TruedNdEta -> Draw("hist same");
    MC1_h1D_TruedNdEta -> Draw("hist same");
    c1 -> Write("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");
    c1 -> Clear();

    c1 -> cd();
    MC2_h1D_TruedNdEta -> Draw("hist");
    MC1_h1D_TruedNdEta -> Draw("ep same");
    c1 -> Write("h1D_TruedNdEta");
    c1 -> Clear();

    file_out -> Close();

    

    return 0;
}


std::map<std::string, TH1D*> GetFinalResult::GetAlphaCorrectionH1DMap(std::string alpha_correction_input_directory_in, std::vector<std::string> map_name_in){
  
  TFile * file_in = TFile::Open(alpha_correction_input_directory_in.c_str());
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


std::map<std::string, TH2D*> GetFinalResult::GetGeoAccCorrH2DMap(std::string GeoAccCorr_input_directory_in, std::vector<std::string> map_name_in){
  
  TFile * file_in = TFile::Open(GeoAccCorr_input_directory_in.c_str());
  std::map<std::string, TH2D*> h2D_GeoAccCorr_map; h2D_GeoAccCorr_map.clear();
  
  
  for (TObject* keyAsObj : *file_in->GetListOfKeys()){
    auto key = dynamic_cast<TKey*>(keyAsObj);
    std::string hist_name  = key->GetName();
    std::string class_name = key->GetClassName();

    if (class_name != "TH2D") {continue;}
    if (std::find(map_name_in.begin(), map_name_in.end(), hist_name) == map_name_in.end()) {continue;}

    h2D_GeoAccCorr_map.insert(
      std::make_pair(
        hist_name,
        (TH2D*)file_in->Get(hist_name.c_str())
      )
    );
  }

  for (auto &pair : h2D_GeoAccCorr_map){
    // pair.second->SetDirectory(0);
    std::cout << "Geo Acc Corr map name : " << pair.first << std::endl;
  }

  if (h2D_GeoAccCorr_map.size() == 0){
    std::cout << "Error : h2D_GeoAccCorr_map.size() == 0" << std::endl;
    exit(1);
  }

  return h2D_GeoAccCorr_map;
}