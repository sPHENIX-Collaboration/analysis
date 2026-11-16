#ifndef DIFFERENTIALCONTAINER_H
#define DIFFERENTIALCONTAINER_H

#include "HistogramTools.h"

struct DifferentialContainer
{
  std::string particle_name;
  std::vector<TH1F*> hists;
  HistogramInfo diff_variable;
  HistogramInfo massbins;

  DifferentialContainer(const std::string& particle, const HistogramInfo& hmassbins, const HistogramInfo& diff_var)
  : particle_name(particle), massbins(hmassbins), diff_variable(diff_var)
  {
    hists = makeDifferentialHistograms(massbins,diff_var);
  }

  DifferentialContainer(TFile* f, const std::string& particle, const HistogramInfo& hmassbins, const HistogramInfo& diff_var)
  : particle_name(particle), massbins(hmassbins), diff_variable(diff_var)
  {
    for(int i=0; i<diff_variable.bins.size()+2; i++)
    {
      std::string name = massbins.name + "_vs" + diff_variable.name + "_" + std::to_string(i);
      std::cout << "importing " << name << std::endl;
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

#endif // DIFFERENTIALCONTAINER_H
