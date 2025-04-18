std::pair<int,int> get_DeltaPhi_SingleBin(TH1D * hist_in, std::pair<double, double> range_in)
{
    int bin_min = hist_in -> FindBin(range_in.first);
    
    int bin_max = hist_in -> FindBin(range_in.second);
    bin_max = (fabs(hist_in -> GetXaxis() -> GetBinLowEdge(bin_max) - range_in.second) < 1e-9) ? bin_max - 1 : bin_max;

    std::cout<<std::endl;
    std::cout<<"Input range : "<<range_in.first<<", "<<range_in.second<<std::endl;
    std::cout<<"Final selected bins: "<<bin_min<<", "<<bin_max<<std::endl;
    std::cout<<"Output range : "<<hist_in -> GetXaxis() -> GetBinLowEdge(bin_min)<<", "<<hist_in -> GetXaxis() -> GetBinUpEdge(bin_max)<<std::endl;

    return std::make_pair(bin_min, bin_max);
}

double get_EvtCount(TH2D * hist_in, int centrality_bin_in)
{
    double count = 0;

    // note : X : vtxZ
    // note : Y : centrality

    
    if (true) {
        for (int xi = 3; xi <= 6; xi++){ // note : vtxZ bin

            // if (vtxZ_index_map.find(xi - 1) == vtxZ_index_map.end()){
            //     continue;
            // }

            count += hist_in -> GetBinContent(xi, centrality_bin_in + 1);
        }
    }

    return count;

}

// int DeltaPhi()
// {
//     // int Mbin = 0;
//     std::string mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed";
    
    
//     TCanvas * c = new TCanvas("c", "c", 800, 600);
//     c -> Print("DeltaPhi.pdf(");

//     TF1 * line = new TF1("line", "pol1",-10,10);
//     line -> SetParameters(0,1);
//     line -> SetLineStyle(2);
//     line -> SetLineWidth(1);
//     line -> SetLineColor(kRed);

//     vector<TGraph*> eta_correlation; eta_correlation.clear();

//     for (int i = 0; i < 15; i++) // note : 0 to 14, the Mbin;
//     {
//         std::string Mbin_folder_name = Form("vtxZ_-10_10cm_MBin%d",i);
//         std::string data_file = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin%d_00054280_00000_DeltaPhi.root",i);
//         std::string MC_file = Form("MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin%d_00001_DeltaPhi.root",i);

//         TFile * file_data = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), data_file.c_str()));
//         TFile * file_MC = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), MC_file.c_str()));

//         for (int eta_i = 7; eta_i <= 19; eta_i++)
//         {
//             std::string hist_name = Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_i);
//             TH1D * data = (TH1D*)file_data->Get(hist_name.c_str());
//             TH1D * MC = (TH1D*)file_MC->Get(hist_name.c_str());

//             data->Scale(1.0/data->Integral());
//             MC->Scale(1.0/MC->Integral());

//             eta_correlation.push_back(new TGraph());
//             eta_correlation.back()->SetTitle(Form(";Data;MC"));
//             eta_correlation.back()->SetMarkerStyle(20);
//             eta_correlation.back()->SetMarkerSize(0.5);
//             eta_correlation.back()->SetMarkerColor(eta_i-7 + 1);
//             eta_correlation.back()->SetLineColor(eta_i-7 + 1);

//             for (int delta_phi_i = 0; delta_phi_i < 27; delta_phi_i++){
//                 std::pair<double,double> delta_phi_range = {
//                     -0.018 - 0.002 * delta_phi_i,
//                     0.018 + 0.002 * delta_phi_i
//                 };
//                 std::pair<int,int> delta_phi_bin = get_DeltaPhi_SingleBin(data, delta_phi_range);

//                 eta_correlation.back()->SetPoint(
//                     delta_phi_i, 
//                     data->Integral(delta_phi_bin.first, delta_phi_bin.second),
//                     MC->Integral(delta_phi_bin.first, delta_phi_bin.second)
//                 );
//             }

//             eta_correlation.back()->GetYaxis()->SetRangeUser(0.7,1);
//             eta_correlation.back()->GetXaxis()->SetLimits(0.7,1);

//             c->cd();
//             if (eta_i == 7) eta_correlation.back()->Draw("APL");
//             else eta_correlation.back()->Draw("PL same");
//         }

//         line -> Draw("lsame");
//         c -> Print("DeltaPhi.pdf");
//         c -> Clear();
//         eta_correlation.clear();
//     }

//     c -> Print("DeltaPhi.pdf)");

