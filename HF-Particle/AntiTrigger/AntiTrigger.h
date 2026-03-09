#ifndef ANTITRIGGER_H
#define ANTITRIGGER_H

//sPHENIX stuff
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <phool/getClass.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/IteratorRange.h> 
#include <HepMC/SimpleVector.h>
#pragma GCC diagnostic pop

#include <TDatabasePDG.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class AntiTrigger : public SubsysReco
{
 public:
  AntiTrigger();

  explicit AntiTrigger(const std::string &name);

  virtual ~AntiTrigger(){}

  int Init(PHCompositeNode *topNode);

  int process_event(PHCompositeNode *topNode);

  int End(PHCompositeNode *topNode);

  int parseParticleList();

  bool findParticle(PHCompositeNode *topNode);

  bool particleIsInList(std::string particle);

  void printInfo();

  //User configuration
  void setParticleList(std::vector<std::string> particleList) { m_particleList = particleList; }
 
 private:

  PHHepMCGenEventMap *m_geneventmap = NULL;
  PHHepMCGenEvent *m_genevt = NULL;

  int m_counter = 0;

  std::vector<std::string> m_particleList;
  std::vector<int> m_particleIDs;
};

#endif  //ANTITRIGGER_H
