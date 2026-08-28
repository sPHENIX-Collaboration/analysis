#ifndef KINEMATIC_THRESHOLD_H
#define KINEMATIC_THRESHOLD_H

#include "../util/binning.h"

// For near-threshold decays in bins of mother pT, both signal and background are modified by the daughter pT cutoff in reconstruction.
// Some fraction of decays at a given mass produce one or both daughter tracks with pT too small to be reconstructed.
// This fraction goes up with decreasing mother pT and mother mass.
// The functions below semi-analytically calculate this threshold turn-on as a function of mass, mother pT, and daughter pT cutoff.
// This also assumes reconstruction is being done at mid-rapidity.

// Lab-frame daughter pT as a function of CM-frame daughter theta, for a given CM-frame daughter (E,p)
Double_t lab_pt_vs_cm_decay_axis(Double_t* v, Double_t* par)
{
  double theta = v[0];
  double phi = v[1];
  double mother_mass = par[0];
  double mother_pt = par[1];
  double pcm = par[2];
  double Ecm = par[3];

  double mother_E = sqrt(pow(mother_pt,2)+pow(mother_mass,2));
  double mother_gamma = mother_E/mother_mass;
  double mother_beta = mother_pt/(mother_gamma*mother_mass);

  double pt_lab = sqrt(pow(mother_gamma*(pcm*cos(theta)*cos(phi)+mother_beta*Ecm),2)+pow(pcm*cos(theta)*sin(phi),2));
  return pt_lab;
}

// Comparator that tests if the lab-frame daughter pT is above the cutoff
Double_t pt_above_cutoff(Double_t* v, Double_t* par)
{
  double theta = v[0];
  double pt_cutoff = par[4];
  if(lab_pt_vs_cm_decay_axis(v,par)<pt_cutoff) return 0.;
  // integration weight for spherical surface element is cos(theta)
  else return cos(theta);
}

// threshold turnon function
Double_t kinematic_threshold_turnon(Double_t* v, Double_t* par)
{
  double mass = v[0];
  double mother_pt = par[0];
  double daughter_pt_cutoff = par[1];
  double daughter1_mass = par[2];
  double daughter2_mass = par[3];

  // energy must be conserved
  if(mass<daughter1_mass+daughter2_mass) return 0.;

  double daughter1_Ecm = (pow(mass,2)+pow(daughter1_mass,2)-pow(daughter2_mass,2))/2*mass;
  double daughter2_Ecm = (pow(mass,2)+pow(daughter2_mass,2)-pow(daughter1_mass,2))/2*mass;

  double daughter1_pcm = sqrt(pow(daughter1_Ecm,2)-pow(daughter1_mass,2));
  double daughter2_pcm = sqrt(pow(daughter2_Ecm,2)-pow(daughter2_mass,2));

  // average value of (pt above threshold? 1 : 0) function gives you fraction of reconstructible daughters
  // this function is not that well behaved with the standard adaptive singular integrator (sharp transitions from 1 to 0)
  // but is pretty ideal for a MC integrator [TODO]

  ROOT::Math::IntegratorMultiDimOptions::SetDefaultIntegrator("VEGAS");

  TF2* daughter1_comp = new TF2("daughter1_comp",&pt_above_cutoff,-M_PI/2.,M_PI/2.,0.,2*M_PI,5);
  daughter1_comp->SetParameters(mass,mother_pt,daughter1_pcm,daughter1_Ecm,daughter_pt_cutoff);
  double daughter1_frac = daughter1_comp->Integral(-M_PI/2.,M_PI/2.,0.,2*M_PI,1e-4) / (4.*M_PI);
  delete daughter1_comp;

  TF2* daughter2_comp = new TF2("daughter2_comp",&pt_above_cutoff,-M_PI/2.,M_PI/2.,0.,2*M_PI,5);
  daughter2_comp->SetParameters(mass,mother_pt,daughter2_pcm,daughter2_Ecm,daughter_pt_cutoff);
  double daughter2_frac = daughter2_comp->Integral(-M_PI/2.,M_PI/2.,0.,2*M_PI,1e-4) / (4.*M_PI);
  delete daughter2_comp;

  // both daughters must be reconstructible
  return daughter1_frac*daughter2_frac;
}

