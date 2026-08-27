#include <phool/PHRandomSeed.h>
#include <gsl/gsl_rng.h>
#include <algorithm>

using namespace std;

/*
 * This creates the output directory for the plots
 */
std::string plotPath = "./Kshort_daughter_ratios";
string makeDirectory = "mkdir -p " + plotPath;
system(makeDirectory.c_str());

/*
 * This creates an nth-order polynomial for fitting the ratio to
 * Change the order for higher or lower order fits
 */
const unsigned int polynomial_order = 5;
TF1* ratio_function;
Double_t fitfunc(Double_t *x, Double_t *par)
{
  double value = 0;
  for (unsigned int i = 0; i <= polynomial_order; ++i) value += par[i]*pow(x[0], i);

  return value;
}

/*
 * Truncates numbers to better visualisation on plots
 */
template <typename T>
string to_string_with_precision(const T a_value, const int n = 1)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

/*
 * Sets all the canvas stuff and saves the plots with certain extensions
 * just add more extensions to the list
 */
template <typename T>
void savePlots(T myPlot, string plotName, bool logY = false)
{
  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  myPlot.SetMinimum(0);
  myPlot.SetMaximum(2.8);

  if (strncmp(typeid(myPlot).name(), "4TH2F", 5) == 0)
  {
    myPlot.Draw("COLZ");
  }
  else
  {
    myPlot.Sumw2();
    if (logY) gPad->SetLogy();
    myPlot.Draw("PE1");
    ratio_function->Draw("SAME");
  }

  TPaveText *pt;
  pt = new TPaveText(0.15,0.9,0.95,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex = pt->AddText("#it{#bf{sPHENIX}} Internal, #sqrt{s} = 200 GeV, pp");
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  string extensions[] = {".C", ".pdf", ".png"};
  for (auto extension : extensions)
  {
    string output = plotPath + "/" + plotName + extension;
    c1->SaveAs(output.c_str());
  }
}

/*
 * Makes a historgram template with variable bin widths as defined below
 */
float lower_bin_bounds[] = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.4, 1.8, 2.2, 3, 4};
const unsigned int n_variable_bins = sizeof(lower_bin_bounds)/sizeof(lower_bin_bounds[0]) - 1; 

TH1F makeHisto(int nBins, float* min, string xAxisTitle, string unit, int precision, string yAxisTitle = "Raw Ratio")
{
  TH1F myHisto(xAxisTitle.c_str(), xAxisTitle.c_str(), nBins, min);

  if (unit != "") xAxisTitle += " [" + unit +  "]";
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());

  return myHisto;
}

/*
 * Makes a historgram template with fixed bin widths and ranges defines below
 */
float min_pT = 0.2;
float max_pT = 3.2;
int nBins = 30;

TH1F makeHisto(int nBins, float min, float max, string xAxisTitle, string unit, int precision)
{
  TH1F myHisto(xAxisTitle.c_str(), xAxisTitle.c_str(), nBins, min, max);

  if (unit != "") xAxisTitle += " [" + unit +  "]";
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());

  float binWidth = (float) (max - min)/nBins;
  string yAxisTitle;
  yAxisTitle = "#pi^{-}/#pi^{+} ratio from K_{S}^{0} / " + to_string_with_precision(binWidth) + " " + unit;
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());

  return myHisto;
}

void pion_ratio_from_Kshort()
{
  //Input file should be public
  string fileName = "/gpfs/mnt/gpfs02/sphenix/user/cdean/scripts/fitters/files/KShort6RunCombined.root";
  TFile *file = new TFile(fileName.c_str());
  TTree* data = (TTree*)file->Get("DecayTree");

  float piminus_pT; data->SetBranchAddress("track_1_pT", &piminus_pT);
  float piplus_pT; data->SetBranchAddress("track_2_pT", &piplus_pT);
  float K_S0_mass; data->SetBranchAddress("K_S0_mass", &K_S0_mass);

  TH1F piminus_pT_histo = makeHisto(nBins, min_pT, max_pT, "p_{T}", "GeV", 1);
  TH1F piplus_pT_histo = makeHisto(nBins, min_pT, max_pT, "p_{T}", "GeV", 1);

  int tmp = 0;
  int barWidth = 50;
  int num_entries = data->GetEntries();
  for (int  l = 0; l < num_entries; ++l)
  {
    if (tmp != (int)100*l/num_entries)
    {
      tmp = (int)100*l/num_entries;
      if ((tmp%1)  == 0)
      {
        cout << "[";
        int pos = barWidth * tmp/100;
        for (int i = 0; i < barWidth; ++i)
        {
          if (i < pos) cout << "=";
          else if (i == pos) cout << ">";
          else cout << " ";
        }
        cout << "] " << tmp << " %\r";
        cout.flush();
      }
    }

    data->GetEntry(l);

    if (0.47 <= K_S0_mass && K_S0_mass <= 0.52)
    {
      piminus_pT_histo.Fill(piminus_pT);
      piplus_pT_histo.Fill(piplus_pT);
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

  piminus_pT_histo.Sumw2();
  piplus_pT_histo.Sumw2();
  piminus_pT_histo.Divide(&piplus_pT_histo);

  ratio_function = new TF1("fit", fitfunc, min_pT, max_pT, polynomial_order);
  ratio_function->SetLineColor(kRed);
  TFitResultPtr r = piminus_pT_histo.Fit(ratio_function, "RS");

  savePlots(piminus_pT_histo, "Kshort_data_piminus_pT_to_piplus_pT");

  file->Close();
}
