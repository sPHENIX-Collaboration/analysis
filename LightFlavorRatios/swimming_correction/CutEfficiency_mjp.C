#include "../util/HistogramTools.h"
#include "../config/binning.h"
#include "../config/cuts.h"
#include "PVSVUncertainty.h"

TH1F* n_truth_mass = new TH1F("n_truth_mass","num. truth mass",1000,1.05,1.25);
TH1F* n_reco_mass = new TH1F("n_reco_mass","num. reco mass",1000,1.05,1.25);
TH1F* d_truth_mass = new TH1F("d_truth_mass","denom. truth mass",1000,0.35,0.65);
TH1F* d_reco_mass = new TH1F("d_reco_mass","denom. reco mass",1000,0.35,0.65);

// must have some permutation of the corect flavor of daughters
std::string correct_truth_daughters_cut(std::vector<int> daughters, bool include_opposite)
{
  std::vector<std::string> flavor_permutations;
  std::vector<std::string> opposite_flavor_permutations;

  std::vector<int> indices(daughters.size());
  std::iota(indices.begin(),indices.end(),0);

  do
  {
    std::string cut = "(";
    std::string opposite_parity_cut = "(";
    for(int i=0; i<indices.size(); i++)
    {
      cut += "track_"+std::to_string(i+1)+"_true_ID == "+std::to_string(daughters[indices[i]]);
      opposite_parity_cut += "track_"+std::to_string(i+1)+"_true_ID == "+std::to_string(-1*daughters[indices[i]]);
      if(i<indices.size()-1)
      {
        cut += " && ";
        opposite_parity_cut += " && ";
      }
    }
    cut += ")";
    opposite_parity_cut += ")";
    flavor_permutations.push_back(cut);
    opposite_flavor_permutations.push_back(opposite_parity_cut);
  } while(std::next_permutation(indices.begin(),indices.end()));

  std::string total_cut = "";
  for(int i=0;i<flavor_permutations.size();i++)
  {
    total_cut += flavor_permutations[i];
    if(include_opposite)
    {
      total_cut += " || "+opposite_flavor_permutations[i];
    }
    if(i<flavor_permutations.size()-1)
    {
      total_cut += " || ";
    }
  }
  return total_cut;
}

std::string correct_truth_parent_cut(int pdgid, int n_daughters, bool include_opposite)
{
  std::vector<std::string> daughter_cuts;
  std::vector<std::string> opposite_daughter_cuts;

  for(int i=1;i<=n_daughters;i++)
  {
    std::string cut = "Sum$(track_"+std::to_string(i)+"_true_track_history_PDG_ID == "+std::to_string(pdgid)+") > 0";
    std::string opposite_cut = "Sum$(track_"+std::to_string(i)+"_true_track_history_PDG_ID == "+std::to_string(-1*pdgid)+") > 0";
    daughter_cuts.push_back(cut);
    opposite_daughter_cuts.push_back(opposite_cut);
  }

  std::string total_cut = "(";
  for(int i=0;i<daughter_cuts.size();i++)
  {
    total_cut += daughter_cuts[i];
    if(i<daughter_cuts.size()-1)
    {
      total_cut += " && ";
    }
  }
  total_cut += ")";
  if(include_opposite)
  {
    total_cut += " || (";
    for(int i=0;i<daughter_cuts.size();i++)
    {
      total_cut += opposite_daughter_cuts[i];
      if(i<daughter_cuts.size()-1)
      {
        total_cut += " && ";
      }
    }
    total_cut += ")";
  }
  return total_cut;
}

std::string correct_daughter_PDGID_assignment_cut(int n_daughters)
{
  std::vector<std::string> daughter_cuts;
  for(int i=1;i<=n_daughters;i++)
  {
    std::string cut = "abs(track_"+std::to_string(i)+"_PDG_ID) == abs(track_"+std::to_string(i)+"_true_ID)";
    daughter_cuts.push_back(cut);
  }

  std::string total_cut = "";
  for(int i=0;i<n_daughters;i++)
  {
    total_cut += daughter_cuts[i];
    if(i<n_daughters-1)
    {
      total_cut += " && ";
    }
  }
  return total_cut;
}

