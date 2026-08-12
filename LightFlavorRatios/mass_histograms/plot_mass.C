#include "TTree.h"
#include "TFile.h"

#include "../util/binning.h"

void plot_mass(const std::string infile = "Kshort_3runs.root",
               const std::string particle = "K_S0",
               const std::string outfile = "test_out.root",
               const std::map<std::string,HistogramInfo>& massbins_map = BinInfo::mass_bins_MC)
{
  TFile* f = TFile::Open(infile.c_str());
  TTree* t = (TTree*)f->Get("DecayTree");

  HistogramInfo massbins = massbins_map.at(particle);
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

  std::string draw_param = particle+"_mass>>"+mass->GetName();
  std::cout << "draw_param: " << draw_param << std::endl;

  t->Draw(draw_param.c_str(),massbins.cut_string.c_str(),"goff");
  
  for(int ivar=0; ivar<differential_h.size(); ivar++)
  {
    for(int ibin=0; ibin<differential_h[ivar].size(); ibin++)
    {
      HistogramInfo& hinfo = differential_vars[ivar];
      TH1F* h = differential_h[ivar][ibin];
      std::cout << "plotting " << hinfo.title << " bin " << ibin << std::endl;
      std::string draw_param = particle+"_mass>>"+h->GetName();
      std::string cut_param =  massbins.cut_string+(massbins.cut_string.empty()?"":"&&")+hinfo.get_bin_selection(particle+"_"+hinfo.name,ibin);
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
