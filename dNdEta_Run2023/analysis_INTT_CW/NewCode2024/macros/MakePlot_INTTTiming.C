#include "sPhenixStyle.C" 

// a function to shift the bin content of a histogram by nbin_shift
TH1D * hist_shift_inX(TH1D* hist_in, int nbin_shift)
{
    vector<double> bin_center; bin_center.clear();
    vector<double> bin_content; bin_content.clear();

    for (int i = 1; i <= hist_in->GetNbinsX(); i++)
    {
        double hist_X_min = hist_in->GetXaxis()->GetXmin();
        double hist_X_max = hist_in->GetXaxis()->GetXmax();
        double hist_X_range = hist_X_max - hist_X_min;

        double shifted_bin_center = (hist_in->GetBinCenter(i) - nbin_shift < hist_X_min) ? (hist_in->GetBinCenter(i) - nbin_shift) + hist_X_range : hist_in->GetBinCenter(i) - nbin_shift;

        bin_center.push_back(shifted_bin_center);
        bin_content.push_back(hist_in->GetBinContent(i));
    }

    TH1D * hist_out = new TH1D("","",138,-10.5,127.5);
    hist_out -> Reset("ICESM");

    for (int i = 0; i < bin_center.size(); i++)
    {
        hist_out -> SetBinContent(
            hist_out -> FindBin(bin_center.at(i) - 0.5),
            bin_content.at(i)
        );
    }

    return hist_out;
}

float LeftMargin = 0.14;
float RightMargin = 0.07;

std::string sPH_label_data = "Preliminary";
std::string Collision_str = "Au+Au#kern[0.25]{#sqrt{s_{NN}}} = 200 GeV";
std::string RHIC_str = "Bunch crossing interval: 2 BCOs";
std::string ReadOut_str = "INTT Trigger+Extended readout";
std::string sPHENIX_date = "March 20, 2025";

std::string file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/INTT_timing_withTrig30cm/completed";
std::string file_name = "BcoDiffNtuple_54280_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_merged.root";
std::string hist_name_noindex = "all_felix_BcoDiffHist_";

// void plot_two_panels() {
//     // Create a canvas
//     TCanvas *c = new TCanvas("c", "Two Panels Example", 800, 800);
//     c->Divide(1, 2, 0, 0); // Divide into 2 vertical pads with no margins
    
//     // Create two identical histograms
//     TH1D *h1 = new TH1D("h1", "Histogram 1;X-axis;Y-axis", 50, -4, 4);
//     TH1D *h2 = new TH1D("h2", "Histogram 2;X-axis;Y-axis", 50, -4, 4);
    
//     // Fill histograms with random data
//     TRandom randGen;
//     for (int i = 0; i < 1000; i++) {
//         double val = randGen.Gaus(0, 1);
//         h1->Fill(val);
//         h2->Fill(val);
//     }
    
//     // Get the two pads
//     TPad *pad1 = (TPad*)c->cd(1);
//     pad1->SetBottomMargin(0);
//     pad1->SetTopMargin(0.1);
//     h1->Draw();
    
//     TPad *pad2 = (TPad*)c->cd(2);
//     pad2->SetTopMargin(0);
//     pad2->SetBottomMargin(0.1);
//     h2->Draw();
    
//     // Update canvas
//     c->Update();
// }

std::vector<TH1D *> prepare_hist()
{
    int trigger_bin = 2;
    int trigger_bin_shift = 55;

    std::vector<TH1D *> hist_vec; hist_vec.clear();
    THStack * hs = new THStack("","");

    TFile *file = TFile::Open(Form("%s/%s", file_directory.c_str(), file_name.c_str()));

    for (int i = 0; i < 8; i++)
    {
        hist_vec.push_back((TH1D*)file->Get(Form("%s%d", hist_name_noindex.c_str(), i)));
        
        hs->Add(hist_vec.back());
    }
    
    TH1D * hist_first = (TH1D*)hs->GetStack()->Last();
    TH1D * hist_sum = hist_shift_inX(hist_first, trigger_bin_shift);
    hist_sum -> SetName("hist_sum");
    hist_sum -> SetTitle(";Beam crossing (106 ns / crossing);Normalized INTT hit counts");
    hist_sum -> SetLineColor(38);

    hist_sum -> Scale(1.0/hist_sum->Integral());

    TH1D * hist_trigger = new TH1D("","",1,-0.5,0.5);
    hist_trigger -> SetBinContent(1, hist_sum->GetBinContent(hist_sum -> GetMaximumBin()));
    hist_trigger -> SetLineColor(kRed);

    hist_sum -> SetBinContent(hist_sum -> GetMaximumBin(), 0);

    hist_sum -> GetXaxis() -> SetRangeUser(-1.5, 100);

    return {
        hist_sum,
        hist_trigger
    };
}

