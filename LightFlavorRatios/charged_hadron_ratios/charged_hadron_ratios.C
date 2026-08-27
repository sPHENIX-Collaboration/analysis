#include <algorithm>

float lower_bin_bounds_pion[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.8, 2.1, 2.4, 2.7, 3, 4};
const unsigned int n_variable_bins_pion = sizeof(lower_bin_bounds_pion)/sizeof(lower_bin_bounds_pion[0]) - 1; 
float lower_bin_bounds[] = {0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
const unsigned int n_variable_bins = sizeof(lower_bin_bounds)/sizeof(lower_bin_bounds[0]) - 1; 

float min_track_pT_proton = 1.0;
float min_track_pT_kaon = 0.75;
int nBins_eta = 50;
int nBins_phi = 20;
int nBins_pT = 20;
float eta_max = 1.2;
float phi_max = M_PI;

string plotPath = "plots_negative_track_to_positive_track_with_DCA/";
//string plotPath = "plots_negative_track_to_positive_track/";
string makeDirectory = "mkdir -p " + plotPath;
system(makeDirectory.c_str());

template <typename T>
string to_string_with_precision(const T a_value, const int n = 0)
{
  ostringstream out;
  out.precision(n);
  out << fixed << a_value;
  return out.str();
}

//Initialise histograms
TH1F makeHisto(int nBins, float min, float max, string xAxisTitle, string unit, string yAxisTitle)
{
  TH1F myHisto(yAxisTitle.c_str(), yAxisTitle.c_str(), nBins, min, max);
  if (unit != "") xAxisTitle += " [" + unit +  "]";
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());
  
  return myHisto;
}

TH1F makeHisto(int nBins, float* min, string xAxisTitle, string unit, string yAxisTitle = "Raw Ratio")
{
  TH1F myHisto(xAxisTitle.c_str(), xAxisTitle.c_str(), nBins, min);

  if (unit != "") xAxisTitle += " [" + unit +  "]";
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());

  return myHisto;
}

TH1F pT_proton_histo = makeHisto(nBins_pT, 0, min_track_pT_proton, "p_{T}", "GeV", "p Yield");
TH1F pT_antiproton_histo = makeHisto(nBins_pT, 0, min_track_pT_proton, "p_{T}", "GeV", "#bar{p} Yield");
TH1F pT_proton_ratio = makeHisto(nBins_pT, 0, min_track_pT_proton, "p_{T}", "GeV", "#bar{p}/p Raw Ratio");
TH1F pT_piplus_histo = makeHisto(nBins_pT, 0, 4, "p_{T}", "GeV", "#pi^{+} Yield");
TH1F pT_piminus_histo = makeHisto(nBins_pT, 0, 4, "p_{T}", "GeV", "#pi^{-} Yield");
TH1F pT_pion_ratio = makeHisto(nBins_pT, 0, 4, "p_{T}", "GeV", "#pi^{-}/#pi^{+} Raw Ratio");
TH1F pT_Kplus_histo = makeHisto(nBins_pT, 0, min_track_pT_kaon, "p_{T}", "GeV", "K^{+} Yield");
TH1F pT_Kminus_histo = makeHisto(nBins_pT, 0, min_track_pT_kaon, "p_{T}", "GeV", "K^{-} Yield");
TH1F pT_Kaon_ratio = makeHisto(nBins_pT, 0, min_track_pT_kaon, "p_{T}", "GeV", "K^{-}/K^{+} Raw Ratio");
TH1F eta_proton_histo = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "p Yield");
TH1F eta_antiproton_histo = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "#bar{p} Yield");
TH1F eta_proton_ratio = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "#bar{p}/p Raw Ratio");
TH1F eta_piplus_histo = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "#pi^{+} Yield");
TH1F eta_piminus_histo = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "#pi^{-} Yield");
TH1F eta_pion_ratio = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "#pi^{-}/#pi^{+} Raw Ratio");
TH1F eta_Kplus_histo = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "K^{+} Yield");
TH1F eta_Kminus_histo = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "K^{-} Yield");
TH1F eta_Kaon_ratio = makeHisto(nBins_eta, -1*eta_max, eta_max, "#eta", "", "K^{-}/K^{+} Raw Ratio");
TH1F phi_proton_histo = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "p Yield");
TH1F phi_antiproton_histo = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "#bar{p} Yield");
TH1F phi_proton_ratio = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "#bar{p}/p Raw Ratio");
TH1F phi_piplus_histo = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "#pi^{+} Yield");
TH1F phi_piminus_histo = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "#pi^{-} Yield");
TH1F phi_pion_ratio = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "#pi^{-}/#pi^{+} Raw Ratio");
TH1F phi_Kplus_histo = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "K^{+} Yield");
TH1F phi_Kminus_histo = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "K^{-} Yield");
TH1F phi_Kaon_ratio = makeHisto(nBins_phi, -1*phi_max, phi_max, "#phi", "", "K^{-}/K^{+} Raw Ratio");

