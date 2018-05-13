#include "PHPy6GenTrigger.h"
#include "PHPy6ParticleTrigger.h"
#include <phool/PHCompositeNode.h>
#include <phool/phool.h>
#include <phool/getClass.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>

#include <cstdlib>
#include <iostream>
using namespace std;

//___________________________________________________________________________
PHPy6ParticleTrigger::PHPy6ParticleTrigger(const std::string &name):
  PHPy6GenTrigger(name),

  _theParticleType(0),
  _Q2Min(0.0),

  _doTheParticleType(false),
  _doQ2Min(false)
{}


bool PHPy6ParticleTrigger::Apply( const HepMC::GenEvent* evt )
{

  if(_doQ2Min && evt->pdf_info()->scalePDF() < _Q2Min) return false;
  //  cout<<"EVENT"<<endl;
  /*  
  for ( HepMC::GenEvent::particle_const_iterator p
          = evt->particles_begin(); p != evt->particles_end(); ++p ){
    if((*p)->status()==1) cout<<(*p)->pdg_id()<<" "<<(*p)->momentum().eta()<<" "<<sqrt(pow((*p)->momentum().px(),2) + pow((*p)->momentum().py(),2))<<endl;
										   
  }
  */
  if(!_doTheParticleType) return true;

  // Loop over all particles in the event
  for ( HepMC::GenEvent::particle_const_iterator p
          = evt->particles_begin(); p != evt->particles_end(); ++p ){
  
    int pi_num = 0;  

    if((*p)->pdg_id() != _theParticleType) continue;
    
    //if((*p)->pdg_id() == _theParticleType) return true;

    
    for ( HepMC::GenVertex::particle_iterator decay
	    = (*p)->end_vertex()->
	    particles_begin(HepMC::children);
	  decay != (*p)->end_vertex()->
	    particles_end(HepMC::children);
	  ++decay )
      {
	// check if particle decays further 
	if(!(*decay)->end_vertex()){
	  if(abs((*decay)->pdg_id()) == 211) pi_num++;
	}
	
	// loop over decay if particle decays further 
	else if((*decay)->end_vertex()){
	  
	  //further decay loop /
	  for ( HepMC::GenVertex::particle_iterator second_decay
		  = (*decay)->end_vertex()->
		  particles_begin(HepMC::children);
		second_decay != (*decay)->end_vertex()->
		  particles_end(HepMC::children);
		++second_decay )
	    {
	      if (abs((*second_decay)->pdg_id()) == 211) pi_num++;
	      
	      if ((*second_decay)->status() != 1){
		for ( HepMC::GenVertex::particle_iterator third_decay
			= (*second_decay)->end_vertex()->
			particles_begin(HepMC::children);
		      third_decay != (*second_decay)->end_vertex()->
			particles_end(HepMC::children);
		      ++third_decay )
		  {
		    if(abs((*third_decay)->pdg_id()) == 211) pi_num++;
		  }
	      }
	    }
	}
      }
    
    if (pi_num == 3) return true;
    
  }//pythia event for loop

return false;

}

void PHPy6ParticleTrigger::SetParticleType(int pid) {
  _theParticleType = pid;
  _doTheParticleType = true;
}

void PHPy6ParticleTrigger::SetQ2Min(double Q2) {
  _Q2Min = Q2;
  _doQ2Min = true;
}
