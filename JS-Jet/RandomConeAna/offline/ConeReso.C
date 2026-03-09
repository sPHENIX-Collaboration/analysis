
#include <iostream>
#include <vector>

#include "sPhenixStyle.h"
#include "sPhenixStyle.C"

int ConeReso()
{

    // set sPHENIX style
    SetsPhenixStyle();
    TH1::SetDefaultSumw2();
    TH2::SetDefaultSumw2();

   
    // in/out file names
    TString input_file = "/sphenix/user/tmengel/JetPerformancePPG/RandomConeAna/condor/merging/rootfiles/HIJING.root";
    TString output_file = "HIJING_ConePlots.root";


    // open input file
    TFile *f = new TFile(input_file, "READ");
    if(!f->IsOpen() || f->IsZombie()){ std::cout << "File " << input_file << " is zombie" << std::endl;  return -1; }

    // get tree
    TTree *t = (TTree*)f->Get("event_info");
    if(!t){ std::cout << "Tree event_info not found in " << input_file << std::endl; return -1; }

    // input variables
    int m_event_id = 0;
    int m_centrality = 0;
    float m_impactparam = 0;
    float m_zvtx = 0;
    double m_mbd_NS = 0;

    // rho variables
    float m_TowerRho_AREA_rho = 0;
    float m_TowerRho_AREA_sigma = 0;
    float m_TowerRho_MULT_rho = 0;
    float m_TowerRho_MULT_sigma = 0;

    // random cone variables
    float m_RandomCone_Tower_basic_r04_R = 0;
    float m_RandomCone_Tower_basic_r04_eta = 0;
    float m_RandomCone_Tower_basic_r04_phi = 0;
    float m_RandomCone_Tower_basic_r04_pt = 0;
    int m_RandomCone_Tower_basic_r04_nclustered = 0;

    float m_RandomCone_Tower_randomize_etaphi_r04_R = 0;
    float m_RandomCone_Tower_randomize_etaphi_r04_eta = 0;
    float m_RandomCone_Tower_randomize_etaphi_r04_phi = 0;
    float m_RandomCone_Tower_randomize_etaphi_r04_pt = 0;
    int m_RandomCone_Tower_randomize_etaphi_r04_nclustered = 0;

    float m_RandomCone_Tower_Sub1_basic_r04_R = 0;
    float m_RandomCone_Tower_Sub1_basic_r04_eta = 0;
    float m_RandomCone_Tower_Sub1_basic_r04_phi = 0;
    float m_RandomCone_Tower_Sub1_basic_r04_pt = 0;
    int m_RandomCone_Tower_Sub1_basic_r04_nclustered = 0;

    float m_RandomCone_Tower_Sub1_randomize_etaphi_r04_R = 0;
    float m_RandomCone_Tower_Sub1_randomize_etaphi_r04_eta = 0;
    float m_RandomCone_Tower_Sub1_randomize_etaphi_r04_phi = 0;
    float m_RandomCone_Tower_Sub1_randomize_etaphi_r04_pt = 0;
    int m_RandomCone_Tower_Sub1_randomize_etaphi_r04_nclustered = 0;


    // set branch addresses
    t->SetBranchAddress("event_id", &m_event_id);
    t->SetBranchAddress("centrality", &m_centrality);
    t->SetBranchAddress("impactparam", &m_impactparam);
    t->SetBranchAddress("zvtx", &m_zvtx);
    t->SetBranchAddress("mbd_NS", &m_mbd_NS);
    t->SetBranchAddress("TowerRho_AREA_rho", &m_TowerRho_AREA_rho);
    t->SetBranchAddress("TowerRho_AREA_sigma", &m_TowerRho_AREA_sigma);
    t->SetBranchAddress("TowerRho_MULT_rho", &m_TowerRho_MULT_rho);
    t->SetBranchAddress("TowerRho_MULT_sigma", &m_TowerRho_MULT_sigma);
    t->SetBranchAddress("RandomCone_Tower_basic_r04_R", &m_RandomCone_Tower_basic_r04_R);
    t->SetBranchAddress("RandomCone_Tower_basic_r04_eta", &m_RandomCone_Tower_basic_r04_eta);
    t->SetBranchAddress("RandomCone_Tower_basic_r04_phi", &m_RandomCone_Tower_basic_r04_phi);
    t->SetBranchAddress("RandomCone_Tower_basic_r04_pt", &m_RandomCone_Tower_basic_r04_pt);
    t->SetBranchAddress("RandomCone_Tower_basic_r04_nclustered", &m_RandomCone_Tower_basic_r04_nclustered);
    t->SetBranchAddress("RandomCone_Tower_randomize_etaphi_r04_R", &m_RandomCone_Tower_randomize_etaphi_r04_R);
    t->SetBranchAddress("RandomCone_Tower_randomize_etaphi_r04_eta", &m_RandomCone_Tower_randomize_etaphi_r04_eta);
    t->SetBranchAddress("RandomCone_Tower_randomize_etaphi_r04_phi", &m_RandomCone_Tower_randomize_etaphi_r04_phi);
    t->SetBranchAddress("RandomCone_Tower_randomize_etaphi_r04_pt", &m_RandomCone_Tower_randomize_etaphi_r04_pt);
    t->SetBranchAddress("RandomCone_Tower_randomize_etaphi_r04_nclustered", &m_RandomCone_Tower_randomize_etaphi_r04_nclustered);
    t->SetBranchAddress("RandomCone_Tower_Sub1_basic_r04_R", &m_RandomCone_Tower_Sub1_basic_r04_R);
    t->SetBranchAddress("RandomCone_Tower_Sub1_basic_r04_eta", &m_RandomCone_Tower_Sub1_basic_r04_eta);
    t->SetBranchAddress("RandomCone_Tower_Sub1_basic_r04_phi", &m_RandomCone_Tower_Sub1_basic_r04_phi);
    t->SetBranchAddress("RandomCone_Tower_Sub1_basic_r04_pt", &m_RandomCone_Tower_Sub1_basic_r04_pt);
    t->SetBranchAddress("RandomCone_Tower_Sub1_basic_r04_nclustered", &m_RandomCone_Tower_Sub1_basic_r04_nclustered);
    t->SetBranchAddress("RandomCone_Tower_Sub1_randomize_etaphi_r04_R", &m_RandomCone_Tower_Sub1_randomize_etaphi_r04_R);
    t->SetBranchAddress("RandomCone_Tower_Sub1_randomize_etaphi_r04_eta", &m_RandomCone_Tower_Sub1_randomize_etaphi_r04_eta);
    t->SetBranchAddress("RandomCone_Tower_Sub1_randomize_etaphi_r04_phi", &m_RandomCone_Tower_Sub1_randomize_etaphi_r04_phi);
    t->SetBranchAddress("RandomCone_Tower_Sub1_randomize_etaphi_r04_pt", &m_RandomCone_Tower_Sub1_randomize_etaphi_r04_pt);
    t->SetBranchAddress("RandomCone_Tower_Sub1_randomize_etaphi_r04_nclustered", &m_RandomCone_Tower_Sub1_randomize_etaphi_r04_nclustered);

    // get number of entries
    int nentries = t->GetEntries();

    // declare histograms
    const int resp_N = 250;
    double max_resp = 60;
    double min_resp = -60;
    double delta_resp = (max_resp - min_resp)/resp_N;
    double resp_bins[resp_N+1];
    for(int i = 0; i < resp_N+1; i++)
    {
        resp_bins[i] = min_resp + i*delta_resp;
    }

    const double cent_bins[] = {0, 10, 20, 30, 40, 50, 60, 80, 100};
    const int cent_N = sizeof(cent_bins)/sizeof(double) - 1;

    const int rho_N = 250;
    double rho_max = 125;
    double rho_min = 0;
    double delta_rho = (rho_max - rho_min)/rho_N;
    double rho_bins[rho_N+1];
    for(int i = 0; i < rho_N+1; i++)
    {
        rho_bins[i] = rho_min + i*delta_rho;
    }

    const int rho_mult_N = 200;
    double rho_mult_max = 2.0;
    double rho_mult_min = 0;
    double delta_rho_mult = (rho_mult_max - rho_mult_min)/rho_mult_N;
    double rho_bins_mult[rho_mult_N+1];
    for(int i = 0; i < rho_mult_N+1; i++)
    {
        rho_bins_mult[i] = rho_mult_min + i*delta_rho_mult;
    }

    const int nclustered_N = 150;
    double nclustered_max = 150;
    double nclustered_min = 0;
    double delta_nclustered = (nclustered_max - nclustered_min)/nclustered_N;
    double nclustered_bins[nclustered_N+1];
    for(int i = 0; i < nclustered_N+1; i++)
    {
        nclustered_bins[i] = nclustered_min + i*delta_nclustered;
    }



    // cone residual histograms
    TH2D * h2_cone_residual_pt_basic_area = new TH2D("h2_cone_residual_pt_basic_area", "h2_cone_residual_pt_basic_area", resp_N, resp_bins, cent_N, cent_bins);
    TH2D * h2_cone_residual_pt_basic_mult = new TH2D("h2_cone_residual_pt_basic_mult", "h2_cone_residual_pt_basic_mult", resp_N, resp_bins, cent_N, cent_bins);
    TH2D * h2_cone_residual_pt_basic_sub1 = new TH2D("h2_cone_residual_pt_basic_sub1", "h2_cone_residual_pt_basic_sub1", resp_N, resp_bins, cent_N, cent_bins);

    TH2D * h2_cone_residual_pt_randomize_etaphi_area = new TH2D("h2_cone_residual_pt_randomize_etaphi_area", "h2_cone_residual_pt_randomize_etaphi_area", resp_N, resp_bins, cent_N, cent_bins);
    TH2D * h2_cone_residual_pt_randomize_etaphi_mult = new TH2D("h2_cone_residual_pt_randomize_etaphi_mult", "h2_cone_residual_pt_randomize_etaphi_mult", resp_N, resp_bins, cent_N, cent_bins);
    TH2D * h2_cone_residual_pt_randomize_etaphi_sub1 = new TH2D("h2_cone_residual_pt_randomize_etaphi_sub1", "h2_cone_residual_pt_randomize_etaphi_sub1", resp_N, resp_bins, cent_N, cent_bins);

    // nclustered histograms
    TH2D * h2_nclustered_basic = new TH2D("h2_nclustered_basic", "h2_nclustered_basic", cent_N, cent_bins, nclustered_N, nclustered_bins);
    TH2D * h2_nclustered_randomize_etaphi = new TH2D("h2_nclustered_randomize_etaphi", "h2_nclustered_randomize_etaphi", cent_N, cent_bins , nclustered_N, nclustered_bins);

    // phi and eta cross check histograms
    TH1D * h1_phi_cross_check_basic = new TH1D("h1_phi_cross_check_basic", "h1_phi_cross_check_basic", 100, -TMath::Pi(), TMath::Pi());
    TH1D * h1_phi_cross_check_randomize_etaphi = new TH1D("h1_phi_cross_check_randomize_etaphi", "h1_phi_cross_check_randomize_etaphi", 100, -TMath::Pi(), TMath::Pi());
    TH1D * h1_eta_cross_check_basic = new TH1D("h1_eta_cross_check_basic", "h1_eta_cross_check_basic", 100, -1.5, 1.5);
    TH1D * h1_eta_cross_check_randomize_etaphi = new TH1D("h1_eta_cross_check_randomize_etaphi", "h1_eta_cross_check_randomize_etaphi", 100, -1.5, 1.5);

    // rho histograms
    TH2D * h2_rho_area_rho = new TH2D("h2_rho_area_rho", "h2_rho_area_rho", rho_N, rho_bins, cent_N, cent_bins);
    TH2D * h2_rho_mult_rho = new TH2D("h2_rho_mult_rho", "h2_rho_mult_rho", rho_mult_N, rho_bins_mult, cent_N, cent_bins);
    TH2D * h2_rho_area_sigma = new TH2D("h2_rho_area_sigma", "h2_rho_area_sigma", rho_N, rho_bins, cent_N, cent_bins);
    TH2D * h2_rho_mult_sigma = new TH2D("h2_rho_mult_sigma", "h2_rho_mult_sigma", rho_mult_N, rho_bins_mult, cent_N, cent_bins);

    // mbd, zvtx, impact parameter, and centrality histograms
    TH1D * h1_mbd = new TH1D("h1_mbd", "h1_mbd", 100, 0, 1);
    TH1D * h1_zvtx = new TH1D("h1_zvtx", "h1_zvtx", 100, -50, 50);
    TH1D * h1_impactparam = new TH1D("h1_impactparam", "h1_impactparam", 100, 0, 10);
    TH1D * h1_centrality = new TH1D("h1_centrality", "h1_centrality", cent_N, cent_bins);


    // loop over entries
    for (int i = 0; i < nentries; i++)
    {
        t->GetEntry(i);
        
        // basic cone area and multiplicity
        float pt_basic_area = m_RandomCone_Tower_basic_r04_pt - m_TowerRho_AREA_rho*(TMath::Pi()*m_RandomCone_Tower_basic_r04_R*m_RandomCone_Tower_basic_r04_R);
        float pt_basic_mult = m_RandomCone_Tower_basic_r04_pt - m_TowerRho_MULT_rho*m_RandomCone_Tower_basic_r04_nclustered;
        float pt_basic_sub1 = m_RandomCone_Tower_Sub1_basic_r04_pt;

        float pt_randomize_etaphi_area = m_RandomCone_Tower_randomize_etaphi_r04_pt - m_TowerRho_AREA_rho*(TMath::Pi()*m_RandomCone_Tower_randomize_etaphi_r04_R*m_RandomCone_Tower_randomize_etaphi_r04_R);
        float pt_randomize_etaphi_mult = m_RandomCone_Tower_randomize_etaphi_r04_pt - m_TowerRho_MULT_rho*m_RandomCone_Tower_randomize_etaphi_r04_nclustered;
        float pt_randomize_etaphi_sub1 = m_RandomCone_Tower_Sub1_randomize_etaphi_r04_pt;

        // fill histograms
        h2_cone_residual_pt_basic_area->Fill(pt_basic_area, m_centrality);
        h2_cone_residual_pt_basic_mult->Fill(pt_basic_mult, m_centrality);
        h2_cone_residual_pt_basic_sub1->Fill(pt_basic_sub1, m_centrality);

        h2_cone_residual_pt_randomize_etaphi_area->Fill(pt_randomize_etaphi_area, m_centrality);
        h2_cone_residual_pt_randomize_etaphi_mult->Fill(pt_randomize_etaphi_mult, m_centrality);
        h2_cone_residual_pt_randomize_etaphi_sub1->Fill(pt_randomize_etaphi_sub1, m_centrality);

        // nclustered histograms
        h2_nclustered_basic->Fill(m_RandomCone_Tower_basic_r04_nclustered, m_centrality);
        h2_nclustered_randomize_etaphi->Fill(m_RandomCone_Tower_randomize_etaphi_r04_nclustered, m_centrality);

        // phi and eta cross check histograms
        h1_phi_cross_check_basic->Fill(m_RandomCone_Tower_basic_r04_phi);
        h1_phi_cross_check_randomize_etaphi->Fill(m_RandomCone_Tower_randomize_etaphi_r04_phi);
        h1_eta_cross_check_basic->Fill(m_RandomCone_Tower_basic_r04_eta);
        h1_eta_cross_check_randomize_etaphi->Fill(m_RandomCone_Tower_randomize_etaphi_r04_eta);

        // rho histograms
        h2_rho_area_rho->Fill(m_TowerRho_AREA_rho, m_centrality);
        h2_rho_mult_rho->Fill(m_TowerRho_MULT_rho, m_centrality);
        h2_rho_area_sigma->Fill(m_TowerRho_AREA_sigma, m_centrality);
        h2_rho_mult_sigma->Fill(m_TowerRho_MULT_sigma, m_centrality);


        // mbd, zvtx, impact parameter, and centrality histograms
        h1_mbd->Fill(m_mbd_NS);
        h1_zvtx->Fill(m_zvtx);
        h1_impactparam->Fill(m_impactparam);
        h1_centrality->Fill(m_centrality);


    }


    // make 1D sigma histograms
    TH1D * h1_cone_residual_pt_basic_area_sigma = new TH1D("h1_cone_residual_pt_basic_area_sigma", "h1_cone_residual_pt_basic_area_sigma", cent_N, cent_bins);
    TH1D * h1_cone_residual_pt_basic_mult_sigma = new TH1D("h1_cone_residual_pt_basic_mult_sigma", "h1_cone_residual_pt_basic_mult_sigma", cent_N, cent_bins);
    TH1D * h1_cone_residual_pt_basic_sub1_sigma = new TH1D("h1_cone_residual_pt_basic_sub1_sigma", "h1_cone_residual_pt_basic_sub1_sigma", cent_N, cent_bins);

    TH1D * h1_cone_residual_pt_randomize_etaphi_area_sigma = new TH1D("h1_cone_residual_pt_randomize_etaphi_area_sigma", "h1_cone_residual_pt_randomize_etaphi_area_sigma", cent_N, cent_bins);
    TH1D * h1_cone_residual_pt_randomize_etaphi_mult_sigma = new TH1D("h1_cone_residual_pt_randomize_etaphi_mult_sigma", "h1_cone_residual_pt_randomize_etaphi_mult_sigma", cent_N, cent_bins);
    TH1D * h1_cone_residual_pt_randomize_etaphi_sub1_sigma = new TH1D("h1_cone_residual_pt_randomize_etaphi_sub1_sigma", "h1_cone_residual_pt_randomize_etaphi_sub1_sigma", cent_N, cent_bins);

    // make vectors of 2D histograms for easier access
    std::vector<TH2D*> h2_cone_residual_pt_vec = {h2_cone_residual_pt_basic_area, 
                                                  h2_cone_residual_pt_basic_mult, 
                                                  h2_cone_residual_pt_basic_sub1, 
                                                  h2_cone_residual_pt_randomize_etaphi_area, 
                                                  h2_cone_residual_pt_randomize_etaphi_mult, 
                                                  h2_cone_residual_pt_randomize_etaphi_sub1};

    // 1D vector
    std::vector<TH1D*> h1_cone_residual_pt_sigma_vec = {h1_cone_residual_pt_basic_area_sigma, 
                                                        h1_cone_residual_pt_basic_mult_sigma, 
                                                        h1_cone_residual_pt_basic_sub1_sigma, 
                                                        h1_cone_residual_pt_randomize_etaphi_area_sigma, 
                                                        h1_cone_residual_pt_randomize_etaphi_mult_sigma, 
                                                        h1_cone_residual_pt_randomize_etaphi_sub1_sigma};

    // create output file
    TFile *fout = new TFile(output_file, "RECREATE");

    // loop over 2D histograms, split by centrality and get RMS
    for (unsigned int ihist = 0; ihist < h2_cone_residual_pt_vec.size(); ihist++)
    {
        // get 2D histogram
        TH2D * h2_temp = h2_cone_residual_pt_vec.at(ihist);
        h2_temp->GetXaxis()->SetTitle("#delta p_{T}");
        h2_temp->GetYaxis()->SetTitle("Centrality");
        
        // get 1D histograms
        TH1D * h1_temp = h1_cone_residual_pt_sigma_vec.at(ihist);
        h1_temp->GetXaxis()->SetTitle("Centrality");
        h1_temp->GetYaxis()->SetTitle("#sigma(#delta p_{T})");

        // write 2D histograms
        fout->cd();
        h2_temp->Write();

        
        for (int icent = 1; icent <= cent_N; icent++)
        {
            h2_temp->GetYaxis()->SetRange(icent, icent);
            std::string hist_name = h2_temp->GetName();
            TH1D * h1_temp = (TH1D*)h2_temp->ProjectionX(Form("%s_cent%d", hist_name.c_str(), icent));
            
            h1_temp->GetXaxis()->SetTitle("#delta p_{T}");
            h1_temp->GetYaxis()->SetTitle("Counts");

            double sigma = h1_temp->GetRMS();
            double sigma_err = h1_temp->GetRMSError();

            h1_cone_residual_pt_sigma_vec.at(ihist)->SetBinContent(icent, sigma);
            h1_cone_residual_pt_sigma_vec.at(ihist)->SetBinError(icent, sigma_err);

            // write histograms
            fout->cd();
            h1_temp->Write();

            delete h1_temp;
        }

        // write 1D sigma histograms
        fout->cd();
        h1_temp->Write();

        delete h2_temp;
        delete h1_temp;
    }


    // loop over centrality bins and get rho histograms
    for (int icent = 1; icent <= cent_N; icent++)
    {
        h2_rho_area_rho->GetYaxis()->SetRange(icent, icent);
        h2_rho_mult_rho->GetYaxis()->SetRange(icent, icent);
        h2_rho_area_sigma->GetYaxis()->SetRange(icent, icent);
        h2_rho_mult_sigma->GetYaxis()->SetRange(icent, icent);

        TH1D * h1_rho_area_rho = (TH1D*)h2_rho_area_rho->ProjectionX(Form("%s_cent%d", h2_rho_area_rho->GetName(), icent));
        TH1D * h1_rho_mult_rho = (TH1D*)h2_rho_mult_rho->ProjectionX(Form("%s_cent%d", h2_rho_mult_rho->GetName(), icent));
        TH1D * h1_rho_area_sigma = (TH1D*)h2_rho_area_sigma->ProjectionX(Form("%s_cent%d", h2_rho_area_sigma->GetName(), icent));
        TH1D * h1_rho_mult_sigma = (TH1D*)h2_rho_mult_sigma->ProjectionX(Form("%s_cent%d", h2_rho_mult_sigma->GetName(), icent));

        h1_rho_area_rho->GetXaxis()->SetTitle("#rho_{area} (GeV)");
        h1_rho_mult_rho->GetXaxis()->SetTitle("#rho_{mult} (GeV)");
        h1_rho_area_sigma->GetXaxis()->SetTitle("#rho_{area} (GeV)");
        h1_rho_mult_sigma->GetXaxis()->SetTitle("#rho_{mult} (GeV)");

        // write histograms
        fout->cd();
        h1_rho_area_rho->Write();
        h1_rho_mult_rho->Write();
        h1_rho_area_sigma->Write();
        h1_rho_mult_sigma->Write();

        delete h1_rho_area_rho;
        delete h1_rho_mult_rho;
        delete h1_rho_area_sigma;
        delete h1_rho_mult_sigma;
    }



    // project nclustered histograms
    TProfile * p_nclustered_basic = h2_nclustered_basic->ProfileX();
    TProfile * p_nclustered_randomize_etaphi = h2_nclustered_randomize_etaphi->ProfileX();
    TH1D * h1_nclustered_basic = (TH1D*)p_nclustered_basic->ProjectionX(Form("%s_projX", h2_nclustered_basic->GetName()));
    TH1D * h1_nclustered_randomize_etaphi = (TH1D*)p_nclustered_randomize_etaphi->ProjectionX(Form("%s_projX", h2_nclustered_randomize_etaphi->GetName()));

    // write nclustered histograms
    fout->cd();
    h2_nclustered_basic->Write();
    h2_nclustered_randomize_etaphi->Write();
        p_nclustered_basic->Write();
    p_nclustered_randomize_etaphi->Write();
    h1_nclustered_basic->Write();
    h1_nclustered_randomize_etaphi->Write();

    // write eta and phi cross check histograms
    fout->cd();
    h1_phi_cross_check_basic->Write();
    h1_phi_cross_check_randomize_etaphi->Write();
    h1_eta_cross_check_basic->Write();
    h1_eta_cross_check_randomize_etaphi->Write();

    // write rho zvtx, impact parameter, and centrality histograms
    fout->cd();
    h1_mbd->Write();
    h1_zvtx->Write();
    h1_impactparam->Write();
    h1_centrality->Write();



    // close output file
    fout->Close();

    // close input file
    f->Close();

    return 0;

}