#ifndef EtaDistReader_h
#define EtaDistReader_h

#include "INTTEta.h"

// todo : the signal_region and centrality_region is hard_written in the INTTEta.h

class EtaDistReader : public INTTEta
{
    public:
        EtaDistReader(string run_type, string out_folder_directory, vector<pair<int,vector<int>>> included_eta_z_map, string input_file_directory, bool centrality_Z_map_bool = 0) : 
        INTTEta(run_type, out_folder_directory), included_eta_z_map(included_eta_z_map), input_file_directory(input_file_directory), centrality_Z_map_bool(centrality_Z_map_bool)
        {
            cout<<"In EtaDistReader, centrality region size : "<<centrality_region.size()<<endl;    
            cout<<"In EtaDistReader, plot_text: "<<plot_text<<endl;
            cout<<"In EtaDistReader, signal_region: "<<signal_region<<endl;

            gErrorIgnoreLevel=kInfo;

            file_in = TFile::Open(input_file_directory.c_str());

            N_centrality_bin = centrality_region.size();
            eta_correction = included_eta_z_map[0].first - 1; // todo : the eta bin starts from 15, so the correction is 14
            
            SignalNTrack_eta_z_Multi_2D.clear();
            SignalNTrack_eta_z_Single_2D.clear();
            TrueNtrack_eta_z_MC_2D.clear();
            dNdeta_1D_MC.clear();
            dNdeta_1D_reco_single.clear();
            dNdeta_1D_reco_multi.clear();
            DeltaPhi_Multi_1D.clear();
            DeltaPhi_Multi_Stack.clear();
            SignalNTrack_Single.clear(); 
            SignalNTrack_Multi.clear();
            N_event_counting = vector<vector<int>>(N_centrality_bin, vector<int>(included_eta_z_map.size(),0));
            N_event_counting_MC = vector<vector<int>>(N_centrality_bin, vector<int>(included_eta_z_map.size(),0));

            DeltaPhi_Multi_Stack_hist_out.clear();

            ReadFileHist();
            InitHist();
            MainPreparation();
            FinaldNdEta();


            return;
        };

        vector<TH1F *> GetDeltaPhi_Multi_stack_1D();
        vector<TH1F *> GetdNdeta_1D_MC();
        vector<TH1F *> GetdNdeta_1D_reco_single();
        vector<TH1F *> GetdNdeta_1D_reco_multi();
        vector<string> Get_centrality_region() {return centrality_region;};

        ~EtaDistReader()
        {
            file_in -> Close();
            cout<<"EtaDistReader done, goodbye"<<endl;
        };

    protected:

        bool centrality_Z_map_bool;
        vector<pair<int,vector<int>>> included_eta_z_map;
        string input_file_directory;
        int eta_correction; // todo : the eta bin starts from 15, so the correction is 14
        int N_centrality_bin;

        TFile * file_in;
        string color_code[5] = {"#fedc97", "#b5b682", "#7c9885", "#28666e", "#033f63"};
        // vector<string> centrality_region;

        vector<TH2F *> SignalNTrack_eta_z_Multi_2D;
        vector<TH2F *> SignalNTrack_eta_z_Single_2D;
        map<string,TH1F *> DeltaPhi_Multi_1D;
        vector<TH2F *> TrueNtrack_eta_z_MC_2D;
        TH2F * centrality_Z_map;
        TH2F * centrality_Z_map_MC;
        TH2F * eta_z_ref;

        vector<TH1F *> dNdeta_1D_MC;
        vector<TH1F *> dNdeta_1D_reco_single;
        vector<TH1F *> dNdeta_1D_reco_multi;

        map<string, THStack *> DeltaPhi_Multi_Stack;
        vector<TH1F *> DeltaPhi_Multi_Stack_hist_out;
        map<string, int> SignalNTrack_Single;
        map<string, int> SignalNTrack_Multi;
        vector<vector<int>> N_event_counting;
        vector<vector<int>> N_event_counting_MC;

