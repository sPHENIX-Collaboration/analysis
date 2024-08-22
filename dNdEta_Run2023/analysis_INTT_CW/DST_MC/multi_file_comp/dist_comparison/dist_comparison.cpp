#include "sPhenixStyle.C"
#include "../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;


class DistComp
{
    public:
        DistComp(vector<vector<string>> input_file_full_info_in, vector<string> checked_hist_name_in, string output_folder_name_in, bool rebin_tag_in = false, int rebin_Nbins_in = 2);
    protected:
        // note : vector<vector<string>> input_file_full_info { {data_type, full_input_file_directory, abbreviation_of_file} }
        // note : vector<vector<string>> input_file_full_info { 
        // note :     {
        // note :         data_type, 
        // note :         full_input_file_directory, 
        // note :         abbreviation_of_file // note : corr. / test / data / data_z_shifted <------> Original / Inner cluster #phi rotated by #pi
        // note :     } 
        // note : }
        vector<vector<string>> input_file_full_info;
        vector<string> checked_hist_name;
        string output_folder_name;
        string sph_labeling;
        bool rebin_tag;
        int rebin_Nbins;
        string rebin_text;

        void Object_init();
        void PrintPlots();

        vector<TFile *> file_in;
        vector<vector<TH1F*>> hist_vec;

        map<string, string> hist_Xtitle_correction = {
            {"exclusive_cluster_inner_eta","Cluster inner #eta"},
            {"exclusive_cluster_inner_phi","Cluster inner #phi"},
            {"exclusive_cluster_outer_eta","Cluster outer #eta"},
            {"exclusive_cluster_outer_phi","Cluster outer #phi"}
        };

        TCanvas * c1;
        TLegend * legend;
        TLatex * ltx;
        TLatex * draw_text;
        TLine * coord_line;

        vector<string> color_code_vec = {"#c48045", "#D8364D", "#7c9885", "#28666e", "#000000"};
};


DistComp::DistComp(vector<vector<string>> input_file_full_info_in, vector<string> checked_hist_name_in, string output_folder_name_in, bool rebin_tag_in, int rebin_Nbins_in)
: input_file_full_info(input_file_full_info_in),
checked_hist_name(checked_hist_name_in),
output_folder_name(output_folder_name_in),
rebin_tag(rebin_tag_in),
rebin_Nbins(rebin_Nbins_in)
{
    system(Form("if [ ! -d %s ]; then mkdir %s; fi", output_folder_name.c_str(), output_folder_name.c_str()));

    file_in.clear();
    hist_vec.clear();
    hist_vec=vector<vector<TH1F*>>(input_file_full_info.size());

    
    bool found_data_tag = false;
    for (int i = 0; i < input_file_full_info.size(); i++) {
        if (input_file_full_info[i][0] == "data") {
            found_data_tag = true;
            break;
        }
    }
    sph_labeling = (found_data_tag) ? "Work-in-progress" : "Simulation";

    rebin_text = (rebin_tag) ? Form("rebin%d_", rebin_Nbins) : "";

    Object_init();
    PrintPlots();
}

