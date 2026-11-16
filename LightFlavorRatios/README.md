# Light-Flavor Ratio Analysis

Repository for determination of ratios of resonances, originally constructed for PPG 16 (Lambda/2Kshort ratio), but is extensible to a general particle-to-particle yield ratio.

## Usage

For running on data, just run one of the following macros:

- `yield_and_ratios/Lambda_Ks_ratio.C` is the default (Lambda + anti-Lambda)/2Ks analysis.
- `yield_and_ratios/Lambda_Ks_ratio_pos.C` is a Lambda/Ks analysis.
- `yield_and_ratios/Lambda_Ks_ratio_neg.C` is an anti-Lambda/Ks analysis.
- `yield_and_ratios/Lambda_Ks_ratio_NN.C` is the (Lambda + anti-Lambda)/2Ks analysis using a track sample corrected by the momentum-scale NN correction before KFParticle.

For running on MC, use one of the following macros:

- `yield_and_ratios/Lambda_Ks_ratio_MC.C` is the default (Lambda + anti-Lambda)/2Ks analysis.
- `yield_and_ratios/Lambda_Ks_ratio_pos.C` is a Lambda/Ks analysis.
- `yield_and_ratios/Lambda_Ks_ratio_neg.C` is an anti-Lambda/Ks analysis.

The MC macros import mass histograms that have been pre-generated and merged from a much larger dataset. To regenerate these mass histograms, `condor_submit` the job file `mass_histograms/submit_MC.job`, and run `mass_histograms/merge_MC.sh` to merge into the appropriate input file locations.

### Closure Test

The MC closure test is performed on the simulation samples residing in `/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/`. Due to the large number of $\Lambda$ and $K_{S}^{0}$ in the simulation sample, the closure test uses the binned workflow. To replicate the results, do the following:

- Generate mass histograms using `mass_histograms/plot_mass.C`, via Condor job `mass_histograms/submit_MC.job`. [Note: this is already done most of the time.]
- Merge the mass histograms using `mass_histograms/merge_MC.sh`. [Note: this is also usually already done.]
- Analyze the mass histograms and plot results using `yield_and_ratios/run_all_MC.sh`, creating the output files `yield_and_ratios/fits_MC.root`, `fits_MC_pos.root`, and `fits_MC_neg.root` for each of the parity-inclusive and parity-separated analyses; this also creates plots in the folders `yield_and_ratios/plots_MC`, `plots_MC_pos`, and `plots_MC_neg` (N.B. currently you may have to ensure those folders exist before you run this step).
- Generate a truth reference using `truth_ratio/calculate_truth_ratio.C`, via Condor job `truth_ratio/submit_truth_ratio_all.job`.
- Merge the truth histograms using `truth_ratio/merge_output.sh`.
- Compare the MC reco and truth by running `truth_ratio/run_truth_comparison.sh`, which will create plots in the folder `truth_ratio/plots`.

The main analysis macros are `yield_and_ratios/Lambda_Kshort_ratio_MC.C`, `yield_and_ratios/Lambda_Kshort_ratio_MC_pos.C`, and `yield_and_ratios/Lambda_Kshort_ratio_MC_neg.C`, which correspond to the $(\Lambda+\overline{\Lambda})/2K_{S}^{0}$, $\Lambda/K_{S}^{0}$, and $\overline{\Lambda}/K_{S}^{0}$ analyses respectively. They already point to the most updated versions of the corrections; if you'd like to recalculate the corrections yourself, perform the following steps:

For the geometric acceptance correction:

- Build and install the `ResonanceGeometricAcceptance` analysis module in `geometric_acceptance/ResonanceGeometricAcceptance`.
- Calculate the geometric acceptance for each file using `geometric_acceptance/analysis/Fun4All_HFG_GeometricAcceptance.C`, via Condor job `geometric_acceptance/analysis/submit_geometric_acceptance.job`.
- Merge the geometric acceptance files and calculate the correction using `geometric_acceptance/analysis/merge_geomAccept_hists.sh`.

For the cut efficiency correction:

- Calculate the cut efficiency for each file using `swimming_correction/CutEfficiency_mjp.C`, via Condor job `swimming_correction/submit_cuteff_mjp.job`.
- Merge the cut efficiency files and calculate the correction using `swimming_correction/merge_cutefficiency_mjp.sh`.

