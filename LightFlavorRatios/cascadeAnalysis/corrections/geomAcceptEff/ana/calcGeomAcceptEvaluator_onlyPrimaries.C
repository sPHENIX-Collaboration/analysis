#include <any>
#include <cmath>

using namespace std;
using namespace ROOT::Math;

bool saveFinalPlots = true;
bool printMarkdownTables = true;
bool printLatexTables = true;
bool printArrays = true;

string plotPath = "plots_evaluator/";
string makeDirectory = "mkdir -p " + plotPath;

//Custom binning scheme for LF analysis
//Camerons
//float lower_bin_bounds[] = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.4, 1.8, 2.2, 3, 4};
//Tonys
//float lower_bin_bounds[] = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.2, 1.5, 2, 3, 4};
//const unsigned int n_variable_bins = sizeof(lower_bin_bounds)/sizeof(lower_bin_bounds[0]) - 1; 
//Thomass
/*
vector<float> lower_bin_bounds_pT = {0.75, 0.909375, 1.05792, 1.23212, 1.46559, 2.0};
const unsigned int n_variable_bins_pT = 5;
vector<float> lower_bin_bounds_eta = {-1.1,-0.916345,-0.755196,-0.585922,-0.390321,-0.159045,0.122585,0.394623,0.637892,0.849511,1.1};
const unsigned int n_variable_bins_eta = 10;
vector<float> lower_bin_bounds_phi = {-M_PI,-2.51177,-2.0196,-1.55623,-1.0106,-0.530045,0.0583016,1.05245,2.12079,2.59601,M_PI};
const unsigned int n_variable_bins_phi = 10;
*/
vector<double> lower_bin_bounds_pT = {0.75, 1.07, 1.25, 1.49, 2.0};
const unsigned int n_variable_bins_pT = 4;
//vector<float> lower_bin_bounds_eta = {-1.1,-0.82,-0.56,-0.20,0.30,0.72,1.1};
vector<double> lower_bin_bounds_eta = {-1.1,-0.733333,-0.366666,0.0,0.366668,0.733335,1.1};
const unsigned int n_variable_bins_eta = 6;
//vector<float> lower_bin_bounds_phi = {-M_PI,-2.14,-1.37,-0.61,0.35,2.21,M_PI};
vector<double> lower_bin_bounds_phi = {-M_PI,-2.09439,-1.04719,0.0,1.04721,2.09441,M_PI};
const unsigned int n_variable_bins_phi = 6;
//vector<float> lower_bin_bounds_rap = {-1.0,-0.64,-0.41,-0.15,0.21,0.55,1.0};
vector<double> lower_bin_bounds_rap = {-1.0,-0.66,-0.33,0.0,0.33,0.66,1.0};
const unsigned int n_variable_bins_rap = 6;

template <typename T>
string to_string_with_precision(const T a_value, const int n = 0)
{
    ostringstream out;
    out.precision(n);
    out << fixed << a_value;
    return out.str();
}

//Initialise histograms
TH1F makeHisto(int nBins, double min, double max, string type, string xAxisTitle, string unit, int precision, string yAxisTitle = "Geo. Accept.")
{
  string histo_name = type + "_" + xAxisTitle;
  TH1F myHisto(histo_name.c_str(), histo_name.c_str(), nBins, min, max);

  if (unit != "") xAxisTitle += " [" + unit +  "]";
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());

  double binWidth = (double) (max - min)/nBins;
  if (unit != "") yAxisTitle += " / " + to_string_with_precision(binWidth, precision) + " " + unit;
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());

  myHisto.GetXaxis()->SetNdivisions(505);

  return myHisto;
}

TH1F makeHisto(int nBins, double* min, string type, string xAxisTitle, string unit, int precision, string yAxisTitle = "Geo. Accept.")
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
  float floats[20];
  int ints[2];

  //any and variant are a pain here with TBranch 
  map<string, float> float_vars{
    {"gflavor", floats[0]},
    {"gparentflavor", floats[1]},
    {"gparentid", floats[2]},
    {"nmaps", floats[3]},
    {"gpt", floats[4]},
    {"geta", floats[5]},
    {"gphi", floats[6]},
    {"pt", floats[7]},
    {"eta", floats[8]},
    {"phi", floats[9]},
    {"event", floats[10]},
    {"gvz", floats[11]},
    {"gnmaps", floats[12]},
    {"gtrackID", floats[13]},
    {"gprimary", floats[14]},
  };
    
  map<string, int> int_vars{
  };
};

struct TrackInfo
{
    float gflavor;
    float gparentflavor;
    float gparentid;
    float nmaps;
    float gpt;
    float geta;
    float gphi;
    float pt;
    float eta;
    float phi;
    float event;
    float gvz;
    float gnmaps;
    float gtrackID;
    float gprimary;
};

struct KS0Candidate
{
    TrackInfo pion1;      
    TrackInfo pion2;       
    TrackInfo mother_ks; 
    float event;

    float mother_pt;
    float mother_eta;
    float mother_phi;
    float mother_rapidity;
    float mother_mass;
};

struct CascadeCandidate
{
    TrackInfo xi_pion;      
    TrackInfo lambda;       
    TrackInfo lambda_pion;  
    TrackInfo proton;      
    TrackInfo mother_xi; 
    float event;

