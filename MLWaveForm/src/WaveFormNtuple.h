// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef WAVEFORMNTUPLE_H
#define WAVEFORMNTUPLE_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <fstream>

class PHCompositeNode;
class TFile;
class TNtuple;

class WaveFormNtuple : public SubsysReco
{
 public:

  WaveFormNtuple(const std::string &name = "WaveFormNtuple");

  ~WaveFormNtuple() override = default;

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

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

 private:
  TFile *outfile {nullptr};
  TNtuple *ntup {nullptr};
};

#endif // WAVEFORM_H
