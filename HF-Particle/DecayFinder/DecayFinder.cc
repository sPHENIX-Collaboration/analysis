#include "DecayFinder.h"

/*
 * Find decay topologies in HepMC
 * Cameron Dean
 * 04/06/2021
 */

namespace DecayFinderOptions
{
  bool triggerOnDecay = false;
}

typedef std::pair<int, float> particle_pair;
KFParticle_particleList kfp_list;
std::map<std::string, particle_pair> particleList = kfp_list.getParticleList();

DecayFinder::DecayFinder()
  : SubsysReco("DECAYFINDER")
{
}

DecayFinder::DecayFinder(const std::string &name)
  : SubsysReco(name)
{
}

int DecayFinder::Init(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_SOME)
  {
    std::cout << "DecayFinder name: " << Name() << std::endl;
    std::cout << "Decay descriptor: " << m_decayDescriptor << std::endl;
  }

  int canSearchDecay = parseDecayDescriptor();

  return canSearchDecay;
}

int DecayFinder::process_event(PHCompositeNode *topNode)
{
  bool decayFound = findDecay();

  if (decayFound) m_counter += 1;
   
  if (DecayFinderOptions::triggerOnDecay && !decayFound) 
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "The decay, " << m_decayDescriptor << " was not found in this event, skipping" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  else 
  { 
    return Fun4AllReturnCodes::EVENT_OK;
  }
}

int DecayFinder::End(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_SOME) printInfo();

  return 0;
}

