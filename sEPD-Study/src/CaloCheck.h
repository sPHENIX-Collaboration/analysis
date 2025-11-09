// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef CALOCHECK_H
#define CALOCHECK_H

#include <fun4all/SubsysReco.h>

// -- c++
#include <string>

class PHCompositeNode;

class CaloCheck : public SubsysReco
{
 public:
  explicit CaloCheck(const std::string &name = "CaloCheck");

  /** Called during initialization.
      Typically this is where you can book histograms, and e.g.
      register them to Fun4AllServer (so they can be output to file
      using Fun4AllServer::dumpHistos() method).
   */
  int Init(PHCompositeNode *topNode) override;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Clean up internals after each event.
  int ResetEvent(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

 private:
};

#endif  // CALOCHECK_H