        TH1F * temp_hist;

        void ReadFileHist();
        void InitHist();
        void MainPreparation();
        void FinaldNdEta();

};

void EtaDistReader::ReadFileHist()
{

    // note : the histograms of the number of tracklets in the signal region for both methods
    // note : the 1D vector is for different centrality bins
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        SignalNTrack_eta_z_Multi_2D.push_back((TH2F *) file_in -> Get(Form("Reco_SignalNTracklet_Multi_MBin%i",Mbin)));
        SignalNTrack_eta_z_Single_2D.push_back((TH2F *) file_in -> Get(Form("Reco_SignalNTracklet_Single_MBin%i",Mbin)));
    }

    // note : the 1D histograms of the DeltaPhi distributions of each centrality-eta-z bin (the multi method, for the background subtraction)
    // note : the format of the key is "MBin_EtaBin_ZBin"
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++) // note : centrality bin
    {
        for (auto eta_z : included_eta_z_map) // note : eta bin
        {
            for(int zbin = 0; zbin < eta_z.second.size(); zbin++) // note :  Z bin
            {
                DeltaPhi_Multi_1D[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] = (TH1F *) file_in -> Get(Form("Reco_DeltaPhi1D_Multi_MBin%i_Eta%i_Z%i",Mbin,eta_z.first,eta_z.second[zbin]));
                DeltaPhi_Multi_1D[Form("%i_%i_%i",Mbin,eta_z.first,eta_z.second[zbin])] -> SetFillColor(TColor::GetColor(color_code[zbin%5].c_str()));
            }
        }
    }

    // note : the histograms of the number of True track from the MC
    // note : the 1D vector is for different centrality bins
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {
        TrueNtrack_eta_z_MC_2D.push_back((TH2F *) file_in -> Get(Form("NTrueTrack_MBin%i",Mbin)));
    }

    // note : the centrality Z map, to keep the N event counting
    string centrality_Z_map_name = (centrality_Z_map_bool == 0) ? "MBin_Z_evt_map" : "Z_MBin_evt_map"; // todo : the correct name should be "MBin_Z_evt_map", correct next time
    centrality_Z_map    = (TH2F *) file_in -> Get(centrality_Z_map_name.c_str());
    centrality_Z_map_MC = (TH2F *) file_in -> Get((centrality_Z_map_name+"_MC").c_str()); 

    // note : the reference map of Eta-Z 2D histogram
    eta_z_ref = (TH2F *) file_in -> Get("Eta_Z_reference");
    
    cout<<"ReadFileHist done"<<endl;

    return;

}

