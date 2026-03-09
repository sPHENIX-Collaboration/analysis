#include <iostream>
#include <TH2D.h>
#include <TH1D.h>
#include <TChain.h>
#include <TMath.h>
#include <TTree.h>
#include <TFile.h>
#include <sstream> //std::ostringstsream
#include <fstream> //std::ifstream
#include <iostream> //std::cout, std::endl
#include <cmath>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <string>
#include <set>
#include <TVector3.h>
#include <map>
#include <vector>
#include <TDatabasePDG.h>
#include <tuple>
#include <TProfile.h>
#include <TProfile2D.h>
#include "TRandom.h"
#include "TRandom3.h"
#include "unfold_Def.h"
#include "RooUnfold.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"

using namespace std;

void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et);
bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi);
void get_calibjet(float& calibjet_pt, float& calibjet_eta, float& calibjet_phi, float jet_pt, float jet_eta, float jet_phi, bool dijet, TF1* f_corr, float jes_para, float jer_para);
void get_truthjet(float& goodtruthjet_pt, float& goodtruthjet_eta, float& goodtruthjet_phi, float jet_pt, float jet_eta, float jet_phi, bool dijet);
void match_meas_truth(float meas_eta, float meas_phi, bool& matched, float truth_eta, float truth_phi, float jet_radius);
void fill_response_matrix(TH2D*& h_truth, TH2D*& h_meas, RooUnfoldResponse*& h_resp, TH2D*& h_fake, TH2D*& h_miss, RooUnfoldResponse*& h_count, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale);
void fill_response_matrix_full(TH2D*& h_truth, TH2D*& h_meas, RooUnfoldResponse*& h_resp, TH2D*& h_fake, TH2D*& h_miss, RooUnfoldResponse*& h_count, TH2D*& h_count_fake, TH2D*& h_count_miss, TH2D*& h_jetpt, TH2D*& h_caloet, TH2D*& h_nw_jetpt, TH2D*& h_nw_caloet, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale);
void fill_trim_response_matrix(RooUnfoldResponse*& h_resp, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale, const TMatrixD& counts_matrix, TH2D* h_counts_measured, TH2D* h_counts_truth, float trim_value);
void fill_reweighted_trim_response_matrix(RooUnfoldResponse*& h_resp, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale, const TMatrixD& counts_matrix, TH2D* h_counts_measured, TH2D* h_counts_truth, float trim_value, TH2D* h_prior_weights);

