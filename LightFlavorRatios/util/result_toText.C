struct plotData
{
  std::string x_vals;
  std::string x_err_low;
  std::string x_err_high;

  std::string y_vals;
  std::vector<std::string> y_err_names;
  std::vector<std::string> y_errs_low;
  std::vector<std::string> y_errs_high;

  void append_all(const std::string& s)
  {
    x_vals += s;
    x_err_low += s;
    x_err_high += s;
    y_vals += s;
    for(int i=0;i<y_err_names.size();i++)
    {
      y_errs_low[i] += s;
      y_errs_high[i] += s;
    }
  }

  plotData(const std::vector<std::string> errnames)
  {
    for(const std::string& name : errnames)
    {
      y_err_names.push_back(name);
      y_errs_low.push_back("");
      y_errs_high.push_back("");
    }

    append_all("{");
  } 
};

plotData TH1_getText(const std::string& filename, const std::string& hname)
{
  TFile* f = TFile::Open(filename.c_str());
  TH1* h = (TH1*)f->Get(hname.c_str());

  plotData pd({"y_err"});

  const int nbins = h->GetNbinsX();

  for(int i=1;i<=nbins;i++)
  {
    pd.x_vals += std::to_string(h->GetBinCenter(i));
    pd.x_err_low += std::to_string(h->GetBinCenter(i)-h->GetBinLowEdge(i));
    pd.x_err_high += std::to_string((h->GetBinLowEdge(i)+h->GetBinWidth(i))-h->GetBinCenter(i));

    pd.y_vals += std::to_string(h->GetBinContent(i));
    pd.y_errs_low[0] += std::to_string(h->GetBinError(i));
    pd.y_errs_high[0] += std::to_string(h->GetBinError(i));

    std::string separator = ", ";
    if(i==nbins) separator = "}";

    pd.append_all(separator);
  }

  return pd;
}

plotData TGraphMultiErrors_StatSys_getText(const std::string& filename, const std::string& gname)
{
  TFile* f = TFile::Open(filename.c_str());
  TGraphMultiErrors* g = (TGraphMultiErrors*)f->Get(gname.c_str());

  plotData pd({"y_err_stat","y_err_sys"});

  for(int i=0;i<g->GetN();i++)
  {
    pd.x_vals += std::to_string(g->GetPointX(i));
    pd.y_vals += std::to_string(g->GetPointY(i));
    pd.x_err_low += std::to_string(g->GetErrorXlow(i));
    pd.x_err_high += std::to_string(g->GetErrorXhigh(i));
    pd.y_errs_low[0] += std::to_string(g->GetErrorYlow(i,0));
    pd.y_errs_high[0] += std::to_string(g->GetErrorYhigh(i,0));
    pd.y_errs_low[1] += std::to_string(g->GetErrorYlow(i,1));
    pd.y_errs_high[1] += std::to_string(g->GetErrorYhigh(i,1));

    std::string separator = ", ";
    if(i==g->GetN()-1) separator = "}";

    pd.append_all(separator);
  }

  return pd;
}

void result_toText(const std::string& filename = "../yield_and_ratios/fits.root",
                   const std::string& hname = "K_S0_yield_vspT")
{
  plotData pd = TH1_getText(filename,hname);
  std::cout << "x_vals = " << pd.x_vals << ";" << std::endl;
  std::cout << "y_vals = " << pd.y_vals << ";" << std::endl;
}
/*
void result_toText(const std::string& filename = "../yield_and_ratios/temp_ratio_results_systematic.root",
                   const std::string& gname = "ratio_pt_sys")
{
  plotData pd = TGraphMultiErrors_StatSys_getText(filename,gname);
  std::cout << "x_vals = " << pd.x_vals << ";" << std::endl;
  std::cout << "x_errs_low = " << pd.x_err_low << ";" << std::endl;
  std::cout << "x_errs_high = " << pd.x_err_high << ";" << std::endl;
  std::cout << "y_vals = " << pd.y_vals << ";" << std::endl;
  for(int i=0;i<pd.y_err_names.size();i++)
  {
    std::cout << pd.y_err_names[i] << "_low = " << pd.y_errs_low[i] << ";" << std::endl;
    std::cout << pd.y_err_names[i] << "_high = " << pd.y_errs_high[i] << ";" << std::endl;
  }
}
*/
