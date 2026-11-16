#include <phool/PHRandomSeed.h>
#include <gsl/gsl_rng.h>

namespace Settings
{

  const std::string mother_name = "#Lambda";
  const float mother_mass = 1.115;

  const std::string daughter1_name = "proton";
  const float daughter1_mass = .938;

  const std::string daughter2_name = "#pi";
  const float daughter2_mass = .1395;

/*
  const std::string mother_name = "K_{S}^{0}";
  const float mother_mass = 0.4976;

  const std::string daughter1_name = "#pi^{+}";
  const float daughter1_mass = .1395;

  const std::string daughter2_name = "#pi^{-}";
  const float daughter2_mass = .1395;
*/

  const float mother_pt_low = 0.;
  const float mother_pt_high = 4.;
  const int mother_pt_nbins = 400;

  const float daughter_pt_low = 0.;
  const float daughter_pt_high = 4.;
  const int daughter_pt_nbins = 400;

  const int n_mother_samples = 200;
  const int n_daughter_samples = 200;

  const float min_daughter_pt = 0.160;

  const bool debug = false;
};

float get_daughter_pt_CM(const float mother_mass, const float daughter1_mass, const float daughter2_mass)
{
  const float daughter1_gamma_CM = (pow(mother_mass,2.)+pow(daughter1_mass,2.)-pow(daughter2_mass,2.))/(2.*mother_mass*daughter1_mass);
  return sqrt(pow(daughter1_gamma_CM,2.)-1)*daughter1_mass;
}

std::string bin_name(const int bin_number)
{
  const float bin_pt_low = Settings::mother_pt_low+((float)bin_number)/Settings::mother_pt_nbins * (Settings::mother_pt_high-Settings::mother_pt_low);
  const float bin_pt_high = bin_pt_low + (1./Settings::mother_pt_nbins) * (Settings::mother_pt_high-Settings::mother_pt_low);
  return std::to_string(bin_pt_low) + " <= mother pT <= " + std::to_string(bin_pt_high);
}