TF1* threshold_turnon_TF1(std::pair<double,double> mass_range, double mother_pt, double daughter_pt_cutoff, int daughter1_pdgid, int daughter2_pdgid)
{
  double daughter1_mass = TDatabasePDG::Instance()->GetParticle(daughter1_pdgid)->Mass();
  double daughter2_mass = TDatabasePDG::Instance()->GetParticle(daughter2_pdgid)->Mass();

  TF1* threshold_TF1 = new TF1("threshold_turnon",&kinematic_threshold_turnon,mass_range.first,mass_range.second,4);
  threshold_TF1->SetParameters(mother_pt,daughter_pt_cutoff,daughter1_mass,daughter2_mass);
  return threshold_TF1;
}

// These functions turned out to be quite slow, so here are lookup tables from which we can draw the relevant info

void build_turnon_lookup_tables(std::string mother_name, int daughter1_pdgid, int daughter2_pdgid)
{
  const double ptcut_min = 0.;
  const double ptcut_max = 0.5;
  const int nbins_ptcut = 100;

  HistogramInfo massbins = BinInfo::mass_bins.at(mother_name);
  const int nbins_mass = 100;
  const double mass_min = 0.9*massbins.bins[0];
  const double mass_max = 1.1*massbins.bins.back();

  const double daughter1_mass = TDatabasePDG::Instance()->GetParticle(daughter1_pdgid)->Mass();
  const double daughter2_mass = TDatabasePDG::Instance()->GetParticle(daughter2_pdgid)->Mass();

  std::string filename = "threshold_turnon_tables_"+mother_name+".root";
  TFile* f = new TFile(filename.c_str(),"RECREATE");
  // dummy, to get bin centers
  TH1F* h_pt = makeHistogram("","",BinInfo::final_pt_bins);
  for(int i=1;i<=h_pt->GetNbinsX();i++)
  {
    std::cout << "building threshold turnon lookup table for " << mother_name << " bin " << i << std::endl;
    double mother_pT = h_pt->GetBinLowEdge(i);
    std::string name = "threshold_bin"+std::to_string(i);
    std::string title = "Threshold turnon modification for mother pT = "+std::to_string(mother_pT);

    TH2F* threshold = new TH2F(name.c_str(),title.c_str(),nbins_mass,mass_min,mass_max,nbins_ptcut,ptcut_min,ptcut_max);

    for(int j=1;j<=threshold->GetNbinsX();j++)
    {
      double mass = threshold->GetXaxis()->GetBinCenter(j);
      for(int k=1;k<=threshold->GetNbinsY();k++)
      {
        std::cout << "sub-bin (" << j << ", " << k << ")" << std::endl;
        double ptcut = threshold->GetYaxis()->GetBinCenter(k);
        Double_t v[1] = {mass};
        Double_t par[4] = {mother_pT,ptcut,daughter1_mass,daughter2_mass};
        threshold->SetBinContent(j,k,kinematic_threshold_turnon(v,par));
      }
    }
    threshold->Write();
  }
  f->Close();
}

class LinearSidebandThresholdFast
{
  public:
  LinearSidebandThresholdFast(std::string mother_name, float daughter_pt_cut, int mother_pt_bin, std::pair<float,float> lsb, std::pair<float,float> rsb)
  {
    std::string filename = "threshold_turnon_tables_"+mother_name+"_daughterpT_"+std::to_string(daughter_pt_cut)+".root";
    tablefile = TFile::Open(filename.c_str());
    h_threshold = (TH1F*)tablefile->Get(("threshold_bin"+std::to_string(mother_pt_bin)).c_str());
    h_threshold->SetDirectory(nullptr);
    tablefile->Close();
    left_sideband = lsb;
    right_sideband = rsb;
  }
  double operator()(double* v, double* par)
  {
    double x = v[0];
    double slope = par[0];
    double c = par[1];
    double spectrum_cutoff = par[2];
    if(restrict_to_sidebands && (x<left_sideband.first || x>left_sideband.second) && (x<right_sideband.first || x>right_sideband.second))
    {
      TF1::RejectPoint();
    }
    return (slope*x+c) * h_threshold->Interpolate(x,spectrum_cutoff);
  }
  bool restrict_to_sidebands = true;
  private:
  TFile* tablefile;
  TH1F* h_threshold;
  std::pair<float,float> left_sideband;
  std::pair<float,float> right_sideband;
};
#endif
