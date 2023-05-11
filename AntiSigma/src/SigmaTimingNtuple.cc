#include "SigmaTimingNtuple.h"

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHRandomSeed.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include <gsl/gsl_const.h>
#include <gsl/gsl_randist.h>

#include <boost/foreach.hpp>

#include<sstream>

using namespace std;

// speed of light in cm/ns
const double C_light = GSL_CONST_MKS_SPEED_OF_LIGHT / 1e7;
const double MASSNEUTRON = 0.939565330;
const double MASSPION = 0.13957018;

SigmaTimingNtuple::SigmaTimingNtuple(const std::string &name, const std::string &filename):
  SubsysReco( name ),
  nblocks(0),
  hm(NULL),
  _filename(filename),
  outfile(NULL)
{
  RandomGenerator = gsl_rng_alloc(gsl_rng_mt19937);
  unsigned int seed = PHRandomSeed(); // fixed seed is handled in this funtcion
  cout << Name() << " random seed: " << seed << endl;
  gsl_rng_set(RandomGenerator,seed);
  return;
}

SigmaTimingNtuple::~SigmaTimingNtuple()
{
  //  delete ntup;
  delete hm;
} 


int
SigmaTimingNtuple::Init( PHCompositeNode* )
{
  ostringstream hname, htit;
  hm = new  Fun4AllHistoManager(Name());
  outfile = new TFile(_filename.c_str(), "RECREATE");

  ntupprim = new TNtuple("prim", "Primaries", "pid:phi:theta:eta:e:p");
  ntupsec = new TNtuple("sec", "Secondaries", "pid:phi:theta:eta:e:p");
  ntupt = new TNtuple("tnt", "G4Hits", "ID:t:dtotal");
  ntupsigma = new TNtuple("sigma","Sigmas","mass:inv1:inv2:inv3:t:tres");
//  ntupavet = new TNtuple("time", "G4Hits", "ID:avt");
  //  ntup->SetDirectory(0);
  TH1 *h1 = new TH1F("edep1GeV","edep 0-1GeV",1000,0,1);
  eloss.push_back(h1);
  h1 = new TH1F("edep100GeV","edep 0-100GeV",1000,0,100);
  eloss.push_back(h1);
  return 0;
}