int DecayFinder::parseDecayDescriptor()
{
  bool ddCanBeParsed = true;

  size_t daughterLocator;

  std::string mother;
  std::string intermediate;
  std::string daughter;

  int mother_charge = 0;
  std::vector<int> intermediates_charge;
  std::vector<int> daughters_charge;

  std::string decayArrow = "->";
  std::string chargeIndicator = "^";
  std::string startIntermediate = "(";
  std::string endIntermediate = ")";

  std::string manipulateDecayDescriptor = m_decayDescriptor;

  //Remove all spaces before we begin
  size_t pos; 
  while ((pos = manipulateDecayDescriptor.find(" ")) != std::string::npos) manipulateDecayDescriptor.replace(pos, 1, "");   

  //Check for charge conjugate requirement
  std::string checkForCC = manipulateDecayDescriptor.substr(0, 1) + manipulateDecayDescriptor.substr(manipulateDecayDescriptor.size()-3, 3);
  std::for_each(checkForCC.begin(), checkForCC.end(), [](char & c) {c = ::toupper(c);});

  //Remove the CC check if needed
  if (checkForCC == "[]CC")
  {
    manipulateDecayDescriptor = manipulateDecayDescriptor.substr(1, manipulateDecayDescriptor.size()-4);
    m_getChargeConjugate = true;
  }

//std::cout << manipulateDecayDescriptor << std::endl;

  //Try and find the initial particle
  size_t findMotherEndPoint = manipulateDecayDescriptor.find(decayArrow);
  mother = manipulateDecayDescriptor.substr(0, findMotherEndPoint);
  if (findParticle(mother)) m_mother_ID = particleList.find(mother.c_str())->second.first;
  else ddCanBeParsed = false;
  manipulateDecayDescriptor.erase(0, findMotherEndPoint + decayArrow.length());
  
//std::cout << manipulateDecayDescriptor << std::endl;

//std::cout << "The mother particle is: " << mother << std::endl;
//std::cout << "The mother ID is: " << m_mother_ID << std::endl;

  //Try and find the intermediates
  while ((pos = manipulateDecayDescriptor.find(startIntermediate)) != std::string::npos)
  {
    size_t findIntermediateStartPoint = manipulateDecayDescriptor.find(startIntermediate, pos);
    size_t findIntermediateEndPoint = manipulateDecayDescriptor.find(endIntermediate, pos);
    std::string intermediateDecay = manipulateDecayDescriptor.substr(pos + 1, findIntermediateEndPoint - pos - 1);
//std::cout << "My intermediate string: " << intermediateDecay << std::endl;

    intermediate = intermediateDecay.substr(0, intermediateDecay.find(decayArrow));
//std::cout << "The intermediate particle is: " << intermediate << std::endl;
    if (findParticle(intermediate)) m_intermediates_ID.push_back(particleList.find(intermediate.c_str())->second.first);
    else ddCanBeParsed = false;
    
    //Now find the daughters associated to this intermediate
    int nDaughters = 0;
    intermediateDecay.erase(0, intermediateDecay.find(decayArrow) + decayArrow.length());
    while ((daughterLocator = intermediateDecay.find(chargeIndicator)) != std::string::npos)
    {
//std::cout << "My intermediate string: " << intermediateDecay << std::endl;
      daughter = intermediateDecay.substr(0, daughterLocator);
//std::cout << "daughter: " << daughter << std::endl;
      if (findParticle(daughter)) 
      {
        m_daughters_ID.push_back(particleList.find(daughter.c_str())->second.first);
        std::string daughterChargeString = intermediateDecay.substr(daughterLocator + 1, 1);
	if (daughterChargeString == "+")
        {
          daughters_charge.push_back(+1);
        }
        else if (daughterChargeString == "-")
        {
          daughters_charge.push_back(-1);
        }
        else
        {
          if (Verbosity() >= VERBOSITY_SOME) std::cout << "The charge of " << daughterChargeString << " was not known" << std::endl;
          ddCanBeParsed = false;
        }
      }
      else ddCanBeParsed = false;
      intermediateDecay.erase(0, daughterLocator + 2);
      ++nDaughters;
    } 
//std::cout << "pos: " << pos << "," << "findIntermediateEndPoint: " << findIntermediateEndPoint << std::endl; 
//std::cout << "My intermediate string: " << manipulateDecayDescriptor.substr(pos, findIntermediateEndPoint - pos) << std::endl;
//std::cout << manipulateDecayDescriptor << std::endl;
    //manipulateDecayDescriptor.erase(findIntermediateStartPoint, findIntermediateEndPoint + 1);
    manipulateDecayDescriptor.erase(findIntermediateStartPoint, findIntermediateEndPoint + 1 - findIntermediateStartPoint);
//std::cout << manipulateDecayDescriptor << std::endl;
    m_nTracksFromIntermediates.push_back(nDaughters);
    m_nTracksFromMother += 1;
  }

  while ((daughterLocator = manipulateDecayDescriptor.find(chargeIndicator)) != std::string::npos)
  {
//std::cout << "My decay string: " << manipulateDecayDescriptor << std::endl;
    daughter = manipulateDecayDescriptor.substr(0, daughterLocator);
//std::cout << "daughter: " << daughter << std::endl;
    if (findParticle(daughter))
    {
      m_daughters_ID.push_back(particleList.find(daughter.c_str())->second.first);
      std::string daughterChargeString = manipulateDecayDescriptor.substr(daughterLocator + 1, 1);
      if (daughterChargeString == "+")
      {
        daughters_charge.push_back(+1);
      }
      else if (daughterChargeString == "-")
      {
        daughters_charge.push_back(-1);
      }
      else
      {
        if (Verbosity() >= VERBOSITY_SOME) std::cout << "The charge of " << daughterChargeString << " was not known" << std::endl;
        ddCanBeParsed = false;
      }
    }
    else ddCanBeParsed = false;
    manipulateDecayDescriptor.erase(0, daughterLocator + 2);
    m_nTracksFromMother += 1;
  }

  int trackStart = 0;
  int trackEnd = 0;
  for (unsigned int i = 0; i < m_intermediates_ID.size(); ++i)
  {
    trackStart = trackEnd;
    trackEnd = m_nTracksFromIntermediates[i] + trackStart;

//std::cout << "trackStart: " << trackStart << ", trackEnd: " << trackEnd << std::endl;

    int vtxCharge = 0;

    for (unsigned int j = trackStart; j < trackEnd; ++j)
    {
      vtxCharge += daughters_charge[j];
    }

    intermediates_charge.push_back(vtxCharge);
  }
  
  for (unsigned int i = 0; i < m_daughters_ID.size(); ++i) {std::cout << daughters_charge[i] << std::endl; mother_charge += daughters_charge[i];}

//std::cout << "m_mother_charge: " << mother_charge << std::endl;
  m_mother_ID = mother_charge == 0 ? m_mother_ID : mother_charge*m_mother_ID;
//std::cout << "Mother ID: " << m_mother_ID  << std::endl;
  for (unsigned int i = 0; i < m_intermediates_ID.size(); ++i) 
  {
    m_intermediates_ID[i] = intermediates_charge[i] == 0 ? m_intermediates_ID[i] : intermediates_charge[i]*m_intermediates_ID[i];
//std::cout << "Intermediate ID: " << m_intermediates_ID[i] << std::endl; 
  }
  for (unsigned int i = 0; i < m_daughters_ID.size(); ++i) 
  {
    m_daughters_ID[i] = daughters_charge[i]*m_daughters_ID[i];
//std::cout << "Daughter ID: " << m_daughters_ID[i] << std::endl; 
  }

  if (ddCanBeParsed)
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "Your decay descriptor can be parsed" << std::endl;
    return 0;
  }
  else
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "Your decay descriptor cannot be parsed, " << Name() << " will not be registered" << std::endl;
    return Fun4AllReturnCodes::DONOTREGISTERSUBSYSTEM;
  }
}

bool DecayFinder::findDecay()
{
  bool decayWasFound = false;

  return decayWasFound;
}

bool DecayFinder::findParticle(std::string particle)
{
  bool particleFound = true;
  if (!particleList.count(particle))
  {
    if (Verbosity() >= VERBOSITY_SOME)
    {
      std::cout << "The particle, " << particle << " is not in the particle list" << std::endl;
      std::cout << "Check KFParticle_particleList.cc for a list of available particles" << std::endl;
    }
    particleFound = false;
  }

  return particleFound;
}

void DecayFinder::printInfo()
{
  std::cout << "\n---------------DecayFinder information---------------" << std::endl;
  std::cout << "Module name: " << Name() << std::endl; 
  std::cout << "Decay descriptor: " << m_decayDescriptor << std::endl;
  std::cout << "Number of successful decays: " << m_counter << std::endl;
  std::cout << "---------------------------------------------------\n" << std::endl;
}

