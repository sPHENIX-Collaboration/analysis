/// ---------------------------------------------------------------------------
/*! \file   SLambdaJetHunterConfig.h
 *  \author Derek Anderson
 *  \date   01.18.2024
 *
 *  Configuration struct for 'SLambdaJetHunter' module.
 */
/// ---------------------------------------------------------------------------

#ifndef SLAMBDAJETHUNTERCONFIG_H
#define SLAMBDAJETHUNTERCONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! User options for module
  // --------------------------------------------------------------------------
  struct SLambdaJetHunterConfig {

    // system options
    int    verbosity   {0};
    bool   isDebugOn   {false};
    bool   isEmbed     {false};
    string moduleName  {"SLambdaJetHunter"};
    string outFileName {""};
    string outTreeName {"LambdaJetTree"};

    // jet options
    int    associator {0};
    bool   isCharged  {true};
    float  rJet       {0.4};
    string jetAlgo    {"antikt_algorithm"};
    string jetRecomb  {"pt_scheme"};

    // vertex cuts
    pair<float, float> vzAccept;
    pair<float, float> vrAccept;

    // particle & jet cuts
    pair<Types::ParInfo, Types::ParInfo> parAccept;
    pair<Types::JetInfo, Types::JetInfo> jetAccept;

  };

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
