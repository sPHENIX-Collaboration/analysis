// ----------------------------------------------------------------------------
// 'GenInfo.cc'
// Derek Anderson
// 03.06.2024
//
// Utility class to hold event-level generator
// information.
// ----------------------------------------------------------------------------

#define SCORRELATORUTILITIES_GENINFO_CC

// class definition
#include "GenInfo.h"

// make comon namespaces implicit
using namespace std;



namespace SColdQcdCorrelatorAnalysis {


  // internal methods ---------------------------------------------------------

  void Types::GenInfo::Minimize() {

    // minimize atomic members
    nChrgPar = -1 * numeric_limits<int>::max();
    nNeuPar  = -1 * numeric_limits<int>::max();
    eSumChrg = -1. * numeric_limits<double>::max();
    eSumNeu  = -1. * numeric_limits<double>::max();

    // minimize parton info
    partons = make_pair(ParInfo(Const::Init::Minimize), ParInfo(Const::Init::Minimize));
    return;

  }  // end 'Minimize()'



  void Types::GenInfo::Maximize() {

    // maximize atomic members
    nChrgPar = numeric_limits<int>::max();
    nNeuPar  = numeric_limits<int>::max();
    eSumChrg = numeric_limits<double>::max();
    eSumNeu  = numeric_limits<double>::max();

    // maximize parton info
    partons = make_pair(ParInfo(Const::Init::Maximize), ParInfo(Const::Init::Maximize));
    return;

  }  // end 'Maximize()'



  // public methods -----------------------------------------------------------

  void Types::GenInfo::Reset() {

    // reset embed flag
    isEmbed = false;

    // maximize everyting else
    Maximize();
    return;

  }  // end 'Reset()'



  void Types::GenInfo::SetInfo(
    PHCompositeNode* topNode,
    const bool embed,
    const vector<int> evtsToGrab
  ) {

    // set embed flag
    isEmbed = embed;

    // set parton info
    isEmbed  = embed;
    if (isEmbed) {
      partons.first  = Tools::GetPartonInfo(topNode, Const::SubEvt::EmbedSignal, Const::HardScatterStatus::First);
      partons.second = Tools::GetPartonInfo(topNode, Const::SubEvt::EmbedSignal, Const::HardScatterStatus::Second);

    } else {
      partons.first  = Tools::GetPartonInfo(topNode, Const::SubEvt::NotEmbedSignal, Const::HardScatterStatus::First);
      partons.second = Tools::GetPartonInfo(topNode, Const::SubEvt::NotEmbedSignal, Const::HardScatterStatus::Second);
    }

    // get sums
    nChrgPar = Tools::GetNumFinalStatePars(topNode, evtsToGrab, Const::Subset::Charged);
    nNeuPar  = Tools::GetNumFinalStatePars(topNode, evtsToGrab, Const::Subset::Neutral);
    eSumChrg = Tools::GetSumFinalStateParEne(topNode, evtsToGrab, Const::Subset::Charged);
    eSumNeu  = Tools::GetSumFinalStateParEne(topNode, evtsToGrab, Const::Subset::Neutral);
    return;

  }  // end 'SetInfo(PHCompositeNode*, vector<int>)'



  // static methods -----------------------------------------------------------

  vector<string> Types::GenInfo::GetListOfMembers() {

    // get parton members
    vector<string> membersParA = ParInfo::GetListOfMembers();
    vector<string> membersParB = ParInfo::GetListOfMembers();

    // add tags to parton members
    Interfaces::AddTagToLeaves("PartonA", membersParA);
    Interfaces::AddTagToLeaves("PartonB", membersParB);

    // construct list
    vector<string> members = {
      "nChrgPar",
      "nNeuPar",
      "isEmbed",
      "eSumChrg",
      "eSumNeu"
    };
    Interfaces::CombineLeafLists(membersParA, members);
    Interfaces::CombineLeafLists(membersParB, members);
    return members;

  }  // end 'GetListOfMembers()'



  // ctor/dtor ----------------------------------------------------------------

  Types::GenInfo::GenInfo() {

    /* nothing to do */

  }  // end ctor()



  Types::GenInfo::~GenInfo() {

    /* nothing to do */

  }  // end dtor()



  Types::GenInfo::GenInfo(const Const::Init init) {

    switch (init) {
      case Const::Init::Minimize:
        Minimize();
        break;
      case Const::Init::Maximize:
        Maximize();
        break;
      default:
        Maximize();
        break;
    }

  }  // end ctor(Const::init)



  Types::GenInfo::GenInfo(PHCompositeNode* topNode, const bool embed, vector<int> evtsToGrab) {

    SetInfo(topNode, embed, evtsToGrab);

  }  // end ctor(PHCompositeNode*, int, vector<int>)'

}  // end SColdQcdCorrelatorAnalysis namespace

// end ------------------------------------------------------------------------

