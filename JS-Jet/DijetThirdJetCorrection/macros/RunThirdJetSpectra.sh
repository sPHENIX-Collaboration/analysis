#! /bin/bash
export MYINSTALL=/sphenix/user/sgross/install_dir
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/bin/root.exe -x -b /sphenix/user/sgross/sphenix_analysis/JS-Jet/DijetThirdJetCorrection/macros/RunThirdJetSpectra.C\(\"$1\",\"$2\"\) 
