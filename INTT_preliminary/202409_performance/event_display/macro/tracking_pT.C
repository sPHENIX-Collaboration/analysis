// dca_meanx, dca_meany set by hand

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <sstream>
#include <iomanip>

#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TRandom3.h>
#include <TVector3.h>
#include <TColor.h>

int n_dotracking = 0;

using namespace std;
TCanvas *c;

TH2F *h_dphi_nocut;

#include "include_tracking/least_square2.cc"
#include "track_pT.hh"

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
  vector<double> zline;
  double theta_clus;
  double phi_clus;

  double dca_mean[3];

  void set(int Evt, uint64_t Bco, double X, double Y, double Z, double Adc, double Size, int Layer, double X_vtx, double Y_vtx, double Z_vtx)
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
    //
  }

  /*void print()
    {
    cout << "(" << setw(8) << x << ", " << setw(8) << y << ", " << setw(8) << z << ")\t"
    << size << "\t"
    << layer
    << endl;
    }*/

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

struct clustEvent
{
  int run_no = 0;
  int ievt = 0;
  // int bco_full = 0;
  bool mag_on = false;
  vector<cluster> vclus;
  vector<truth *> vtruth;
  vector<track *> vtrack;

  double dcaz_max = 9999;
  double dcaz_min = -9999;
  double dca2d_max = 9999;
  double dca2d_min = -9999;

  double dca_mean[3] = {0, 0, 0};

  void clear()
  {
    for (auto itr = vtrack.begin(); itr != vtrack.end(); ++itr)
      {
	delete *itr;
      }
    vtrack.clear();
  }

  void makeonetrack()
  {
    for (unsigned int i = 0; i < vtrack.size(); i++)
      {
	for (unsigned int j = i + 1; j < vtrack.size(); j++)
	  {
	    bool share_point = ((vtrack[i]->p1 == vtrack[j]->p1) || (vtrack[i]->p2 == vtrack[j]->p2));
	    share_point = share_point || ((vtrack[i]->p1 == vtrack[j]->p2) || (vtrack[i]->p2 == vtrack[j]->p1));
	    if (share_point)
	      {
		if (fabs(vtrack[i]->dca_2d) > fabs(vtrack[j]->dca_2d))
		  {
		    vtrack[i]->is_deleted = true;
		    continue;
		  }
		else
		  {
		    vtrack[j]->is_deleted = true;
		  }
	      }
	  }
      }
  }

  void dca_check(bool Debug)
  {
    for (unsigned int i = 0; i < vtrack.size(); i++)
      {
	if (vtrack[i]->dca_2d > dca2d_max)
	  vtrack[i]->dca2d_range_out = true;
	if (vtrack[i]->dca_2d < dca2d_min)
	  vtrack[i]->dca2d_range_out = true;

	if (vtrack[i]->dca[2] > dcaz_max)
	  vtrack[i]->dcaz_range_out = true;
	if (vtrack[i]->dca[2] < dcaz_min)
	  vtrack[i]->dcaz_range_out = true;

	if (vtrack[i]->dcaz_range_out || vtrack[i]->dca2d_range_out)
	  vtrack[i]->dca_range_out = true;

	// // if (Debug)
	// {
	//     if (vtrack[i]->dca_mean[2] > vtrack[i]->p1.z() && vtrack[i]->dca_mean[2] < vtrack[i]->p2.z())
	//         vtrack[i]->dca_range_out = true;
	//     if (vtrack[i]->dca_mean[2] > vtrack[i]->p2.z() && vtrack[i]->dca_mean[2] < vtrack[i]->p1.z())
	//         vtrack[i]->dca_range_out = true;
	// }
      }
  }

  void truth_check()
  {
    for (unsigned int i = 0; i < vtruth.size(); i++)
      {
	double p = sqrt(vtruth[i]->m_truthpt * vtruth[i]->m_truthpt + vtruth[i]->m_truthpz * vtruth[i]->m_truthpz);

	if (vtruth[i]->m_status == 1 && fabs(vtruth[i]->m_trutheta) < 2 && p > (50 * 1e-3))
	  {
	    if (fabs(vtruth[i]->m_truthz_out) < 22.8)
	      {
		vtruth[i]->is_intt = true;
	      }
	  }
      }
  }

  void cluster_check()
  {
    for (unsigned int itrt = 0; itrt < vtruth.size(); itrt++)
      {
	for (unsigned int icls = 0; icls < vclus.size(); icls++)
	  {
	    if (vclus[icls].layer < 2)
	      continue;

	    double d_phi = vclus[icls].getphi_clus() - vtruth[itrt]->m_truthphi;
	    d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

	    if (fabs(d_phi) < (0.0014 * 3))
	      {
		vtruth[itrt]->have_cluster = true;
		break;
	      }
	  }
      }
  }

  void track_check()
  {
    for (unsigned int i = 0; i < vtruth.size(); i++)
      {
	for (unsigned int itrk = 0; itrk < vtrack.size(); itrk++)
	  {
	    double d_phi = vtrack[itrk]->getphi() - vtruth[i]->m_truthphi;
	    d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

	    if (fabs(d_phi) < (3.3e-4 * 3))
	      {
		vtruth[i]->have_track = true;
		break;
	      }
	  }
      }
  }

  void charge_check()
  {
    for (unsigned int i = 0; i < vtruth.size(); i++)
      {
	if ((abs(vtruth[i]->m_truthpid) != 22 && abs(vtruth[i]->m_truthpid) != 2112 && abs(vtruth[i]->m_truthpid) != 130 && abs(vtruth[i]->m_truthpid) != 310 && abs(vtruth[i]->m_truthpid) != 111))
	  vtruth[i]->is_charged = true;
      }
  }

  void draw_intt(int mode)
  {

    const double kLayer_radii[4] = {7.1888, 7.800, 9.680, 10.330};
    if (mode == 0)
      {
	for (int i = 0; i < 4; i++)
	  {
	    auto circle = new TEllipse(0.0, 0.0, kLayer_radii[i]);
	    circle->SetLineColorAlpha(kGray, 0.5);
	    circle->SetLineWidth(2);
	    circle->SetFillStyle(0);
	    circle->Draw("same");
	  }
      }
    else if (mode == 1)
      {
	for (int j = 0; j < 2; j++)
	  {
	    for (int i = 0; i < 4; i++)
	      {
		TLine *line = new TLine(-22.8, (2 * j - 1) * kLayer_radii[i], 22.8, (2 * j - 1) * kLayer_radii[i]);
		line->SetLineColorAlpha(kGray, 0.5);
		line->SetLineWidth(2);
		line->Draw("same");
	      }
	  }
      }
  }

  void draw_frame(int mode = 0)
  {
    TH1 *flame;
    string mag;
    if (mag_on)
      mag = "B-on";
    else
      mag = "B-off";

    string title;

    if (mode == 0)
      {
	if (run_no == 1)
	  title = Form("x-y plane {MC(%s) event %d};x [cm];y [cm]", mag.c_str(), ievt);
	else
	  title = Form("x-y plane {run%d(%s) event %d};x [cm];y [cm]", run_no, mag.c_str(), ievt);
	flame = gPad->DrawFrame(-15, -15, 15, 15, title.c_str());
      }
    else if (mode == 1)
      {
	if (run_no == 1)
	  title = Form("z-r plane {MC(%s) event %d};z [cm];r [cm]", mag.c_str(), ievt);
	else
	  title = Form("z-r plane {run%d(%s) event %d};z [cm];r [cm]", run_no, mag.c_str(), ievt);
	flame = gPad->DrawFrame(-25, -15, 25, 15, title.c_str());
      }
  }

