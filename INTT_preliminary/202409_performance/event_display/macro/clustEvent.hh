#pragma once

#include "cluster.hh"

class clustEvent
{
public:
  int run_no = 0;
  int ievt = 0;
  uint64_t bco_intt = 0;
  bool mag_on = false;

  double dcaz_max = 9999;
  double dcaz_min = -9999;
  double dca2d_max = 9999;
  double dca2d_min = -9999;

  double dca_mean[3] = {0, 0, 0};

  vector < cluster > vclus;
  vector < truth* > vtruth;
  vector < track* > vtrack;
  vector < int > colorset;

public:
  clustEvent();
  
  int GetClusterNum()
  {
    return vclus.size();
  }

  int GetAssociatedClusterNum()
  {
    int counter = 0;
    for( auto& cluster : vclus )
      if( cluster.is_associated == true )
	counter++;

    return counter;
  }

  double GetAssociatedClusterRatio()
  {
    return double(GetAssociatedClusterNum()) / GetClusterNum();
  }
  
  int GetTrackNum()
  {
    return vtrack.size();
  }

  int GetGoodTrackNum( bool dca_range_cut = false,
		       bool is_deleted = false,
		       bool reverse = false );

  double GetTrackLeftRightAsymmetry();
  double GetTrackUpDownAsymmetry();
  
  int GetTruthNum()
  {
    return vtruth.size();
  }
  
  void clear();
  void makeonetrack();

  void dca_check(bool Debug);
  void truth_check();
  void cluster_check();
  void track_check();
  void charge_check();
  
  void draw_intt(int mode);
  void draw_frame(int mode = 0, bool is_preliminary=false );
  void draw_signature( bool is_preliminary = false );
  void draw_date();
  
  string GetDate();
  
  void draw_tracklets(int mode = 0,
		      bool does_overlay = false,
		      int color = kBlack,
		      bool dca_range_cut = false,
		      bool is_deleted = false,
		      bool reverse = false );

  void draw_trackline(int mode = 0,
		      bool does_overlay = false,
		      //int color = kBlack,
		      bool dca_range_cut = false,
		      bool is_deleted = false,
		      bool reverse = false );

  double rad_deg(double Rad);
  void draw_curve2(int mode,
		   double px,
		   double py,
		   double pz,
		   double rad,
		   double cx,
		   double cy,
		   int color,
		   const vector<double> &V );
  
  void draw_trackcurve( int mode		= 0,
			bool does_overlay	= false,
			bool dca_range_cut	= false,
			bool is_deleted		= false,
			bool reverse		= false );
  
  void draw_truthcurve(int mode			= 0,
		       bool does_overlay	= false,
		       int color		= kBlack,
		       bool only_far_cluster	= false,
		       bool only_far_track	= false);
  void draw_truthline(int mode			= 0,
		      bool does_overlay		= false,
		      int color			= kBlack,
		      bool only_far_cluster	= false,
		      bool only_far_track	= false);
  void draw_clusters(int mode			= 0,
		     bool does_overlay		= false,
		     int color			= kBlack);

  vector<double> get_crosspoint(const vector<double> &V,
				double cx,
				double cy,
				double rad,
				double p1_x,
				double p1_y); // ax + by + c = 0;
  
  vector<double> get_closestpoint(const vector<vector<double>> VV, double p1_x, double p1_y);
  vector<double> get_crossframe(double cx, double cy, double rad, double p1_x, double p1_y);
  
  void SetTrackInfoToCluster( ); // int i, bool flag, double theta, double phi )
  void Print();
};

#ifndef clustEvent_cc
#include "clustEvent.cc"
#endif
