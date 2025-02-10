#include "vtxZDist_comp.h"

vtxZDist_comp::vtxZDist_comp(
        std::string sPH_labeling_in,
        std::string MC_labeling_in,
        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> data_input_directory_pair_vec_in, // note: directory, and description
        std::vector<std::pair<std::string, std::pair<std::string, std::string>>> MC_input_directory_pair_vec_in, // note: directory, and description
        std::map<std::string, std::vector<std::tuple<double,double,std::string>>> labelling_vec_map_in, // note: starting XY, and description 
        std::string output_directory_in,

        bool WithVtxZReWeighting_in // note: MC with reweighting
):
    sPH_labeling(sPH_labeling_in),
    MC_labeling(MC_labeling_in),
    data_input_directory_pair_vec(data_input_directory_pair_vec_in),
    MC_input_directory_pair_vec(MC_input_directory_pair_vec_in),
    labelling_vec_map(labelling_vec_map_in),
    output_directory(output_directory_in),

    WithVtxZReWeighting(WithVtxZReWeighting_in),

    logy_max_factor(1000),
    lineary_max_factor(1.5)
{
    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi", output_directory.c_str(), output_directory.c_str()));    

    for (auto &pair : data_input_directory_pair_vec) // note : {direcotry -> <description, folder_name>}
    {
        std::cout<<"data_input_directory_pair_vec: "<<pair.first<<" "<<pair.second.first<<" "<<pair.second.second<<std::endl;
        system(Form("if [ ! -d %s/Data_%s ]; then mkdir -p %s/Data_%s; fi", output_directory.c_str(), pair.second.second.c_str(), output_directory.c_str(), pair.second.second.c_str()));    
    }

    for (auto &pair : MC_input_directory_pair_vec)
    {
        std::cout<<"MC_input_directory_pair_vec: "<<pair.first<<" "<<pair.second.first<<" "<<pair.second.second<<std::endl;
        system(Form("if [ ! -d %s/MC_%s ]; then mkdir -p %s/MC_%s; fi", output_directory.c_str(), pair.second.second.c_str(), output_directory.c_str(), pair.second.second.c_str()));    
    }

    data_input_directory_map.clear();
    for (auto &pair : data_input_directory_pair_vec)
    {
        data_input_directory_map.insert(
            std::make_pair(
                pair.first,
                pair.second
            )
        );
    }

    MC_input_directory_map.clear();
    for (auto &pair : MC_input_directory_pair_vec)
    {
        MC_input_directory_map.insert(
            std::make_pair(
                pair.first,
                pair.second
            )
        );
    }

    SetsPhenixStyle();
    TGaxis::SetMaxDigits(4);
    c1 = new TCanvas("c1","c1",950,800);

    ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    All_leg = new TLegend(0.4, 0.82, 0.8, 0.88);
    All_leg -> SetBorderSize(0);
    All_leg -> SetTextSize(0.025);

    All_leg_long = new TLegend(0.7, 0.5, 0.8, 0.88);
    All_leg_long -> SetBorderSize(0);

    sub_ltx = new TLatex();
    sub_ltx->SetNDC();
    sub_ltx->SetTextSize(0.03);
    // ltx->SetTextAlign(31);

    PrepareInputFiles();

    nCentrality_bin = Constants::centrality_edges.size() - 1;
}

std::string vtxZDist_comp::string_convertor(std::string input_string)
{
    std::string output_string;

    for (int i = 0; i < input_string.size(); i++)
    {
        if (input_string[i] == ' ')
        {
            output_string += "_";
        }
        else if (input_string[i] == '.')
        {
            output_string += "p";
        }
        else if (input_string[i] == ',')
        {
            output_string += "_";
        }
        else
        {
            output_string += input_string[i];
        }
    }

    // std::cout<<"before: "<<input_string<<" after: "<<output_string<<std::endl;

    return output_string;
}