  void draw_tracklets(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
  {

    bool condition = !reverse;
    for (unsigned int i = 0; i < vtrack.size(); i++)
      {
	if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
	  {
	    continue;
	  }

	if (vtrack[i]->is_deleted == condition && is_deleted == true)
	  {
	    continue;
	  }

	TGraph *g_temp = new TGraph();
	g_temp->SetMarkerColor(color);
	g_temp->SetMarkerStyle(20);
	g_temp->SetMarkerSize(0.8);

	if (mode == 0)
	  {
	    g_temp->SetPoint(0, vtrack[i]->p1.x(), vtrack[i]->p1.y());
	    g_temp->SetPoint(1, vtrack[i]->p2.x(), vtrack[i]->p2.y());
	  }
	else if (mode == 1)
	  {
	    g_temp->SetPoint(0, vtrack[i]->p1.z(), vtrack[i]->getpointr(1));
	    g_temp->SetPoint(1, vtrack[i]->p2.z(), vtrack[i]->getpointr(2));
	  }
	g_temp->Draw("PLsame");
      }
  }

  void draw_trackline(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
  {
    bool condition = !reverse;
    vector<int> colorset = {kOrange - 3, kOrange + 3, kPink - 3, kPink + 3, /* kViolet - 3, kViolet + 3,*/ kAzure - 3, kAzure + 3, kTeal - 3, kTeal + 3, kSpring - 3, kSpring + 3};
    int color_ = 0;

    for (unsigned int i = 0; i < vtrack.size(); i++)
      {

	if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
	  {
	    continue;
	  }

	if (vtrack[i]->is_deleted == condition && is_deleted == true)
	  {
	    continue;
	  }
	color_ = color_ % colorset.size();
	TGraph *g_temp = new TGraph();
	g_temp->SetMarkerColor(colorset[color_]);
	// g_temp->SetLineColor(color);
	g_temp->SetMarkerStyle(20);
	g_temp->SetMarkerSize(0.8);

	if (mode == 0)
	  {
	    vtrack[i]->track_xy->SetLineColorAlpha(colorset[color_], 0.5);
	    // vtrack[i]->track_xy->SetLineColorAlpha(color, 0.5);
	    if (vtrack[i]->dca_mean[0] < vtrack[i]->p1.x())
	      {
		vtrack[i]->track_xy->SetRange(vtrack[i]->dca_mean[0], 15);
	      }
	    else
	      {
		vtrack[i]->track_xy->SetRange(-15, vtrack[i]->dca_mean[0]);
	      }

	    vtrack[i]->track_xy->Draw("Lsame");
	    g_temp->SetPoint(0, vtrack[i]->dca_mean[0], vtrack[i]->dca_mean[1]);
	    g_temp->SetPoint(1, vtrack[i]->p1.x(), vtrack[i]->p1.y());
	    g_temp->SetPoint(2, vtrack[i]->p2.x(), vtrack[i]->p2.y());
	  }
	else if (mode == 1)
	  {

	    if (isinf(vtrack[i]->track_rz->GetParameter(1)))
	      {
		TLine *line = nullptr;
		if (vtrack[i]->p1.y() > 0)
		  line = new TLine(vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3), vtrack[i]->dca_mean[2], 15);
		else
		  line = new TLine(vtrack[i]->dca_mean[2], -15, vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3));

		// line->SetLineColorAlpha(color, 0.5);
		line->SetLineColorAlpha(colorset[color_], 0.5);
		line->SetLineWidth(2);
		line->Draw("same");
	      }
	    else
	      {

		vtrack[i]->track_rz->SetLineColorAlpha(colorset[color_], 0.5);
		// vtrack[i]->track_rz->SetLineColorAlpha(color, 0.5);

		int y = 0;
		if (vtrack[i]->p1.y() > 0)
		  y = 15;
		else
		  y = -15;

		double r1 = vtrack[i]->track_rz_inv->Eval(vtrack[i]->getpointr(3));
		double r2 = vtrack[i]->track_rz_inv->Eval(y);

		if (r1 <= r2)
		  vtrack[i]->track_rz->SetRange(r1, r2);
		else
		  vtrack[i]->track_rz->SetRange(r2, r1);

		vtrack[i]->track_rz->Draw("Lsame");
	      }
	    g_temp->SetPoint(0, vtrack[i]->dca_mean[2], vtrack[i]->getpointr(3));
	    vector<TLine *> zlines;
	    for (int j = 0; j < 2; j++)
	      {
		TLine *line = new TLine(vtrack[i]->zline[j][0], vtrack[i]->getpointr(1 + j), vtrack[i]->zline[j][1], vtrack[i]->getpointr(1 + j));
		line->SetLineColor(colorset[color_]);
		line->SetLineWidth(3);
		zlines.push_back(line);
	      }
	    for (int j = 0; j < 2; j++)
	      zlines[j]->Draw("same");
	    // g_temp->SetPoint(1, vtrack[i]->p1.z(), vtrack[i]->getpointr(1));
	    // g_temp->SetPoint(2, vtrack[i]->p2.z(), vtrack[i]->getpointr(2));
	  }
	g_temp->Draw("Psame");
	color_++;
      }
  }

  double rad_deg(double Rad)
  {
    Rad = Rad / M_PI * 180;
    if (Rad < 0)
      Rad += 360;

    return Rad;
  }

  void draw_curve2(int mode, double px, double py, double pz, double rad, double cx, double cy, int color, const vector<double> &V)
  {
    if (V.size() == 2)
      {
	// cout<<"cross : "<<V.at(0)<<", "<<V.at(1)<<endl;
	double phi1 = atan2(V.at(1) - cy, V.at(0) - cx);
	phi1 = rad_deg(phi1);

	double phic = atan2(dca_mean[1] - cy, dca_mean[0] - cx);
	phic = rad_deg(phic);
	double phi_min, phi_max;

	phi_min = (phi1 < phic) ? phi1 : phic;
	phi_max = (phi1 < phic) ? phic : phi1;
	TEllipse *circle = nullptr;
	// TEllipse *circle2 = nullptr;
	circle = new TEllipse(cx, cy, rad, 0, phi_min, phi_max);
	if ((phi_max - phi_min) > 180)
	  circle = new TEllipse(cx, cy, rad, 0, -(360 - phi_max), phi_min);

	if (mode == 0)
	  {
	    circle->SetLineColorAlpha(color, 0.5);
	    circle->SetFillStyle(0);
	    circle->SetLineWidth(2);
	    circle->SetLineStyle(1);
	    circle->SetNoEdges(1);
	    circle->Draw("same");
	  }
	else if (mode == 1)
	  {
	    circle->SetLineColorAlpha(color, 0.9);
	    circle->SetFillStyle(0);
	    circle->SetLineWidth(3);
	    circle->SetLineStyle(9);
	    circle->SetNoEdges(1);
	    circle->Draw("same");
	  }
      }

    // TEllipse *circle2 = new TEllipse(cx, cy, rad, 0, 0, 360);
    // circle2->SetLineColorAlpha(kGray, 0.2);
    // circle2->SetFillStyle(0);
    // circle2->SetLineWidth(2);
    // circle2->SetLineStyle(1);
    // circle2->SetNoEdges(1);
    // circle2->Draw("same");
  }

