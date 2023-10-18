# This directory contains code to run starlight
# Note that you have to compile starlight, or
# use the default one in the sPHENIX environment
# You also have to set up the slight.in config file.

# The following are examples you can use to set up
# production of phstarlight formatted files
# Note: we need to convert the starlight output format to HEPMC2 
# so that we can input it into the sPHENIX simulation
runstarlight.cmd    - script to run starlight
submit_starlight.sh - script to submit runstarlight.cmd to condor


# The following are used to analyze the phstarlight output
ana_starlight.C - Create pdst with basic info on the J/Psi
AcceptanceMRP.C - Make acceptance plots from one set of ana_starlight.C output

plot_acceptance.C    - make plots of the combined from the output of AcceptanceMRP.C
plot_total_plusbkg.C - make plots, now with the hijing background

