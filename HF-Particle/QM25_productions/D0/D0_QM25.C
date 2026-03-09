using namespace std;

string plotPath = "/sphenix/user/cdean/public/QM25";

string changeType[] = {"oldKFP_base_cut_ana475p017_optimized"}; 
//string changeType[] = {"D0_eta", "D0_pT", "D0_decLenOverErr", "PV_z", "track_TPC_states", "track_MVTX_states", "track_pT", "track_eta", "track_INTT_hits", "D0_decLen", "D0_DIRA", "D0_chi2"};

class plotParameters
{
  public:
    string branch;
    string title;
    string units;
    int bins;
    float min;
    float max;
    int precision; 

    plotParameters(string constructor_branch, string constructor_title, string constructor_units, int constructor_bins, float constructor_min, float constructor_max, int candidate_precision)
    {
      branch = constructor_branch;
      title = constructor_title;
      units = constructor_units;
      bins = constructor_bins;
      min = constructor_min;
      max = constructor_max;
      precision = candidate_precision;
    }
};

template <typename T>
string to_string_with_precision(const T a_value, const int n = 1)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

template <typename T>
void savePlots(T myPlot, string plotName, bool logY)
{
  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  myPlot.GetXaxis()->SetNdivisions(505);
  myPlot.GetYaxis()->SetNdivisions(505);

  if (strncmp(typeid(myPlot).name(), "4TH2F", 5) == 0)
  {
    myPlot.Draw("COLZ");
  }
  else
  {
    myPlot.Sumw2();
    if (logY) gPad->SetLogy();
    myPlot.Draw("PE1");
  }

  TPaveText *pt;
  pt = new TPaveText(0.25,0.94,0.99,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex = pt->AddText("#it{#bf{sPHENIX}} Internal, #sqrt{s} = 200 GeV, pp");
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  string extensions[] = {".png", ".C", ".pdf"};
  for (auto extension : extensions)
  {
    string output = plotPath + "/plots/" + plotName + extension;
    c1->SaveAs(output.c_str());
  }
}

TH1F makeHisto(int nBins, float min, float max, string xAxisTitle, string unit, int precision)
{
  TH1F myHisto(xAxisTitle.c_str(), xAxisTitle.c_str(), nBins, min, max);
  
  if (unit != "") xAxisTitle += " [" + unit +  "]";  
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());

  float binWidth = (float) (max - min)/nBins;
  string yAxisTitle;
  if (unit != "") yAxisTitle = "Candidates / " + to_string_with_precision(binWidth, precision) + " " + unit;
  else yAxisTitle = "Candidates";
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());
  
  return myHisto;
}

TH2F makeHisto(int x_nBins, float x_min, float x_max, string xAxisTitle, string x_unit, int y_nBins, float y_min, float y_max, string yAxisTitle, string y_unit)
{
  string combined_title = xAxisTitle + yAxisTitle;
  TH2F myHisto(combined_title.c_str(), combined_title.c_str(), x_nBins, x_min, x_max, y_nBins, y_min, y_max);
  
  if (x_unit != "") xAxisTitle += " [" + x_unit +  "]";  
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());
  
  if (y_unit != "") yAxisTitle += " [" + y_unit +  "]";  
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());

  return myHisto;
}

