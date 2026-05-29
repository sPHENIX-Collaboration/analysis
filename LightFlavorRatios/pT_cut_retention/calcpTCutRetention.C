#include <any>
#include <cmath>

using namespace std;

bool saveFinalPlots = true;
bool printMarkdownTables = true;
bool printLatexTables = true;
bool printArrays = true;

bool global_isSim = true;

//Custom binning scheme for LF analysis
float lower_bin_bounds[] = {0.5, 0.8, 1.1, 1.4, 1.8, 2.2, 3, 4};
const unsigned int n_variable_bins = sizeof(lower_bin_bounds)/sizeof(lower_bin_bounds[0]) - 1; 

template <typename T>
string to_string_with_precision(const T a_value, const int n = 0)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

//Initialise histograms
TH1F makeHisto(int nBins, float min, float max, string type, string xAxisTitle, string unit, int precision, string yAxisTitle = "p_{T} Accept.")
{
  string histo_name = type + "_" + xAxisTitle;
  TH1F myHisto(histo_name.c_str(), histo_name.c_str(), nBins, min, max);
  
  if (unit != "") xAxisTitle += " [" + unit +  "]";  
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());

  float binWidth = (float) (max - min)/nBins;
  if (unit != "") yAxisTitle += " / " + to_string_with_precision(binWidth, precision) + " " + unit;
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());
  
  return myHisto;
}

TH1F makeHisto(int nBins, float* min, string type, string xAxisTitle, string unit, int precision, string yAxisTitle = "p_{T} Accept.")
{
  string histo_name = type + "_" + xAxisTitle;
  TH1F myHisto(histo_name.c_str(), histo_name.c_str(), nBins, min);

  if (unit != "") xAxisTitle += " [" + unit +  "]";
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());

  return myHisto;
}

//What branches to read into the analysis
struct variableMap
{
  float floats[21];
  double doubles[1];
  unsigned int ints[10];

  //any and variant are a pain here with TBranch 
  map<string, float> float_vars{
    {"Lambda0_mass", floats[0]},
    {"Lambda0_DIRA", floats[1]},
    {"Lambda0_chi2", floats[2]},
    {"K_S0_mass", floats[3]},
    {"K_S0_DIRA", floats[4]},
    {"K_S0_chi2", floats[5]},
    {"track_1_chi2", floats[6]},
    {"track_1_IP_xy", floats[7]},
    {"track_2_chi2", floats[8]},
    {"track_2_IP_xy", floats[9]},
    {"track_1_track_2_DCA", floats[10]},
    {"track_1_track_2_DCA_xy", floats[11]},
    {"track_1_pT", floats[12]},
    {"track_2_pT", floats[13]},
    {"Lambda0_pT", floats[13]},
    {"Lambda0_pseudorapidity", floats[14]},
    {"Lambda0_rapidity", floats[15]},
    {"Lambda0_phi", floats[16]},
    {"K_S0_pT", floats[17]},
    {"K_S0_pseudorapidity", floats[18]},
    {"K_S0_rapidity", floats[19]},
    {"K_S0_phi", floats[20]},
  };
    
  map<string, unsigned int> int_vars{
    {"Lambda0_nDoF", ints[0]},
    {"K_S0_nDoF", ints[1]},
    {"track_1_nDoF", ints[2]},
    {"track_2_nDoF", ints[3]},
    {"track_1_MVTX_nStates", ints[4]},
    {"track_1_INTT_nStates", ints[5]},
    {"track_1_TPC_nStates", ints[6]},
    {"track_2_MVTX_nStates", ints[7]},
    {"track_2_INTT_nStates", ints[8]},
    {"track_2_TPC_nStates", ints[9]},
  };

  map<string, double> double_vars{
    {"sWeight", doubles[0]},
  };
};

