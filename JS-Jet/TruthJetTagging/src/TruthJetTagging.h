// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef TRUTHJETTAGGING_H
#define TRUTHJETTAGGING_H

#include <fun4all/SubsysReco.h>
#include <g4jets/Jet.h>
#include <g4jets/Jetv1.h>
#include <g4main/PHG4TruthInfoContainer.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
#pragma GCC diagnostic pop
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <string>
#include <vector>
class PHCompositeNode;

class TruthJetTagging : public SubsysReco
{
 public:

  TruthJetTagging(const std::string &name = "TruthJetTagging");

  ~TruthJetTagging() override;

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for first event when run number is known.
      Typically this is where you may want to fetch data from
      database, because you know the run number. A place
      to book histograms which have to know the run number.
   */
  int InitRun(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of each run.
  int EndRun(const int runnumber) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

  /// Reset
  int Reset(PHCompositeNode * /*topNode*/) override;

  void Print(const std::string &what = "ALL") const override;
  


  void add_algo(std::string algoname)
  {
    _algorithms.push_back(algoname);
  }
 
  void add_radius(float radius)
  {
    _radii.push_back(radius);
  } 
  void do_photon_tagging(bool dotag)
  {
    _do_photon_tagging = dotag;
  }
  void do_hf_tagging(bool dotag)
  {
    _do_hf_tagging = dotag;
  }
  void set_embedding_id(int id)
  {
    _embedding_id = id;
  }

 float
  deltaR(float eta1, float eta2, float phi1, float phi2)
  {
    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if (dphi > +3.14159)
      dphi -= 2 * 3.14159;
    if (dphi < -3.14159)
      dphi += 2 * 3.14159;

    return sqrt(pow(deta, 2) + pow(dphi, 2));

  }



  float TruthPhotonTagging(PHG4TruthInfoContainer* truthnode, Jet* tjet);
  int hadron_tagging(Jet* this_jet, HepMC::GenEvent* theEvent, const double match_radius);


 private:

  std::vector<std::string> _algorithms;
  std::vector<float> _radii;
  bool _do_photon_tagging;
  bool _do_hf_tagging;
  int _embedding_id;

};

#endif // TRUTHJETTAGGING_H
