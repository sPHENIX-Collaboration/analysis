#include "DirectPhotonPythia.h"
 
#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>

#include <TLorentzVector.h>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TH2D.h>
#include <TDatabasePDG.h>

#include <cmath>
#include <iostream>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>
using namespace std;

DirectPhotonPythia::DirectPhotonPythia(std::string filename) :
    SubsysReco("DirectPhoton" ),
    _embedding_id(1)
{
  _foutname = filename;

  _pt_min = 25;
  _pt_max = 100;

  _eta_min = -.6;
  _eta_max = +.6;

  _rejection_action = Fun4AllReturnCodes::DISCARDEVENT;
}

int
DirectPhotonPythia::Init(PHCompositeNode *topNode)
{

  _verbose = true;

  _ievent = 0;

  _total_pass = 0;

  _f = new TFile(_foutname.c_str(), "RECREATE");

  _h1 = new TH1D("h1", "", 100, 0, 100.0);
  _h2 = new TH2D("h2", "", 100, 0, 100.0, 40, -2, +2);
  _h2_b = new TH2D("h2_b", "", 100, 0, 100.0, 40, -2, +2);
  _h2_c = new TH2D("h2_c", "", 100, 0, 100.0, 40, -2, +2);
  _h2all = new TH2D("h2all", "", 100, 0, 100.0, 40, -2, +2);

 _ntp_gamma = new TNtuple("ntp_gamma","truth shower => best cluster","ev:px:py:pz:pt:eta:phi");

  return 0;

}

int
DirectPhotonPythia::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

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
      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle(
          (*p)->pdg_id());
      if (pdg_p)
        {
          if (TString(pdg_p->GetName()) == "gamma" && !(*p)->end_vertex() && (*p)->status() == 1 )
            {
	      if((*p)->momentum().perp()>5.) _h2->Fill((*p)->momentum().perp(), (*p)->momentum().pseudoRapidity());
              if((*p)->momentum().perp()>5.) _h1->Fill((*p)->momentum().perp());
              
              if((*p)->momentum().perp()>5.) {
		const float px = (*p)->momentum().x(), 
                            py = (*p)->momentum().y(), 
                            pz = (*p)->momentum().z(),
		            pt = (*p)->momentum().perp(), 
                           eta = (*p)->momentum().pseudoRapidity(),
                           phi = (*p)->momentum().phi();
                float shower_data[70] = {(float)_ievent,px,py,pz,pt,eta,phi};
               
                _ntp_gamma -> Fill(shower_data);

                 return 0;
             }
            }
        }
      }

      return Fun4AllReturnCodes::ABORTEVENT;
}

int
DirectPhotonPythia::End(PHCompositeNode *topNode)
{

  if (verbosity >= DirectPhotonPythia::VERBOSITY_SOME)
    std::cout << __PRETTY_FUNCTION__ << " DVP PASSED " << _total_pass
        << " events" << std::endl;

  _f->cd();
  _f->Write();
  //_f->Close();

  return 0;
}


