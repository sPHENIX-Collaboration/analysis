#!/usr/bin/env bash

# 1) Combine outputs
echo "==> Running get_combinedoutput.C"
root -l -q -b get_combinedoutput.C

# 2) Compute purity and efficiency
echo "==> Running get_purityefficiency.C"
root -l -q -b get_purityefficiency.C

# 3) Perform unfolding iterations
echo "==> Running do_unfolding_iter.C"
root -l -q -b do_unfolding_iter.C

# 4) Generate the final spectrum
echo "==> Running get_finalspectrum.C"
root -l -q -b get_finalspectrum.C

# Done
echo "All analysis steps"

