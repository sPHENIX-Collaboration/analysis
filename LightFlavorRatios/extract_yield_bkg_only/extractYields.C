float lower_bin_bounds[] = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.8, 2.1, 2.4, 2.7, 3, 4};
const unsigned int n_variable_bins = sizeof(lower_bin_bounds)/sizeof(lower_bin_bounds[0]) - 1; 

string plotPath = "plots_with_track_PT_cut/";

const unsigned int Kshort_polynomial_order = 3;
const unsigned int Lambda_polynomial_order = 3;

struct fit_ranges
{
  float min;
  float ignore_min;
  float ignore_max;
  float max;
};

fit_ranges Kshort_ranges{0.4, 0.47, 0.535, 0.6};
fit_ranges Lambda_ranges{1.09, 1.105, 1.125, 1.1425};

float Kshort_xVals[n_variable_bins], Kshort_xErrs[n_variable_bins], Kshort_yVals[n_variable_bins], Kshort_yErrs[n_variable_bins];
float Lambda_xVals[n_variable_bins], Lambda_xErrs[n_variable_bins], Lambda_yVals[n_variable_bins], Lambda_yErrs[n_variable_bins];

TF1 *fitFunc, *fitFunc_fullBkg;

template <typename T>
string to_string_with_precision(const T a_value, const int n = 0)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

class fitVals
{
public:
  string pTmin;
  string pTmax;
  string yield;
  string error;
  
  fitVals(float min, float max, float fitYield, float fitErr)
  {
    pTmin = to_string_with_precision(min, 1);
    pTmax = to_string_with_precision(max, 1);
    yield = to_string_with_precision(fitYield, 0);
    error = to_string_with_precision(fitErr, 0);
  };
  ~fitVals();

};

//Initialise histograms
TH1F makeHisto(int nBins, float min, float max, string type, string xAxisTitle, string unit, int precision)
{
  string histo_name = type;
  //string histo_name = type + "_" + xAxisTitle;
  TH1F myHisto(histo_name.c_str(), histo_name.c_str(), nBins, min, max);
  
  if (unit != "") xAxisTitle += " [" + unit +  "]";  
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());

  float binWidth = (float) (max - min)/nBins;
  string yAxisTitle = "Candidates";
  if (unit != "") yAxisTitle += " / " + to_string_with_precision(binWidth, precision) + " " + unit;
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());
  
  return myHisto;
}

Double_t fitfunc_Kshort(Double_t *x, Double_t *par)
{
  if (x[0] > Kshort_ranges.ignore_min && x[0] < Kshort_ranges.ignore_max) 
  {
    TF1::RejectPoint();
    return 0;
  }

  double value = 0;
  for (unsigned int i = 0; i <= Kshort_polynomial_order; ++i) value += par[i]*pow(x[0], i);

  return value;
}

Double_t fitfunc_Lambda(Double_t *x, Double_t *par)
{
  if (x[0] > Lambda_ranges.ignore_min && x[0] < Lambda_ranges.ignore_max)
  {
    TF1::RejectPoint();
    return 0;
  }

  double value = 0;
  for (unsigned int i = 0; i <= Lambda_polynomial_order; ++i) value += par[i]*pow(x[0], i);

  return value;
}

Double_t fitfunc_noGap(Double_t *x, Double_t *par)
{
  double value = 0;
  for (unsigned int i = 0; i <= Kshort_polynomial_order; ++i) value += par[i]*pow(x[0], i);

  return value;
}

Double_t fitfunc_Lambda_noGap(Double_t *x, Double_t *par)
{
  double value = 0;
  for (unsigned int i = 0; i <= Lambda_polynomial_order; ++i) value += par[i]*pow(x[0], i);

  return value;
}