void vtxZDist_comp::PrepareInputFiles()
{
    data_h1D_map_map.clear();
    data_h2D_map_map.clear();
    MC_h1D_map_map.clear();
    MC_h2D_map_map.clear();

    // note : for data ----------------------------------------------------------------------------------------------------
    for (auto &pair : data_input_directory_pair_vec) // note : <file_name, description>
    {
        file_data_vec.push_back(
            TFile::Open(Form("%s",pair.first.c_str()))
        );

        if (file_data_vec.back() == nullptr)
        {
            std::cout << "Error: file_in can not be opened" << std::endl;
            exit(1);
        }

        // note : initialize the vector
        data_h2D_map_map.insert( // note : file_name, hist_name, hist
            std::make_pair(
                pair.first,
                std::map<std::string, TH2D*>{} // note : also map<string,TH2D*>
            )
        ); // note : the size is number of files
        data_h1D_map_map.insert(
            std::make_pair(
                pair.first,
                std::map<std::string, TH1D*>{} // note : also map<string,TH1D*>
            )
        ); // note : the size is number of files

        for (TObject* keyAsObj : *file_data_vec.back()->GetListOfKeys())
        {
            auto key = dynamic_cast<TKey*>(keyAsObj);
            std::string hist_name  = key->GetName();
            std::string class_name = key->GetClassName();

            if (class_name == "TH2D")
            {
                data_h2D_map_map[pair.first].insert(
                    std::make_pair(
                        hist_name.c_str(),
                        (TH2D*) file_data_vec.back() -> Get( hist_name.c_str() )
                    )
                );
            }
            else if (class_name == "TH1D")
            {
                data_h1D_map_map[pair.first].insert(
                    std::make_pair(
                        hist_name.c_str(),
                        (TH1D*) file_data_vec.back() -> Get( hist_name.c_str() )
                    )
                );

                data_h1D_map_map[pair.first][hist_name] -> SetLineColor(TColor::GetColor(color_code[data_h1D_map_map.size()-1].c_str()));
                data_h1D_map_map[pair.first][hist_name] -> SetMarkerColor(TColor::GetColor(color_code[data_h1D_map_map.size()-1].c_str()));
                data_h1D_map_map[pair.first][hist_name] -> SetMarkerStyle(20);
                data_h1D_map_map[pair.first][hist_name] -> SetMarkerSize(0.8);
                data_h1D_map_map[pair.first][hist_name] -> Sumw2(true);
                data_h1D_map_map[pair.first][hist_name] -> Scale(1.0/data_h1D_map_map[pair.first][hist_name]->Integral(-1,-1));

                std::string Yaxis_title = data_h1D_map_map[pair.first][hist_name] -> GetYaxis() -> GetTitle();
                Yaxis_title += "(A.U.)";
                data_h1D_map_map[pair.first][hist_name] -> GetYaxis() -> SetTitle(Yaxis_title.c_str());
            }
        }
    }

    // note : for MC ----------------------------------------------------------------------------------------------------
    for (auto &pair : MC_input_directory_pair_vec)
    {
        file_MC_vec.push_back(
            TFile::Open(Form("%s",pair.first.c_str()))
        );

        if (file_MC_vec.back() == nullptr)
        {
            std::cout << "Error: file_in can not be opened" << std::endl;
            exit(1);
        }

        // note : initialize the vector
        MC_h2D_map_map.insert(
            std::make_pair(
                pair.first,
                std::map<std::string, TH2D*>{}
            )
        );
        MC_h1D_map_map.insert(
            std::make_pair(
                pair.first,
                std::map<std::string, TH1D*>{}
            )
        );

        for (TObject* keyAsObj : *file_MC_vec.back()->GetListOfKeys())
        {
            auto key = dynamic_cast<TKey*>(keyAsObj);
            std::string hist_name  = key->GetName();
            std::string class_name = key->GetClassName();

            if (class_name == "TH2D")
            {
                MC_h2D_map_map[pair.first].insert(
                    std::make_pair(
                        hist_name.c_str(),
                        (TH2D*) file_MC_vec.back() -> Get( hist_name.c_str() )
                    )
                );
            }
            else if (class_name == "TH1D")
            {
                MC_h1D_map_map[pair.first].insert(
                    std::make_pair(
                        hist_name.c_str(),
                        (TH1D*) file_MC_vec.back() -> Get( hist_name.c_str() )
                    )
                );

                MC_h1D_map_map[pair.first][hist_name] -> SetLineColor(TColor::GetColor( color_code[MC_h1D_map_map.size() - 1 + data_h1D_map_map.size()].c_str() ));
                MC_h1D_map_map[pair.first][hist_name] -> SetMarkerColor(TColor::GetColor( color_code[MC_h1D_map_map.size() - 1 + data_h1D_map_map.size()].c_str() ));
                MC_h1D_map_map[pair.first][hist_name] -> SetMarkerStyle(20);
                MC_h1D_map_map[pair.first][hist_name] -> Sumw2(true);
                MC_h1D_map_map[pair.first][hist_name] -> Scale(1.0/MC_h1D_map_map[pair.first][hist_name]->Integral(-1,-1));

                std::string Yaxis_title = MC_h1D_map_map[pair.first][hist_name] -> GetYaxis() -> GetTitle();
                Yaxis_title += "(A.U.)";
                MC_h1D_map_map[pair.first][hist_name] -> GetYaxis() -> SetTitle(Yaxis_title.c_str());
            }
        }
    }

}

