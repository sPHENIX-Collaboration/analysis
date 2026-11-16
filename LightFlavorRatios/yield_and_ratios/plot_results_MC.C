#include "plot_results.C"

void plot_results_MC()
{
  plot_results("fits_MC.root","plots_MC");
  plot_results("fits_MC_pos.root","plots_MC_pos");
  plot_results("fits_MC_neg.root","plots_MC_neg");
}
