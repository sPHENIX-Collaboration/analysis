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
  _tree_dis = new TNtuple("tree_dis","DIS kinematics info","event:ev_x1:ev_x2:ev_Q2:x:Q2:y:s:e1_px:e1_py:e1_pz:e1_p:e1_e:e1_eta:e1_theta:e1_phi");

  return 0;
}

int
DISKinematics::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  //cout << "Processing event #" << _ievent << endl;

  PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topNode,
                                                                "PHHepMCGenEvent");
  HepMC::GenEvent* theEvent = genevt->getEvent();

  float ev_x1 = theEvent->pdf_info()->x1();
  float ev_x2 = theEvent->pdf_info()->x2();
  float ev_Q2 = theEvent->pdf_info()->scalePDF();

  /* incoming electron data */
  float e0_e = _e_ebeam;

  TLorentzVector v4_e0(0, 0, -e0_e, e0_e);

  /* incoming proton data */
  float p0_e = _e_pbeam;

  TLorentzVector v4_p0(0, 0, p0_e, p0_e);

  /* Loop over all particles, look for first stable electron in event record*/
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
    {

      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );

      /* electron found */
      if ( TString(pdg_p->GetName()) == "e-" && (*p)->status() == 1 )
        {
          /* scattered electron data */
          float e1_px = (*p)->momentum().px();
          float e1_py = (*p)->momentum().py();
          float e1_pz = (*p)->momentum().pz();
          float e1_p = sqrt(e1_px*e1_px + e1_py*e1_py + e1_pz*e1_pz);
          float e1_e = (*p)->momentum().e();
          float e1_eta = (*p)->momentum().eta();
          float e1_theta = (*p)->momentum().theta();
          float e1_phi = (*p)->momentum().phi();

          TLorentzVector v4_e1( e1_px, e1_py, e1_pz, e1_e );

          /* for purpose of calculations, 'theta' angle of the scattered electron is defined as angle
             between 'scattered electron' and 'direction of incoming electron'. Since initial electron
             has 'theta = Pi' in coordinate system, we need to use 'theta_rel = Pi - theta' for calculations
             of kinematics. */
          //float e1_theta_rel = M_PI - e1_theta;

          /* event kinematics */
          float dis_x = ev_x2;
          float dis_Q2 = ev_Q2;

          float dis_y = dis_Q2 / (4.*dis_x*e0_e*p0_e);
          float dis_s = 4 * e0_e * p0_e;

          /* fill output TNtuple */
          float dis_data[70] = {(float)_ievent,ev_x1,ev_x2,ev_Q2,dis_x,dis_Q2,dis_y,dis_s,e1_px,e1_py,e1_pz,e1_p,e1_e,e1_eta,e1_theta,e1_phi};
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
