/// ---------------------------------------------------------------------------
/*! \file   TwrInterfaces.cc
 *  \author Derek Anderson
 *  \date   08.04.2024
 *
 *  Calorimeter tower-related interfaces.
 */
/// ---------------------------------------------------------------------------

#define SCORRELATORUTILITIES_TWRINTERFACES_CC

// namespace definition 
#include "TwrInterfaces.h"

// make common namespaces implicit
using namespace std;
using namespace findNode;



// tower interfaces ===========================================================

namespace SColdQcdCorrelatorAnalysis {

  // --------------------------------------------------------------------------
  //! Get raw tower container from node tree
  // --------------------------------------------------------------------------
  RawTowerContainer* Interfaces::GetRawTowerStore(PHCompositeNode* topNode, const string node) {

    // grab clusters
    RawTowerContainer* towerStore = getClass<RawTowerContainer>(topNode, node.data());
    if (!towerStore) {
      cout << PHWHERE
           << "PANIC: " << node << " node is missing!"
           << endl;
      assert(towerStore);
    }
    return towerStore;

  }  // end 'GetRawTowerStore(PHCompositeNode*, string)'



  // --------------------------------------------------------------------------
  //! Get tower info container from node tree
  // --------------------------------------------------------------------------
  TowerInfoContainer* Interfaces::GetTowerInfoStore(PHCompositeNode* topNode, const string node) {

    // grab clusters
    TowerInfoContainer* towerStore = getClass<TowerInfoContainer>(topNode, node.data());
    if (!towerStore) {
      cout << PHWHERE
           << "PANIC: " << node << " node is missing!"
           << endl;
      assert(towerStore);
    }
    return towerStore;

  }  // end 'GetTowerInfoStore(PHCompositeNode*, string)'



  // --------------------------------------------------------------------------
  //! Get raw towers from container
  // --------------------------------------------------------------------------
  RawTowerContainer::ConstRange Interfaces::GetRawTowers(PHCompositeNode* topNode, const string store) {

    // get store and return range of clusters
    RawTowerContainer* towerStore = GetRawTowerStore(topNode, store);
    return towerStore -> getTowers();

  }  // end 'GetRawTowers(PHCompositeNode*, string)'



  // --------------------------------------------------------------------------
  //! Get tower geometry container from node tree
  // --------------------------------------------------------------------------
  RawTowerGeomContainer* Interfaces::GetTowerGeometries(PHCompositeNode* topNode, const string node) {

    // grab container
    RawTowerGeomContainer* geometries = getClass<RawTowerGeomContainer>(topNode, node.data());
    if (!geometries) {
      cout << PHWHERE
           << "PANIC: " << node << " node is missing!"
           << endl;
      assert(geometries);
    }
    return geometries;

  }  // end 'GetTowerGeomtries(PHCompositeNode*, string)'



  // --------------------------------------------------------------------------
  //! Get a specific tower geometry
  // --------------------------------------------------------------------------
  RawTowerGeom* Interfaces::GetTowerGeometry(PHCompositeNode* topNode, const int subsys, const int rawKey) {

    // grab geometry container
    RawTowerGeomContainer* geometries = GetTowerGeometries( 
      topNode,
      Const::MapIndexOntoTowerGeom()[ subsys ]
    );

    // now grab geometry associated with RawTower key
    RawTowerGeom* geometry = geometries -> get_tower_geometry(rawKey);
    if (!geometry) {
      cout << PHWHERE
           << "PANIC: geometry is missing for key " << rawKey << " from node " << Const::MapIndexOntoTowerGeom()[ subsys ] << "!"
           << endl;
      assert(geometry);
    }
    return geometry;

  }  // end 'GetTowerGeometry(PHCompositeNode*, int, int)'

}  // end SColdQcdCorrealtorAnalysis namespace

// end ------------------------------------------------------------------------