// "primary parent" means that no other hadron/lepton is encountered in the post-hadronization PDGID history
// in other words, the mother is the first hadron in the post-hadronization PDGID history
std::pair<int,int> posthadronization_history_info(std::vector<int>* pdgid_history)
{
  int n_entries = 0;
  int primary_parent = 0;
  for(int i=0;i<pdgid_history->size();i++)
  {
    const int pdgid = pdgid_history->at(i);
    // "nontrivial" here means "meson, baryon, or lepton"
    const std::string particle_class = TDatabasePDG::Instance()->GetParticle(pdgid)->ParticleClass();
    if(particle_class == "Meson" || particle_class == "Baryon" || particle_class == "Lepton")
    {
      n_entries++;
      primary_parent = pdgid;
    }
    else
    {
      break;
    }
  }
  return {n_entries,primary_parent};
}

std::string full_truth_cut(const std::string& name, const int pdgid, const std::vector<int> daughters, const bool include_opposite)
{
  std::string correct_truth_daughters = correct_truth_daughters_cut(daughters,include_opposite);
  std::string correct_truth_parent = correct_truth_parent_cut(pdgid,daughters.size(),include_opposite);
  std::string same_truth_parent = "fabs(track_1_true_track_history_pz[0] - track_2_true_track_history_pz[0])<1e-6";
  std::string rapidity_cut = "fabs("+name+"_rapidity)<1.";
  std::string geoaccept_cut = "track_1_pT>0.2 && track_2_pT>0.2 && track_1_MVTX_nHits>0 && track_2_MVTX_nHits>0 && fabs(primary_vertex_z)<10.";
  std::string correct_daughter_pdgid = correct_daughter_PDGID_assignment_cut(daughters.size());

  return "("+correct_truth_daughters+") && ("+correct_truth_parent+") && ("+same_truth_parent+") && ("+geoaccept_cut+") && ("+correct_daughter_pdgid+") && ("+rapidity_cut+") && ("+generate_fiducial_cuts(name,{BinInfo::final_pt_bins,BinInfo::final_eta_bins,BinInfo::final_phi_bins,BinInfo::final_rapidity_bins})+")";
}

std::string full_reco_cut(const std::string& name, const int pdgid, const std::vector<int> daughters, const bool include_opposite, const CutSettings& cuts, const std::vector<HistogramInfo>& variables, const HistogramInfo& massbins)
{
  std::string truth_cut = full_truth_cut(name,pdgid,daughters,include_opposite);
  std::string reco_cuts = generate_selection_cutstring(cuts,variables);
  // for sideband-subtraction yield extraction, must also include the restriction of the yield to the signal window
  std::string mass_window_cuts = name+"_mass>"+std::to_string(massbins.bins.front())+" && "+name+"_mass<"+std::to_string(massbins.bins.back());
  std::string correct_daughter_pdgid = correct_daughter_PDGID_assignment_cut(daughters.size());
  return truth_cut+" && "+reco_cuts+" && "+correct_daughter_pdgid+" && "+mass_window_cuts;
}

