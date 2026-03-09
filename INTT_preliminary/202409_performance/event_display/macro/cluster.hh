#pragma once

struct cluster
{
  int evt;
  uint64_t bco;

  double x, y, z, r;
  double adc;
  double size;
  int layer;
  TVector3 pos;

  double x_vtx;
  double y_vtx;
  double z_vtx;
  double r_vtx;
  vector < double > zline;
  double theta_clus;
  double phi_clus;

  // information related to tracklet
  bool is_associated;
  double track_incoming_theta;
  double track_incoming_phi;

  double dca_mean[3];

  void set(int Evt, uint64_t Bco,
	   double X, double Y, double Z,
	   double Adc, double Size, int Layer,
	   double X_vtx, double Y_vtx, double Z_vtx )
  {
    evt = Evt;
    bco = Bco;
    x = X;
    y = Y;
    z = Z;
    adc = Adc;
    size = Size;
    layer = Layer;
    pos = TVector3(x, y, z);
    r = y / fabs(y) * sqrt(x * x + y * y);
    get_zline();

    // truth vertex
    x_vtx = X_vtx;
    y_vtx = Y_vtx;
    z_vtx = Z_vtx;
    r_vtx = (y_vtx / fabs(y_vtx)) * sqrt(x_vtx * x_vtx + y_vtx * y_vtx);

    is_associated = false;
    track_incoming_theta = 0;
    track_incoming_phi = 0;
    //
  }

  void print()
  {
    cout << "(" << setw(8) << x << ", " << setw(8) << y << ", " << setw(8) << z << ")\t"
	 << size << "\t"
	 << layer << "\t"
	 << adc << "\t"
	 << is_associated << " "
	 << track_incoming_theta
	 << endl;
  }
  
  double getphi_clus() // from truth vertex
  {
    phi_clus = atan2((y - y_vtx), (x - x_vtx));

    return phi_clus;
  }

  double gettheta_clus() // from truth vertex
  {
    // from the truth vertex
    theta_clus = atan2((fabs(r) - fabs(r_vtx)), (z - z_vtx));

    return theta_clus;
  }

  void get_zline()
  {
    double chip_width;

    if (fabs(z) <= 12.8)
      chip_width = 0.8;
    else
      chip_width = 1.;
    
    zline.push_back(z - chip_width);
    zline.push_back(z + chip_width);
  }
  
};
