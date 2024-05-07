#include "ReadINTTZ_v2.C"
#include "../sPhenixStyle.C"

void zvtx_study()
{   
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/SemiFinal_EvtZ_MC_ZF_zvtx";
    string input_filename = "INTT_zvtx.root";
    TFile * file_in = TFile::Open(Form("%s/%s",input_directory.c_str(), input_filename.c_str()));
    TTree * tree_in = (TTree *)file_in->Get("tree_Z");
    ReadINTTZ_v2 * read_tree = new ReadINTTZ_v2(tree_in);

    cout<<"tree entry : "<<tree_in -> GetEntries()<<endl;

    SetsPhenixStyle();
    
    TCanvas * c1 = new TCanvas("","",950,800);
    c1 -> cd();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    map<int,int> centrality_map = {
        {5, 0},
        {15, 1},
        {25, 2},
        {35, 3},
        {45, 4},
        {55, 5},
        {65, 6},
        {75, 7},
        {85, 8},
        {95, 9}
    };

    vector<string> centrality_region = {
        "0-5%",
        "5-15%",
        "15-25%",
        "25-35%",
        "35-45%",
        "45-55%",
        "55-65%",
        "65-75%",
        "75-85%",
        "85-95%",
        "0-95%"
    };

    TF1 * gaus_fit = new TF1("gaus_fit","gaus",-40,40);
    gaus_fit -> SetLineColor(2);
    gaus_fit -> SetNpx(2000);

    // TH2F * Z
    TGraph * Z_resolution_centrality_gr = new TGraph();
    Z_resolution_centrality_gr -> SetMarkerStyle(20);
    Z_resolution_centrality_gr -> SetMarkerSize(1.5);
    Z_resolution_centrality_gr -> GetXaxis()->SetTitle("Centrality bin"); 
    Z_resolution_centrality_gr -> GetYaxis()->SetTitle("#DeltaZ width");

    vector<TH1F *> Z_resolution; Z_resolution.clear();
    for (int i = 0; i < centrality_region.size(); i++)
    {
        Z_resolution.push_back(new TH1F("","",100,-40,40)); // note : unit: mm
        Z_resolution[i]->GetXaxis()->SetTitle("#DeltaZ (Reco - True) [mm]");
        Z_resolution[i]->GetYaxis()->SetTitle("Entry");
    }

    for (int event_i = 0; event_i < tree_in -> GetEntries(); event_i++)
    {
        read_tree->LoadTree(event_i);
        read_tree->GetEntry(event_i);
        
        if (read_tree -> nclu_inner == -1)   {continue;}
        if (read_tree -> nclu_outer == -1)   {continue;}
        if (read_tree -> good_zvtx_tag != 1) {continue;}

        Z_resolution[ centrality_map[read_tree->Centrality_bin] ] -> Fill( read_tree->LB_Gaus_Mean_mean - read_tree->MC_true_zvtx );
        Z_resolution[ Z_resolution.size() - 1 ]                   -> Fill( read_tree->LB_Gaus_Mean_mean - read_tree->MC_true_zvtx );
        // cout<<"test : "<<event_i<<" read_tree->Centrality_bin "<<read_tree->Centrality_bin<<endl;
        // cout<<read_tree->MC_true_zvtx<<endl;
    }

    c1 -> Print(Form("%s/INTT_Z_resolution_centrality.pdf(", input_directory.c_str()));
    for (int i = 0; i < Z_resolution.size(); i++)
    {
        Z_resolution[i] -> SetMinimum(0);
        Z_resolution[i] -> Draw("hist");
        
        if (i == Z_resolution.size() - 1) 
        {
            Z_resolution[i] -> Fit(gaus_fit,"NQ","", Z_resolution[i]->GetStdDev() * -1, Z_resolution[i]->GetStdDev());
            gaus_fit -> SetRange(gaus_fit->GetParameter(1) - 1.5 * gaus_fit->GetParameter(2), gaus_fit->GetParameter(1) + 1.5 * gaus_fit->GetParameter(2));
        }
        else 
        {
            Z_resolution[i] -> Fit(gaus_fit,"NQ");
        }

        
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", "Simulation"));
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        gaus_fit -> Draw("lsame");

        Z_resolution_centrality_gr -> SetPoint(i, i, gaus_fit->GetParameter(2));

        c1 -> Print(Form("%s/INTT_Z_resolution_centrality.pdf", input_directory.c_str()));
        c1 -> Clear();
    }
    c1 -> Print(Form("%s/INTT_Z_resolution_centrality.pdf)", input_directory.c_str()));
    c1 -> Clear();

    Z_resolution_centrality_gr -> Draw("ap");
    Z_resolution_centrality_gr -> GetXaxis() -> SetLimits(-1,11);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} %s", "Simulation"));
    c1 -> Print(Form("%s/Z_resolution_centrality_gr.pdf", input_directory.c_str()));

    for (int i = 0; i < Z_resolution_centrality_gr->GetN(); i++)
    {
        cout<<i<<" "<<Z_resolution_centrality_gr->GetPointY(i)<<endl;
    }
}