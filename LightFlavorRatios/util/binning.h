#ifndef BINNING_H
#define BINNING_H

#include "TH1F.h"
#include "TH2F.h"

#include <map>
#include <regex>

std::vector<std::string> cutvars_from_cutstring(std::string cutstring)
{
  // all possible tokens dividing cut variables
  std::regex dividers("[&|!<=>]+");
  // strip whitespace
  cutstring.erase(std::remove_if(cutstring.begin(),cutstring.end(),::isspace),cutstring.end());
  std::vector<std::string> statement_list(std::sregex_token_iterator(cutstring.begin(),cutstring.end(),dividers,-1),
                                          std::sregex_token_iterator());
  std::vector<std::string> cutvars;
  for(std::string statement : statement_list)
  {
    // is this a numeric value?
    bool is_numeric;
    try
    {
      size_t nparsed;
      double val = std::stod(statement,&nparsed);
      is_numeric = (nparsed == statement.size());
    }
    catch(const std::invalid_argument&)
    {
      is_numeric = false;
    }
    catch(const std::out_of_range&)
    {
      is_numeric = false;
    }
    // cut variables are the non-numeric strings
    if(!is_numeric)
    {
      cutvars.push_back(statement);
    } 
  }

  return cutvars;
}

struct HistogramInfo
{
  std::string name;
  std::string title;
  std::string axis_label;
  std::vector<double> bins;
  std::string cut_string;

  HistogramInfo(const std::string& hname, const std::string& htitle, const std::vector<double>& hbins,
                const std::string& haxislabel = "", const std::string& hcutstring = "")
  : name(hname), title(htitle), bins(hbins), axis_label(haxislabel), cut_string(hcutstring)
  {
  }

  // constructor for uniform binning
  HistogramInfo(const std::string& hname, const std::string& htitle, const int nBins, const float xmin, const float xmax, 
                const std::string& haxislabel = "", const std::string& hcutstring = "")
  : name(hname), title(htitle), axis_label(haxislabel), cut_string(hcutstring)
  {
    const float interval = (xmax-xmin)/nBins;
    for(int i=0; i<=nBins; i++)
    {
      bins.push_back(xmin+i*interval);
    }
  }

  std::string get_bin_selection(std::string var, int bin) const
  {
    if(bin==0) // underflow
    {
      return var+"<"+std::to_string(bins[0]);
    }
    else if(bin<bins.size()) // Nbins = bins.size()-1
    {
      return var+">="+std::to_string(bins[bin-1])+"&&"+var+"<"+std::to_string(bins[bin]);
    }
    else if(bin==bins.size()) // overflow
    {
      return var+">="+std::to_string(bins[bins.size()-1]);
    }
    else
    {
      return "";
    }
  }

  std::vector<std::string> get_cutvars(TTree* t)
  {
    std::vector<std::string> cut_vars;

    TObjArray* branches = t->GetListOfBranches();
    size_t nbranches = t->GetNbranches();
    for(size_t i=0;i<nbranches;i++)
    {
      std::string branchname = branches->At(i)->GetName();
      if(cut_string.find(branchname) != std::string::npos)
      {
        cut_vars.push_back(branchname);
      }
    }

    return cut_vars;
  }
};

bool isIntBranch(TBranch* b)
{
  TClass* c;
  EDataType type;
  b->GetExpectedType(c,type);
  std::vector<EDataType> integral_datatypes = {
    kInt_t,
    kUInt_t,
    kChar_t,
    kUChar_t,
    kShort_t,
    kUShort_t,
    kLong_t,
    kULong_t,
    kLong64_t,
    kULong64_t
  };
  return std::find(integral_datatypes.begin(),integral_datatypes.end(),type)!=integral_datatypes.end();
}

int findBin(float val, std::vector<double> bins)
{
  int thisbin = -1;
  for(int i=0; i<bins.size(); i++)
  {
    if(val<bins[i])
    {
      thisbin = i;
      break;
    }
  }
  // if val is not < any bin edge, it belongs in last bin
  if(thisbin == -1)
  {
    thisbin = bins.size();
  }
  return thisbin;
}