int MakePlot_INTTTiming_TwoPanel(int event_id = 0)
{   
    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1", "c1", 1000, 800);

    c1 -> cd();
    TPad * pad1 = new TPad("pad1", "pad1", 0, 0.5, 1, 1);
    pad1 -> Draw();
    pad1 -> cd();
    pad1 -> SetBottomMargin(0.02);
    pad1 -> SetTopMargin(0.07);
    pad1 -> SetLeftMargin(LeftMargin);
    pad1 -> SetRightMargin(RightMargin);
    // pad1 -> SetLogy();

    c1 -> cd();
    TPad * pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.5);
    pad2 -> Draw();
    pad2 -> cd();
    pad2 -> SetTopMargin(0.02);
    pad2 -> SetLeftMargin(LeftMargin);
    pad2 -> SetRightMargin(RightMargin);
    pad2 -> SetLogy();

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.08);

    double label_shit_x = 0.23;
    double label_shit_y = -0.03;

    TLegend * leg = new TLegend(0.15 + label_shit_x,0.41 + label_shit_y,0.85 ,0.57+label_shit_y);
    // leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.06);
    leg -> SetMargin(0.12);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.09);
    // ltx->SetTextAlign(31);
    
    std::vector<TH1D *> hist_vec = prepare_hist();
    TH1D * hist_sum = hist_vec.at(0);
    TH1D * hist_trigger = hist_vec.at(1);
    
    double Y_text_size = hist_sum -> GetYaxis() -> GetTitleSize();

    hist_sum -> GetYaxis() -> CenterTitle();
    hist_sum -> GetXaxis() -> CenterTitle();
    hist_sum -> GetYaxis() -> SetNdivisions(505);
    hist_sum -> GetXaxis() -> SetTitleSize(Y_text_size * 1.3);
    hist_sum -> GetYaxis() -> SetTitleSize(Y_text_size * 1.3);
    hist_sum -> GetYaxis() -> SetTitleOffset(0.8);

    leg -> AddEntry(hist_trigger, "Hits associated with trigger", "f");
    leg -> AddEntry(hist_sum, "Hits from other beam crossings", "f");

    TH2D * hist_sum_short = (TH2D*) hist_sum -> Clone("hist_sum_short");
    TH2D * hist_trigger_short = (TH2D*) hist_trigger -> Clone("hist_trigger_short");

    hist_sum_short -> GetYaxis() -> SetTitleOffset(0.8);
    hist_sum_short -> GetYaxis() -> SetTitleSize(Y_text_size * 1.3);
    hist_sum_short -> GetXaxis() -> SetTitleOffset(1.0);
    hist_sum_short -> GetXaxis() -> SetTitleSize(Y_text_size * 1.3);

    pad1 -> cd();
    hist_sum -> GetXaxis() -> SetLabelOffset(999);
    hist_sum -> SetMaximum(hist_trigger_short->GetBinContent(hist_trigger_short->GetMaximumBin())*1.1);
    hist_sum -> SetMinimum(0);
    hist_sum -> Draw("hist");

    ltx->DrawLatex(0.15 + label_shit_x, 0.85 + label_shit_y, Form("#it{#bf{sPHENIX}} %s #it{%s}", sPH_label_data.c_str(), sPHENIX_date.c_str())); 
    
    draw_text -> DrawLatex(0.15 + label_shit_x, 0.77 + label_shit_y, Collision_str.c_str());
    draw_text -> DrawLatex(0.15 + label_shit_x, 0.69 + label_shit_y, RHIC_str.c_str());
    draw_text -> DrawLatex(0.15 + label_shit_x, 0.61 + label_shit_y, ReadOut_str.c_str());

    leg -> Draw("same");
    hist_trigger->Draw("hist same");

    pad2 -> cd();
    

    // hist_sum_short -> SetMinimum(0);
    hist_sum_short -> SetMaximum(hist_sum_short->GetBinContent(hist_sum_short->GetMaximumBin())*500);
    hist_sum_short->Draw("hist");
    hist_trigger_short->Draw("hist same");

    c1 -> Print(Form("%s/INTT_timing_2panel.pdf",file_directory.c_str()));

    return 888;
}


