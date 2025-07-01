#include "../AvgVtxXY/AvgVtxXY.h"
#include "../EvtVtxZTracklet/EvtVtxZProtoTracklet.h"
#include "../TrackletHistogram/TrackletHistogramNew.h"
#include "../PreparedNdEta/PreparedNdEtaEach.h"

R__LOAD_LIBRARY(../AvgVtxXY/libavgvtxxy.so)
R__LOAD_LIBRARY(../EvtVtxZTracklet/libEvtVtxZProtoTracklet.so)
R__LOAD_LIBRARY(../TrackletHistogram/libTrackletHistogramNew.so)
R__LOAD_LIBRARY(../PreparedNdEta/libPreparedNdEtaEach.so)

TH2D * GetGoodColMap (std::string ColMulMask_map_dir_in, std::string ColMulMask_map_file_in, std::string map_name_in)
{
  TFile * f = TFile::Open(Form("%s/%s", ColMulMask_map_dir_in.c_str(), ColMulMask_map_file_in.c_str()));
  TH2D * h = (TH2D*)f->Get(map_name_in.c_str());
  return h;
}

TH1D * GetDataVtxZH1D (std::string data_vtxZ_directory_file_in, std::string data_vtxZ_hist_name_in)
{
  TFile * f2 = TFile::Open(Form("%s", data_vtxZ_directory_file_in.c_str()));
  TH1D * h2 = (TH1D*)f2->Get(data_vtxZ_hist_name_in.c_str());
  return h2;
}

TH1D * GetVtxZReweightH1D (TH1D * data_hist, TH1D * mc_hist)
{
    if (data_hist->GetNbinsX() != mc_hist->GetNbinsX()){
        cout<<"GetVtxZReweightH1D: data_hist->GetNbinsX() != mc_hist->GetNbinsX()"<<endl;
        exit(1);
        return nullptr;
    }
    if (data_hist->GetXaxis()->GetXmin() != mc_hist->GetXaxis()->GetXmin()){
        cout<<"GetVtxZReweightH1D: data_hist->GetXaxis()->GetXmin() != mc_hist->GetXaxis()->GetXmin()"<<endl;
        exit(1);
        return nullptr;
    }
    if (data_hist->GetXaxis()->GetXmax() != mc_hist->GetXaxis()->GetXmax()){
        cout<<"GetVtxZReweightH1D: data_hist->GetXaxis()->GetXmax() != mc_hist->GetXaxis()->GetXmax()"<<endl;
        exit(1);
        return nullptr;
    }

    TH1D * h3 = (TH1D*)data_hist->Clone("reweight_hist");
    h3 -> Scale(1.0 / h3->Integral());
    mc_hist -> Scale(1.0 / mc_hist->Integral());
    h3->Divide(mc_hist); // note : data / mc
    return h3;
}