void vtxZDist_comp::MakePlots(std::string draw_method, bool isData)
{
    std::string sub_label_text = (isData) ? "Data" : "MC";
    
    std::map<std::string, std::pair<std::string, std::string>> input_directory_map;
    std::vector<std::string> h1_only_logy_plot_vec;
    std::map<std::string, std::map<std::string, TH1D*>> *h1D_map_map ;
    std::map<std::string, std::map<std::string, TH2D*>> *h2D_map_map ;

    if (isData)
    {
        input_directory_map = data_input_directory_map;
        h1_only_logy_plot_vec = h1data_only_logy_plot_vec;
        h1D_map_map = &data_h1D_map_map;
        h2D_map_map = &data_h2D_map_map;
    }
    else
    {
        input_directory_map = MC_input_directory_map;
        h1_only_logy_plot_vec = h1MC_only_logy_plot_vec;
        h1D_map_map = &MC_h1D_map_map;
        h2D_map_map = &MC_h2D_map_map;
    }

    for (auto &pair : *(h1D_map_map)) // note : pair.first : directory name
    {
        for (auto &pair2 : pair.second) // note : pair2.first : hist name
        {   
            c1 -> cd();

            bool set_log_y = false;
            if (std::find(h1comp_logy_plot_vec.begin(), h1comp_logy_plot_vec.end(), pair2.first) != h1comp_logy_plot_vec.end())
            {
                set_log_y = true;
            }
            if (std::find(h1_only_logy_plot_vec.begin(), h1_only_logy_plot_vec.end(), pair2.first) != h1_only_logy_plot_vec.end())
            {
                set_log_y = true;
            }

            c1 -> SetLogy(set_log_y);

            std::string log_text;

            if (set_log_y){
                pair2.second -> SetMaximum( pair2.second->GetBinContent(pair2.second->GetMaximumBin()) * logy_max_factor );
                log_text = "_logy";
            }
            else {
                pair2.second -> SetMaximum( pair2.second->GetBinContent(pair2.second->GetMaximumBin()) * lineary_max_factor );
                log_text = "";
            }

            
            pair2.second -> GetXaxis() -> SetNdivisions(505);
            pair2.second -> Draw("hist");

            std::string final_labelling = (isData) ? sPH_labeling : MC_labeling;

            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", final_labelling.c_str()));        

            std::string leg_label = (draw_method == "hist") ? "f" : "ep";

            All_leg -> Clear();
            All_leg -> AddEntry(pair2.second, input_directory_map[pair.first].first.c_str(), leg_label.c_str());
            All_leg -> Draw("same");

            // note : draw labelling
            if (labelling_vec_map.find(sub_label_text) != labelling_vec_map.end())
            {
                for (auto &tuple : labelling_vec_map[sub_label_text])
                {
                    double x1 = std::get<0>(tuple);
                    double y1 = std::get<1>(tuple);
                    std::string text = std::get<2>(tuple);

                    sub_ltx->DrawLatex(x1, y1, text.c_str());
                }

            }

            // todo: no sensor plot
            if (pair2.first.find("_l") != std::string::npos && pair2.first.find("_phi") != std::string::npos && pair2.first.find("_z") != std::string::npos)
            {
                std::cout<<"what? : "<<pair2.first<<std::endl;
                // c1 -> Print(Form("%s/data_%s_Draw%s.pdf",(output_directory + "/" + sensor_folder).c_str(),pair.first.c_str(),draw_method.c_str()));
            }
            else
            {
                c1 -> Print(
                    Form(
                        "%s/%s_%s_Draw%s%s.pdf",
                        (output_directory + "/" + sub_label_text + "_" + input_directory_map[pair.first].second).c_str(),
                        sub_label_text.c_str(),
                        pair2.first.c_str(),
                        draw_method.c_str(),
                        log_text.c_str()
                    )
                );
            }

            c1 -> Clear();

        } // note : end of all the histograms {h1D}
    } // note : end of all the data files {h1D}

    for (auto &pair : *(h2D_map_map)) // note : pair.first : directory name
    {
        for (auto &pair2 : pair.second) // note : pair2.first : hist name
        {   
            c1 -> cd();
            c1 -> SetLogy(false);

            std::string log_text = "";
            if (std::find(h2_logz_plot_vec.begin(), h2_logz_plot_vec.end(), pair2.first) != h2_logz_plot_vec.end())
            {
                c1 -> SetLogz();
                log_text = "_logz";
            }

            pair2.second -> Draw("colz0");

            std::string final_labelling = (isData) ? sPH_labeling : MC_labeling;

            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", final_labelling.c_str()));        

            // note : draw labelling
            if (labelling_vec_map.find(sub_label_text) != labelling_vec_map.end())
            {
                for (auto &tuple : labelling_vec_map[sub_label_text])
                {
                    double x1 = std::get<0>(tuple);
                    double y1 = std::get<1>(tuple);
                    std::string text = std::get<2>(tuple);

                    sub_ltx->DrawLatex(x1, y1, text.c_str());
                }

            }

            c1 -> Print(
                Form(
                    "%s/%s_%s%s.pdf",
                    (output_directory + "/" + sub_label_text + "_" + input_directory_map[pair.first].second).c_str(),
                    sub_label_text.c_str(),
                    pair2.first.c_str(), 
                    log_text.c_str()
                )
            );
            c1 -> Clear();
            c1 -> SetLogz(false);

        } // note : end of all the histograms {h2D}
    } // note : end of all the data files {h2D}
}

