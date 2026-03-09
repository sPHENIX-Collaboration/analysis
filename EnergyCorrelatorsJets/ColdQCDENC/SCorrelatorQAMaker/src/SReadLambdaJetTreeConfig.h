// ----------------------------------------------------------------------------
// 'ReadLambdaJetTreeConfig.h'
// Derek Anderson
// 02.19.2024
//
// SCorrelatorQAMaker plugin to read lambda-tagged jet
// trees and draw plots and calculate the total no.
// of lambdas.
// ----------------------------------------------------------------------------

#ifndef SCORRELATORQAMAKER_SREADLAMBDAJETTREECONFIG_H
#define SCORRELATORQAMAKER_SREADLAMBDAJETTREECONFIG_H

// make common namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {

  // SReadLambdaJetTreeConfig definition --------------------------------------

  struct SReadLambdaJetTreeConfig {

    // i/o options
    string inFileName {""};
    string inTreeName {""};

    // calculation options
    double ptJetMin  {0.2};
    double ptLamMin  {0.2};
    double etaJetMax {0.7};
    double etaLamMax {1.1};
    double zLeadMin  {0.5};

    // histogram options
    bool     centerTitle  {true};
    float    defOffX      {1.0};
    float    defOffY      {1.2};
    float    defOffZ      {0.7};
    float    defTitleX    {0.04};
    float    defTitleY    {0.04};
    float    defTitleZ    {0.04};
    float    defLabelX    {0.03};
    float    defLabelY    {0.03};
    float    defLabelZ    {0.03};
    uint16_t defLineStyle {1};
    uint16_t defFillStyle {0};
    uint16_t defMarkStyle {20};
    uint16_t defHistColor {923};
    uint16_t defHistFont  {42};

  };  // end SReadLambdaJetTreeConfig

}  // end SColdQcdCorrelatorAnalysis namespace

#endif

// end ------------------------------------------------------------------------
