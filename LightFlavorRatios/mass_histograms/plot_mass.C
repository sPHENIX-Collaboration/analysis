#include "TTree.h"
#include "TFile.h"

#include "../util/binning.h"

void plot_mass(std::string infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/data/KK_reco/outputKFParticle_KK_reco_00053877_00000_00000.root",
               std::string particle = "phi",
               std::string outfile = "test_out.root")
{
  TFile* f = TFile::Open(infile.c_str());
  TTree* t = (TTree*)f->Get("DecayTree");

  HistogramInfo massbins = BinInfo::mass_bins.at(particle);
  HistogramInfo ptbins = BinInfo::final_pt_bins;
  HistogramInfo ybins = BinInfo::final_rapidity_bins;
  HistogramInfo phibins = BinInfo::final_phi_bins;
  HistogramInfo ntrkbins = BinInfo::final_ntrack_bins;

  TH1F* mass = makeHistogram(massbins);
  std::vector<TH1F*> mass_vspt = makeDifferentialHistograms(massbins,ptbins);
  std::vector<TH1F*> mass_vsy = makeDifferentialHistograms(massbins,ybins);
  std::vector<TH1F*> mass_vsphi = makeDifferentialHistograms(massbins,phibins);
  std::vector<TH1F*> mass_vsntrk = makeDifferentialHistograms(massbins,ntrkbins);

  std::string draw_param = particle+"_mass>>"+mass->GetName();
  std::cout << "draw_param: " << draw_param << std::endl;

  t->Draw(draw_param.c_str(),massbins.cut_string.c_str(),"goff");
  
  for(int i=0; i<mass_vspt.size(); i++)
  {
    std::string pt_draw_param = particle+"_mass>>"+mass_vspt[i]->GetName();
    std::string pt_cut_param = massbins.cut_string+(massbins.cut_string.empty()?"":"&&")+ptbins.get_bin_selection(particle+"_pT",i);
    t->Draw(pt_draw_param.c_str(),pt_cut_param.c_str(),"goff");
    std::cout << "pt entries, bin " << i << " is " << mass_vspt[i]->GetEntries() << std::endl;
  }

  for(int i=0; i<mass_vsy.size(); i++)
  {
    std::string y_draw_param = particle+"_mass>>"+mass_vsy[i]->GetName();
    std::string y_cut_param = massbins.cut_string+(massbins.cut_string.empty()?"":"&&")+ybins.get_bin_selection(particle+"_y",i);
    t->Draw(y_draw_param.c_str(),y_cut_param.c_str(),"goff");
  }

  for(int i=0; i<mass_vsphi.size(); i++)
  {
    std::string phi_draw_param = particle+"_mass>>"+mass_vsphi[i]->GetName();
    std::string phi_cut_param = massbins.cut_string+(massbins.cut_string.empty()?"":"&&")+phibins.get_bin_selection(particle+"_phi",i);
    t->Draw(phi_draw_param.c_str(),phi_cut_param.c_str(),"goff");
  }

  for(int i=0; i<mass_vsntrk.size(); i++)
  {
    std::string ntrk_draw_param = particle+"_mass>>"+mass_vsntrk[i]->GetName();
    std::string ntrk_cut_param = massbins.cut_string+(massbins.cut_string.empty()?"":"&&")+ntrkbins.get_bin_selection("nTracksOfBC",i);
    t->Draw(ntrk_draw_param.c_str(),ntrk_cut_param.c_str(),"goff");
  }

  TFile* outf = new TFile(outfile.c_str(),"RECREATE");
  mass->Write();
  //mass_vspt.Write();
  //mass_vsntrk.Write();
  //mass_vsy.Write();
  //mass_vsphi.Write();
  for(TH1F* hpt : mass_vspt) hpt->Write();
  for(TH1F* hntrk : mass_vsntrk) hntrk->Write();
  for(TH1F* hy : mass_vsy) hy->Write();
  for(TH1F* hphi : mass_vsphi) hphi->Write();
}
