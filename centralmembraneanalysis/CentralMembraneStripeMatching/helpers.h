#ifndef TPCCALIB_STRIPEMATCHING_HELPERS_H
#define TPCCALIB_STRIPEMATCHING_HELPERS_H

#include "StripeMatchingTypes.h"

#include <array>
#include <utility>
#include <vector>

class TFile;
class TH2;
class TObject;

// Keep a phi difference inside [-pi, pi].
double wrap_delta_phi(double dphi);

// Distance in the local (R, R*dphi) plane.
double distance_Rdphi(double phi1, double r1, double phi2, double r2);

// Median and MAD helpers for robust fit scales.
double median_value(std::vector<double> values);
double robust_mad_sigma(const std::vector<double> &values, double fallback);

// ROOT object and histogram helpers.
void safe_write_object(TObject *obj);
TH2 *load_detached_histogram(TFile *file, const char *histogram_name);

// Removes stripe candidates with too few nearby neighbors.
void filter_isolated_stripes(const std::vector<std::array<double, 3>> &input, std::vector<std::array<double, 3>> &output);

// Returns {cleaned histogram, radial gap centers}.
std::pair<TH2 *, std::vector<double>> clean_laminations(TH2 *histogram);

#endif