void post_draw_process(TTree* t, TH1F* h, const HistogramInfo& var, const std::string& name, TEntryList* elist, const int pdgid, const int n_daughters, TH1F* h_mass, PVSVUncertainty& pvsv, bool fill_aux_histograms = false)
{
  t->ResetBranchAddresses();
  t->SetBranchStatus("*",true);
  std::vector<std::vector<int>*> daughter_pdgid_histories;
  std::vector<float> daughter_PV_DCA_err;
  std::vector<std::array<float,21>> daughter_covariance;
  daughter_pdgid_histories.resize(n_daughters);
  daughter_PV_DCA_err.resize(n_daughters);
  daughter_covariance.resize(n_daughters);
  float var_branch;
  float mass;
  float mother_vertex_volume;
  float primary_vertex_volume;
  float primary_vertex_covariance[6];

  for(int i=0;i<n_daughters;i++)
  {
    daughter_pdgid_histories[i] = nullptr;
    t->SetBranchAddress(("track_"+std::to_string(i+1)+"_true_track_history_PDG_ID").c_str(),&daughter_pdgid_histories[i]);
    t->SetBranchAddress(("track_"+std::to_string(i+1)+"_PV_DCA_Err").c_str(),&daughter_PV_DCA_err[i]);
    t->SetBranchAddress(("track_"+std::to_string(i+1)+"_Covariance").c_str(),daughter_covariance[i].data());
  }
  t->SetBranchAddress((name+"_"+var.name).c_str(),&var_branch);
  t->SetBranchAddress((name+"_mass").c_str(),&mass);
  t->SetBranchAddress((name+"_vertex_volume").c_str(),&mother_vertex_volume);
  t->SetBranchAddress("primary_vertex_volume",&primary_vertex_volume);
  t->SetBranchAddress("primary_vertex_Covariance",primary_vertex_covariance);

  int npassing = 0;

  //t->SetEntryList(elist);
  for(size_t e=0;e<elist->GetN();e++)
  {
    //std::cout << elist->GetEntry(e) << std::endl;
    t->GetEntry(elist->GetEntry(e));
    
    // special case for Kshorts
    if(pdgid==310)
    {
      std::pair<int,int> daughter1_historyinfo = posthadronization_history_info(daughter_pdgid_histories[0]);
      std::pair<int,int> daughter2_historyinfo = posthadronization_history_info(daughter_pdgid_histories[1]);
      bool primary_Kshort = (daughter_pdgid_histories[0]->at(0) == 310 && daughter_pdgid_histories[1]->at(0) == 310 && daughter1_historyinfo.first == 1 && daughter2_historyinfo.first == 1);
      bool primary_K0 = daughter_pdgid_histories[0]->at(0) == 310 && daughter_pdgid_histories[1]->at(0) == 310 && daughter1_historyinfo.first == 2 && daughter2_historyinfo.first == 2 && ((daughter1_historyinfo.second == 311 && daughter2_historyinfo.second == 311) || (daughter1_historyinfo.second == -311 && daughter2_historyinfo.second == -311));
      if(primary_Kshort || primary_K0)
      {
        h->Fill(var_branch);
        if(fill_aux_histograms)
        {
          h_mass->Fill(mass);
          pvsv.PV_sigma->Fill(pow(primary_vertex_volume,1./3.));
          pvsv.PV_sigma_xy->Fill(sqrt(primary_vertex_covariance[0]+primary_vertex_covariance[2]-2.*primary_vertex_covariance[1]));
          pvsv.SV_sigma->Fill(pow(mother_vertex_volume,1./3.));
          pvsv.track_1_PV_DCA_sigma->Fill(daughter_PV_DCA_err[0]);
          pvsv.track_2_PV_DCA_sigma->Fill(daughter_PV_DCA_err[1]);
          pvsv.track_1_PV_DCA_xy_sigma->Fill(sqrt(daughter_covariance[0][0]+daughter_covariance[0][2]-2.*daughter_covariance[0][1]));
          pvsv.track_2_PV_DCA_xy_sigma->Fill(sqrt(daughter_covariance[1][0]+daughter_covariance[1][2]-2.*daughter_covariance[1][1]));
        }
        npassing++;
      }
      else
      {
        std::cout << "non-passing entry: " << elist->GetEntry(e) << std::endl;
      }
    }
    else
    {
      bool all_daughters_primary_parents = std::all_of(daughter_pdgid_histories.begin(),daughter_pdgid_histories.end(),
             [&](std::vector<int>* history){return (abs(history->at(0))==abs(pdgid) && posthadronization_history_info(history).first==1);});
      if(all_daughters_primary_parents)
      {
        h->Fill(var_branch);
        if(fill_aux_histograms)
        {
          h_mass->Fill(mass);
          pvsv.PV_sigma->Fill(pow(primary_vertex_volume,1./3.));
          pvsv.PV_sigma_xy->Fill(sqrt(primary_vertex_covariance[0]+primary_vertex_covariance[2]-2.*primary_vertex_covariance[1]));
          pvsv.SV_sigma->Fill(pow(mother_vertex_volume,1./3.));
          pvsv.track_1_PV_DCA_sigma->Fill(daughter_PV_DCA_err[0]);
          pvsv.track_2_PV_DCA_sigma->Fill(daughter_PV_DCA_err[1]);
          pvsv.track_1_PV_DCA_xy_sigma->Fill(sqrt(daughter_covariance[0][0]+daughter_covariance[0][2]-2.*daughter_covariance[0][1]));
          pvsv.track_2_PV_DCA_xy_sigma->Fill(sqrt(daughter_covariance[1][0]+daughter_covariance[1][2]-2.*daughter_covariance[1][1]));
        }
        npassing++;
      }
    }
  }
  std::cout << "npassing: " << npassing << std::endl;
}