int MakePlot_INTTTiming_chop(int event_id = 0)
{   
    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1", "c1", 900, 800);

    c1 -> cd();
    TPad * pad1 = new TPad("pad1", "pad1", 0, 0, 1, 1);
    pad1 -> Draw();
    pad1 -> cd();
    pad1 -> SetBottomMargin(0.15);
    pad1 -> SetTopMargin(0.07);
    pad1 -> SetLeftMargin(LeftMargin);
    pad1 -> SetRightMargin(RightMargin);
    pad1 -> SetLogy();


    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.04);

    double label_shit_x = 0.12;
    double label_shit_y = -0.02;

    TLegend * leg = new TLegend(0.15 + label_shit_x, 0.55 + label_shit_y,0.85 ,0.67+label_shit_y);
    // leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.04);
    leg -> SetMargin(0.12);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    // ltx->SetTextAlign(31);
    
    std::vector<TH1D *> hist_vec = prepare_hist();
    TH1D * hist_sum = hist_vec.at(0);
    TH1D * hist_trigger = hist_vec.at(1);
    
    double Y_text_size = hist_sum -> GetYaxis() -> GetTitleSize();

    hist_sum -> GetYaxis() -> CenterTitle();
    hist_sum -> GetXaxis() -> CenterTitle();
    hist_sum -> GetYaxis() -> SetNdivisions(505);
    hist_sum -> GetXaxis() -> SetTitleOffset(1.25);
    // hist_sum -> GetYaxis() -> SetTitleSize(Y_text_size * 1.3);
    // hist_sum -> GetYaxis() -> SetTitleOffset(0.8);

    leg -> AddEntry(hist_trigger, "Hits associated with trigger", "f");
    leg -> AddEntry(hist_sum, "Hits from other beam crossings", "f");

    pad1 -> cd();
    // hist_sum -> GetXaxis() -> SetLabelOffset(999);
    hist_sum -> SetMaximum(hist_trigger->GetBinContent(hist_trigger->GetMaximumBin())*100);
    // hist_sum -> SetMinimum(0);
    hist_sum -> Draw("hist");

    ltx->DrawLatex(0.15 + label_shit_x, 0.85 + label_shit_y, Form("#it{#bf{sPHENIX}} %s #it{%s}", sPH_label_data.c_str(), sPHENIX_date.c_str())); 
    
    draw_text -> DrawLatex(0.15 + label_shit_x, 0.80 + label_shit_y, Collision_str.c_str());
    draw_text -> DrawLatex(0.15 + label_shit_x, 0.75 + label_shit_y, RHIC_str.c_str());
    draw_text -> DrawLatex(0.15 + label_shit_x, 0.70 + label_shit_y, ReadOut_str.c_str());

    leg -> Draw("same");
    hist_trigger->Draw("hist same");


    c1 -> Print(Form("%s/INTT_timing_chop.pdf",file_directory.c_str()));

    return 888;
}


