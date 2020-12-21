// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef SYNRADANA_H
#define SYNRADANA_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <vector>

class PHCompositeNode;
class Fun4AllHistoManager;

class SynRadAna : public SubsysReco
{
 public:
  SynRadAna(const std::string &filename = "SynRadAna.root");

  virtual ~SynRadAna();

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

  //! embedding ID for the event
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int get_embedding_id() const { return _embedding_id; }
  //
  //! embedding ID for the event
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  void set_embedding_id(int id) { _embedding_id = id; }

  void
  AddNode(const std::string &name)
  {
    _node_postfix.push_back(name);
  }

  void
  AddHit(const std::string &name)
  {
    _node_postfix.push_back(name);
  }

  void
  AddTower(const std::string &name)
  {
    _tower_postfix.push_back(name);
  }

  bool isDoMvtxHits() const
  {
    return do_MVTXHits;
  }

  void setDoMvtxHits(bool doMvtxHits)
  {
    do_MVTXHits = doMvtxHits;
  }

  bool isDoTPCHits() const
  {
    return do_TPCHits;
  }

  void setDoTPCHits(bool doTPCHits)
  {
    do_TPCHits = doTPCHits;
  }

  bool isDoPhoton() const
  {
    return do_photon;
  }

  void setDoPhoton(bool doPhoton)
  {
    do_photon = doPhoton;
  }

 private:
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int _embedding_id;

  double m_eventWeight;

  bool do_photon;
  bool do_MVTXHits;
  bool do_TPCHits;

  std::string m_outputFIle;

  std::vector<std::string> _node_postfix;
  std::vector<std::string> _tower_postfix;
  //! Get a pointer to the default hist manager for QA modules
  Fun4AllHistoManager *
  getHistoManager();

  // common prefix for QA histograms
  std::string get_histo_prefix();
};

#endif  // SYNRADANA_H