  void draw_trackcurve(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
  {
    vector<int> colorset = {kOrange - 3, kOrange + 3, kPink - 3, kPink + 3, /*kViolet - 3, kViolet + 3,*/ kAzure - 3, kAzure + 3, kTeal - 3, kTeal + 3, kSpring - 3, kSpring + 3};
    int color_ = 0;
    bool condition = !reverse;

    for (unsigned int i = 0; i < vtrack.size(); i++)
      {
	color_ = color_ % colorset.size();

	if (vtrack[i]->dca_range_out == condition && dca_range_cut == true)
	  {
	    continue;
	  }

	if (vtrack[i]->is_deleted == condition && is_deleted == true)
	  {
	    continue;
	  }

	TGraph *g_temp = new TGraph();
	// g_temp->SetMarkerColor(kBlack);
	// g_temp->SetMarkerColor(color);
	g_temp->SetMarkerColor(colorset[color_]);
	// g_temp->SetLineColor(colorset[color_]);
	g_temp->SetMarkerStyle(20);
	g_temp->SetMarkerSize(0.8);

	if (mode == 0)
	  {
	    vector<double> cross_framepoint = get_crossframe(vtrack[i]->cx, vtrack[i]->cy, vtrack[i]->rad, vtrack[i]->p1.x(), vtrack[i]->p1.y());
	    // cout<<"cross_framepoint.size() : "<<cross_framepoint.size()<<endl;
	    // if(cross_framepoint.size() == 2)
	    // cout<<"cross : "<<cross_framepoint.at(0)<<", "<<cross_framepoint.at(1)<<endl;
	    draw_curve2(0, vtrack[i]->p_reco[0], vtrack[i]->p_reco[1], vtrack[i]->p_reco[2], vtrack[i]->rad, vtrack[i]->cx, vtrack[i]->cy, colorset[color_], cross_framepoint);

	    // cout<<"p1 : "<<vtrack[i]->p1.x()<<"  "<<vtrack[i]->p1.y()<<endl;

	    g_temp->SetPoint(0, vtrack[i]->dca_mean[0], vtrack[i]->dca_mean[1]);
	    g_temp->SetPoint(1, vtrack[i]->p1.x(), vtrack[i]->p1.y());
	    g_temp->SetPoint(2, vtrack[i]->p2.x(), vtrack[i]->p2.y());

	    // cout << "cross_emc_size : " << vtrack[i]->cross_emc.size() << endl;
	  }

	g_temp->Draw("Psame");
	color_++;
      }
  }
  void draw_truthcurve(int mode = 0, bool does_overlay = false, int color = kBlack, bool only_far_cluster = false, bool only_far_track = false)
  {
    for (unsigned int i = 0; i < vtruth.size(); i++)
      {
	if (vtruth[i]->is_intt == false)
	  continue;

	if (vtruth[i]->is_charged == false)
	  continue;

	if (vtruth[i]->have_track == true && only_far_track)
	  continue;

	if (vtruth[i]->have_cluster == true && only_far_cluster)
	  continue;

	TGraph *g_temp = new TGraph();
	g_temp->SetMarkerColor(color);
	g_temp->SetLineColor(color);
	g_temp->SetMarkerStyle(20);
	g_temp->SetMarkerSize(0.8);

	if (mode == 0)
	  {
	    vector<double> cross_framepoint = get_crossframe(vtruth[i]->center[0], vtruth[i]->center[1], vtruth[i]->m_rad, vtruth[i]->m_truthpx, vtruth[i]->m_truthpy);
	    draw_curve2(1, vtruth[i]->m_truthpx, vtruth[i]->m_truthpy, vtruth[i]->m_truthpz, vtruth[i]->m_rad, vtruth[i]->center[0], vtruth[i]->center[1], color, cross_framepoint);

	    g_temp->SetPoint(0, vtruth[i]->m_xvtx, vtruth[i]->m_yvtx);
	  }

	g_temp->Draw("Psame");
      }
  }

  void draw_truthline(int mode = 0, bool does_overlay = false, int color = kBlack, bool only_far_cluster = false, bool only_far_track = false)
  {
    for (unsigned int i = 0; i < vtruth.size(); i++)
      {
	if (vtruth[i]->is_intt == false)
	  continue;

	if (vtruth[i]->is_charged == false)
	  continue;

	if (vtruth[i]->have_track == true && only_far_track)
	  continue;

	if (vtruth[i]->have_cluster == true && only_far_cluster)
	  continue;

	TGraph *g_temp = new TGraph();
	g_temp->SetMarkerColor(color);
	g_temp->SetLineColor(color);
	g_temp->SetMarkerStyle(20);
	g_temp->SetMarkerSize(0.8);

	if (mode == 0)
	  {
	    vtruth[i]->truth_xy->SetLineColorAlpha(color, 0.8);
	    vtruth[i]->truth_xy->SetLineStyle(9);
	    vtruth[i]->truth_xy->SetLineWidth(3);

	    if (vtruth[i]->m_truthpx > 0)
	      {
		vtruth[i]->truth_xy->SetRange(vtruth[i]->m_xvtx, 15);
	      }
	    else
	      {
		vtruth[i]->truth_xy->SetRange(-15, vtruth[i]->m_xvtx);
	      }
	    vtruth[i]->truth_xy->Draw("Lsame");

	    g_temp->SetPoint(0, vtruth[i]->m_xvtx, vtruth[i]->m_yvtx);
	  }
	else if (mode == 1)
	  {
	    vtruth[i]->truth_rz->SetLineColorAlpha(color, 0.8);
	    vtruth[i]->truth_rz->SetLineStyle(9);
	    vtruth[i]->truth_rz->SetLineWidth(3);

	    int y = 0;
	    if (vtruth[i]->m_truthpy > 0)
	      y = 15;
	    else
	      y = -15;
	    double r_min = vtruth[i]->truth_rz->GetX(vtruth[i]->getpointr(3));
	    double r_max = vtruth[i]->truth_rz->GetX(y);
	    if (r_min < r_max)
	      vtruth[i]->truth_rz->SetRange(r_min, r_max);
	    else
	      vtruth[i]->truth_rz->SetRange(r_max, r_min);

	    vtruth[i]->truth_rz->Draw("Lsame");
	    g_temp->SetPoint(0, vtruth[i]->m_zvtx, vtruth[i]->getpointr(3));
	  }
	g_temp->Draw("Psame");
      }
  }

  void draw_clusters(int mode = 0, bool does_overlay = false, int color = kBlack)
  {
    for (unsigned int i = 0; i < vclus.size(); i++)
      {
	TGraph *g_temp = new TGraph();
	g_temp->SetMarkerColorAlpha(color, 0.5);
	// g_temp->SetLineColor(color);
	g_temp->SetMarkerStyle(20);
	g_temp->SetMarkerSize(0.8);
	TLine *line = new TLine(vclus[i].zline[0], vclus[i].r, vclus[i].zline[1], vclus[i].r);
	line->SetLineColorAlpha(color, 0.5);
	line->SetLineWidth(3);

	if (mode == 0)
	  {
	    g_temp->SetPoint(0, vclus[i].x, vclus[i].y);
	    g_temp->Draw("psame");
	  }
	else if (mode == 1)
	  {
	    line->Draw("same");
	    // g_temp->SetPoint(0, vclus[i].z, vclus[i].r);
	  }
	// g_temp->Draw("psame");
      }
  }

  //////////////////////////////////
  //////////////////////////////////

  // vector<double> get_crossline(double x1, double y1, double r1)
  // {
  //     vector<double> cross_co;
  //     // ax + by + c = 0
  //     cross_co.push_back(2 * (cx - x1));                                                           // a
  //     cross_co.push_back(2 * (cy - y1));                                                           // b
  //     cross_co.push_back((x1 + cx) * (x1 - cx) + (y1 + cy) * (y1 - cy) + (rad + r1) * (rad - r1)); // c

  //     return cross_co;
  // }

  vector<double> get_crosspoint(const vector<double> &V, double cx, double cy, double rad, double p1_x, double p1_y) // ax + by + c = 0
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
	// cout << "d > rad" << endl;
      }
    else if (d == rad)
      {
	// cout << "d == rad" << endl;

	if (a * cx + b * cy + c > 0)
	  theta += M_PI;
	cross.push_back(rad * cos(theta) + cx);
	cross.push_back(rad * sin(theta) + cy);
      }
    else
      {
	// cout << "d < rad" << endl;
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
	cross = get_closestpoint(temp_cross, p1_x, p1_y);
      }
    // cout << "cross size : " << cross.size() << endl;
    // for (int i = 0; i < cross.size(); i++)
    //     cout << cross[i] << endl;
    return cross;
  }

  vector<double> get_closestpoint(const vector<vector<double>> VV, double p1_x, double p1_y)
  {
    vector<double> closest;
    double pre_phi = 999;
    double phi_p1 = atan2(p1_y - dca_mean[1], p1_x - dca_mean[0]);
    for (unsigned int i = 0; i < VV.at(0).size(); i++)
      {
	// cout << "dca_mean : " << dca_mean[0] << "  " << dca_mean[1] << endl;
	// cout << "VV : " << VV[0][i] << "  " << VV[1][i] << endl;
	// cout << "pVV : " << VV[0][i] - dca_mean[0] << "  " << VV[1][i] - dca_mean[1] << endl;
	// cout << "p1  : " << p1.x() - dca_mean[0] << "  " << p1.y() - dca_mean[1] << endl;
	// cout << VV.at(i).at(0) << endl;
	double dot = (VV[0][i] - dca_mean[0]) * (p1_x - dca_mean[0]) + (VV[1][i] - dca_mean[1]) * (p1_y - dca_mean[1]);
	double temp_phi = atan2(VV[1][i] - dca_mean[1], VV[0][i] - dca_mean[0]);
	// cout << "dot : " << dot << endl;
	if (dot > 0)
	  {
	    if (fabs(temp_phi - phi_p1) < fabs(pre_phi - phi_p1))
	      {
		closest.erase(closest.begin(), closest.end());
		// cout << "VV size : " << VV.size() << " " << VV.at(0).size() << endl;
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
    //  // cout << closest[i] << endl;

    return closest;
  }

  vector<double> get_crossframe(double cx, double cy, double rad, double p1_x, double p1_y)
  {
    // cout << endl;
    // cout << "calc frame" << endl;
    double size = 15;
    vector<vector<double>> crossframe(2, vector<double>(0));
    for (int j = 0; j < 2; j++)
      {
	for (int i = 0; i < 2; i++)
	  {
	    double a_ = (j == 0) ? 1 : 0;
	    double b_ = (j == 0) ? 0 : 1;
	    double c_ = (2 * i - 1) * size;
	    // cout << "kesu : " << a_ << " " << b_ << " " << c_ << endl;
	    vector<double> co_ = {a_, b_, c_};
	    vector<double> temp_crossframe = get_crosspoint(co_, cx, cy, rad, p1_x, p1_y);
	    if (temp_crossframe.size() == 2)
	      {
		// cout << "temp_crossframe : " << temp_crossframe[0] << "  " << temp_crossframe[1] << endl;
		crossframe.at(0).push_back(temp_crossframe[0]);
		crossframe.at(1).push_back(temp_crossframe[1]);
	      }
	  }
      }
    // cout << "crossframe" << endl;
    // cout << "size : " << crossframe.size() << "  " << crossframe.at(0).size() << endl;
    for (int j = 0; j < crossframe.at(0).size(); j++)
      {
	for (int i = 0; i < crossframe.size(); i++)
	  {
	    // cout << crossframe[i][j] << ", ";
	  }
	// cout << endl;
      }
    vector<double> result;
    if (crossframe.at(0).size() != 0)
      result = get_closestpoint(crossframe, p1_x, p1_y);
    // cout << "closest frame" << endl;
    for (int i = 0; i < result.size(); i++)
      {
	// cout << result[i] << ", ";
      }
    // cout << endl;
    // cout << "calc end" << endl;

    return result;
  }
};

template <class D>
void erase(vector<D> &vec)
{
  vec.erase(vec.begin(), vec.end());
}

template <class D>
void reverse(vector<D> &vec)
{
  reverse(vec.begin(), vec.end());
}

bool dotracking(clustEvent &vCluster)
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
	  vCluster.vtrack.push_back(tr);
        }
    }

  return true;
}