int MakePlot_INTTTiming_smallpad(int event_id = 0)
{    
    double label_shit_x = 0.09;
    double label_shit_y = -0.02;

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);
    // c1 -> SetLogy();

    TPad * pad1 = new TPad("pad1", "pad1", 0, 0, 1, 1);
    pad1 -> Draw();
    pad1 -> cd();
    pad1 -> SetRightMargin(RightMargin);
    // pad1 -> SetLogy();

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.04);

    TLegend * leg = new TLegend(0.21 + label_shit_x, 0.61 + label_shit_y, 0.8, 0.71 + label_shit_y);
    // leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.04);
    leg -> SetMargin(0.12);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);


    std::vector<TH1D *> hist_vec = prepare_hist();
    TH1D * hist_sum = hist_vec.at(0);
    TH1D * hist_trigger = hist_vec.at(1);

    // leg -> AddEntry(hist_sum, "Hits from other bunch crossings", "f");
    // leg -> AddEntry(hist_trigger, "Hits associated with trigger", "f");

    leg -> AddEntry(hist_trigger, "Hits associated with trigger", "f");
    leg -> AddEntry(hist_sum, "Hits from other beam crossings", "f");

    hist_sum -> GetYaxis() -> CenterTitle();
    hist_sum -> GetXaxis() -> CenterTitle();
    hist_sum -> SetMaximum(hist_trigger->GetBinContent(1) * 1.3);
    hist_sum->Draw("hist");
    // hist_trigger->Draw("hist same");
    // ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_data.c_str())); 
    ltx->DrawLatex(0.21 + label_shit_x, 0.89 + label_shit_y, Form("#it{#bf{sPHENIX}} %s #it{%s}", sPH_label_data.c_str(), sPHENIX_date.c_str())); 
    
    draw_text -> DrawLatex(0.21 + label_shit_x, 0.83 + label_shit_y, Collision_str.c_str());
    draw_text -> DrawLatex(0.21 + label_shit_x, 0.78 + label_shit_y, RHIC_str.c_str());
    draw_text -> DrawLatex(0.21 + label_shit_x, 0.73 + label_shit_y, ReadOut_str.c_str());

    leg -> Draw("same");

    hist_trigger->Draw("hist same");

    c1 -> cd();
    TPad * pad2 = new TPad("pad2", "pad2", 0.4, 0.25, 0.8, 0.55);
    pad2 -> Draw();
    pad2 -> cd();
    pad2 -> SetLeftMargin(0.2);
    // pad2 -> SetLogy();

    TH1D * hist_small = (TH1D*) hist_sum->Clone("hist_small");
    hist_small -> SetTitle(";;");
    hist_small -> SetMaximum(hist_sum->GetBinContent(hist_sum->GetMaximumBin()) * 0.3);
    hist_small -> SetMinimum(0);
    hist_small -> GetXaxis() -> SetNdivisions(505);
    hist_small -> GetYaxis() -> SetNdivisions(505);
    hist_small -> GetXaxis() -> SetRangeUser(39, 80);

    hist_small -> GetXaxis() -> SetLabelSize( hist_small -> GetXaxis() -> GetLabelSize() * 2.3 );
    hist_small -> GetXaxis() -> SetTickLength( hist_small -> GetXaxis() -> GetTickLength() * 1.5 );

    hist_small -> GetYaxis() -> SetLabelSize( hist_small -> GetYaxis() -> GetLabelSize() * 2.3 );
    hist_small -> GetYaxis() -> SetTickLength( hist_small -> GetYaxis() -> GetTickLength() * 1.5 );

    hist_small->Draw("hist");


    hist_sum->Draw("hist same");

    pad2->RedrawAxis();

    c1 -> Print(Form("%s/INTT_timing_smallpad.pdf",file_directory.c_str()));

    return 888;
}