//     return 0;
// }

int DeltaPhi()
{
    std::string mother_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed";
    
    double nominal_deltaphi = 0.026;
    double vary_deltaphi = 0.04;
    
    TCanvas * c1 = new TCanvas("c1", "c1", 800, 1000);
    c1 -> Print("DeltaPhi_dNdEta.pdf(");

    TF1 * line = new TF1("line", "pol1",-10,10);
    line -> SetParameters(0,1);
    line -> SetLineStyle(2);
    line -> SetLineWidth(1);
    line -> SetLineColor(kRed);

    vector<TH1D*> h1D_nominal_data_RecoTracklet_vec; h1D_nominal_data_RecoTracklet_vec.clear();
    vector<TH1D*> h1D_nominal_MC_RecoTracklet_vec; h1D_nominal_MC_RecoTracklet_vec.clear();
    vector<TH1D*> h1D_nominal_correction_vec; h1D_nominal_correction_vec.clear(); 

    vector<TH1D*> h1D_vary_data_RecoTracklet_vec; h1D_vary_data_RecoTracklet_vec.clear();
    vector<TH1D*> h1D_vary_MC_RecoTracklet_vec; h1D_vary_MC_RecoTracklet_vec.clear();
    vector<TH1D*> h1D_vary_correction_vec; h1D_vary_correction_vec.clear(); 

    for (int i = 0; i < 15; i++) // note : 0 to 14, the Mbin
    {
        std::string Mbin_folder_name = Form("vtxZ_-10_10cm_MBin%d",i);
        
        std::string data_file = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin%d_00054280_00000_DeltaPhi.root",i);
        std::string data_event_file = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin%d_00054280_00000_dNdEta.root",i);

        std::string MC_file = Form("MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin%d_00001_DeltaPhi.root",i);
        std::string MC_event_file = Form("MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin%d_00001_dNdEta.root",i);
        std::string TruthHadron_file = Form("MC_PreparedNdEtaEach_AllSensor_VtxZ10_Mbin%d_00001_dNdEta.root",i);

        std::string recoevt_hist_name = "h2D_RecoEvtCount_vtxZCentrality";
        std::string TruthHadron_hist_name = "h1D_TruedNdEta";
        std::string data_dNdEta = "h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC";

        TFile * file_data = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), data_file.c_str()));
        TFile * file_data_event = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), data_event_file.c_str()));
        TH2D * data_recoevt = (TH2D*)file_data_event->Get(recoevt_hist_name.c_str());
        TH1D * h1D_data_dNdEta = (TH1D*)file_data_event->Get(data_dNdEta.c_str());

        TFile * file_MC = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), MC_file.c_str()));
        TFile * file_MC_event = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), MC_event_file.c_str()));
        TH2D * MC_recoevt = (TH2D*)file_MC_event->Get(recoevt_hist_name.c_str());

        TFile * file_TruthHadron = TFile::Open(Form("%s/%s/Folder_BaseLine/Run_0/completed/%s", mother_directory.c_str(), Mbin_folder_name.c_str(), TruthHadron_file.c_str()));
        TH1D * TruthHadron = (TH1D*)file_TruthHadron->Get(TruthHadron_hist_name.c_str());


        h1D_nominal_data_RecoTracklet_vec.push_back(new TH1D(Form("h1D_nominal_data_RecoTracklet_Mbin%d",i), "", 27, -2.7, 2.7));
        h1D_nominal_MC_RecoTracklet_vec.push_back(new TH1D(Form("h1D_nominal_MC_RecoTracklet_Mbin%d",i), "", 27, -2.7, 2.7));
        
        h1D_vary_data_RecoTracklet_vec.push_back(new TH1D(Form("h1D_vary_data_RecoTracklet_Mbin%d",i), "", 27, -2.7, 2.7));
        h1D_vary_MC_RecoTracklet_vec.push_back(new TH1D(Form("h1D_vary_MC_RecoTracklet_Mbin%d",i), "", 27, -2.7, 2.7));
        

        for (int eta_i = 7; eta_i <= 19; eta_i++)
        {
            std::string hist_name = Form("h1D_RotatedBkg_DeltaPhi_Signal_Eta%d", eta_i);
            TH1D * data = (TH1D*)file_data->Get(hist_name.c_str());
            TH1D * MC = (TH1D*)file_MC->Get(hist_name.c_str());

            std::pair<int,int> nominal_delta_phi_bin = get_DeltaPhi_SingleBin(data, {-1*nominal_deltaphi, nominal_deltaphi});
            std::pair<int,int> vary_delta_phi_bin = get_DeltaPhi_SingleBin(data, {-1*vary_deltaphi, vary_deltaphi});

            h1D_nominal_data_RecoTracklet_vec.back() -> SetBinContent(eta_i + 1, data -> Integral(nominal_delta_phi_bin.first, nominal_delta_phi_bin.second));
            h1D_nominal_MC_RecoTracklet_vec.back() -> SetBinContent(eta_i + 1, MC -> Integral(nominal_delta_phi_bin.first, nominal_delta_phi_bin.second));

            h1D_vary_data_RecoTracklet_vec.back() -> SetBinContent(eta_i + 1, data -> Integral(vary_delta_phi_bin.first, vary_delta_phi_bin.second));
            h1D_vary_MC_RecoTracklet_vec.back() -> SetBinContent(eta_i + 1, MC -> Integral(vary_delta_phi_bin.first, vary_delta_phi_bin.second));
        }

        h1D_nominal_data_RecoTracklet_vec.back() -> Scale(1. / (get_EvtCount(data_recoevt,i) * h1D_nominal_data_RecoTracklet_vec.back() -> GetBinWidth(1)));
        h1D_nominal_MC_RecoTracklet_vec.back() -> Scale(1. / (get_EvtCount(MC_recoevt, i) * h1D_nominal_MC_RecoTracklet_vec.back() -> GetBinWidth(1)));

        h1D_vary_data_RecoTracklet_vec.back() -> Scale(1. / (get_EvtCount(data_recoevt,i) * h1D_vary_data_RecoTracklet_vec.back() -> GetBinWidth(1)));
        h1D_vary_MC_RecoTracklet_vec.back() -> Scale(1. / (get_EvtCount(MC_recoevt, i) * h1D_vary_MC_RecoTracklet_vec.back() -> GetBinWidth(1)));


        h1D_nominal_correction_vec.push_back((TH1D*) TruthHadron -> Clone(Form("h1D_nominal_correction_Mbin%d",i)));
        h1D_vary_correction_vec.push_back((TH1D*) TruthHadron -> Clone(Form("h1D_vary_correction_Mbin%d",i)));
        
        h1D_nominal_correction_vec.back() -> Divide(h1D_nominal_MC_RecoTracklet_vec.back(), TruthHadron);
        h1D_vary_correction_vec.back() -> Divide(h1D_vary_MC_RecoTracklet_vec.back(), TruthHadron);

        h1D_nominal_data_RecoTracklet_vec.back() -> Divide(h1D_nominal_correction_vec.back()); 
        h1D_vary_data_RecoTracklet_vec.back() -> Divide(h1D_vary_correction_vec.back());

        c1 -> cd();

        TPad * pad1 = new TPad("pad1", "pad1", 0, 0.4, 1, 1);
        pad1 -> Draw();
        pad1 -> cd();

        h1D_nominal_data_RecoTracklet_vec.back() -> SetLineColor(kBlack);
        h1D_vary_data_RecoTracklet_vec.back() -> SetMarkerStyle(20);
        h1D_vary_data_RecoTracklet_vec.back() -> SetMarkerColor(kRed);

        h1D_data_dNdEta -> SetMarkerStyle(20);
        h1D_data_dNdEta -> SetMarkerColor(kBlue);

        h1D_nominal_data_RecoTracklet_vec.back() -> Draw("hist");
        h1D_vary_data_RecoTracklet_vec.back() -> Draw("ep same");
        h1D_data_dNdEta -> Draw("ep same");

        c1 -> cd();
        TPad * pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.35);
        pad2 -> Draw();
        pad2 -> cd();
        
        TH1D * ratio = (TH1D*) h1D_vary_data_RecoTracklet_vec.back() -> Clone(Form("ratio_Mbin%d",i));
        ratio -> Sumw2(true);
        ratio -> Divide(h1D_nominal_data_RecoTracklet_vec.back());

        ratio -> SetMinimum(0.95);
        ratio -> SetMaximum(1.05);

        ratio -> SetMarkerSize(2);
        ratio -> SetMarkerStyle(20);
        ratio -> SetMarkerColor(kBlack);

        ratio -> Draw("hist text90");

        TLine * line = new TLine(-2.7, 1, 2.7, 1);
        line -> SetLineColor(kBlack);
        line -> SetLineStyle(2);

        line -> Draw("same");


        c1 -> Print("DeltaPhi_dNdEta.pdf");
    }

    c1 -> Print("DeltaPhi_dNdEta.pdf)");


    return 888;
}