#pragma once
#include "include_tracking/least_square2.cc"

class track
{
public:
  TF1 *track_xy;
  TF1 *track_rz;
  TF1 *track_zx;
  TF1 *track_zy;
  
  TF1 *track_rz_inv;
  
  double phi_in;
  double phi_out;
  
  double theta_in;
  double theta_out;
  
  double d_phi;
  double d_theta;
  
  double dca[3];
  double dca_mean[3];

  vector < int > cluster_ids;
  TVector3 p1;
  TVector3 p2;
  
  double dca_2d;
  double phi;
  double theta;
  double phi_tracklet;
  double theta_tracklet;
  
  double charge;
  vector < vector < double > > zline;

  vector<double> x;
  vector<double> y;
  vector<double> z;
  vector<double> r;
  vector<double> x_err;
  vector<double> y_err;
  vector<double> z_err;

  double pT;
  double rad;
  double cx, cy;
  double p_reco[3];

  double track_theta; // in z-r plane, degree
  
  double A[3][3];
  double B[1][3];
  double beta[1][3];

  bool dca2d_range_out = false;
  bool dcaz_range_out = false;
  bool dca_range_out = false;
  bool is_deleted = false;

  track() : track_xy(nullptr),
	    track_rz(nullptr),
	    track_zx(nullptr),
	    track_zy(nullptr),
	    track_rz_inv(nullptr),
	    zline(2, vector<double>(0))

  {
    p1 = TVector3();
    p2 = TVector3();
  };

  virtual ~track()
  {
    if (track_xy != nullptr)
      delete track_xy;
    if (track_rz != nullptr)
      delete track_rz;
    if (track_zx != nullptr)
      delete track_zx;
    if (track_zy != nullptr)
      delete track_zy;
    if (track_rz_inv != nullptr)
      delete track_rz_inv;
  };

  void calc_pT()
  {
    TVector3 p3 = {dca_mean[0], dca_mean[1], dca_mean[2]}; // vertex

    TVector3 mid1 = TVector3((p1.x() + p2.x()) / 2, (p1.y() + p2.y()) / 2, (p1.z() + p2.z()) / 2);
    TVector3 mid2 = TVector3((p3.x() + p1.x()) / 2, (p3.y() + p1.y()) / 2, (p3.z() + p1.z()) / 2);

    double alpha = atan2((p2.y() - p1.y()), (p2.x() - p1.x())) + M_PI / 2;
    double beta = atan2((p1.y() - p3.y()), (p1.x() - p3.x())) + M_PI / 2;

    cx = (mid1.y() - mid2.y() - mid1.x() * tan(alpha) + mid2.x() * tan(beta)) / (tan(beta) - tan(alpha));
    cy = mid1.y() - (mid1.x() - cx) * tan(alpha);

    rad = sqrt((p1.x() - cx) * (p1.x() - cx) + (p1.y() - cy) * (p1.y() - cy)); //[cm]
    pT = /*charge **/ 0.3 * 1.4 * rad / 100;                                   //[GeV/c]

    double phi_p = atan2(p3.y() - cy, p3.x() - cx) + (M_PI / 2);
    double phi_p1 = atan2(p1.y() - cy, p1.x() - cx) + (M_PI / 2);
    // cout<<"p1 : "<<p1.x()<<"  "<<p1.y()<<endl;
    // cout<<"phi : "<<phi_p/M_PI * 180<<"  "<<phi_p1/M_PI * 180<<endl;
    double d_phi = phi_p1 - phi_p;
    // cout<<"d_phi : "<<d_phi/M_PI * 180<<endl;
    // d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

    // if(fabs(d_phi) > M_PI)
    // {
    //     cout<<"fabs(d_phi) > M_PI  "<<d_phi<<endl;
    //     cout<<(M_PI * (int)(d_phi / (M_PI)))<<endl;
    // // d_phi -=  (M_PI * (int)(d_phi / (M_PI)));
    // d_phi -=  2 * (M_PI * (int)(d_phi / (M_PI)));

    //     // d_phi -=d_phi * fabs(d_phi) *  M_PI * (int)(d_phi / M_PI);
    // }
    // cout<<" d_phi : "<<d_phi/M_PI * 180<<endl;

    double phi_offset = 0;
    double cross = p3.x() * p1.y() - p3.y() * p1.x();

    if ((d_phi) < 0)
      // if((phi_p1 - phi_p) < 0)
      phi_offset = M_PI;
    // double sign = d_phi / fabs(d_phi);

    // double sign = ((phi_p1 -(M_PI/2))  - (phi_p - (M_PI/2)))/fabs((phi_p1 - (M_PI/2)) - (phi_p - (M_PI/2)));
    // cout<<sign<<endl;
    // double sign_rz = (p1.z() - dca_mean[2]) / fabs(p1.z() - dca_mean[2]);
    // double sign_x = (p1.x() - p3.x()) / fabs(p1.x() - p3.x());
    // double sign_y = (p1.y() - p3.y()) / fabs(p1.y() - p3.y());
    // cout<<"  sign_x,y : "<<sign_x<<"  "<<sign_y<<endl;
    double t_ = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
    // cout<<"no_sign : "<<fabs(pT) * cos(phi_p)<<"  "<<fabs(pT) * sin(phi_p)<<endl;
    // cout<<"phi : "<<phi_p + phi_offset<<endl;

    p_reco[0] = /*sign_x **/ fabs(pT) * cos(phi_p + phi_offset);
    p_reco[1] = /*sign_y **/ fabs(pT) * sin(phi_p + phi_offset);
    // cout<<"p1 : "<<p1.x() - dca_mean[0]<<"  "<<p1.y() - dca_mean[1]<<endl;
    // cout<<"p  : "<<p_reco[0]<<"  "<<p_reco[1]<<endl;
    p_reco[2] = fabs(pT) / t_ * p1.z();
    // cout<<endl;
  }

