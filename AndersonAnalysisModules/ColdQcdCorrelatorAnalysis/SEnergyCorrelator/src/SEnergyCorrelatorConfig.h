// ----------------------------------------------------------------------------
// 'SEnergyCorrelatorConfig.h'
// Derek Anderson
// 01.18.2024
//
// Configuration struct for 'SEnergyCorrelator' module.
// ----------------------------------------------------------------------------

#ifndef SENERGYCORRELATORCONFIG_H
#define SENERGYCORRELATORCONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SEnergyCorrelatorConfig definition ---------------------------------------

  struct SEnergyCorrelatorConfig {

    // system options
    int            verbosity     = 0;
    bool           isDebugOn     = false;
    bool           isBatchOn     = false;
    bool           isEmbed       = false;
    bool           isStandalone  = true;
    bool           isInTreeTruth = false;
    bool           isLegacyInput = true;
    string         moduleName    = "SEnergyCorrelator";
    string         inNodeName    = "";
    string         inTreeName    = "";
    string         outFileName   = "";
    vector<string> inFileNames;

    // calculation options
    int                          subEvtOpt     = Const::SubEvtOpt::Everything;
    bool                         doPackageCalc = true;
    bool                         applyCstCuts  = false;
    bool                         selectSubEvts = false;
    uint64_t                     nBinsDr       = 75;
    vector<int>                  subEvtsToUse  = {};
    vector<uint32_t>             nPoints       = {2};
    vector<pair<double, double>> ptJetBins     = {{0., 100.}};
    pair<double, double>         drBinRange    = {1e-5, 1.};
    bool modCsts = false;
    float effVal = 1;
    float pTSmear = 0;
    float theta = 0;
    bool modJets = false;
    float jetPtSmear = 0;

    //Manual Calculation option
    bool doManualCalc = false;
    int mom_option = 0;
    int norm_option = 0;

    // acceptance parameters
    pair<Types::JetInfo, Types::JetInfo> jetAccept;
    pair<Types::CstInfo, Types::CstInfo> cstAccept;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