template <typename T>
void savePlots(T myPlot, string plotName, float xMin, float xMax, float yield = 0, float error = 0)
{
  TGaxis::SetMaxDigits(3);
  string makeDirectory = "mkdir -p " + plotPath;
  system(makeDirectory.c_str());

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  myPlot.GetXaxis()->SetNdivisions(505);
  myPlot.GetYaxis()->SetRangeUser(0, myPlot.GetMaximum()*1.4);
  myPlot.Sumw2();
  myPlot.Draw("PE1");
  fitFunc->Draw("SAME");
  fitFunc_fullBkg->Draw("SAME");

  TPaveText *pt;
  pt = new TPaveText(0.15,0.74,0.95,0.89, "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  TText *pt_LaTex;
  pt->AddText("#it{#bf{sPHENIX}} Internal, #sqrt{s} = 200 GeV, pp");
  string range = to_string_with_precision(xMin, 1) + " #leq p_{T} [GeV] < " + to_string_with_precision(xMax, 1);
  string result = "Yield = " + to_string_with_precision(yield, 0) + " #pm " + to_string_with_precision(error, 0);
  pt->AddText(range.c_str());
  pt->AddText(result.c_str());
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  string extensions[] = {".C", ".pdf", ".png", ".root"};
  for (auto extension : extensions)
  {
    string output = plotPath + plotName + extension;
    c1->SaveAs(output.c_str());
  }
}

void processData(string type = "Kshort2pipi")
{
  bool processingKshort = type == "Kshort2pipi" ? true : false;

  float xVals[n_variable_bins], xErrs[n_variable_bins], yVals[n_variable_bins], yErrs[n_variable_bins];

  string dir = "/sphenix/tg/tg01/hf/aopatton/SVLooseJun4/";
  string fileName = processingKshort ? dir + "6RunsCombinedKShortSVLoose.root"
                                     : dir + "6RunsCombinedLambdaSVLoose.root";

  TFile *file = new TFile(fileName.c_str());
  TTree* data = (TTree*)file->Get("DecayTree");
  string massBranch = processingKshort ? "K_S0_mass" : "Lambda0_mass";
  string pTBranch = processingKshort ? "K_S0_pT" : "Lambda0_pT";

  string mass_string = processingKshort ? "m_{#pi#pi}" : "m_{p#pi}";

  float mass_min = processingKshort ? Kshort_ranges.min : Lambda_ranges.min;
  float mass_max = processingKshort ? Kshort_ranges.max : Lambda_ranges.max;

  for (int i = 0; i < n_variable_bins; ++i)
  {
    float min = lower_bin_bounds[i];
    float max = lower_bin_bounds[i+1];
    float signal_yield = 0;
    float signal_error = 0;

    string title = type + "_pT_range_" + to_string_with_precision(min,1) + "_to_" + to_string_with_precision(max,1);
    TH1F binnedMass = makeHisto(50, mass_min, mass_max, title.c_str(), mass_string.c_str(), "GeV", 3);
    string fillString = massBranch + " >> " + title;
    string cutString = to_string_with_precision(min,1) + " <= " + pTBranch + " && " + pTBranch + " < " + to_string_with_precision(max,1) + " && min(track_1_pT, track_2_pT) >= 0.2";
    data->Draw(fillString.c_str(), cutString.c_str());

    if (processingKshort) fitFunc = new TF1("fit", fitfunc_Kshort, mass_min, mass_max, Kshort_polynomial_order);
    else fitFunc = new TF1("fit", fitfunc_Lambda, mass_min, mass_max, Lambda_polynomial_order);
    fitFunc->SetLineColor(kRed);
    TFitResultPtr r = binnedMass.Fit(fitFunc, "RS");

    //Need to account for the region over the signal
    if (processingKshort) fitFunc_fullBkg = new TF1("fit", fitfunc_noGap, mass_min, mass_max, Kshort_polynomial_order);
    else fitFunc_fullBkg = new TF1("fit", fitfunc_Lambda_noGap, mass_min, mass_max, Lambda_polynomial_order);
    fitFunc_fullBkg->SetLineColor(kBlue);
    for (int j = 0; j < fitFunc_fullBkg->GetNpar(); ++j) fitFunc_fullBkg->SetParameter(j, fitFunc->GetParameter(j));

    float bkg_area = fitFunc_fullBkg->Integral(mass_min, mass_max);
    float bkg_areaErr = fitFunc->IntegralError(mass_min, mass_max, r->GetParams(), r->GetCovarianceMatrix().GetMatrixArray());

    float binWidth = binnedMass.GetBinWidth(1);
    float bkg_yield = bkg_area / binWidth;
    float bkg_yieldErr = bkg_areaErr / binWidth;

    signal_yield = (float) binnedMass.GetEntries() - bkg_yield;
    signal_error = signal_yield*(bkg_yieldErr/bkg_yield);

    cout << "Number of entries in histogram = " << (float) binnedMass.GetEntries() << endl;
    cout << "Background yield from total integral = " << bkg_yield << " +/- " << bkg_yieldErr << endl;
    cout << "Signal yield = " << signal_yield << " +/- " << signal_error << endl;

    savePlots(binnedMass, title.c_str(), min, max, signal_yield, signal_error);

    if (processingKshort)
    {
      Kshort_xVals[i] = xVals[i] = (max + min)/2;
      Kshort_xErrs[i] = xErrs[i] = (max - min)/2;
      Kshort_yVals[i] = yVals[i] = signal_yield;
      Kshort_yErrs[i] = yErrs[i] = signal_error;
    }
    else
    {
      Lambda_xVals[i] = xVals[i] = (max + min)/2;
      Lambda_xErrs[i] = xErrs[i] = (max - min)/2;
      Lambda_yVals[i] = yVals[i] = signal_yield;
      Lambda_yErrs[i] = yErrs[i] = signal_error;
    }
  }
  
   TCanvas *c1 = new TCanvas("c1","A Simple Graph",800,800);

   TGraphErrors *gr = new TGraphErrors(n_variable_bins,xVals,yVals,xErrs,yErrs);
   string graph_x_axis_title = processingKshort ? "K^{0}_{S} p_{T} [GeV]" : "#Lambda^{0} p_{T} [GeV]";
   gr->GetXaxis()->SetTitle(graph_x_axis_title.c_str());
   gr->GetYaxis()->SetTitle("Yield");
   gr->SetMarkerSize(2);
   gr->Draw("A*");

  string extensions[] = {".C", ".pdf", ".png", ".root"};
  for (auto extension : extensions)
  {
    string output = plotPath + type + "_yield" + extension;
    c1->SaveAs(output.c_str());
  }
}

void extractYields()
{
  processData();
  processData("Lambda02ppi");

  TCanvas *c1 = new TCanvas("c1","A Simple Graph",800,800);

  float xVals[n_variable_bins], xErrs[n_variable_bins], yVals[n_variable_bins], yErrs[n_variable_bins];

  for (int i = 0; i < n_variable_bins; ++i)
  {
    xVals[i] = Kshort_xVals[i];
    xErrs[i] = Kshort_xErrs[i];

    float rawRatio = Lambda_yVals[i]/(2*Kshort_yVals[i]);
    float rawError = rawRatio*(TMath::Sqrt(TMath::Power(Lambda_yErrs[i]/Lambda_yVals[i], 2) + TMath::Power(Kshort_yErrs[i]/Kshort_yVals[i], 2)));
    yVals[i] = rawRatio;
    yErrs[i] = rawError;

    cout << "pT bin mean: " << xVals[i] << ", Lambda yield = " << Lambda_yVals[i] << ", Kshort yield = " << Kshort_yVals[i] << ", raw ratio = " << yVals[i] << " +/- " << yErrs[i] << endl;
  }

  TGraphErrors *gr = new TGraphErrors(n_variable_bins,xVals,yVals,xErrs,yErrs);
  gr->GetXaxis()->SetTitle("p_{T[GeV]}");
  gr->GetYaxis()->SetTitle("#Lambda^{0}/2K^{0}_{S} Raw Ratio");
  gr->GetXaxis()->SetRangeUser(0.5, 4);
  gr->GetYaxis()->SetRangeUser(0, 0.415);
  gr->SetMarkerSize(2);
  gr->Draw("A*");

  string extensions[] = {".C", ".pdf", ".png", ".root"};
  for (auto extension : extensions)
  {
    string output = plotPath + "RawRatio" + extension;
    c1->SaveAs(output.c_str());
  }
}
