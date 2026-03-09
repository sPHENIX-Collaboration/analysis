#include "sPhenixStyle.C"

int TCanvasInTFile_MakePlot()
{   
    string input_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Dec042024/completed/Run24NewCode_EvtVtxZTracklet/completed";
    string intput_filename = "Plot_Data_EvtVtxZProtoTracklet_FieldOff_BcoFullDiff_VtxZReco_Tracklet_TrackletRotate_FirstRun20241218_00054280_00000.root";
    string output_direcotry = input_directory + "/INTTvtxZEventDisplay";
    bool isInternal = true;
    string plot_text = (isInternal) ? "Internal" : "Preliminary";
    string Bid_pad_name = "pad_EvtZDist";
    bool DrawLegend = true;

    system(Form("if [ ! -d %s ]; then mkdir -p %s; fi",output_direcotry.c_str(),output_direcotry.c_str()));

    TFile * file_in = TFile::Open(Form("%s/%s",input_directory.c_str(),intput_filename.c_str()));

    SetsPhenixStyle();

    std::map<std::string, TCanvas*> data_canvas_map;
    std::map<std::string, TPad*> data_pad_map;
    std::map<std::string, TF1*> data_TF1_map;

    const std::vector<std::string> color_code = {
        "#9e0142",
        "#d53e4f",
        "#f46d43",
        "#fdae61",
        "#fee08b",
        "#e6f598",
        "#abdda4",
        "#66c2a5",
        "#3288bd",
        "#5e4fa2" 
    };

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLegend * leg = new TLegend(0.2,0.68,0.8,0.77);
    leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.02);
    leg -> SetMargin(0.2);

    for (TObject* keyAsObj : *file_in->GetListOfKeys())
    {
        auto key = dynamic_cast<TKey*>(keyAsObj);
        std::string canvas_name  = key->GetName();
        std::string class_name = key->GetClassName();

        std::cout<<std::endl;
        std::cout<<"canvas_name : "<<canvas_name<<", class_name : "<<class_name<<std::endl;

        if (class_name == "TCanvas")
        {
            data_pad_map.clear();
            data_TF1_map.clear();

            data_canvas_map[canvas_name.c_str()] = (TCanvas*) file_in -> Get( canvas_name.c_str() );

            TList *primitives = data_canvas_map[canvas_name.c_str()]->GetListOfPrimitives();

            // Iterate through the list to find TPads
            TIter next(primitives);
            TObject *obj;
            std::cout << "List of TPad objects in the canvas:" << std::endl;
            while ((obj = next())) {
                if (obj->InheritsFrom("TPad")) {
                    TPad *pad = dynamic_cast<TPad*>(obj);
                    if (pad) {
                        std::cout << "TPad Name: " << pad->GetName() << ", Title: " << pad->GetTitle() << std::endl;

                        data_pad_map[Form("%s_%s",canvas_name.c_str(),pad->GetName())] = dynamic_cast<TPad*>(data_canvas_map[canvas_name.c_str()]->GetPrimitive(pad->GetName()));
                    }
                }
            }

            for (auto &pair : data_pad_map){
                int line_count = 0;
                std::cout<<"pair.first : "<<pair.first<<std::endl;
                TList *primitives = data_pad_map[pair.first]->GetListOfPrimitives();

                TIter next(primitives);
                while ((obj = next())) {
                    if (obj->InheritsFrom("TF1")) {
                        TF1 *tf1 = dynamic_cast<TF1*>(obj);
                        if (tf1) {
                            std::cout << "TF1 Name: " << tf1->GetName() << ", Title: " << tf1->GetTitle() << std::endl;

                            data_TF1_map[Form("%s_%s",pair.first.c_str(),tf1->GetName())] = dynamic_cast<TF1*>(data_pad_map[pair.first]->GetPrimitive(tf1->GetName()));
                            data_TF1_map[Form("%s_%s",pair.first.c_str(),tf1->GetName())] -> SetLineColor( TColor::GetColor(color_code[line_count].c_str()) );
                            data_TF1_map[Form("%s_%s",pair.first.c_str(),tf1->GetName())] -> SetLineWidth(1);
                            // cout<<Form("%s_%s",pair.first.c_str(),tf1->GetName())<<" get range: "<<data_TF1_map[Form("%s_%s",pair.first.c_str(),tf1->GetName())]->GetXmin()<<" ~ "<<data_TF1_map[Form("%s_%s",pair.first.c_str(),tf1->GetName())]->GetXmax()<<endl;

                            double fit_xmin, fit_xmax;
                            data_TF1_map[Form("%s_%s",pair.first.c_str(),tf1->GetName())] -> GetRange(fit_xmin, fit_xmax);

                            leg -> AddEntry(data_TF1_map[Form("%s_%s",pair.first.c_str(),tf1->GetName())], Form("Gaus, range: [%.2f, %.2f] cm", fit_xmin, fit_xmax), "l");

                            line_count++;
                        }
                    }
                }
            }

            // data_canvas_map[canvas_name.c_str()]->Update();
            // data_canvas_map[canvas_name.c_str()]->Draw();
            data_canvas_map[canvas_name.c_str()] -> cd();
            data_pad_map[Form("%s_%s", canvas_name.c_str(), Bid_pad_name.c_str())]->cd();
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", plot_text.c_str()));
            if (DrawLegend) {leg->Draw("same");}
            data_canvas_map[canvas_name.c_str()]->Print(Form("%s/%s.pdf",output_direcotry.c_str(),canvas_name.c_str()));
            
            leg -> Clear();
            std::cout<<"------------------------------------------------------------------------------------"<<std::endl;

        }


    }

    


    return 0;
}