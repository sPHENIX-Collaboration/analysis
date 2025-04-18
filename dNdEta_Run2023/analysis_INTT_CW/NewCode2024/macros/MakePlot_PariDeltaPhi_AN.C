#include "sPhenixStyle.C"
#include "../Constants.cpp"

int MakePlot_PariDeltaPhi_AN()
{
    int Mbin = 0;

    std::string input_file_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin%d/Folder_BaseLine/Run_0/completed", Mbin); 
    std::string input_file_name = Form("Data_PreparedNdEtaEach_AlphaCorr_AllSensor_VtxZ10_Mbin%d_00054280_00000_DeltaPhi.root", Mbin);
    
    std::string target_canvas_name = "c1_hstack1D_DeltaPhi_Eta13";

    std::string output_directory = input_file_directory;

    std::pair<std::string, std::string> axes_label = {"Cluster Pair#kern[0.3]{#Delta#phi} [Radian]", "Counts (/0.001)"};
    
    std::string sPH_label = "Internal";
    std::vector<std::tuple<double,double,std::string>> additional_text = {
        // {0.2, 0.9, "The cluster pairs post the VtxZ linking requirement are filled"},

        // {0.22, 0.9, "Au+Au 200 GeV"},
        // {0.22, 0.86, "HIJING"},
        // {0.22, 0.86, "Centrality 0-70%"},
        // {0.22, 0.82, "|INTT vtxZ| #leq 10 cm"}

        // {0.22, 0.90, "Inner clusters rotated by #pi in #phi angle"},
        {0.22, 0.86 + 0.02, "Au+Au#kern[0.25]{#sqrt{s_{NN}}} = 200 GeV"},
        {0.22, 0.81 + 0.02, "-0.1#kern[1]{#leq}#kern[1]{#eta} < 0.1"},
        {0.22, 0.76 + 0.02, Form("Centrality: %s%%", Constants::centrality_text[Mbin].c_str())},
        {0.22, 0.71 + 0.02, "|INTT vtxZ| #leq 10 cm"}

    };

    std::vector<std::tuple<int, std::string,std::string>> legend_text = {
        {1, "Nominal", "ep"},
        {92, "Inner barrel rotated by#kern[0.6]{#pi}", "ep"},
        {96, "Subtracted", "f"},
        {41, "Pol-2 Bkg Fit", "l"}
    };

    double y_max = 2500000;
    
    SetsPhenixStyle();

    TCanvas * c2 = new TCanvas("c2", "c2", 950, 800);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.045);

    TLegend * leg = new TLegend(0.4,0.6,0.73,0.7);
    // leg -> SetNColumns(2);
    // leg -> SetBorderSize(1);
    leg -> SetTextSize(0.04);
    leg -> SetMargin(0.15);

    TFile * file_in = TFile::Open(Form("%s/%s", input_file_directory.c_str(), input_file_name.c_str()));
    TCanvas * c1 = (TCanvas*)file_in->Get(target_canvas_name.c_str());



    TList* primitives = c1->GetListOfPrimitives(); // Get the list of objects (primitives) on the canvas
    if (!primitives) {
        std::cerr << "No primitives found in TCanvas!" << std::endl;
        file_in->Close();
        return 666;
    }

    int legend_index = 0;
    // Loop through the primitives and modify the TH1Ds

    std::vector<TH1D*> hists; hists.clear();
    std::vector<TF1*> fits; fits.clear();

    for (auto* obj : *primitives) {

        TH1D* hist = dynamic_cast<TH1D*>(obj); // Check if the primitive is a TH1D
        TF1 * fit = dynamic_cast<TF1*>(obj);
        
        if (hist) {
            
            int color = std::get<0>(legend_text[legend_index]);
            std::string text = std::get<1>(legend_text[legend_index]);
            std::string draw_type = std::get<2>(legend_text[legend_index]);

            hist -> SetLineColor( color );
            std::cout<<hist->GetName()<<", set line color : "<<hist->GetLineColor()<<std::endl;
            // leg -> AddEntry(hist, text.c_str(), draw_type.c_str());

            hist -> SetMaximum(y_max);
            hist -> SetMinimum(0);

            hist -> SetMarkerSize(0.65);
            hist -> SetMarkerColor(color);

            // hist -> GetXaxis() -> SetNdivisions(505);
            hist -> GetYaxis() -> SetNdivisions(505);

            if (axes_label.first.size() > 0){
                hist -> GetXaxis() -> SetTitle(axes_label.first.c_str());
            }
            if (axes_label.second.size() > 0){
                hist -> GetYaxis() -> SetTitle(axes_label.second.c_str());
            }

            hists.push_back(hist);

            legend_index += 1;
        }

        if (fit){
            int color = std::get<0>(legend_text[legend_index]);
            std::string text = std::get<1>(legend_text[legend_index]);
            std::string draw_type = std::get<2>(legend_text[legend_index]);

            fit -> SetLineColor( color );
            fit -> SetLineWidth(1);
            fit -> SetLineStyle(2);
            std::cout<<fit->GetName()<<", set line color : "<<fit->GetLineColor()<<", lien style : "<<fit -> GetLineStyle()<<std::endl;
            // leg -> AddEntry(fit, text.c_str(), draw_type.c_str());

            fits.push_back(fit);

            legend_index += 1;
        }
    }

    // c1 -> Clear();
    // c1 -> Draw();

    c2 -> cd();

    hists[0] -> Draw("ep");

    // int color = std::get<0>(legend_text[0]);
    std::string text = std::get<1>(legend_text[0]);
    std::string draw_type = std::get<2>(legend_text[0]);
    leg -> AddEntry(hists[0], text.c_str(), draw_type.c_str());

    hists[1] -> Draw("ep same");
    // int color = std::get<0>(legend_text[1]);
    text = std::get<1>(legend_text[1]);
    draw_type = std::get<2>(legend_text[1]);
    leg -> AddEntry(hists[1], text.c_str(), draw_type.c_str());

    hists[2] -> Draw("hist same");
    // int color = std::get<0>(legend_text[1]);
    text = std::get<1>(legend_text[2]);
    draw_type = std::get<2>(legend_text[2]);
    leg -> AddEntry(hists[2], text.c_str(), draw_type.c_str());


    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label.c_str())); 

    for (auto text : additional_text){
        double x = std::get<0>(text);
        double y = std::get<1>(text);
        std::string text_str = std::get<2>(text);

        draw_text -> DrawLatex(x, y, text_str.c_str());
    }

    leg -> Draw("same");

    c2 -> Print(Form("%s/%s.pdf", output_directory.c_str(), target_canvas_name.c_str()));


    return 0;
}