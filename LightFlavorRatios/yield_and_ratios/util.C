float text_size = 0.04;

float alpha = 0.2;
string black_color = "#000000";
string red_color = "#fe0100";
string orange_color = "#FF9E00";
string yellow_color = "#F7D619";
string green_color = "#0a9900";
string blue_color = "#0800ff";
string cyan_color = "#00FBFF";
string indigo_color = "#7402D1";
string purple_color = "#DA19F7";

struct data_point
{
  double x_centroid {0};
  double x_low {0};
  double x_high {0};

  double y_centroid {0};
  double y_stat_err {0};
  double y_syst_err_low {0};
  double y_syst_err_high {0};
};

class plot_info
{
  public:
    string legend_info = "Dummy legend information";
    string color = black_color;
    int marker = 8;
    unsigned int nPoints = 1;
    vector<double> x_centroids;
    vector<double> x_lows;
    vector<double> x_highs;
    vector<double> y_centroids;
    vector<double> y_stat_errs;
    vector<double> y_syst_err_lows;
    vector<double> y_syst_err_highs;

    plot_info(string constructor_legend_info, string constructor_color, int constructor_marker, vector<data_point> constructor_data)
    {
      legend_info = constructor_legend_info;
      color = constructor_color;
      marker = constructor_marker;
      nPoints = constructor_data.size();
      for (data_point this_data : constructor_data)
      {
         x_centroids.push_back(this_data.x_centroid);
         x_lows.push_back(this_data.x_centroid - this_data.x_low);
         x_highs.push_back(this_data.x_high - this_data.x_centroid);
         y_centroids.push_back(this_data.y_centroid);
         y_stat_errs.push_back(this_data.y_stat_err);
         y_syst_err_lows.push_back(this_data.y_syst_err_low);
         y_syst_err_highs.push_back(this_data.y_syst_err_high);
      }
    }
};

TGraphMultiErrors* makeGraph(plot_info this_info)
{
  auto graph = new TGraphMultiErrors("", "", this_info.nPoints,
                                     &this_info.x_centroids[0],
                                     &this_info.y_centroids[0],
                                     &this_info.x_lows[0],
                                     &this_info.x_highs[0],
                                     &this_info.y_stat_errs[0],
                                     &this_info.y_stat_errs[0]);
  graph->AddYError(this_info.nPoints, &this_info.y_syst_err_lows[0], &this_info.y_syst_err_highs[0]);
  Int_t ci = TColor::GetColor(this_info.color.c_str());
  graph->SetMarkerColor(ci);
  graph->SetMarkerSize(2);
  graph->SetLineColor(ci);
  graph->SetFillColor(ci);
  graph->SetMarkerStyle(this_info.marker);
  graph->GetAttLine(0)->SetLineColor(ci);
  graph->GetAttLine(1)->SetLineColor(ci);
  graph->GetAttFill(1)->SetFillStyle(1001);
  graph->GetAttFill(1)->SetFillColorAlpha(ci, alpha);

  return graph;
}

void makeLine(float min, float max)
{
  //Add line at y = 1
  TF1 *line = new TF1("line", "pol1", min, max);
  line->SetParameters(1, 0);
  line->SetLineColor(1);
  line->SetLineStyle(2);
  line->SetLineWidth(2);
  line->Draw("SAME");
}
