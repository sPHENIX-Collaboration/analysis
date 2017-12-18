#include <iostream>

// --- header for this clase
#include <TreeMaker.h>

// --- basic sPhenix environment stuff
#include <fun4all/Fun4AllServer.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// --- jet specific stuff
#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>
#include <g4jets/JetV1.h>
#include <g4jets/JetMapV1.h>

using std::cout;
using std::endl;



int TreeMaker::CreateNode(PHCompositeNode *topNode)
{

  cout << "TreeMaker::CreateNode called" << endl;

  PHNodeIterator iter(topNode);

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
  JetMap* test_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_Mod_r02");
  if ( !test_jets )
    {
      // --- If the new node doesn't already exist (good) then make it
      if ( verbosity > 0 ) cout << "TreeMaker::CreateNode : creating AntiKt_Tower_Mod_r02 node " << endl;
      JetMap *sub_jets = new JetMapV1();
      PHIODataNode<PHObject> *subjetNode = new PHIODataNode<PHObject>( sub_jets, "AntiKt_Tower_Mod_r02", "PHObject");
      // --- node structure for jets is DST -> ANTIKT -> TOWER (or CLUSTER or TRACK or whatever) -> specific jet node
      towerNode->addNode(subjetNode);
    }
  else
    {
      // --- If the new node already exists (bad) issue a warning and then do nothing
      cout << "TreeMaker::CreateNode : AntiKt_Tower_Mod_r02 already exists! " << endl;
    }

  return 0;

}
