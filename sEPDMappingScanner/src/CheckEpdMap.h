#ifndef CHECK_EPD_MAP_H
#define CHECK_EPD_MAP_H

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <iostream>
#include <tuple>

class PHCompositeNode;
class EpdGeom;
class TowerInfoContainer;
class TProfile2D;
class TH1I;
class TH2I;
class TH2F;
class TH1F;

/**
 * sanity check for the sEPD channel map.
 *
 * When the run starts (InitRun) the module:
 *   1. pulls the EpdGeom node,
 *   2. builds the channel-number -> key vector from the CDBTTree,
 *   3. compares with TowerInfoDefs::encode_epd().
 *
 * 
 */
class CheckEpdMap : public SubsysReco
{
 public:
  explicit CheckEpdMap(const std::string& name = "CheckEpdMap");
  ~CheckEpdMap() override = default;


  int InitRun(PHCompositeNode* topNode) override;

 
  int process_event(PHCompositeNode* topNode) override;
  int End(PHCompositeNode*) override;
  void PrintReverseTable(std::ostream &os = std::cout) const;

  void setSimulationMode(bool b) { m_simulationMode = b; }
  void SetOutputFile(const std::string &fname) { outputFile = fname; }
  //void   SetJobIndex(int idx) { jobIndex = idx; }

 private:
    // [0] = south, [1] = north
    TProfile2D* m_profOld [2]{};
    TProfile2D* m_profNew [2]{};
    std::uint64_t m_eventCounter{0};

    TH1I*  m_diffCount = nullptr;   
    TH2F*  m_diffADC   = nullptr;
    TH1F*  m_hitCenterDist = nullptr;
    //TH2I*  hSimMatch[2];

    TH2F*  m_simMatchPolarS01 = nullptr;
    TH2F*  m_simMatchPolarN01 = nullptr;

    TH2F*  m_simMatchPolarS = nullptr;
    TH2F*  m_simMatchPolarN = nullptr;

    

    std::vector<unsigned> m_keyVec;
   
    static constexpr int NRING = 16;
    static constexpr int NPHI  = 24;
    mutable int m_reverse[2][NRING][NPHI]{};

    static constexpr int NSEC = 12;
    static constexpr int NTILE = 31;
    //map from (arm,sector,tile_id) -> (arm,ring,phi)
    std::tuple<uint32_t,uint32_t,uint32_t> m_tile_bin[2][NSEC][NTILE]; 
    std::tuple<uint32_t,uint32_t,uint32_t> m_module_coords[2][NRING][NPHI];
    //int m_primaryTrackID = std::numeric_limits<unsigned int>::max();;

    TH2F* m_tileHist[2][NRING][NPHI] = {};
    TowerInfoContainer *towers;
    EpdGeom *geom = nullptr;
    bool m_simulationMode = false;
    std::string          outputFile;


    std::pair<
    std::vector<unsigned>,   // vkey
    std::vector<int>         // mapped_vals
    > build_key_vector() const;

    void check_map(const std::vector<unsigned>& vkey,
        const std::vector<int> mapped_vals) const;

    int processEventDataMode(PHCompositeNode *topNode);
    
    int processEventSimulationMode(PHCompositeNode *topNode);

    void BuildSimMap();
    void InitializeSimMatchMap();

    int Getphimap(int phiindex);

    int Getrmap(int rindex);
    

    
};

#endif  // CHECK_EPD_MAP_H