  void calc_line()
  {
    x.push_back(p1.x());
    x.push_back(p2.x());
    x.push_back(dca_mean[0]);

    x_err.push_back(1.);
    x_err.push_back(1.);
    x_err.push_back(1.);

    y.push_back(p1.y());
    y.push_back(p2.y());
    y.push_back(dca_mean[1]);

    y_err.push_back(1.);
    y_err.push_back(1.);
    y_err.push_back(1.);

    LeastSquare least_square;
    least_square.Setdatax(x);
    least_square.Setdatay(y);
    least_square.Setdatayerr(y_err);

    least_square.SetDebugMode(false);
    least_square.Calc(0);

    if (track_xy != nullptr)
      {
	delete track_xy;
	track_xy = nullptr;
      }

    track_xy = new TF1("track_xy", "pol1", -15, 15);

    track_xy->SetParameter(1, least_square.GetSlope());
    track_xy->SetParameter(0, least_square.GetIntercept());

    z.push_back(p1.z());
    z.push_back(p2.z());
    z.push_back(dca_mean[2]);

    r.push_back(getpointr(1));
    r.push_back(getpointr(2));
    r.push_back(getpointr(3));

    z_err.push_back(20. / sqrt(12.)); // [mm]
    z_err.push_back(20. / sqrt(12.)); // [mm]
    z_err.push_back(0.8);             // [mm]

    ///////////////////////////////////////////
    // r-z plane                             //
    ///////////////////////////////////////////
    LeastSquare least_square_zr;
    least_square_zr.Setdatax(r);
    least_square_zr.Setdatay(z);
    least_square_zr.Setdatayerr(z_err);

    // least_square_zr.SetDebugMode(false);
    least_square_zr.Calc(1);

    if (track_rz != nullptr)
      {
	delete track_rz;
	track_rz = nullptr;
      }
    double slope_inv = least_square_zr.GetSlope();
    double intercept_inv = least_square_zr.GetIntercept();

    track_rz = new TF1("track_rz", "pol1", -25, 25);

    track_rz->SetParameter(1, getslope_inv(slope_inv, intercept_inv));
    track_rz->SetParameter(0, getintercept_inv(slope_inv, intercept_inv));

    track_theta = 360.0 * TMath::ATan( track_rz->GetParameter(1) ) / TMath::Pi() / 2 ; // degree
    
    // cout << "Track z-r: " << track_rz->GetParameter( 1 ) << "\t"
    // 	 << track_theta << endl; // values look OK
    //        track_rz->SetParameter(1, least_square_zr.GetSlope());
    // track_rz->SetParameter(0, least_square_zr.GetIntercept());

    /*
///////////////////////////////////////////
// z-x plane                             //
///////////////////////////////////////////
LeastSquare least_square_zx;
least_square_zx.Setdatax(z);
least_square_zx.Setdatay(x);
least_square_zx.Setdatayerr(x_err);

// least_square_zx.SetDebugMode(false);
least_square_zx.Calc(1);

if (track_zx != nullptr)
{
delete track_zx;
track_zx = nullptr;
}
double slope_inv_zx = least_square_zx.GetSlope();
double intercept_inv_zx = least_square_zx.GetIntercept();

track_zx = new TF1("track_zx", "pol1", -25, 25);

track_zx->SetParameter(1, getslope_inv(slope_inv_zx, intercept_inv_zx));
track_zx->SetParameter(0, getintercept_inv(slope_inv_zx, intercept_inv_zx));

///////////////////////////////////////////
// z-y plane                             //
///////////////////////////////////////////
LeastSquare least_square_zy;
least_square_zy.Setdatax(z);
least_square_zy.Setdatay(y);
least_square_zy.Setdatayerr(y_err);

// least_square_zy.SetDebugMode(false);
least_square_zy.Calc(1);

if (track_zy != nullptr)
{
delete track_zy;
track_zy = nullptr;
}
double slope_inv_zy = least_square_zy.GetSlope();
double intercept_inv_zy = least_square_zy.GetIntercept();

track_zy = new TF1("track_zy", "pol1", -25, 25);

track_zy->SetParameter(1, getslope_inv(slope_inv_zy, intercept_inv_zy));
track_zy->SetParameter(0, getintercept_inv(slope_inv_zy, intercept_inv_zy));

cout << "parameter calc" << endl;
cout << "slope in x-y: "
<< track_xy->GetParameter(1) << "\t"
<< track_zy->GetParameter(1) / track_zx->GetParameter(1) << "\t" 
<< 100 * (track_xy->GetParameter(1) -  track_zy->GetParameter(1) / track_zx->GetParameter(1) ) / track_xy->GetParameter(1) <<  "%"
<< endl;
    */
  }

