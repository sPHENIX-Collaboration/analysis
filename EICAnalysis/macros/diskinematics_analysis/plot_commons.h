/* Header file with useful functions for plotting */

/*------------------------------------------*/
/*------------Create Histogram--------------*/
/*------------------------------------------*/
/**
 *\ The creation of this histogram includes a method for setting the bins to a logarithmic scale along with the axes
 **/
/*------------------------------------------*/
TH2F* make_TH2D_xylog( int x_nbins, int x_min, int x_max, int y_nbins, int y_min, int y_max )
{
  TH2F* hnew = new TH2F("hnew", "", x_nbins, x_min, x_max, y_nbins, y_min, y_max );
  TAxis *xaxis = hnew->GetXaxis();
  TAxis *yaxis = hnew->GetYaxis();
  int xbins = xaxis->GetNbins();
  int ybins = yaxis->GetNbins();

  Axis_t xmin = xaxis->GetXmin();
  Axis_t xmax = xaxis->GetXmax();
  Axis_t xwidth = (xmax - xmin ) / xbins;
  Axis_t *new_xbins = new Axis_t[xbins + 1];

  Axis_t ymin = yaxis->GetXmin();
  Axis_t ymax = yaxis->GetXmax();
  Axis_t ywidth = (ymax - ymin) / ybins;
  Axis_t *new_ybins = new Axis_t[ybins + 1];

  for( int i =0; i <= xbins; i++)
    {
      new_xbins[i] = TMath::Power( 10, xmin + i*xwidth);
    }
  xaxis->Set(xbins, new_xbins);

  for( int i =0; i <= ybins; i++)
    {
      new_ybins[i] = TMath::Power( 10, ymin + i*ywidth);
    }
  yaxis->Set(ybins, new_ybins);

  return hnew;
}
