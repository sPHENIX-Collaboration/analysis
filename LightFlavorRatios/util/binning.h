#ifndef BINNING_H
#define BINNING_H

#include <map>

struct HistogramInfo
{
  std::string name;
  std::string title;
  std::string axis_label;
  std::vector<float> bins;
  std::string cut_string;

  HistogramInfo(const std::string& hname, const std::string& htitle, const std::vector<float>& hbins,
                const std::string& haxislabel = "", const std::string& hcutstring = "")
  : name(hname), title(htitle), bins(hbins), axis_label(haxislabel), cut_string(hcutstring)
  {}

  // constructor for uniform binning
  HistogramInfo(const std::string& hname, const std::string& htitle, const int nBins, const float xmin, const float xmax, 
                const std::string& haxislabel = "", const std::string& hcutstring = "")
  : name(hname), title(htitle), axis_label(haxislabel)
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
};

int findBin(float val, std::vector<float> bins)
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

std::vector<float> makeLogBins(const int nBins, const float xmin, const float xmax)
{
  std::vector<float> bins;
  const float log_interval = (log(xmax)-log(xmin))/nBins;
  for(int i=0; i<=nBins; i++)
  {
    bins.push_back(exp(log(xmin) + i*log_interval));
  }
  return bins;
}

TH1F* makeHistogram(const HistogramInfo& hinfo)
{
  TH1F* h = new TH1F(hinfo.name.c_str(),(hinfo.title+";"+hinfo.axis_label).c_str(),hinfo.bins.size()-1,hinfo.bins.data());
  return h;
}

TH1F* makeHistogram(const std::string& basename, const std::string& basetitle, const HistogramInfo& hinfo)
{
  TH1F* h = new TH1F((basename+"_vs"+hinfo.name).c_str(),(basetitle+" vs. "+hinfo.title).c_str(),hinfo.bins.size()-1,hinfo.bins.data());
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
  static const HistogramInfo final_pt_bins("pT","pT",10,0.2,3.,"pT [GeV]");
  static const HistogramInfo final_rapidity_bins("rapidity","rapidity",15,-1.1,1.1,"rapidity");
  static const HistogramInfo final_eta_bins("pseudorapidity","#eta",15,-1.1,1.1,"#eta");
  static const HistogramInfo final_phi_bins("phi","#phi",15,-M_PI,M_PI,"#phi");
  static const HistogramInfo final_ntrack_bins("ntrk","nTracks",makeLogBins(5,1.,20),"number of tracks");

  static const HistogramInfo pt_bins("pt","pT",100,0.,1.1,"pT [GeV]");
  static const HistogramInfo rapidity_bins("y","rapidity",100,-1.5,1.5,"y");
  static const HistogramInfo phi_bins("phi","#phi",100,-M_PI,M_PI,"#phi");
  static const HistogramInfo ntrack_bins("ntrk","nTracks",201,-0.5,200.5,"number of tracks");

  static const std::map<std::string,HistogramInfo> mass_bins = {
    {"K_S0", HistogramInfo("Kshort_mass","K^{0}_{S} mass",100,0.4,0.6,"mass [GeV/c^{2}]")},
    {"phi", HistogramInfo("phi_mass","#phi mass",100,0.95,1.1,"mass [GeV/c^{2}]",
                          "track_1_MVTX_nStates>=2 && track_2_MVTX_nStates>=2 &&"
                          "track_1_IP_xy<=0.05 && track_2_IP_xy<=0.05 && "
                          "phi_decayLength<=0.05")},
    {"Lambda0",HistogramInfo("lambda_mass","#Lambda mass",100,1.08,1.15,"mass [GeV/c^{2}]")}
  };
} // namespace BinInfo
#endif // BINNING_H