    float mother_pt;
    float mother_eta;
    float mother_phi;
    float mother_rapidity;
    float mother_mass;
};

//What histograms to build for each ratio check
class histos
{
  public:
    TH1F Ximinus_all;
    TH1F K_S0_all;
    TH1F Ximinus_inGeo;
    TH1F K_S0_inGeo;
    TH1F ratio;
    TH1F inv_ratio;
    bool variable_bins = false;
    int nBins = 15;

  histos(bool constructor_variable_bins = false, double range = 1., string variable = "", string unit = "", vector<double> lower_bin_bounds = {}, const unsigned int n_variable_bins = 0)
  {
    variable_bins = constructor_variable_bins;
    if (constructor_variable_bins)
    {
      Ximinus_all   = makeHisto(n_variable_bins, lower_bin_bounds.data(), "Ximinus_all", variable.c_str(), unit.c_str(), 1);
      K_S0_all      = makeHisto(n_variable_bins, lower_bin_bounds.data(), "K_S0_all", variable.c_str(), unit.c_str(), 1);
      Ximinus_inGeo = makeHisto(n_variable_bins, lower_bin_bounds.data(), "Ximinus_inGeo", variable.c_str(), unit.c_str(), 1);
      K_S0_inGeo    = makeHisto(n_variable_bins, lower_bin_bounds.data(), "K_S0_inGeo", variable.c_str(), unit.c_str(), 1);
      ratio         = makeHisto(n_variable_bins, lower_bin_bounds.data(), "ratio", variable.c_str(), unit.c_str(), 1, "#Xi^{-}/K_{S}^{0} Geo. Accept.");
      inv_ratio     = makeHisto(n_variable_bins, lower_bin_bounds.data(), "inv_ratio", variable.c_str(), unit.c_str(), 1, "#Xi^{-}/K_{S}^{0} Geo. Accept.");
    }
    else
    {
      Ximinus_all   = makeHisto(nBins, -1*range, range, "Ximinus_all", variable.c_str(), unit.c_str(), 1);
      K_S0_all      = makeHisto(nBins, -1*range, range, "K_S0_all", variable.c_str(), unit.c_str(), 1);
      Ximinus_inGeo = makeHisto(nBins, -1*range, range, "Ximinus_inGeo", variable.c_str(), unit.c_str(), 1);
      K_S0_inGeo    = makeHisto(nBins, -1*range, range, "K_S0_inGeo", variable.c_str(), unit.c_str(), 1);
      ratio         = makeHisto(nBins, -1*range, range, "ratio", variable.c_str(), unit.c_str(), 1, "#Xi^{-}/K_{S}^{0} Geo. Accept.");
      inv_ratio     = makeHisto(nBins, -1*range, range, "inv_ratio", variable.c_str(), unit.c_str(), 1, "#Xi^{-}/K_{S}^{0} Geo. Accept.");
    }
  }
};

template <typename T>
void savePlots(T myPlot, string plotName, bool logY = false, float yMin = 0, float yMax = 1)
{
  TGaxis::SetMaxDigits(3);
  system(makeDirectory.c_str());

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  gPad->SetTopMargin(0.1);

  if (yMin >= 0) myPlot.GetYaxis()->SetRangeUser(yMin, yMax);

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
  TText *pt_LaTex = pt->AddText("#it{#bf{sPHENIX}} Simulation, #sqrt{s} = 200 GeV, pp");
  pt->SetBorderSize(0);
  pt->Draw();
  gPad->Modified();

  string extensions[] = {".C", ".pdf", ".png", ".root"};
  for (auto extension : extensions)
  {
    string output = plotPath + "/" + plotName + extension;
    c1->SaveAs(output.c_str());
  }
}

