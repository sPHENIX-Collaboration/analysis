#include "DrawPlot.h"
#include "../sigmaEff.h"

void draw_angle_diff()
{   
    string folder_direction_1 = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC/folder_MC_ZF_1_30400_advanced";
    string file_name_1 = "MC_angle_diff.root";
    TFile * f1 = TFile::Open(Form("%s/%s", folder_direction_1.c_str(),file_name_1.c_str()));
    TTree * t1 = (TTree *) f1 -> Get("tree");
    vector<double> * vec_read_1 = new vector<double>();
    t1 -> SetBranchAddress("angle_diff", &vec_read_1);

    vector<float> vec1; vec1.clear();
    cout<<"start running 1"<<endl;
    for (int i = 0; i < 1000; i++){
        t1 -> GetEntry(i);

        if (i % 100 == 0) cout<<"vec1 running "<<i<<endl;

        for (int i1 = 0; i1 < vec_read_1 -> size(); i1++){
            if (vec_read_1 -> at(i1) < 1)
                vec1.push_back(vec_read_1 -> at(i1));
        }
    }

    cout<<"start sigmaEff_avg, input size : "<<vec1.size()<<endl;
    vector<float> angle_diff_width = sigmaEff_avg(vec1, 0.341);
    cout<<"start sigmaEff_avg"<<endl;

    string folder_direction_2 = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC/folder_MC_ZF_1_30400_advanced_inner_phi_rotate";
    string file_name_2 = "MC_angle_diff.root";
    TFile * f2 = TFile::Open(Form("%s/%s", folder_direction_2.c_str(),file_name_2.c_str()));
    TTree * t2 = (TTree *) f2 -> Get("tree");

    cout<<"making plot"<<endl;
    DrawPlot * plot1 = new DrawPlot(1000, t1, "angle_diff", t2, "angle_diff", 100,0.,1.,"#Delta#phi [degree]", "A.U.", 0, 1, 50, -1);
    plot1 -> c1 -> cd();
    

    TLine * effi_sig_range_line = new TLine();
    effi_sig_range_line -> SetLineWidth(3);
    effi_sig_range_line -> SetLineColor(TColor::GetColor("#A08144"));
    effi_sig_range_line -> SetLineStyle(2);
    effi_sig_range_line -> DrawLine(angle_diff_width[1],0,angle_diff_width[1],plot1 -> hist1 -> GetMaximum());
    effi_sig_range_line -> DrawLine(angle_diff_width[2],0,angle_diff_width[2],plot1 -> hist1 -> GetMaximum());    
    
    TLatex *draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.04);
    draw_text -> DrawLatex(0.45, 0.87, Form("EffiSig min : %.2f mm",angle_diff_width[1]));
    draw_text -> DrawLatex(0.45, 0.83, Form("EffiSig max : %.2f mm",angle_diff_width[2]));
    draw_text -> DrawLatex(0.45, 0.79, Form("EffiSig avg : %.2f mm",angle_diff_width[0]));

    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "#it{#bf{sPHENIX INTT}} Simulation");

    plot1 -> canvas_draw("/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC","angle_diff.pdf");

    








    // string folder_direction_2 = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC/folder_MC_ZF_1_30400_advanced_inner_phi_rotate";
    // string file_name_2 = "MC_angle_diff.root";
    // TFile * f2 = TFile::Open(Form("%s/%s", folder_direction_2.c_str(),file_name_2.c_str()));
    // TTree * t2 = (TTree * ) f2 -> Get("tree");
    // vector<double> * vec_read_2 = new vector<double>();
    // t2 -> SetBranchAddress("angle_diff", &vec_read_2);
    
    // vector<double> vec2; vec2.clear();

    // cout<<"start running 2"<<endl;
    // for (int i = 0; i < 2000; i++){
    //     t2 -> GetEntry(i);
    //     for (int i1 = 0; i1 < vec_read_2 -> size(); i1++){
    //         vec2.push_back(vec_read_2 -> at(i1));
    //     }
    // }

    // cout<<"making plot"<<endl;
    // DrawPlot * plot1 = new DrawPlot(vec1, vec2, 100,0.,1.,"#Delta#phi [degree]", "A.U.", 0, 1, 50, -1);
    // plot1 -> canvas_draw("/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC","angle_diff.pdf");

}