void CutEfficiency_mjp(const std::string& numerator_name = "Lambda0",
                       const int numerator_pdgid = 3122,
                       const std::vector<int> numerator_daughters = {-211,2212},
                       const std::string& numerator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/ppi_reco/outputKFParticle_ppi_reco_000001.root",
                       const bool numerator_include_opposite = true,
                       const CutSettings& numerator_cuts = StandardCuts::MC_Lambda0_cuts, 
                       const HistogramInfo& numerator_massbins = BinInfo::Lambda0_MC_mass_bins,
                       const std::string& denominator_name = "K_S0",
                       const int denominator_pdgid = 310,
                       const std::vector<int> denominator_daughters = {211, -211},
                       const std::string& denominator_infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/pipi_reco/outputKFParticle_pipi_reco_000001.root",
                       const bool denominator_include_opposite = false,
                       const CutSettings& denominator_cuts = StandardCuts::MC_K_S0_cuts, 
                       const HistogramInfo& denominator_massbins = BinInfo::K_S0_MC_mass_bins,
                       const std::string& outfile = "test.root")
{
  bool verbose = true;

  TFile* f_numerator = TFile::Open(numerator_infile.c_str());
  TFile* f_denominator = TFile::Open(denominator_infile.c_str());

  TTree* t_numerator = (TTree*)f_numerator->Get("DecayTree");
  TTree* t_denominator = (TTree*)f_denominator->Get("DecayTree");

  std::cout << "total entries: " << t_numerator->GetEntries() << " " << t_denominator->GetEntries() << std::endl;

  std::vector<HistogramInfo> variables =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_phi_bins,
    BinInfo::final_rapidity_bins
  };

  if(verbose)
  {
    std::string numerator_correct_truth_daughters = correct_truth_daughters_cut(numerator_daughters,numerator_include_opposite);
    std::string denominator_correct_truth_daughters = correct_truth_daughters_cut(denominator_daughters,denominator_include_opposite);

    std::cout << "correct truth daughter cuts:" << std::endl
     << "numerator:" << std::endl
     << numerator_correct_truth_daughters << std::endl
     << "denominator:" << std::endl
     << denominator_correct_truth_daughters << std::endl;

    std::string numerator_correct_truth_parent = correct_truth_parent_cut(numerator_pdgid,numerator_daughters.size(),numerator_include_opposite);
    std::string denominator_correct_truth_parent = correct_truth_parent_cut(denominator_pdgid,denominator_daughters.size(),denominator_include_opposite);

    std::cout << "correct truth parent cuts:" << std::endl
     << "numerator:" << std::endl
     << numerator_correct_truth_parent << std::endl
     << "denominator:" << std::endl
     << denominator_correct_truth_parent << std::endl;
/*
    std::string numerator_primary_truth_parent = primary_truth_parent_cut(numerator_pdgid,numerator_daughters.size(),numerator_include_opposite);
    std::string denominator_primary_truth_parent = primary_truth_parent_cut(denominator_pdgid,denominator_daughters.size(),denominator_include_opposite);

    std::cout << "primary truth parent cuts:" << std::endl
     << "numerator:" << std::endl
     << numerator_primary_truth_parent << std::endl
     << "denominator:" << std::endl
     << denominator_primary_truth_parent << std::endl;
*/
  }

  std::string numerator_truth_cut = full_truth_cut(numerator_name,numerator_pdgid,numerator_daughters,numerator_include_opposite);
  std::string numerator_reco_cut = full_reco_cut(numerator_name,numerator_pdgid,numerator_daughters,numerator_include_opposite,numerator_cuts,variables,numerator_massbins);

  std::string denominator_truth_cut = full_truth_cut(denominator_name,denominator_pdgid,denominator_daughters,denominator_include_opposite);
  std::string denominator_reco_cut = full_reco_cut(denominator_name,denominator_pdgid,denominator_daughters,denominator_include_opposite,denominator_cuts,variables,denominator_massbins);

  if(verbose)
  {
    std::cout << "full cuts:" << std::endl
     << numerator_name << " truth:" << std::endl
     << numerator_truth_cut << std::endl
     << numerator_name << " reco:" << std::endl
     << numerator_reco_cut << std::endl
     << denominator_name << " truth:" << std::endl
     << denominator_truth_cut << std::endl
     << denominator_name << " reco:" << std::endl
     << denominator_reco_cut << std::endl;
  }

  TFile* fout = new TFile(outfile.c_str(),"RECREATE");
