The <a href="https://github.com/sPHENIX-Collaboration/analysis/tree/master/ParticleID/RICHAnalysis"> main library for RICH analysis</a> (which I think should include ALL Cerenkov PID detectors):
* FastPid_RICH: Fun4All module that should contain the fast simulation / parametrization- right now it extrapolates charged tracks to the gas RICH volume and assigns PID's based on some simple eta and momentum cut and the true PID. It needs work to implement the actual parametrization, but all the input / output calls should be there. This class would need to be copied and adapted for FastPid_mRICH and FastPid_DIRC.
* TrackProjectorPid: Helper class to do track projections to spheres, planes, and cylinders.
* PidInfo and PidInfoContainer: Class that stores both track ID and PID information, and the corresponding container class
* RICHParticleID: Fun4All module that calls the IRT algorithm (using Alessio's implementation) to calculate Cerenkov angels for tracks and photon hits from the gas RICH

<a href="https://github.com/sPHENIX-Collaboration/analysis/tree/master/ParticleID/RICHAnalysis_macros/macros_fun4all"/> Fun4All macros</a> for running:
* Fun4All_G4_EICDetector_RICH.C: Run a reduced version of the Geant4 simulation (omitting all calorimeters to speed up the processing)
* Fun4All_RICHReco.C: Macro calling the RICH reconstruction with a boolean to switch between "FastSim" and hit/IRT based reconstruction
