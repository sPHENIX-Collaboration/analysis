#include <TreeMaker.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <TLorentzVector.h>
#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>
#include <jetbackground/TowerBackground.h>



int TreeMaker::GetTowerBackgrounds(PHCompositeNode *topNode)
{

  // -----------------------------------------------------------------------------------------------------
  // --- tower background part
  // -----------------------------------------------------------------------------------------------------



  TowerBackground* towerbackground1 = findNode::getClass<TowerBackground>(topNode,"TowerBackground_Sub1");
  //cout << "TowerBackground_Sub1" << endl;
  bbkg_n = 0;
  for (int layer = 0; layer < 3; layer++)
  {
    for (unsigned int n = 0; n < towerbackground1->get_UE( layer ).size(); n++)
    {
      bbkg_layer[ bbkg_n ] = layer;
      bbkg_eta[ bbkg_n ] = n;
      bbkg_e[ bbkg_n ] =  towerbackground1->get_UE( layer ).at( n );
      bbkg_n++;
    } // loop over size of tower background
  } // loop over calorimeter layers



  TowerBackground* towerbackground2 = findNode::getClass<TowerBackground>(topNode,"TowerBackground_Sub2");
  //cout << "TowerBackground_Sub2" << endl;
  bbkg2_n = 0;
  for (int layer = 0; layer < 3; layer++)
  {
    for (unsigned int n = 0; n < towerbackground2->get_UE( layer ).size(); n++)
    {
      bbkg2_layer[ bbkg2_n ] = layer;
      bbkg2_eta[ bbkg2_n ] = n;
      bbkg2_e[ bbkg2_n ] =  towerbackground2->get_UE( layer ).at( n );
      bbkg2_n++;
    } // loop over size of tower background
  } // loop over calorimeter layers

  return 0;

}