int Run_GeoOffset(
    int process_id = 0,
    int run_num = -1,
    int nevents = -1,
    string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro", // note : 
    string input_filename = "file_list_54280_intt.txt", // note : the ntuple
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
    
    // todo : modify here
    std::string output_file_name_suffix = "_test1",
    
    // todo : the data vertex Z distribution should be updated
    // std::string data_vtxZ_directory_file = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Jan172025/Run4/EvtVtxZ/completed/VtxZDist/completed/Data_vtxZDist_VtxZQA_EvtBcoFullDiffCut61_00054280_merged.root", // note : full directory
    // std::string data_vtxZ_hist_name = "h1D_INTTz_Inclusive70",

    // todo: the map file should be updated
    std::string ColMulMask_map_dir = "/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_HIJING_MDC2_ana472_20250307/Run7/EvtVtxZ/ColumnCheck/baseline/completed/MulMap/completed",
    std::string ColMulMask_map_file = "MulMap_BcoFullDiffCut_Mbin70_VtxZ-30to30cm_ClusQAAdc35PhiSize40_00054280.root"
)
{
    
    bool Global_GeoOffsetTag = (process_id == 0) ? false : true;
    int Global_ClusAdcCut = 35;
    int Global_ClusPhiSizeCut = 40;

    bool is_tracklet_DeltaPhiCut_changed = true;
    std::pair<double,double> tracklet_DeltaPhiCut = {-0.04, 0.04}; 

    std::string GeoOffset_string = (Global_GeoOffsetTag) ? "" : "_NoGeoOffset";

    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    std::string final_output_directory = output_directory + Form("/Run%s_%s", GeoOffset_string.c_str(), job_index.c_str()); 
    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi", final_output_directory.c_str(), final_output_directory.c_str()));

    std::pair<double,double> AvgVtxXY_MBD_vtxZ_cut = {-20, 20};
    std::pair<int,int> AvgVtxXY_INTTNClus_cut = {20, 350};
    int AvgVtxXY_ClusAdc_cut = Global_ClusAdcCut;
    int AvgVtxXY_ClusPhiSize_cut = Global_ClusPhiSizeCut;

    bool AvgVtxXY_HaveGeoOffsetTag = Global_GeoOffsetTag;
    double AvgVtxXY_random_range_XYZ = 0.025; // note : unit : cm [250 um]

    AvgVtxXY * avgXY = new AvgVtxXY(
        process_id,
        run_num,
        nevents,
        input_directory,
        input_filename,
        final_output_directory,
        output_file_name_suffix,

        AvgVtxXY_MBD_vtxZ_cut,
        AvgVtxXY_INTTNClus_cut,
        AvgVtxXY_ClusAdc_cut,
        AvgVtxXY_ClusPhiSize_cut,

        AvgVtxXY_HaveGeoOffsetTag,
        AvgVtxXY_random_range_XYZ
    );

    std::string AvgXY_output_filename = avgXY->GetOutputFileName();
    cout<<"AvgXY: final_output_file_name: "<<AvgXY_output_filename<<endl;
    std::map<std::string, std::vector<double>> GeoOffset_map = avgXY->GetGeoOffsetMap();

    avgXY -> PreparePairs();
  
    // avgXY -> FindVertexQuadrant( 1, 0.4, {0, 0} ); // note : unit : cm
    // avgXY -> FindVertexLineFill({-0.022,0.2229}, 100, 0.25, 0.0001); // note : unit : cm
    // avgXY -> EndRun();

    avgXY -> FindVertexQuadrant( 9, 0.4, {0, 0} ); // note : unit : cm
    avgXY -> FindVertexLineFill(avgXY -> GetVertexQuadrant(), 100, 0.25, 0.0001); // note : unit : cm
    
    std::pair<double, double> vertexXYIncm = avgXY -> GetFinalAvgVtxXY();
    cout<<"vertexXYIncm: "<<vertexXYIncm.first<<" "<<vertexXYIncm.second<<endl;

    avgXY -> EndRun();

    std::cout<<"test test: done with AvgVtxXY"<<std::endl;

    // Division : --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool evtZ_IsFieldOn = false;
    bool evtZ_IsDCACutApplied = true;
    std::pair<std::pair<double,double>,std::pair<double,double>> evtZ_DeltaPhiCutInDegree = {{-0.6,0.6},{-1000.,1000.}}; // note : in degree
    std::pair<std::pair<double,double>,std::pair<double,double>> evtZ_DCAcutIncm = {{-0.1,0.1},{-1000.,1000.}}; // note : in cm
    int evtZ_ClusAdcCut = Global_ClusAdcCut;
    int evtZ_ClusPhiSizeCut = Global_ClusPhiSizeCut;
    
    bool evtZ_PrintRecoDetails = false;
    bool evtZ_DrawEvtVtxZ = (process_id < 10) ? true : false;

    bool evtZ_RunInttBcoFullDiff = false;
    bool evtZ_RunVtxZReco = true;
    bool evtZ_RunTrackletPair = false;
    bool evtZ_RunTrackletPairRotate = false;
    
    bool evtZ_HaveGeoOffsetTag = Global_GeoOffsetTag;

    EvtVtxZProtoTracklet * evtVtxZ = new EvtVtxZProtoTracklet(
        process_id,
        run_num,
        nevents,
        input_directory,
        input_filename,
        final_output_directory,
        output_file_name_suffix,

        vertexXYIncm,
        evtZ_IsFieldOn,
        evtZ_IsDCACutApplied,
        evtZ_DeltaPhiCutInDegree,
        evtZ_DCAcutIncm,
        evtZ_ClusAdcCut,
        evtZ_ClusPhiSizeCut,

        evtZ_PrintRecoDetails,
        evtZ_DrawEvtVtxZ,

        evtZ_RunInttBcoFullDiff,
        evtZ_RunVtxZReco,
        evtZ_RunTrackletPair,
        evtZ_RunTrackletPairRotate,

        evtZ_HaveGeoOffsetTag
    );

    std::string evtVtxZ_output_filename = evtVtxZ->GetOutputFileName();
    cout<<"evtVtxZ: final_output_file_name: "<<evtVtxZ_output_filename<<endl;

    evtVtxZ -> SetGeoOffset(GeoOffset_map);
    evtVtxZ -> MainProcess();

    // TH1D * h1D_MC_INTTRecoVtxZ = (TH1D*) (evtVtxZ -> GetINTTRecovtxZH1D())->Clone("h1D_MC_INTTRecoVtxZ");
    // TH1D * h1D_data_INTTRecoVtxZ = GetDataVtxZH1D(data_vtxZ_directory_file, data_vtxZ_hist_name);
    // TH1D * h1D_vtxZReweight = GetVtxZReweightH1D(h1D_data_INTTRecoVtxZ, h1D_MC_INTTRecoVtxZ);

    // TFile * file_out = new TFile(Form("%s/%s", final_output_directory.c_str(), "VtxZ_hist.root"), "recreate");
    // std::cout<<"test1: h1D_MC_INTTRecoVtxZ->GetName(): "<<h1D_MC_INTTRecoVtxZ->GetName()<<std::endl;
    // std::cout<<"test1: h1D_MC_INTTRecoVtxZ->GetNbinsX(): "<<h1D_MC_INTTRecoVtxZ->GetNbinsX()<<std::endl;
    // h1D_MC_INTTRecoVtxZ -> Write("h1D_MC_INTTRecoVtxZ"); 
    // h1D_data_INTTRecoVtxZ -> Write("h1D_data_INTTRecoVtxZ"); 
    // h1D_vtxZReweight -> Write("h1D_vtxZReweight"); 
    // file_out -> Close();

    // for (int i = 0; i < h1D_vtxZReweight->GetNbinsX(); i++){
    //     cout<<"h1D_vtxZReweight, index: "<<i+1
    //     <<", center: "<<h1D_vtxZReweight -> GetBinCenter(i+1)
    //     <<", data: "  <<Form("%.3f",h1D_data_INTTRecoVtxZ->GetBinContent(i+1))
    //     <<", MC: "    <<Form("%.3f",h1D_MC_INTTRecoVtxZ->GetBinContent(i+1))
    //     <<", ratio: " <<Form("%.3f",h1D_vtxZReweight->GetBinContent(i+1))<<endl;
    // }

    std::cout<<"test test: closing in EvtVtxZProtoTracklet "<<std::endl;
    evtVtxZ -> EndRun(0); // note : do not close the file_in

    std::cout<<"test test: done with EvtVtxZProtoTracklet"<<std::endl;
    sleep(30);

    // Division : --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // std::pair<bool, TH1D*> Track_vtxZReweight = {true, h1D_vtxZReweight};
    std::pair<bool, TH1D*> Track_vtxZReweight = {false, nullptr};
    bool Track_BcoFullDiffCut = false;
    bool Track_INTT_vtxZ_QA = true;
    std::pair<bool, std::pair<double, double>> Track_isClusQA = {true, {Global_ClusAdcCut, Global_ClusPhiSizeCut}}; // note : {adc, phi size}
    bool Track_HaveGeoOffsetTag = Global_GeoOffsetTag;
    std::pair<bool, int> Track_SetRandomHits = {false, 0};
    bool Track_RandInttZ = false;

    bool Track_ColMulMask = true;
    

    TrackletHistogramNew * THN = new TrackletHistogramNew(
        process_id, 
        run_num,
        nevents,
        final_output_directory,
        evtVtxZ_output_filename,
        final_output_directory,

        output_file_name_suffix,
        vertexXYIncm,

        Track_vtxZReweight,
        Track_BcoFullDiffCut,
        Track_INTT_vtxZ_QA,
        Track_isClusQA,
        Track_HaveGeoOffsetTag,
        Track_SetRandomHits,
        Track_RandInttZ,
        Track_ColMulMask
    );

    std::string Track_output_filename = THN->GetOutputFileName();
    cout<<"Track: final_output_file_name: "<<Track_output_filename<<endl;

    THN -> SetGeoOffset(GeoOffset_map);

    if (Track_ColMulMask){
        THN -> SetGoodColMap(
        GetGoodColMap(ColMulMask_map_dir, ColMulMask_map_file, THN->GetGoodColMapName())
        );
    }

    THN -> MainProcess();

    std::cout<<"test test: closing in TrackletHistogramNew "<<std::endl;
    THN -> EndRun();

    std::cout<<"test test: done with TrackletHistogramNew"<<std::endl;
    sleep(30);

    // Division : --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool dNdeta_ApplyAlphaCorr = false;
    bool dNdeta_isTypeA = false;
    std::pair<double,double> dNdeta_cut_INTTvtxZ = {-10, 10};
    int dNdeta_SelectedMbin = 70; // note : 0, 1, ---- 10, 70, 100 

    bool dNdeta_ApplyGeoAccCorr = false;

    PreparedNdEtaEach * pdndeta = new PreparedNdEtaEach(
        process_id, 
        run_num,
        final_output_directory,
        Track_output_filename,
        final_output_directory,
        output_file_name_suffix,

        dNdeta_ApplyAlphaCorr,
        dNdeta_ApplyGeoAccCorr,

        dNdeta_isTypeA,
        dNdeta_cut_INTTvtxZ,
        dNdeta_SelectedMbin
    );

    // todo: if change cut
    if (is_tracklet_DeltaPhiCut_changed) {pdndeta->SetBkgRotated_DeltaPhi_Signal_range(tracklet_DeltaPhiCut);}

    std::vector<std::string> dNdeta_output_filename = pdndeta->GetOutputFileName();
    cout<<"dNdeta: final_output_file_name: "<<dNdeta_output_filename[0]<<endl;
    cout<<"dNdeta: final_output_file_name: "<<dNdeta_output_filename[1]<<endl;

    pdndeta -> PrepareStacks();
    pdndeta -> DoFittings();
    pdndeta -> PrepareMultiplicity();
    pdndeta -> PreparedNdEtaHist();
    pdndeta -> DeriveAlphaCorrection();

    std::cout<<"test test: closing in PreparedNdEtaEach "<<std::endl;
    pdndeta -> EndRun();

    std::cout<<"test test: done with PreparedNdEtaEach"<<std::endl;

    // Division : --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    std::cout<<"test test: closing in AvgVtxXY "<<std::endl;    

    std::cout<<"test test: done with in AvgVtxXY "<<std::endl;    

    sleep(30);

    system(Form("mv %s %s/completed", final_output_directory.c_str(), output_directory.c_str()));

    return 0;
}