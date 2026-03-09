float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.045;
float LeftMargin = 0.15;
float RightMargin = 0.05;
float TopMargin = 0.08;
float BottomMargin = 0.13;
float textscale = 2.6;

std::vector<TGraphAsymmErrors *> v_STAR;

void getHistogram(std::string HepdataFile = "./dNdPt/HEPData-ins1676541-v1-Figure_2___0.40__Mee__0.76_GeV_c2_in_Au+Au_collisions.root")
{
    TFile *f = new TFile(HepdataFile.c_str(), "READ");
    TH1F *hm = (TH1F *)f->Get("Hist1D_y1");
    hm->SetDirectory(0);
    return hm;
}

void draw_dNdPt()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "./dNdPt";
    system(Form("mkdir -p %s", plotdir.c_str()));

    std::string filepath = "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Sim_HIJING_MDC2_ana472_20250307/";
     ROOT::EnableImplicitMT();
    std::vector<std::string> filelist;
    for (int i = 0; i < 100; i++)
    {
        std::string fname = filepath + "ntuple_" + std::string(TString::Format("%05d", i).Data()) + ".root";

        // Check if file exists and is valid
        TFile *f = TFile::Open(fname.c_str());
        if (!f || f->IsZombie() || f->GetSize() <= 0)
        {
            std::cout << "Skipping invalid or empty file: " << fname << std::endl;
            if (f)
                f->Close();
            continue;
        }

        // Check if file contains the required tree
        if (!f->GetListOfKeys()->Contains("EventTree"))
        {
            std::cout << "Skipping file without EventTree: " << fname << std::endl;
            f->Close();
            continue;
        }

        f->Close();
        std::cout << "Adding file: " << fname << std::endl;
        filelist.push_back(fname);
    }
    ROOT::RDataFrame df("EventTree", filelist);

    // first, filter the events with is_min_bias==1 and MBD_centrality > 60 and MBD_centrality < 80
    auto df_minbias = df.Filter("is_min_bias==1 && MBD_centrality >= 60 && MBD_centrality < 80");
    int Nevt = df_minbias.Count().GetValue();
    // Add a new column, PrimaryG4P_Pt_eta1p1, with PrimaryG4P_isChargeHadron==1 within |eta|<=1.1 for each event
    auto df_Nptcl = df_minbias.Define("PrimaryG4P_Pt_eta1p1",
                                      [](const std::vector<float> &PrimaryG4P_Pt, const std::vector<float> &PrimaryG4P_Eta, const std::vector<bool> &PrimaryG4P_isChargeHadron)
                                      {
                                          std::vector<float> Pt_eta1p1;
                                          for (size_t i = 0; i < PrimaryG4P_Pt.size(); i++)
                                          {
                                              if (PrimaryG4P_isChargeHadron[i] == 1 && fabs(PrimaryG4P_Eta[i]) <= 1.1)
                                              {
                                                  Pt_eta1p1.push_back(PrimaryG4P_Pt[i]);
                                              }
                                          }
                                          return Pt_eta1p1;
                                      },
                                      {"PrimaryG4P_Pt", "PrimaryG4P_Eta", "PrimaryG4P_isChargeHadron"});

    // histogra of PrimaryG4P_Pt with PrimaryG4P_isChargeHadron==1 within |eta|<=1.1
    auto hM_dNchdPt = df_Nptcl.Histo1D({"hM_dNchdPt", "dNch/dPt;Pt (GeV/c);dNch/dPt", 50, 0, 1}, "PrimaryG4P_Pt_eta1p1");

    // normalize the histogram
    hM_dNchdPt->Scale(1./Nevt, "width");

    // get the histogram from Hepdata
    TH1F *h_STAR_Mee0p4to0p76 = getHistogram(Form("%s/HEPData-ins1676541-v1-Figure_2___0.40__Mee__0.76_GeV_c2_in_Au+Au_collisions.root",plotdir.c_str()));


    // draw the histogram
    TCanvas *c = new TCanvas("c", "c", 800, 600);
    c->SetLogy(1);
    c->cd();
    hM_dNchdPt->SetLineColor(kBlack);
    hM_dNchdPt->SetLineWidth(2);
    hM_dNchdPt->Draw("HIST");
    c->SaveAs(Form("%s/dNdPt.png", plotdir.c_str()));
    c->SaveAs(Form("%s/dNdPt.pdf", plotdir.c_str()));
}