void EtaDistReader::InitHist()
{
    for (int i = 0; i < N_centrality_bin; i++)
    {
        dNdeta_1D_MC.push_back(new TH1F("","",included_eta_z_map.size(), eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.));
        dNdeta_1D_MC[i] -> SetMarkerStyle(20);
        dNdeta_1D_MC[i] -> SetMarkerSize(0.8);
        dNdeta_1D_MC[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_MC[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_MC[i] -> SetLineWidth(2);
        dNdeta_1D_MC[i] -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
        dNdeta_1D_MC[i] -> GetXaxis() -> SetTitle("#eta");
        // dNdeta_1D_MC[i] -> GetYaxis() -> SetRangeUser(0,50);
        dNdeta_1D_MC[i] -> SetTitleSize(0.06, "X");
        dNdeta_1D_MC[i] -> SetTitleSize(0.06, "Y");
        dNdeta_1D_MC[i] -> GetXaxis() -> SetTitleOffset (0.71);
        dNdeta_1D_MC[i] -> GetYaxis() -> SetTitleOffset (1.1);
        dNdeta_1D_MC[i] -> GetXaxis() -> CenterTitle(true);
        dNdeta_1D_MC[i] -> GetYaxis() -> CenterTitle(true);

        dNdeta_1D_reco_single.push_back(new TH1F("","",included_eta_z_map.size(), eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.));
        dNdeta_1D_reco_single[i] -> SetMarkerStyle(20);
        dNdeta_1D_reco_single[i] -> SetMarkerSize(0.8);
        dNdeta_1D_reco_single[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single[i] -> SetLineWidth(2);
        dNdeta_1D_reco_single[i] -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
        dNdeta_1D_reco_single[i] -> GetXaxis() -> SetTitle("#eta");
        // dNdeta_1D_reco_single[i] -> GetYaxis() -> SetRangeUser(0,50);
        dNdeta_1D_reco_single[i] -> SetTitleSize(0.06, "X");
        dNdeta_1D_reco_single[i] -> SetTitleSize(0.06, "Y");
        dNdeta_1D_reco_single[i] -> GetXaxis() -> SetTitleOffset (0.71);
        dNdeta_1D_reco_single[i] -> GetYaxis() -> SetTitleOffset (1.1);
        dNdeta_1D_reco_single[i] -> GetXaxis() -> CenterTitle(true);
        dNdeta_1D_reco_single[i] -> GetYaxis() -> CenterTitle(true);

        dNdeta_1D_reco_multi.push_back(new TH1F("","",included_eta_z_map.size(), eta_z_ref -> GetXaxis() ->GetBinLowEdge(included_eta_z_map[0].first), eta_z_ref -> GetXaxis() ->GetBinCenter(included_eta_z_map[included_eta_z_map.size() - 1].first) + eta_z_ref -> GetXaxis() ->GetBinWidth(included_eta_z_map[included_eta_z_map.size() - 1].first) / 2.));
        dNdeta_1D_reco_multi[i] -> SetMarkerStyle(20);
        dNdeta_1D_reco_multi[i] -> SetMarkerSize(0.8);
        dNdeta_1D_reco_multi[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_multi[i] -> SetLineColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_multi[i] -> SetLineWidth(2);
        dNdeta_1D_reco_multi[i] -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
        dNdeta_1D_reco_multi[i] -> GetXaxis() -> SetTitle("#eta");
        // dNdeta_1D_reco_multi[i] -> GetYaxis() -> SetRangeUser(0,50);
        dNdeta_1D_reco_multi[i] -> SetTitleSize(0.06, "X");
        dNdeta_1D_reco_multi[i] -> SetTitleSize(0.06, "Y");
        dNdeta_1D_reco_multi[i] -> GetXaxis() -> SetTitleOffset (0.71);
        dNdeta_1D_reco_multi[i] -> GetYaxis() -> SetTitleOffset (1.1);
        dNdeta_1D_reco_multi[i] -> GetXaxis() -> CenterTitle(true);
        dNdeta_1D_reco_multi[i] -> GetYaxis() -> CenterTitle(true);
    }

    cout<<"InitHist done"<<endl;
}




// note : to count the number of reco tracklets in the signal region with the selected eta bin and z bin, for each centrality bin
// note : for both methods
// note : to increase the statistic, try to stack the DeltaPhi distributions with same eta bin but different z bin
// note : the key is "MBin_EtaBin"
// note : for each eta bin and each centrality bin, count the number of event. (Each eta bin has different selected z region, so the number of event is different for each eta bin)
void EtaDistReader::MainPreparation()
{
    c1 -> cd();
    c1 -> Print(Form("%s/Reco_DeltaPhi_Multi_Stack.pdf(", out_folder_directory.c_str())); 
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++) // note : centrality bin
    {
        for (auto eta_z : included_eta_z_map) // note : eta bin
        {
            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] = new THStack(Form("DeltaPhi_Multi_Stack_MBin%i_Eta%i",Mbin,eta_z.first),Form("DeltaPhi_Multi_Stack_MBin%i_Eta%i",Mbin,eta_z.first));
            for(int zbin : eta_z.second) // note :  Z bin
            {
                // note : the key to the map: "MBin_EtaBin"
                DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> Add(DeltaPhi_Multi_1D[Form("%i_%i_%i",Mbin,eta_z.first,zbin)]);
                SignalNTrack_Single[Form("%i_%i",Mbin,eta_z.first)] += SignalNTrack_eta_z_Single_2D[Mbin] -> GetBinContent(eta_z.first, zbin); // note : the signal counting for the single method
                SignalNTrack_Multi[Form("%i_%i",Mbin,eta_z.first)]  +=  SignalNTrack_eta_z_Multi_2D[Mbin] -> GetBinContent(eta_z.first, zbin); // note : the signal counting for the multi method

                // note : to count the TrueNTtrack in the true level information
                dNdeta_1D_MC[Mbin]->SetBinContent(eta_z.first - eta_correction, TrueNtrack_eta_z_MC_2D[Mbin] -> GetBinContent(eta_z.first, zbin) + dNdeta_1D_MC[Mbin]->GetBinContent(eta_z.first - eta_correction));

                N_event_counting[Mbin][eta_z.first - eta_correction - 1]    += centrality_Z_map -> GetBinContent(Mbin + 1, zbin);
                N_event_counting_MC[Mbin][eta_z.first - eta_correction - 1] += centrality_Z_map_MC -> GetBinContent(Mbin + 1, zbin);
            }

            // note : Background fit on the DeltaPhi distributions, to know the background level
            // note : print the DeltaPhi distributions for each centrality bin adn each eta bin, after stacking
            temp_hist = (TH1F *) DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> GetStack() -> Last();
            
            DeltaPhi_Multi_Stack_hist_out.push_back(temp_hist);
            DeltaPhi_Multi_Stack_hist_out[DeltaPhi_Multi_Stack_hist_out.size() - 1] -> SetTitle(Form("MBin%i_EtaBin%i",Mbin,eta_z.first));

            double hist_offset = get_dist_offset(temp_hist, 15);

            bkg_fit_pol2 -> SetParameters(hist_offset, 0, -0.2, 0, signal_region);
            bkg_fit_pol2 -> FixParameter(4, signal_region);
            bkg_fit_pol2 -> SetParLimits(2, -100, 0);
            temp_hist -> Fit(bkg_fit_pol2,"NQ");
            // note : extract the background region (which includes the signal region also)
            draw_pol2_line -> SetParameters(bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3));

            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> SetMinimum(0);
            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> SetMaximum( temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            DeltaPhi_Multi_Stack[Form("%i_%i",Mbin,eta_z.first)] -> Draw(); 

            draw_pol2_line -> Draw("lsame");
            
            double pol2_bkg_integral = fabs(draw_pol2_line -> Integral( -1. * signal_region, signal_region )) / temp_hist -> GetBinWidth(1);
            // cout<<i<<" "<<i1<<" pol2_bkg integral: "<<pol2_bkg_integral<<endl;

            // note : for the multi method, count the number of tracklets after the back subtraction
            dNdeta_1D_reco_multi[Mbin]  -> SetBinContent(eta_z.first - eta_correction, SignalNTrack_Multi[Form("%i_%i",Mbin,eta_z.first)] - pol2_bkg_integral);
            dNdeta_1D_reco_single[Mbin] -> SetBinContent(eta_z.first - eta_correction, SignalNTrack_Single[Form("%i_%i",Mbin,eta_z.first)]);

            coord_line -> DrawLine(-1*signal_region, 0, -1 * signal_region, temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            coord_line -> DrawLine(signal_region, 0, signal_region, temp_hist -> GetBinContent(temp_hist -> GetMaximumBin()) * 1.5);
            
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
            
            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s, #eta: %.2f ~ %.2f",centrality_region[Mbin].c_str(), 
                eta_z_ref -> GetXaxis() -> GetBinCenter(eta_z.first) - eta_z_ref -> GetXaxis() -> GetBinWidth(eta_z.first)/2., 
                eta_z_ref -> GetXaxis() -> GetBinCenter(eta_z.first) + eta_z_ref -> GetXaxis() -> GetBinWidth(eta_z.first)/2.)
            );
            
            draw_text -> DrawLatex(0.21, 0.85, Form("MBin: %i, #eta bin: %i", Mbin, eta_z.first));
            draw_text -> DrawLatex(0.21, 0.80, Form("pol2: %.2f + %.2f(x-%.2f) + %.2f(x-%.2f)^{2}", bkg_fit_pol2 -> GetParameter(0), bkg_fit_pol2 -> GetParameter(1), bkg_fit_pol2 -> GetParameter(3), bkg_fit_pol2 -> GetParameter(2), bkg_fit_pol2 -> GetParameter(3)));

            c1 -> Print(Form("%s/Reco_DeltaPhi_Multi_Stack.pdf", out_folder_directory.c_str()));
            c1 -> Clear();
        }
    }
    c1 -> Print(Form("%s/Reco_DeltaPhi_Multi_Stack.pdf)", out_folder_directory.c_str()));
    c1 -> Clear();

    cout<<"MainPreparation done"<<endl;
    return;
}

void EtaDistReader::FinaldNdEta()
{
    c1 -> cd();
    c1 -> Print(Form("%s/dNdeta_combine_final_no_correction.pdf(", out_folder_directory.c_str()));
    for (int Mbin = 0; Mbin < N_centrality_bin; Mbin++)
    {   
        // // note : check the ratio of the bin error to the bin content of the three histograms
        // for (int i = 0; i < included_eta_z_map.size(); i++)
        // {
        //     cout<<" "<<endl;
        //     cout<<"1MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_single[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_multi[Mbin] -> GetBinContent(i+1)<<endl;
        // }

        dNdeta_1D_MC[Mbin] -> Scale(1./double(dNdeta_1D_MC[Mbin] -> GetBinWidth(1) ));
        dNdeta_1D_reco_single[Mbin] -> Scale(1./double(dNdeta_1D_reco_single[Mbin] -> GetBinWidth(1) ));
        dNdeta_1D_reco_multi[Mbin] -> Scale(1./double(dNdeta_1D_reco_multi[Mbin] -> GetBinWidth(1) ));

        // // note : check the ratio of the bin error to the bin content of the three histograms
        // for (int i = 0; i < included_eta_z_map.size(); i++)
        // {
        //     cout<<" "<<endl;
        //     cout<<"2MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_single[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_multi[Mbin] -> GetBinContent(i+1)<<endl;
        // }

        for (int i = 0; i < included_eta_z_map.size(); i++)
        {
            dNdeta_1D_MC[Mbin] -> SetBinContent(i+1, dNdeta_1D_MC[Mbin] -> GetBinContent(i+1) / double(N_event_counting_MC[Mbin][i]));
            dNdeta_1D_MC[Mbin] -> SetBinError(i+1, dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / double(N_event_counting_MC[Mbin][i]));

            dNdeta_1D_reco_single[Mbin] -> SetBinContent(i+1, dNdeta_1D_reco_single[Mbin] -> GetBinContent(i+1) / double(N_event_counting[Mbin][i]));
            dNdeta_1D_reco_single[Mbin] -> SetBinError(i+1, dNdeta_1D_reco_single[Mbin] -> GetBinError(i+1) / double(N_event_counting[Mbin][i]));

            dNdeta_1D_reco_multi[Mbin]  -> SetBinContent(i+1, dNdeta_1D_reco_multi[Mbin] -> GetBinContent(i+1) / double(N_event_counting[Mbin][i]));
            dNdeta_1D_reco_multi[Mbin]  -> SetBinError(i+1, dNdeta_1D_reco_multi[Mbin] -> GetBinError(i+1) / double(N_event_counting[Mbin][i]));
        }
        
        cout<<"----------- for the case of method tight ----------------" <<endl;
        for (int i = 0; i < included_eta_z_map.size(); i++)
        {
            // note : to print the ratio between reco track and MC track
            std::cout << "centrality bin : "<<Mbin<<", ";
            for (int bin_i = 0; bin_i < dNdeta_1D_MC[Mbin] -> GetNbinsX(); bin_i++) {
                std::cout <<"--"<<dNdeta_1D_reco_single[Mbin] -> GetBinContent(bin_i+1) <<", "<< dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)<< ", " << Form("%.3f",dNdeta_1D_reco_single[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)) <<"--, ";
            }
            std::cout << std::endl;
        }

        cout<<"----------- for the case of method inclusive ----------------" <<endl;
        for (int i = 0; i < included_eta_z_map.size(); i++)
        {
            // note : to print the ratio between reco track and MC track
            std::cout << "centrality bin : "<<Mbin<<", ";
            for (int bin_i = 0; bin_i < dNdeta_1D_MC[Mbin] -> GetNbinsX(); bin_i++) {
                std::cout <<"--"<<dNdeta_1D_reco_multi[Mbin] -> GetBinContent(bin_i+1) <<", "<< dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)<< ", " << Form( "%.3f", dNdeta_1D_reco_multi[Mbin] -> GetBinContent(bin_i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(bin_i+1)) <<"--, ";
            }
            std::cout << std::endl;
        }

        // note : check the bin content of the three histograms
        // note : and check the bin error of the three histograms
        // for (int i = 0; i < included_eta_z_map.size(); i++)
        // {
        //     cout<<" "<<endl;
        //     cout<<"3MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi[Mbin] -> GetBinContent(i+1)<<endl;
        //     cout<<"3MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1)<<" reco_single: "<<dNdeta_1D_reco_single[Mbin] -> GetBinError(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi[Mbin] -> GetBinError(i+1)<<endl;
        // }

        // // note : check the ratio of the bin error to the bin content of the three histograms
        // for (int i = 0; i < included_eta_z_map.size(); i++)
        // {
        //     cout<<" "<<endl;
        //     cout<<"3MBin: "<<Mbin<<" eta bin: "<<i<<" MC: "<<dNdeta_1D_MC[Mbin] -> GetBinError(i+1) / dNdeta_1D_MC[Mbin] -> GetBinContent(i+1)<<" reco_single: "<<dNdeta_1D_reco_single[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_single[Mbin] -> GetBinContent(i+1)<<" reco_multi: "<<dNdeta_1D_reco_multi[Mbin] -> GetBinError(i+1) / dNdeta_1D_reco_multi[Mbin] -> GetBinContent(i+1)<<endl;
        // }

        dNdeta_1D_MC[Mbin] -> GetYaxis() -> SetRangeUser(0, dNdeta_1D_MC[Mbin] -> GetMaximum() * 1.5);

        dNdeta_1D_MC[Mbin] -> Draw("hist");
        dNdeta_1D_reco_single[Mbin] -> Draw("p same");
        dNdeta_1D_reco_multi[Mbin]  -> Draw("p same");

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", plot_text.c_str()));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[Mbin].c_str()));
       
        c1 -> Print(Form("%s/dNdeta_combine_final_no_correction.pdf", out_folder_directory.c_str()));
        c1 -> Clear();
    }
    c1 -> Print(Form("%s/dNdeta_combine_final_no_correction.pdf)", out_folder_directory.c_str()));

    cout<<"FinaldNdEta done"<<endl;
    return;
}

vector<TH1F *> EtaDistReader::GetDeltaPhi_Multi_stack_1D()
{
    return DeltaPhi_Multi_Stack_hist_out;
}
vector<TH1F *> EtaDistReader::GetdNdeta_1D_MC()
{
    return dNdeta_1D_MC;    
}
vector<TH1F *> EtaDistReader::GetdNdeta_1D_reco_single()
{
    return dNdeta_1D_reco_single;    
}
vector<TH1F *> EtaDistReader::GetdNdeta_1D_reco_multi()
{
    return dNdeta_1D_reco_multi;    
}

#endif