std::vector<double> makeLogBins(const int nBins, const float xmin, const float xmax)
{
  std::vector<double> bins;
  const float log_interval = (log(xmax)-log(xmin))/nBins;
  for(int i=0; i<=nBins; i++)
  {
    bins.push_back(exp(log(xmin) + i*log_interval));
  }
  return bins;
}

TH1F* makeHistogram(const HistogramInfo& hinfo)
{
  TH1F* h = new TH1F(hinfo.name.c_str(),(hinfo.title+";"+hinfo.axis_label+";Candidates").c_str(),hinfo.bins.size()-1,hinfo.bins.data());
  return h;
}

TH1F* makeHistogram(const std::string& basename, const std::string& basetitle, const HistogramInfo& hinfo)
{
  TH1F* h = new TH1F((basename+"_vs"+hinfo.name).c_str(),(basetitle+" vs. "+hinfo.title+";"+hinfo.axis_label+";Candidates").c_str(),hinfo.bins.size()-1,hinfo.bins.data());
  return h;
}

TH2F* make2DHistogram(const std::string& basename, const std::string& basetitle, const HistogramInfo& hinfo_x, const HistogramInfo& hinfo_y)
{
  const std::string name = basename + hinfo_x.name + "_vs" + hinfo_y.name;
  const std::string title = basetitle + " " + hinfo_x.title + " vs. " + hinfo_y.title + ";" + hinfo_x.axis_label + ";" + hinfo_y.axis_label;
  TH2F* h = new TH2F(name.c_str(),title.c_str(),hinfo_x.bins.size()-1,hinfo_x.bins.data(),hinfo_y.bins.size()-1,hinfo_y.bins.data());
  return h;
}

std::vector<TH1F*> makeDifferentialHistograms(const HistogramInfo& hinfo_x, const HistogramInfo& hinfo_y)
{
  std::vector<TH1F*> h_out;
  for(int i=0; i<=hinfo_y.bins.size(); i++)
  {
    const std::string name = hinfo_x.name + "_vs" + hinfo_y.name + "_" + std::to_string(i);
    const std::string title = hinfo_x.title + " vs. " + hinfo_y.title + 
      " (bin " + std::to_string(i) + ", " + hinfo_y.get_bin_selection(hinfo_y.name,i) + ");" + hinfo_x.axis_label;
    h_out.push_back(new TH1F(name.c_str(),title.c_str(),hinfo_x.bins.size()-1,hinfo_x.bins.data()));
  }
  return h_out;
}


namespace BinInfo
{
  // applied to all daughter tracks
  static const std::string general_daughter_track_cuts = //"track_1_bunch_crossing > 0 && track_1_bunch_crossing < 360 && "
                                                         //"track_2_bunch_crossing > 0 && track_2_bunch_crossing < 360 && "
                                                         "track_1_pT>0.2 && track_2_pT>0.2";

  std::string fiducial_cuts(const std::string& particle_name,const std::vector<HistogramInfo> variables)
  {
    std::string cut;
    for(int i=0;i<variables.size();i++)
    {
      cut += particle_name+"_"+variables[i].name+">="+std::to_string(variables[i].bins.front())
             +" && "+particle_name+"_"+variables[i].name+"<="+std::to_string(variables[i].bins.back());
      if(i<variables.size()-1) cut += " && ";
    }
    return cut;
  }

  // replaces KFParticle selection cuts for MC sample
  static const std::string MC_daughter_selection_cuts = "track_1_MVTX_nHits > 0 && track_2_MVTX_nHits > 0 && abs(primary_vertex_z) < 10 && track_1_INTT_nHits > 0 && track_2_INTT_nHits > 0 && track_1_TPC_nHits > 19 && track_2_TPC_nHits > 19 && (track_1_chi2/track_1_nDoF) <= 300 && (track_2_chi2/track_2_nDoF) <= 300";

