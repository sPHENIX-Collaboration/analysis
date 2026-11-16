#ifndef HISTOGRAM_TOOLS_H
#define HISTOGRAM_TOOLS_H

#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>

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
      size_t nparsed = 0;
      std::stod(statement,&nparsed);
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

  HistogramInfo(const std::string& hname, const std::string& htitle, const std::vector<double>& hbins,
                const std::string& haxislabel = "")
  : name(hname), title(htitle), axis_label(haxislabel), bins(hbins)
  {
  }

  // constructor for uniform binning
  HistogramInfo(const std::string& hname, const std::string& htitle, const int nBins, const float xmin, const float xmax, 
                const std::string& haxislabel = "")
  : name(hname), title(htitle), axis_label(haxislabel)
  {
    const float interval = (xmax-xmin)/nBins;
    for(int i=0; i<=nBins; i++)
    {
      bins.push_back(xmin+i*interval);
    }
  }

  std::string get_bin_selection(std::string var, size_t bin) const
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

std::vector<std::string> get_cutvars(TTree* t,std::string cut_string)
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
  for(size_t i=0; i<bins.size(); i++)
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
  for(size_t i=0; i<=hinfo_y.bins.size(); i++)
  {
    const std::string name = hinfo_x.name + "_vs" + hinfo_y.name + "_" + std::to_string(i);
    const std::string title = hinfo_x.title + " vs. " + hinfo_y.title + 
      " (bin " + std::to_string(i) + ", " + hinfo_y.get_bin_selection(hinfo_y.name,i) + ");" + hinfo_x.axis_label;
    h_out.push_back(new TH1F(name.c_str(),title.c_str(),hinfo_x.bins.size()-1,hinfo_x.bins.data()));
  }
  return h_out;
}

#endif // HISTOGRAM_TOOLS_H
