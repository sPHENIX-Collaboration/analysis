#include <TreeMaker.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// --- calorimeter towers
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>

// --- jet specific stuff
#include <g4jets/Jet.h>
#include <g4jets/JetMap.h>
#include <g4jets/JetV1.h>
#include <g4jets/JetMapV1.h>

using std::cout;
using std::endl;



int TreeMaker::CopyAndMakeJets(PHCompositeNode *topNode)
{

  // -----------------------------------------------------------------------------------------------------
  // ---
  // -----------------------------------------------------------------------------------------------------

  // --- calorimeter tower containers
  RawTowerContainer* towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  RawTowerContainer* towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  RawTowerContainer* towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");

  // --- calorimeter geometry objects
  RawTowerGeomContainer* geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer* geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer* geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  // --- jet objects
  JetMap* old_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r02");
  JetMap* new_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_Mod_r02"); // this node is created in createnode
  if ( verbosity > 0 )
    {
      cout << "Regular jet node: " << old_jets << endl;
      cout << "Modified jet node: " << new_jets << endl;
    }
  if ( !old_jets || ! new_jets )
    {
      cout << "One or more invalid pointers, exiting event" << endl;
      return 0;
    }

  // --- print sizes of old and new objects for diagnostic purposes
  if ( verbosity > 0 )
    {
      cout << "process_event: entering with # original jets = " << old_jets->size() << endl;
      cout << "process_event: entering with # new jets = " << new_jets->size() << endl;
    }

  // --- iterate over jets
  int ijet = 0;
  for ( JetMap::Iter iter = old_jets->begin(); iter != old_jets->end(); ++iter )
    {
      // --- get information from this jet (an element of the jet container)
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      // --- create a new jet object
      Jet *new_jet = new JetV1();
      float new_total_px = 0;
      float new_total_py = 0;
      float new_total_pz = 0;
      float new_total_e = 0;
      if ( verbosity > 3 && this_pt > 5 )
        {
          cout << "process_event: unsubtracted jet with e / pt / eta / phi = "
               << this_e << " / " << this_pt << " / " << this_eta << " / " << this_phi << endl;
        }
      // --- now loop over individual constituents of this jet
      for ( Jet::ConstIter comp = this_jet->begin_comp(); comp != this_jet->end_comp(); ++comp )
        {
          // --- (*.comp).first is the layer:
          // ---  0 = void
          // ---  1 = particle
          // ---  2 = track
          // ---  3 = emcal tower
          // ---  4 = emcal cluster
          // ---  5 = ihcal tower
          // ---  6 = ihcal cluster
          // ---  7 = ohcal tower
          // ---  8 = ohcal cluster
          // ---  9 = femc tower
          // --- 10 = femc cluster
          // --- 11 = fhcal tower
          // --- 12 = fhcal cluster
          // --- 13 = emcal retower (combined towers to match ihcal geometry)
          // --- 14 = subtracted emcal tower
          // --- 15 = subtracted ihcal tower
          // --- 16 = subtracted ohcal tower
          RawTower *tower = 0;
          RawTowerGeom *tower_geom = 0;
          double comp_e = 0;
          double comp_eta = 0;
          double comp_phi = 0;
          int comp_ieta = 0;
          int comp_iphi = 0;
          //double comp_background = 0;
          // --- layer 5 is inner hcal towers
          if ( (*comp).first == 5 )
            {
              tower = towersIH3->getTower( (*comp).second );
              tower_geom = geomIH->get_tower_geometry(tower->get_key());
              comp_ieta = geomIH->get_etabin( tower_geom->get_eta() );
              comp_iphi = geomIH->get_phibin( tower_geom->get_phi() );
            }
          // --- layer 7 is outer hcal towers
          else if ( (*comp).first == 7 )
            {
              tower = towersOH3->getTower( (*comp).second );
              tower_geom = geomOH->get_tower_geometry(tower->get_key());
              comp_ieta = geomOH->get_etabin( tower_geom->get_eta() );
              comp_iphi = geomOH->get_phibin( tower_geom->get_phi() );
            }
          // --- layer 3 is EMCal towers
          // --- layer 13 is re-towered emcal, which uses IHCal geom... not using for now
          else if ( (*comp).first == 3 )
            {
              tower = towersEM3->getTower( (*comp).second );
              tower_geom = geomEM->get_tower_geometry(tower->get_key());
              comp_ieta = geomEM->get_etabin( tower_geom->get_eta() );
              comp_iphi = geomEM->get_phibin( tower_geom->get_phi() );
            }

          // --- if tower and tower_geom exist, get energy from there
          if ( tower && tower_geom )
            {
              comp_e = tower->get_energy();
              comp_eta = tower_geom->get_eta();
              comp_phi = tower_geom->get_phi();
            }

          // --- if very high verbosity, print lots of stuff to screen
          if ( verbosity > 4 && this_jet->get_pt() > 5 )
            {
              cout << "process_event: --> constituent in layer " << (*comp).first
                   << ", has unsub E = " << comp_e << ", is at ieta #" << comp_ieta
                   << " and iphi # = " << comp_iphi << endl;
            }

          // --- update constituent energy based on some algorithm to be developed
          double comp_new_e = comp_e;

          // --- define new kinematics for constituent
          double comp_px = comp_new_e / cosh( comp_eta ) * cos( comp_phi );
          double comp_py = comp_new_e / cosh( comp_eta ) * sin( comp_phi );
          double comp_pz = comp_new_e * tanh( comp_eta );

          // --- add up the total for the new jet based on the modified constituents
          new_total_px += comp_px;
          new_total_py += comp_py;
          new_total_pz += comp_pz;
          new_total_e += comp_new_e;
        } // end of loop over jet constituents

      // --- set the properties for the new jet
      new_jet->set_px( new_total_px );
      new_jet->set_py( new_total_py );
      new_jet->set_pz( new_total_pz );
      new_jet->set_e( new_total_e );
      new_jet->set_id(ijet);

      // --- put the jet into a new container class
      new_jets->insert( new_jet );

      if (verbosity > 1 && this_pt > 5)
        {
          cout << "process_event: old jet #" << ijet << ", old px / py / pz / e = "
               << this_jet->get_px() << " / " << this_jet->get_py() << " / "
               << this_jet->get_pz() << " / " << this_jet->get_e() << endl;
          cout << "process_event: new jet #" << ijet << ", new px / py / pz / e = "
               << new_jet->get_px() << " / " << new_jet->get_py() << " / "
               << new_jet->get_pz() << " / " << new_jet->get_e() << endl;
        }

      ijet++; // increment jet counter

    } // end of loop over jet maps (list of jets)

  if ( verbosity > 0 )
    {
      cout << "process_event: exiting with # original jets = " << old_jets->size() << endl;
      cout << "process_event: exiting with # new jets = " << new_jets->size() << endl;
    }

  return 0;

}
