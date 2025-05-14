#ifndef CHECK_EPD_MAP_H
#define CHECK_EPD_MAP_H

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <iostream>

class PHCompositeNode;
class EpdGeom;
class TowerInfoContainer;
class TProfile2D;
class TH1I;
class TH2F;

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

 private:
    // [0] = south, [1] = north
    TProfile2D* m_profOld [2]{};
    TProfile2D* m_profNew [2]{};
    std::uint64_t m_eventCounter{0};

    TH1I*  m_diffCount = nullptr;   
    TH2F*  m_diffADC   = nullptr;

    std::vector<unsigned> m_keyVec;
   
    static constexpr int NRING = 16;
    static constexpr int NPHI  = 24;
    mutable int m_reverse[2][NRING][NPHI]{};

    TowerInfoContainer *towers;

    std::pair<
    std::vector<unsigned>,   // vkey
    std::vector<int>         // mapped_vals
    > build_key_vector() const;

    void check_map(const std::vector<unsigned>& vkey,
        const std::vector<int> mapped_vals) const;
    
    

    
};

#endif  // CHECK_EPD_MAP_H