  void calc_inv()
  {
    if (track_rz_inv != nullptr)
      {
	delete track_rz_inv;
	track_rz_inv = nullptr;
      }

    double slope_ = 1 / track_rz->GetParameter(1);
    double intercept_ = -track_rz->GetParameter(0) / track_rz->GetParameter(1);

    track_rz_inv = new TF1("track_rz_inv", "pol1", -25, 25);

    track_rz_inv->SetParameter(1, slope_);
    track_rz_inv->SetParameter(0, intercept_);
  }

  double getslope_inv(double Slope, double Intercept)
  {
    return 1 / Slope;
  }

  double getintercept_inv(double Slope, double Intercept)
  {
    return -Intercept / Slope;
  }

  double getpointr(int mode)
  {
    double r = 0;

    if (mode == 1)
      {
	r = (p1.y() / fabs(p1.y())) * sqrt(p1.x() * p1.x() + p1.y() * p1.y()); // returns +r or -r, the sign is the sign of y
      }
    else if (mode == 2)
      {
	r = (p2.y() / fabs(p2.y())) * sqrt(p2.x() * p2.x() + p2.y() * p2.y());
      }
    else if (mode == 3)
      {
	r = (dca_mean[1] / fabs(dca_mean[1])) * sqrt(dca_mean[0] * dca_mean[0] + dca_mean[1] * dca_mean[1]);
      }

    return r;
  }

  double getphi()
  {
    phi = atan(track_xy->GetParameter(1));

    if (p1.x() < dca_mean[0])
      {
	if (p1.y() < dca_mean[1])
	  phi -= M_PI;
	else
	  phi += M_PI;
      }

    return phi;
  }