void D0_QM25()
{
  int maxDigits = 3;
  TGaxis::SetMaxDigits(maxDigits);
  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat(".3f");

  TCanvas *c1  = new TCanvas("", "",800,800);

  for (auto change : changeType)
  {
    string fileDir = "/sphenix/tg/tg01/hf/cdean/QM25_productions_ana475p017_20250401_2326/Kpi_reco/";

    TChain *myChain = new TChain("DecayTree");
    TSystemDirectory dir("reco_files", fileDir.c_str());
    TList *files = dir.GetListOfFiles();
    TIter fileIterator(files);
    TSystemFile *file;

    int maxFiles = -1;
    int currentLoadedFiles = 0;
    while ((file = (TSystemFile*)fileIterator()))
    {
      string fileName(file->GetName());
      if (fileName.find(".root") != string::npos)
      {
        if (fileName.find("53879") != string::npos) continue;
        ++currentLoadedFiles;
        if (currentLoadedFiles == maxFiles) break;
        string inputFile = fileDir + fileName;
        myChain->Add(inputFile.c_str());
      }
    }
    vector<TCut> individualCuts;

    individualCuts.push_back("D0_pT > 0.75");
    individualCuts.push_back("D0_decayLength_xy > 0.008 && D0_decayLength_xy < 0.07"); 
    individualCuts.push_back("D0_decayLength_xy/D0_decayLengthErr_xy > 0.05"); 
    individualCuts.push_back("abs(D0_DIRA) > 0.985");
    individualCuts.push_back("abs(D0_chi2) < 14");
    individualCuts.push_back("D0_IP < 0.005");
    individualCuts.push_back("abs(primary_vertex_z) < 10.");
    individualCuts.push_back("min(track_1_TPC_nStates, track_2_TPC_nStates) > 25");
    individualCuts.push_back("min(track_1_INTT_nHits, track_2_INTT_nHits) > 1");
    individualCuts.push_back("min(track_1_MVTX_nStates, track_2_MVTX_nStates) == 3");
    individualCuts.push_back("0 <= track_1_bunch_crossing && track_1_bunch_crossing < 350");
    individualCuts.push_back("min(track_1_pT, track_2_pT) > 0.2");
    individualCuts.push_back("max(abs(track_1_IP_xy), abs(track_2_IP_xy)) < 0.02");
    individualCuts.push_back("track_1_track_2_DCA_xy < 0.002");
 
    TCut total;
    for (auto cut : individualCuts)
    {
      total += cut;
    }

    string outFileName = plotPath + "/output_D0_" + change + ".root";
    TFile *outFile = TFile::Open(outFileName.c_str(),"RECREATE");
    TTree* dataWithCut = myChain->CopyTree(total);
    outFile->Write();

    int standard_bins = 50, standard_2D_bins = 150;
    vector<plotParameters> plots, twoD_plots;

    twoD_plots.push_back(plotParameters("D0_mass", "m_{K^{+}#pi^{-}}", "GeV", standard_bins, 1.7, 2.0, 2));
    twoD_plots.push_back(plotParameters("D0_pseudorapidity", "D^{0} #eta", "", standard_bins, -1.2, 1.2, 2));
    twoD_plots.push_back(plotParameters("D0_pT", "D^{0} p_{T}", "GeV", standard_bins, 0.5, 4, 2));
    twoD_plots.push_back(plotParameters("D0_decayLength/D0_decayLengthErr", "D^{0}  Decay Length Over Error", "", standard_bins, 4, 10, 2));
    twoD_plots.push_back(plotParameters("D0_decayLength", "D^{0} decay length", "cm", standard_bins, 0, 0.06, 4));
    twoD_plots.push_back(plotParameters("D0_DIRA", "D^{0} DIRA", "", standard_bins, 0.9, 1, 2));
    twoD_plots.push_back(plotParameters("D0_chi2", "D^{0} SV #chi^{2}", "", standard_bins, 0, 25, 2));
    twoD_plots.push_back(plotParameters("D0_IP", "D^{0} IP", "cm", standard_bins, 0, 0.2, 4));
    twoD_plots.push_back(plotParameters("primary_vertex_z", "vtx_{z}", "cm", standard_bins, -13.5, 13.5, 2));
    twoD_plots.push_back(plotParameters("min(track_1_TPC_nStates, track_2_TPC_nStates)", "min. track TPC states", "", 31, 20, 50, 2));
    twoD_plots.push_back(plotParameters("min(track_1_INTT_nHits, track_2_INTT_nHits)", "min. track INTT clusters", "", 4, 0, 3, 2));
    twoD_plots.push_back(plotParameters("min(track_1_MVTX_nStates, track_2_MVTX_nStates)", "min. track MVTX states", "", 6, 0, 5, 2));
    twoD_plots.push_back(plotParameters("min(track_1_pT, track_2_pT)", "min. track p_{T}", "GeV", standard_bins, 0, 2, 2));
    twoD_plots.push_back(plotParameters("track_1_pseudorapidity", "track 1 #eta", "", standard_bins, -1.2, 1.2, 2));
    twoD_plots.push_back(plotParameters("track_2_pseudorapidity", "track 2 #eta", "", standard_bins, -1.2, 1.2, 2));
    twoD_plots.push_back(plotParameters("max(abs(track_1_IP_xy), abs(track_2_IP_xy))", "max. |track IP_{xy}|", "cm", standard_bins, 0, 0.15, 4));
    twoD_plots.push_back(plotParameters("track_1_track_2_DCA_xy", "track-to-track DCA", "cm", standard_bins, 0, 0.005, 4));

    for (unsigned int i = 0; i < twoD_plots.size() - 1; ++i)
    {
      for (unsigned int j = i+1; j < twoD_plots.size(); ++j)
      {
        TH2F twoD_plot = makeHisto(standard_2D_bins, twoD_plots[i].min, twoD_plots[i].max, twoD_plots[i].title, twoD_plots[i].units,
                                   standard_2D_bins, twoD_plots[j].min, twoD_plots[j].max, twoD_plots[j].title, twoD_plots[j].units);
        string send_to_plot = twoD_plots[j].branch + ":" + twoD_plots[i].branch +  " >> " + twoD_plots[i].title + twoD_plots[j].title;
        dataWithCut->Draw(send_to_plot.c_str(), total);
        string saveName_first = twoD_plots[i].branch == "D0_decayLength/D0_decayLengthErr" ? "D0_decayLength_over_D0_decayLengthErr" : twoD_plots[i].branch;
        string saveName_second = twoD_plots[j].branch == "D0_decayLength/D0_decayLengthErr" ? "D0_decayLength_over_D0_decayLengthErr" : twoD_plots[j].branch;
        string saveName = saveName_first + "_" + saveName_second + "_altered_" + change;
        savePlots(twoD_plot, saveName, false); 
      }
    }

    for (auto& plot : twoD_plots)
    {
      TH1F histo = makeHisto(plot.bins, plot.min, plot.max, plot.title, plot.units, plot.precision);
      string send_to_plot =  plot.branch + " >> " + plot.title;
      dataWithCut->Draw(send_to_plot.c_str(), total);
      if (plot.branch == "D0_decayLength/D0_decayLengthErr") savePlots(histo, "D0_decayLength_over_D0_decayLengthErr", false);
      else savePlots(histo, plot.branch, false);
    }
  }
}