TF1* f_jer = new TF1("f_jer", "sqrt( 0.095077098*0.095077098 + (0.63134847*0.63134847/x) + (2.1664610*2.1664610/(x*x)) )", 0, 100);
TRandom3 randGen(1234);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                               //
//  Note: 5.13.25                                                                                                                //
//  This macro creates response matrices for 2D unfolding of leading jet pT and transverse region energy density                 //
//                                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void analysis_sim_dijet(std::string runtype = "mb", int start_seg = 0, int end_seg = 200, int iter = 1, bool clusters = true, bool emcal_clusters = false)  {
    
    ////////// General Set up //////////
    double weight_scale = 1.0, truthjet_pt_min = 0, truthjet_pt_max = 3000;
    string count_filename;
    if (runtype == "mb") {
        std::cout << "Not using MB dataset" << std::endl;
        //weight_scale = MB_scale;
        //truthjet_pt_min = 0;
        //truthjet_pt_max = 14;
    } else if (runtype == "jet10") {
        weight_scale = Jet10GeV_scale;
        truthjet_pt_min = 14;
        truthjet_pt_max = 22;
        if (iter > 1) count_filename = "analysis_sim_run21_output/output_dijet_sim_iter_1_jet10.root";
    } else if (runtype == "jet20") {
        weight_scale = Jet20GeV_scale;
        truthjet_pt_min = 22;
        truthjet_pt_max = 35;
        if (iter > 1) count_filename = "analysis_sim_run21_output/output_dijet_sim_iter_1_jet20.root";
    } else if (runtype == "jet30") {
        weight_scale = Jet30GeV_scale;
        truthjet_pt_min = 35;
        truthjet_pt_max = 52;
        if (iter > 1) count_filename = "analysis_sim_run21_output/output_dijet_sim_iter_1_jet30.root";
    } else if (runtype == "jet50") {
        weight_scale = Jet50GeV_scale;
        truthjet_pt_min = 52;
        truthjet_pt_max = 3000;
        if (iter > 1) count_filename = "analysis_sim_run21_output/output_dijet_sim_iter_1_jet50.root";
    } else {
        std::cout << "Unknown runtype" << std::endl;
        return;
    }

    TFile *f_count;
    RooUnfoldResponse* h_count[7];
    std::vector<std::string> count_matrix_names = {"h_respmatrix_calib_dijet_counts","h_respmatrix_calib_dijet_jesdown_counts","h_respmatrix_calib_dijet_jesup_counts",
    "h_respmatrix_calib_dijet_jerdown_counts","h_respmatrix_calib_dijet_jerup_counts","h_respmatrix_calib_dijet_half1_counts","h_respmatrix_calib_dijet_half2_counts"};
    std::vector<TMatrixD> counts_matrix;
    //counts_matrix.resize(7);
    TH2D* counts_measured[7]; 
    TH2D* counts_truth[7]; 
    if (iter > 1) {
        f_count = new TFile(count_filename.c_str(),"READ");
        for (int i = 0; i < count_matrix_names.size(); i++) {
            h_count[i] = dynamic_cast<RooUnfoldResponse*>(f_count->Get(count_matrix_names[i].c_str()));
            const TMatrixD& mat_ref = h_count[i]->Mresponse(false);
            TMatrixD mat_copy(mat_ref);
            counts_matrix.push_back(mat_copy);
            std::cout << "Matrix " << i << " size: " << counts_matrix[i].GetNrows() << " x " << counts_matrix[i].GetNcols() << std::endl;
            counts_measured[i] = dynamic_cast<TH2D*>(h_count[i]->Hmeasured());
            counts_truth[i] = dynamic_cast<TH2D*>(h_count[i]->Htruth());
        }
        f_count->Close();

        std::cout << "counts_measured: " << counts_measured[0]->GetEntries() << " counts_truth: " << counts_truth[0]->GetEntries() << std::endl;
    }

    TFile *f_reweight; 
    TH2D* weights[7][3];
    std::vector<std::string> pw_syst = {"weights_calib_dijet","weights_calib_dijet_jesdown","weights_calib_dijet_jesup",
    "weights_calib_dijet_jerdown","weights_calib_dijet_jerup","weights_calib_dijet_half1","weights_calib_dijet_half2"};
    std::vector<std::string> pw_trim = {"","_trim_5","_trim_10"};
    if (iter > 2) {
        f_reweight = new TFile("output_reweighted_respmatrix_run21_iter_2.root", "READ");
        for (int i = 0; i < pw_syst.size(); i++) {
            for (int j = 0; j < pw_trim.size(); j++) {
                weights[i][j] = dynamic_cast<TH2D*>(f_reweight->Get((pw_syst[i]+pw_trim[j]).c_str()));
            }
        }
    }

    ////////// Files //////////
    //TFile *f_out = new TFile(Form("analysis_sim_output/output_dijet_sim_iter_%d_%s_%d_%d.root", iter, runtype.c_str(), start_seg, end_seg), "RECREATE");
    TFile *f_out = new TFile(Form("analysis_sim_run21_output/output_dijet_sim_iter_%d_%s_%d_%d.root", iter, runtype.c_str(), start_seg, end_seg), "RECREATE");
    TChain chain("T");
    for (int i = start_seg; i < end_seg; ++i) {
        //chain.Add(Form("/sphenix/tg/tg01/jets/egm2153/UEinppOutput/sim_run22_%s_3sigma_output_%d.root", runtype.c_str(), i));
        chain.Add(Form("/sphenix/tg/tg01/jets/egm2153/UEinppOutput/sim_run21_%s_3sigma_output_%d.root", runtype.c_str(), i));
    }
    chain.SetBranchStatus("*", 0);

    float zvertex; chain.SetBranchStatus("zvtx", 1); chain.SetBranchAddress("zvtx", &zvertex);
    std::vector<float>* unsubjet_e = nullptr; chain.SetBranchStatus("e", 1); chain.SetBranchAddress("e", &unsubjet_e);
    std::vector<float>* unsubjet_pt = nullptr; chain.SetBranchStatus("pt", 1); chain.SetBranchAddress("pt", &unsubjet_pt);
    std::vector<float>* unsubjet_eta = nullptr; chain.SetBranchStatus("eta", 1); chain.SetBranchAddress("eta", &unsubjet_eta);
    std::vector<float>* unsubjet_phi = nullptr; chain.SetBranchStatus("phi", 1); chain.SetBranchAddress("phi", &unsubjet_phi);
    std::vector<float>* unsubjet_emcal_calo_e = nullptr; chain.SetBranchStatus("jetEmcalE", 1); chain.SetBranchAddress("jetEmcalE", &unsubjet_emcal_calo_e);
    std::vector<float>* unsubjet_ihcal_calo_e = nullptr; chain.SetBranchStatus("jetIhcalE", 1); chain.SetBranchAddress("jetIhcalE", &unsubjet_ihcal_calo_e);
    std::vector<float>* unsubjet_ohcal_calo_e = nullptr; chain.SetBranchStatus("jetOhcalE", 1); chain.SetBranchAddress("jetOhcalE", &unsubjet_ohcal_calo_e);

    std::vector<float>* truthjet_e = nullptr; chain.SetBranchStatus("truthE", 1); chain.SetBranchAddress("truthE", &truthjet_e);
    std::vector<float>* truthjet_pt = nullptr; chain.SetBranchStatus("truthPt", 1); chain.SetBranchAddress("truthPt", &truthjet_pt);
    std::vector<float>* truthjet_eta = nullptr; chain.SetBranchStatus("truthEta", 1); chain.SetBranchAddress("truthEta", &truthjet_eta);
    std::vector<float>* truthjet_phi = nullptr; chain.SetBranchStatus("truthPhi", 1); chain.SetBranchAddress("truthPhi", &truthjet_phi);

    int truthpar_n = 0; chain.SetBranchStatus("truthpar_n", 1); chain.SetBranchAddress("truthpar_n",&truthpar_n);
    float truthpar_e[100000] = {0.0}; chain.SetBranchStatus("truthpar_e", 1); chain.SetBranchAddress("truthpar_e",truthpar_e);
    float truthpar_eta[100000] = {0.0}; chain.SetBranchStatus("truthpar_eta", 1); chain.SetBranchAddress("truthpar_eta",truthpar_eta);
    float truthpar_phi[100000] = {0.0}; chain.SetBranchStatus("truthpar_phi", 1); chain.SetBranchAddress("truthpar_phi",truthpar_phi);
    int truthpar_pid[100000] = {0}; chain.SetBranchStatus("truthpar_pid", 1); chain.SetBranchAddress("truthpar_pid",truthpar_pid);

    int emcaln = 0; float emcale[24576] = {0.0}; float emcaleta[24576] = {0.0}; float emcalphi[24576] = {0.0};
    int ihcaln = 0; float ihcale[1536] = {0.0}; float ihcaleta[1536] = {0.0}; float ihcalphi[1536] = {0.0};
    int ohcaln = 0; float ohcale[1536] = {0.0}; float ohcaleta[1536] = {0.0}; float ohcalphi[1536] = {0.0};
    int clsmult = 0; float cluster_e[10000] = {0.0}; float cluster_eta[10000] = {0.0}; float cluster_phi[10000] = {0.0};
    if (!clusters) {
        chain.SetBranchStatus("emcaln", 1); chain.SetBranchAddress("emcaln",&emcaln);
        chain.SetBranchStatus("emcale", 1); chain.SetBranchAddress("emcale",emcale);
        chain.SetBranchStatus("emcaleta", 1); chain.SetBranchAddress("emcaleta",emcaleta);
        chain.SetBranchStatus("emcalphi", 1); chain.SetBranchAddress("emcalphi",emcalphi);
        chain.SetBranchStatus("ihcaln", 1); chain.SetBranchAddress("ihcaln",&ihcaln);
        chain.SetBranchStatus("ihcale", 1); chain.SetBranchAddress("ihcale",ihcale);
        chain.SetBranchStatus("ihcaleta", 1); chain.SetBranchAddress("ihcaleta",ihcaleta);
        chain.SetBranchStatus("ihcalphi", 1); chain.SetBranchAddress("ihcalphi",ihcalphi);
        chain.SetBranchStatus("ohcaln", 1); chain.SetBranchAddress("ohcaln",&ohcaln);
        chain.SetBranchStatus("ohcale", 1); chain.SetBranchAddress("ohcale",ohcale);
        chain.SetBranchStatus("ohcaleta", 1); chain.SetBranchAddress("ohcaleta",ohcaleta);
        chain.SetBranchStatus("ohcalphi", 1); chain.SetBranchAddress("ohcalphi",ohcalphi);
    } else if (clusters && !emcal_clusters) {
        chain.SetBranchStatus("clsmult", 1); chain.SetBranchAddress("clsmult",&clsmult);
        chain.SetBranchStatus("cluster_e", 1); chain.SetBranchAddress("cluster_e",cluster_e);
        chain.SetBranchStatus("cluster_eta", 1); chain.SetBranchAddress("cluster_eta",cluster_eta);
        chain.SetBranchStatus("cluster_phi", 1); chain.SetBranchAddress("cluster_phi",cluster_phi);
    } else {
        chain.SetBranchStatus("emcal_clsmult", 1); chain.SetBranchAddress("emcal_clsmult",&clsmult);
        chain.SetBranchStatus("emcal_cluster_e", 1); chain.SetBranchAddress("emcal_cluster_e",cluster_e);
        chain.SetBranchStatus("emcal_cluster_eta", 1); chain.SetBranchAddress("emcal_cluster_eta",cluster_eta);
        chain.SetBranchStatus("emcal_cluster_phi", 1); chain.SetBranchAddress("emcal_cluster_phi",cluster_phi);
    }

    ////////// JES func //////////
    TFile *corrFile = new TFile("JES_Calib_Default.root", "READ");
    if (!corrFile) {
        std::cout << "Error: cannot open JES_Calib_Default.root" << std::endl;
        return;
    }
    TF1 *f_corr = (TF1*)corrFile->Get("JES_Calib_Default_Func");
    if (!f_corr) {
        std::cout << "Error: cannot open f_corr" << std::endl;
        return;
    }

    ////////// Histograms //////////
    TH1D* h_zvertex = new TH1D("h_zvertex", ";Z-vertex [cm]", 400, -200, 200);
    TH1D* h_deltaphi_record = new TH1D("h_deltaphi_record","",125,-2*M_PI,2*M_PI);
    TH1D* h_xj_record = new TH1D("h_xj_record","",20,0,1);
    TH1D* h_lead_spectra_record = new TH1D("h_lead_spectra_record",";p_{T} [GeV]", 1000, 0, 100);
    TH1D* h_sub_spectra_record = new TH1D("h_sub_spectra_record",";p_{T} [GeV]", 1000, 0, 100);
    TH1D* h_truth_deltaphi_record = new TH1D("h_truth_deltaphi_record","",125,-2*M_PI,2*M_PI);
    TH1D* h_truth_xj_record = new TH1D("h_truth_xj_record","",20,0,1);
    TH1D* h_lead_truth_spectra_record = new TH1D("h_lead_truth_spectra_record",";p_{T} [GeV]", 1000, 0, 100);
    TH1D* h_sub_truth_spectra_record = new TH1D("h_sub_truth_spectra_record",";p_{T} [GeV]", 1000, 0, 100);
    TH1D *h_et_transverse_record = new TH1D("h_et_transverse_record", ";#SigmaE_{T} [GeV]", 7000, -20, 50);
    TH1D* h_et_truth_transverse_record = new TH1D("h_et_truth_transverse_record", ";#SigmaE_{T} [GeV]", 7000, -20, 50);
    TH1D *h_nw_et_transverse_record = new TH1D("h_nw_et_transverse_record", ";#SigmaE_{T} [GeV]", 7000, -20, 50);
    TH1D* h_nw_et_truth_transverse_record = new TH1D("h_nw_et_truth_transverse_record", ";#SigmaE_{T} [GeV]", 7000, -20, 50);
    TH2D* h_ue_pt_transverse_record = new TH2D("h_ue_pt_transverse_record","", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_ue_pt_truth_transverse_record = new TH2D("h_ue_pt_truth_transverse_record","",truthnpt, truthptbins, truthnet, truthetbins);

    TH2D* h_truth_calib_dijet = new TH2D("h_truth_calib_dijet", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Truth} [GeV]", truthnpt, truthptbins, truthnet, truthetbins);
    TH2D* h_measure_calib_dijet = new TH2D("h_measure_calib_dijet", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_fake_calib_dijet = new TH2D("h_fake_calib_dijet", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_miss_calib_dijet = new TH2D("h_miss_calib_dijet", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);
    TH2D* h_counts_fake_calib_dijet = new TH2D("h_counts_fake_calib_dijet", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_counts_miss_calib_dijet = new TH2D("h_counts_miss_calib_dijet", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);
    TH2D* h_jet_pt_respmatrix_calib_dijet = new TH2D("h_jet_pt_respmatrix_calib_dijet",";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
    TH2D* h_calo_et_respmatrix_calib_dijet = new TH2D("h_calo_et_respmatrix_calib_dijet",";#SigmaE_{T}^{Reco} [GeV];#SigmaE_{T}^{Truth} [GeV]", calibnet, calibetbins, truthnet, truthetbins);
    TH2D* h_nw_jet_pt_respmatrix_calib_dijet = new TH2D("h_nw_jet_pt_respmatrix_calib_dijet",";p_{T}^{Calib jet} [GeV];p_{T}^{Truth jet} [GeV]", calibnpt, calibptbins, truthnpt, truthptbins);
    TH2D* h_nw_calo_et_respmatrix_calib_dijet = new TH2D("h_nw_calo_et_respmatrix_calib_dijet",";#SigmaE_{T}^{Reco} [GeV];#SigmaE_{T}^{Truth} [GeV]", calibnet, calibetbins, truthnet, truthetbins);

    TH2D* h_truth_calib_dijet_jesdown = new TH2D("h_truth_calib_dijet_jesdown", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Truth} [GeV]", truthnpt, truthptbins, truthnet, truthetbins);
    TH2D* h_measure_calib_dijet_jesdown = new TH2D("h_measure_calib_dijet_jesdown", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_fake_calib_dijet_jesdown = new TH2D("h_fake_calib_dijet_jesdown", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_miss_calib_dijet_jesdown = new TH2D("h_miss_calib_dijet_jesdown", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);
    TH2D* h_truth_calib_dijet_jesup = new TH2D("h_truth_calib_dijet_jesup", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Truth} [GeV]", truthnpt, truthptbins, truthnet, truthetbins);
    TH2D* h_measure_calib_dijet_jesup = new TH2D("h_measure_calib_dijet_jesup", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_fake_calib_dijet_jesup = new TH2D("h_fake_calib_dijet_jesup", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_miss_calib_dijet_jesup = new TH2D("h_miss_calib_dijet_jesup", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);
    
    TH2D* h_truth_calib_dijet_jerdown = new TH2D("h_truth_calib_dijet_jerdown", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Truth} [GeV]", truthnpt, truthptbins, truthnet, truthetbins);
    TH2D* h_measure_calib_dijet_jerdown = new TH2D("h_measure_calib_dijet_jerdown", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_fake_calib_dijet_jerdown = new TH2D("h_fake_calib_dijet_jerdown", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_miss_calib_dijet_jerdown = new TH2D("h_miss_calib_dijet_jerdown", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);
    TH2D* h_truth_calib_dijet_jerup = new TH2D("h_truth_calib_dijet_jerup", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Truth} [GeV]", truthnpt, truthptbins, truthnet, truthetbins);
    TH2D* h_measure_calib_dijet_jerup = new TH2D("h_measure_calib_dijet_jerup", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_fake_calib_dijet_jerup = new TH2D("h_fake_calib_dijet_jerup", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_miss_calib_dijet_jerup = new TH2D("h_miss_calib_dijet_jerup", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);
    
    TH2D* h_truth_calib_dijet_half1 = new TH2D("h_truth_calib_dijet_half1", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Truth} [GeV]", truthnpt, truthptbins, truthnet, truthetbins);
    TH2D* h_measure_calib_dijet_half1 = new TH2D("h_measure_calib_dijet_half1", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_fake_calib_dijet_half1 = new TH2D("h_fake_calib_dijet_half1", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_miss_calib_dijet_half1 = new TH2D("h_miss_calib_dijet_half1", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);
    TH2D* h_truth_calib_dijet_half2 = new TH2D("h_truth_calib_dijet_half2", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Truth} [GeV]", truthnpt, truthptbins, truthnet, truthetbins);
    TH2D* h_measure_calib_dijet_half2 = new TH2D("h_measure_calib_dijet_half2", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_fake_calib_dijet_half2 = new TH2D("h_fake_calib_dijet_half2", ";p_{T}^{Calib jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", calibnpt, calibptbins, calibnet, calibetbins);
    TH2D* h_miss_calib_dijet_half2 = new TH2D("h_miss_calib_dijet_half2", ";p_{T}^{Truth jet} [GeV];#SigmaE_{T}^{Reco} [GeV]", truthnpt, truthptbins, calibnet, calibetbins);

    std::vector<int> trim_val = {0, 5, 10};
    std::vector<std::string> respmatrix_tags = {"", "_counts", "_trim_5", "_trim_10", "_reweight", "_reweight_trim_5", "_reweight_trim_10"};
    RooUnfoldResponse* h_respmatrix_calib_dijet[7];
    RooUnfoldResponse* h_respmatrix_calib_dijet_jesdown[7];
    RooUnfoldResponse* h_respmatrix_calib_dijet_jesup[7];
    RooUnfoldResponse* h_respmatrix_calib_dijet_jerdown[7];
    RooUnfoldResponse* h_respmatrix_calib_dijet_jerup[7];
    RooUnfoldResponse* h_respmatrix_calib_dijet_half1[7];
    RooUnfoldResponse* h_respmatrix_calib_dijet_half2[7];

    for (int i = 0; i < 7; i++) {
        h_respmatrix_calib_dijet[i] = new RooUnfoldResponse(("h_respmatrix_calib_dijet" + respmatrix_tags[i]).c_str(),""); h_respmatrix_calib_dijet[i]->Setup(h_measure_calib_dijet, h_truth_calib_dijet);
        h_respmatrix_calib_dijet_jesdown[i] = new RooUnfoldResponse(("h_respmatrix_calib_dijet_jesdown" + respmatrix_tags[i]).c_str(),""); h_respmatrix_calib_dijet_jesdown[i]->Setup(h_measure_calib_dijet_jesdown, h_truth_calib_dijet_jesdown);
        h_respmatrix_calib_dijet_jesup[i] = new RooUnfoldResponse(("h_respmatrix_calib_dijet_jesup" + respmatrix_tags[i]).c_str(),""); h_respmatrix_calib_dijet_jesup[i]->Setup(h_measure_calib_dijet_jesup, h_truth_calib_dijet_jesup);
        h_respmatrix_calib_dijet_jerdown[i] = new RooUnfoldResponse(("h_respmatrix_calib_dijet_jerdown" + respmatrix_tags[i]).c_str(),""); h_respmatrix_calib_dijet_jerdown[i]->Setup(h_measure_calib_dijet_jerdown, h_truth_calib_dijet_jerdown);
        h_respmatrix_calib_dijet_jerup[i] = new RooUnfoldResponse(("h_respmatrix_calib_dijet_jerup" + respmatrix_tags[i]).c_str(),""); h_respmatrix_calib_dijet_jerup[i]->Setup(h_measure_calib_dijet_jerup, h_truth_calib_dijet_jerup);
        h_respmatrix_calib_dijet_half1[i] = new RooUnfoldResponse(("h_respmatrix_calib_dijet_half1" + respmatrix_tags[i]).c_str(),""); h_respmatrix_calib_dijet_half1[i]->Setup(h_measure_calib_dijet_half1, h_truth_calib_dijet_half1);
        h_respmatrix_calib_dijet_half2[i] = new RooUnfoldResponse(("h_respmatrix_calib_dijet_half2" + respmatrix_tags[i]).c_str(),""); h_respmatrix_calib_dijet_half2[i]->Setup(h_measure_calib_dijet_half2, h_truth_calib_dijet_half2);
    }
    
    
    ////////// Event Loop //////////
    std::cout << "Data analysis started." << std::endl;
    Long64_t nEntries = chain.GetEntries();
    std::cout << "Total number of events: " << nEntries << std::endl;
    
    // Event variables setup.
    bool reco_dijet = false; bool truth_dijet = false;
    float goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi;
    float calibjet_pt_dijet, calibjet_eta_dijet, calibjet_phi_dijet;
    float calibjet_pt_dijet_jesdown, calibjet_eta_dijet_jesdown, calibjet_phi_dijet_jesdown, calibjet_pt_dijet_jesup, calibjet_eta_dijet_jesup, calibjet_phi_dijet_jesup;
    float calibjet_pt_dijet_jerdown, calibjet_eta_dijet_jerdown, calibjet_phi_dijet_jerdown, calibjet_pt_dijet_jerup, calibjet_eta_dijet_jerup, calibjet_phi_dijet_jerup;
    bool calibjet_matched_dijet, calibjet_matched_dijet_jesdown, calibjet_matched_dijet_jesup, calibjet_matched_dijet_jerdown, calibjet_matched_dijet_jerup;
    
    for (Long64_t entry = 0; entry < nEntries; ++entry) {
    //for (Long64_t entry = 0; entry < 20; ++entry) {
        if (entry % 1000 == 0) cout << "event " << entry << endl;
        chain.GetEntry(entry);

        // Z-vertex cut.
        if (isnan(zvertex)) { continue; }
        if (fabs(zvertex) > 30) { continue; }

        // Fill z-vertex histogram.
        h_zvertex->Fill(zvertex);

        //////////////////////////// SETUP JET VARIABLES FOR UNFOLDING ////////////////////////////

        // indices to find leading and subleading jets 
        int ind_truth_lead = -1; int ind_truth_sub = -1;
        int ind_lead = -1; int ind_sub = -1;
        float lead_e = 0; float truthlead_e = 0;
        truth_dijet = false; reco_dijet = false;

        //std::cout << "reco jets " << unsubjet_pt->size() << " truth jets " << truthjet_pt->size() << std::endl;

        // if both nreco jets < 2 and nTruthJet jets < 2, discard event
        if (unsubjet_pt->size() < 2 && truthjet_pt->size() < 2) {
            //std::cout << std::endl;
            continue;
        }

        //std::cout << "reco jets " << unsubjet_pt->size() << " truth jets " << truthjet_pt->size() << std::endl;

        // find reco leading and subleading jets if nreco jets >= 2
        TVector3 lead, sub;
        if (unsubjet_pt->size() >= 2) {
            get_leading_subleading_jet(ind_lead, ind_sub, unsubjet_pt);  
            lead.SetPtEtaPhi(unsubjet_pt->at(ind_lead), unsubjet_eta->at(ind_lead), unsubjet_phi->at(ind_lead));
            sub.SetPtEtaPhi(unsubjet_pt->at(ind_sub), unsubjet_eta->at(ind_sub), unsubjet_phi->at(ind_sub));
            lead_e = unsubjet_e->at(ind_lead);
            if (unsubjet_e->at(ind_sub)/unsubjet_e->at(ind_lead) > 0.3 && match_leading_subleading_jet(unsubjet_phi->at(ind_lead), unsubjet_phi->at(ind_sub))) {
                reco_dijet = true;
            } else {
                reco_dijet = false;
            }
        } else {
            lead.SetPtEtaPhi(0,0,0);
            sub.SetPtEtaPhi(0,0,0);
            lead_e = 0;
            reco_dijet = false;
        }

        //find truth leading and subleading jets if ntruth jets >= 2
        TVector3 truthlead, truthsub;
        if (truthjet_pt->size() >= 2) {
            get_leading_subleading_jet(ind_truth_lead, ind_truth_sub, truthjet_pt);
            truthlead.SetPtEtaPhi(truthjet_pt->at(ind_truth_lead), truthjet_eta->at(ind_truth_lead), truthjet_phi->at(ind_truth_lead));
            truthsub.SetPtEtaPhi(truthjet_pt->at(ind_truth_sub), truthjet_eta->at(ind_truth_sub), truthjet_phi->at(ind_truth_sub));
            truthlead_e = truthjet_e->at(ind_truth_lead);
            // if both leading truth jet outside of trigger range, discard event
            if (truthlead.Pt() < truthjet_pt_min || truthlead.Pt() > truthjet_pt_max) {
                continue;
            } else if (truthjet_e->at(ind_truth_sub)/truthjet_e->at(ind_truth_lead) > 0.3 && match_leading_subleading_jet(truthjet_phi->at(ind_truth_lead), truthjet_phi->at(ind_truth_sub))) {
                truth_dijet = true;
            } else {
                truth_dijet = false;
            }
        } else {
            truthlead.SetPtEtaPhi(0,0,0);
            truthsub.SetPtEtaPhi(0,0,0);
            truthlead_e = 0;
            truth_dijet = false;
        }

        // if both reco and truth don't have a dijet, discard event
        if (!reco_dijet && !truth_dijet) { continue; }

        //std::cout << "Reco lead: pt " << lead.Pt() << " e " << lead_e << " eta " << lead.Eta() << " phi " << lead.Phi()  << " dijet " << reco_dijet << std::endl;
        //std::cout << "Truth lead: pt " << truthlead.Pt() << " truth e " << truthlead_e << " truth eta " << truthlead.Eta() << " truth phi " << truthlead.Phi() << " dijet " << truth_dijet << std::endl;

        get_truthjet(goodtruthjet_pt, goodtruthjet_eta, goodtruthjet_phi, truthlead.Pt(), truthlead.Eta(), truthlead.Phi(), truth_dijet);
        get_calibjet(calibjet_pt_dijet, calibjet_eta_dijet, calibjet_phi_dijet, lead.Pt(), lead.Eta(), lead.Phi(), reco_dijet, f_corr, 1, 0.1);
        get_calibjet(calibjet_pt_dijet_jesdown, calibjet_eta_dijet_jesdown, calibjet_phi_dijet_jesdown, lead.Pt(), lead.Eta(), lead.Phi(), reco_dijet, f_corr, 0.94, 0.1);
        get_calibjet(calibjet_pt_dijet_jesup, calibjet_eta_dijet_jesup, calibjet_phi_dijet_jesup, lead.Pt(), lead.Eta(), lead.Phi(), reco_dijet, f_corr, 1.06, 0.1);
        get_calibjet(calibjet_pt_dijet_jerdown, calibjet_eta_dijet_jerdown, calibjet_phi_dijet_jerdown, lead.Pt(), lead.Eta(), lead.Phi(), reco_dijet, f_corr, 1, 0.05);
        get_calibjet(calibjet_pt_dijet_jerup, calibjet_eta_dijet_jerup, calibjet_phi_dijet_jerup, lead.Pt(), lead.Eta(), lead.Phi(), reco_dijet, f_corr, 1, 0.15);
        
        //std::cout << "Good reco lead: pt " << calibjet_pt_dijet << " eta " << calibjet_eta_dijet << " phi " << calibjet_phi_dijet << std::endl;
        //std::cout << "Good truth lead: pt " << goodtruthjet_pt << " truth eta " << goodtruthjet_eta << " truth phi " << goodtruthjet_phi << std::endl;
        
        match_meas_truth(calibjet_eta_dijet, calibjet_phi_dijet, calibjet_matched_dijet, goodtruthjet_eta, goodtruthjet_phi, jet_radius);
        match_meas_truth(calibjet_eta_dijet_jesdown, calibjet_phi_dijet_jesdown, calibjet_matched_dijet_jesdown, goodtruthjet_eta, goodtruthjet_phi, jet_radius);
        match_meas_truth(calibjet_eta_dijet_jesup, calibjet_phi_dijet_jesup, calibjet_matched_dijet_jesup, goodtruthjet_eta, goodtruthjet_phi, jet_radius);
        match_meas_truth(calibjet_eta_dijet_jerdown, calibjet_phi_dijet_jerdown, calibjet_matched_dijet_jerdown, goodtruthjet_eta, goodtruthjet_phi, jet_radius);
        match_meas_truth(calibjet_eta_dijet_jerup, calibjet_phi_dijet_jerup, calibjet_matched_dijet_jerup, goodtruthjet_eta, goodtruthjet_phi, jet_radius);
        
        //std::cout << "Match " << calibjet_matched_dijet << std::endl;
    
        //////////////////////////// SETUP UE VARIABLES FOR UNFOLDING //////////////////////////////

        // find reco ET information 
        float et_transverse = 0;
        if (!clusters) {
            for (int i = 0; i < emcaln; i++) {
                float dphi = get_dphi(lead.Phi(),emcalphi[i]);
                if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += emcale[i]/cosh(emcaleta[i]); } 
            }
            for (int i = 0; i < ihcaln; i++) {
                float dphi = get_dphi(lead.Phi(),ihcalphi[i]);
                if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += ihcale[i]/cosh(ihcaleta[i]); } 
            }
            for (int i = 0; i < ohcaln; i++) {
                float dphi = get_dphi(lead.Phi(),ohcalphi[i]);
                if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += ohcale[i]/cosh(ohcaleta[i]); } 
            }
        } else {
            for (int i = 0; i < clsmult; i++) {
                float dphi = get_dphi(lead.Phi(),cluster_phi[i]);
                if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { et_transverse += cluster_e[i]/cosh(cluster_eta[i]); }
            }
        }

        // find truth ET information
        float truth_et_transverse = 0;
        for (int i = 0; i < truthpar_n; i++) {
            if (fabs(truthpar_eta[i]) > 1.1) { continue; }
            if ((truthpar_pid[i] == 22 || truthpar_pid[i] == 111) && fabs(truthpar_e[i]) < 0.2) { continue; }
            else if (fabs(truthpar_e[i]) < 0.2) { continue; } // edited from 0.5 to 0.2
            float dphi = get_dphi(truthlead.Phi(),truthpar_phi[i]);
            if (fabs(dphi) > M_PI/3.0 && fabs(dphi) < (2.0*M_PI)/3.0) { truth_et_transverse += truthpar_e[i]/cosh(truthpar_eta[i]); } 
        }

        //std::cout << "Truth ET " << truth_et_transverse << " Reco ET " << et_transverse << std::endl;
        //std::cout << std::endl;

        //////////////////////////// RECORD QA PLOTS FOR ALL EVENTS IN UNFOLDING PROCEDURE ////////////////////////////
        if (reco_dijet) {
            h_deltaphi_record->Fill(lead.DeltaPhi(sub), weight_scale);
            h_xj_record->Fill(sub.Pt()/lead.Pt(), weight_scale);
            h_lead_spectra_record->Fill(lead.Pt(), weight_scale);
            h_sub_spectra_record->Fill(sub.Pt(), weight_scale);
            h_nw_et_transverse_record->Fill(et_transverse);
            h_et_transverse_record->Fill(et_transverse, weight_scale);
            h_ue_pt_transverse_record->Fill(lead.Pt(),et_transverse, weight_scale);
        }
            
        if (truth_dijet) {
            h_truth_deltaphi_record->Fill(truthlead.DeltaPhi(truthsub), weight_scale);
            h_truth_xj_record->Fill(truthsub.Pt()/truthlead.Pt(), weight_scale);
            h_lead_truth_spectra_record->Fill(truthlead.Pt(), weight_scale);
            h_sub_truth_spectra_record->Fill(truthsub.Pt(), weight_scale);
            h_nw_et_truth_transverse_record->Fill(truth_et_transverse);
            h_et_truth_transverse_record->Fill(truth_et_transverse, weight_scale);
            h_ue_pt_truth_transverse_record->Fill(truthlead.Pt(),truth_et_transverse, weight_scale);
        }

        //////////////////////////// FILL RESPONSE MATRICES ////////////////////////////
        fill_response_matrix_full(h_truth_calib_dijet, h_measure_calib_dijet, h_respmatrix_calib_dijet[0], h_fake_calib_dijet, h_miss_calib_dijet, h_respmatrix_calib_dijet[1], h_counts_fake_calib_dijet, h_counts_miss_calib_dijet, h_jet_pt_respmatrix_calib_dijet, h_calo_et_respmatrix_calib_dijet, h_nw_jet_pt_respmatrix_calib_dijet, h_nw_calo_et_respmatrix_calib_dijet, calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale);
        fill_response_matrix(h_truth_calib_dijet_jesdown, h_measure_calib_dijet_jesdown, h_respmatrix_calib_dijet_jesdown[0], h_fake_calib_dijet_jesdown, h_miss_calib_dijet_jesdown, h_respmatrix_calib_dijet_jesdown[1], calibjet_pt_dijet_jesdown, calibjet_matched_dijet_jesdown, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale);
        fill_response_matrix(h_truth_calib_dijet_jesup, h_measure_calib_dijet_jesup, h_respmatrix_calib_dijet_jesup[0], h_fake_calib_dijet_jesup, h_miss_calib_dijet_jesup, h_respmatrix_calib_dijet_jesup[1], calibjet_pt_dijet_jesup, calibjet_matched_dijet_jesup, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale);
        fill_response_matrix(h_truth_calib_dijet_jerdown, h_measure_calib_dijet_jerdown, h_respmatrix_calib_dijet_jerdown[0], h_fake_calib_dijet_jerdown, h_miss_calib_dijet_jerdown, h_respmatrix_calib_dijet_jerdown[1], calibjet_pt_dijet_jerdown, calibjet_matched_dijet_jerdown, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale);
        fill_response_matrix(h_truth_calib_dijet_jerup, h_measure_calib_dijet_jerup, h_respmatrix_calib_dijet_jerup[0], h_fake_calib_dijet_jerup, h_miss_calib_dijet_jerup, h_respmatrix_calib_dijet_jerup[1], calibjet_pt_dijet_jerup, calibjet_matched_dijet_jerup, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale);
        if (entry % 2 == 0){ fill_response_matrix(h_truth_calib_dijet_half1, h_measure_calib_dijet_half1, h_respmatrix_calib_dijet_half1[0], h_fake_calib_dijet_half1, h_miss_calib_dijet_half1, h_respmatrix_calib_dijet_half1[1], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale); }   
        else { fill_response_matrix(h_truth_calib_dijet_half2, h_measure_calib_dijet_half2, h_respmatrix_calib_dijet_half2[0], h_fake_calib_dijet_half2, h_miss_calib_dijet_half2, h_respmatrix_calib_dijet_half2[1], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale); }
        
        if (iter > 1) {
            for (int i = 1; i < 3; i++) {
                fill_trim_response_matrix(h_respmatrix_calib_dijet[i+1], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[0], counts_measured[0], counts_truth[0], trim_val[i]);
                fill_trim_response_matrix(h_respmatrix_calib_dijet_jesdown[i+1], calibjet_pt_dijet_jesdown, calibjet_matched_dijet_jesdown, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[1], counts_measured[1], counts_truth[1], trim_val[i]);
                fill_trim_response_matrix(h_respmatrix_calib_dijet_jesup[i+1], calibjet_pt_dijet_jesup, calibjet_matched_dijet_jesup, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[2], counts_measured[2], counts_truth[2], trim_val[i]);
                fill_trim_response_matrix(h_respmatrix_calib_dijet_jerdown[i+1], calibjet_pt_dijet_jerdown, calibjet_matched_dijet_jerdown, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[3], counts_measured[3], counts_truth[3], trim_val[i]);
                fill_trim_response_matrix(h_respmatrix_calib_dijet_jerup[i+1], calibjet_pt_dijet_jerup, calibjet_matched_dijet_jerup, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[4], counts_measured[4], counts_truth[4], trim_val[i]);
                if (entry % 2 == 0) { fill_trim_response_matrix(h_respmatrix_calib_dijet_half1[i+1], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[5], counts_measured[5], counts_truth[5], trim_val[i]); }
                else { fill_trim_response_matrix(h_respmatrix_calib_dijet_half2[i+1], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[6], counts_measured[6], counts_truth[6], trim_val[i]); }
            }
        }

        if (iter > 2) {
            for (int i = 0; i < 3; i++) {
                fill_reweighted_trim_response_matrix(h_respmatrix_calib_dijet[i+4], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[0], counts_measured[0], counts_truth[0], trim_val[i], weights[0][i]);
                fill_reweighted_trim_response_matrix(h_respmatrix_calib_dijet_jesdown[i+4], calibjet_pt_dijet_jesdown, calibjet_matched_dijet_jesdown, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[1], counts_measured[1], counts_truth[1], trim_val[i], weights[1][i]);
                fill_reweighted_trim_response_matrix(h_respmatrix_calib_dijet_jesup[i+4], calibjet_pt_dijet_jesup, calibjet_matched_dijet_jesup, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[2], counts_measured[2], counts_truth[2], trim_val[i], weights[2][i]);
                fill_reweighted_trim_response_matrix(h_respmatrix_calib_dijet_jerdown[i+4], calibjet_pt_dijet_jerdown, calibjet_matched_dijet_jerdown, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[3], counts_measured[3], counts_truth[3], trim_val[i], weights[3][i]);
                fill_reweighted_trim_response_matrix(h_respmatrix_calib_dijet_jerup[i+4], calibjet_pt_dijet_jerup, calibjet_matched_dijet_jerup, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[4], counts_measured[4], counts_truth[4], trim_val[i], weights[4][i]);
                if (entry % 2 == 0) { fill_reweighted_trim_response_matrix(h_respmatrix_calib_dijet_half1[i+4], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[5], counts_measured[5], counts_truth[5], trim_val[i], weights[5][i]); }
                else { fill_reweighted_trim_response_matrix(h_respmatrix_calib_dijet_half2[i+4], calibjet_pt_dijet, calibjet_matched_dijet, goodtruthjet_pt, et_transverse, truth_et_transverse, weight_scale, counts_matrix[6], counts_measured[6], counts_truth[6], trim_val[i], weights[6][i]); }
            } 
        }
    }

    std::cout << "Writing histograms..." << std::endl;
    f_out->cd();

    h_zvertex->Write();
    h_deltaphi_record->Write(); h_xj_record->Write(); h_lead_spectra_record->Write(); h_sub_spectra_record->Write(); h_et_transverse_record->Write(); h_nw_et_transverse_record->Write(); h_ue_pt_transverse_record->Write(); 
    h_truth_deltaphi_record->Write(); h_truth_xj_record->Write(); h_lead_truth_spectra_record->Write(); h_sub_truth_spectra_record->Write(); h_et_truth_transverse_record->Write(); h_nw_et_truth_transverse_record->Write(); h_ue_pt_truth_transverse_record->Write(); 
    h_truth_calib_dijet->Write(); h_measure_calib_dijet->Write(); h_fake_calib_dijet->Write(); h_miss_calib_dijet->Write(); h_counts_fake_calib_dijet->Write(); h_counts_miss_calib_dijet->Write(); 
    h_jet_pt_respmatrix_calib_dijet->Write(); h_calo_et_respmatrix_calib_dijet->Write(); h_nw_jet_pt_respmatrix_calib_dijet->Write(); h_nw_calo_et_respmatrix_calib_dijet->Write();
    h_truth_calib_dijet_jesdown->Write(); h_measure_calib_dijet_jesdown->Write(); h_fake_calib_dijet_jesdown->Write(); h_miss_calib_dijet_jesdown->Write();
    h_truth_calib_dijet_jesup->Write(); h_measure_calib_dijet_jesup->Write(); h_fake_calib_dijet_jesup->Write(); h_miss_calib_dijet_jesup->Write();
    h_truth_calib_dijet_jerdown->Write(); h_measure_calib_dijet_jerdown->Write();  h_fake_calib_dijet_jerdown->Write(); h_miss_calib_dijet_jerdown->Write();
    h_truth_calib_dijet_jerup->Write(); h_measure_calib_dijet_jerup->Write(); h_fake_calib_dijet_jerup->Write(); h_miss_calib_dijet_jerup->Write();
    h_truth_calib_dijet_half1->Write(); h_measure_calib_dijet_half1->Write(); h_fake_calib_dijet_half1->Write(); h_miss_calib_dijet_half1->Write();
    h_truth_calib_dijet_half2->Write(); h_measure_calib_dijet_half2->Write(); h_fake_calib_dijet_half2->Write(); h_miss_calib_dijet_half2->Write();

    for (int i = 0; i < 7; i++) {
        h_respmatrix_calib_dijet[i]->Write();
        h_respmatrix_calib_dijet_jesdown[i]->Write();
        h_respmatrix_calib_dijet_jesup[i]->Write();
        h_respmatrix_calib_dijet_jerdown[i]->Write();
        h_respmatrix_calib_dijet_jerup[i]->Write();
        h_respmatrix_calib_dijet_half1[i]->Write();
        h_respmatrix_calib_dijet_half2[i]->Write();
    }
    f_out->Close();
    std::cout << "All done!" << std::endl;

}

////////////////////////////////////////// Functions //////////////////////////////////////////
void get_leading_subleading_jet(int& leadingjet_index, int& subleadingjet_index, std::vector<float>* jet_et) {
  leadingjet_index = -1;
  subleadingjet_index = -1;
  float leadingjet_et = -9999;
  float subleadingjet_et = -9999;
  for (int ij = 0; ij < jet_et->size(); ++ij) {
    float jetet = jet_et->at(ij);
    if (jetet > leadingjet_et) {
      subleadingjet_et = leadingjet_et;
      subleadingjet_index = leadingjet_index;
      leadingjet_et = jetet;
      leadingjet_index = ij;
    } else if (jetet > subleadingjet_et) {
      subleadingjet_et = jetet;
      subleadingjet_index = ij;
    }
  }
}

bool match_leading_subleading_jet(float leadingjet_phi, float subleadingjet_phi) {
  float dijet_min_phi = 3*TMath::Pi()/4.;
  float dphi = get_dphi(leadingjet_phi, subleadingjet_phi);
  return dphi > dijet_min_phi;
}

void get_calibjet(float& calibjet_pt, float& calibjet_eta, float& calibjet_phi, float jet_pt, float jet_eta, float jet_phi, bool dijet, TF1* f_corr, float jes_para, float jer_para) {
    calibjet_pt = -9999;
    calibjet_eta = -9999;
    calibjet_phi = -9999;
    if (!dijet) return;
    double calib_pt = f_corr->Eval(jet_pt) * (1 + randGen.Gaus(0.0, jer_para)) * jes_para;
    if (calib_pt < calibptbins[0] || calib_pt > calibptbins[calibnpt]) return;
    calibjet_pt = calib_pt;
    calibjet_eta = jet_eta;
    calibjet_phi = jet_phi;
}

void get_truthjet(float& goodtruthjet_pt, float& goodtruthjet_eta, float& goodtruthjet_phi, float jet_pt, float jet_eta, float jet_phi, bool dijet) {
    goodtruthjet_pt = -9999;
    goodtruthjet_eta = -9999;
    goodtruthjet_phi = -9999;
    if (!dijet) return;
    if (jet_pt < truthptbins[0] || jet_pt > truthptbins[truthnpt]) return;
    goodtruthjet_pt = jet_pt;
    goodtruthjet_eta = jet_eta;
    goodtruthjet_phi = jet_phi;
}

void match_meas_truth(float meas_eta, float meas_phi, bool& matched, float truth_eta, float truth_phi, float jet_radius) {
    if (meas_eta == -9999 || meas_phi == -9999 || truth_eta == -9999 || truth_phi == -9999) {
        matched = false;
        return; 
    } else {
        float dR = get_dR(meas_eta, meas_phi, truth_eta, truth_phi);
        if (dR < jet_radius * 0.75) {
            //std::cout << "dR = " << dR << std::endl;
            matched = true;
            return;
        } else {
            //std::cout << "dR = " << dR << std::endl;
            matched = false;
            return;
        }
    }
}

void fill_response_matrix(TH2D*& h_truth, TH2D*& h_meas, RooUnfoldResponse*& h_resp, TH2D*& h_fake, TH2D*& h_miss, RooUnfoldResponse*& h_count, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale) {
    if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet] && truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        if (matched) {
            h_meas->Fill(meas_pt, meas_et, weight_scale);
            h_truth->Fill(truth_pt, truth_et, weight_scale);
            h_resp->Fill(meas_pt, meas_et, truth_pt, truth_et, weight_scale);
            h_count->Fill(meas_pt, meas_et, truth_pt, truth_et);
        } else {
            h_meas->Fill(meas_pt, meas_et, weight_scale);
            h_fake->Fill(meas_pt, meas_et, weight_scale);
            h_resp->Fake(meas_pt, meas_et, weight_scale);
            h_truth->Fill(truth_pt, truth_et, weight_scale);
            h_miss->Fill(truth_pt, truth_et, weight_scale);
            h_resp->Miss(truth_pt, truth_et, weight_scale);
            h_count->Fake(meas_pt, meas_et);
            h_count->Miss(truth_pt, truth_et);
        }
    } else if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet]) {
        h_meas->Fill(meas_pt, meas_et, weight_scale);
        h_fake->Fill(meas_pt, meas_et, weight_scale);
        h_resp->Fake(meas_pt, meas_et, weight_scale);
        h_count->Fake(meas_pt, meas_et);

    } else if (truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        h_truth->Fill(truth_pt, truth_et, weight_scale);
        h_miss->Fill(truth_pt, truth_et, weight_scale);
        h_resp->Miss(truth_pt, truth_et, weight_scale);
        h_count->Miss(truth_pt, truth_et);
    }
}

void fill_response_matrix_full(TH2D*& h_truth, TH2D*& h_meas, RooUnfoldResponse*& h_resp, TH2D*& h_fake, TH2D*& h_miss, RooUnfoldResponse*& h_count, TH2D*& h_count_fake, TH2D*& h_count_miss, TH2D*& h_jetpt, TH2D*& h_caloet, TH2D*& h_nw_jetpt, TH2D*& h_nw_caloet, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale) {
    if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet] && truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        if (matched) {
            // measured, truth and response matrix 
            h_meas->Fill(meas_pt, meas_et, weight_scale);
            h_truth->Fill(truth_pt, truth_et, weight_scale);
            h_resp->Fill(meas_pt, meas_et, truth_pt, truth_et, weight_scale);
            
            // QA on response matrix 
            h_jetpt->Fill(meas_pt, truth_pt, weight_scale);
            h_caloet->Fill(meas_et, truth_et, weight_scale);
            h_nw_jetpt->Fill(meas_pt, truth_pt);
            h_nw_caloet->Fill(meas_et, truth_et);
            h_count->Fill(meas_pt, meas_et, truth_pt, truth_et);
        } else {
            h_meas->Fill(meas_pt, meas_et, weight_scale);
            h_fake->Fill(meas_pt, meas_et, weight_scale);
            h_resp->Fake(meas_pt, meas_et, weight_scale);
            h_truth->Fill(truth_pt, truth_et, weight_scale);
            h_miss->Fill(truth_pt, truth_et, weight_scale);
            h_resp->Miss(truth_pt, truth_et, weight_scale);

            // QA on response matrix 
            h_count->Fake(meas_pt, meas_et);
            h_count->Miss(truth_pt, truth_et);
            h_count_fake->Fill(meas_pt, meas_et);
            h_count_miss->Fill(truth_pt, truth_et);
        }
    } else if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet]) {
        h_meas->Fill(meas_pt, meas_et, weight_scale);
        h_fake->Fill(meas_pt, meas_et, weight_scale);
        h_resp->Fake(meas_pt, meas_et, weight_scale);

        // QA on response matrix 
        h_count->Fake(meas_pt, meas_et);
        h_count_fake->Fill(meas_pt, meas_et);

    } else if (truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        h_truth->Fill(truth_pt, truth_et, weight_scale);
        h_miss->Fill(truth_pt, truth_et, weight_scale);
        h_resp->Miss(truth_pt, truth_et, weight_scale);

        // QA on response matrix 
        h_count->Miss(truth_pt, truth_et);
        h_count_miss->Fill(truth_pt, truth_et);
    }
}

void fill_trim_response_matrix(RooUnfoldResponse*& h_resp, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale, const TMatrixD& counts_matrix, TH2D* h_counts_measured, TH2D* h_counts_truth, float trim_value) { 
    if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet] && truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        // only fill trimmed bins 

        int meas_bin = h_counts_measured->GetNbinsX() * (h_counts_measured->GetYaxis()->FindBin(meas_et) - 1) + (h_counts_measured->GetXaxis()->FindBin(meas_pt) - 1);
        int truth_bin = h_counts_truth->GetNbinsX() * (h_counts_truth->GetYaxis()->FindBin(truth_et) - 1) + (h_counts_truth->GetXaxis()->FindBin(truth_pt) - 1);
        //std::cout << "meas (pt,et) " << meas_pt << " " << meas_et << " meas_bin " << meas_bin << " truth (pt,et) " << truth_pt << " " << truth_et << " truth_bin " << truth_bin << std::endl;
        if (matched && counts_matrix(meas_bin, truth_bin) >= trim_value) {
             h_resp->Fill(meas_pt, meas_et, truth_pt, truth_et, weight_scale);
        } else {
            h_resp->Fake(meas_pt, meas_et, weight_scale);
            h_resp->Miss(truth_pt, truth_et, weight_scale);
        }
    } else if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet]) {
        h_resp->Fake(meas_pt, meas_et, weight_scale);

    } else if (truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        h_resp->Miss(truth_pt, truth_et, weight_scale);
    }
}

