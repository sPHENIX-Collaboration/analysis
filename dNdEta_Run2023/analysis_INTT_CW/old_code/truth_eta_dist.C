// #include "../DST_MC/INTTReadTree.h"
#include "../../INTTDSTchain.C"

void Characterize_Pad (TPad *pad, float left = 0.15, float right = 0.1, float top = 0.1, float bottom = 0.12, bool set_logY = false, int setgrid_bool = 0)
{
	if (setgrid_bool == true) {pad -> SetGrid (1, 1);}
	pad -> SetLeftMargin   (left);
	pad -> SetRightMargin  (right);
	pad -> SetTopMargin    (top);
	pad -> SetBottomMargin (bottom);
    pad -> SetTicks(1,1);
    if (set_logY == true)
    {
        pad -> SetLogy (1);
    }
	
}

void truth_eta_dist()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams";
    string file_name = "MC_ZF_xyvtx";
    string out_folder_directory = input_directory + "/Eta_test_" + file_name;
    string MC_list_name = "file_list.txt";
    string tree_name = "EventTree";

    double INTT_layer_R = 7.188; // note: the innermost, B0L0
    double runEvent = 80000;

    // INTTReadTree * INTTClu = new INTTReadTree(data_type, input_directory, MC_list_name, tree_name, clu_size_cut, clu_sum_adc_cut);

    TChain * chain_in = new TChain(tree_name.c_str());
    INTTDSTchain * inttDSTMC = new INTTDSTchain(chain_in, input_directory, MC_list_name);
    long long N_event = chain_in->GetEntries();
    std::printf("inttDSTMC N event : %lli \n", N_event);

    // cout<<"Total event : "<<INTTClu -> GetNEvt()<<endl;

    TCanvas * c3 = new TCanvas("c3","c3",900,800); c3 -> cd();
    // TPad *pad_obj = new TPad(Form("pad_obj"), "", 0.0, 0.0, 1.0, 1.0);
    // Characterize_Pad(pad_obj, 0.18,  0.1,  0.1,  0.12, 0, 0);
    // pad_obj -> SetTicks(1,1);
    // pad_obj -> Draw();
    // pad_obj -> cd();

    TH1F * true_track_Eta_1D = new TH1F("","",145,-2.9,2.9);
    true_track_Eta_1D -> SetMarkerStyle(20);
    true_track_Eta_1D -> SetMarkerSize(0.8);
    true_track_Eta_1D -> SetMarkerColor(TColor::GetColor("#1A3947"));
    true_track_Eta_1D -> SetLineColor(TColor::GetColor("#1A3947"));
    true_track_Eta_1D -> SetLineWidth(2);
    true_track_Eta_1D -> GetYaxis() -> SetTitle("dN_{ch}/d#eta");
    true_track_Eta_1D -> GetXaxis() -> SetTitle("#eta");
    true_track_Eta_1D -> GetYaxis() -> SetRangeUser(0,50);
    true_track_Eta_1D -> SetTitleSize(0.06, "X");
    true_track_Eta_1D -> SetTitleSize(0.06, "Y");
	true_track_Eta_1D -> GetXaxis() -> SetTitleOffset (0.71);
    true_track_Eta_1D -> GetYaxis() -> SetTitleOffset (1.1);
	true_track_Eta_1D -> GetXaxis() -> CenterTitle(true);
    true_track_Eta_1D -> GetYaxis() -> CenterTitle(true);

    TH2F * true_track_EtaPhi_2D = new TH2F("","",1450,-2.9,2.9, 300, -4, 4);
    true_track_EtaPhi_2D -> GetYaxis() -> SetTitle("#phi");
    true_track_EtaPhi_2D -> GetXaxis() -> SetTitle("#eta");
    // true_track_EtaPhi_2D -> GetYaxis() -> SetRangeUser(0,50);
    // true_track_EtaPhi_2D -> SetTitleSize(0.06, "X");
    // true_track_EtaPhi_2D -> SetTitleSize(0.06, "Y");
	// true_track_EtaPhi_2D -> GetXaxis() -> SetTitleOffset (0.71);
    // true_track_EtaPhi_2D -> GetYaxis() -> SetTitleOffset (1.1);
	// true_track_EtaPhi_2D -> GetXaxis() -> CenterTitle(true);
    // true_track_EtaPhi_2D -> GetYaxis() -> CenterTitle(true);

    TH2F * true_track_EtaZ_2D = new TH2F("","",1450,-2.9,2.9, 300, -23, -17);
    true_track_EtaZ_2D -> GetYaxis() -> SetTitle("Z");
    true_track_EtaZ_2D -> GetXaxis() -> SetTitle("#eta");

    TH1F * true_track_PID = new TH1F("","",20000,-10000,10000);
    true_track_PID -> GetYaxis() -> SetTitle("PID");
    true_track_PID -> GetXaxis() -> SetTitle("Entry");


    cout<<"true_track_Eta_1D, binwidth : "<<true_track_Eta_1D->GetBinWidth(1)<<endl;
    double good_event_counting = 0;

    double z_range_l = -23;
    double z_range_r = -17;

    std::unordered_map<int, int> PID_countMap;

    for (int event_i = 0; event_i < 80000; event_i ++)
    {
        inttDSTMC->LoadTree(event_i);
        inttDSTMC->GetEntry(event_i);

        double z = inttDSTMC -> TruthPV_trig_z;

        if (inttDSTMC -> NTruthVtx != 1) {continue;}
        // if (event_i % 100 == 0){cout<<inttDSTMC -> centrality_bimp<<endl;}
        // if (inttDSTMC -> centrality_bimp != 5) {continue;}
        // if (z < z_range_l || z > z_range_r) {continue;}
        
        double INTT_eta_acceptance_l = -0.5 * TMath::Log((sqrt(pow(-23.-z_range_l,2)+pow(INTT_layer_R,2))-(-23.-z_range_l)) / (sqrt(pow(-23.-z_range_l,2)+pow(INTT_layer_R,2))+(-23.-z_range_l))); // note : left
        double INTT_eta_acceptance_r =  -0.5 * TMath::Log((sqrt(pow(23.-z_range_r,2)+pow(INTT_layer_R,2))-(23.-z_range_r)) / (sqrt(pow(23.-z_range_r,2)+pow(INTT_layer_R,2))+(23.-z_range_r))); // note : right
        

        good_event_counting += 1;

        // if (event_i % 100 == 0){cout<<"z : "<<z<<" eta : "<<INTT_eta_acceptance_l<<" "<<INTT_eta_acceptance_r<<" N tracks : "<<inttDSTMC->UniqueAncG4P_Eta->size()<<" NClus : "<<inttDSTMC->NClus<<endl;}

        for (int track_i = 0; track_i < inttDSTMC->UniqueAncG4P_Eta->size(); track_i++)
        {
            if (inttDSTMC->UniqueAncG4P_Eta->at(track_i) > INTT_eta_acceptance_l && inttDSTMC->UniqueAncG4P_Eta->at(track_i) < INTT_eta_acceptance_r)
            {
                true_track_Eta_1D -> Fill(inttDSTMC->UniqueAncG4P_Eta->at(track_i));
                // double fillID = true_track_PID -> Fill(inttDSTMC->UniqueAncG4P_PID->at(track_i));
                // if (fillID == -1) {cout<<"PID out of the set range : "<<inttDSTMC->UniqueAncG4P_PID->at(track_i)<<endl;}
                int value = inttDSTMC->UniqueAncG4P_PID->at(track_i);
                PID_countMap[value]++;

                // true_track_EtaZ_2D -> Fill(inttDSTMC->UniqueAncG4P_Eta->at(track_i),z);
            }

            // true_track_Eta_1D -> Fill(inttDSTMC->UniqueAncG4P_Eta->at(track_i));
            // true_track_EtaPhi_2D -> Fill(inttDSTMC->UniqueAncG4P_Eta->at(track_i),inttDSTMC->UniqueAncG4P_Phi->at(track_i));

            true_track_EtaZ_2D -> Fill(inttDSTMC->UniqueAncG4P_Eta->at(track_i),z);
        }

        cout<<"z : "<<z<<" eta : "<<INTT_eta_acceptance_l<<" "<<INTT_eta_acceptance_r<<" N tracks : "<<inttDSTMC->UniqueAncG4P_Eta->size()<<" NClus : "<<inttDSTMC->NClus<<endl;
        // break;

    }

    // true_track_Eta_1D -> Scale(1./double(runEvent * true_track_Eta_1D -> GetBinWidth(1) ));
    // true_track_Eta_1D -> Scale(1./double(runEvent));
    // true_track_Eta_1D -> GetYaxis() -> SetRangeUser(0,800);

    cout<<" good_event_counting : "<<good_event_counting<<endl;

    true_track_Eta_1D -> Scale(1./double(true_track_Eta_1D -> GetBinWidth(1) ));
    true_track_Eta_1D -> Scale(1./double(good_event_counting));
    true_track_Eta_1D -> GetYaxis() -> SetRangeUser(0,1000);

    true_track_Eta_1D -> Draw("hist");
    c3->Print(Form("%s/true_track_Eta_1D.pdf",out_folder_directory.c_str()));

    // pad_obj -> Clear();

    true_track_EtaPhi_2D -> Draw("colz0");
    c3->Print(Form("%s/true_track_EtaPhi_2D.pdf",out_folder_directory.c_str()));

    true_track_EtaZ_2D -> Draw("colz0");
    c3->Print(Form("%s/true_track_EtaZ_2D.pdf",out_folder_directory.c_str()));
    c3 -> Clear();

    std::cout << "PID   Count" << std::endl;
    for (const auto& pair : PID_countMap) {
        std::cout << pair.first << "       " << pair.second << std::endl;
    }

    // c3 -> SetLogy(1);
    // true_track_PID -> Draw("hist");
    // c3->Print(Form("%s/true_track_PID.pdf",out_folder_directory.c_str()));
    // c3 -> Clear();

}