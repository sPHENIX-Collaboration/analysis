#include "DISKinematics.h"

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

DISKinematics::DISKinematics(std::string filename) :
  SubsysReco("DISKinematics" ),
  _embedding_id(1)
{
  _foutname = filename;
  _ebeam_E = 10;
  _pbeam_E = 250;
}

int
DISKinematics::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  _fout_root = new TFile(_foutname.c_str(), "RECREATE");
  _tree_dis = new TNtuple("tree_dis","DIS kinematics info","event:true_process:true_x1:true_x2:true_Q2:event_s:electron_x:electron_Q2:electron_y:e1_px:e1_py:e1_pz:e1_p:e1_E:e1_eta:e1_theta:e1_phi:e0_E:p0_E");

  return 0;
}

int
DISKinematics::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  //  cout << "Processing event #" << _ievent << endl;

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

  int true_process_id = theEvent->signal_process_id();
  float ev_x1 = -999;
  float ev_x2 = -999;
  float ev_Q2 = -999;

  if ( theEvent->pdf_info() )
    {
      ev_x1 = theEvent->pdf_info()->x1();
      ev_x2 = theEvent->pdf_info()->x2();
      ev_Q2 = theEvent->pdf_info()->scalePDF();
    }

  /* incoming electron data */
  //  float e0_E = _e_ebeam;

  //  TLorentzVector v4_e0(0, 0, -e0_E, e0_E);

  /* incoming proton data */
  //  float p0_E = _e_pbeam;

  //  TLorentzVector v4_p0(0, 0, p0_E, p0_E);

  /* Loop over all particles, look for beam particles, as well as final state electron */
  HepMC::GenParticle *particle_e0 = NULL;
  HepMC::GenParticle *particle_e1 = NULL;
  HepMC::GenParticle *particle_p0 = NULL;
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
    {

      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );

      /* beam electron found */
      if ( particle_e0 == NULL && TString(pdg_p->GetName()) == "e-" && (*p)->status() == 3 && (*p)->production_vertex() == NULL )
	particle_e0 = (*p);

      /* beam proton found */
      if (  particle_p0 == NULL && TString(pdg_p->GetName()) == "proton" && (*p)->status() == 3 && (*p)->production_vertex() == NULL )
	particle_p0 = (*p);

      /* final state electron found */
      //if ( particle_e1 == NULL && TString(pdg_p->GetName()) == "e-" && (*p)->status() == 1 )
      //particle_e1 = (*p);
      if ( (*p)->production_vertex() != NULL )
	{
	  if ( particle_e1 == NULL && TString(pdg_p->GetName()) == "e-" && (*p)->status() == 3 &&
	       (*p)->production_vertex()->particles_out_size() == 2 && (*p)->production_vertex()->particles_in_size() == 2 )
	    {
	      particle_e1 = (*p);
	    }
	}

      /* break loop if all found */
      if ( particle_e0 && particle_e1 && particle_p0 )
	break;

    } /* end loop over all particles in event record */

  /* if scattered electron found */
  if ( particle_e1 )
    {

      float e0_E = 0;
      float p0_E = 0;

      /* if beam particles found */
      if ( particle_e0 && particle_p0 )
	{
	  e0_E = particle_e0->momentum().e();
	  p0_E = particle_p0->momentum().e();
	}

      /* scattered electron data */
      float e1_px = particle_e1->momentum().px();
      float e1_py = particle_e1->momentum().py();
      float e1_pz = particle_e1->momentum().pz();
      float e1_p = sqrt(e1_px*e1_px + e1_py*e1_py + e1_pz*e1_pz);
      float e1_E = particle_e1->momentum().e();
      float e1_eta = particle_e1->momentum().eta();
      float e1_theta = particle_e1->momentum().theta();
      float e1_phi = particle_e1->momentum().phi();

      TLorentzVector v4_e1( e1_px, e1_py, e1_pz, e1_E );

      /* for purpose of calculations, 'theta' angle of the scattered electron is defined as angle
	 between 'scattered electron' and 'direction of incoming electron'. Since initial electron
	 has 'theta = Pi' in coordinate system, we need to use 'theta_rel = Pi - theta' for calculations
	 of kinematics. */
      float e1_theta_rel = M_PI - e1_theta;

      /* event kinematics */
      float dis_s = 4 * e0_E * p0_E;

      //      float dis_y = 1 - ( e1_E / (2*e0_E) ) * ( 1 - cos( e1_theta_rel ) );
      float dis_y = 1 - (e1_E/e0_E) * pow( cos( e1_theta_rel / 2. ), 2 );

      float dis_Q2 = 2 * e0_E * e1_E * ( 1 - cos( e1_theta_rel ) );

      //      float dis_x = e1_E * ( 1 + cos( e1_theta_rel ) ) / ( 2 * dis_y * p0_E );
      float dis_x = dis_Q2 / ( dis_s * dis_y );


      /* fill output TNtuple */
      float dis_data[70] = {(float)_ievent,(float)true_process_id,ev_x1,ev_x2,ev_Q2,dis_s,dis_x,dis_Q2,dis_y,e1_px,e1_py,e1_pz,e1_p,e1_E,e1_eta,e1_theta,e1_phi,e0_E,p0_E};

      _tree_dis -> Fill(dis_data);

    }
  /* if no scattered electron found */
  else {
    cout << "WARNING: Did not find scattered electron in event " << _ievent << endl;
  }

  return 0;
}

int
DISKinematics::End(PHCompositeNode *topNode)
{
  _fout_root->cd();
  _tree_dis->Write();
  _fout_root->Close();

  return 0;
}
