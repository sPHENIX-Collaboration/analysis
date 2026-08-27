# Cascade/KShort Ratio Analysis
This sub-directory is established to archive the code used for the initial determination of the Xi/KS0 ratio in pp collisions. 

The data files for the reconstructed KFParticle yield of the cascade are located here:
`/sphenix/tg/tg01/hf/rosstom/CascadeAnalysisFiles/cascadeKFParticleFiles`
The files for the simulated SvtxEvaluator output used for geometric acceptance and efficiency calculations are located here:
`/sphenix/tg/tg01/hf/rosstom/CascadeAnalysisFiles/cascadeEvaluatorSim`
`/sphenix/tg/tg01/hf/rosstom/CascadeAnalysisFiles/ks0EvaluatorSim`
And the files used for the simulated KFParticle reconstruction of decays within the sPHENIX acceptance are located here:
`/sphenix/tg/tg01/hf/rosstom/CascadeAnalysisFiles/cascadeKFPSim`

Within this directory there should be all relevant code used to produce those files as well as analyze them to determine the raw yield, corrections, and systematic uncertainties for each decay channel.

- corrections: directory storing all of the relevant code used to calculate the corrections to the raw yield
- finalPlots: directory with plotting code used to produce final ratio plots and plots showcasing the corrections and systematics for each bin
- rawYieldFit: directory storing the code used to extract the fit yield for each bin as well as the alternate fit models used for the fit function systematic
- rawYieldReco: directory storing the code used to perform the inital reconstrution of the data sample with KFParticle
- systematics: directory storing the code used to calculate systematic uncertainties

# Contact Info
Thomas Marshall - tmarshall1218@gmail.com
Feel free to reach out with any questions that may arise and I will be happy to help explain any of the code included in this directory
