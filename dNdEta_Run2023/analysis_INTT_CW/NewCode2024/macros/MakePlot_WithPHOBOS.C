#include "../Constants.cpp"

TGraphAsymmErrors * GetPHOBOSData(string PHOBOS_directory, string PHOBOS_filename)
{
    ifstream PHOBOS_file(Form("%s/%s",PHOBOS_directory.c_str(),PHOBOS_filename.c_str()));
    if(!PHOBOS_file.is_open())
    {
        cout<<"Error: PHOBOS file not found!"<<endl;
        return nullptr;
    }

    TGraphAsymmErrors * PHOBOS_data = new TGraphAsymmErrors();
    PHOBOS_data -> SetTitle("PHOBOS_data;#eta;dN_{ch}/d#eta");

    std::string line;
    while (std::getline(PHOBOS_file, line)) {
        // Ignore lines that start with '#' or '&'
        if (line.empty() || line[0] == '#' || line[0] == '&') continue;

        std::istringstream iss(line);
        double eta;
        double dNdEta; 
        double err_plus; 
        double err_neg;
        
        if (iss >> eta >> dNdEta >> err_plus >> err_neg) {
            PHOBOS_data -> SetPoint(PHOBOS_data -> GetN(), eta, dNdEta);
            PHOBOS_data -> SetPointError(PHOBOS_data -> GetN()-1, 0, 0, fabs(err_neg), err_plus);

            std::cout<<eta<<" "<<dNdEta<<" "<<err_plus<<" "<<err_neg<<std::endl;

            // eta.push_back(eta);
            // dN_dEta.push_back(dNdEta);
            // err_plus.push_back(err_plus);
            // err_minus.push_back(err_neg);
        }
    }

    PHOBOS_file.close();

    PHOBOS_data -> SetMarkerStyle(21);
    PHOBOS_data -> SetMarkerSize(1);
    PHOBOS_data -> SetMarkerColor(2);
    PHOBOS_data -> SetFillColorAlpha(45,0.5);
    PHOBOS_data -> SetLineColorAlpha(1,0);
    PHOBOS_data -> SetLineWidth(0);
    PHOBOS_data -> GetXaxis() -> SetLimits(-2.7,2.7);


    return PHOBOS_data;

}

int MakePlot_WithPHOBOS(int Mbin = 0)
{
    // int Mbin = 0;

    string PHOBOS_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/DataPoint_PHOBOS-PhysRevC.83.024913";
    string PHOBOS_filename = Form("AuAu_200GeV_Centrality_%s.txt",Constants::centrality_text[Mbin].c_str());

    string sPHENIX_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280",Mbin,Mbin); 
    string sPHENIX_filename = Form("Final_Mbin%d_00054280.root", Mbin);
    string sPHENIX_gr_name = "gE_dNdEta_reco";


    string sPHENIX2_directory = Form("/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_Nominal0Adc/completed/vtxZ_-10_10cm_MBin%d/Final_Mbin%d_00054280",Mbin,Mbin); 
    string sPHENIX2_filename = Form("Final_Mbin%d_00054280.root", Mbin);
    string sPHENIX2_gr_name = "gE_dNdEta_reco";

    std::string output_directory = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280_HR_Feb102025/Run6_EvtZFitWidthChange/EvtVtxZ/FinalResult_10cm_Pol2BkgFit/completed/comp_PHOBOS";

    TFile * sPHENIX_file = TFile::Open(Form("%s/%s",sPHENIX_directory.c_str(),sPHENIX_filename.c_str()));
    TGraphErrors * sPHENIX_data = (TGraphErrors*)sPHENIX_file -> Get(sPHENIX_gr_name.c_str());
    sPHENIX_data -> GetYaxis() -> SetRangeUser(0, Constants::centrality_Hist_Ymax[Mbin]);
    sPHENIX_data -> SetMarkerStyle(20);
    sPHENIX_data -> SetMarkerSize(1);
    sPHENIX_data -> SetMarkerColor(1);
    sPHENIX_data -> SetFillColorAlpha(1,0.5);
    sPHENIX_data -> SetLineColorAlpha(1,0);
    sPHENIX_data -> SetLineWidth(0);
    sPHENIX_data -> GetXaxis() -> SetLimits(-2.7,2.7);

    TFile * sPHENIX2_file = TFile::Open(Form("%s/%s",sPHENIX2_directory.c_str(),sPHENIX2_filename.c_str()));
    TGraphErrors * sPHENIX2_data = (TGraphErrors*)sPHENIX2_file -> Get(sPHENIX2_gr_name.c_str());
    sPHENIX2_data -> GetYaxis() -> SetRangeUser(0, Constants::centrality_Hist_Ymax[Mbin]);
    sPHENIX2_data -> SetMarkerStyle(20);
    sPHENIX2_data -> SetMarkerSize(1);
    sPHENIX2_data -> SetMarkerColor(4);
    sPHENIX2_data -> SetFillColorAlpha(4,0.5);
    sPHENIX2_data -> SetLineColorAlpha(1,0);
    sPHENIX2_data -> SetLineWidth(0);
    sPHENIX2_data -> GetXaxis() -> SetLimits(-2.7,2.7);

    

    TCanvas * c = new TCanvas("c","c",800,600);
    

    // PHOBOS_data -> Draw("E3 same");
    // PHOBOS_data -> Draw("P same");  

    if (Mbin < 11)
    {
        TGraphAsymmErrors * PHOBOS_data = GetPHOBOSData(PHOBOS_directory, PHOBOS_filename);
        PHOBOS_data -> GetYaxis() -> SetRangeUser(0, Constants::centrality_Hist_Ymax[Mbin]);

        PHOBOS_data -> Draw("AE3");
        PHOBOS_data -> Draw("P same");  

        sPHENIX_data -> Draw("P2 same");
        sPHENIX2_data -> Draw("P2 same");
    }
    else 
    {
        sPHENIX_data -> Draw("AP2");
        sPHENIX2_data -> Draw("P2 same");
    
    }





    c->Print(Form("%s/sPHENIX_vs_PHOBOS_Mbin%d.pdf",output_directory.c_str(),Mbin));

    return 888;
}