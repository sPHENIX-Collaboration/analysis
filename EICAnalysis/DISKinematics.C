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
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

using namespace std;

DISKinematics::DISKinematics(std::string filename) :
  SubsysReco("DISKinematics" )
{
  _foutname = filename;
  _e_ebeam = 10;
  _e_pbeam = 250;
}

int
DISKinematics::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  _fout_root = new TFile(_foutname.c_str(), "RECREATE");
  _tree_dis = new TNtuple("tree_dis","DIS kinematics info","event:x:Q2:e_px:e_py:e_pz:e_p:e_e:e_eta:e_phi");

  return 0;
}

int
DISKinematics::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  //  cout << "Processing event #" << _ievent << endl;

  PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topNode,
                                                                "PHHepMCGenEvent");
  HepMC::GenEvent* theEvent = genevt->getEvent();

  /* Loop over all particles, look for first stable electron in event record*/
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
    {

      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );

      /* electron found */
      if ( TString(pdg_p->GetName()) == "e-" && (*p)->status() == 1 )
	{

	  /* electron data */
	  float e_px = (*p)->momentum().px();
	  float e_py = (*p)->momentum().py();
	  float e_pz = (*p)->momentum().pz();
	  float e_p = sqrt(e_px*e_px + e_py*e_py + e_pz*e_pz);
	  float e_e = (*p)->momentum().e();
	  float e_eta = (*p)->momentum().eta();
	  float e_theta = (*p)->momentum().theta();
	  float e_phi = (*p)->momentum().phi();

	  /* event kinematics */
	  TLorentzVector v4_electron(0,0,-_e_ebeam,_e_ebeam);
	  TLorentzVector v4_proton(0,0,_e_pbeam,_e_pbeam);
	  TLorentzVector v4_electron_prime(e_px,e_py,e_pz,e_e);
	  //d1d2angle = v_d1.Angle(v_d2.Vect()); // get angle between v_d1 and v_d2

	  float dis_Q2 = 2*_e_ebeam*e_e*(1+cos(e_theta));

	  float dis_y = 1 - ( e_e / _e_ebeam ) * cos(e_theta/2.) * cos(e_theta/2.);
	  float dis_s = 4 * _e_ebeam * _e_pbeam;

	  //	  TLorentzVector v4_q = (v4_electron - v4_electron_prime);
	  //	  float dis_x = dis_Q2 / ( v4_q * v4_proton );
	  float dis_x = dis_Q2 / ( dis_s * dis_y );


	  /* fill output TNtuple */
	  float dis_data[70] = {(float)_ievent,dis_x,dis_Q2,e_px,e_py,e_pz,e_p,e_e,e_eta,e_phi};
	  _tree_dis -> Fill(dis_data);

	  /* break loop */
	  break;
	}
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