template <typename T>
void saveManyPlots(vector<T> myPlot, string plotName, bool logY = false, vector<string> legendText = {""})
{
  TGaxis::SetMaxDigits(3);
  system(makeDirectory.c_str());

  TCanvas *c1  = new TCanvas("myCanvas", "myCanvas",800,800);

  if (logY) gPad->SetLogy();

  gPad->SetTopMargin(0.1);
  int colors[] = {1, 6, 4, 2, 3, 6, 7, 8, 9};
  Size_t markerStyle = 8;
  Size_t markerSize = 1;

  float legendYpos[2] = {0.65, 0.90};
  float legendTextSize = ((legendYpos[1] - legendYpos[0])/(1.6*myPlot.size()));
  TLegend *legend = new TLegend(0.50,legendYpos[0],0.80, legendYpos[1]);
  legend->SetTextFont(42);

  for (unsigned int i = 0; i < myPlot.size(); ++i)
  {
    float transparency = 1;
    myPlot[i].SetFillColorAlpha(colors[i], transparency);
    myPlot[i].SetLineColor(colors[i]);
    myPlot[i].SetMarkerColorAlpha(colors[i], transparency);
    myPlot[i].SetMarkerSize(markerSize);
    myPlot[i].SetMarkerStyle(markerStyle);
    if (legendText.size() < 2) legend->AddEntry(&myPlot[i], myPlot[i].GetXaxis()->GetTitle(), "PE1");
    else legend->AddEntry(&myPlot[i], legendText[i].c_str(), "PE1");
  }
 
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
  if (strncmp(typeid(myPlot).name(), "4TH2F", 5) == 0) pt = new TPaveText(0.15,0.75,0.65,1., "NDC");
  else pt = new TPaveText(0.25,0.9,0.65,1., "NDC");
  pt->SetFillColor(0);
  pt->SetFillStyle(0);
  pt->SetTextFont(42);
  string label = "#it{#bf{sPHENIX}} Simulation";
  TText *pt_LaTex = pt->AddText(label.c_str());
  pt->SetBorderSize(0);
  pt->Draw();

  legend->SetFillColor(0);
  legend->SetFillStyle(0);
  //legend->SetTextSize(legendTextSize);
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

void reconstructKS0Mother(KS0Candidate& cand, bool useTruthValues)
{
    if (useTruthValues)
    {
      float pi_mass = TDatabasePDG::Instance()->GetParticle(cand.pion1.gflavor)->Mass();
      PtEtaPhiMVector pi1_true_LV = PtEtaPhiMVector(cand.pion1.gpt, cand.pion1.geta, cand.pion1.gphi, pi_mass);
      PtEtaPhiMVector pi2_true_LV = PtEtaPhiMVector(cand.pion2.gpt, cand.pion2.geta, cand.pion2.gphi, pi_mass);

      PtEtaPhiMVector mother_true_LV = pi1_true_LV + pi2_true_LV;
      
      cand.mother_pt = mother_true_LV.Pt();
      cand.mother_eta = mother_true_LV.Eta();
      cand.mother_rapidity = mother_true_LV.Rapidity();
      cand.mother_phi = mother_true_LV.Phi();
      cand.mother_mass = mother_true_LV.M();
    }
    else 
    {
      float pi_mass = TDatabasePDG::Instance()->GetParticle(cand.pion1.gflavor)->Mass();
      PtEtaPhiMVector pi1_reco_LV = PtEtaPhiMVector(cand.pion1.pt, cand.pion1.eta, cand.pion1.phi, pi_mass);
      PtEtaPhiMVector pi2_reco_LV = PtEtaPhiMVector(cand.pion2.pt, cand.pion2.eta, cand.pion2.phi, pi_mass);

      PtEtaPhiMVector mother_reco_LV = pi1_reco_LV + pi2_reco_LV;
      
      cand.mother_pt = mother_reco_LV.Pt();
      cand.mother_eta = mother_reco_LV.Eta();
      cand.mother_rapidity = mother_reco_LV.Rapidity();
      cand.mother_phi = mother_reco_LV.Phi();
      cand.mother_mass = mother_reco_LV.M();
    }
}

void reconstructXiMother(CascadeCandidate& cand, bool useTruthValues)
{
    if (useTruthValues)
    {
      float xi_pi_mass = TDatabasePDG::Instance()->GetParticle(cand.xi_pion.gflavor)->Mass();
      PtEtaPhiMVector xi_pi_true_LV = PtEtaPhiMVector(cand.xi_pion.gpt, cand.xi_pion.geta, cand.xi_pion.gphi, xi_pi_mass);
      float lam_pi_mass = TDatabasePDG::Instance()->GetParticle(cand.lambda_pion.gflavor)->Mass();
      PtEtaPhiMVector lam_pi_true_LV = PtEtaPhiMVector(cand.lambda_pion.gpt, cand.lambda_pion.geta, cand.lambda_pion.gphi, lam_pi_mass);
      float lam_pro_mass = TDatabasePDG::Instance()->GetParticle(cand.proton.gflavor)->Mass();
      PtEtaPhiMVector lam_pro_true_LV = PtEtaPhiMVector(cand.proton.gpt, cand.proton.geta, cand.proton.gphi, lam_pro_mass);

      PtEtaPhiMVector lam_true_LV = lam_pi_true_LV + lam_pro_true_LV;
      PtEtaPhiMVector mother_true_LV = lam_true_LV + xi_pi_true_LV;
      
      cand.mother_pt = mother_true_LV.Pt();
      cand.mother_eta = mother_true_LV.Eta();
      cand.mother_rapidity = mother_true_LV.Rapidity();
      cand.mother_phi = mother_true_LV.Phi();
      cand.mother_mass = mother_true_LV.M();
    }
    else 
    {
      float xi_pi_mass = TDatabasePDG::Instance()->GetParticle(cand.xi_pion.gflavor)->Mass();
      PtEtaPhiMVector xi_pi_reco_LV = PtEtaPhiMVector(cand.xi_pion.pt, cand.xi_pion.eta, cand.xi_pion.phi, xi_pi_mass);
      float lam_pi_mass = TDatabasePDG::Instance()->GetParticle(cand.lambda_pion.gflavor)->Mass();
      PtEtaPhiMVector lam_pi_reco_LV = PtEtaPhiMVector(cand.lambda_pion.pt, cand.lambda_pion.eta, cand.lambda_pion.phi, lam_pi_mass);
      float lam_pro_mass = TDatabasePDG::Instance()->GetParticle(cand.proton.gflavor)->Mass();
      PtEtaPhiMVector lam_pro_reco_LV = PtEtaPhiMVector(cand.proton.pt, cand.proton.eta, cand.proton.phi, lam_pro_mass);

      PtEtaPhiMVector lam_reco_LV = lam_pi_reco_LV + lam_pro_reco_LV;
      PtEtaPhiMVector mother_reco_LV = lam_reco_LV + xi_pi_reco_LV;
      
      cand.mother_pt = mother_reco_LV.Pt();
      cand.mother_eta = mother_reco_LV.Eta();
      cand.mother_rapidity = mother_reco_LV.Rapidity();
      cand.mother_phi = mother_reco_LV.Phi();
      cand.mother_mass = mother_reco_LV.M();
    }
}

void processData(histos& pT, histos& eta, histos& phi, histos& rap, string type = "Kshort2pipi")
{
  string dir = "/sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/sim/geomAccept/";
  string fileName = type == "Kshort2pipi" ? dir + "output_Kshort_ntp_gtrack_20260513_g4svtx_eval.root"
                                          : dir + "output_cascade_ntp_gtrack_4M.root";
  //string fileName = type == "Kshort2pipi" ? dir + "output_Kshort_ntp_gtrack_20260520_pTref3p0.root"
  //                                        : dir + "output_cascade_ntp_gtrack_20260520_pTref3p0.root";
  TFile *file = new TFile(fileName.c_str());
  TTree* data = (TTree*)file->Get("ntp_gtrack");

  variableMap inputMap;
  for (auto &[branch, var] : inputMap.float_vars) data->SetBranchAddress(branch.c_str(), &var);

  unsigned int tmp = 0;
  unsigned int barWidth = 50;

  float min = type == "Kshort2pipi" ? 0.47 : 1.3;
  float max = type == "Kshort2pipi" ? 0.51 : 1.34;

  string name_truth = type == "Kshort2pipi" ? "Kshort_truthInvMass" : "Xi_truthInvMass";
  string name_reco = type == "Kshort2pipi" ? "Kshort_recoInvMass" : "Xi_recoInvMass";
  string axis = type == "Kshort2pipi" ? "m_{#pi#pi}" : "m_{#Lambda#pi}";

  TH1F truthMass = makeHisto(40, min, max, name_truth, axis, "GeV", 3, "Candidates");
  TH1F recoMass = makeHisto(40, min, max, name_reco, axis, "GeV", 3, "Candidates"); 

  string name_truthPT = type == "Kshort2pipi" ? "Kshort_truthPT" : "Xi_truthPT";
  string name_recoPT = type == "Kshort2pipi" ? "Kshort_recoPT" : "Xi_recoPT";
  string axisPT = type == "Kshort2pipi" ? "K_{S}^{0} truth p_{T}" : "#Xi^{-} truth p_{T}";

  TH1F truthPT = makeHisto(100, 0, 3, name_truthPT, axisPT, "GeV", 2, "Candidates");
  TH1F recoPT = makeHisto(100, 0, 3, name_recoPT, axisPT, "GeV", 2, "Candidates"); 

  //Geometric Acceptance
  int matched_counter = 0;
  int unmatched_counter = 0;
  int nRecoTrack = 0;
  int nRecoMother = 0;
  unsigned int num_entries = data->GetEntries();
  if (type == "Kshort2pipi")
  {
    auto isKS0     = [&](float id){ return fabs(fabs(id) - 310.f) < 0.5f; };
    auto isPion   = [&](float id){ return fabs(fabs(id) -  211.f) < 0.5f; };
    auto sameID   = [&](float a, float b){ return fabs(a - b) < 0.5f; };
    auto isPrimary = [&](float val){ return fabs(fabs(val) - 1.f) < 0.5f; };

    std::vector<TrackInfo> eventTracks;
    std::vector<KS0Candidate> candidates;

    float currentEvent = -999.f;
   
    auto processKS0Event = [&](std::vector<TrackInfo>& tracks)
    {
        if (tracks.empty()) return;

        std::vector<TrackInfo*> pions;
        std::vector<TrackInfo*> mom_ks0s;

        for (auto& t : tracks)
        {
            if (isPion(t.gflavor) && isKS0(t.gparentflavor)) pions.push_back(&t);
            if (isKS0(t.gflavor) && isPrimary(t.gprimary)) mom_ks0s.push_back(&t);
        }

        int p1_idx = -1; 
        for (auto* p1 : pions)
        {
            p1_idx++;
            int p2_idx = -1;
            for (auto* p2 : pions)
            {
                p2_idx++;
                if (p2_idx <= p1_idx) continue;       

                if (sameID(p1->gtrackID, p2->gtrackID)) continue;

                if (!sameID(p1->gparentid, p2->gparentid)) continue;

                for (auto* ks : mom_ks0s)
                {
                  if (!sameID(ks->gtrackID, p1->gparentid)) continue;
                          
                  KS0Candidate cand;
                  cand.pion1     = *p1;
                  cand.pion2     = *p2;
                  cand.event     = p1->event;
                  reconstructKS0Mother(cand, true);
                  candidates.push_back(cand);
                }
            }
        }
        if (candidates.size() > 0)
        {
          matched_counter += candidates.size();
        }     
        else
        {
          ++unmatched_counter;
        }

        for (auto& cand : candidates)
        {   
          bool p1_in = cand.pion1.nmaps > 0;
          bool p2_in = cand.pion2.nmaps > 0;
          
          if (p1_in && p2_in)
          {
            ++nRecoMother;
          }
 
          if (abs(cand.mother_rapidity) > 1.0) continue;          

          truthMass.Fill(cand.mother_mass);
          truthPT.Fill(cand.mother_pt);

	  pT.K_S0_all.Fill(cand.mother_pt);
	  eta.K_S0_all.Fill(cand.mother_eta);
	  rap.K_S0_all.Fill(cand.mother_rapidity);
	  phi.K_S0_all.Fill(cand.mother_phi);
          
          if (p1_in && p2_in)
          {	
            pT.K_S0_inGeo.Fill(cand.mother_pt);
	    eta.K_S0_inGeo.Fill(cand.mother_eta);
	    rap.K_S0_inGeo.Fill(cand.mother_rapidity);
	    phi.K_S0_inGeo.Fill(cand.mother_phi);
            
            reconstructKS0Mother(cand, false);

            recoMass.Fill(cand.mother_mass);
            recoPT.Fill(cand.mother_pt);
          }
        }
    };
                      

    for (Long64_t i = 0; i < num_entries; ++i)
    {
      if (tmp != (unsigned int)100*i/num_entries)
      {
	tmp = (unsigned int)100*i/num_entries;
	if ((tmp%1)  == 0)
	{
	  cout << "[";
	  unsigned int pos = barWidth * tmp/100;
	  for (unsigned int l = 0; l < barWidth; ++l)
	  {
	    if (l < pos) cout << "=";
	    else if (l == pos) cout << ">";
	    else cout << " ";
	  }
	  cout << "] " << tmp << " %\r";
	  cout.flush();
	}
      }
 
      data->GetEntry(i);

      if (inputMap.float_vars["nmaps"] > 0) ++nRecoTrack;

      if (abs(inputMap.float_vars["gvz"]) > 10) continue;

      float thisEvent = inputMap.float_vars["event"];

      // When the event number changes, process the accumulated tracks
      if (thisEvent != currentEvent && !eventTracks.empty())
      {   
        processKS0Event(eventTracks);
        eventTracks.clear();
        candidates.clear();
      }

      currentEvent = thisEvent;

      TrackInfo t;
      t.event         = inputMap.float_vars["event"];
      t.gflavor       = inputMap.float_vars["gflavor"];
      t.gparentflavor = inputMap.float_vars["gparentflavor"];
      t.gparentid     = inputMap.float_vars["gparentid"];
      t.gpt           = inputMap.float_vars["gpt"];
      t.geta          = inputMap.float_vars["geta"];
      t.gphi          = inputMap.float_vars["gphi"];
      t.nmaps         = inputMap.float_vars["nmaps"];
      t.pt            = inputMap.float_vars["pt"];
      t.eta           = inputMap.float_vars["eta"];
      t.phi           = inputMap.float_vars["phi"];
      t.gvz           = inputMap.float_vars["gvz"];
      t.gnmaps        = inputMap.float_vars["gnmaps"];
      t.gtrackID      = inputMap.float_vars["gtrackID"];
      t.gprimary      = inputMap.float_vars["gprimary"];
      eventTracks.push_back(t);
    }
    processKS0Event(eventTracks);
  }
  else
  {
    auto isXi     = [&](float id){ return fabs(fabs(id) - 3312.f) < 0.5f; };
    auto isLambda = [&](float id){ return fabs(fabs(id) - 3122.f) < 0.5f; };
    auto isPion   = [&](float id){ return fabs(fabs(id) -  211.f) < 0.5f; };
    auto isProton = [&](float id){ return fabs(fabs(id) - 2212.f) < 0.5f; };
    auto sameID   = [&](float a, float b){ return fabs(a - b) < 0.5f; };
    auto isPrimary = [&](float val){ return fabs(fabs(val) - 1.f) < 0.5f; };

    std::vector<TrackInfo> eventTracks;
    std::vector<CascadeCandidate> candidates;

    float currentEvent = -999.f;
   
    auto processXiEvent = [&](std::vector<TrackInfo>& tracks)
    {
        if (tracks.empty()) return;

        std::vector<TrackInfo*> xi_pions;
        std::vector<TrackInfo*> lambdas;
        std::vector<TrackInfo*> lambda_pions;
        std::vector<TrackInfo*> protons;
        std::vector<TrackInfo*> mom_xis;

        for (auto& t : tracks)
        {
            if (isPion(t.gflavor)   && isXi(t.gparentflavor))     xi_pions.push_back(&t);
            if (isLambda(t.gflavor) && isXi(t.gparentflavor))     lambdas.push_back(&t);
            if (isPion(t.gflavor)   && isLambda(t.gparentflavor)) lambda_pions.push_back(&t);
            if (isProton(t.gflavor) && isLambda(t.gparentflavor)) protons.push_back(&t);
            if (isXi(t.gflavor) && isPrimary(t.gprimary)) mom_xis.push_back(&t);
        }
 
        for (auto* xp : xi_pions)
        {
            for (auto* lam : lambdas)
            {
                if (!sameID(xp->gparentid, lam->gparentid)) continue;

                for (auto* lp : lambda_pions)
                {
                    if (lp->gflavor != xp->gflavor) continue;
 
                    for (auto* pr : protons)
                    {
                        if (!sameID(lp->gparentid, pr->gparentid)) continue;

                        if (!sameID(lam->gtrackID, lp->gparentid)) continue;
                    
                        for(auto* mx : mom_xis)
                        {
                          if (!sameID(mx->gtrackID, xp->gparentid)) continue;
                          
                          CascadeCandidate cand;
                          cand.xi_pion     = *xp;
                          cand.lambda      = *lam;
                          cand.lambda_pion = *lp;
                          cand.proton      = *pr;
                          cand.event       = xp->event;
                          reconstructXiMother(cand, true);
                          candidates.push_back(cand);
                        }
                    }
                }
            }
        }
        if (candidates.size() > 0)
        {
          matched_counter += candidates.size();
        }     
        else
        {
          ++unmatched_counter;
        }

        for (auto& cand : candidates)
        {   
          bool xp_in = cand.xi_pion.nmaps > 0;
          bool lp_in = cand.lambda_pion.nmaps > 0;
          bool pr_in = cand.proton.nmaps > 0;
          
          if (!(xp_in && lp_in && pr_in))
          {
            ++nRecoMother;
          }
 
          if (abs(cand.mother_rapidity) > 1.0) continue;          

          truthMass.Fill(cand.mother_mass);
          truthPT.Fill(cand.mother_pt);

	  pT.Ximinus_all.Fill(cand.mother_pt);
	  eta.Ximinus_all.Fill(cand.mother_eta);
	  rap.Ximinus_all.Fill(cand.mother_rapidity);
	  phi.Ximinus_all.Fill(cand.mother_phi);
          
          if (xp_in && lp_in && pr_in)
          {	
            pT.Ximinus_inGeo.Fill(cand.mother_pt);
	    eta.Ximinus_inGeo.Fill(cand.mother_eta);
	    rap.Ximinus_inGeo.Fill(cand.mother_rapidity);
	    phi.Ximinus_inGeo.Fill(cand.mother_phi);
            
            reconstructXiMother(cand, false);

            recoMass.Fill(cand.mother_mass);
            recoPT.Fill(cand.mother_pt);
          }
        }
    };
                      

    for (Long64_t i = 0; i < num_entries; ++i)
    {
      if (tmp != (unsigned int)100*i/num_entries)
      {
	tmp = (unsigned int)100*i/num_entries;
	if ((tmp%1)  == 0)
	{
	  cout << "[";
	  unsigned int pos = barWidth * tmp/100;
	  for (unsigned int l = 0; l < barWidth; ++l)
	  {
	    if (l < pos) cout << "=";
	    else if (l == pos) cout << ">";
	    else cout << " ";
	  }
	  cout << "] " << tmp << " %\r";
	  cout.flush();
	}
      }
 
      data->GetEntry(i);

      if (inputMap.float_vars["nmaps"] > 0) ++nRecoTrack;

      if (abs(inputMap.float_vars["gvz"]) > 10) continue;

      float thisEvent = inputMap.float_vars["event"];

      // When the event number changes, process the accumulated tracks
      if (thisEvent != currentEvent && !eventTracks.empty())
      {   
        processXiEvent(eventTracks);
        eventTracks.clear();
        candidates.clear();
      }

      currentEvent = thisEvent;

      TrackInfo t;
      t.event         = inputMap.float_vars["event"];
      t.gflavor       = inputMap.float_vars["gflavor"];
      t.gparentflavor = inputMap.float_vars["gparentflavor"];
      t.gparentid     = inputMap.float_vars["gparentid"];
      t.gpt           = inputMap.float_vars["gpt"];
      t.geta          = inputMap.float_vars["geta"];
      t.gphi          = inputMap.float_vars["gphi"];
      t.nmaps         = inputMap.float_vars["nmaps"];
      t.pt            = inputMap.float_vars["pt"];
      t.eta           = inputMap.float_vars["eta"];
      t.phi           = inputMap.float_vars["phi"];
      t.gvz           = inputMap.float_vars["gvz"];
      t.gnmaps        = inputMap.float_vars["gnmaps"];
      t.gtrackID      = inputMap.float_vars["gtrackID"];
      t.gprimary      = inputMap.float_vars["gprimary"];
      eventTracks.push_back(t);
    }
    processXiEvent(eventTracks);
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
  
  savePlots(truthMass, name_truth, false, -1);
  savePlots(recoMass, name_reco, false, -1);
  
  vector<TH1F> plots = {truthPT, recoPT};
  vector<string> legend = {"Denominator", "Numerator"};
  saveManyPlots(plots, name_truthPT, true, legend);

  recoPT.Sumw2();
  truthPT.Sumw2();
  TH1F ratio = recoPT;
  ratio.Divide(&truthPT);
  string title = type == "Kshort2pipi" ? "Kshort_ratio" : "Xi_ratio";
  savePlots(ratio, title, false, 0, 0.25);

  cout << "=========" << type << " diagnostics" << "=========" << endl;
  cout << "Within all mother rapidities" << endl;
  cout << "There are " << matched_counter << " matched mothers and " << unmatched_counter << " unmatched mothers" << endl;
  cout << "There are " << nRecoTrack << " reconstructed tracks with at least one MVTX state and " << nRecoMother << " reconstructed mothers with MVTX states on both tracks" << endl;
}

void makeRatios(histos &histoSet, string trailer)
{
  string K_S0_saveName = "KS0_geometric_acceptance_ratio_onlyPrimaries_" + trailer;
  string Ximinus_saveName = "Ximinus_geometric_acceptance_ratio_onlyPrimaries_" + trailer;
  string ratio_saveName = "Ximinus_to_KS0_geometric_acceptance_ratio_onlyPrimaries_" + trailer;
  string inv_ratio_saveName = "K_S0_to_Ximinus_geometric_acceptance_ratio_onlyPrimaries_" + trailer;

  histoSet.Ximinus_all.Sumw2();
  histoSet.K_S0_all.Sumw2();

  histoSet.Ximinus_inGeo.Sumw2();
  histoSet.K_S0_inGeo.Sumw2();

  histoSet.Ximinus_inGeo.Divide(&histoSet.Ximinus_all);
  histoSet.K_S0_inGeo.Divide(&histoSet.K_S0_all);

  histoSet.ratio = histoSet.Ximinus_inGeo;
  histoSet.inv_ratio = histoSet.K_S0_inGeo;

  histoSet.ratio.Divide(&histoSet.K_S0_inGeo);
  histoSet.inv_ratio.Divide(&histoSet.Ximinus_inGeo);

  if (saveFinalPlots)
  {
    savePlots(histoSet.K_S0_inGeo, K_S0_saveName.c_str(), false, 0, 0.070);
    savePlots(histoSet.Ximinus_inGeo, Ximinus_saveName.c_str(), false, 0, 0.010);

    savePlots(histoSet.ratio, ratio_saveName.c_str(), false, 0, 0.1);
    savePlots(histoSet.inv_ratio, inv_ratio_saveName.c_str(), false, 0, 125);
  }
}

void makeTable(string var, histos theseHistos, string type = "Markdown", const unsigned int n_variable_bins = 0)
{
  cout << endl;
 
  string startline = type == "Markdown" ? "| " : "";
  string separator = type == "Markdown" ? " | " : " & ";
  string endline  = type == "Markdown" ? " | " : " \\\\ ";

  int nBins = theseHistos.variable_bins ? n_variable_bins : theseHistos.nBins;

  if (type != "Markdown") cout << "\\begin{tabular}{@{}ccccc@{}}" << endl << "\\toprule[1pt]" << endl;
  
  cout << startline << var << separator << "$K_S^0$" << separator << "$\\Xi^-$" << separator << "$\\Xi^- / K_S^0$" << separator << "$K_S^0 / \\Xi^-$" << endline << endl;

  if (type == "Markdown") cout << "|:--:|:--:|:--:|:--:|:--:|" << endl;
  else cout << "\\midrule[0.2pt]" << endl;

  for (int i = 1; i <= nBins; ++i)
  {
    string low = to_string_with_precision(theseHistos.ratio.GetXaxis()->GetBinLowEdge(i), 2);
    string high = to_string_with_precision(theseHistos.ratio.GetXaxis()->GetBinUpEdge(i), 2);

    string Ks0_content = to_string_with_precision(theseHistos.K_S0_inGeo.GetBinContent(i), 4);
    string Ks0_error = to_string_with_precision(theseHistos.K_S0_inGeo.GetBinError(i), 4);

    string Ximinus_content = to_string_with_precision(theseHistos.Ximinus_inGeo.GetBinContent(i), 4);
    string Ximinus_error = to_string_with_precision(theseHistos.Ximinus_inGeo.GetBinError(i), 4);

    string content = to_string_with_precision(theseHistos.ratio.GetBinContent(i), 2);
    string error = to_string_with_precision(theseHistos.ratio.GetBinError(i), 2);

    string inv_content = to_string_with_precision(theseHistos.inv_ratio.GetBinContent(i), 2);
    string inv_error = to_string_with_precision(theseHistos.inv_ratio.GetBinError(i), 2);
    cout << startline << low << " $\\rightarrow$ " << high << separator << Ks0_content << " $\\pm$ " << Ks0_error << separator << Ximinus_content << " $\\pm$ " << Ximinus_error << separator << content << " $\\pm$ " << error << separator << inv_content << " $\\pm$ " << inv_error << endline << endl;
  }

  if (type != "Markdown") cout << "\\bottomrule[1pt]" << endl << "\\end{tabular}" << endl;
}

void makeArray(string type, histos theseHistos, const unsigned int n_variable_bins)
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

  cout << "float x[] = {";
  for (auto &val : xVal) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "float ex[] = {";
  for (auto &val : xErr) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "float y[] = {";
  for (auto &val : yVal) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "float ey[] = {";
  for (auto &val : yErr) cout << to_string_with_precision(val, 3) << ", ";
  cout << "};" << endl;

  cout << "const int n = sizeof(x)/sizeof(x[0]);" << endl;
}

void calcGeomAcceptEvaluator_onlyPrimaries()
{
  histos pT(true, 0, "pT", "GeV", lower_bin_bounds_pT, n_variable_bins_pT);
  histos eta(true, 1.1, "#eta", "", lower_bin_bounds_eta, n_variable_bins_eta);
  histos phi(true, M_PI, "#phi", "rad", lower_bin_bounds_phi, n_variable_bins_phi);
  histos rap(true, 1.0, "y", "", lower_bin_bounds_rap, n_variable_bins_rap);

  cout << "Processing K-short data" << endl;
  processData(pT, eta, phi, rap);
  cout << "Processing Xi-minus data" << endl;
  processData(pT, eta, phi, rap, "Ximinus2Lambdapi");

  TFile* outputFile = TFile::Open("./plots_evaluator/geometricAcceptanceEfficiencyCorrections.root", "RECREATE"); 
  pT.Ximinus_all.Sumw2();
  pT.K_S0_all.Sumw2();
  pT.Ximinus_inGeo.Sumw2();
  pT.K_S0_inGeo.Sumw2();
  pT.Ximinus_inGeo.Divide(&pT.Ximinus_all);
  pT.K_S0_inGeo.Divide(&pT.K_S0_all);
  TH1F* h_geoAccEff_pT_KS0 = &pT.K_S0_inGeo;
  TH1F* h_geoAccEff_pT_Xi = &pT.Ximinus_inGeo;
  h_geoAccEff_pT_KS0->SetName("h_geoAccEff_pT_KS0");
  h_geoAccEff_pT_Xi->SetName("h_geoAccEff_pT_Xi");
  eta.Ximinus_all.Sumw2();
  eta.K_S0_all.Sumw2();
  eta.Ximinus_inGeo.Sumw2();
  eta.K_S0_inGeo.Sumw2();
  eta.Ximinus_inGeo.Divide(&eta.Ximinus_all);
  eta.K_S0_inGeo.Divide(&eta.K_S0_all);
  TH1F* h_geoAccEff_eta_KS0 = &eta.K_S0_inGeo;
  TH1F* h_geoAccEff_eta_Xi = &eta.Ximinus_inGeo;
  h_geoAccEff_eta_KS0->SetName("h_geoAccEff_eta_KS0");
  h_geoAccEff_eta_Xi->SetName("h_geoAccEff_eta_Xi");
  phi.Ximinus_all.Sumw2();
  phi.K_S0_all.Sumw2();
  phi.Ximinus_inGeo.Sumw2();
  phi.K_S0_inGeo.Sumw2();
  phi.Ximinus_inGeo.Divide(&phi.Ximinus_all);
  phi.K_S0_inGeo.Divide(&phi.K_S0_all);
  TH1F* h_geoAccEff_phi_KS0 = &phi.K_S0_inGeo;
  TH1F* h_geoAccEff_phi_Xi = &phi.Ximinus_inGeo;
  h_geoAccEff_phi_KS0->SetName("h_geoAccEff_phi_KS0");
  h_geoAccEff_phi_Xi->SetName("h_geoAccEff_phi_Xi");
  rap.Ximinus_all.Sumw2();
  rap.K_S0_all.Sumw2();
  rap.Ximinus_inGeo.Sumw2();
  rap.K_S0_inGeo.Sumw2();
  rap.Ximinus_inGeo.Divide(&rap.Ximinus_all);
  rap.K_S0_inGeo.Divide(&rap.K_S0_all);
  TH1F* h_geoAccEff_rap_KS0 = &rap.K_S0_inGeo;
  TH1F* h_geoAccEff_rap_Xi = &rap.Ximinus_inGeo;
  h_geoAccEff_rap_KS0->SetName("h_geoAccEff_rap_KS0");
  h_geoAccEff_rap_Xi->SetName("h_geoAccEff_rap_Xi");

  h_geoAccEff_pT_KS0->Write(); 
  h_geoAccEff_pT_Xi->Write(); 
  h_geoAccEff_eta_KS0->Write(); 
  h_geoAccEff_eta_Xi->Write(); 
  h_geoAccEff_phi_KS0->Write(); 
  h_geoAccEff_phi_Xi->Write(); 
  h_geoAccEff_rap_KS0->Write(); 
  h_geoAccEff_rap_Xi->Write(); 

  outputFile->Close(); 

  /*
  makeRatios(pT, "pT");
  makeRatios(eta, "eta");
  makeRatios(phi, "phi");
  makeRatios(rap, "rap");

  if (printMarkdownTables)
  {
    makeTable("$p_{T}$ [GeV]", pT, "Markdown",n_variable_bins_pT);
    //makeTable("$\\eta$", eta, "Markdown",n_variable_bins_eta);
    //makeTable("$y$", rap, "Markdown");
    //makeTable("$\\phi$", phi, "Markdown",n_variable_bins_phi);
  }

  if (printLatexTables)
  {
    makeTable("$p_{T}$ [GeV]", pT, "LaTex",n_variable_bins_pT);
    makeTable("$\\eta$", eta, "LaTex",n_variable_bins_eta);
    makeTable("$y$", rap, "LaTex");
    makeTable("$\\phi$", phi, "LaTex",n_variable_bins_phi);
  }

  if (printArrays)
  {
    makeArray("pT", pT,n_variable_bins_pT);
  }
  */
}