void tracking_pT(int run_no = 41981, bool mag_on = true, bool debug = false )
{
  int sigma = 3;
  int nloop = 60;

  bool geant = false;
  if (run_no == 1)
    geant = true;

  bool does_reverse = false;
  // does_reverse = true;

  string data_dir = "/sphenix/u/nukazuka/work_now/analysis/tracking/hinakos/work/F4AInttRead/macro/";
  string fname = data_dir + Form("InttAna_run%d.root", run_no); // with no any cut
  // if (mag_on)
  //   fname = Form("AnaTutorial_inttana_%d_mag.root", run_no); // with no any cut

  // string fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/readdst/AnaTutorial_inttana_41045.root";//
  // string fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/readdst/AnaTutorial_inttana_40806.root";//
  // string fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_1K.root";
  // string fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_vtx0.root";
  if (geant)
    {
      if (mag_on)
        {
	  // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_1K.root";
	  fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_zvtxwidth10_ptmin0.root";
	  // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_geant_10K_bon_lowpt5_minus.root";
        }

      else
	{
	  fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_vtx0.root";
	  // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_ptmin0.root";
	  // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_zvtxwidth10_ptmin0.root";
	  // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_geant_10K_lowpt5_minus.root";
	  // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_geant_10K.root";
	}
    }
  
  TFile *f = TFile::Open(fname.c_str());
  cout << "opened file : " << fname << endl;

  string dir_out = "results/tracking/";
  TString fname_out = "tracking_run" + to_string( run_no ) + ".root";
  // fname_out.ReplaceAll("AnaTutorial_inttana", "tracking");

  // if (does_reverse)
  //   fname_out.ReplaceAll(".root", "_reverse.root");
  // if (debug)
  //   fname_out.ReplaceAll(".root", "_debug.root");

  // fname_out.ReplaceAll(".root", "_test.root");

  fname_out = dir_out + fname_out;
  cout << "out put file : " << fname_out << endl;

  TH1F *h_dcaz_one_ = new TH1F("h_dcaz_one_", "", 100, -200, 200);

  TFile *froot = new TFile(fname_out, "recreate");
  TH1F *h_nclus = new TH1F("h_nclus", "", 100, 0, 100);

  TTree *temp_tree_ = new TTree("temp_tree_", "temp_tree_");
  TTree *clus_tree = new TTree("clus_tree", "clus_tree");
  TTree *truth_tree = new TTree("truth_tree", "truth_tree");
  TTree *track_tree = new TTree("track_tree", "track_tree");

  h_dphi_nocut = new TH2F("h_dphi_nocut", "d_phi : phi_in;phi_in;d_phi", 200, -3.2, 3.2, 200, -0.1, 0.1);

  TH1F *h_dcaz_sigma_one = new TH1F("h_dcaz_sigma_one", "h_dcaz_sigma_one;dca_z sigma of one event;entries", 100, 0, 15);
  TH1D *h_xvtx = new TH1D("h_xvtx", "h_xvtx", 100, -0.01, 0.01);
  TH1D *h_yvtx = new TH1D("h_yvtx", "h_yvtx", 100, -0.01, 0.01);
  TH1D *h_zvtx = new TH1D("h_zvtx", "h_zvtx", 100, -20, 20);
  TH2D *h_zr[2];
  for (int i = 0; i < 2; i++)
    h_zr[i] = new TH2D(Form("h_zr_%d", i), "h_zr;z;r", 100, -20, 20, 100, -10, 10);

  c = new TCanvas(fname_out.ReplaceAll(".root", ".pdf"), "c", 1200, 600);
  c->Divide(2, 1);
  c->cd(1);

  int page_counter = 0;
  c->Print(((string)c->GetName() + "[").c_str());

  int evt_temp_;
  vector<double> d_phi_;
  vector<double> d_theta_;
  vector<double> phi_in_;
  vector<double> dca_x_;
  vector<double> dca_y_;
  vector<double> dca_z_;
  vector<double> dca_2d_;
  double zvtx_one_;
  double zvtx_sigma_one_;
  temp_tree_->Branch("evt_temp_", &evt_temp_, "evt_temp_/I");
  temp_tree_->Branch("d_phi_", &d_phi_);
  temp_tree_->Branch("d_theta_", &d_theta_);
  temp_tree_->Branch("phi_in_", &phi_in_);
  temp_tree_->Branch("dca_x_", &dca_x_);
  temp_tree_->Branch("dca_y_", &dca_y_);
  temp_tree_->Branch("dca_z_", &dca_z_);
  temp_tree_->Branch("dca_2d_", &dca_2d_);
  temp_tree_->Branch("zvtx_one_", &zvtx_one_, "zvtx_one_/D");
  temp_tree_->Branch("zvtx_sigma_one_", &zvtx_sigma_one_, "zvtx_sigma_one_/D");

  int evt_clus;
  vector<double> x_in;
  vector<double> y_in;
  vector<double> z_in;
  vector<double> r_in;
  vector<double> phi_in;
  vector<double> theta_in;
  vector<double> x_out;
  vector<double> y_out;
  vector<double> z_out;
  vector<double> r_out;
  vector<double> phi_out;
  vector<double> theta_out;
  clus_tree->Branch("evt_clus", &evt_clus, "evt_clus/I");
  clus_tree->Branch("x_in", &x_in);
  clus_tree->Branch("y_in", &y_in);
  clus_tree->Branch("z_in", &z_in);
  clus_tree->Branch("r_in", &r_in);
  clus_tree->Branch("phi_in", &phi_in);
  clus_tree->Branch("theta_in", &theta_in);
  clus_tree->Branch("x_out", &x_out);
  clus_tree->Branch("y_out", &y_out);
  clus_tree->Branch("z_out", &z_out);
  clus_tree->Branch("r_out", &r_out);
  clus_tree->Branch("phi_out", &phi_out);
  clus_tree->Branch("theta_out", &theta_out);

  int evt_track;
  int ntrack = 0;
  vector<double> slope_rz;
  vector<double> phi_tracklets;
  vector<double> theta_tracklets;
  vector<double> phi_track;
  vector<double> theta_track;
  vector<double> z_tracklets_out;
  vector<double> dca_2d;
  vector<double> dca_z;
  vector<int> is_deleted;
  vector<int> dca_range_out;
  vector<int> dca2d_range_out;
  vector<int> dcaz_range_out;
  vector<double> pT;
  vector<double> px_truth;
  vector<double> py_truth;
  vector<double> pz_truth;
  vector<double> px;
  vector<double> py;
  vector<double> pz;
  vector<double> rad;
  vector<double> pT_truth;
  vector<double> charge;
  double x_vertex;
  double y_vertex;
  double z_vertex;
  track_tree->Branch("evt_track", &evt_track, "evt_track/I");
  track_tree->Branch("ntrack", &ntrack, "ntrack/I");
  track_tree->Branch("phi_tracklets", &phi_tracklets);
  track_tree->Branch("slope_rz", &slope_rz);
  track_tree->Branch("theta_tracklets", &theta_tracklets);
  track_tree->Branch("phi_track", &phi_track);
  track_tree->Branch("theta_track", &theta_track);
  track_tree->Branch("z_tracklets_out", &z_tracklets_out);
  track_tree->Branch("dca_2d", &dca_2d);
  track_tree->Branch("dca_z", &dca_z);
  track_tree->Branch("is_deleted", &is_deleted);
  track_tree->Branch("dca_range_out", &dca_range_out);
  track_tree->Branch("dcaz_range_out", &dcaz_range_out);
  track_tree->Branch("dca2d_range_out", &dca2d_range_out);
  track_tree->Branch("pT", &pT);
  track_tree->Branch("px", &px);
  track_tree->Branch("py", &py);
  track_tree->Branch("pz", &pz);
  track_tree->Branch("px_truth", &px_truth);
  track_tree->Branch("py_truth", &py_truth);
  track_tree->Branch("pz_truth", &pz_truth);
  track_tree->Branch("pT_truth", &pT_truth);
  track_tree->Branch("charge", &charge);
  track_tree->Branch("rad", &rad);
  track_tree->Branch("x_vertex", &x_vertex, "x_vertex/D");
  track_tree->Branch("y_vertex", &y_vertex, "y_vertex/D");
  track_tree->Branch("z_vertex", &z_vertex, "z_vertex/D");

  int evt_truth;
  int ntruth = 0;
  int ntruth_cut = 0;
  vector<double> truthpx;
  vector<double> truthpy;
  vector<double> truthpz;
  vector<double> truthpt;
  vector<double> trutheta;
  vector<double> truththeta;
  vector<double> truthphi;
  vector<double> truthxvtx;
  vector<double> truthyvtx;
  vector<double> truthzvtx;
  vector<double> truthzout;
  vector<int> truthpid;
  vector<int> status;
  truth_tree->Branch("evt_truth", &evt_truth, "evt_truth/I");
  truth_tree->Branch("ntruth_cut", &ntruth_cut, "ntruth_cut/I");
  truth_tree->Branch("truthpx", &truthpx);
  truth_tree->Branch("truthpy", &truthpy);
  truth_tree->Branch("truthpz", &truthpz);
  truth_tree->Branch("truthpt", &truthpt);
  truth_tree->Branch("trutheta", &trutheta);
  truth_tree->Branch("truththeta", &truththeta);
  truth_tree->Branch("truthphi", &truthphi);
  truth_tree->Branch("truthxvtx", &truthxvtx);
  truth_tree->Branch("truthyvtx", &truthyvtx);
  truth_tree->Branch("truthzvtx", &truthzvtx);
  truth_tree->Branch("truthzout", &truthzout);
  truth_tree->Branch("truthpid", &truthpid);
  truth_tree->Branch("status", &status);

  // from input file
  TNtuple *ntp_clus = (TNtuple *)f->Get("ntp_clus");
  TTree *hepmctree = (TTree *)f->Get("hepmctree");
  bool no_mc = true;
  if( hepmctree == nullptr )
    no_mc = true;
  else if( hepmctree->GetEntries() == 0 )    
    no_mc = true;
  
  cout << "no_mc : " << no_mc << endl;

  Float_t nclus, nclus2, bco_full, evt, size, adc, x, y, z, lay, lad, sen, x_vtx, y_vtx, z_vtx;

  ntp_clus->SetBranchAddress("nclus", &nclus);
  ntp_clus->SetBranchAddress("nclus2", &nclus2);
  ntp_clus->SetBranchAddress("bco_full", &bco_full);
  ntp_clus->SetBranchAddress("evt", &evt);
  ntp_clus->SetBranchAddress("size", &size);
  ntp_clus->SetBranchAddress("adc", &adc);
  ntp_clus->SetBranchAddress("x", &x);
  ntp_clus->SetBranchAddress("y", &y);
  ntp_clus->SetBranchAddress("z", &z);
  ntp_clus->SetBranchAddress("lay", &lay);
  ntp_clus->SetBranchAddress("lad", &lad);
  ntp_clus->SetBranchAddress("sen", &sen);
  ntp_clus->SetBranchAddress("x_vtx", &x_vtx);
  ntp_clus->SetBranchAddress("y_vtx", &y_vtx);
  ntp_clus->SetBranchAddress("z_vtx", &z_vtx);

  double m_truthpx, m_truthpy, m_truthpz, m_truthpt, m_trutheta, m_truththeta, m_truthphi, m_xvtx, m_yvtx, m_zvtx;
  int m_evt, m_status, m_truthpid;

  hepmctree->SetBranchAddress("m_evt", &m_evt);
  hepmctree->SetBranchAddress("m_status", &m_status);
  hepmctree->SetBranchAddress("m_truthpx", &m_truthpx);
  hepmctree->SetBranchAddress("m_truthpy", &m_truthpy);
  hepmctree->SetBranchAddress("m_truthpz", &m_truthpz);
  hepmctree->SetBranchAddress("m_truthpt", &m_truthpt);
  hepmctree->SetBranchAddress("m_trutheta", &m_trutheta);
  hepmctree->SetBranchAddress("m_truthpid", &m_truthpid);
  hepmctree->SetBranchAddress("m_truththeta", &m_truththeta);
  hepmctree->SetBranchAddress("m_truthphi", &m_truthphi);
  hepmctree->SetBranchAddress("m_xvtx", &m_xvtx);
  hepmctree->SetBranchAddress("m_yvtx", &m_yvtx);
  hepmctree->SetBranchAddress("m_zvtx", &m_zvtx);

  int sum_event = 0;
  int ntrack_ = 0;
  for (int icls = 0; icls < ntp_clus->GetEntries(); icls++)
    {
      cout << Form("  -----  event %d {event gene.}  -----  ", sum_event) << endl;

      clustEvent event{};
      cluster clust{};

      ntp_clus->GetEntry(icls); // first cluster in one event
      clust.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
      event.vclus.push_back(clust);

      for (int j = 1; j < nclus; j++) // second~ cluster in one event
        {
	  ntp_clus->GetEntry(++icls);
	  cluster clust2{};
	  clust2.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
	  event.vclus.push_back(clust2);
        }

      dotracking(event);
      ntrack_ = event.vtrack.size();
      h_dcaz_one_->Reset();
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  h_dcaz_one_->Fill(event.vtrack[itrk]->dca[2]);
        }
      zvtx_one_ = h_dcaz_one_->GetMean();
      zvtx_sigma_one_ = h_dcaz_one_->GetStdDev();

      evt_temp_ = sum_event;
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  dca_x_.push_back(event.vtrack[itrk]->dca[0]);
	  dca_y_.push_back(event.vtrack[itrk]->dca[1]);
	  dca_z_.push_back(event.vtrack[itrk]->dca[2]);
	  dca_2d_.push_back(event.vtrack[itrk]->dca_2d);
	  d_phi_.push_back(event.vtrack[itrk]->d_phi);
	  d_theta_.push_back(event.vtrack[itrk]->d_theta);
	  phi_in_.push_back(event.vtrack[itrk]->phi_in);
        }
      if (does_reverse)
        {
	  reverse(dca_x_);
	  reverse(dca_y_);
	  reverse(dca_z_);
	  reverse(dca_2d_);
	  reverse(d_phi_);
	  reverse(d_theta_);
	  reverse(phi_in_);
        }
      temp_tree_->Fill();
      erase(dca_x_);
      erase(dca_y_);
      erase(dca_z_);
      erase(dca_2d_);
      erase(d_phi_);
      erase(d_theta_);
      erase(phi_in_);

      event.clear();
      sum_event++;

      if (debug && sum_event > nloop)
	break;
    }
  n_dotracking++;
  TH1F *h_dcax = new TH1F("h_dcax", "h_dcax", 100, -0.4, 0.4);
  temp_tree_->Draw("dca_x_>>h_dcax");
  TH1F *h_dcay = new TH1F("h_dcay", "h_dcay", 100, -0.4, 0.4);
  temp_tree_->Draw("dca_y_>>h_dcay");
  TH1F *h_dcaz = new TH1F("h_dcaz", "h_dcaz;DCA_z[cm]", 60, -150, 150);
  temp_tree_->Draw("dca_z_>>h_dcaz");
  TH1F *h_dtheta = new TH1F("h_dtheta", "dtheta{inner - outer};dtheta;entries", 100, -3.2, 3.2);
  temp_tree_->Draw("d_theta_>>h_dtheta");
  TH1F *h_dphi = new TH1F("h_dphi", "#Delta #phi_{AB};#Delta #phi_{AB}", 100, -1, 1);
  temp_tree_->Draw("d_phi_>>h_dphi");

  TH1F *h_dca2d = new TH1F("h_dca2d", "h_dca", 100, -10, 10);
  temp_tree_->Draw("dca_2d_>>h_dca2d");

  vector<TH1F *> h_ = {h_dcax, h_dcay, h_dcaz, h_dphi, h_dtheta, h_dca2d};

  // the mean of DCA in all event
  double dcax_mean = h_dcax->GetMean();
  double dcay_mean = h_dcay->GetMean();
  if (!(geant))
    {
      dcax_mean = -0.019;
      dcay_mean = 0.198;
    }
  double dcaz_mean = h_dcaz->GetMean();
  double dca2d_mean = h_dca2d->GetMean();

  double dcax_sigma = h_dcax->GetStdDev();
  double dcay_sigma = h_dcay->GetStdDev();
  double dcaz_sigma = h_dcaz->GetStdDev();
  double dca2d_sigma = h_dca2d->GetStdDev();

  dca2d_sigma *= sigma;
  dcaz_sigma *= sigma;

  double dca2d_max = dca2d_mean + dca2d_sigma;
  double dca2d_min = dca2d_mean - dca2d_sigma;
  double dcaz_max = dcaz_mean + dcaz_sigma;
  double dcaz_min = dcaz_mean - dcaz_sigma;

  // tracking
  int ihit = 0;
  int itruth = 0;
  int temp_evt = 0;
  //  for (int ievt = 0; ievt < sum_event; ievt++, ihit++, itruth++)
  for (int ievt = 0; ievt < 100; ievt++, ihit++, itruth++)
    {
      temp_tree_->GetEntry(ievt);
      cout << Form("  -----  event %d  -----  ", ievt) << endl;
      ntruth = 0;
      ntruth_cut = 0;

      clustEvent event;
      event.ievt = ievt;
      event.mag_on = mag_on;
      event.run_no = run_no;
      // event.bco_full = bco_full;

      if (ihit < ntp_clus->GetEntries())
        {
	  ntp_clus->GetEntry(ihit);

	  if (!(no_mc))
            {
	      hepmctree->GetEntry(itruth);
	      while (m_evt != evt)
                {
		  itruth++;
		  hepmctree->GetEntry(itruth);
                }
	      temp_evt = m_evt;
            }
	  cluster clust{};

	  clust.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
	  event.vclus.push_back(clust);

	  for (int j = 1; j < nclus; j++) // second~ cluster in one event
            {
	      ntp_clus->GetEntry(++ihit);
	      cluster clust2{};

	      clust2.set(evt, 0, x, y, z, adc, size, lay, x_vtx, y_vtx, z_vtx);
	      event.vclus.push_back(clust2);
            }

	  if (!(no_mc))
            {
	      while (m_evt == temp_evt)
                {
		  ntruth++;
		  truth *tru = new truth();
		  tru->set_truth(m_truthpx, m_truthpy, m_truthpz, m_truthpt, m_status, m_trutheta, m_truthpid, m_truththeta, m_truthphi, m_xvtx, m_yvtx, m_zvtx);
		  event.vtruth.push_back(tru);
		  itruth++;
		  if (itruth == hepmctree->GetEntries())
		    break;

		  hepmctree->GetEntry(itruth);
                }
	      itruth--;
            }
        }

      event.dca_mean[0] = dcax_mean;
      event.dca_mean[1] = dcay_mean;
      event.dca_mean[2] = zvtx_one_;

      // from dca_mean
      dotracking(event);
      ntrack_ = event.vtrack.size();
      evt_clus = ievt;
      evt_track = ievt;
      evt_truth = ievt;
      h_dcaz_one_->Reset();
      for (int itrk = 0; itrk < ntrack_; itrk++) // from dca_mean
        {
	  h_dcaz_one_->Fill(event.vtrack[itrk]->dca[2]);
        }
      double dcaz_mean_one = h_dcaz_one_->GetMean();    // one event
      double dcaz_sigma_one = h_dcaz_one_->GetStdDev(); // one event
      h_dcaz_sigma_one->Fill(dcaz_sigma_one);
      dcaz_sigma_one *= sigma;
      double dcaz_max_one = dcaz_mean_one + dcaz_sigma;
      double dcaz_min_one = dcaz_mean_one - dcaz_sigma;
      zvtx_sigma_one_ *= sigma;
      // double dcaz_max_one = zvtx_one_ + zvtx_sigma_one_;
      // double dcaz_min_one = zvtx_one_ - zvtx_sigma_one_;

      event.dca2d_max = dca2d_max;
      event.dca2d_min = dca2d_min;
      event.dcaz_max = dcaz_max_one;
      event.dcaz_min = dcaz_min_one;

      if (!(no_mc))
        {
	  for (int itrt = 0; itrt < ntruth; itrt++)
            {
	      event.vtruth[itrt]->dca_mean[0] = dcax_mean;
	      event.vtruth[itrt]->dca_mean[1] = dcay_mean;
	      event.vtruth[itrt]->dca_mean[2] = dcaz_mean_one;

	      event.vtruth[itrt]->calc_line();
	      event.vtruth[itrt]->calc_center();
            }
        }
      h_nclus->Fill(event.vclus.size());

      for (int iclus = 0; iclus < event.vclus.size(); iclus++)
        {
	  event.vclus[iclus].dca_mean[0] = dcax_mean;
	  event.vclus[iclus].dca_mean[1] = dcay_mean;
	  event.vclus[iclus].dca_mean[2] = dcaz_mean_one;

	  if (event.vclus[iclus].layer < 2)
            {
	      x_in.push_back(event.vclus[iclus].x);
	      y_in.push_back(event.vclus[iclus].y);
	      z_in.push_back(event.vclus[iclus].z);
	      r_in.push_back(event.vclus[iclus].r);
	      phi_in.push_back(event.vclus[iclus].getphi_clus());
	      theta_in.push_back(event.vclus[iclus].gettheta_clus());
            }
	  if (1 < event.vclus[iclus].layer)
            {
	      x_out.push_back(event.vclus[iclus].x);
	      y_out.push_back(event.vclus[iclus].y);
	      z_out.push_back(event.vclus[iclus].z);
	      r_out.push_back(event.vclus[iclus].r);
	      phi_out.push_back(event.vclus[iclus].getphi_clus());
	      theta_out.push_back(event.vclus[iclus].gettheta_clus());
            }
        }
      if (does_reverse)
        {
	  reverse(x_out);
	  reverse(y_out);
	  reverse(z_out);
	  reverse(r_out);
	  reverse(phi_out);
	  reverse(theta_out);

	  reverse(x_in);
	  reverse(y_in);
	  reverse(z_in);
	  reverse(r_in);
	  reverse(phi_in);
	  reverse(theta_in);
        }
      clus_tree->Fill();
      erase(x_in);
      erase(y_in);
      erase(z_in);
      erase(r_in);
      erase(phi_in);
      erase(theta_in);
      erase(x_out);
      erase(y_out);
      erase(z_out);
      erase(r_out);
      erase(phi_out);
      erase(theta_out);

      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  event.vtrack[itrk]->dca_mean[0] = dcax_mean;
	  event.vtrack[itrk]->dca_mean[1] = dcay_mean;
	  event.vtrack[itrk]->dca_mean[2] = dcaz_mean_one;
	  event.vtrack[itrk]->calc_line();
	  event.vtrack[itrk]->calc_inv();
	  event.vtrack[itrk]->calc_pT();
        }
      event.dca_check(debug);
      event.makeonetrack();

      event.charge_check();
      event.truth_check();

      event.cluster_check();
      event.track_check();
      ntrack = 0;
      for (int itrk = 0; itrk < ntrack_; itrk++)
        {
	  slope_rz.push_back(event.vtrack[itrk]->track_rz->GetParameter(1));
	  phi_tracklets.push_back(event.vtrack[itrk]->getphi_tracklet());
	  theta_tracklets.push_back(event.vtrack[itrk]->gettheta_tracklet());
	  phi_track.push_back(event.vtrack[itrk]->getphi());
	  theta_track.push_back(event.vtrack[itrk]->gettheta());
	  dca_z.push_back(event.vtrack[itrk]->dca[2]);
	  dca_2d.push_back(event.vtrack[itrk]->dca_2d);
	  z_tracklets_out.push_back(event.vtrack[itrk]->p2.z());
	  pT.push_back(event.vtrack[itrk]->pT);
	  px.push_back(event.vtrack[itrk]->p_reco[0]);
	  py.push_back(event.vtrack[itrk]->p_reco[1]);
	  pz.push_back(event.vtrack[itrk]->p_reco[2]);
	  rad.push_back(event.vtrack[itrk]->rad);
	  charge.push_back(event.vtrack[itrk]->charge);
	  if (!(no_mc))
            {
	      pT_truth.push_back(event.vtruth[0]->m_truthpt);
	      px_truth.push_back(event.vtruth[0]->m_truthpx);
	      py_truth.push_back(event.vtruth[0]->m_truthpy);
	      pz_truth.push_back(event.vtruth[0]->m_truthpz);
            }

	  is_deleted.push_back(event.vtrack[itrk]->is_deleted);
	  dca2d_range_out.push_back(event.vtrack[itrk]->dca2d_range_out);
	  dcaz_range_out.push_back(event.vtrack[itrk]->dcaz_range_out);
	  dca_range_out.push_back(event.vtrack[itrk]->dca_range_out);

	  x_vertex = event.vtrack[itrk]->dca_mean[0];
	  y_vertex = event.vtrack[itrk]->dca_mean[1];
	  z_vertex = event.vtrack[itrk]->dca_mean[2];

	  if (event.vtrack[itrk]->is_deleted == true)
	    continue;
	  if (event.vtrack[itrk]->dca_range_out == true)
	    continue;
	  ntrack++;
	  h_xvtx->Fill(x_vertex);
	  h_yvtx->Fill(y_vertex);
	  h_zvtx->Fill(z_vertex);
        }

      if (does_reverse)
        {
	  reverse(slope_rz);
	  reverse(phi_tracklets);
	  reverse(theta_tracklets);
	  reverse(phi_track);
	  reverse(theta_track);
	  reverse(dca_z);
	  reverse(dca_2d);
	  reverse(is_deleted);
	  reverse(dca_range_out);
	  reverse(dca2d_range_out);
	  reverse(dcaz_range_out);
	  reverse(z_tracklets_out);
	  reverse(pT);
	  reverse(px);
	  reverse(py);
	  reverse(pz);
	  reverse(pT_truth);
	  reverse(px_truth);
	  reverse(py_truth);
	  reverse(pz_truth);
	  reverse(charge);
	  reverse(rad);
        }

      track_tree->Fill();

      erase(slope_rz);
      erase(phi_tracklets);
      erase(theta_tracklets);
      erase(phi_track);
      erase(theta_track);
      erase(dca_z);
      erase(dca_2d);
      erase(is_deleted);
      erase(dca_range_out);
      erase(dca2d_range_out);
      erase(dcaz_range_out);
      erase(z_tracklets_out);
      erase(pT);
      erase(px);
      erase(py);
      erase(pz);
      erase(pT_truth);
      erase(px_truth);
      erase(py_truth);
      erase(pz_truth);
      erase(charge);
      erase(rad);
      if (!(no_mc))
        {
	  for (int itrt = 0; itrt < ntruth; itrt++)
            {

	      if (event.vtruth[itrt]->is_charged == false)
		continue;

	      if (event.vtruth[itrt]->is_intt == false)
		continue;
	      ntruth_cut++;

	      truthpx.push_back(event.vtruth[itrt]->m_truthpx);
	      truthpy.push_back(event.vtruth[itrt]->m_truthpy);
	      truthpz.push_back(event.vtruth[itrt]->m_truthpz);
	      truthpt.push_back(event.vtruth[itrt]->m_truthpt);
	      trutheta.push_back(event.vtruth[itrt]->m_trutheta);
	      truththeta.push_back(event.vtruth[itrt]->m_truththeta);
	      truthphi.push_back(event.vtruth[itrt]->m_truthphi);
	      truthpid.push_back(event.vtruth[itrt]->m_truthpid);
	      status.push_back(event.vtruth[itrt]->m_status);
	      truthxvtx.push_back(event.vtruth[itrt]->m_xvtx);
	      truthyvtx.push_back(event.vtruth[itrt]->m_yvtx);
	      truthzvtx.push_back(event.vtruth[itrt]->m_zvtx);
	      truthzout.push_back(event.vtruth[itrt]->m_truthz_out);
            }
	  if (does_reverse)
            {
	      reverse(truthpx);
	      reverse(truthpy);
	      reverse(truthpz);
	      reverse(truthpt);
	      reverse(trutheta);
	      reverse(truththeta);
	      reverse(truthphi);
	      reverse(truthpid);
	      reverse(status);
	      reverse(truthxvtx);
	      reverse(truthyvtx);
	      reverse(truthzvtx);
	      reverse(truthzout);
            }
	  truth_tree->Fill();

	  erase(truthpx);
	  erase(truthpy);
	  erase(truthpz);
	  erase(truthpt);
	  erase(trutheta);
	  erase(truththeta);
	  erase(truthphi);
	  erase(truthpid);
	  erase(status);
	  erase(truthxvtx);
	  erase(truthyvtx);
	  erase(truthzvtx);
	  erase(truthzout);
        }

      // drawing
      // if (fabs(event.vclus.size() - 2 * ntrack) < 5 && /*debug &&*/ 10 < event.vclus.size() && event.vclus.size() < 50 /* && && geant &&*/ /*ievt < 500*/)
      // if (ievt < nloop && ievt == 56 /*&& ievt==56*/ /*&& fabs(event.dca_mean[2]) < 2. && ntrack < 20*/)
      {
	if( ievt > 100 )
	  continue;

	// c->cd(1);
	// event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(0, true, kBlack, false, false);
	// event.draw_clusters(0, true, kBlack);
	// c->Print(c->GetName());
	// c->cd(2);
	// event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(1, true, kBlack, false, false);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());

	// c->cd(1);
	// // // event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(0, false, kBlack, false, false);
	// event.draw_clusters(0, true, kBlack);

	// c->cd(2);
	// // // event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(1, false, kBlack, false, false);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_tracklets(0, false, kRed, true, false);
	// // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	// c->cd(2);
	// event.draw_tracklets(1, false, kRed, true, false);
	// // event.draw_truthline(1, true, TColor::GetColor(232, 85, 98));
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_tracklets(0, false, kAzure + 1, true, true);
	// // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	// c->cd(2);
	// event.draw_tracklets(1, false, kAzure + 1, true, true);
	// // event.draw_truthline(1, true, TColor::GetColor(232, 85, 98));
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_tracklets(0, false, kPink, false, true, true);
	// // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	// c->cd(2);
	// event.draw_tracklets(1, false, kPink, false, true, true);
	// // event.draw_truthline(1, true, TColor::GetColor(232, 85, 98));
	// c->Print(c->GetName());

	// c->cd(1);
	// // event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_truthcurve(0, false, TColor::GetColor(232, 85, 98));
	// // event.draw_tracklets(0, false, kGray + 1, false, false);
	// // event.draw_tracklets(0, true, TColor::GetColor(0, 118, 186), true, true);
	// event.draw_clusters(0, true, kBlack);
	// // event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// // event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->cd(2);
	// event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// // event.draw_tracklets(1, false, kGray + 1, false, false);
	// // event.draw_tracklets(1, true, TColor::GetColor(0, 118, 186), true, true);
	// event.draw_clusters(1, true, kBlack);
	// // event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->Print(c->GetName());
	// TCanvas *c0 = new TCanvas("c0", "c0", 1200, 600);
	// c0->Divide(2, 1);

	c->cd(1);
	// c0->cd(1);
	event.draw_frame(0);
	event.draw_intt(0);
	// event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_truthcurve(0, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(0, false, kGray + 1, false, false);
	// event.draw_tracklets(0, true, TColor::GetColor(0, 118, 186), true, true);
	event.draw_clusters(0, true, kBlack);
	// event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	if (!(no_mc))
	  cout << "draw_truth" << endl;
	if (mag_on)
	  {
	    // if (!(no_mc))
	    //     event.draw_truthcurve(0, true, kGray + 1);
	    // event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	    event.draw_trackcurve(0, true, TColor::GetColor(232, 85, 98), true, true, false);
	  }
	else
	  {
	    // if (!(no_mc))
	    //     event.draw_truthline(0, true, kGray + 1);
	    event.draw_trackline(0, true, TColor::GetColor(232, 85, 98) /*TColor::GetColor(88, 171, 141)*/, true, true, false);

	    // event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	  }
	// event.draw_tracklets(0, true, kGray + 1, true, true);
	c->cd(2);
	// c0->cd(2);
	event.draw_frame(1);
	event.draw_intt(1);
	// event.draw_tracklets(1, false, kGray + 1, false, false);
	// event.draw_tracklets(1, true, TColor::GetColor(0, 118, 186), true, true);
	event.draw_clusters(1, true, kBlack);
	// if (!(no_mc))
	//     event.draw_truthline(1, true, kGray + 1);
	event.draw_trackline(1, true, TColor::GetColor(232, 85, 98), true, true, false);
	// event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);

	// event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	// event.draw_tracklets(1, true, kGray + 1, true, true);
	// if(ievt == 56)
	// c->Write();
	// c0->Write();

	c->Print(c->GetName());

	/*c->cd(1);
	  event.draw_clusters(0, false, kBlack);
	  if(mag_on)
	  {
	  event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	  event.draw_truthcurve(0, true, TColor::GetColor(232, 85, 98));
	  }
	  else{
	  event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	  event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
	  }
	  c->cd(2);
	  event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
	  event.draw_clusters(1, true, kBlack);
	  event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);

	  c->Print(c->GetName());*/

	// c->cd(1);
	// event.draw_clusters(0, false, kBlack);
	// event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->cd(2);
	// event.draw_clusters(1, false, kBlack);
	// event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->Print(c->GetName());
	// c->cd(1);
	// event.draw_clusters(0, false, kBlack);
	// event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->cd(2);
	// event.draw_clusters(1, false, kBlack);
	// event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);
	// c->Print(c->GetName());

	// c->cd(1);
	// // event.draw_trackline(0, false, TColor::GetColor(88, 171, 141), true, true, false);
	// event.draw_truthline(0, false, kRed);
	// event.draw_clusters(0, true, kBlack);

	// c->cd(2);
	// // event.draw_trackline(1, false, TColor::GetColor(88, 171, 141), true, true, false);
	// event.draw_truthline(1, false,kRed);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());

	// c->cd(1);
	// event.draw_truthline(0, false, kBlue, true);
	// event.draw_clusters(0, true, kBlack);

	// c->cd(2);
	// event.draw_truthline(1, false, kBlue, true);
	// event.draw_clusters(1, true, kBlack);
	// c->Print(c->GetName());
      }
      event.clear();


    }

  // c->Print(c->GetName());
  c->Print(((string)c->GetName() + "]").c_str());

  froot->Write();
  for (int ihst = 0; ihst < h_.size(); ihst++)
    {
      froot->WriteTObject(h_[ihst], h_[ihst]->GetName());
    }
  froot->Close();
}