  double gettheta()
  {
    theta = atan(track_rz->GetParameter(1));

    if (p1.z() < dca_mean[2])
      {
	if (getpointr(1) < getpointr(3))
	  theta -= M_PI;
	else
	  theta += M_PI;
      }

    if ( getpointr(1) < getpointr(3) )
      theta = fabs(theta);

    return theta;
  }

  double getphi_tracklet()
  {
    phi_tracklet = atan2((p2.y() - p1.y()), (p2.x() - p1.x()));
    return phi_tracklet;
  }

  double gettheta_tracklet()
  {
    theta_tracklet = atan2((fabs(getpointr(2)) - fabs(getpointr(1))), (p2.z() - p1.z()));

    return theta_tracklet;
  }

  vector<double> get_crossline(double x1, double y1, double r1)
  {
    vector<double> cross_co;
    // ax + by + c = 0
    cross_co.push_back(2 * (cx - x1));                                                           // a
    cross_co.push_back(2 * (cy - y1));                                                           // b
    cross_co.push_back((x1 + cx) * (x1 - cx) + (y1 + cy) * (y1 - cy) + (rad + r1) * (rad - r1)); // c

    return cross_co;
  }

  vector<double> get_crosspoint(const vector<double> &V) // ax + by + c = 0
  {
    vector<double> cross;
    double a = V[0];
    double b = V[1];
    double c = V[2];

    double d = fabs(a * cx + b * cy + c) / sqrt(a * a + b * b);
    // cout << "d ; " << d << " " << rad << endl;

    double theta = atan2(b, a);

    if (d > rad)
      {
	cout << "d > rad" << endl;
      }
    else if (d == rad)
      {
	cout << "d == rad" << endl;

	if (a * cx + b * cy + c > 0)
	  theta += M_PI;
	cross.push_back(rad * cos(theta) + cx);
	cross.push_back(rad * sin(theta) + cy);
      }
    else
      {
	cout << "d < rad" << endl;
	double alpha, beta, phi;
	phi = acos(d / rad);
	alpha = theta - phi;
	beta = theta + phi;
	if (a * cx + b * cy + c > 0)
	  {
	    alpha += M_PI;
	    beta += M_PI;
	  }
	// double temp_cross[2][2];
	vector<vector<double>> temp_cross = {{rad * cos(alpha) + cx, rad * cos(beta) + cx}, {rad * sin(alpha) + cy, rad * sin(beta) + cy}};
	// for (unsigned int j = 0; j < temp_cross.at(0).size(); j++)
	// {
	//     cout << "temp_cross : ";
	//     for (unsigned int i = 0; i < temp_cross.size(); i++)
	//         cout << temp_cross.at(j).at(i) << "  ";
	//     cout << endl;
	// }
	cross = get_closestpoint(temp_cross);
      }
    // cout << "cross size : " << cross.size() << endl;
    // for (int i = 0; i < cross.size(); i++)
    //     cout << cross[i] << endl;
    return cross;
  }

  vector<double> get_closestpoint(const vector<vector<double>> VV)
  {
    vector<double> closest;
    double pre_phi = 999;
    double phi_p1 = atan2(p1.y() - dca_mean[1], p1.x() - dca_mean[0]);
    for (unsigned int i = 0; i < VV.at(0).size(); i++)
      {
	cout << "dca_mean : " << dca_mean[0] << "  " << dca_mean[1] << endl;
	cout << "VV : " << VV[0][i] << "  " << VV[1][i] << endl;
	cout << "pVV : " << VV[0][i] - dca_mean[0] << "  " << VV[1][i] - dca_mean[1] << endl;
	cout << "p1  : " << p1.x() - dca_mean[0] << "  " << p1.y() - dca_mean[1] << endl;
	// cout << VV.at(i).at(0) << endl;
	double dot = (VV[0][i] - dca_mean[0]) * (p1.x() - dca_mean[0]) + (VV[1][i] - dca_mean[1]) * (p1.y() - dca_mean[1]);
	double temp_phi = atan2(VV[1][i] - dca_mean[1], VV[0][i] - dca_mean[0]);
	cout << "dot : " << dot << endl;
	if (dot > 0)
	  {
	    if (fabs(temp_phi - phi_p1) < fabs(pre_phi - phi_p1))
	      {
		closest.erase(closest.begin(), closest.end());
		cout << "VV size : " << VV.size() << " " << VV.at(0).size() << endl;
		for (int j = 0; j < 2; j++)
		  closest.push_back(VV[j][i]);
		pre_phi = temp_phi;
	      }
	  }
      }
    // closest = temp_closest;
    // for (unsigned int i = 0; i < VV.at(0).size(); i++)
    // {
    //     for (int j = 0; j < 2; j++)
    //         closest.push_back(VV[j][i]);
    // }
    // for (unsigned int i = 0; i < closest.size(); i++)
    //     cout << closest[i] << endl;

    return closest;
  }