//What histograms to build for each ratio check
class histos
{
  public:
    TH1F Lambda0_all;
    TH1F K_S0_all;
    TH1F Lambda0_pT_accept;
    TH1F K_S0_pT_accept;
    TH1F ratio;
    TH1F inv_ratio;
    bool variable_bins = false;
    int nBins = 15;

  histos(bool constructor_variable_bins = false, float range = 1., string variable = "", string unit = "")
  {
    variable_bins = constructor_variable_bins;
    if (constructor_variable_bins)
    {
      Lambda0_all       = makeHisto(n_variable_bins, lower_bin_bounds, "Lambda0_all", variable.c_str(), unit.c_str(), 1);
      K_S0_all          = makeHisto(n_variable_bins, lower_bin_bounds, "K_S0_all", variable.c_str(), unit.c_str(), 1);
      Lambda0_pT_accept = makeHisto(n_variable_bins, lower_bin_bounds, "Lambda0_pT_accept", variable.c_str(), unit.c_str(), 1);
      K_S0_pT_accept    = makeHisto(n_variable_bins, lower_bin_bounds, "K_S0_pT_accept", variable.c_str(), unit.c_str(), 1);
      ratio             = makeHisto(n_variable_bins, lower_bin_bounds, "ratio", variable.c_str(), unit.c_str(), 1, "#Lambda^{0}/K_{S}^{0} p_{T} Accept.");
      inv_ratio         = makeHisto(n_variable_bins, lower_bin_bounds, "inv_ratio", variable.c_str(), unit.c_str(), 1, "#Lambda^{0}/K_{S}^{0} p_{T} Accept.");
    }
    else
    {
      Lambda0_all       = makeHisto(nBins, -1*range, range, "Lambda0_all", variable.c_str(), unit.c_str(), 1);
      K_S0_all          = makeHisto(nBins, -1*range, range, "K_S0_all", variable.c_str(), unit.c_str(), 1);
      Lambda0_pT_accept = makeHisto(nBins, -1*range, range, "Lambda0_pT_accept", variable.c_str(), unit.c_str(), 1);
      K_S0_pT_accept    = makeHisto(nBins, -1*range, range, "K_S0_pT_accept", variable.c_str(), unit.c_str(), 1);
      ratio             = makeHisto(nBins, -1*range, range, "ratio", variable.c_str(), unit.c_str(), 1, "#Lambda^{0}/K_{S}^{0} p_{T} Accept.");
      inv_ratio         = makeHisto(nBins, -1*range, range, "inv_ratio", variable.c_str(), unit.c_str(), 1, "#Lambda^{0}/K_{S}^{0} p_{T} Accept.");
    }
  }
};

histos pT(true, 0, "pT", "GeV");
histos eta(false, 1.1, "#eta", "");
histos phi(false, M_PI, "#phi", "");
histos rap(false, 1.0, "y", "");

