#include <cmath>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

using namespace std;

int counter = 0;
bool debug = false;
bool doTrackingEfficiency = true;

float calcEta(float r, float z)
{
  float theta = atan2(r, z);
  float eta = -1*log(tan(0.5*theta));
  return eta;
}

bool isInRange(float min, float value, float max)
{
  return min <= value && value <= max;
}

bool isInAcceptance(float tr1_pT, float tr2_pT, float tr1_eta, float tr2_eta, float SV[3])
{
  float pT_min = 0.15; //Do we need a minimum pT cut if its not in tracking eff calc?

  float mvtx_radius_max = 3.5; //Tracking requires us to decay before L1 of MVTX. Is this also in Tony's tracking eff calc?
  float mvtx_z_max = 13.5; 

  float tpc_radius_max = 58.0; //We don't need to pass all the way through the TPC to make a track
  float tpc_z_max = 102.325;

  bool tr1_pT_accept = tr1_pT >= pT_min ? true : false;
  bool tr2_pT_accept = tr2_pT >= pT_min ? true : false;
 
  float radius_SV = sqrt(pow(SV[0], 2) + pow(SV[1], 2));
  bool radius_accept = radius_SV < mvtx_radius_max ? true : false;

  bool z_accept = abs(SV[2]) < mvtx_z_max ? true : false;

  //What is the range of eta for this SV where a track will pass MVTX L1?
  //Don't need to worry about having a radial SV beyond L1 giving strange eta, radial cut will reject
  float mvtx_eta_min = calcEta(mvtx_radius_max - radius_SV, -1*mvtx_z_max - SV[2]);
  float mvtx_eta_max = calcEta(mvtx_radius_max - radius_SV, mvtx_z_max - SV[2]);

  //What is the range of eta for this SV where a track will pass through the TPC?
  float tpc_eta_min = calcEta(tpc_radius_max - radius_SV, -1*tpc_z_max - SV[2]);
  float tpc_eta_max = calcEta(tpc_radius_max - radius_SV, tpc_z_max - SV[2]);

  bool accept = tr1_pT_accept && tr2_pT_accept && radius_accept && z_accept
                && isInRange(mvtx_eta_min, tr1_eta, mvtx_eta_max) && isInRange(mvtx_eta_min, tr2_eta, mvtx_eta_max)
                && isInRange(tpc_eta_min, tr1_eta, tpc_eta_max) && isInRange(tpc_eta_min, tr2_eta, tpc_eta_max);

  if (debug && counter < 20)
  {
    cout << "Candidate counter " << counter << endl;
    std::cout << "SV radius is " << radius_SV << ", SV z is " << SV[2] << std::endl;
    std::cout << "mvtx_eta_min: " << mvtx_eta_min << ", tr1_eta: " << tr1_eta << ", tr2_eta: " << tr2_eta << ", mvtx_eta_max: " << mvtx_eta_max << std::endl;
    std::cout << "tpc_eta_min: " << tpc_eta_min << ", tr1_eta: " << tr1_eta << ", tr2_eta: " << tr2_eta << ", tpc_eta_max: " << tpc_eta_max << std::endl;
    if (accept) std::cout << BOLDGREEN << "Candidate is in geometric acceptance" << RESET << std::endl;
    else std::cout << BOLDRED << "Candidate failed geometric acceptance" << RESET << std::endl;
  }

  return accept;
}

