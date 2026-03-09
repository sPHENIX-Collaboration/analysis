#include "AntiTrigger.h"

/*
 * Find decay topologies in HepMC
 * Cameron Dean
 * 04/06/2021
 */

AntiTrigger::AntiTrigger()
  : SubsysReco("ANTITRIGGER")
{
}

AntiTrigger::AntiTrigger(const std::string &name)
  : SubsysReco(name)
{
}

int AntiTrigger::Init(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_SOME)
  {
    std::cout << "AntiTrigger name: " << Name() << std::endl;

    int numberOfParticles = m_particleList.size();

    std::cout << "Particles to reject: "; 
    for (int i = 0; i < numberOfParticles; ++i)
    {
      std::cout << m_particleList[i]; 
      if (i != numberOfParticles - 1) std::cout << ", ";
    }
    std::cout << std::endl;
  }

  int canSearchDecay = parseParticleList();

  return canSearchDecay;
}

int AntiTrigger::process_event(PHCompositeNode *topNode)
{
  bool particleFound = findParticle(topNode);

  if (particleFound) 
  {
    m_counter += 1;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  else 
  { 
    return Fun4AllReturnCodes::EVENT_OK;
  }
}

int AntiTrigger::End(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_SOME) printInfo();

  return 0;
}

int AntiTrigger::parseParticleList()
{
  bool listCanBeParsed = true;

  for (int i = 0; i < m_particleList.size(); ++i)
  {  
    if (particleIsInList(m_particleList[i])) 
    {
      m_particleIDs.push_back(TDatabasePDG::Instance()->GetParticle(m_particleList[i].c_str())->PdgCode());
    }
    else
    {
      listCanBeParsed = false;
    }
  }

  if (listCanBeParsed)
  {
    if (Verbosity() >= VERBOSITY_MORE) std::cout << "Your particle list can be parsed" << std::endl;
    return 0;
  }
  else
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "Your particle list cannot be parsed, " << Name() << " will not be registered" << std::endl;
    return Fun4AllReturnCodes::DONOTREGISTERSUBSYSTEM;
  }
}

bool AntiTrigger::findParticle(PHCompositeNode *topNode)
{
  bool particleWasFound = false;

  m_geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!m_geneventmap)
  {
    std::cout << "AntiTrigger: Missing node PHHepMCGenEventMap" << std::endl;
    return 0;
  }

  m_genevt = m_geneventmap->get(1);
  if (!m_genevt)
  {
    std::cout <<  "AntiTrigger: Missing node PHHepMCGenEvent" << std::endl;
    return 0;
  }

  HepMC::GenEvent* theEvent = m_genevt->getEvent();

  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin(); p != theEvent->particles_end(); ++p)
  {
    if (std::find(m_particleIDs.begin(), m_particleIDs.end(), abs((*p)->pdg_id())) != m_particleIDs.end())
    {
      particleWasFound = true;
      if (Verbosity() >= VERBOSITY_MORE) std::cout << "This event will be rejected due to: " << (*p)->pdg_id() << std::endl;
    }
  }

  return particleWasFound;
}

bool AntiTrigger::particleIsInList(std::string particle)
{
  bool particleFound = true;
  if (!TDatabasePDG::Instance()->GetParticle(particle.c_str()))
  {
    if (Verbosity() >= VERBOSITY_SOME)
    {
      std::cout << "The particle, " << particle << " is not in the particle list" << std::endl;
      std::cout << "Check TDatabasePDG for a list of available particles" << std::endl;
    }
    particleFound = false;
  }

  return particleFound;
}

void AntiTrigger::printInfo()
{
  std::cout << "\n---------------AntiTrigger information---------------" << std::endl;
  std::cout << "AntiTrigger name: " << Name() << std::endl;
  int numberOfParticles = m_particleList.size();
  std::cout << "Particles to reject: "; 
  for (int i; i < numberOfParticles; ++i)
  {
    std::cout << m_particleList[i]; 
    if (i != numberOfParticles - 1) std::cout << ", ";
  }
  std::cout << std::endl;
  std::cout << "Number of events skipped: " << m_counter << std::endl;
  std::cout << "-----------------------------------------------------\n" << std::endl;
}
