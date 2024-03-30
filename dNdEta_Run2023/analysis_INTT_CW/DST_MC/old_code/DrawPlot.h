#ifndef DrawPlot_h
#define DrawPlot_h

#include "../sPhenixStyle.C"

class DrawPlot 
{
    public :
        TCanvas * c1;
        TH1F * hist1;
        TH1F * hist2;

        DrawPlot(vector<double> vec1, vector<double> vec2, int Nbins_1D, double edgeL_1D, double edgeR_1D, string X_title, string Y_title, bool set_logY, bool normalized = true, int normalized_binL = -1, int normalized_binR = -1, int sizeX = 950, int sizeY = 800, int stat_box = 0); // note : two vectors
        DrawPlot(long long N_event, TTree* tree1, string branch_name1, TTree* tree2, string branch_name2, int Nbins_1D, double edgeL_1D, double edgeR_1D, string X_title, string Y_title, bool set_logY, bool normalized = true, int normalized_binL = -1, int normalized_binR = -1, int sizeX = 950, int sizeY = 800, int stat_box = 0); // note : two trees
        void canvas_draw(string output_directoy, string plot_name);
    
    private :     
        int sizeX; 
        int sizeY;
        int Nbins_1D; 
        double edgeL_1D; 
        double edgeR_1D;
        bool set_logY;
        string X_title;
        string Y_title;
        bool normalized;
        int normalized_binL; 
        int normalized_binR;
        int stat_box;

        void canvas_init();
        void fill_1D_hist(TH1F * hist_in, vector<double> vec1);
        void hist_init(TH1F * hist_in, string LineColor);
        void hist_normalize (TH1F * hist_in);
        
};

DrawPlot::DrawPlot(vector<double> vec1, vector<double> vec2, int Nbins_1D, double edgeL_1D, double edgeR_1D, string X_title, string Y_title, bool set_logY, bool normalized, int normalized_binL, int normalized_binR, int sizeX, int sizeY, int stat_box)
:sizeX(sizeX), sizeY(sizeY), Nbins_1D(Nbins_1D), edgeL_1D(edgeL_1D), edgeR_1D(edgeR_1D), set_logY(set_logY), X_title(X_title), Y_title(Y_title), normalized(normalized), normalized_binL(normalized_binL), normalized_binR(normalized_binR), stat_box(stat_box)
{
    SetsPhenixStyle();
    canvas_init();

    hist1 = new TH1F("hist1","", Nbins_1D, edgeL_1D, edgeR_1D); hist_init(hist1, "#1A3947");
    hist2 = new TH1F("hist2","", Nbins_1D, edgeL_1D, edgeR_1D); hist_init(hist2, "#732d41");
    fill_1D_hist(hist1, vec1); 
    fill_1D_hist(hist2, vec2);

    if (normalized){
        hist_normalize(hist1);
        hist_normalize(hist2);
    }

    double Y_range_up = ( hist1->GetBinContent(hist1->GetMaximumBin()) > hist2->GetBinContent(hist2->GetMaximumBin()) ) ? hist1->GetBinContent(hist1->GetMaximumBin()) * 1.4 : hist2->GetBinContent(hist2->GetMaximumBin()) * 1.4;
    hist1 -> SetMaximum(Y_range_up);
    hist1 -> SetMinimum(0);

    hist1 -> Draw("hist");
    hist2 -> Draw("hist same");
}

DrawPlot::DrawPlot(long long N_event, TTree* tree1, string branch_name1, TTree* tree2, string branch_name2, int Nbins_1D, double edgeL_1D, double edgeR_1D, string X_title, string Y_title, bool set_logY, bool normalized, int normalized_binL, int normalized_binR, int sizeX, int sizeY, int stat_box)
:sizeX(sizeX), sizeY(sizeY), Nbins_1D(Nbins_1D), edgeL_1D(edgeL_1D), edgeR_1D(edgeR_1D), set_logY(set_logY), X_title(X_title), Y_title(Y_title), normalized(normalized), normalized_binL(normalized_binL), normalized_binR(normalized_binR), stat_box(stat_box)
{
    SetsPhenixStyle();
    canvas_init();
    
    hist1 = new TH1F("hist1","", Nbins_1D, edgeL_1D, edgeR_1D); hist_init(hist1, "#1A3947");
    hist2 = new TH1F("hist2","", Nbins_1D, edgeL_1D, edgeR_1D); hist_init(hist2, "#eb1765");

    tree1 -> Draw(Form("%s>>%s",branch_name1.c_str(),"hist1"), "", "", N_event);
    tree2 -> Draw(Form("%s>>%s",branch_name2.c_str(),"hist2"), "", "", N_event);

    if (normalized){
        hist_normalize(hist1);
        hist_normalize(hist2);
    }

    double Y_range_up = ( hist1->GetBinContent(hist1->GetMaximumBin()) > hist2->GetBinContent(hist2->GetMaximumBin()) ) ? hist1->GetBinContent(hist1->GetMaximumBin()) * 1.4 : hist2->GetBinContent(hist2->GetMaximumBin()) * 1.4;
    hist1 -> SetMaximum(Y_range_up);
    hist1 -> SetMinimum(0);

    hist1 -> Draw("hist");
    hist2 -> Draw("hist same");
}

void DrawPlot::hist_init(TH1F * hist_in, string LineColor)
{
    hist_in -> SetLineColor(TColor::GetColor(LineColor.c_str())); 
    hist_in -> SetLineWidth(2);
    hist_in -> SetStats(stat_box);
    hist_in -> GetXaxis() -> SetTitle(X_title.c_str());
    hist_in -> GetXaxis() -> SetNdivisions(505);
    hist_in -> GetYaxis() -> SetTitle(Y_title.c_str());
}

void DrawPlot::fill_1D_hist(TH1F * hist_in, vector<double> vec1)
{
    for (auto element : vec1){ hist_in -> Fill(element); }
}

void DrawPlot::hist_normalize(TH1F * hist_in)
{
    cout<<"normalize consider region : "<<normalized_binL<<" "<<normalized_binR<<endl;
    cout<<"the input hist : "<< hist_in -> GetName() <<" integral : "<<hist_in -> Integral(normalized_binL,normalized_binR)<<endl;
    hist_in -> Scale(1./hist_in -> Integral(normalized_binL,normalized_binR));
}

void DrawPlot::canvas_init()
{
    c1 = new TCanvas("","",sizeX,sizeY);
}


void DrawPlot::canvas_draw(string output_directoy, string plot_name)
{
    c1 -> Print((output_directoy +"/"+ plot_name).c_str());
}



#endif