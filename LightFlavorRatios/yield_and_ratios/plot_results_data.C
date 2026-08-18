#include "plot_results.C"

void plot_results_data()
{
  plot_results("fits.root","plots");
  plot_results("fits_pos.root","plots_pos");
  plot_results("fits_neg.root","plots_neg");
}