  static const std::string MC_Ks_cuts = "abs(track_1_PV_DCA_xy) >= 0.01 && abs(track_2_PV_DCA_xy) >= 0.01 && K_S0_DIRA >= 0.98 && track_1_track_2_DCA_xy <= 1.25 && track_1_track_2_DCA <= 0.75";

  static const std::string MC_lambda_cuts = "abs(track_1_PV_DCA_xy) >= 0.01 && abs(track_2_PV_DCA_xy) >= 0.01 && Lambda0_DIRA >= 0.98 && track_1_track_2_DCA_xy <= 1.25 && track_1_track_2_DCA <= 0.75";
                                                 
  //static const HistogramInfo final_pt_bins("pT","pT",10,0.2,3.,"pT [GeV/c]");
  // special bins to sync with Tony's tracking efficiency
  static const HistogramInfo final_pt_bins("pT","pT",{0.6,0.7,0.8,0.9,1.,1.1,1.2,1.3,1.4,1.5,1.8,2.1,2.4,2.7,3.,3.99999},"pT [GeV/c]");
//  static const HistogramInfo final_pt_bins("pT","pT",{0.8,1.1,1.4,1.8,2.2,3.,3.9999},"pT [GeV/c]");
  static const HistogramInfo final_rapidity_bins("rapidity","rapidity",15,-0.8,0.8,"rapidity");
  static const HistogramInfo final_eta_bins("pseudorapidity","#eta",15,-0.8,0.8,"#eta");
  static const HistogramInfo final_phi_bins("phi","#phi",15,-M_PI,M_PI,"#phi");
  static const HistogramInfo final_ntrack_bins("ntrk","nTracks",makeLogBins(5,1.,20),"number of tracks");

  static const HistogramInfo pt_bins("pt","pT",100,0.,1.1,"pT [GeV/c]");
  static const HistogramInfo rapidity_bins("y","rapidity",100,-1.5,1.5,"y");
  static const HistogramInfo phi_bins("phi","#phi",100,-M_PI,M_PI,"#phi");
  static const HistogramInfo ntrack_bins("ntrk","nTracks",201,-0.5,200.5,"number of tracks");

  const std::string Ks_fiducial_cuts = fiducial_cuts("K_S0",{final_pt_bins,final_eta_bins,final_phi_bins,final_rapidity_bins});
  const std::string lambda_fiducial_cuts = fiducial_cuts("Lambda0",{final_pt_bins,final_eta_bins,final_phi_bins,final_rapidity_bins});

  static const std::map<std::string,HistogramInfo> mass_bins = {
    {"K_S0", HistogramInfo("K_S0_mass","K^{0}_{S} mass",100,0.43,0.6,"mass [GeV/c^{2}]",
                           general_daughter_track_cuts+" && "+Ks_fiducial_cuts)},
    {"phi", HistogramInfo("phi_mass","#phi mass",100,0.95,1.1,"mass [GeV/c^{2}]",
                          "track_1_MVTX_nStates>=2 && track_2_MVTX_nStates>=2 &&"
                          "track_1_PV_DCA_xy<=0.05 && track_2_PV_DCA_xy<=0.05 && "
                          "phi_decayLength<=0.05")},
    {"Lambda0",HistogramInfo("Lambda0_mass","#Lambda mass",100,1.1,1.135,"mass [GeV/c^{2}]",
                             general_daughter_track_cuts+" && "+lambda_fiducial_cuts)}
  };