void CalculateKinematicAcceptance()
{
  TFile* fout = new TFile("kinematic_acceptance.root","RECREATE");

  TTree* outtree = new TTree("candidates","Candidates");

  int mother_ptbin;
  float mother_mass = Settings::mother_mass;
  float mother_pt;
  float mother_gamma;
  float mother_beta;
  float mother_E;

  float theta_xy;
  float theta_rz;
  float daughter_p_CM;
  float daughter_px_CM;
  float daughter_py_CM;
  float daughter_pz_CM;

  float daughter1_mass = Settings::daughter1_mass;
  float daughter1_gamma_CM;
  float daughter1_E_CM;
  float daughter1_px_lab;
  float daughter1_py_lab;
  float daughter1_pt_lab;

  float daughter2_mass = Settings::daughter2_mass;
  float daughter2_gamma_CM;
  float daughter2_E_CM;
  float daughter2_px_lab;
  float daughter2_py_lab;
  float daughter2_pt_lab;

  outtree->Branch("mother_ptbin",&mother_ptbin);
  outtree->Branch("mother_mass",&mother_mass);
  outtree->Branch("mother_pt",&mother_pt);
  outtree->Branch("mother_gamma",&mother_gamma);
  outtree->Branch("mother_beta",&mother_beta);
  outtree->Branch("mother_E",&mother_E);

  outtree->Branch("theta_xy",&theta_xy);
  outtree->Branch("theta_rz",&theta_rz);
  outtree->Branch("daughter_p_CM",&daughter_p_CM);
  outtree->Branch("daughter_px_CM",&daughter_px_CM);
  outtree->Branch("daughter_py_CM",&daughter_py_CM);

  outtree->Branch("daughter1_mass",&daughter1_mass);
  outtree->Branch("daughter1_gamma_CM",&daughter1_gamma_CM);
  outtree->Branch("daughter1_E_CM",&daughter1_E_CM);
  outtree->Branch("daughter1_px_lab",&daughter1_px_lab);
  outtree->Branch("daughter1_py_lab",&daughter1_py_lab);
  outtree->Branch("daughter1_pt_lab",&daughter1_pt_lab);

  outtree->Branch("daughter2_mass",&daughter2_mass);
  outtree->Branch("daughter2_gamma_CM",&daughter2_gamma_CM);
  outtree->Branch("daughter2_E_CM",&daughter2_E_CM);
  outtree->Branch("daughter2_px_lab",&daughter2_px_lab);
  outtree->Branch("daughter2_py_lab",&daughter2_py_lab);
  outtree->Branch("daughter2_pt_lab",&daughter2_pt_lab);

  TH1F* acceptance = new TH1F("acceptance",(Settings::mother_name+" kinematic acceptance;"+Settings::mother_name+" pT [GeV];acceptance").c_str(),
                              Settings::mother_pt_nbins,Settings::mother_pt_low,Settings::mother_pt_high);
  TH1F* pt1 = new TH1F("pt1",(Settings::daughter1_name+" pT").c_str(),
                              Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high);
  TH1F* pt2 = new TH1F("pt2",(Settings::daughter2_name+" pT").c_str(),
                              Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high);
  TH2F* pt1pt2 = new TH2F("pt1pt2",("("+Settings::daughter1_name+", "+Settings::daughter2_name+") pT").c_str(),
                              Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high,
                              Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high);

  std::vector<TH1F*> pt1_vs_motherpt;
  std::vector<TH1F*> pt2_vs_motherpt;
  std::vector<TH2F*> pt1pt2_vs_motherpt;
  
  for(int i=0; i<Settings::mother_pt_nbins; i++)
  {
    std::string name_suffix = "_bin"+std::to_string(i);
    std::string title_suffix = bin_name(i);
    pt1_vs_motherpt.push_back(new TH1F(("pt1_vs_motherpt"+name_suffix).c_str(),(Settings::daughter1_name+" pT, "+title_suffix).c_str(),
                                        Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high));
    pt2_vs_motherpt.push_back(new TH1F(("pt2_vs_motherpt"+name_suffix).c_str(),(Settings::daughter2_name+" pT, "+title_suffix).c_str(),
                                        Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high));
    pt1pt2_vs_motherpt.push_back(new TH2F(("pt1pt2_vs_motherpt"+name_suffix).c_str(),("("+Settings::daughter1_name+","+Settings::daughter2_name+") pT, "+title_suffix).c_str(),
                                        Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high,
                                        Settings::daughter_pt_nbins,Settings::daughter_pt_low,Settings::daughter_pt_high));
  }

  const uint seed = PHRandomSeed();
  std::unique_ptr<gsl_rng> m_rng;
  m_rng.reset(gsl_rng_alloc(gsl_rng_mt19937));
  gsl_rng_set(m_rng.get(), seed);

  for(int impt = 0; impt<Settings::mother_pt_nbins; impt++)
  {
    mother_ptbin = impt;

    const float bin_pt_low = acceptance->GetBinLowEdge(impt+1);
    const float bin_pt_high = bin_pt_low + acceptance->GetBinWidth(impt+1);

    std::cout << "mother pt bin " << impt << std::endl;

    for(int ims=0; ims<Settings::n_mother_samples; ims++)
    {
      mother_pt = bin_pt_low + (bin_pt_high-bin_pt_low) * gsl_rng_uniform_pos(m_rng.get());

      mother_gamma = sqrt(1.+pow(mother_pt/mother_mass,2.));
      mother_beta = mother_pt/(mother_gamma*mother_mass);
      mother_E = mother_gamma*mother_mass;

      if(Settings::debug)
      {
        std::cout << "pt in (" << bin_pt_low << ", " << bin_pt_high << ")" << std::endl;
        std::cout << "mother pt " << mother_pt << " E " << mother_E << " gamma " << mother_gamma << " beta " << mother_beta << std::endl;
      }

      for(int is=0; is<Settings::n_daughter_samples; is++)
      {
        // pick random decay axis (equivalent to random point on sphere) in a way that respects angle dependence of solid-angle element
        theta_xy = 2.*M_PI*gsl_rng_uniform_pos(m_rng.get());
        theta_rz = acos(2*gsl_rng_uniform_pos(m_rng.get())-1.);

        daughter1_gamma_CM = (pow(Settings::mother_mass,2.)+pow(Settings::daughter1_mass,2.)-pow(Settings::daughter2_mass,2.))/(2.*Settings::mother_mass*Settings::daughter1_mass);
        daughter2_gamma_CM = (pow(Settings::mother_mass,2.)+pow(Settings::daughter2_mass,2.)-pow(Settings::daughter1_mass,2.))/(2.*Settings::mother_mass*Settings::daughter2_mass);

        daughter1_E_CM = daughter1_gamma_CM * Settings::daughter1_mass;
        daughter2_E_CM = daughter2_gamma_CM * Settings::daughter2_mass;

        daughter_p_CM = sqrt(pow(daughter1_gamma_CM,2.)-1.) * Settings::daughter1_mass;
        daughter_px_CM = daughter_p_CM * cos(theta_xy) * cos(theta_rz);
        daughter_py_CM = daughter_p_CM * sin(theta_xy) * cos(theta_rz);
        daughter_pz_CM = daughter_p_CM * sin(theta_rz);

        if(gsl_rng_uniform_pos(m_rng.get())>0.5)
        {
          daughter1_px_lab = mother_gamma * (daughter_px_CM + mother_beta*daughter1_E_CM);
          daughter2_px_lab = mother_gamma * (-daughter_px_CM + mother_beta*daughter2_E_CM);

          daughter1_py_lab = daughter_py_CM;
          daughter2_py_lab = -daughter_py_CM;
        }
        else
        {
          daughter1_px_lab = mother_gamma * (-daughter_px_CM + mother_beta*daughter1_E_CM);
          daughter2_px_lab = mother_gamma * (daughter_px_CM + mother_beta*daughter2_E_CM);

          daughter1_py_lab = -daughter_py_CM;
          daughter2_py_lab = daughter_py_CM;
        }

        daughter1_pt_lab = sqrt(pow(daughter1_px_lab,2.)+pow(daughter_py_CM,2.));
        daughter2_pt_lab = sqrt(pow(daughter2_px_lab,2.)+pow(daughter_py_CM,2.));

        if(Settings::debug)
        {
          std::cout << "sample " << is << std::endl;
          std::cout << "theta_xy " << theta_xy << " theta_rz " << theta_rz << std::endl;
          std::cout << Settings::daughter1_name << " gammaCM " << daughter1_gamma_CM << " E_CM " << daughter1_E_CM << " p_CM " << daughter_p_CM << " pt_lab " << daughter1_pt_lab << std::endl;
          std::cout << Settings::daughter2_name << " gammaCM " << daughter2_gamma_CM << " E_CM " << daughter2_E_CM << " p_CM " << daughter_p_CM << " pt_lab " << daughter2_pt_lab << std::endl;
        }

        outtree->Fill();

        pt1->Fill(daughter1_pt_lab);
        pt2->Fill(daughter2_pt_lab);
        pt1pt2->Fill(daughter1_pt_lab,daughter2_pt_lab);

        pt1_vs_motherpt[impt]->Fill(daughter1_pt_lab);
        pt2_vs_motherpt[impt]->Fill(daughter2_pt_lab);
        pt1pt2_vs_motherpt[impt]->Fill(daughter1_pt_lab,daughter2_pt_lab);

        if(daughter1_pt_lab>Settings::min_daughter_pt && daughter2_pt_lab>Settings::min_daughter_pt)
        {
          acceptance->Fill(mother_pt);
        }
      }
    }
  }
  acceptance->Scale(1./(Settings::n_mother_samples*Settings::n_daughter_samples));
  acceptance->SetMinimum(0.);

  outtree->Write();

  acceptance->Write();
  pt1->Write();
  pt2->Write();
  pt1pt2->Write();
  for(int i=0; i<Settings::mother_pt_nbins; i++)
  {
    pt1_vs_motherpt[i]->Write();
    pt2_vs_motherpt[i]->Write();
    pt1pt2_vs_motherpt[i]->Write();
  }
}
