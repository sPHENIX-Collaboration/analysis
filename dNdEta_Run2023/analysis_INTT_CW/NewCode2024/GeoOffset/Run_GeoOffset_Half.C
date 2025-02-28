#include "../AvgVtxXY/AvgVtxXY.h"
#include "../EvtVtxZTracklet/EvtVtxZProtoTracklet.h"
#include "../TrackletHistogram/TrackletHistogramNew.h"
#include "../PreparedNdEta/PreparedNdEtaEach.h"

R__LOAD_LIBRARY(../AvgVtxXY/libavgvtxxy.so)
R__LOAD_LIBRARY(../EvtVtxZTracklet/libEvtVtxZProtoTracklet.so)
R__LOAD_LIBRARY(../TrackletHistogram/libTrackletHistogramNew.so)
R__LOAD_LIBRARY(../PreparedNdEta/libPreparedNdEtaEach.so)

int Run_GeoOffset_Half(
    int process_id = 0,
    int run_num = -1,
    int nevents = -1,
    string input_directory = "/sphenix/user/ChengWei/INTT/INTT/general_codes/CWShih/INTTBcoResolution/macro", // note : 
    string input_filename = "file_list_54280_intt.txt", // note : the ntuple
    string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/completed/BCO_check",
    
    // todo : modify here
    std::string output_file_name_suffix = "_test1"
    std::string AvgVtxXY_input_filename = "AvgVtxXY_GeoOffset_test1",
)
{   

    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    TFile * file_in_geo = TFile::Open(Form("%s/%s_%s.root", input_directory.c_str(), AvgVtxXY_input_filename.c_str(), job_index.c_str()));
    


    // Division : --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    std::pair<bool, TH1D*> Track_vtxZReweight = {false, nullptr};
    bool Track_BcoFullDiffCut = false;
    bool Track_INTT_vtxZ_QA = true;
    std::pair<bool, std::pair<double, double>> Track_isClusQA = {true, {35, 500}}; // note : {adc, phi size}
    bool Track_HaveGeoOffsetTag = true;
    std::pair<bool, int> Track_SetRandomHits = {false, 0};
    bool Track_RandInttZ = false;

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
        Track_RandInttZ
    );

    std::string Track_output_filename = THN->GetOutputFileName();
    cout<<"Track: final_output_file_name: "<<Track_output_filename<<endl;

    THN -> SetGeoOffset(GeoOffset_map);
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

    PreparedNdEtaEach * pdndeta = new PreparedNdEtaEach(
        process_id, 
        run_num,
        final_output_directory,
        Track_output_filename,
        final_output_directory,
        output_file_name_suffix,

        dNdeta_ApplyAlphaCorr,
        dNdeta_isTypeA,
        dNdeta_cut_INTTvtxZ,
        dNdeta_SelectedMbin
    );

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

    avgXY -> EndRun();

    std::cout<<"test test: done with in AvgVtxXY "<<std::endl;    

    sleep(30);

    system(Form("mv %s %s/completed", final_output_directory.c_str(), output_directory.c_str()));

    return 0;
}