template <typename T>
void savePlots(T myPlot, string plotName, bool logY = false, float yMin = 0, float yMax = 1)
{
  std::string plotPath = "plots/";
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

TH1F makeHisto(int nBins, float* min, string xAxisTitle, string unit, int precision, string yAxisTitle = "Geo. Accept.")
{
  TH1F myHisto(xAxisTitle.c_str(), xAxisTitle.c_str(), nBins, min);

  if (unit != "") xAxisTitle += " [" + unit +  "]";
  myHisto.GetXaxis()->SetTitle(xAxisTitle.c_str());
  myHisto.GetYaxis()->SetTitle(yAxisTitle.c_str());

  return myHisto;
}

void calcGeomAccept()
{
  string fileName = "/sphenix/u/cdean/analysis/HF-Particle/KFParticle_sPHENIX/hf_generator/Lambda2ppi_20260304/outputHFTrackEff_Lambda2ppi.root";
  TFile *file = new TFile(fileName.c_str());
  TTree* data = (TTree*)file->Get("HFTrackEfficiency");

  string fileName2 = "/sphenix/u/cdean/analysis/HF-Particle/KFParticle_sPHENIX/hf_generator/Kshort2pipi_20260304/outputHFTrackEff_Kshort2pipi.root";
  TFile *file2 = new TFile(fileName2.c_str());
  TTree* data2 = (TTree*)file2->Get("HFTrackEfficiency");

  float Lambda0_pT; data->SetBranchAddress("true_mother_pT",&Lambda0_pT);
  float Lambda0_track_1_pT; data->SetBranchAddress("true_track_1_pT",&Lambda0_track_1_pT);
  float Lambda0_track_2_pT; data->SetBranchAddress("true_track_2_pT",&Lambda0_track_2_pT);
  float Lambda0_track_1_eta; data->SetBranchAddress("true_track_1_eta",&Lambda0_track_1_eta);
  float Lambda0_track_2_eta; data->SetBranchAddress("true_track_2_eta",&Lambda0_track_2_eta);
  float Lambda0_SV[3];
  data->SetBranchAddress("true_secondary_vertex_x",&Lambda0_SV[0]);
  data->SetBranchAddress("true_secondary_vertex_y",&Lambda0_SV[1]);
  data->SetBranchAddress("true_secondary_vertex_z",&Lambda0_SV[2]);

  float K_S0_pT; data2->SetBranchAddress("true_mother_pT",&K_S0_pT);
  float K_S0_track_1_pT; data2->SetBranchAddress("true_track_1_pT",&K_S0_track_1_pT);
  float K_S0_track_2_pT; data2->SetBranchAddress("true_track_2_pT",&K_S0_track_2_pT);
  float K_S0_track_1_eta; data2->SetBranchAddress("true_track_1_eta",&K_S0_track_1_eta);
  float K_S0_track_2_eta; data2->SetBranchAddress("true_track_2_eta",&K_S0_track_2_eta);
  float K_S0_SV[3];
  data2->SetBranchAddress("true_secondary_vertex_x",&K_S0_SV[0]);
  data2->SetBranchAddress("true_secondary_vertex_y",&K_S0_SV[1]);
  data2->SetBranchAddress("true_secondary_vertex_z",&K_S0_SV[2]);

  bool Lambda0_all_tracks_reconstructed; data->SetBranchAddress("all_tracks_reconstructed", &Lambda0_all_tracks_reconstructed);
  bool K_S0_all_tracks_reconstructed; data2->SetBranchAddress("all_tracks_reconstructed", &K_S0_all_tracks_reconstructed);

  const unsigned int n_variable_bins = 7; 
  float lower_bin_bounds[n_variable_bins + 1] = {0.5, 0.8, 1.1, 1.4, 1.8, 2.2, 3, 4};

  TH1F Lambda0_all_pT = makeHisto(n_variable_bins, lower_bin_bounds, "pT", "GeV", 1);
  TH1F K_S0_all_pT = makeHisto(n_variable_bins, lower_bin_bounds, "pT", "GeV", 1);

  TH1F Lambda0_in_geometry_pT = makeHisto(n_variable_bins, lower_bin_bounds, "pT", "GeV", 1);
  TH1F K_S0_in_geometry_pT = makeHisto(n_variable_bins, lower_bin_bounds, "pT", "GeV", 1);

  TH1F final_ratio = makeHisto(n_variable_bins, lower_bin_bounds, "pT", "GeV", 1, "#Lambda^{0}/K_{S}^{0} Geo. Accept.");

  int tmp = 0;
  int barWidth = 50;

  //Lambda0 Geometric Acceptance
  int num_entries = data->GetEntries();
  for (int  l = 0; l < num_entries; ++l)
  {
    if (!debug)
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
    }

    data->GetEntry(l);
    counter = l;

    Lambda0_all_pT.Fill(Lambda0_pT);

    bool accepted = doTrackingEfficiency ? Lambda0_all_tracks_reconstructed : isInAcceptance(Lambda0_track_1_pT, Lambda0_track_2_pT, Lambda0_track_1_eta, Lambda0_track_2_eta, Lambda0_SV);

    if (accepted) Lambda0_in_geometry_pT.Fill(Lambda0_pT);
  }

  tmp = 0; //cout<<"Creating Particles: 0%,"<<flush;
  barWidth = 50;

  //K_S0 Geometric Acceptance
  num_entries = data2->GetEntries();
  for (int  l = 0; l < num_entries; ++l)
  {
    if (!debug)
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
    }

    data2->GetEntry(l);
    counter = l;

    K_S0_all_pT.Fill(K_S0_pT);

    bool accepted = doTrackingEfficiency ? K_S0_all_tracks_reconstructed : isInAcceptance(K_S0_track_1_pT, K_S0_track_2_pT, K_S0_track_1_eta, K_S0_track_2_eta, K_S0_SV);

    if (accepted) K_S0_in_geometry_pT.Fill(K_S0_pT);
  }

  if (!debug)
  {
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

  Lambda0_all_pT.Sumw2();
  K_S0_all_pT.Sumw2();

  Lambda0_in_geometry_pT.Sumw2();
  K_S0_in_geometry_pT.Sumw2();

  Lambda0_in_geometry_pT.Divide(&Lambda0_all_pT);
  K_S0_in_geometry_pT.Divide(&K_S0_all_pT);

  savePlots(Lambda0_in_geometry_pT, "Lambda0_geometric_acceptance_ratio", false, 0, 0.01);
  savePlots(K_S0_in_geometry_pT, "KS0_geometric_acceptance_ratio", false, 0, 0.01);

  final_ratio = Lambda0_in_geometry_pT; 
  final_ratio.Divide(&K_S0_in_geometry_pT);

  savePlots(final_ratio, "Lambda0_to_KS0_geometric_acceptance_ratio", false, 0, 1);

  //Geometric + efficiency ratio
  for (int i = 0; i <= n_variable_bins; ++i)
  {
    float low_pT = final_ratio.GetXaxis()->GetBinLowEdge(i);
    float high_pT = final_ratio.GetXaxis()->GetBinUpEdge(i);
    float content = final_ratio.GetBinContent(i);
    std::cout << "For bin " << i << ", lower pT is " << low_pT << ", upper pT is " << high_pT << ", Lambda/Kshort efficiency is " << content << std::endl;
  } 

  file->Close();
  file2->Close();
}
