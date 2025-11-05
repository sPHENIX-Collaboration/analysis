#include "TTree.h"
#include "TFile.h"

#include "../util/binning.h"

void plot_mass(std::string infile = "/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/data/KK_reco/outputKFParticle_KK_reco_00053877_00000_00000.root",
               std::string particle = "phi",
               std::string outfile = "test_out.root")
{
  TFile* f = TFile::Open(infile.c_str());
  TTree* t = (TTree*)f->Get("DecayTree");

  HistogramInfo hmass = BinInfo::mass_bins.at(particle);
  HistogramInfo hpt = BinInfo::pt_bins;
  HistogramInfo hy = BinInfo::rapidity_bins;
  HistogramInfo hphi = BinInfo::phi_bins;
  HistogramInfo hntrk = BinInfo::ntrack_bins;

  std::string draw_param = particle+"_mass>>"+hmass.name+"("+hmass.bin_string()+")";
  std::cout << "draw_param: " << draw_param << std::endl;

  std::string pt_drawparam = particle+"_pT:"+particle+"_mass>>"+hmass.name+hpt.name + 
    "("+hmass.bin_string()+","+hpt.bin_string()+")";

  std::cout << "draw_param: " << pt_drawparam << std::endl;

  std::string ntrk_drawparam = "nTracksOfBC:"+particle+"_mass>>"+hmass.name+hntrk.name +
    "("+hmass.bin_string()+","+hntrk.bin_string()+")";

  std::string y_drawparam = particle+"_rapidity:"+particle+"_mass>>"+hmass.name+hy.name +
    "("+hmass.bin_string()+","+hy.bin_string()+")";

  std::string phi_drawparam = particle+"_phi:"+particle+"_mass>>"+hmass.name+hphi.name + 
    "("+hmass.bin_string()+","+hphi.bin_string()+")";

  t->Draw(draw_param.c_str(),hmass.cut_string.c_str(),"");
  TH1F* mass = (TH1F*)gPad->GetPrimitive(hmass.name.c_str());
  mass->SetTitle(hmass.title.c_str());

  t->Draw(pt_drawparam.c_str(),hmass.cut_string.c_str(),"COLZ");
  TH2F* mass_vspt = (TH2F*)gPad->GetPrimitive((hmass.name+hpt.name).c_str());
  mass_vspt->SetTitle((hmass.title+hpt.title).c_str());

  t->Draw(ntrk_drawparam.c_str(),hmass.cut_string.c_str(),"COLZ");
  TH2F* mass_vsntrk = (TH2F*)gPad->GetPrimitive((hmass.name+hntrk.name).c_str());
  mass_vsntrk->SetTitle((hmass.title+hntrk.title).c_str());

  t->Draw(y_drawparam.c_str(),hmass.cut_string.c_str(),"COLZ");
  TH2F* mass_vsy = (TH2F*)gPad->GetPrimitive((hmass.name+hy.name).c_str());
  mass_vsy->SetTitle((hmass.title+hy.title).c_str());

  t->Draw(phi_drawparam.c_str(),hmass.cut_string.c_str(),"COLZ");
  TH2F* mass_vsphi = (TH2F*)gPad->GetPrimitive((hmass.name+hphi.name).c_str());
  mass_vsphi->SetTitle((hmass.title+hphi.title).c_str());

  TFile* outf = new TFile(outfile.c_str(),"RECREATE");
  mass->Write();
  mass_vspt->Write();
  mass_vsntrk->Write();
  mass_vsy->Write();
  mass_vsphi->Write();
}
