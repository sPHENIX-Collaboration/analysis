# SCorrelatorQAMaker [Under Construction]

A module to generate TNtuples, histograms, functions useful for producing QA plots, determining event/track selection cuts, etc. relevant to
the Cold QCD Energy-Energy Correlator analysis.  Specific routines (such as running the signa-DCA calculation) are compartmentalized into
"plugins."

### TO-DO
  - [Major] Finish skeleton/plumbing
  - [Major] Add sigma-DCA calculation to this module
  - [Major] Move all QA operations from SCorrelatorJetTree to this module
  - [Major] Move weird track check from SCorrelatorJetTree to this module
  - [Major] Move weird cst check from SEnergyCorrelator to this module
