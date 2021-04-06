#ifndef HFTRIGGER_H
#define HFTRIGGER_H

//sPHENIX stuff
#include <fun4all/Fun4AllReturnCodes.h>
#include <kfparticle_sphenix/KFParticle_particleList.h> 
#include <fun4all/SubsysReco.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

//class KFParticle_particleList;

class DecayFinder : public SubsysReco
{
 public:
  DecayFinder();

  explicit DecayFinder(const std::string &name);

  virtual ~DecayFinder(){}

  int Init(PHCompositeNode *topNode);

  int process_event(PHCompositeNode *topNode);

  int End(PHCompositeNode *topNode);

  int parseDecayDescriptor();

  bool findDecay();

  bool findParticle(std::string particle);

  void printInfo();

  //User configuration
  void setDecayDescriptor(std::string decayDescriptor) { m_decayDescriptor = decayDescriptor; }
 
 private:

  int m_counter = 0;

  bool m_getChargeConjugate = false;

  std::string m_decayDescriptor;

  int m_mother_ID;
  std::vector<int> m_intermediates_ID;
  std::vector<int> m_daughters_ID;

  int m_nTracksFromMother;
  std::vector<int> m_nTracksFromIntermediates;
};

#endif  //HFTRIGGER_H