int
SigmaTimingNtuple::process_event( PHCompositeNode* topNode )
{
  ostringstream nodename;

  map<int, PHG4Particle*>::const_iterator particle_iter;
  PHG4TruthInfoContainer *_truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (_truth_container->size() > 3)
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  double sigmass = 0;

  PHG4TruthInfoContainer::ConstRange primary_range =
    _truth_container->GetPrimaryParticleRange();
  float ntvars[6] = {0};
  for (PHG4TruthInfoContainer::ConstIterator particle_iter = primary_range.first;
       particle_iter != primary_range.second; ++particle_iter)
  {
    PHG4Particle *particle = particle_iter->second;

    ntvars[0] = particle->get_pid();
    ntvars[1] = atan2(particle->get_py(), particle->get_px());
    ntvars[2] = atan(sqrt(particle->get_py()*particle->get_py() + 
			  particle->get_px()*particle->get_px()) /
		     particle->get_pz());
    if (ntvars[2] < 0)
    {
      ntvars[2]+=M_PI;
    }
    ntvars[3] = 0.5*log((particle->get_e()+particle->get_pz())/
			(particle->get_e()-particle->get_pz()));
    ntvars[4] = particle->get_e();
    ntvars[5] = sqrt(particle->get_py()*particle->get_py() + 
		     particle->get_px()*particle->get_px() +
		     particle->get_pz()*particle->get_pz());
    sigmass = sqrt(ntvars[4]*ntvars[4] - ntvars[5]*ntvars[5]);
    // cout << " primary particle " << particle->get_name() 
    // 	 << ", pid: " << particle->get_pid() 
    // 	 << ", mass: " << sigmass << endl;
    ntupprim->Fill(ntvars);
  }
  bool anti_neutron_ok = false;
  bool piminus_ok = false;
  int anti_neutron_trk_id = 0;
//  double piminus_mom = NAN;
  double anti_neutron_mom = NAN;

  double E_an = NAN;
  double px_an = NAN;
  double py_an = NAN;
  double pz_an = NAN;

  double E_pi = NAN;
  double px_pi = NAN;
  double py_pi = NAN;
  double pz_pi = NAN;

  double p_scale = NAN;
  double e_an_tof = NAN;   
  double e_an_tof_tres = NAN;
  double p_scale_tres = NAN;

  double t_ave = NAN;
  double t_res = NAN;

  PHG4TruthInfoContainer::ConstRange secondary_range =
    _truth_container->GetSecondaryParticleRange();
  for (PHG4TruthInfoContainer::ConstIterator particle_iter = secondary_range.first;
       particle_iter != secondary_range.second; ++particle_iter)
  {
    PHG4Particle *particle = particle_iter->second;
    ntvars[0] = particle->get_pid();
    ntvars[1] = atan2(particle->get_py(), particle->get_px());
    ntvars[2] = atan(sqrt(particle->get_py()*particle->get_py() + 
			  particle->get_px()*particle->get_px()) /
		     particle->get_pz());
    if (ntvars[2] < 0)
    {
      ntvars[2]+=M_PI;
    }
    ntvars[3] = 0.5*log((particle->get_e()+particle->get_pz())/
			(particle->get_e()-particle->get_pz()));
    ntvars[4] = particle->get_e();
    ntvars[5] = sqrt(particle->get_py()*particle->get_py() + 
		     particle->get_px()*particle->get_px() +
		     particle->get_pz()*particle->get_pz());
    ntupsec->Fill(ntvars);
    if (particle->get_pid() == -211)
    {
      piminus_ok = true;
//      piminus_mom = ntvars[5]; 
      E_pi = ntvars[4];
      px_pi = particle->get_px();
      py_pi = particle->get_py();
      pz_pi = particle->get_pz();
    }
    else if (particle->get_pid() == -2112)
    {
      anti_neutron_ok = true;
      anti_neutron_trk_id = particle->get_track_id();
      anti_neutron_mom = ntvars[5];
      E_an = ntvars[4];
      px_an = particle->get_px();
      py_an = particle->get_py();
      pz_an = particle->get_pz();
    }
    // cout << " secondary particle " << particle->get_name() 
    // 	 << ", pid: " << particle->get_pid() 
    // 	 << ", G4 track id: " << particle->get_track_id() << endl;
  }
  if (! (piminus_ok && anti_neutron_ok))
  {
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  set<string>::const_iterator iter;
  vector<TH1 *>::const_iterator eiter;

  PHG4HitContainer *hits = nullptr;

  hits = findNode::getClass<PHG4HitContainer>(topNode,"G4HIT_CALO");

  if (hits)
  {
    //          double numhits = hits->size();
    //          nhits[i]->Fill(numhits);
    PHG4HitContainer::ConstRange hit_range = hits->getHits();
    for ( PHG4HitContainer::ConstIterator hit_iter = hit_range.first ; hit_iter !=  hit_range.second; hit_iter++ )

    {
      if (hit_iter->second->get_trkid() == anti_neutron_trk_id)
      {
//	cout << "G4 trkid: " << hit_iter->second->get_trkid() << endl; 
	t_ave = hit_iter->second->get_avg_t();
	double dtotal = get_dtotal(hit_iter->second, ntvars[0],ntvars[1]);
	ntupt->Fill(hit_iter->second->get_layer(),t_ave,dtotal);
	double dist = sqrt(hit_iter->second->get_avg_x()*hit_iter->second->get_avg_x()+
			   hit_iter->second->get_avg_y()*hit_iter->second->get_avg_y()+
			   hit_iter->second->get_avg_z()*hit_iter->second->get_avg_z());
	double beta = dist/t_ave/C_light;
	double neutMom = MASSNEUTRON * beta / sqrt(1.0 - beta*beta);
	e_an_tof = sqrt(neutMom*neutMom+MASSNEUTRON*MASSNEUTRON);
//	cout << "neut mom: " << neutMom << ", orig: " << anti_neutron_mom << endl;
	p_scale = neutMom/anti_neutron_mom;
	t_res = t_ave+gsl_ran_gaussian(RandomGenerator, 1.);
	double beta_tres = dist/t_res/C_light;
double neutMom_tres = MASSNEUTRON * beta_tres / sqrt(1.0 - beta_tres*beta_tres);
	e_an_tof_tres = sqrt(neutMom_tres*neutMom_tres+MASSNEUTRON*MASSNEUTRON);
	p_scale_tres = neutMom_tres/anti_neutron_mom;

      }
    }
    double im2 = MASSNEUTRON*MASSNEUTRON +MASSPION*MASSPION +2*(E_an * E_pi - (px_an*px_pi+py_an*py_pi+pz_an*pz_pi));
    double im2a = MASSNEUTRON*MASSNEUTRON +MASSPION*MASSPION +2*(e_an_tof * E_pi - ((px_an*p_scale)*px_pi+(py_an*p_scale)*py_pi+(pz_an*p_scale)*pz_pi));
    double im2b = MASSNEUTRON*MASSNEUTRON +MASSPION*MASSPION +2*(e_an_tof_tres * E_pi - ((px_an*p_scale_tres)*px_pi+(py_an*p_scale_tres)*py_pi+(pz_an*p_scale_tres)*pz_pi));
//    cout << "im2: " << im2 << ", inv mass: " << sqrt(im2) << endl;
    ntupsigma->Fill(sigmass,sqrt(im2),sqrt(im2a),sqrt(im2b),t_ave,t_res);
  }

  return 0;
}

int
SigmaTimingNtuple::End(PHCompositeNode * topNode)
{
  outfile->cd();
  //  ntup->Write();
  ntupt->Write();
  ntupprim->Write();
  ntupsec->Write();
  ntupsigma->Write();
  outfile->Write();
  outfile->Close();
  delete outfile;
  hm->dumpHistos(_filename, "UPDATE");
  return 0;
}

void
SigmaTimingNtuple::AddNode(const std::string &name, const int detid)
{
  _node_postfix.insert(name);
  _detid[name] = detid;
  return;
}

double
SigmaTimingNtuple::get_dtotal(const PHG4Hit *hit, const double phi, const double theta)
{
  double phi_hit =  atan2(hit->get_avg_y(),hit->get_avg_x());
  double theta_hit = atan(sqrt(hit->get_avg_x()*hit->get_avg_x()+
			       hit->get_avg_y()*hit->get_avg_y()) /
			  hit->get_avg_z());
  double diffphi =  phi_hit - phi;
  if (diffphi > M_PI)
  {
    diffphi -= 2*M_PI;
  }
  else if (diffphi < - M_PI)
  {
    diffphi += 2*M_PI;
  }
  if (theta_hit < 0)
  {
    theta_hit += M_PI;
  }
  double difftheta = theta_hit-theta;
  double deltasqrt = sqrt(diffphi*diffphi+difftheta*difftheta);
  return deltasqrt;
}
