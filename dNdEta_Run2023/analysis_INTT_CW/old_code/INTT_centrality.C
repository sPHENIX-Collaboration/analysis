#include "sPhenixStyle.C"

void INTT_centrality()
{
    string mother_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869";
    string file_name = "beam_inttall-00020869-0000_event_base_ana_cluster_full_survey_3.32_excludeR40000_200kEvent_3HotCut";
    SetsPhenixStyle();

    TFile * file_in = new TFile(Form("%s/%s.root",mother_folder_directory.c_str(),file_name.c_str()),"read");
    TTree * tree = (TTree *)file_in->Get("tree_clu");

    int N_hits;
    int N_cluster_inner;
    int N_cluster_outer;
    vector<int>* column_vec = new vector<int>();
    vector<double>* avg_chan_vec = new vector<double>();
    vector<int>* sum_adc_vec = new vector<int>();
    vector<int>* sum_adc_conv_vec = new vector<int>();
    vector<int>* size_vec = new vector<int>();
    vector<double>* x_vec = new vector<double>();
    vector<double>* y_vec = new vector<double>();
    vector<double>* z_vec = new vector<double>();
    vector<int>* layer_vec = new vector<int>();
    vector<double>* phi_vec = new vector<double>();

    tree -> SetBranchStatus("*",0);
    tree -> SetBranchStatus("nclu_inner",1);
    tree -> SetBranchStatus("nclu_outer",1);
    tree -> SetBranchAddress("nclu_inner",&N_cluster_inner);
    tree -> SetBranchAddress("nclu_outer",&N_cluster_outer);
    
    // tree -> SetBranchAddress("nhits",&N_hits);
    // tree -> SetBranchAddress("column", &column_vec);
    // tree -> SetBranchAddress("avg_chan", &avg_chan_vec);
    // tree -> SetBranchAddress("sum_adc", &sum_adc_vec);
    // tree -> SetBranchAddress("sum_adc_conv", &sum_adc_conv_vec);
    // tree -> SetBranchAddress("size", &size_vec);
    // tree -> SetBranchAddress("x", &x_vec);
    // tree -> SetBranchAddress("y", &y_vec);
    // tree -> SetBranchAddress("z", &z_vec);
    // tree -> SetBranchAddress("layer", &layer_vec);
    // tree -> SetBranchAddress("phi", &phi_vec);

    TLine * coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(2);
    // coord_line -> SetLineStyle(2);

    TH1F * INTT_N_clu = new TH1F("","",100,0,8000);
    INTT_N_clu -> GetXaxis() -> SetTitle("N clusters");
    INTT_N_clu -> GetYaxis() -> SetTitle("Entry");

    vector<long long>N_clu_vec; N_clu_vec.clear();

    for (int i = 0; i < tree -> GetEntries(); i++)
    {
        tree -> GetEntry(i);

        if (N_cluster_inner == -1 || N_cluster_outer == -1) continue;

        INTT_N_clu -> Fill(N_cluster_inner + N_cluster_outer);
        N_clu_vec.push_back(N_cluster_inner + N_cluster_outer);
    }

    double total_entry = INTT_N_clu -> Integral(1,100);

    int sort_clu_index[N_clu_vec.size()];
    cout<<N_clu_vec.size()<<endl;
    TMath::Sort(int(N_clu_vec.size()), &N_clu_vec[0], sort_clu_index);

    
    TCanvas * c1 = new TCanvas("","",1200,800);
    INTT_N_clu -> Draw("hist");
    c1 -> SetLogy(1);

    for (int i = 1; i < 20; i++)
    {
        cout<<"centrality line : "<<i * 5<<" %%, "<< int(total_entry * (i * 5. /100.))<<" entry_line : "<<N_clu_vec[ sort_clu_index[ int(total_entry * (i * 5. /100.)) ] ]<<endl;

        coord_line -> DrawLine(N_clu_vec[ sort_clu_index[ int(total_entry * (i * 5. /100.)) ] ], 0, N_clu_vec[ sort_clu_index[ int(total_entry * (i * 5. /100.)) ] ], INTT_N_clu -> GetBinContent( int( N_clu_vec[ sort_clu_index[ int(total_entry * (i * 5. /100.)) ] ] / double(INTT_N_clu -> GetBinWidth(1)) ) + 1 ));
    }

    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->DrawLatex(gPad->GetLeftMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Work-in-progress");
    ltx->DrawLatex(0.48, 0.835, "Run 20869");
    ltx->DrawLatex(0.48, 0.785, "Au+Au #sqrt{s_{NN}} = 200 GeV");
    ltx->DrawLatex(0.48, 0.735, "Interval : 5%");

    c1 -> Print(Form("%s/folder_%s_advanced/INTT_centrality.pdf",mother_folder_directory.c_str(),file_name.c_str()));
    

}