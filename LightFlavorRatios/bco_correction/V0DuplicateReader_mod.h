#ifndef V0_DUPLICATE_READER_H
#define V0_DUPLICATE_READER_H

#include <TTree.h>

#include <cmath>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class V0DuplicateReader
{
 public:
  enum class ParticleType
  {
    K0s,
    Lambda
  };

  V0DuplicateReader(TTree* tree, ParticleType particleType)
  : m_tree(tree)
  , m_particleType(particleType)
  {
    if (!m_tree)
    {
      throw std::runtime_error("V0DuplicateReader: input tree is null");
    }

    prefix = (m_particleType == ParticleType::K0s) ? "K_S0" : "Lambda0";

    setupBranches();

    //setupCommonBranches();
    //setupParticleBranches();
  }

  void enableDeltaBCOCut(long long minDeltaBCO, long long maxDeltaBCO)
  {
    m_useDeltaBCOCut = true;
    m_minDeltaBCO = minDeltaBCO;
    m_maxDeltaBCO = maxDeltaBCO;
  }

  void disableDeltaBCOCut()
  {
    m_useDeltaBCOCut = false;
  }

  bool loadEntry(Long64_t entry)
  {
    if (!m_tree) return false;

    m_tree->GetEntry(entry);

    Long64_t m_bco = get<Long64_t>("Collision_BCO");
    Long64_t m_eventBCO = get<Long64_t>("GL1_BCO");
    int m_runNumber = get<int>("runNumber");
    
    m_deltaBCO = (m_bco >= 0 && m_eventBCO >= 0) ? (m_bco - m_eventBCO) : -1;

    if (m_useDeltaBCOCut)
    {
      m_passesDeltaBCO = (m_deltaBCO >= m_minDeltaBCO && m_deltaBCO < m_maxDeltaBCO);
    }
    else
    {
      m_passesDeltaBCO = true;
    }

    if (!m_passesDeltaBCO)
    {
      m_currentEntryIsUnique = false;
      return true;
    }

    std::vector<std::string> candidateBranches = {
      "mass",
      "pT",
      "pseudorapidity",
      "phi",
      "decayLength",
      "DIRA",
      "IP",
      "x",
      "y",
      "z",
      "chi2"
    };

    std::vector<float> candidateVariables;

    for(std::string name : candidateBranches)
    {
      candidateVariables.push_back(get<float>(prefix+"_"+name));
    }

    const std::string eventKey =
      std::to_string(m_runNumber) + "_" + std::to_string(m_bco);

    const std::string candidateKey = buildCandidateKey(candidateVariables);
    const std::string fullKey = eventKey + "__" + candidateKey;

    const auto [it, inserted] = m_seenCandidateKeys.insert(fullKey);

    if (inserted)
    {
      ++m_numberOfUniqueCandidates;
      m_currentEntryIsUnique = true;
    }
    else
    {
      ++m_numberOfDuplicateCandidates;
      m_currentEntryIsUnique = false;
    }

    return true;
  }

  bool isCurrentEntryUnique() const
  {
    return m_currentEntryIsUnique;
  }

  bool passesDeltaBCOCut() const
  {
    return m_passesDeltaBCO;
  }

  long long deltaBCO() const
  {
    return m_deltaBCO;
  }

  void reset()
  {
    m_seenCandidateKeys.clear();
    m_numberOfUniqueCandidates = 0;
    m_numberOfDuplicateCandidates = 0;
    m_currentEntryIsUnique = false;
    m_deltaBCO = -1;
    m_passesDeltaBCO = true;
  }

  Long64_t entries() const
  {
    return m_tree ? m_tree->GetEntries() : 0;
  }
/*
  int runNumber() const { return m_runNumber; }
  Long64_t bco() const { return m_bco; }
  Long64_t eventBCO() const { return m_eventBCO; }
  int eventNumber() const { return m_eventNumber; }

  float mass() const { return m_mass; }
  float pt() const { return m_pt; }
  float eta() const { return m_eta; }
  float phi() const { return m_phi; }
  float decayLength() const { return m_decayLength; }
  float dira() const { return m_dira; }
  float ip() const { return m_ip; }
  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }
  float chi2() const { return m_chi2; }
  float rapidity() const { return m_rapidity; }
*/
  long long numberOfUniqueCandidates() const
  {
    return m_numberOfUniqueCandidates;
  }

  long long numberOfDuplicateCandidates() const
  {
    return m_numberOfDuplicateCandidates;
  }

  template<typename T> 
  T get(const std::string& name)
  {
    T val = *(T*)leafmap[name.c_str()];
    return val;
  }

 private:
  void setupBranches()
  {
    TObjArray* branches = m_tree->GetListOfBranches();
    size_t nbranches = m_tree->GetNbranches();
    for(size_t i=0;i<nbranches;i++)
    {
      std::string branchname = branches->At(i)->GetName();
      leafmap[branchname] = m_tree->GetLeaf(branchname.c_str())->GetValuePointer();
    }
  }

/*
  void setupCommonBranches()
  {
    m_tree->SetBranchAddress("runNumber", &m_runNumber);
    m_tree->SetBranchAddress("eventNumber", &m_eventNumber);
    m_tree->SetBranchAddress("BCO", &m_bco);
    m_tree->SetBranchAddress("event_bco", &m_eventBCO);
  }

  void setupParticleBranches()
  {
    const std::string prefix = (m_particleType == ParticleType::K0s) ? "K_S0" : "Lambda0";

    m_tree->SetBranchAddress((prefix + "_mass").c_str(), &m_mass);
    m_tree->SetBranchAddress((prefix + "_pT").c_str(), &m_pt);
    m_tree->SetBranchAddress((prefix + "_pseudorapidity").c_str(), &m_eta);
    m_tree->SetBranchAddress((prefix + "_phi").c_str(), &m_phi);
    m_tree->SetBranchAddress((prefix + "_decayLength").c_str(), &m_decayLength);
    m_tree->SetBranchAddress((prefix + "_DIRA").c_str(), &m_dira);
    m_tree->SetBranchAddress((prefix + "_IP").c_str(), &m_ip);
    m_tree->SetBranchAddress((prefix + "_x").c_str(), &m_x);
    m_tree->SetBranchAddress((prefix + "_y").c_str(), &m_y);
    m_tree->SetBranchAddress((prefix + "_z").c_str(), &m_z);
    m_tree->SetBranchAddress((prefix + "_chi2").c_str(), &m_chi2);
    m_tree->SetBranchAddress((prefix + "_rapidity").c_str(), &m_rapidity);
  }
*/
  static std::string buildCandidateKey(const std::vector<float>& candidateVariables)
  {
    std::string key;
    key.reserve(candidateVariables.size() * 16);

    for (float value : candidateVariables)
    {
      const long long roundedValue = llround(value * 1e6);
      key += std::to_string(roundedValue);
      key += "_";
    }

    return key;
  }

 private:
  TTree* m_tree = nullptr;
  ParticleType m_particleType;

  std::string prefix;

  std::map<std::string,void*> leafmap;

  std::set<std::string> m_seenCandidateKeys;

  long long m_numberOfUniqueCandidates = 0;
  long long m_numberOfDuplicateCandidates = 0;
  bool m_currentEntryIsUnique = false;

  bool m_useDeltaBCOCut = false;
  long long m_minDeltaBCO = 0;
  long long m_maxDeltaBCO = 350;
  long long m_deltaBCO = -1;
  bool m_passesDeltaBCO = true;
/*
  int m_runNumber = -1;
  int m_eventNumber = -1;
  Long64_t m_bco = -1;
  Long64_t m_eventBCO = -1;

  float m_mass = 0.0;
  float m_pt = 0.0;
  float m_eta = 0.0;
  float m_phi = 0.0;
  float m_decayLength = 0.0;
  float m_dira = 0.0;
  float m_ip = 0.0;
  float m_x = 0.0;
  float m_y = 0.0;
  float m_z = 0.0;
  float m_chi2 = 0.0;
  float m_rapidity = 0.0;
*/
};

#endif