void DistComp::Object_init()
{
    SetsPhenixStyle();

    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    double legend_text_size = 0.03;
    double legend_upper_y = 0.9;
    legend = new TLegend(
        0.45, // note : x1
        legend_upper_y - (input_file_full_info.size()) * (legend_text_size + 0.005), // note : y1
        0.8, // note : x2
        legend_upper_y // note : y2
    );
    // legend -> SetMargin(0);
    legend->SetTextSize(legend_text_size);

    c1 = new TCanvas("c1","",950,800);

    coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);


    for (int file_i = 0; file_i < input_file_full_info.size(); file_i++)
    {
        file_in.push_back(TFile::Open(input_file_full_info[file_i][1].c_str()));
        for (string hist_name : checked_hist_name)
        {
            cout<<"test, working on : "<<hist_name<<endl;

            if (rebin_tag){
                hist_vec[file_i].push_back( (TH1F*)((TH1F*)file_in[file_i]->Get(hist_name.c_str()))->Rebin(rebin_Nbins, ((TH1F*)file_in[file_i]->Get(hist_name.c_str())) -> GetName() ) );
            }
            else{
                hist_vec[file_i].push_back( (TH1F*)file_in[file_i]->Get(hist_name.c_str()) );
            }
            
            if (hist_Xtitle_correction.find(hist_name) != hist_Xtitle_correction.end())
            {
                hist_vec[file_i].back()->GetXaxis()->SetTitle( hist_Xtitle_correction[hist_name].c_str() );
            }
            else
            {
                hist_vec[file_i].back()->GetXaxis()->SetTitle( ((TH1F*)file_in[file_i]->Get(hist_name.c_str())) -> GetXaxis() -> GetTitle() );
            }

            hist_vec[file_i].back()->SetTitle( ((TH1F*)file_in[file_i]->Get(hist_name.c_str())) -> GetName() );
            hist_vec[file_i].back()->GetYaxis()->SetTitle( ((TH1F*)file_in[file_i]->Get(hist_name.c_str())) -> GetYaxis() -> GetTitle() );
            hist_vec[file_i].back()->SetLineColor(TColor::GetColor(color_code_vec[file_i].c_str()));
            hist_vec[file_i].back()->SetMarkerColor(TColor::GetColor(color_code_vec[file_i].c_str()));
            hist_vec[file_i].back()->SetMarkerSize(0.8);
            // for (int i = 1; i <= hist_vec[file_i].back()->GetNbinsX(); i++)
            // {
            //     if (hist_vec[file_i].back()->GetBinCenter(i) > 1.5) {hist_vec[file_i].back()->SetBinContent(i,0);}
            // }
            hist_vec[file_i].back()->Scale( 1. / hist_vec[file_i].back() -> Integral(-1,-1) );
            hist_vec[file_i].back()->GetXaxis()->SetNdivisions(705);
        }
    }
}

void DistComp::PrintPlots()
{
    for (int plot_i = 0; plot_i < checked_hist_name.size(); plot_i++)
    {
        c1 -> cd();

        for (int file_i = 0; file_i < input_file_full_info.size(); file_i++)
        {
            string draw_text = (input_file_full_info[file_i][0] == "data") ? "p" : "hist";
            if (file_i == 0 && draw_text == "p") { draw_text = "ep"; }
            else if (file_i == 0 && draw_text == "hist") { draw_text = "hist"; }
            else { draw_text = draw_text += " same";}

            string legned_text = (input_file_full_info[file_i][0] == "data") ? "ep" : "f";

            if (file_i == 0)
            {
                hist_vec[file_i][plot_i] -> Draw(draw_text.c_str());
                hist_vec[file_i][plot_i] -> SetMaximum( 1.8 * hist_vec[file_i][plot_i]->GetBinContent(hist_vec[file_i][plot_i]->GetMaximumBin()) );
                // hist_vec[file_i][plot_i] -> SetMinimum(0);
            }
            else
            {
                hist_vec[file_i][plot_i] -> Draw(draw_text.c_str());
            }
            legend -> AddEntry(hist_vec[file_i][plot_i], input_file_full_info[file_i][2].c_str(), legned_text.c_str());
        }

        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));
        legend -> Draw("same");
        c1 -> Print(Form("%s/%s%s.pdf", output_folder_name.c_str(), rebin_text.c_str(), checked_hist_name[plot_i].c_str()));

        c1 -> SetLogy(1);
        hist_vec[0][plot_i] -> SetMaximum(  pow(10, TMath::Log10(hist_vec[0][plot_i]->GetBinContent(hist_vec[0][plot_i]->GetMaximumBin())) + 2.5) );
        c1 -> Print(Form("%s/%slog_%s.pdf", output_folder_name.c_str(), rebin_text.c_str(), checked_hist_name[plot_i].c_str()));
        c1 -> SetLogy(0);

        c1 -> Clear();
        legend -> Clear();
    }
}

