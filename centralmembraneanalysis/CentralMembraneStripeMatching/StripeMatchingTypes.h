#ifndef TPCCALIB_STRIPEMATCHINGTYPES_H
#define TPCCALIB_STRIPEMATCHINGTYPES_H

#include <array>

// A stripe is stored as std::array<double, 3> everywhere in v3.
// The indices below replace the old StripeCluster object fields.
inline constexpr int stripe_phi = 0;
inline constexpr int stripe_r = 1;
inline constexpr int stripe_content = 2;

#endif
