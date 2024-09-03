#pragma once

#include "cluster.hh"
#include "clustEvent.hh"
#include "analysis_tracking.hh"

//extern int n_dotracking;

bool dotracking(clustEvent &vCluster, TH2F* h_dphi_nocut )
{
  for (unsigned int i = 0; i < vCluster.vclus.size(); i++)
    {
      cluster c1 = vCluster.vclus[i];
      if (2 <= c1.layer) // inner
	continue;

      for (unsigned int j = i + 1; j < vCluster.vclus.size(); j++)
        {
	  cluster c2 = vCluster.vclus[j];
	  if (c2.layer <= 1) // outer
	    continue;

	  TVector3 beamspot = {vCluster.dca_mean[0], vCluster.dca_mean[1], 0};
	  // TVector3 beamspot = {0, 0, 0};

	  TVector3 p1 = c1.pos - beamspot;
	  TVector3 p2 = c2.pos - beamspot;

	  double p1_phi = atan2(p1.y(), p1.x());
	  double p2_phi = atan2(p2.y(), p2.x());
	  double d_phi = p2_phi - p1_phi;
	  d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

	  double p1_r = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
	  double p2_r = sqrt(p2.x() * p2.x() + p2.y() * p2.y());
	  double p1_theta = atan2(p1_r, p1.z());
	  double p2_theta = atan2(p2_r, p2.z());
	  double d_theta = p2_theta - p1_theta;
	  if (n_dotracking == 1)
	    h_dphi_nocut->Fill(p1_phi, d_phi);

	  if (fabs(d_phi) > 0.04)
	    continue;

	  TVector3 u = p2 - p1;
	  double unorm = sqrt(u.x() * u.x() + u.y() * u.y());
	  // unit vector in 2D
	  double ux = u.x() / unorm;
	  double uy = u.y() / unorm;
	  double uz = u.z() / unorm; // same normalization factor(2D) is multiplied

	  TVector3 p0 = beamspot - p1;

	  double dca_p0 = p0.x() * uy - p0.y() * ux; // cross product of p0 x u
	  double len_p0 = p0.x() * ux + p0.y() * uy; // dot product of p0 . u

	  // beam center in X-Y plane
	  double vx = len_p0 * ux + p1.x();
	  double vy = len_p0 * uy + p1.y();
	  double vz = len_p0 * uz + p1.z();

	  track *tr = new track();
	  tr->phi_in = p1_phi;
	  tr->phi_out = p2_phi;
	  tr->theta_in = c1.gettheta_clus();
	  tr->theta_out = c2.gettheta_clus();
	  tr->d_phi = d_phi;
	  tr->d_theta = d_theta;
	  tr->charge = dca_p0 / fabs(dca_p0);

	  tr->dca[0] = vx;
	  tr->dca[1] = vy;
	  tr->dca[2] = vz;

	  tr->p1 = c1.pos;
	  tr->p2 = c2.pos;

	  tr->zline.at(0) = c1.zline;
	  tr->zline.at(1) = c2.zline;

	  tr->dca_2d = dca_p0;

	  tr->AddAssociatedClusterId( i );
	  tr->AddAssociatedClusterId( j );

	  vCluster.vtrack.push_back(tr);
        } // end of for (unsigned int j = i + 1; j < vCluster.vclus.size(); j++)
    } // end of   for (unsigned int i = 0; i < vCluster.vclus.size(); i++)

  return true;
}