void vtxZDist_comp::MakeDataPlot(std::string draw_method)
{
    MakePlots(draw_method, true);
}

void vtxZDist_comp::MakeMCPlot(std::string draw_method)
{
    MakePlots(draw_method, false);
}

void vtxZDist_comp::MakeVtxZCheckPlot()
{
    // note : use the Inclusive70 as the reference
    for (auto &pair : data_h1D_map_map) // note : files
    {
        std::vector<std::pair<std::string, TH1D*>> all_Mbin_vtxZ_map; all_Mbin_vtxZ_map.clear();

        for (int i = 0; i < nCentrality_bin; i++)
        {
            make_comparison(
                {
                    {Form("Centrality [%s]%", Constants::centrality_text[i].c_str()), (TH1D*)(pair.second[Form("h1D_INTTz_Mbin%d", i)])->Clone(Form("h1D_INTTz_Mbin%d", i))}
                },
                {
                    {"Inclusive70", (TH1D*)(pair.second["h1D_INTTz_Inclusive70"])->Clone("h1D_INTTz_Inclusive70")}
                },
                output_directory + "/" + "Data_"+data_input_directory_map[pair.first].second,
                Form("Comp_Inclusive70_INTTz_Mbin%d", i),
                "Data",
                false,
                false, // note : isData_more
                0,
                -99
            );

            all_Mbin_vtxZ_map.push_back(
                std::make_pair(
                    Form("Centrality [%s]%", Constants::centrality_text[i].c_str()),
                    (TH1D*) pair.second[Form("h1D_INTTz_Mbin%d", i)] -> Clone(Form("h1D_INTTz_Mbin%d", i))
                )
            );
        }

        make_comparison(
            all_Mbin_vtxZ_map,
            {
                {"Inclusive70", (TH1D*)(pair.second["h1D_INTTz_Inclusive70"])->Clone("h1D_INTTz_Inclusive70")}
            },
            output_directory + "/" + "Data_"+data_input_directory_map[pair.first].second,
            Form("Comp_Inclusive70_INTTz_AllMbin"),
            "Data",
            false,
            true, // note : isData_more
            0,
            -99
        );
    }


    for (auto &pair : MC_h1D_map_map) // note : files
    {
        std::vector<std::pair<std::string, TH1D*>> all_Mbin_vtxZ_map; all_Mbin_vtxZ_map.clear();

        for (int i = 0; i < nCentrality_bin; i++)
        {
            make_comparison(
                {
                    {Form("Centrality [%s]%", Constants::centrality_text[i].c_str()), (TH1D*)(pair.second[Form("h1D_INTTz_Mbin%d", i)])->Clone(Form("h1D_INTTz_Mbin%d", i))}
                },
                {
                    {"Inclusive70", (TH1D*)(pair.second["h1D_INTTz_Inclusive70"])->Clone("h1D_INTTz_Inclusive70")}
                },
                output_directory + "/" + "MC_"+MC_input_directory_map[pair.first].second,
                Form("Comp_Inclusive70_INTTz_Mbin%d", i),
                "MC",
                false,
                false, // note : isData_more
                0,
                -99
            );

            all_Mbin_vtxZ_map.push_back(
                std::make_pair(
                    Form("Centrality [%s]%", Constants::centrality_text[i].c_str()),
                    (TH1D*) pair.second[Form("h1D_INTTz_Mbin%d", i)] -> Clone(Form("h1D_INTTz_Mbin%d", i))
                )
            );
        }

        make_comparison(
            all_Mbin_vtxZ_map,
            {
                {"Inclusive70", (TH1D*)(pair.second["h1D_INTTz_Inclusive70"])->Clone("h1D_INTTz_Inclusive70")}
            },
            output_directory + "/" + "MC_"+MC_input_directory_map[pair.first].second,
            Form("Comp_Inclusive70_INTTz_AllMbin"),
            "MC",
            false,
            true, // note : isData_more
            0,
            -99
        );
    }
}

