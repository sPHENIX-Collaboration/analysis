int DataMcComp()
{
    TFile * file_in_data = TFile::Open("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run24NewCode_TrackHist/completed/Run24NewCode_dNdEta/completed/Data_PreparedNdEtaEach_ApplyAlphaCorr_AllSensor_VtxZ10_Mbin70_SecondRun_00054280_00000_dNdEta.root");
    TFile * file_in_mc = TFile::Open("/sphenix/user/ChengWei/sPH_dNdeta/Run24AuAuMC/Sim_Ntuple_HIJING_ana443_20241102/Run24NewCode_dNdEta/completed/MC_PreparedNdEtaEach_ApplyAlphaCorr_AllSensor_VtxZ10_Mbin70_SecondRun_00002_dNdEta.root");

    TH1D * data_h1D_FitBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_data->Get("h1D_FitBkg_RecoTrackletEtaPerEvt");
    TH1D * data_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_data->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * data_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_data->Get("h1D_FitBkg_RecoTrackletEtaPerEvtPostAC");
    TH1D * data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_data->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    data_h1D_FitBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    data_h1D_FitBkg_RecoTrackletEtaPerEvt -> SetLineColor(1);

    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(1);
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(1);

    data_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    data_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);

    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(1);
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(1);



    TH1D * MC_h1D_FitBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc->Get("h1D_FitBkg_RecoTrackletEtaPerEvt");
    TH1D * MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt = (TH1D*)file_in_mc->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    TH1D * MC_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc->Get("h1D_FitBkg_RecoTrackletEtaPerEvtPostAC");
    TH1D * MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC = (TH1D*)file_in_mc->Get("h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC");

    MC_h1D_FitBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(2);
    MC_h1D_FitBkg_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetMarkerColor(2);
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> SetLineColor(2);

    MC_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetMarkerColor(2);
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvtPostAC -> SetLineColor(2);

    

    TH1D * h1D_TruedNdEta = (TH1D*)file_in_mc->Get("h1D_TruedNdEta");
    h1D_TruedNdEta -> SetLineColor(3);


    TFile * file_out = new TFile("SecondRun_Mbin70.root", "RECREATE");
    TCanvas * c1 = new TCanvas("c1", "c1", 800, 600);

    c1 -> cd();
    data_h1D_FitBkg_RecoTrackletEtaPerEvt -> Draw("ep");
    MC_h1D_FitBkg_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_FitBkg_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("ep");
    MC_h1D_RotatedBkg_RecoTrackletEtaPerEvt -> Draw("hist same");
    c1 -> Write("h1D_RotatedBkg_RecoTrackletEtaPerEvt");
    c1 -> Clear();

    c1 -> cd();
    data_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC -> Draw("ep");
    MC_h1D_FitBkg_RecoTrackletEtaPerEvtPostAC -> Draw("hist same");
    h1D_TruedNdEta -> Draw("hist same");
    c1 -> Write("h1D_FitBkg_RecoTrackletEtaPerEvtPostAC");
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