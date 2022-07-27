#include <iostream>

// --- header for this clase
#include <TreeMaker.h>

// --- basic sPhenix environment stuff
#include <fun4all/Fun4AllServer.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// --- jet specific stuff
#include <g4jets/JetMap.h>
#include <g4jets/JetMapV1.h>

// --- calorimeter clusters
#include <calobase/RawClusterContainer.h>

using std::cout;
using std::endl;



int TreeMaker::CreateNode(PHCompositeNode *topNode)
{

  cout << "TreeMaker::CreateNode called" << endl;

  PHNodeIterator iter(topNode);

  // -----------------------------
  // --- Make the new jet node ---
  // -----------------------------

  // --- Look for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if ( !dstNode )
    {
      cout << PHWHERE << "DST node not found, doing nothing." << endl;
      return -2;
    }

  // --- Look for the ANTIKT node
  PHCompositeNode *antiktNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "ANTIKT"));
  if ( !antiktNode )
    {
      cout << PHWHERE << "ANTIKT node not found, doing nothing." << endl;
      return -2;
    }

  // --- Look for the TOWER node
  PHCompositeNode *towerNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "TOWER"));
  if ( !towerNode )
    {
      cout << PHWHERE << "TOWER node not found, doing nothing." << endl;
      return -2;
    }

  // --- Check if the new node exists (it should not) and then store the new jet collection
  JetMap* check_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_Mod_r02");
  if ( !check_jets )
    {
      // --- If the new node doesn't already exist (good) then make it
      cout << "TreeMaker::CreateNode : creating AntiKt_Tower_Mod_r02 node " << endl;
      JetMap *mod_jets = new JetMapV1();
      PHIODataNode<PHObject> *modjetNode = new PHIODataNode<PHObject>( mod_jets, "AntiKt_Tower_Mod_r02", "PHObject");
      // --- node structure for jets is DST -> ANTIKT -> TOWER (or CLUSTER or TRACK or whatever) -> specific jet node
      towerNode->addNode(modjetNode);
    }
  else
    {
      // --- If the new node already exists (bad) issue a warning and then do nothing
      cout << "TreeMaker::CreateNode : AntiKt_Tower_Mod_r02 already exists! " << endl;
    }

  // --------------------------------------
  // --- Make the new cemc cluster node ---
  // --------------------------------------

  // --- Look for the CEMC node (main node for us)
  PHCompositeNode *cemcNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "CEMC"));
  if ( !cemcNode )
    {
      cout << PHWHERE << "CEMC node not found, doing nothing." << endl;
      return -2;
    }

  // --- Check if the new node exists (it should not) and then store the new cluster collection
  RawClusterContainer* check_cemc_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC_MOD");
  if ( !check_cemc_clusters )
    {
      // --- If the new node doesn't already exist (good) then make it
      cout << "TreeMaker::CreateNode : creating CLUSTER_CEMC_MOD node " << endl;
      RawClusterContainer *mod_cemc_clusters = new RawClusterContainer();
      PHIODataNode<PHObject> *clusterNode = new PHIODataNode<PHObject>( mod_cemc_clusters, "CLUSTER_CEMC_MOD", "PHObject");
      // --- node structure for clusters is CEMC -> CLUSTER_CEMC
      cemcNode->addNode(clusterNode);
    }
  else
    {
      // --- If the new node already exists (bad) issue a warning and then do nothing
      cout << "TreeMaker::CreateNode : CLUSTER_CEMC_MOD already exists! " << endl;
    }

  // --- Look for the HCALIN node (main node for us)
  PHCompositeNode *hcalinNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "HCALIN"));
  if ( !hcalinNode )
    {
      cout << PHWHERE << "HCALIN node not found, doing nothing." << endl;
      return -2;
    }

  // --- Check if the new node exists (it should not) and then store the new cluster collection
  RawClusterContainer* check_hcalin_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN_MOD");
  if ( !check_hcalin_clusters )
    {
      // --- If the new node doesn't already exist (good) then make it
      cout << "TreeMaker::CreateNode : creating CLUSTER_HCALIN_MOD node " << endl;
      RawClusterContainer *mod_hcalin_clusters = new RawClusterContainer();
      PHIODataNode<PHObject> *clusterNode = new PHIODataNode<PHObject>( mod_hcalin_clusters, "CLUSTER_HCALIN_MOD", "PHObject");
      // --- node structure for clusters is HCALIN -> CLUSTER_HCALIN
      hcalinNode->addNode(clusterNode);
    }
  else
    {
      // --- If the new node already exists (bad) issue a warning and then do nothing
      cout << "TreeMaker::CreateNode : CLUSTER_HCALIN_MOD already exists! " << endl;
    }

  // --- Look for the HCALOUT node (main node for us)
  PHCompositeNode *hcaloutNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "HCALOUT"));
  if ( !hcaloutNode )
    {
      cout << PHWHERE << "HCALOUT node not found, doing nothing." << endl;
      return -2;
    }

  // --- Check if the new node exists (it should not) and then store the new cluster collection
  RawClusterContainer* check_hcalout_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT_MOD");
  if ( !check_hcalout_clusters )
    {
      // --- If the new node doesn't already exist (good) then make it
      cout << "TreeMaker::CreateNode : creating CLUSTER_HCALOUT_MOD node " << endl;
      RawClusterContainer *mod_hcalout_clusters = new RawClusterContainer();
      PHIODataNode<PHObject> *clusterNode = new PHIODataNode<PHObject>( mod_hcalout_clusters, "CLUSTER_HCALOUT_MOD", "PHObject");
      // --- node structure for clusters is HCALOUT -> CLUSTER_HCALOUT
      hcaloutNode->addNode(clusterNode);
    }
  else
    {
      // --- If the new node already exists (bad) issue a warning and then do nothing
      cout << "TreeMaker::CreateNode : CLUSTER_HCALOUT_MOD already exists! " << endl;
    }

  // ------------
  // --- All done
  // ------------

  return 0;

}