void vtxZDist_comp::MakeComparisonPlot()
{
    std::vector<std::pair<std::string, TH1D*>> data_hist_vec; data_hist_vec.clear();
    std::vector<std::pair<std::string, TH1D*>> MC_hist_vec; MC_hist_vec.clear();

    std::vector<std::string> plot_name; plot_name.clear();
    for (auto &pair : data_h1D_map_map){ // note : std::map<std::string, std::map<std::string, TH1D*>>
        for (auto &pair2 : pair.second){
            plot_name.push_back(pair2.first);
        }
        break;
    }

    for (int i = 0; i < plot_name.size(); i++)
    {
        for (auto &pair : data_h1D_map_map){ // note : std::map<std::string, std::map<std::string, TH1D*>>
            data_hist_vec.push_back(
                {
                    data_input_directory_map[pair.first].first, // note : the description of that root file
                    pair.second[plot_name[i]]
                }
            );

            // std::cout<<"Index: "<<i<<", for plot: "<<plot_name[i]<<", data: "<<pair.first<<", hist: "<<pair.second[plot_name[i]]->GetName()<<", NbinsX"<<pair.second[plot_name[i]]->GetNbinsX()<<std::endl;
        }

        for (auto &pair : MC_h1D_map_map){ // note : std::map<std::string, std::map<std::string, TH1D*>>
            MC_hist_vec.push_back(
                {
                    MC_input_directory_map[pair.first].first, // note : the description of that root file
                    pair.second[plot_name[i]]
                }
            );

            // std::cout<<"Index: "<<i<<", for plot: "<<plot_name[i]<<", MC: "<<pair.first<<", hist: "<<pair.second[plot_name[i]]->GetName()<<", NbinsX"<<pair.second[plot_name[i]]->GetNbinsX()<<std::endl;
        }

        bool m_set_log_y = false;

        if (std::find(h1comp_logy_plot_vec.begin(), h1comp_logy_plot_vec.end(), plot_name[i]) != h1comp_logy_plot_vec.end())
        {
            m_set_log_y = true;
        }

        make_comparison(
            data_hist_vec,
            MC_hist_vec,
            output_directory,
            Form("Comp_%s", plot_name[i].c_str()),
            "Comp",
            m_set_log_y,
            false, // note : isData_more
            0,
            -99
        );

        data_hist_vec.clear();
        MC_hist_vec.clear();

    }


}

void vtxZDist_comp::PrepareINTTvtxZReWeight()
{
    TFile * file_out = new TFile(Form("%s/INTTvtxZReWeight.root", output_directory.c_str()), "RECREATE");
    
    // todo : only the first data file is used as the reference 
    // todo : only based on the Inclusive70, and Inclusive100

    TH1D * data_reference_Inclusive70 = (TH1D*) data_h1D_map_map[data_input_directory_pair_vec[0].first]["h1D_INTTz_Inclusive70"] -> Clone("h1D_INTTz_Inclusive70");
    data_reference_Inclusive70 -> Sumw2(true);

    TH1D * data_reference_Inclusive100 = (TH1D*) data_h1D_map_map[data_input_directory_pair_vec[0].first]["h1D_INTTz_Inclusive100"] -> Clone("h1D_INTTz_Inclusive100");
    data_reference_Inclusive100 -> Sumw2(true);

    TH1D * data_reference_Inclusive70_tight = (TH1D*) data_h1D_map_map[data_input_directory_pair_vec[0].first]["h1D_INTTz_Inclusive70_tight"] -> Clone("h1D_INTTz_Inclusive70_tight");
    data_reference_Inclusive70_tight -> Sumw2(true);

    TH1D * data_reference_Inclusive100_tight = (TH1D*) data_h1D_map_map[data_input_directory_pair_vec[0].first]["h1D_INTTz_Inclusive100_tight"] -> Clone("h1D_INTTz_Inclusive100_tight");
    data_reference_Inclusive100_tight -> Sumw2(true);

    std::map<std::string, TH1D*> reweighting_ratio; reweighting_ratio.clear();

    int count = 0; // note : should correspond to file index

    for (auto &pair : MC_h1D_map_map){
        reweighting_ratio.insert(
            std::make_pair(
                Form("%d_Inclusive70", count),
                (TH1D*) pair.second["h1D_INTTz_Inclusive70"] -> Clone((MC_input_directory_map[pair.first].second + "_Inclusive70").c_str())
            )
        );

        reweighting_ratio[Form("%d_Inclusive70", count)] -> Sumw2(true);
        reweighting_ratio[Form("%d_Inclusive70", count)] -> Divide(data_reference_Inclusive70, pair.second["h1D_INTTz_Inclusive70"], 1, 1);
        reweighting_ratio[Form("%d_Inclusive70", count)] -> GetYaxis() -> SetTitle("Data/MC");
        reweighting_ratio[Form("%d_Inclusive70", count)] -> Write((MC_input_directory_map[pair.first].second + "_Inclusive70").c_str());


        reweighting_ratio.insert(
            std::make_pair(
                Form("%d_Inclusive100", count),
                (TH1D*) pair.second["h1D_INTTz_Inclusive100"] -> Clone((MC_input_directory_map[pair.first].second + "_Inclusive100").c_str())
            )
        );

        reweighting_ratio[Form("%d_Inclusive100", count)] -> Sumw2(true);
        reweighting_ratio[Form("%d_Inclusive100", count)] -> Divide(data_reference_Inclusive100, pair.second["h1D_INTTz_Inclusive100"], 1, 1);
        reweighting_ratio[Form("%d_Inclusive100", count)] -> GetYaxis() -> SetTitle("Data/MC");
        reweighting_ratio[Form("%d_Inclusive100", count)] -> Write((MC_input_directory_map[pair.first].second + "_Inclusive100").c_str());


        // Division ---for tight-----------------------------------------------------------------------------------------------------------------------------
        reweighting_ratio.insert(
            std::make_pair(
                Form("%d_Inclusive70_tight", count),
                (TH1D*) pair.second["h1D_INTTz_Inclusive70_tight"] -> Clone((MC_input_directory_map[pair.first].second + "_Inclusive70_tight").c_str())
            )
        );

        reweighting_ratio[Form("%d_Inclusive70_tight", count)] -> Sumw2(true);
        reweighting_ratio[Form("%d_Inclusive70_tight", count)] -> Divide(data_reference_Inclusive70_tight, pair.second["h1D_INTTz_Inclusive70_tight"], 1, 1);
        reweighting_ratio[Form("%d_Inclusive70_tight", count)] -> GetYaxis() -> SetTitle("Data/MC");
        reweighting_ratio[Form("%d_Inclusive70_tight", count)] -> Write((MC_input_directory_map[pair.first].second + "_Inclusive70_tight").c_str());


        reweighting_ratio.insert(
            std::make_pair(
                Form("%d_Inclusive100_tight", count),
                (TH1D*) pair.second["h1D_INTTz_Inclusive100_tight"] -> Clone((MC_input_directory_map[pair.first].second + "_Inclusive100_tight").c_str())
            )
        );

        reweighting_ratio[Form("%d_Inclusive100_tight", count)] -> Sumw2(true);
        reweighting_ratio[Form("%d_Inclusive100_tight", count)] -> Divide(data_reference_Inclusive100_tight, pair.second["h1D_INTTz_Inclusive100_tight"], 1, 1);
        reweighting_ratio[Form("%d_Inclusive100_tight", count)] -> GetYaxis() -> SetTitle("Data/MC");
        reweighting_ratio[Form("%d_Inclusive100_tight", count)] -> Write((MC_input_directory_map[pair.first].second + "_Inclusive100_tight").c_str());

        count++;
    }

    file_out -> Close();

}