template <typename T>
void savePlots(T myPlot, string plotName)
{
  TGaxis::SetMaxDigits(3);

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  myPlot.GetXaxis()->SetNdivisions(505);
  myPlot.GetYaxis()->SetRangeUser(0, myPlot.GetMaximum()*1.2);
  //myPlot.Sumw2();
  myPlot.Draw("PE1");

  TPaveText *pt;
  pt = new TPaveText(0.15,0.84,0.95,0.89, "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex;
  pt->AddText("#it{#bf{sPHENIX}} Internal, #sqrt{s} = 200 GeV, pp");
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  string extensions[] = {".C", ".pdf", ".png"};
  for (auto extension : extensions)
  {
    string output = plotPath + plotName + extension;
    c1->SaveAs(output.c_str());
  }
}

template <typename T>
void saveManyPlots(vector<T> myPlot, string plotName, bool logY = false, vector<string> legendText = {""})
{
  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  if (logY) gPad->SetLogy();

  int colors[] = {4, 2, 6, 1, 3, 6, 7, 8, 9};
  Size_t markerStyle[] = {8, 21, 33, 34};
  Size_t markerSize = 1;

  float legendYpos[2] = {0.65, 0.85};
  float legendTextSize = ((legendYpos[1] - legendYpos[0])/(1.6*myPlot.size()));
  TLegend *legend = new TLegend(0.65,legendYpos[0],0.80, legendYpos[1]);
  legend->SetTextFont(42);

  for (unsigned int i = 0; i < myPlot.size(); ++i)
  {
    float transparency = 1;
    myPlot[i].SetFillColorAlpha(colors[i], transparency);
    myPlot[i].SetLineColor(colors[i]);
    myPlot[i].SetMarkerColorAlpha(colors[i], transparency);
    myPlot[i].SetMarkerSize(markerSize);
    myPlot[i].SetMarkerStyle(markerStyle[i]);
    if (legendText.size() < 2) legend->AddEntry(&myPlot[i], myPlot[i].GetXaxis()->GetTitle(), "PL");
    else legend->AddEntry(&myPlot[i], legendText[i].c_str(), "PL");
  }

  myPlot[0].GetXaxis()->SetNdivisions(505);
  float min = std::max(myPlot[0].GetMinimum(), 1.);
  myPlot[0].SetMinimum(min*0.8);
  float max = logY ? myPlot[0].GetMaximum()*1e3 : myPlot[0].GetMaximum()*1.2;
  myPlot[0].SetMaximum(max);
  if (strncmp(typeid(myPlot[0]).name(), "4TH2F", 5) == 0)
  {
    myPlot[0].Draw();
    for (unsigned int i = 0; i < myPlot.size(); ++i) myPlot[i].Draw("SAME");
  }
  else
  {
    myPlot[0].Draw("PE1");
    for (unsigned int i = 0; i < myPlot.size(); ++i) myPlot[i].Draw("PE1SAME");
  }

  TPaveText *pt;
  pt = new TPaveText(0.15,0.84,0.95,0.89, "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex;
  pt->AddText("#it{#bf{sPHENIX}} Internal, #sqrt{s} = 200 GeV, pp");
  pt->SetBorderSize(0);
  pt->Draw();

  legend->SetFillColor(0);
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->Draw();

  gPad->Modified();

  string extensions[] = {".C", ".pdf", ".png"};
  for (auto extension : extensions)
  {
    string output = plotPath + "/" + plotName + extension;
    c1->SaveAs(output.c_str());
  }
}
bool isInRange(float min, float value, float max)
{
  return min <= value && value <= max;
}

void processData()
{
  string fileName = "/gpfs/mnt/gpfs02/sphenix/user/cdean/software/TrackingAnalysis/Physics_Val_TF/data/macro/evalFile_00079516_withDCA.root";

  TFile *file = new TFile(fileName.c_str());
  TNtuple* data = (TNtuple*)file->Get("ntp_track");

  float track_eta;                  data->SetBranchAddress("eta", &track_eta);
  float track_phi;                  data->SetBranchAddress("phi", &track_phi);
  float track_pT;                   data->SetBranchAddress("pt", &track_pT);
  float track_charge;               data->SetBranchAddress("charge", &track_charge);
  float track_nmaps;                data->SetBranchAddress("nmaps", &track_nmaps);
  float track_dedx;                 data->SetBranchAddress("dedx", &track_dedx);
  float track_expected_proton_dedx; data->SetBranchAddress("prdedx", &track_expected_proton_dedx);
  float track_expected_pion_dedx;   data->SetBranchAddress("pidedx", &track_expected_pion_dedx);
  float track_expected_kaon_dedx;   data->SetBranchAddress("kdedx", &track_expected_kaon_dedx);
  float track_dca3dxy;              data->SetBranchAddress("dca3dxy", &track_dca3dxy);

  int tmp = 0;
  int barWidth = 50;
  uint64_t nEntries = data->GetEntries();

  for (uint64_t i = 0; i < nEntries; i++)
  {
     if (tmp != (int)100*i/nEntries)
    {
      tmp = (int)100*i/nEntries;
      if ((tmp%1)  == 0)
      {
        cout << "[";
        int pos = barWidth * tmp/100;
        for (int k = 0; k < barWidth; ++k)
        {
          if (k < pos) cout << "=";
          else if (k == pos) cout << ">";
          else cout << " ";
        }
        cout << "] " << tmp << " %\r";
        cout.flush();
      }
    }

    data->GetEntry(i);

    if (track_nmaps < 2) continue;
    if (abs(track_dca3dxy) > 0.1) continue;

    bool accept_proton = isInRange(0.4,  track_dedx/track_expected_proton_dedx, 1.8);
    bool accept_pion =   isInRange(0.01, track_dedx/track_expected_pion_dedx,   1.2);
    bool accept_kaon =   isInRange(0.2,  track_dedx/track_expected_kaon_dedx,   1.8);

    if (track_charge > 0) //Positive tracks
    {
      if (accept_proton && track_pT <= min_track_pT_proton)
      { 
        pT_proton_histo.Fill(track_pT);
        eta_proton_histo.Fill(track_eta);
        phi_proton_histo.Fill(track_phi);
      }
      if (accept_pion)
      { 
        pT_piplus_histo.Fill(track_pT);
        eta_piplus_histo.Fill(track_eta);
        phi_piplus_histo.Fill(track_phi);
      }
      if (accept_kaon && track_pT <= min_track_pT_kaon)
      { 
        pT_Kplus_histo.Fill(track_pT);
        eta_Kplus_histo.Fill(track_eta);
        phi_Kplus_histo.Fill(track_phi);
      }
    }
    else
    {
      if (accept_proton && track_pT <= min_track_pT_proton)
      { 
        pT_antiproton_histo.Fill(track_pT);
        eta_antiproton_histo.Fill(track_eta);
        phi_antiproton_histo.Fill(track_phi);
        pT_proton_ratio.Fill(track_pT);
        eta_proton_ratio.Fill(track_eta);
        phi_proton_ratio.Fill(track_phi);
      }
      if (accept_pion)
      { 
        pT_piminus_histo.Fill(track_pT);
        eta_piminus_histo.Fill(track_eta);
        phi_piminus_histo.Fill(track_phi);
        pT_pion_ratio.Fill(track_pT);
        eta_pion_ratio.Fill(track_eta);
        phi_pion_ratio.Fill(track_phi);
      }
      if (accept_kaon && track_pT <= min_track_pT_kaon)
      { 
        pT_Kminus_histo.Fill(track_pT);
        eta_Kminus_histo.Fill(track_eta);
        phi_Kminus_histo.Fill(track_phi);
        pT_Kaon_ratio.Fill(track_pT);
        eta_Kaon_ratio.Fill(track_eta);
        phi_Kaon_ratio.Fill(track_phi);
      }
    }
  }

  cout << "[";
  int pos = barWidth * tmp;
  for (int i = 0; i < barWidth; ++i)
  {
    if (i < pos) cout << "=";
    else if (i == pos) cout << ">";
    else cout << " ";
  }
  cout << "] 100 %\r";
  cout.flush();
  cout<<endl;
}

void charged_hadron_ratios()
{
  processData();

  pT_proton_ratio.Sumw2();
  eta_proton_ratio.Sumw2();
  phi_proton_ratio.Sumw2();

  pT_pion_ratio.Sumw2();
  eta_pion_ratio.Sumw2();
  phi_pion_ratio.Sumw2();

  pT_Kaon_ratio.Sumw2();
  eta_Kaon_ratio.Sumw2();
  phi_Kaon_ratio.Sumw2();

  pT_proton_histo.Sumw2();
  eta_proton_histo.Sumw2();
  phi_proton_histo.Sumw2();

  pT_piplus_histo.Sumw2();
  eta_piplus_histo.Sumw2();
  phi_piplus_histo.Sumw2();

  pT_Kplus_histo.Sumw2();
  eta_Kplus_histo.Sumw2();
  phi_Kplus_histo.Sumw2(); 

  pT_antiproton_histo.Sumw2();
  eta_antiproton_histo.Sumw2();
  phi_antiproton_histo.Sumw2();

  pT_piminus_histo.Sumw2();
  eta_piminus_histo.Sumw2();
  phi_piminus_histo.Sumw2();

  pT_Kminus_histo.Sumw2();
  eta_Kminus_histo.Sumw2();
  phi_Kminus_histo.Sumw2();

  vector<TH1F> pT_proton_histos = {pT_antiproton_histo, pT_proton_histo};
  saveManyPlots(pT_proton_histos, "pT_proton_histos", true, {"#bar{p}", "p"});

  vector<TH1F> pT_pion_histos = {pT_piminus_histo, pT_piplus_histo};
  saveManyPlots(pT_pion_histos, "pT_pion_histos", true, {"#pi^{-}", "#pi^{+}"});

  vector<TH1F> pT_Kaon_histos = {pT_Kminus_histo, pT_Kplus_histo};
  saveManyPlots(pT_Kaon_histos, "pT_Kaon_histos", true, {"K^{-}", "K^{+}"});

  vector<TH1F> eta_proton_histos = {eta_antiproton_histo, eta_proton_histo};
  saveManyPlots(eta_proton_histos, "eta_proton_histos", false, {"#bar{p}", "p"});

  vector<TH1F> eta_pion_histos = {eta_piminus_histo, eta_piplus_histo};
  saveManyPlots(eta_pion_histos, "eta_pion_histos", false, {"#pi^{-}", "#pi^{+}"});

  vector<TH1F> eta_Kaon_histos = {eta_Kminus_histo, eta_Kplus_histo};
  saveManyPlots(eta_Kaon_histos, "eta_Kaon_histos", false, {"K^{-}", "K^{+}"});

  vector<TH1F> phi_proton_histos = {phi_antiproton_histo, phi_proton_histo};
  saveManyPlots(phi_proton_histos, "phi_proton_histos", false, {"#bar{p}", "p"});

  vector<TH1F> phi_pion_histos = {phi_piminus_histo, phi_piplus_histo};
  saveManyPlots(phi_pion_histos, "phi_pion_histos", false, {"#pi^{-}", "#pi^{+}"});

  vector<TH1F> phi_Kaon_histos = {phi_Kminus_histo, phi_Kplus_histo};
  saveManyPlots(phi_Kaon_histos, "phi_Kaon_histos", false, {"K^{-}", "K^{+}"});

  pT_proton_ratio.Divide(&pT_proton_histo);
  eta_proton_ratio.Divide(&eta_proton_histo);
  phi_proton_ratio.Divide(&phi_proton_histo);

  pT_pion_ratio.Divide(&pT_piplus_histo);
  eta_pion_ratio.Divide(&eta_piplus_histo);
  phi_pion_ratio.Divide(&phi_piplus_histo);

  pT_Kaon_ratio.Divide(&pT_Kplus_histo);
  eta_Kaon_ratio.Divide(&eta_Kplus_histo);
  phi_Kaon_ratio.Divide(&phi_Kplus_histo);

  savePlots(pT_proton_ratio, "pT_proton_ratio");
  savePlots(eta_proton_ratio, "eta_proton_ratio");
  savePlots(phi_proton_ratio, "phi_proton_ratio");

  savePlots(pT_pion_ratio, "pT_pion_ratio");
  savePlots(eta_pion_ratio, "eta_pion_ratio");
  savePlots(phi_pion_ratio, "phi_pion_ratio");

  savePlots(pT_Kaon_ratio, "pT_Kaon_ratio");
  savePlots(eta_Kaon_ratio, "eta_Kaon_ratio");
  savePlots(phi_Kaon_ratio, "phi_Kaon_ratio");
}
