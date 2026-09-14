#include "plot_results.C"

void plot_results_NN()
{
  plot_results("fits_NN.root","plots_NN");
  plot_results("fits_NN_pos.root","plots_NN_pos");
  plot_results("fits_NN_neg.root","plots_NN_neg");
}