double vtxZDist_comp::GetNonZeroMinimalBin(TH1D * hist_in)
{
    double min = 10000000;
    for (int i = 1; i <= hist_in->GetNbinsX(); i++)
    {
        if (hist_in->GetBinContent(i) > 0 && hist_in->GetBinContent(i) < min)
        {
            min = hist_in->GetBinContent(i);
        }
    }

    return min;
}

std::pair<double,double> vtxZDist_comp::make_comparison(
    std::vector<std::pair<std::string, TH1D*>> data_hist_vec_in,  // note : description, hist
    std::vector<std::pair<std::string, TH1D*>> MC_hist_vec_in,    // note : description, hist
    std::string output_directory_in, 
    std::string output_filename_in, 
    std::string sub_label_text_in, // note : "Data", "MC", "Comp"
    bool set_log_y, 
    bool isData_more,
    double MainPlot_y_low, 
    double MainPlot_y_top, 
    double ratio_y_low,
    double ratio_y_top
)
{

    std::cout<<"In make_comparison, working on "<<output_filename_in<<std::endl;

    if (MainPlot_y_top == -99)
    {
        for (auto &pair : data_hist_vec_in)
        {
            MainPlot_y_top = (pair.second->GetBinContent(pair.second->GetMaximumBin()) > MainPlot_y_top) ? pair.second->GetBinContent(pair.second->GetMaximumBin()) : MainPlot_y_top;
        }

        for (auto &pair : MC_hist_vec_in)
        {
            MainPlot_y_top = (pair.second->GetBinContent(pair.second->GetMaximumBin()) > MainPlot_y_top) ? pair.second->GetBinContent(pair.second->GetMaximumBin()) : MainPlot_y_top;
        }


        if (set_log_y){            
            MainPlot_y_top = MainPlot_y_top * logy_max_factor;
        }
        else{
            MainPlot_y_top = MainPlot_y_top * lineary_max_factor;
        }
    }

    if (MainPlot_y_low == 0 && set_log_y)
    {
        MainPlot_y_low = 1000000;

        for (auto &pair : data_hist_vec_in)
        {
            double min = GetNonZeroMinimalBin(pair.second);

            MainPlot_y_low = (min < MainPlot_y_low) ? min : MainPlot_y_low;
        }

        for (auto &pair : MC_hist_vec_in)
        {
            double min = GetNonZeroMinimalBin(pair.second);

            MainPlot_y_low = (min < MainPlot_y_low) ? min : MainPlot_y_low;
        }
    }

    std::vector<int> data_hist_nbinsX; data_hist_nbinsX.clear();
    std::vector<int> MC_hist_nbinsX; MC_hist_nbinsX.clear();

    int color_index = 0;

    for (auto &pair : data_hist_vec_in)
    {

        std::cout<<"Data: "<<pair.first<<", "<<pair.second->GetName()<<", "<<pair.second->GetNbinsX()<<std::endl;

        data_hist_nbinsX.push_back(pair.second->GetNbinsX());
        pair.second->SetLineColor(TColor::GetColor(color_code[color_index].c_str()));
        pair.second->SetMarkerColor(TColor::GetColor(color_code[color_index].c_str()));
        color_index += 1;
    }

    for (auto &pair : MC_hist_vec_in)
    {
        std::cout<<"MC: "<<pair.first<<", "<<pair.second->GetName()<<", "<<pair.second->GetNbinsX()<<std::endl;

        MC_hist_nbinsX.push_back(pair.second->GetNbinsX());
        pair.second->SetLineColor(TColor::GetColor(color_code[color_index].c_str()));
        pair.second->SetMarkerColor(TColor::GetColor(color_code[color_index].c_str()));
        color_index += 1;
    }

    for (int &data_nbin : data_hist_nbinsX){
        for (int &MC_nbin : MC_hist_nbinsX){
            if (data_nbin != MC_nbin){
                std::cout<<"Error: Different binning for data and MC, data: "<<data_nbin<<", MC: "<<MC_nbin<<std::endl;
                exit(1);
            }
        }
    }

    TH1D * h1D_pad1 = (TH1D*)data_hist_vec_in.front().second->Clone("h1D_pad1");
    TH1D * h1D_pad2 = (TH1D*)data_hist_vec_in.front().second->Clone("h1D_pad2");
    h1D_pad1 -> Reset("ICESM");
    h1D_pad2 -> Reset("ICESM");
    h1D_pad1 -> SetLineColorAlpha(1,0.);
    h1D_pad2 -> SetLineColorAlpha(1,0.);
    h1D_pad1 -> SetFillColorAlpha(1,0.);
    h1D_pad2 -> SetFillColorAlpha(1,0.);
    h1D_pad1 -> SetMarkerColorAlpha(1,0.);
    h1D_pad2 -> SetMarkerColorAlpha(1,0.);


    SetsPhenixStyle();

    TCanvas * c1 = new TCanvas("", "c1", 950, 800);

    TPad * pad2 = new TPad("", "pad2", 0.0, 0.0, 1.0, 0.29);
    pad2 -> SetBottomMargin(0.3);
    pad2->Draw();
    
    TPad * pad1 = new TPad("", "pad1", 0.0, 0.29, 1.0, 1.0);
    pad1 -> SetTopMargin(0.065);
    pad1 -> SetBottomMargin(0.02);
    pad1->Draw();

    int total_hist = data_hist_vec_in.size() + MC_hist_vec_in.size();

    // todo: set the size fo the legend
    double leg_x1 = (total_hist <= 4) ? 0.45 : 0.65;
    double leg_y1 = (total_hist <= 4) ? 0.76 : 0.4;
    double leg_x2 = (total_hist <= 4) ? 0.8 : 0.8;
    double leg_y2 = (total_hist <= 4) ? 0.88 : 0.88;

    TLegend * leg = new TLegend(leg_x1, leg_y1, leg_x2, leg_y2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.025);

    h1D_pad1->GetXaxis()->SetLabelOffset(999);
    h1D_pad1->GetXaxis()->SetLabelSize(0);
    h1D_pad1->GetXaxis()->SetTitleOffset(999);
    h1D_pad1->GetYaxis()->SetLabelSize(0.045);
    h1D_pad1->GetYaxis()->SetTitleOffset(0.8);
    h1D_pad1->GetYaxis()->SetTitleSize(0.07);    
    h1D_pad1->GetXaxis()->SetNdivisions();

    for (auto &pair : MC_hist_vec_in){
        leg -> AddEntry(pair.second, pair.first.c_str(), "f");
    }
    
    for (auto &pair : data_hist_vec_in){
        // pair.second->GetXaxis()->SetNdivisions();
        leg -> AddEntry(pair.second, pair.first.c_str(), "lep");
    }
    
    pad1->cd();

    std::cout<<"For "<<output_filename_in<<", MainPlot_y_low: "<<MainPlot_y_low<<", MainPlot_y_top: "<<MainPlot_y_top<<std::endl;

    h1D_pad1 -> SetMinimum(MainPlot_y_low);
    h1D_pad1 -> SetMaximum(MainPlot_y_top);
    h1D_pad1 -> Draw("hist");
    
    for (int i = 0; i < MC_hist_vec_in.size(); i++){
        MC_hist_vec_in[i].second -> Draw("hist same");
    }

    for (int i = 0; i < data_hist_vec_in.size(); i++){
        data_hist_vec_in[i].second -> Draw("ep same");
    }

    leg -> Draw("same");

    pad1->SetLogy(set_log_y);

    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_labeling.c_str()));

    if (labelling_vec_map.find(sub_label_text_in) != labelling_vec_map.end())
    {
        for (auto &tuple : labelling_vec_map[sub_label_text_in])
        {
            double x1 = std::get<0>(tuple);
            double y1 = std::get<1>(tuple);
            std::string text = std::get<2>(tuple);

            sub_ltx->DrawLatex(x1, y1, text.c_str());
        }
    }

    TLine * line = new TLine();
    line -> SetLineStyle(2);
    line -> SetLineWidth(2);
    line -> SetLineColor(28);

    pad2->cd();

    std::vector<TH1D*> ratio_hist_vec_in; ratio_hist_vec_in.clear();

    std::vector<std::pair<std::string, TH1D*>> hist_vec_in = (isData_more) ? data_hist_vec_in : MC_hist_vec_in;

    for (int i = 0; i < hist_vec_in.size(); i++){
        
        std::pair<std::string, TH1D*> pair = hist_vec_in[i];

        // todo: can only do the ratio w.r.t the first data hist
        if (isData_more){
            ratio_hist_vec_in.push_back( // note : hist_vec_in: data
                (TH1D*) MC_hist_vec_in[0].second->Clone(Form("ratio_hist_%s", pair.first.c_str()))
            );

            ratio_hist_vec_in.back() -> Sumw2(true);
            ratio_hist_vec_in.back() -> Divide(pair.second, ratio_hist_vec_in.back());

        }
        else { // note : hist_vec_in: MC
            ratio_hist_vec_in.push_back(
                (TH1D*) data_hist_vec_in[0].second->Clone(Form("ratio_hist_%s", pair.first.c_str()))
            );

            ratio_hist_vec_in.back() -> Sumw2(true);
            ratio_hist_vec_in.back() -> Divide(pair.second);
        }


        // if (ratio_hist_vec_in.back()->GetXaxis()->GetLabelOffset() > 10)
        // {
        //     ratio_hist_vec_in.back()->GetXaxis()->SetLabelOffset(0.01);
        // }

        ratio_hist_vec_in.back()->SetMarkerStyle(20);
        ratio_hist_vec_in.back()->SetMarkerSize(0.8);
        ratio_hist_vec_in.back()->SetMarkerColor(pair.second->GetLineColor());
        ratio_hist_vec_in.back()->SetLineColor(pair.second->GetLineColor());

        if (i == 0){
            h1D_pad2->GetXaxis()->SetLabelSize(0.11);
            h1D_pad2->GetXaxis()->SetTitleOffset(0.8);
            h1D_pad2->GetXaxis()->SetTitleSize(0.14);
            h1D_pad2->GetXaxis()->SetNdivisions();

            h1D_pad2->GetYaxis()->SetLabelSize(0.11);
            h1D_pad2->GetYaxis()->SetTitleOffset(0.4);
            h1D_pad2->GetYaxis()->SetTitleSize(0.14);
            h1D_pad2->GetYaxis()->SetNdivisions(505);

            h1D_pad2 -> SetMinimum(ratio_y_low);
            h1D_pad2 -> SetMaximum(ratio_y_top);

            h1D_pad2->GetYaxis()->SetTitle("Data/MC");


            h1D_pad2->Draw("ep");
            line -> DrawLine(h1D_pad2->GetXaxis()->GetXmin(), 1, h1D_pad2->GetXaxis()->GetXmax(), 1);

            ratio_hist_vec_in.back()->Draw("ep same");
        }
        else {
            ratio_hist_vec_in.back()->Draw("ep same");
        }
    }


    
    // std::string data_hist_name = data_hist->GetName(); 


    // if (data_hist_name.find("_l") != std::string::npos && data_hist_name.find("_phi") != std::string::npos && data_hist_name.find("_z") != std::string::npos)
    // {
    //     std::cout<<"what? : "<<data_hist_name<<std::endl;

    //     // c1 -> Print(Form("%s/Comp_%s.pdf", (output_directory + "/" + sensor_folder).c_str(), output_filename.c_str()));
    // }
    if (true)
    {
        c1 -> Print(Form("%s/Comp_%s.pdf", output_directory_in.c_str(), output_filename_in.c_str()));
    }

    return {};
    
    // if (data_hist_name.find("_l") == std::string::npos || data_hist_name.find("_phi") == std::string::npos || data_hist_name.find("_z") == std::string::npos) {return {};}

    // return GetHistYInfo(ratio_hist);

    
    // pad1 -> Clear();
    // pad2 -> Clear();
    // c1 -> Clear();

    // delete pad1;
    // delete pad2;
    // delete c1;
    // delete ratio_hist;

    // return;
}