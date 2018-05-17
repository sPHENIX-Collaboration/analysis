// Some documentation, possibly //

#include "TMath.h"

#include "SetupDualRICHAnalyzer.h"
#include "dualrich_analyzer.h"

using namespace std;


SetupDualRICHAnalyzer::SetupDualRICHAnalyzer()
{
  _analyzer = new eic_dual_rich();
}

double
SetupDualRICHAnalyzer::calculate_emission_angle( double m_emi[3], double momv[3], PHG4Hit * hit_i )
{
  /* Input parameters for indirect ray tracing algorithm */
  double Ex = m_emi[0];
  double Ey = m_emi[1];
  double Ez = m_emi[2];

  double Dx = hit_i->get_x(0);
  double Dy = hit_i->get_y(0);
  double Dz = hit_i->get_z(0);

  double vx = momv[0];
  double vy = momv[1];
  double vz = momv[2];

  int sec = hit_i->get_detid();
  double cx = -18.5*TMath::Sin(sec*TMath::Pi()/4); // mirror center of each octant
  double cy = 18.5*TMath::Cos(sec*TMath::Pi()/4);
  double cz = 75;

  int select_radiator=0;

  /* Set mirror parameters */
  double R_mirror = 195; // cm
  _analyzer->set_mirror(cx, cy, cz, R_mirror);

  /* Call algorithm to determine emission angle of photon i w.r.t. track j */
  float theta_c = _analyzer->ind_ray(Ex, Ey, Ez, Dx, Dy, Dz, vx, vy, vz, select_radiator); //Indirect Ray Tracing

  return theta_c;
}

bool
SetupDualRICHAnalyzer::get_true_momentum( PHG4TruthInfoContainer* truthinfo, SvtxTrack * track, double arr_mom[3] )
{
  arr_mom[0] = 0;
  arr_mom[1] = 0;
  arr_mom[2] = 0;

  /* Get the track's particle, then get truth momentum */
  PHG4Particle* particle = truthinfo->GetParticle( track->get_truth_track_id() );
  if (particle){
    arr_mom[0] = particle->get_px();
    arr_mom[1] = particle->get_py();
    arr_mom[2] = particle->get_pz();
    return true;
  }

  return false;
}

bool
SetupDualRICHAnalyzer::get_emission_momentum( PHG4TruthInfoContainer* truthinfo, PHG4HitContainer* richhits, SvtxTrack * track, double arr_mom[3] )
{
  arr_mom[0] = 0;
  arr_mom[1] = 0;
  arr_mom[2] = 0;

  /* Get photon emission points, then get truth momentum using the vector from first to last emitted photon */
  vector<double> emix;
  vector<double> emiy;
  vector<double> emiz;

  /* Loop over all G4Hits in container (i.e. RICH photons in event) */
  PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
  PHG4HitContainer::ConstIterator rich_hits_iter;

  for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
    {
      PHG4Hit *hit_i = rich_hits_iter->second;
      PHG4Particle *particle = truthinfo->GetParticle( hit_i->get_trkid() );
      PHG4VtxPoint *vertex = truthinfo->GetVtx( particle->get_vtx_id() );

      /* Should eventually compare that vertices are along the correct tracks, for now we run single tracks */
      emix.push_back( vertex->get_x() );
      emiy.push_back( vertex->get_y() );
      emiz.push_back( vertex->get_z() );
    }


  vector<double>::iterator first;
  vector<double>::iterator last;
  double dx=0;
  double dy=0;
  double dz=0;

  /* Use first-to-last (or first-to-11th) */
  first = std::min_element(emiz.begin(),emiz.end());
  //last = std::max_element(emiz.begin(),emiz.end());
  double p1 = std::distance(emiz.begin(),first);
  //double p2 = std::distance(emiz.begin(),last);
  if (emiz.size() > p1+11){
    dx = emix.at(p1+11) - emix.at(p1);
    dy = emiy.at(p1+11) - emiy.at(p1);
    dz = emiz.at(p1+11) - emiz.at(p1);
  }

  /* Fill "momentum" */
  arr_mom[0] = dx;
  arr_mom[1] = dy;
  arr_mom[2] = dz;

  return true;
}
