#ifndef PILEUPREJECTOR_H
#define PILEUPREJECTOR_H

#include <cstdint>
#include <string>

class Gl1Packet;
class LL1Out;
class PHCompositeNode;
class TriggerRunInfo;

class PileupRejector
{
 public:
  PileupRejector() = default;
  ~PileupRejector() = default;
  
  enum PileupCutStrength
  {
    COMFORT = 0, // Maximizes single interactions
    STRICT = 1, // maximizes percentage of single interactions
    DRACONIAN = 2, // Pileup < 1%
  };


  int decodeEvent(PHCompositeNode *topNode);

  bool isPileup();
  void setCutStrength(PileupCutStrength cutstrength) {m_cutstrength = cutstrength; }

  void Print();

 private:

  double m_chargesum{0};
  int m_hitcut{2};
  double m_prodsigma{0};  
  double m_avgsigma{0};
  double m_maxsigma{0};
  double m_proddelta{0};
  double m_avgdelta{0};
  double m_maxdelta{0};
  double m_pileup{0};

  double m_draconian_time_cut{0.1};
  double m_strict_time_cut{0.2};
  double m_comfort_time_cut{0.5};
  double m_draconian_charge_cut{0.5};
  double m_strict_charge_cut{4.5};
  double m_comfort_charge_cut{9.5};

  PileupCutStrength  m_cutstrength{PileupCutStrength::DRACONIAN};

};

#endif /* PILEUPREJECTOR_H */

