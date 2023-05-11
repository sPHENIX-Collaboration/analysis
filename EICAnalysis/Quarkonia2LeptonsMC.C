#include "Quarkonia2LeptonsMC.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//#include <g4main/PHG4TruthInfoContainer.h>
//#include <g4main/PHG4Particle.h>

#include <TLorentzVector.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TString.h>
#include <TH2D.h>
#include <TDatabasePDG.h>

//#include <cmath>
//#include <iostream>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

using namespace std;

Quarkonia2LeptonsMC::Quarkonia2LeptonsMC(std::string filename) :
  SubsysReco("Quarkonia2LeptonsMC" ),
  _embedding_id(1)
{
  _foutname = filename;
  _tree_quarkonia = NULL;
}

int
Quarkonia2LeptonsMC::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  _fout_root = new TFile(_foutname.c_str(), "RECREATE");
  _tree_quarkonia = new TNtuple("tree_quarkonia","quarkonia info","ev:px:py:pz:e:pt:eta:phi:invmass:ndaughter:d1d2angle");

  return 0;
}


int
Quarkonia2LeptonsMC::process_event(PHCompositeNode *topNode)
{

  _ievent ++;

  //  cout << "Process event # " << _ievent << endl;

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

  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
    {

      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );

      //      if ( (*p)->pdg_id() == 443 )
      if (TString(pdg_p->GetName()) == "J/psi" && (*p)->end_vertex() && (*p)->status() == 2 )
	{

	  /* quarkonia data */
	  float px = (*p)->momentum().px();
	  float py = (*p)->momentum().py();
	  float pz = (*p)->momentum().pz();
	  float pe = (*p)->momentum().e();
	  float pt = sqrt(pow((*p)->momentum().px(),2) + pow((*p)->momentum().py(),2));
	  float eta = (*p)->momentum().eta();
	  float phi = (*p)->momentum().phi();
	  float invmass = (*p)->momentum().m();

	  /* daughter = decay products */
	  float ndaughter = (*p)->end_vertex()->particles_out_size();

	  /* calculate angle between first two daugher particles */
	  float d1d2angle = 0;
	  if ( ndaughter == 2 )
	    {
	      HepMC::GenVertex::particles_out_const_iterator p_d1 = (*p)->end_vertex()->particles_out_const_begin();
	      HepMC::GenVertex::particles_out_const_iterator p_d2 = p_d1 + 1;

	      TLorentzVector v_d1((*p_d1)->momentum().px(),(*p_d1)->momentum().py(),(*p_d1)->momentum().pz(),(*p_d1)->momentum().e());
	      TLorentzVector v_d2((*p_d2)->momentum().px(),(*p_d2)->momentum().py(),(*p_d2)->momentum().pz(),(*p_d2)->momentum().e());

	      d1d2angle = v_d1.Angle(v_d2.Vect()); // get angle between v_d1 and v_d2
	    }

	  float quarkonia_data[70] = {(float)_ievent,px,py,pz,pe,pt,eta,phi,invmass,ndaughter,d1d2angle};
	  _tree_quarkonia -> Fill(quarkonia_data);

	  //	  cout << "Found a J/Psi in event " << _ievent << " with momentum " << pmom << " and which decays into " << ndecay << " particles:" << endl;

	  //HepMC::IteratorRange irange = HepMC::children;
//	  for ( HepMC::GenVertex::particles_out_const_iterator p2 = (*p)->end_vertex()->particles_out_const_begin();
//		p2 != (*p)->end_vertex()->particles_out_const_end(); ++p2 ) {
//	    cout << "    -> " << (*p2)->pdg_id() << endl;
//	  }

	}


//      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );
//
//      if (pdg_p)
//        {
//	  if (TString(pdg_p->GetName()) == "c")
//	    {
//	      cout << "Found a c-quark!" << endl;
//	    }
//	  if (TString(pdg_p->GetName()) == "c_bar")
//	    {
//	      cout << "Found a c-antiquark!" << endl;
//	    }
//	  if (TString(pdg_p->GetName()) == "J/psi" || TString(pdg_p->GetName()) == "J/psi_di")
//	    {
//	      cout << "Found a J/Psi!" << endl;
//	    }
//	  if (TString(pdg_p->GetName()) == "Upsilon")
//	    {
//	      cout << "Found an Upsilon!" << endl;
//	    }
	  //	  if (TString(pdg_p->GetName()) == "J/psi" && !(*p)->end_vertex() && (*p)->status() == 1 )
          //            {
          //              if((*p)->momentum().perp()>5.) _h2->Fill((*p)->momentum().perp(), (*p)->momentum().pseudoRapidity());
          //              if((*p)->momentum().perp()>5.) _h1->Fill((*p)->momentum().perp());
          //              if((*p)->momentum().perp()>5.) {
          //                const float px = (*p)->momentum().x(),
          //                  py = (*p)->momentum().y(),
          //                  pz = (*p)->momentum().z(),
          //                  pt = (*p)->momentum().perp(),
          //                  eta = (*p)->momentum().pseudoRapidity(),
          //                  phi = (*p)->momentum().phi();
          //                float shower_data[70] = {(float)_ievent,px,py,pz,pt,eta,phi};
          //
          //                _ntp_gamma -> Fill(shower_data);
          //
          //                return 0;
          //              }
          //            }
	  //        }
    }

  return Fun4AllReturnCodes::ABORTEVENT;
}

int
Quarkonia2LeptonsMC::End(PHCompositeNode *topNode)
{
  _fout_root->cd();
  _tree_quarkonia->Write();
  _fout_root->Close();

  return 0;
}


