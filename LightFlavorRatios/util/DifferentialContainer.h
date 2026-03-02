#ifndef DIFFERENTIALCONTAINER_H
#define DIFFERENTIALCONTAINER_H

#include "binning.h"

struct DifferentialContainer
{
  std::string particle;
  std::vector<TH1F*> hists;
  HistogramInfo var_info;

  DifferentialContainer(const std::string& hparticle, const HistogramInfo& hinfo)
  : particle(hparticle), var_info(hinfo)
  {
    hists = makeDifferentialHistograms(BinInfo::mass_bins.at(particle),hinfo);
  }

  DifferentialContainer(TFile* f, const std::string& hparticle, const HistogramInfo& hinfo)
  : particle(hparticle), var_info(hinfo)
  {
    for(int i=0; i<hinfo.bins.size()+2; i++)
    {
      std::string name = BinInfo::mass_bins.at(particle).name + "_vs" + hinfo.name + "_" + std::to_string(i);
      TH1F* h = (TH1F*)f->Get(name.c_str());
      hists.push_back(h);
    }
  }

  void Write()
  {
    for(TH1F* h : hists)
    {
      h->Write();
    }
  }
};

#endif