void fill_reweighted_trim_response_matrix(RooUnfoldResponse*& h_resp, float meas_pt, float matched, float truth_pt, float meas_et, float truth_et, float weight_scale, const TMatrixD& counts_matrix, TH2D* h_counts_measured, TH2D* h_counts_truth, float trim_value, TH2D* h_prior_weights) { 
    double prior_weight = h_prior_weights->GetBinContent(h_prior_weights->FindBin(truth_pt,truth_et));
    if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet] && truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        // only fill trimmed bins 
        int meas_bin = h_counts_measured->GetNbinsX() * (h_counts_measured->GetYaxis()->FindBin(meas_et) - 1) + (h_counts_measured->GetXaxis()->FindBin(meas_pt) - 1);
        int truth_bin = h_counts_truth->GetNbinsX() * (h_counts_truth->GetYaxis()->FindBin(truth_et) - 1) + (h_counts_truth->GetXaxis()->FindBin(truth_pt) - 1);
        if (matched && counts_matrix(meas_bin, truth_bin) >= trim_value) {
             h_resp->Fill(meas_pt, meas_et, truth_pt, truth_et, weight_scale*prior_weight);
        } else {
            h_resp->Fake(meas_pt, meas_et, weight_scale);
            h_resp->Miss(truth_pt, truth_et, weight_scale*prior_weight);
        }
    } else if (meas_pt >= calibptbins[0] && meas_et >= calibetbins[0] && meas_et <= calibetbins[calibnet]) {
        h_resp->Fake(meas_pt, meas_et, weight_scale);

    } else if (truth_pt >= truthptbins[0] && truth_et >= truthetbins[0] && truth_et <= truthetbins[truthnet]) {
        h_resp->Miss(truth_pt, truth_et, weight_scale*prior_weight);
    }
}