  static const std::map<std::string,HistogramInfo> mass_bins_pos = {
    {"K_S0", HistogramInfo("K_S0_mass","K^{0}_{S} mass",100,0.43,0.6,"mass [GeV/c^{2}]",
                           general_daughter_track_cuts+" && "+Ks_fiducial_cuts)},
    {"phi", HistogramInfo("phi_mass","#phi mass",100,0.95,1.1,"mass [GeV/c^{2}]",
                          "track_1_MVTX_nStates>=2 && track_2_MVTX_nStates>=2 &&"
                          "track_1_PV_DCA_xy<=0.05 && track_2_PV_DCA_xy<=0.05 && "
                          "phi_decayLength<=0.05")},
    {"Lambda0",HistogramInfo("Lambda0_mass","#Lambda mass",100,1.1,1.135,"mass [GeV/c^{2}]",
                             "track_1_charge==-1 && track_2_charge==1 && "+general_daughter_track_cuts+" && "+lambda_fiducial_cuts)}
  };

  static const std::map<std::string,HistogramInfo> mass_bins_neg = {
    {"K_S0", HistogramInfo("K_S0_mass","K^{0}_{S} mass",100,0.43,0.6,"mass [GeV/c^{2}]",
                           general_daughter_track_cuts+" && "+Ks_fiducial_cuts)},
    {"phi", HistogramInfo("phi_mass","#phi mass",100,0.95,1.1,"mass [GeV/c^{2}]",
                          "track_1_MVTX_nStates>=2 && track_2_MVTX_nStates>=2 &&"
                          "track_1_PV_DCA_xy<=0.05 && track_2_PV_DCA_xy<=0.05 && "
                          "phi_decayLength<=0.05")},
    {"Lambda0",HistogramInfo("Lambda0_mass","#Lambda mass",100,1.1,1.135,"mass [GeV/c^{2}]",
                             "track_1_charge==1 && track_2_charge==-1 && "+general_daughter_track_cuts+" && "+lambda_fiducial_cuts)}
  };

  static const std::map<std::string,HistogramInfo> mass_bins_MC = {
    {"K_S0", HistogramInfo("K_S0_mass","K^{0}_{S} mass",100,0.43,0.6,"mass [GeV/c^{2}]",
                           general_daughter_track_cuts + " && " + Ks_fiducial_cuts + " && " + MC_daughter_selection_cuts + " && " + MC_Ks_cuts)},
    {"Lambda0",HistogramInfo("Lambda0_mass","#Lambda mass",100,1.1,1.135,"mass [GeV/c^{2}]",
                           general_daughter_track_cuts + " && " + lambda_fiducial_cuts + " && " + MC_daughter_selection_cuts + " && " + MC_lambda_cuts)}
  };

  static const std::map<std::string,HistogramInfo> mass_bins_MC_pos = {
    {"K_S0", HistogramInfo("K_S0_mass","K^{0}_{S} mass",100,0.43,0.6,"mass [GeV/c^{2}]",
                           general_daughter_track_cuts + " && " + Ks_fiducial_cuts + " && " + MC_daughter_selection_cuts + " && " + MC_Ks_cuts)},
    {"Lambda0",HistogramInfo("Lambda0_mass","#Lambda mass",100,1.1,1.135,"mass [GeV/c^{2}]",
                           "track_1_charge==-1 && track_2_charge==1 && " + lambda_fiducial_cuts + " && " + general_daughter_track_cuts + " && " + MC_daughter_selection_cuts + " && " + MC_lambda_cuts)}
  };

  static const std::map<std::string,HistogramInfo> mass_bins_MC_neg = {
    {"K_S0", HistogramInfo("K_S0_mass","K^{0}_{S} mass",100,0.43,0.6,"mass [GeV/c^{2}]",
                           general_daughter_track_cuts + " && " + Ks_fiducial_cuts + " && " + MC_daughter_selection_cuts + " && " + MC_Ks_cuts)},
    {"Lambda0",HistogramInfo("Lambda0_mass","#Lambda mass",100,1.1,1.135,"mass [GeV/c^{2}]",
                           "track_1_charge==1 && track_2_charge==-1 && " + lambda_fiducial_cuts + " && " + general_daughter_track_cuts + " && " + MC_daughter_selection_cuts + " && " + MC_lambda_cuts)}
  };

} // namespace BinInfo
#endif // BINNING_H