/*
  TEntryList* numerator_truth_elist = new TEntryList("numerator_truth_elist","");
  TEntryList* numerator_reco_elist = new TEntryList("numerator_reco_elist","");
  TEntryList* denominator_truth_elist = new TEntryList("denominator_truth_elist","");
  TEntryList* denominator_reco_elist = new TEntryList("denominator_reco_elist","");
*/
  t_numerator->Draw(">>numerator_truth_elist",numerator_truth_cut.c_str(),"entrylist goff");
  t_numerator->Draw(">>numerator_reco_elist",numerator_reco_cut.c_str(),"entrylist goff");
  t_denominator->Draw(">>denominator_truth_elist",denominator_truth_cut.c_str(),"entrylist goff");
  t_denominator->Draw(">>denominator_reco_elist",denominator_reco_cut.c_str(),"entrylist goff");

  TEntryList* numerator_truth_elist = (TEntryList*)gDirectory->Get("numerator_truth_elist");
  TEntryList* numerator_reco_elist = (TEntryList*)gDirectory->Get("numerator_reco_elist");
  TEntryList* denominator_truth_elist = (TEntryList*)gDirectory->Get("denominator_truth_elist");
  TEntryList* denominator_reco_elist = (TEntryList*)gDirectory->Get("denominator_reco_elist");

  std::cout << "pre-primary selection: " << numerator_truth_elist->GetN() << " " << numerator_reco_elist->GetN() << " " << denominator_truth_elist->GetN() << " " << denominator_reco_elist->GetN() << std::endl;

  TH1F* n_truth_mass = new TH1F("n_truth_mass","numerator truth candidate mass",numerator_massbins.bins.size()-1,numerator_massbins.bins.data());
  TH1F* n_reco_mass = new TH1F("n_reco_mass","numerator reco candidate mass",numerator_massbins.bins.size()-1,numerator_massbins.bins.data());
  TH1F* d_truth_mass = new TH1F("d_truth_mass","denominator truth candidate mass",denominator_massbins.bins.size()-1,denominator_massbins.bins.data());
  TH1F* d_reco_mass = new TH1F("d_reco_mass","denominator reco candidate mass",denominator_massbins.bins.size()-1,denominator_massbins.bins.data());

  PVSVUncertainty n_truth_pvsv(numerator_name+"_truth",numerator_name+" truth",300,0.,0.05);
  PVSVUncertainty n_reco_pvsv(numerator_name+"_reco",numerator_name+" reco",300,0.,0.05);
  PVSVUncertainty d_truth_pvsv(denominator_name+"_truth",denominator_name+" truth",300,0.,0.05);
  PVSVUncertainty d_reco_pvsv(denominator_name+"_reco",denominator_name+" reco",300,0.,0.05);

  for(int ivar=0; ivar<variables.size(); ivar++)
  {
    HistogramInfo& var = variables[ivar];

    TH1F* numerator_truth_h = makeHistogram((numerator_name+"_truth").c_str(),(numerator_name+" truth candidates").c_str(),var);
    TH1F* numerator_reco_h = makeHistogram((numerator_name+"_reco").c_str(),(numerator_name+" reco candidates").c_str(),var);
    TH1F* denominator_truth_h = makeHistogram((denominator_name+"_truth").c_str(),(denominator_name+" truth candidates").c_str(),var);
    TH1F* denominator_reco_h = makeHistogram((denominator_name+"_reco").c_str(),(denominator_name+" reco candidates").c_str(),var);

    numerator_truth_h->Sumw2();
    numerator_reco_h->Sumw2();
    denominator_truth_h->Sumw2();
    denominator_reco_h->Sumw2();

    // only fill auxiliary histograms once
    bool fill_aux_h = (ivar == 0);

    post_draw_process(t_numerator,numerator_truth_h,var,numerator_name,numerator_truth_elist,numerator_pdgid,numerator_daughters.size(),n_truth_mass,n_truth_pvsv,fill_aux_h);
    post_draw_process(t_numerator,numerator_reco_h,var,numerator_name,numerator_reco_elist,numerator_pdgid,numerator_daughters.size(),n_reco_mass,n_reco_pvsv,fill_aux_h);
    post_draw_process(t_denominator,denominator_truth_h,var,denominator_name,denominator_truth_elist,denominator_pdgid,denominator_daughters.size(),d_truth_mass,d_truth_pvsv,fill_aux_h);
    post_draw_process(t_denominator,denominator_reco_h,var,denominator_name,denominator_reco_elist,denominator_pdgid,denominator_daughters.size(),d_reco_mass,d_reco_pvsv,fill_aux_h);

    if(verbose)
    {
      std::cout << var.name << ":" << std::endl;
      std::cout << "numerator: passed " << numerator_reco_h->GetEntries() << " / " << numerator_truth_h->GetEntries() << std::endl;
      std::cout << "denominator: passed " << denominator_reco_h->GetEntries() << " / " << denominator_truth_h->GetEntries() << std::endl;
    }

    numerator_truth_h->Write();
    numerator_reco_h->Write();
    denominator_truth_h->Write();
    denominator_reco_h->Write();
  }

  n_truth_mass->Write();
  n_reco_mass->Write();
  d_truth_mass->Write();
  d_reco_mass->Write();

  n_truth_pvsv.Write();
  n_reco_pvsv.Write();
  d_truth_pvsv.Write();
  d_reco_pvsv.Write();
}