For the closure test, the lambda feed-down is quite small due to the nature of the sample, and the tracking efficiency correction is only applied to data in any case, so neither of those corrections are applied.

## General Workflow Concepts

The core object that does the "actual analysis" part (extraction of yield, calculation of ratio, application of corrections) is `yield_and_ratios/ResonanceRatio.h`. The objects used for differential and integrated yield extraction are instances of RooAbsData, therefore it can accept both binned and unbinned inputs. There are two different general ways of setting this object up, corresponding to unbinned and binned inputs respectively:

- Start with a KFParticle TTree, which is converted into a RooDataSet; division into bins in the set of differential variables is accomplished by `RooDataSet::reduce` according to the binning schemes and cut strings contained in `util/binning.h`. This works best for small datasets and is computationally intractable on large datasets.
- Start with a set of mass histograms, assembled according to the binning scheme contained in `util/binning.h`. These are internally converted to sets of RooDataHists.

## Differential Variables

Currently, the analysis is done in bins of four differential variables:

- pT
- eta
- phi
- rapidity

The binning schemes for these variables are stored in `util/binning.h`, as objects of type `HistogramInfo`. Adding an additional differential variable is straightforward: create a corresponding `HistogramInfo` object in the `BinInfo` namespace, and add that `HistogramInfo` object to the vector of variables provided to the `ResonanceRatio` constructor in your macro. The list of differential variables, and their binning scheme, is consistently taken from `util/binning.h` across the entire analysis framework.

## Mass Bins and Cut Selections

In addition to specifying the binning scheme, `util/binning.h` also contains the cut-strings for each of the particles of interest, as well as the binning schemes of the mass plots (if working with binned data). There are a variety of mass binning schemes provided:

- `mass_bins`, the default for use on data;
- `mass_bins_pos`, the data binning scheme but restricted to Lambda and cutting out anti-Lambda (Kshort is unaltered);
- `mass_bins_neg`, like `mass_bins_pos` but selecting for anti-Lambda;
- `mass_bins_MC`, the default for use on a loose MC sample (contains the cuts that would have been applied in KFParticle);
- `mass_bins_MC_pos`, like `mass_bins_MC` but selecting for Lambda and cutting out anti-Lambda;
- `mass_bins_MC_neg`, like `mass_bins_MC_pos` but selecting for anti-Lambda.

The latter three are used for MC closure tests, for which the KFParticle cuts are intentionally set much looser to allow for the calculation of corrections.

## Yield Extraction

Currently, the analysis uses a simple and robust yield extraction procedure based on sideband subtraction rather than fitting of the signal and background distributions. This process is implemented in `ResonanceRatio::get_yield_constfit`. If yield extraction via fits is desired, this is implemented in `ResonanceRatio::get_yield`. Fitting is accomplished via RooFit's unbinned negative-log-likelihood minimization, and the input functions for these fits are accepted as inputs to the `ResonanceRatio` constructor, as objects that inherit from the `ParticleModel` base class. The 

## Corrections

All correction types in `corrections/` inherit from the `CorrectionHistogram1D` base class; the file from which they are retrieved, and the way in which they are applied, are specified in their corresponding correction type header. The `ResonanceRatio` object accepts a list of `CorrectionHistogram1D` objects as an input, and applies them to the yield ratio results accordingly. The various macros specified above show examples of the creation and application of these corrections.

Generation of the currently-used corrections is accomplished in the following locations:

- Geometric acceptance corrections are generated from `geoaccaptance_correction/`;
- Cut efficiency corrections are generated from `swimming_correction/CutEfficiency.C`;
- Relative tracking efficiency corrections are generated outside of this repository;
- Lambda feed-down corrections are generated in `cascade_feeddown/`.

## Systematic Uncertainties

Systematic uncertainties are currently generated by procedures that are not all centralized in this repository; regardless, the application of these systematic uncertainties to the current plots is implemented in `yield_and_ratios/plot_results_systematics.C`.

## Truth Reference

The generation of a truth reference, for use in the MC closure test, is handled in `truth_ratio/`. Truth references are generated for all three kinds of ratio ((Lambda+anti-Lambda)/2Ks, Lambda/Ks, and anti-Lambda/Ks).
