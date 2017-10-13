#include "Leptoquarks.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TString.h>
#include <TH2D.h>
#include <TDatabasePDG.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

using namespace std;

Leptoquarks::Leptoquarks(std::string filename) :
  SubsysReco("Leptoquarks" ),
  _embedding_id(1)
{
  _foutname = filename;
  _ebeam_E = 10;
  _pbeam_E = 250;
}

int
Leptoquarks::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  return 0;
}

int
Leptoquarks::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  cout << endl;
  cout << "Processing event " << _ievent << endl;

  PHHepMCGenEventMap * geneventmap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if (!geneventmap)
  {
    std::cout <<PHWHERE<<" - Fatal error - missing node PHHepMCGenEventMap"<<std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  PHHepMCGenEvent *genevt = geneventmap->get(_embedding_id);
  if (!genevt)
  {
    std::cout <<PHWHERE<<" - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID "<<_embedding_id;
    std::cout <<". Print PHHepMCGenEventMap:";
    geneventmap->identify();
    return Fun4AllReturnCodes::ABORTRUN;
  }

  HepMC::GenEvent* theEvent = genevt->getEvent();

  if ( !theEvent )
    {
      cout << "Missing GenEvent!" << endl;
    }
  else
    {
      cout << "Found a GenEvent with " << theEvent->particles_size() << " particles and " << theEvent->vertices_size() << " vetices" << endl;
    }

  /* Loop over all particles, look for pions */
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
    {

      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );

      /* check for tau */
      if ( (TString(pdg_p->GetName()) == "tau-") ||
	   (TString(pdg_p->GetName()) == "LQ_ue") ||
	   (TString(pdg_p->GetName()) == "proton") ||
	   (TString(pdg_p->GetName()) == "e-") ||
	   (TString(pdg_p->GetName()) == "pi0")
	   )
//      if ( (TString(pdg_p->GetName()) == "tau")
//	   )
        {
          cout << "Found a " << TString(pdg_p->GetName()) << " with status " << (*p)->status() << " at eta = " << (*p)->momentum().eta() << " and phi = " << (*p)->momentum().phi() << endl;

          if ( (*p)->end_vertex() )
            {
              cout << "  ** end vertex at x = " << (*p)->end_vertex()->position().x() << endl;

              cout << "  **** All INCOMING particles at end vertex: ";
              for (HepMC::GenVertex::particles_in_const_iterator p_in = (*p)->end_vertex()->particles_in_const_begin();
                   p_in != (*p)->end_vertex()->particles_in_const_end(); p_in++ )
                {
                  cout << TDatabasePDG::Instance()->GetParticle( (*p_in)->pdg_id() )->GetName() << " ";
                }
              cout << endl;

              cout << "  **** All OUTGOING particles at end vertex: ";
              for (HepMC::GenVertex::particles_out_const_iterator p_out = (*p)->end_vertex()->particles_out_const_begin();
                   p_out != (*p)->end_vertex()->particles_out_const_end(); p_out++ )
                {
                  cout << TDatabasePDG::Instance()->GetParticle( (*p_out)->pdg_id() )->GetName() << " ";
                }
	      cout << endl;
            }
          else
            cout << "  ** no end vertex for this particle " << endl;
        }




      //      /* Check for pions and ancestors */
      //      if ( (TString(pdg_p->GetName()) == "tau-") ||
      //           (TString(pdg_p->GetName()) == "pi+") )
      //        {
      //          cout << "Found a " << TString(pdg_p->GetName()) << " with status " << (*p)->status() << " at eta = " << (*p)->momentum().eta() << " and phi = " << (*p)->momentum().phi() << endl;
      //


      //          cout << " *** All INCOMING particles at production vertex: " << (*p)->production_vertex()->position[0] << endl;

      //cout << (*p)->barcode() << " ; " << TString(pdg_p->GetName()) << " ; " << (*p)->status() << " ; " << (*p)->production_vertex() << " ; " << (*p)->end_vertex() << " ; " << (*p)->momentum().e() << " ; " << (*p)->momentum().theta() << endl;

    } /* end loop over all particles in event record */

  return 0;
}

int
Leptoquarks::End(PHCompositeNode *topNode)
{
  //_fout_root->cd();
  //_tree_lq->Write();
  //_fout_root->Close();

  return 0;
}