void dist_comparison()
{

    string MC_tracklet_merged_folder_prefix = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test";
    string MC_tracklet_folder_name = "evt_tracklet_NarrowZvtx_n205mm_to_n195mm";

    string tracklet_merged_folder_suffix = "complete_file/merged_file_folder";
    
    string data_tracklet_merged_folder_prefix = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test";
    string data_tracklet_folder_name_1 = "evt_tracklet_NarrowZvtx_n205mm_to_n195mm";
    // string data_tracklet_folder_name_2 = "evt_tracklet_NarrowZvtx_n205mm_to_n195mm";

    vector<vector<string>> input_file_full_info = {
        // {
        //     "MC",
        //     Form("%s/evt_tracklet_Original_MC_zvtx_weighting/%s/merged_hist_full.root",MC_tracklet_merged_folder_prefix.c_str(),tracklet_merged_folder_suffix.c_str()),
        //     "MC zvtx weighted"
        // },
        // {
        //     "MC",
        //     Form("%s/evt_tracklet_Original_MC_NO_zvtx_weighting/%s/merged_hist_full.root",MC_tracklet_merged_folder_prefix.c_str(),tracklet_merged_folder_suffix.c_str()),
        //     "MC NO weighted"
        // },
        // {
        //     "data",
        //     Form("%s/evt_tracklet_original/%s/merged_hist_full.root", data_tracklet_merged_folder_prefix.c_str(), tracklet_merged_folder_suffix.c_str()),
        //     "Data"
        // }

        // {
        //     "MC",
        //     Form("%s/evt_tracklet_Original_MC_zvtxshifted_weighting/%s/merged_hist_full.root",MC_tracklet_merged_folder_prefix.c_str(),tracklet_merged_folder_suffix.c_str()),
        //     "MC zvtx_shifted weighted"
        // },
        // {
        //     "data",
        //     Form("%s/evt_tracklet_INTTz_shifted/%s/merged_hist_full.root", data_tracklet_merged_folder_prefix.c_str(), tracklet_merged_folder_suffix.c_str()),
        //     "Data zvtx_shifted"
        // }

        // {
        //     "MC",
        //     Form("%s/%s/%s/merged_hist_full.root",MC_tracklet_merged_folder_prefix.c_str(), MC_tracklet_folder_name.c_str(), tracklet_merged_folder_suffix.c_str()),
        //     "MC"
        // },
        // {
        //     "data",
        //     Form("%s/%s/%s/merged_hist_full.root", data_tracklet_merged_folder_prefix.c_str(), data_tracklet_folder_name_1.c_str(), tracklet_merged_folder_suffix.c_str()),
        //     "Data, z shifted"
        // },
        // {
        //     "data",
        //     Form("%s/%s/%s/merged_hist_full.root", data_tracklet_merged_folder_prefix.c_str(), data_tracklet_folder_name_2.c_str(), tracklet_merged_folder_suffix.c_str()),
        //     "Data, original"
        // }

        {
            "MC",
            Form("%s/%s/%s/merged_hist_full.root",MC_tracklet_merged_folder_prefix.c_str(), MC_tracklet_folder_name.c_str(), tracklet_merged_folder_suffix.c_str()),
            "MC"
        },
        {
            "data",
            Form("%s/%s/%s/merged_hist_full.root", data_tracklet_merged_folder_prefix.c_str(), data_tracklet_folder_name_1.c_str(), tracklet_merged_folder_suffix.c_str()),
            "Data"
        },

    };

    vector<string> checked_hist_name = {
        "exclusive_NClus_inner",
        "exclusive_NClus_outer",
        "exclusive_NClus_sum",
        "exclusive_cluster_all_eta",
        "exclusive_cluster_all_phi",
        "exclusive_cluster_inner_eta",
        "exclusive_cluster_inner_phi",
        "exclusive_cluster_outer_eta",
        "exclusive_cluster_outer_phi",
        "exclusive_cluster_inner_adc",
        "exclusive_cluster_outer_adc",
        "exclusive_loose_tracklet_eta",
        "exclusive_loose_tracklet_phi",
        "exclusive_tight_tracklet_eta",
        "exclusive_tight_tracklet_phi"
    };

    string output_folder_name = data_tracklet_merged_folder_prefix + "/" + data_tracklet_folder_name_1 + "/" + tracklet_merged_folder_suffix + "/dist_comparison_folder";

    DistComp * dist_comp = new DistComp(input_file_full_info, checked_hist_name, output_folder_name, false, 2);

    return;
}