/// ---------------------------------------------------------------------------
/*! \file    SEnergyCorrelatorConfig.h
 *  \authors Derek Anderson, Alex Clarke
 *  \date    01.18.2024
 *
 *  Configuration struct for 'SEnergyCorrelator' module.
 */
/// ---------------------------------------------------------------------------

#ifndef SENERGYCORRELATORCONFIG_H
#define SENERGYCORRELATORCONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! User options for module
  // --------------------------------------------------------------------------
  struct SEnergyCorrelatorConfig {

    // general correlator parameters
    //   - FIXME: nPoints should probably be a set
    //     or something... TBD...
    vector<uint32_t>     nPoints    {2};
    pair<double, double> drBinRange {1e-5, 1.};
    uint64_t             nBinsDr    {75};

    // basic options
    int  verbosity     {0};
    int  subEvtOpt     {Const::SubEvtOpt::Everything};
    bool isEmbed       {false};
    bool applyCstCuts  {false};
    bool isDebugOn     {false};
    bool isBatchOn     {false};

    // calculation options
    int  momOption     {0};
    int  normOption    {0};
    bool doLocalCalc   {true};
    bool doGlobalCalc  {true};
    bool doManualCalc  {false};
    bool doPackageCalc {true};
    bool doThreePoint  {false};

    // i/o options
    bool           isInTreeTruth {false};
    string         moduleName    {"SEnergyCorrelator"};
    string         inTreeName    {""};
    string         outFileName   {""};
    vector<string> inFileNames   { {} };

    // smearing options
    bool  doJetSmear      {false};  // turn jet smearing on/off
    bool  doCstSmear      {false};  // turn ANY cst smearing on/off
    bool  doCstPtSmear    {false};  // smear cst pt by ptCstSmear
    bool  doCstThetaSmear {false};  // smear cst theta by thCstSmear
    bool  doCstPhiSmear   {false};  // rotate cst phi around original momentum axis
    bool  doCstEff        {false};  // apply reco efficiency to csts
    float ptJetSmear      {0.};     // jet pt smearing width
    float ptCstSmear      {0.};     // cst pt smearing width
    float thCstSmear      {0.};     // cst theta smearing width
    TF1*  funcEff         {NULL};   // cst reconstruction efficiency

    // subevent options
    bool        selectSubEvts {false};
    vector<int> subEvtsToUse  { {} };

    // histogram bins
    vector<pair<double, double>> rlBins    { {{0.0, 1.0}} };
    vector<pair<double, double>> ptJetBins { {{0., 100.}} };
    vector<pair<double, double>> htEvtBins { {{0., 200.}} };

    // acceptance parameters
    pair<Types::JetInfo, Types::JetInfo> jetAccept;
    pair<Types::CstInfo, Types::CstInfo> cstAccept;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
