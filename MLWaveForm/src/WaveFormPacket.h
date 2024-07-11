// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef WAVEFORMPACKET_H
#define WAVEFORMPACKET_H

#include <fun4all/SubsysReco.h>

#include <string>
#include <fstream>

class PHCompositeNode;

class WaveFormPacket : public SubsysReco
{
 public:

  WaveFormPacket(const std::string &name = "WaveFormPacket");

  ~WaveFormPacket() override = default;

  /** Called for each event.
      This is where you do the real work.
   */
  int process_event(PHCompositeNode *topNode) override;

  /// Called at the end of all processing.
  int End(PHCompositeNode *topNode) override;

 private:

  std::fstream outfile;
};

#endif // WAVEFORM_H
