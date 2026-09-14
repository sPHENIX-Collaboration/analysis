#include "TTree.h"
#include "TFile.h"

#include "../config/cuts.h"
#include "../config/binning.h"
#include "../util/HistogramTools.h"

void plot_mass(const std::string& infile = "Kshort_3runs.root",
               const std::string& outfile = "test_out.root",
               const CutSettings& cuts = StandardCuts::data_K_S0_cuts,
               const HistogramInfo& massbins = BinInfo::K_S0_data_mass_bins)
{
  TFile* f = TFile::Open(infile.c_str());
  TTree* t = (TTree*)f->Get("DecayTree");

  std::vector<HistogramInfo> differential_vars =
  {
    BinInfo::final_pt_bins,
    BinInfo::final_eta_bins,
    BinInfo::final_rapidity_bins,
    BinInfo::final_phi_bins,
  };

  TH1F* mass = makeHistogram(massbins);

  std::vector<std::vector<TH1F*>> differential_h;
  for(HistogramInfo& hinfo : differential_vars)
  {
    differential_h.push_back(makeDifferentialHistograms(massbins,hinfo));
  }

  std::string draw_param = cuts.mother_name+"_mass>>"+mass->GetName();
  std::cout << "draw_param: " << draw_param << std::endl;

  std::string cutstring = generate_selection_cutstring(cuts,differential_vars);

  t->Draw(draw_param.c_str(),cutstring.c_str(),"goff");
  
  for(int ivar=0; ivar<differential_h.size(); ivar++)
  {
    for(int ibin=0; ibin<differential_h[ivar].size(); ibin++)
    {
      HistogramInfo& hinfo = differential_vars[ivar];
      TH1F* h = differential_h[ivar][ibin];
      std::cout << "plotting " << hinfo.title << " bin " << ibin << std::endl;
      std::string draw_param = cuts.mother_name+"_mass>>"+h->GetName();
      std::string cut_param =  hinfo.get_bin_selection(cuts.mother_name+"_"+hinfo.name,ibin);
      if(!cutstring.empty()) cut_param += " && "+cutstring;
      std::cout << "cut string: " << cut_param << std::endl;
      t->Draw(draw_param.c_str(),cut_param.c_str(),"goff");
    }
  }

  TFile* outf = new TFile(outfile.c_str(),"RECREATE");
  mass->Write();

  for(std::vector<TH1F*>& var : differential_h)
  {
    for(TH1F* h : var)
    {
      h->Write();
    }
  }
}