int MakePlot_INTTTiming_bkg(int event_id = 0)
{
    std::string file_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/INTT_timing/completed";
    std::string file_name = "BcoDiffNtuple_54280_clonehitremoveBCO_hotchannelremove_hitQA_checkclonehit_merged.root";
    std::string hist_name_noindex = "all_felix_BcoDiffHist_";

    std::string sPH_label_data = "Internal";
    std::string Collision_str = "Au+Au#kern[0.25]{#sqrt{s_{NN}}} = 200 GeV";
    std::string RHIC_str = "Bunch crossing interval: 2 BCOs";
    std::string ReadOut_str = "INTT Trigger+Extended readout";

    int trigger_bin = 1;
    int trigger_bin_shift = 55;

    TFile *file = TFile::Open(Form("%s/%s", file_directory.c_str(), file_name.c_str()));
    
    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);
    c1 -> SetLogy();

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.04);

    TLegend * leg = new TLegend(0.21,0.77,0.8,0.84);
    // leg -> SetNColumns(2);
    leg -> SetBorderSize(0);
    leg -> SetTextSize(0.03);
    leg -> SetMargin(0.12);

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);
    

    // TTree * tree = (TTree*)file->Get("tree");   
    
    // std::vector<int> * felix0_bcodiff = 0;
    // std::vector<int> * felix1_bcodiff = 0;
    // std::vector<int> * felix2_bcodiff = 0;
    // std::vector<int> * felix3_bcodiff = 0;
    // std::vector<int> * felix4_bcodiff = 0;
    // std::vector<int> * felix5_bcodiff = 0;
    // std::vector<int> * felix6_bcodiff = 0;
    // std::vector<int> * felix7_bcodiff = 0;
    
    // tree -> SetBranchAddress("felix0_bcodiff", &felix0_bcodiff);
    // tree -> SetBranchAddress("felix1_bcodiff", &felix1_bcodiff);
    // tree -> SetBranchAddress("felix2_bcodiff", &felix2_bcodiff);
    // tree -> SetBranchAddress("felix3_bcodiff", &felix3_bcodiff);
    // tree -> SetBranchAddress("felix4_bcodiff", &felix4_bcodiff);
    // tree -> SetBranchAddress("felix5_bcodiff", &felix5_bcodiff);
    // tree -> SetBranchAddress("felix6_bcodiff", &felix6_bcodiff);
    // tree -> SetBranchAddress("felix7_bcodiff", &felix7_bcodiff);

    // TH1D * h1D_evt_bcodiff = new TH1D("h1D_evt_bcodiff", ";BCOs in one RHIC circulation;INTT hit counts", 128, 0, 128);

    // SetsPhenixStyle();

    // TCanvas * c1 = new TCanvas("c1", "c1", 950, 800);

    // // c1 -> Print("test.pdf[");

    // TH2D * h2D_Ratio = new TH2D("",";NInttHits at trigger bco;carry ratio",100,0,25000,100,0,0.1);

    // for (int i = 0; i < 1000000; i++)
    // {
    //     tree -> GetEntry(i);

    //     for (int i = 0; i < felix0_bcodiff->size(); i++)
    //     {
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix0_bcodiff->at(i));
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix1_bcodiff->at(i));
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix2_bcodiff->at(i));
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix3_bcodiff->at(i));
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix4_bcodiff->at(i));
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix5_bcodiff->at(i));
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix6_bcodiff->at(i));
    //         h1D_evt_bcodiff->SetBinContent(i+1, h1D_evt_bcodiff->GetBinContent(i+1) + felix7_bcodiff->at(i));
    //     }

    //     h2D_Ratio -> Fill(h1D_evt_bcodiff->GetBinContent(trigger_bin), h1D_evt_bcodiff->GetBinContent(57)/h1D_evt_bcodiff->GetBinContent(trigger_bin));

    //     // if (h1D_evt_bcodiff->GetBinContent(57)/h1D_evt_bcodiff->GetBinContent(trigger_bin) < 0.005 && h1D_evt_bcodiff->GetBinContent(trigger_bin) > 1000)
    //     // {
    //     //     c1 -> SetLogy();
    //     //     h1D_evt_bcodiff -> Draw("hist");
    //     //     c1 -> Print("test.pdf");
    //     // }



    //     h1D_evt_bcodiff -> Reset("ICESM");
    // }

    // h2D_Ratio -> GetXaxis() -> SetNdivisions(505);
    // h2D_Ratio -> Draw("colz0");

    // c1 -> Print("test.pdf]");


    std::vector<TH1D *> hist_vec; hist_vec.clear();
    THStack * hs = new THStack("","");

    for (int i = 0; i < 8; i++)
    {
        hist_vec.push_back((TH1D*)file->Get(Form("%s%d", hist_name_noindex.c_str(), i)));
        
        hs->Add(hist_vec.back());
    }

    // c1 -> SetLogy(1);
    
    TH1D * hist_first = (TH1D*)hs->GetStack()->Last();

    TH1D * hist_sum = hist_shift_inX(hist_first, trigger_bin_shift);
    
    hist_sum -> Scale(1.0/hist_sum->Integral());
    hist_sum -> SetMaximum(hist_sum->GetBinContent(hist_sum->GetMaximumBin())*10000);
    // hist_sum -> SetMinimum(1);

    hist_sum -> SetName("hist_sum");
    // hist_sum -> SetTitle(";BCOs in the RHIC circulations;INTT hit counts");
    hist_sum -> SetTitle(";Crossing relative to RHIC clock [BCO];INTT hit counts [arb. unit]");
    hist_sum -> SetLineColor(kBlack);
    hist_sum -> GetYaxis() -> CenterTitle();
    hist_sum -> GetXaxis() -> CenterTitle();
    hist_sum -> GetYaxis() -> SetNdivisions(505);

    

    TH1D * hist_trigger = (TH1D*) hist_sum->Clone("hist_trigger");
    hist_trigger -> Reset("ICESM");
    hist_trigger -> SetBinContent(trigger_bin, hist_sum->GetBinContent(trigger_bin));
    hist_trigger -> SetLineColor(kRed);

    // leg -> AddEntry(hist_sum, "Hits from other bunch crossings", "f");
    // leg -> AddEntry(hist_trigger, "Hits associated with trigger", "f");

    leg -> AddEntry(hist_trigger, "Hits associated with trigger", "f");
    leg -> AddEntry(hist_sum, "Hits from other bunch crossings", "f");

    hist_sum->Draw("hist");
    // hist_trigger->Draw("hist same");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sPH_label_data.c_str())); 
    
    draw_text -> DrawLatex(0.17, 0.965, Collision_str.c_str());
    draw_text -> DrawLatex(0.21, 0.89, RHIC_str.c_str());
    draw_text -> DrawLatex(0.21, 0.85, ReadOut_str.c_str());

    leg -> Draw("same");

    hist_trigger->Draw("hist same");

    c1 -> Print(Form("hist_sum.pdf"));

    return 888;
}

int MakePlot_INTTTiming()
{
    MakePlot_INTTTiming_TwoPanel();
    MakePlot_INTTTiming_chop();
    MakePlot_INTTTiming_smallpad();

    return 888;
}