template <typename T>
void savePlots(T myPlot, string plotName, bool logY = false, float yMin = 0, float yMax = 1)
{
  TGaxis::SetMaxDigits(3);
  string plotPath = "plots/";
  string makeDirectory = "mkdir -p " + plotPath;
  system(makeDirectory.c_str());

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  myPlot.GetYaxis()->SetRangeUser(yMin, yMax);

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
  pt = new TPaveText(0.15,0.9,0.95,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex;
  if (global_isSim) pt->AddText("#it{#bf{sPHENIX}} Simulation, #sqrt{s} = 200 GeV, pp");
  else pt->AddText("#it{#bf{sPHENIX}} Internal, #sqrt{s} = 200 GeV, pp");
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  string simAddition = global_isSim ? "_sim" : "";

  string extensions[] = {".C", ".pdf", ".png", ".root"};
  for (auto extension : extensions)
  {
    string output = plotPath + plotName + simAddition + extension;
    c1->SaveAs(output.c_str());
  }
}

bool isInRange(float min, float value, float max)
{
  return min <= value && value <= max;
}

bool isAccepted(variableMap inputMap, bool isKshort = false)
{
  if (isKshort)
  {
    if (!isInRange(0.4, inputMap.float_vars["K_S0_mass"], 0.6)) return false;
    if (inputMap.float_vars["K_S0_DIRA"] < 0.99) return false;
    if (inputMap.float_vars["K_S0_chi2"]/inputMap.int_vars["K_S0_nDoF"] > 20) return false;
  }
  else
  {
    if (!isInRange(1.08, inputMap.float_vars["Lambda0_mass"], 1.15)) return false;
    if (inputMap.float_vars["Lambda0_DIRA"] < 0.99) return false;
    if (inputMap.float_vars["Lambda0_chi2"]/inputMap.int_vars["Lambda0_nDoF"] > 20) return false;
  }

  if (inputMap.int_vars["track_1_MVTX_nStates"] < 1) return false;
  if (inputMap.int_vars["track_1_INTT_nStates"] < 1) return false;
  if (inputMap.int_vars["track_1_TPC_nStates"] < 20) return false;
  if (inputMap.int_vars["track_2_MVTX_nStates"] < 1) return false;
  if (inputMap.int_vars["track_2_INTT_nStates"] < 1) return false;
  if (inputMap.int_vars["track_2_TPC_nStates"] < 20) return false;

  if (inputMap.float_vars["track_1_chi2"]/inputMap.int_vars["track_1_nDoF"] > 300) return false;
  if (inputMap.float_vars["track_2_chi2"]/inputMap.int_vars["track_2_nDoF"] > 300) return false;

  if (abs(inputMap.float_vars["track_1_IP_xy"]) < 0.05) return false;
  if (abs(inputMap.float_vars["track_2_IP_xy"]) < 0.05) return false;

  if (abs(inputMap.float_vars["track_1_track_2_DCA"]) > 0.5) return false;
  if (abs(inputMap.float_vars["track_1_track_2_DCA_xy"]) > 1) return false;

  return true;
}

void processData(string type = "Kshort2pipi")
{
  bool processingKshort = type == "Kshort2pipi" ? true : false;

  string dir = global_isSim ? "/sphenix/user/cdean/software/analysis/LightFlavorRatios/geometric_acceptance/simulation/"
                            : "/sphenix/user/cdean/scripts/fitters/files/";
  string fileName = processingKshort ? dir + "KShort6RunCombined_weighted.root"
                                          : dir + "Lambda6RunCombined_weighted.root";
  if (global_isSim)
  {
    fileName = processingKshort ? dir + "outputKFParticle_Kshort_reco.root"
                                    : dir + "outputKFParticle_Lambda_reco.root";
  }

  TFile *file = new TFile(fileName.c_str());
  TTree* data = (TTree*)file->Get("DecayTree");

  variableMap inputMap;
  for (auto &[branch, var] : inputMap.float_vars) data->SetBranchAddress(branch.c_str(), &var);
  for (auto &[branch, var] : inputMap.int_vars)   data->SetBranchAddress(branch.c_str(), &var);
  if (!global_isSim) for (auto &[branch, var] : inputMap.double_vars) data->SetBranchAddress(branch.c_str(), &var);

  int tmp = 0;
  int barWidth = 50;

  //Acceptance
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

    double weight = global_isSim ? 1 : inputMap.double_vars["sWeight"];

    if (global_isSim)
    {
      bool checkCandidate = isAccepted(inputMap, processingKshort);
      if (!checkCandidate) continue;
    }
 
    if (processingKshort)
    {
      pT.K_S0_all.Fill(inputMap.float_vars["K_S0_pT"], weight);
      eta.K_S0_all.Fill(inputMap.float_vars["K_S0_pseudorapidity"], weight);
      rap.K_S0_all.Fill(inputMap.float_vars["K_S0_rapidity"], weight);
      phi.K_S0_all.Fill(inputMap.float_vars["K_S0_phi"], weight);
    }
    else
    {
      pT.Lambda0_all.Fill(inputMap.float_vars["Lambda0_pT"], weight);
      eta.Lambda0_all.Fill(inputMap.float_vars["Lambda0_pseudorapidity"], weight);
      rap.Lambda0_all.Fill(inputMap.float_vars["Lambda0_rapidity"], weight);
      phi.Lambda0_all.Fill(inputMap.float_vars["Lambda0_phi"], weight);
    }

    bool accepted = min(inputMap.float_vars["track_1_pT"], inputMap.float_vars["track_2_pT"]) >= 0.25 ? true : false;

    if (accepted)
    {
      if (processingKshort)
      {
        pT.K_S0_pT_accept.Fill(inputMap.float_vars["K_S0_pT"], weight);
        eta.K_S0_pT_accept.Fill(inputMap.float_vars["K_S0_pseudorapidity"], weight);
        rap.K_S0_pT_accept.Fill(inputMap.float_vars["K_S0_rapidity"], weight);
        phi.K_S0_pT_accept.Fill(inputMap.float_vars["K_S0_phi"], weight);
      }
      else
      {
        pT.Lambda0_pT_accept.Fill(inputMap.float_vars["Lambda0_pT"], weight);
        eta.Lambda0_pT_accept.Fill(inputMap.float_vars["Lambda0_pseudorapidity"], weight);
        rap.Lambda0_pT_accept.Fill(inputMap.float_vars["Lambda0_rapidity"], weight);
        phi.Lambda0_pT_accept.Fill(inputMap.float_vars["Lambda0_phi"], weight);
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

void makeRatios(histos &histoSet, string trailer)
{
  string K_S0_saveName = "KS0_geometric_acceptance_ratio_" + trailer;
  string Lambda0_saveName = "Lambda0_geometric_acceptance_ratio_" + trailer;
  string ratio_saveName = "Lambda0_to_KS0_geometric_acceptance_ratio_" + trailer;
  string inv_ratio_saveName = "K_S0_to_Lambda0_geometric_acceptance_ratio_" + trailer;

  histoSet.Lambda0_all.Sumw2();
  histoSet.K_S0_all.Sumw2();

  histoSet.Lambda0_pT_accept.Sumw2();
  histoSet.K_S0_pT_accept.Sumw2();

  histoSet.Lambda0_pT_accept.Divide(&histoSet.Lambda0_all);
  histoSet.K_S0_pT_accept.Divide(&histoSet.K_S0_all);

  histoSet.ratio = histoSet.Lambda0_pT_accept;
  histoSet.inv_ratio = histoSet.K_S0_pT_accept;

  histoSet.ratio.Divide(&histoSet.K_S0_pT_accept);
  histoSet.inv_ratio.Divide(&histoSet.Lambda0_pT_accept);

  if (saveFinalPlots)
  {
    savePlots(histoSet.K_S0_pT_accept, K_S0_saveName.c_str(), false, 0, 1.1);
    savePlots(histoSet.Lambda0_pT_accept, Lambda0_saveName.c_str(), false, 0, 1.1);

    savePlots(histoSet.ratio, ratio_saveName.c_str(), false, 0, 1.1);
    savePlots(histoSet.inv_ratio, inv_ratio_saveName.c_str(), false, 0, 1.1);
  }
}

void makeTable(string var, histos theseHistos, string type = "Markdown")
{
  cout << endl;
 
  string startline = type == "Markdown" ? "| " : "";
  string separator = type == "Markdown" ? " | " : " & ";
  string endline  = type == "Markdown" ? " | " : " \\\\ ";

  int nBins = theseHistos.variable_bins ? n_variable_bins : theseHistos.nBins;

  if (type != "Markdown") cout << "\\begin{tabular}{@{}ccccc@{}}" << endl << "\\toprule[1pt]" << endl;
  
  cout << startline << var << separator << "$K_S^0$" << separator << "$\\Lambda^0$" << separator << "$\\Lambda^0 / K_S^0$" << separator << "$K_S^0 / \\Lambda^0$" << endline << endl;

  if (type == "Markdown") cout << "|:--:|:--:|:--:|:--:|:--:|" << endl;
  else cout << "\\midrule[0.2pt]" << endl;

  for (int i = 1; i <= nBins; ++i)
  {
    string low = to_string_with_precision(theseHistos.ratio.GetXaxis()->GetBinLowEdge(i), 2);
    string high = to_string_with_precision(theseHistos.ratio.GetXaxis()->GetBinUpEdge(i), 2);

    string Ks0_content = to_string_with_precision(theseHistos.K_S0_pT_accept.GetBinContent(i), 4);
    string Ks0_error = to_string_with_precision(theseHistos.K_S0_pT_accept.GetBinError(i), 4);

    string Lambda_content = to_string_with_precision(theseHistos.Lambda0_pT_accept.GetBinContent(i), 4);
    string Lambda_error = to_string_with_precision(theseHistos.Lambda0_pT_accept.GetBinError(i), 4);

    string content = to_string_with_precision(theseHistos.ratio.GetBinContent(i), 2);
    string error = to_string_with_precision(theseHistos.ratio.GetBinError(i), 2);

    string inv_content = to_string_with_precision(theseHistos.inv_ratio.GetBinContent(i), 2);
    string inv_error = to_string_with_precision(theseHistos.inv_ratio.GetBinError(i), 2);
    cout << startline << low << " $\\rightarrow$ " << high << separator << Ks0_content << " $\\pm$ " << Ks0_error << separator << Lambda_content << " $\\pm$ " << Lambda_error << separator << content << " $\\pm$ " << error << separator << inv_content << " $\\pm$ " << inv_error << endline << endl;
  }

  if (type != "Markdown") cout << "\\bottomrule[1pt]" << endl << "\\end{tabular}" << endl;
}

void makeArray(string type, histos theseHistos)
{
  cout << "Printing arrays for type " << type << endl;
  int nBins = theseHistos.variable_bins ? n_variable_bins : theseHistos.nBins;
  float xVal[nBins], xErr[nBins], yVal[nBins], yErr[nBins];

  for (int i = 0; i <= nBins; ++i)
  {
    xVal[i] = theseHistos.inv_ratio.GetXaxis()->GetBinCenter(i+1);
    xErr[i] = xVal[i] - theseHistos.inv_ratio.GetXaxis()->GetBinLowEdge(i+1);
    yVal[i] = theseHistos.inv_ratio.GetBinContent(i+1);
    yErr[i] = theseHistos.inv_ratio.GetBinError(i+1);
  }

  cout << "  float x[] = {";
  for (auto &val : xVal) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "  float ex[] = {";
  for (auto &val : xErr) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "  float y[] = {";
  for (auto &val : yVal) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "  float ey[] = {";
  for (auto &val : yErr) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "const int n = sizeof(x)/sizeof(x[0]);" << endl;
}

void calcpTCutRetention(bool isSim = true)
{
  global_isSim = isSim;

  cout << "Processing K-short data" << endl;
  processData();
  cout << "Processing Lambda0 data" << endl;
  processData("Lambda02ppi");

  makeRatios(pT, "pT");
  makeRatios(eta, "eta");
  makeRatios(phi, "phi");
  makeRatios(rap, "rap");

  if (printMarkdownTables)
  {
    makeTable("$p_{T}$ [GeV]", pT, "Markdown");
    //makeTable("$\\eta$", eta, "Markdown");
    //makeTable("$y$", rap, "Markdown");
    //makeTable("$\\phi$", phi, "Markdown");
  }

  if (printLatexTables)
  {
    makeTable("$p_{T}$ [GeV]", pT, "LaTex");
    makeTable("$\\eta$", eta, "LaTex");
    makeTable("$y$", rap, "LaTex");
    makeTable("$\\phi$", phi, "LaTex");
  }

  if (printArrays)
  {
    makeArray("pT", pT);
  }
}