  vector<double> get_crossframe(int Detect)
  {
    cout << endl;
    cout << "calc frame" << endl;
    double size;
    if (Detect == 0)
      size = 15;
    else if (Detect == 1)
      size = 120;
    vector<vector<double>> crossframe(2, vector<double>(0));
    for (int j = 0; j < 2; j++)
      {
	for (int i = 0; i < 2; i++)
	  {
	    double a_ = (j == 0) ? 1 : 0;
	    double b_ = (j == 0) ? 0 : 1;
	    double c_ = (2 * i - 1) * size;
	    cout << "kesu : " << a_ << " " << b_ << " " << c_ << endl;
	    vector<double> co_ = {a_, b_, c_};
	    vector<double> temp_crossframe = get_crosspoint(co_);
	    if (temp_crossframe.size() == 2)
	      {
		cout << "temp_crossframe : " << temp_crossframe[0] << "  " << temp_crossframe[1] << endl;
		crossframe.at(0).push_back(temp_crossframe[0]);
		crossframe.at(1).push_back(temp_crossframe[1]);
	      }
	  }
      }
    cout << "crossframe" << endl;
    cout << "size : " << crossframe.size() << "  " << crossframe.at(0).size() << endl;
    for (int j = 0; j < crossframe.at(0).size(); j++)
      {
	for (int i = 0; i < crossframe.size(); i++)
	  {
	    cout << crossframe[i][j] << ", ";
	  }
	cout << endl;
      }
    vector<double> result;
    if (crossframe.at(0).size() != 0)
      result = get_closestpoint(crossframe);
    cout << "closest frame" << endl;
    for (int i = 0; i < result.size(); i++)
      {
	cout << result[i] << ", ";
      }
    cout << endl;
    cout << "calc end" << endl;

    return result;
  }

  void print()
    {
      cout << "p1("
	   << setprecision(4) << setw(8) << p1.x() << ", "
	   << setprecision(4) << setw(8) << p1.y() << ", "
	   << setprecision(4) << setw(8) << p1.z() << ""
	   << ") --- "
	   << "p2("
	   << setprecision(4) << setw(8) << p2.x() << ", "
	   << setprecision(4) << setw(8) << p2.y() << ", "
	   << setprecision(4) << setw(8) << p2.z() << ""
	   << ") --- "
	   << "DCA("
	   << setprecision(4) << setw(8) << dca_mean[0] << ", "
	   << setprecision(4) << setw(8) << dca_mean[1] << ", "
	   << setprecision(4) << setw(8) << dca_mean[2] << ""
	   << ")"
	   << dca_range_out << is_deleted << "  "
	   << "slope = " << track_rz->GetParameter(1)
	   << "  Intercept = " << track_rz->GetParameter(0) << " "
	   << "theta = " << track_theta
	   << endl;
    }


  double GetTrackTheta()
  {
    return track_theta;
  }

  double GetTrackPhi( double r )
  {
    cerr << "double GetTrackPhi( double r ) is under construction" << endl;
    return 0;
  }

  void AddAssociatedClusterId( int i )
  {
    cluster_ids.push_back( i );
  }
    
};
