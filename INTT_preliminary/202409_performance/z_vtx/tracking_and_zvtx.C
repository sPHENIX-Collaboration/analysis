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
TH2F *h_dphi_cut;

#include "least_square2.cc"
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
    int ladder;
    int sensor;        // senser = 0 or 2 : chiptype A // senser = 1 or 3 : chiptype B
    double chip_width; // chip_width=1.6[cm] : chiptype A // chip_width=2.0[cm] : chiptype B

    double x_vtx;
    double y_vtx;
    double zv;
    double r_vtx;

    double theta_clus;
    double phi_clus;

    double dca_mean[3];

    void set(int Evt, uint64_t Bco, double X, double Y, double Z, double Adc, double Size, int Layer, int Ladder, int Sensor)
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
        // truth vertex
        // x_vtx = X_vtx;
        // y_vtx = Y_vtx;
        // zv = Zv;
        // r_vtx = (y_vtx / fabs(y_vtx)) * sqrt(x_vtx * x_vtx + y_vtx * y_vtx);
        ladder = Ladder;
        sensor = Sensor;
        if (sensor == 0 || sensor == 2)
            chip_width = 16 * 1e-1;

        else if (sensor == 1 || sensor == 3)
            chip_width = 20 * 1e-1;
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
        theta_clus = atan2((fabs(r) - fabs(r_vtx)), (z - zv));

        return theta_clus;
    }
};

struct clustEvent
{
    int run_nu = 0;
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

    // double dca_mean[3] = {0, 0, 0};
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
                if (fabs(vtruth[i]->m_truthz_out) < 23.)
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
                    TLine *line = new TLine(-23., (2 * j - 1) * kLayer_radii[i], 23., (2 * j - 1) * kLayer_radii[i]);
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
            if (run_nu == 1)
                title = Form("x-y plane {MC(%s) event %d};x[cm];y[cm]", mag.c_str(), ievt);
            else
                title = Form("x-y plane {run%d(%s) event %d};x[cm];y[cm]", run_nu, mag.c_str(), ievt);
            flame = gPad->DrawFrame(-15, -15, 15, 15, title.c_str());
        }
        else if (mode == 1)
        {
            if (run_nu == 1)
                title = Form("z-r plane {MC(%s) event %d};z[cm];r[cm]", mag.c_str(), ievt);
            else
                title = Form("z-r plane {run%d(%s) event %d};z[cm];r[cm]", run_nu, mag.c_str(), ievt);
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
        vector<int> colorset = {kOrange - 3, kOrange + 3, kPink - 3, kPink + 3, kViolet - 3, kViolet + 3, kAzure - 3, kAzure + 3, kTeal - 3, kTeal + 3, kSpring - 3, kSpring + 3};
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

                    line->SetLineColorAlpha(colorset[color_], 0.5);
                    line->SetLineWidth(2);
                    line->Draw("same");
                }
                else
                {

                    vtrack[i]->track_rz->SetLineColorAlpha(colorset[color_], 0.5);

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
                g_temp->SetPoint(1, vtrack[i]->p1.z(), vtrack[i]->getpointr(1));
                g_temp->SetPoint(2, vtrack[i]->p2.z(), vtrack[i]->getpointr(2));
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

    void draw_curve2(int mode, double px, double py, double pz, double rad, double cx, double cy, int color)
    {
        double sign_x = px / fabs(px);
        double sign_y = py / fabs(py);

        cout << "sign_x : " << sign_x << "  " << sign_y << endl;
        double x1 = sqrt((rad * rad) - ((sign_y * 15 - cy) * (sign_y * 15 - cy))) + cx;  // y = 15
        double x2 = -sqrt((rad * rad) - ((sign_y * 15 - cy) * (sign_y * 15 - cy))) + cx; // y = 15
        double y1 = sqrt((rad * rad) - ((sign_x * 15 - cx) * (sign_x * 15 - cx))) + cy;  // x = 15
        double y2 = -sqrt((rad * rad) - ((sign_x * 15 - cx) * (sign_x * 15 - cx))) + cy; // x = 15
        cout << "frame : " << sign_y * 15 << "  " << sign_x * 15 << endl;

        cout << x1 << "  " << x2 << "  " << y1 << "  " << y2 << "  " << endl;

        // cross
        double cross_x1 = px * (x1 - dca_mean[0]) + py * (sign_y * 15 - dca_mean[1]);
        double cross_x2 = px * (x2 - dca_mean[0]) + py * (sign_y * 15 - dca_mean[1]);
        double cross_y1 = px * (sign_x * 15 - dca_mean[0]) + py * (y1 - dca_mean[1]);
        double cross_y2 = px * (sign_x * 15 - dca_mean[0]) + py * (y2 - dca_mean[1]);

        cout << "cross : " << cross_x1 << "  " << cross_x2 << "  " << cross_y1 << "  " << cross_y2 << "  " << endl;
        double temp_x, temp_y;

        // if(cross_x1 > 0)
        // temp_x = x1;
        // else if(cross_x2 > 0)
        // temp_x = x2;

        // if(cross_y1 > 0)
        // temp_y = y1;
        // else if(cross_y2 > 0)
        // temp_y = y2;

        if (-15 <= x1 && x1 <= 15)
            temp_x = x1;
        if (-15 <= x2 && x2 <= 15)
            temp_x = x2;
        if (-15 <= y1 && y1 <= 15)
            temp_y = y1;
        if (-15 <= y2 && y2 <= 15)
            temp_y = y2;

        double temp_phix = atan2(sign_y * 15 - cy, temp_x - cx);
        double temp_phiy = atan2(temp_y - cy, sign_x * 15 - cx);

        temp_phix = rad_deg(temp_phix);
        temp_phiy = rad_deg(temp_phiy);

        double phi_min, phi_max;
        double phic = atan2(0 - cy, 0 - cx);
        phic = rad_deg(phic);
        double temp_phi;

        bool nan_x = ((rad) * (rad) - (sign_x * 15 - cx) * (sign_x * 15 - cx) < 0);
        bool nan_y = ((rad) * (rad) - (sign_y * 15 - cy) * (sign_y * 15 - cy) < 0);

        if (isnan(temp_phix) && isnan(temp_phiy))
        {
            temp_phi = phic;
        }
        else if (isnan(temp_phix))
        {
            temp_phi = temp_phiy;
        }
        else if (isnan(temp_phiy))
        {
            temp_phi = temp_phix;
        }
        else
        {
            if (fabs(phic - temp_phix) > fabs(phic - temp_phiy))
                temp_phi = temp_phiy;
            else
                temp_phi = temp_phix;
        }

        if (phic < temp_phi)
        {
            phi_min = phic;
            phi_max = temp_phi;
        }
        else
        {
            phi_min = temp_phi;
            phi_max = phic;
        }

        if (fabs(phi_max - phi_min) > 180)
        {
            phi_max = -(360 - phi_max);
            double temp = phi_min;
            phi_min = phi_max;
            phi_max = temp;
        }

        if (mode == 0)
        {
            TEllipse *circle = new TEllipse(cx, cy, rad, 0, phi_min, phi_max);
            circle->SetLineColorAlpha(color, 0.5);
            circle->SetFillStyle(0);
            circle->SetLineWidth(2);
            circle->SetLineStyle(1);
            circle->SetNoEdges(1);
            circle->Draw("same");
            // TEllipse *circle2 = new TEllipse(cx, cy, rad, 0, 0, 360);
            // circle2->SetLineColorAlpha(kGray, 0.2);
            // circle2->SetFillStyle(0);
            // circle2->SetLineWidth(2);
            // circle2->SetLineStyle(1);
            // circle2->SetNoEdges(1);
            // circle2->Draw("same");
        }
        else if (mode == 1)
        {
            TEllipse *circle_truth = new TEllipse(cx, cy, rad, 0, phi_min, phi_max);
            circle_truth->SetLineColorAlpha(color, 0.5);
            circle_truth->SetFillStyle(0);
            circle_truth->SetLineWidth(2);
            circle_truth->SetLineStyle(9);
            circle_truth->SetNoEdges(1);
            circle_truth->Draw("same");
        }
    }

    void draw_trackcurve(int mode = 0, bool does_overlay = false, int color = kBlack, bool dca_range_cut = false, bool is_deleted = false, bool reverse = false)
    {
        vector<int> colorset = {kOrange - 3, kOrange + 3, kPink - 3, kPink + 3, kViolet - 3, kViolet + 3, kAzure - 3, kAzure + 3, kTeal - 3, kTeal + 3, kSpring - 3, kSpring + 3};
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
            g_temp->SetMarkerColor(colorset[color_]);
            g_temp->SetLineColor(colorset[color_]);
            g_temp->SetMarkerStyle(20);
            g_temp->SetMarkerSize(0.8);

            if (mode == 0)
            {
                draw_curve2(0, vtrack[i]->p_reco[0], vtrack[i]->p_reco[1], vtrack[i]->p_reco[2], vtrack[i]->rad, vtrack[i]->cx, vtrack[i]->cy, colorset[color_]);

                cout << "p1 : " << vtrack[i]->p1.x() << "  " << vtrack[i]->p1.y() << endl;

                g_temp->SetPoint(0, vtrack[i]->dca_mean[0], vtrack[i]->dca_mean[1]);
                g_temp->SetPoint(1, vtrack[i]->p1.x(), vtrack[i]->p1.y());
                g_temp->SetPoint(2, vtrack[i]->p2.x(), vtrack[i]->p2.y());
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
                draw_curve2(1, vtruth[i]->m_truthpx, vtruth[i]->m_truthpy, vtruth[i]->m_truthpz, vtruth[i]->m_rad, vtruth[i]->center[0], vtruth[i]->center[1], color);

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
                vtruth[i]->truth_xy->SetLineColorAlpha(color, 0.5);
                vtruth[i]->truth_xy->SetLineStyle(9);
                vtruth[i]->truth_xy->SetLineWidth(2);

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
                vtruth[i]->truth_rz->SetLineColorAlpha(color, 0.5);
                vtruth[i]->truth_rz->SetLineStyle(9);
                vtruth[i]->truth_rz->SetLineWidth(2);

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

            if (mode == 0)
            {
                g_temp->SetPoint(0, vclus[i].x, vclus[i].y);
            }
            else if (mode == 1)
            {
                g_temp->SetPoint(0, vclus[i].z, vclus[i].r);
            }
            g_temp->Draw("psame");
        }
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

bool dotracking(clustEvent &vCluster, double angl)
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
            // TVector3 beamspot = {vCluster.dca_mean[0], vCluster.dca_mean[1], 0};
            // TVector3 beamspot = {0, 0, 0};

            TVector3 p1 = c1.pos - beamspot;
            TVector3 p2 = c2.pos - beamspot;

            // cluster posision + chip width
            TVector3 p1_left(p1.x(), p1.y(), p1.z() - c1.chip_width / 2);  // in_left
            TVector3 p1_right(p1.x(), p1.y(), p1.z() + c1.chip_width / 2); // in_right
            TVector3 p2_left(p2.x(), p2.y(), p2.z() - c2.chip_width / 2);  // out_left
            TVector3 p2_right(p2.x(), p2.y(), p2.z() + c2.chip_width / 2); // out_right
            // cout << p2_left.z() << "   " << p2.z() << "   " << p2_right.z() << endl;

            double p1_phi = atan2(p1.y(), p1.x());
            double p2_phi = atan2(p2.y(), p2.x());
            double d_phi = p2_phi - p1_phi;
            d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

            double p1_r = sqrt(p1.x() * p1.x() + p1.y() * p1.y());
            double p2_r = sqrt(p2.x() * p2.x() + p2.y() * p2.y());
            double p1_theta = atan2(p1_r, p1.z());
            double p2_theta = atan2(p2_r, p2.z());
            double d_theta = p2_theta - p1_theta;
            // if (n_dotracking == 1)
            h_dphi_nocut->Fill(p1_phi, d_phi);

            // if (fabs(d_phi) > 0.01)
            if (fabs(d_phi) > angl)
                continue;
            h_dphi_cut->Fill(p1_phi, d_phi);

            TVector3 u = p2 - p1;
            TVector3 u_l = p2_right - p1_left;
            TVector3 u_r = p2_left - p1_right;
            double unorm = sqrt(u.x() * u.x() + u.y() * u.y());
            // unit vector in 2D
            double ux = u.x() / unorm;
            double uy = u.y() / unorm;
            double uz = u.z() / unorm; // same normalization factor(2D) is multiplied
            double uz_l = u_l.z() / unorm;
            double uz_r = u_r.z() / unorm;

            TVector3 p0 = beamspot - p1;

            double dca_p0 = p0.x() * uy - p0.y() * ux; // cross product of p0 x u
            double len_p0 = p0.x() * ux + p0.y() * uy; // dot product of p0 . u

            // beam center in X-Y plane
            double vx = len_p0 * ux + p1.x();
            double vy = len_p0 * uy + p1.y();
            double vz = len_p0 * uz + p1.z();
            double vz_l = len_p0 * uz_l + p1_left.z();
            double vz_r = len_p0 * uz_r + p1_right.z();

            track *tr = new track();
            tr->phi_in = p1_phi;
            tr->phi_out = p2_phi;
            tr->theta_in = c1.gettheta_clus();
            tr->theta_out = c2.gettheta_clus();
            tr->d_phi = d_phi;
            tr->d_theta = d_theta;
            tr->charge = dca_p0 / fabs(dca_p0);

            // track slope & intercept
            tr->ladder1 = (c1.layer == 0) ? (2 * c1.ladder + 1) : (2 * c1.ladder);
            tr->ladder2 = (c2.layer == 2) ? (24 + 2 * c2.ladder + 1) : (24 + 2 * c2.ladder);

            tr->dca[0] = vx;
            tr->dca[1] = vy;
            tr->dca[2] = vz;
            tr->dca[3] = vz_l;
            tr->dca[4] = vz_r;
            // cout << tr->dca[3] << "   " << tr->dca[2] << "   " << tr->dca[4] << endl;

            tr->p1 = c1.pos;
            tr->p2 = c2.pos;

            tr->dca_2d = dca_p0;
            vCluster.vtrack.push_back(tr);
        }
    }

    return true;
}

double calc_Peak(TH1F *h)
{
    int binN_max = h->GetMaximumBin();                 // getting the highest bin number
    double maxbincontent = h->GetBinContent(binN_max); // getting content of the highest bin
    vector<double> maxbins;
    int count = 1;

    for (int i = binN_max; i <= h->GetNbinsX(); i++)
    {
        // if the contents of the next bin are same as the highest bin's, add to "maxbins"
        maxbins.push_back(i);
        if (h->GetBinContent(i) == h->GetBinContent(i + 1) && h->GetBinContent(i) == maxbincontent)
        {
            maxbins.push_back(i + 1);
            count++;
        }
        else
            break;
    }

    double minbinValue = h->GetXaxis()->GetBinLowEdge(*std::min_element(maxbins.begin(), maxbins.end()));
    double maxbinValue = h->GetXaxis()->GetBinUpEdge(*std::max_element(maxbins.begin(), maxbins.end()));

    double peak = -999.9;
    peak = (minbinValue + maxbinValue) / 2; // calculating peak value
    return peak;
}

double calc_Mean(vector<double> v)
{
    double sum = accumulate(v.begin(), v.end(), 0.0);
    double mean = -999.9;
    mean = sum / v.size();
    // cout << "mean : " << mean << endl;
    return mean;
}

double calu_SgmMean(TH1F *h, vector<double> v, int w)
{
    double mean = h->GetMean();
    double sigma = h->GetStdDev();

    double min = mean - w * sigma;
    double max = mean + w * sigma;

    vector<double> used_value;

    for (int itrk = 0; itrk < v.size(); itrk++)
    {
        if (v[itrk] >= min && v[itrk] <= max)
            used_value.push_back(v[itrk]);
    }

    double sum = accumulate(used_value.begin(), used_value.end(), 0.0);
    double sgmmean = -999.9;
    sgmmean = sum / used_value.size();
    // cout << Form("%dsgm mean : ", w) << sgmmean << endl;

    return sgmmean;
}

vector<double> calc_ErrRange(double minRange, double maxRange, double center, bool useSqrt12 = false)
{
    double step = 0.05;
    vector<double> values;

    if (useSqrt12)
    {
        double rangeWidth = maxRange - minRange;
        double newWidth = rangeWidth / sqrt(12);

        minRange = center - newWidth / 2.0;
        maxRange = center + newWidth / 2.0;
        // cout << "--- Sqrt12 ---" << endl;
    }
    // else
    //     cout << "--- full ---" << endl;

    for (double value = minRange; value <= maxRange + step; value += step)
    {
        values.push_back(value);
        // hist->Fill(value);
        // cout << value << endl;
    }

    return values;
}

TH1F *fill_ErrRange(vector<double> values)
{
    TH1F *hist = new TH1F("hist", "hist", 8000, -200, 200);
    for (int nbin = 0; nbin < values.size(); nbin++)
    {
        hist->Fill(values[nbin]);
        // cout << value << endl;
    }
    return hist;
}

void tracking_and_zvtx(bool debug = false, int run_nu = 1, bool sr = false, int BCO = 0)
{
    int sigma = 3;
    int nloop = 100;
    bool mag_on = false;
    bool geant = false;
    double ang = 0.04;

    if (run_nu == 1)
    {
        geant = true;
        double ang = 0.02;
    }

    // bool does_reverse = false;

    int method = 12;
    int xsize = 200;
    int binw = 1;

    string zv_tit[12] = {
        "Peak",
        "Mean",
        "3sigmaMean",
        "1sigmaMean",
        "Peak_error",
        "Mean_error",
        "3sigmaMean_error",
        "1sigmaMean_error",
        "Peak_error_sq12",
        "Mean_error_sq12",
        "3sigmaMean_error_sq12",
        "1sigmaMean_error_sq12",
    };
    // does_reverse = true;

    string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/inttana_%d.root", run_nu);
    // string fname = Form("/Users/ikemotomahiro/SDCC/datamacro/inttana_%d.root", run_nu);

    // string fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/readdst/AnaTutorial_inttana_%d.root", run_nu);
    if (mag_on)
        fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/readdst/AnaTutorial_inttana_%d_mag.root", run_nu);

    if (sr)
        // fname = Form("/Users/ikemotomahiro/SDCC/datamacro/streaming_readout/ana/inttana_%d_%d.root", run_nu, BCO);
        fname = Form("/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/streaming_readout/ana/inttana_%d_%d.root", run_nu, BCO);

    if (geant)
    {
        if (mag_on)
        {
            // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_1K.root";
            fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_zvtxwidth10_ptmin0.root";
            // fname = "/sphenix/tg/tg01/commissioning/INTT/work/tsujibata/tracking/F4AInttRead/macro/macros/detectors/sPHENIX/AnaTutorial_inttana_geant_10K_bon_lowpt5_minus.root";
        }

        else
            fname = "/sphenix/tg/tg01/commissioning/INTT/work/mahiro/datamacro/inttana_sim_nomg.root";
        // fname = "/Users/ikemotomahiro/SDCC/zvtx/result/zvtx_sim_nomg.root";
    }

    TFile *f = TFile::Open(fname.c_str());
    cout << "opened file : " << fname << endl;

    // string dir_out = "result_tracking";
    TString fname_out = fname.substr(fname.find_last_of("/"), fname.size());
    // fname_out.ReplaceAll("AnaTutorial_inttana", "tracking");
    fname_out.ReplaceAll("inttana", "zvtx");
    // fname_out.ReplaceAll(".root", Form("_ang%f.root", ang));

    // if (does_reverse)
    //     fname_out.ReplaceAll(".root", "_reverse.root");
    if (debug)
        fname_out.ReplaceAll(".root", "_debug.root");

    // fname_out.ReplaceAll(".root", "_test.root");

    fname_out = "result_with_tracking" + fname_out;
    cout << "out put file : " << fname_out << endl;

    TH1F *h_dcaz_one_ = new TH1F("h_dcaz_one_", "", 100, -200, 200);

    TFile *froot = new TFile(fname_out, "recreate");
    TH1F *h_nclus = new TH1F("h_nclus", "", 100, 0, 100);

    TTree *temp_tree_ = new TTree("temp_tree_", "temp_tree_");
    // TTree *clus_tree = new TTree("clus_tree", "clus_tree"); // with tracking
    TTree *clus_tree_ = new TTree("clus_tree_", "clus_tree_"); // without tracking
    TTree *truth_tree = new TTree("truth_tree", "truth_tree");
    // TTree *track_tree = new TTree("track_tree", "track_tree"); // with tracking
    TTree *track_tree_ = new TTree("track_tree_", "track_tree_"); // without

    h_dphi_nocut = new TH2F("h_dphi_nocut", "d_phi : phi_in;phi_in;d_phi", 200, -3.2, 3.2, 200, -0.1, 0.1);
    h_dphi_cut = new TH2F("h_dphi_cut", "d_phi : phi_in;phi_in;d_phi", 200, -3.2, 3.2, 200, -0.1, 0.1);

    TH1F *h_dcaz_sigma_one = new TH1F("h_dcaz_sigma_one", "h_dcaz_sigma_one;dcaz sigma of one event;entries", 100, 0, 15);
    TH1D *h_xvtx = new TH1D("h_xvtx", "h_xvtx", 100, -0.01, 0.01);
    TH1D *h_yvtx = new TH1D("h_yvtx", "h_yvtx", 100, -0.01, 0.01);
    // TH1D *h_zvtx = new TH1D("h_zvtx", "h_zvtx", 100, -20, 20);
    TH2D *h_zr[2];

    TH1F *h_dcaz_ = new TH1F("h_dcaz_", "dcaz distribution", 200, -200, 200);
    TH1F *h_dcaz_err = new TH1F("h_dcaz_err", "dcaz with all range distribution", 8000, -200, 200);
    TH1F *h_dcaz_err_sq12 = new TH1F("h_dcaz_err_sq12", "dcaz with error range distribution", 8000, -200, 200);
    TH1F *h_zvtx[method];
    TH1F *h_zvtx_truth = new TH1F("h_zvtx_truth", "truth zvtx", xsize * 2 / binw, -xsize, xsize);
    TH1F *h_zvtx_CW = new TH1F("h_zvtx_CW", "zvtx by CW", xsize * 2 / binw, -xsize, xsize);

    for (int i = 0; i < 2; i++)
        h_zr[i] = new TH2D(Form("h_zr_%d", i), "h_zr;z;r", 100, -20, 20, 100, -10, 10);

    for (int l = 0; l < method; l++)
    {
        string histName = "hist_" + zv_tit[l];
        string histTitle = zv_tit[l];
        h_zvtx[l] = new TH1F(histName.c_str(), histTitle.c_str(), xsize * 2 / binw, -xsize, xsize);
    }
    c = new TCanvas(fname_out.ReplaceAll(".root", ".pdf"), "c", 1200, 1200);
    c->Divide(2, 2);
    c->cd(1);
    c->Print(((string)c->GetName() + "[").c_str());

    int evt_temp_;
    vector<double> d_phi_;
    vector<double> d_theta_;
    vector<double> phi_in_;
    vector<double> dca_x_;
    vector<double> dca_y_;
    vector<double> dcaz_;
    vector<double> dcaz_l;
    vector<double> dcaz_r;
    vector<double> dcaz_err;
    vector<double> dcaz_err_sq12;
    vector<double> dca_2d_;
    double zvtx_one_;
    double zvtx_sigma_one_;
    int evt_all;
    int ntrack_all;
    vector<int> ladnum_in;
    vector<int> ladnum_out;
    double zvtx_CW;
    double truthzvtx;
    double zv_peak;
    double zv_mean;
    double zv_mean_3sgm;
    double zv_mean_1sgm;
    double zv_peak_err;
    double zv_mean_err;
    double zv_mean_3sgm_err;
    double zv_mean_1sgm_err;
    double zv_peak_err_sq12;
    double zv_mean_err_sq12;
    double zv_mean_3sgm_err_sq12;
    double zv_mean_1sgm_err_sq12;

    temp_tree_->Branch("evt_temp_", &evt_temp_, "evt_temp_/I");
    temp_tree_->Branch("d_phi_", &d_phi_);
    temp_tree_->Branch("d_theta_", &d_theta_);
    temp_tree_->Branch("phi_in_", &phi_in_);
    temp_tree_->Branch("dca_x_", &dca_x_);
    temp_tree_->Branch("dca_y_", &dca_y_);
    temp_tree_->Branch("dcaz_", &dcaz_);
    temp_tree_->Branch("dcaz_l", &dcaz_l);
    temp_tree_->Branch("dcaz_r", &dcaz_r);
    temp_tree_->Branch("dcaz_err", &dcaz_err);
    temp_tree_->Branch("dcaz_err_sq12", &dcaz_err_sq12);
    temp_tree_->Branch("dca_2d_", &dca_2d_);
    temp_tree_->Branch("zvtx_one_", &zvtx_one_, "zvtx_one_/D");
    temp_tree_->Branch("zvtx_sigma_one_", &zvtx_sigma_one_, "zvtx_sigma_one_/D");
    temp_tree_->Branch("evt_all", &evt_all, "evt_all/I");
    temp_tree_->Branch("ntrack_all", &ntrack_all, "ntrack_all/I");
    temp_tree_->Branch("ladnum_in", &ladnum_in);
    temp_tree_->Branch("ladnum_out", &ladnum_out);
    temp_tree_->Branch("zvtx_CW", &zvtx_CW, "zvtx_CW/D");
    temp_tree_->Branch("truthzvtx", &truthzvtx, "truthzvtx/D");
    temp_tree_->Branch("zv_peak", &zv_peak, "zv_peak/D");
    temp_tree_->Branch("zv_mean", &zv_mean, "zv_mean/D");
    temp_tree_->Branch("zv_mean_3sgm", &zv_mean_3sgm, "zv_mean_3sgm/D");
    temp_tree_->Branch("zv_mean_1sgm", &zv_mean_1sgm, "zv_mean_1sgm/D");
    temp_tree_->Branch("zv_peak_err", &zv_peak_err, "zv_peal_err/D");
    temp_tree_->Branch("zv_mean_err", &zv_mean_err, "zv_mean_err/D");
    temp_tree_->Branch("zv_mean_3sgm_err", &zv_mean_3sgm_err, "zv_mean_3sgm_err/D");
    temp_tree_->Branch("zv_mean_1sgm_err", &zv_mean_1sgm_err, "zv_mean_1sgm_err/D");
    temp_tree_->Branch("zv_peak_err_sq12", &zv_peak_err_sq12, "zv_peak_err_sq12/D");
    temp_tree_->Branch("zv_mean_err_sq12", &zv_mean_err_sq12, "zv_mean_err_sq12/D");
    temp_tree_->Branch("zv_mean_3sgm_err_sq12", &zv_mean_3sgm_err_sq12, "zv_mean_3sgm_err_sq12/D");
    temp_tree_->Branch("zv_mean_1sgm_err_sq12", &zv_mean_1sgm_err_sq12, "zv_mean_1sgm_err_sq12/D");

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

    // cluster tree with tracking
    // clus_tree->Branch("evt_clus", &evt_clus, "evt_clus/I");
    // clus_tree->Branch("x_in", &x_in);
    // clus_tree->Branch("y_in", &y_in);
    // clus_tree->Branch("z_in", &z_in);
    // clus_tree->Branch("r_in", &r_in);
    // clus_tree->Branch("phi_in", &phi_in);
    // clus_tree->Branch("theta_in", &theta_in);
    // clus_tree->Branch("x_out", &x_out);
    // clus_tree->Branch("y_out", &y_out);
    // clus_tree->Branch("z_out", &z_out);
    // clus_tree->Branch("r_out", &r_out);
    // clus_tree->Branch("phi_out", &phi_out);
    // clus_tree->Branch("theta_out", &theta_out);

    // cluster tree without tracking
    clus_tree_->Branch("evt_clus", &evt_clus, "evt_clus/I");
    clus_tree_->Branch("x_in", &x_in);
    clus_tree_->Branch("y_in", &y_in);
    clus_tree_->Branch("z_in", &z_in);
    clus_tree_->Branch("r_in", &r_in);
    clus_tree_->Branch("phi_in", &phi_in);
    clus_tree_->Branch("theta_in", &theta_in);
    clus_tree_->Branch("x_out", &x_out);
    clus_tree_->Branch("y_out", &y_out);
    clus_tree_->Branch("z_out", &z_out);
    clus_tree_->Branch("r_out", &r_out);
    clus_tree_->Branch("phi_out", &phi_out);
    clus_tree_->Branch("theta_out", &theta_out);

    int evt_track;
    int ntrack = 0;
    vector<double> slope_rz;
    vector<double> slope_xy;
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

    // track tree with tracking
    /*track_tree->Branch("evt_track", &evt_track, "evt_track/I");
    track_tree->Branch("ntrack", &ntrack, "ntrack/I");
    track_tree->Branch("phi_tracklets", &phi_tracklets);
    track_tree->Branch("slope_rz", &slope_rz);
    track_tree->Branch("slope_xy", &slope_xy);
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
    track_tree->Branch("z_vertex", &z_vertex, "z_vertex/D");*/

    // track tree without tracking
    track_tree_->Branch("evt_track", &evt_track, "evt_track/I");
    track_tree_->Branch("ntrack", &ntrack, "ntrack/I");
    track_tree_->Branch("phi_tracklets", &phi_tracklets);
    track_tree_->Branch("slope_rz", &slope_rz);
    track_tree_->Branch("slope_xy", &slope_xy);
    track_tree_->Branch("theta_tracklets", &theta_tracklets);
    track_tree_->Branch("phi_track", &phi_track);
    track_tree_->Branch("theta_track", &theta_track);
    track_tree_->Branch("z_tracklets_out", &z_tracklets_out);
    track_tree_->Branch("dca_2d", &dca_2d);
    track_tree_->Branch("dca_z", &dca_z);
    track_tree_->Branch("is_deleted", &is_deleted);
    track_tree_->Branch("dca_range_out", &dca_range_out);
    track_tree_->Branch("dcaz_range_out", &dcaz_range_out);
    track_tree_->Branch("dca2d_range_out", &dca2d_range_out);
    track_tree_->Branch("pT", &pT);
    track_tree_->Branch("px", &px);
    track_tree_->Branch("py", &py);
    track_tree_->Branch("pz", &pz);
    track_tree_->Branch("px_truth", &px_truth);
    track_tree_->Branch("py_truth", &py_truth);
    track_tree_->Branch("pz_truth", &pz_truth);
    track_tree_->Branch("pT_truth", &pT_truth);
    track_tree_->Branch("charge", &charge);
    track_tree_->Branch("rad", &rad);
    track_tree_->Branch("x_vertex", &x_vertex, "x_vertex/D");
    track_tree_->Branch("y_vertex", &y_vertex, "y_vertex/D");
    track_tree_->Branch("z_vertex", &z_vertex, "z_vertex/D");

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
    // double truthzvtx;
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
    // truth_tree->Branch("truthzvtx", &truthzvtx);
    truth_tree->Branch("truthzout", &truthzout);
    truth_tree->Branch("truthpid", &truthpid);
    truth_tree->Branch("status", &status);

    // from input file
    TNtuple *ntp_clus = (TNtuple *)f->Get("ntp_clus");
    TNtuple *ntp_evt = (TNtuple *)f->Get("ntp_evt");
    // TTree *hepmctree = (TTree *)f->Get("hepmctree");
    // bool no_mc = (hepmctree->GetEntries() == 0);

    Float_t nclus, nclus2, bco_full, evt, size, adc, x, y, z, lay, lad, sen;

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
    // ntp_clus->SetBranchAddress("x_vtx", &x_vtx);
    // ntp_clus->SetBranchAddress("y_vtx", &y_vtx);
    // ntp_clus->SetBranchAddress("zv", &zv);

    Float_t zvsim, zv;
    ntp_evt->SetBranchAddress("zv", &zv);
    ntp_evt->SetBranchAddress("zvsim", &zvsim);

    // double m_truthpx, m_truthpy, m_truthpz, m_truthpt, m_trutheta, m_truththeta, m_truthphi, m_xvtx, m_yvtx, m_zvtx;
    // int m_evt, m_status, m_truthpid;

    // hepmctree->SetBranchAddress("m_evt", &m_evt);
    // hepmctree->SetBranchAddress("m_status", &m_status);
    // hepmctree->SetBranchAddress("m_truthpx", &m_truthpx);
    // hepmctree->SetBranchAddress("m_truthpy", &m_truthpy);
    // hepmctree->SetBranchAddress("m_truthpz", &m_truthpz);
    // hepmctree->SetBranchAddress("m_truthpt", &m_truthpt);
    // hepmctree->SetBranchAddress("m_trutheta", &m_trutheta);
    // hepmctree->SetBranchAddress("m_truthpid", &m_truthpid);
    // hepmctree->SetBranchAddress("m_truththeta", &m_truththeta);
    // hepmctree->SetBranchAddress("m_truthphi", &m_truthphi);
    // hepmctree->SetBranchAddress("m_xvtx", &m_xvtx);
    // hepmctree->SetBranchAddress("m_yvtx", &m_yvtx);
    // hepmctree->SetBranchAddress("m_zvtx", &m_zvtx);

    int sum_event = 0;
    int ntrack_ = 0;
    for (int icls = 0; icls < ntp_clus->GetEntries(); icls++)
    {
        cout << Form("  -----  event %d {event gene.}  -----  ", sum_event) << endl;

        clustEvent event{};
        cluster clust{};

        ntp_clus->GetEntry(icls); // first cluster in one event
        ntp_evt->GetEntry(sum_event);
        clust.set(evt, 0, x, y, z, adc, size, lay, lad, sen);
        event.vclus.push_back(clust);

        h_dcaz_->Reset();
        h_dcaz_err->Reset();
        h_dcaz_err_sq12->Reset();

        for (int j = 1; j < nclus; j++) // second~ cluster in one event
        {
            ntp_clus->GetEntry(++icls);
            cluster clust2{};
            clust2.set(evt, 0, x, y, z, adc, size, lay, lad, sen);
            event.vclus.push_back(clust2);
        }

        if (!(geant))
        {
            event.dca_mean[0] = -0.019;
            event.dca_mean[1] = 0.198;
        }

        dotracking(event, ang);
        ntrack_ = event.vtrack.size();
        ntrack_all = ntrack_;
        h_dcaz_one_->Reset();
        if (ntrack_all <= 2)
            continue;
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
            dcaz_.push_back(event.vtrack[itrk]->dca[2]);
            dcaz_l.push_back(event.vtrack[itrk]->dca[3]);
            dcaz_r.push_back(event.vtrack[itrk]->dca[4]);
            dca_2d_.push_back(event.vtrack[itrk]->dca_2d);
            d_phi_.push_back(event.vtrack[itrk]->d_phi);
            d_theta_.push_back(event.vtrack[itrk]->d_theta);
            phi_in_.push_back(event.vtrack[itrk]->phi_in);
            ladnum_in.push_back(event.vtrack[itrk]->ladder1);
            ladnum_out.push_back(event.vtrack[itrk]->ladder2);

            h_dcaz_->Fill(dcaz_[itrk]);

            vector<double> errRange = calc_ErrRange(dcaz_l[itrk], dcaz_r[itrk], dcaz_[itrk], false);
            dcaz_err.insert(dcaz_err.end(), errRange.begin(), errRange.end());
            TH1F *Hist_err = fill_ErrRange(dcaz_err);
            h_dcaz_err->Add(Hist_err);
            delete Hist_err;
            errRange.clear();

            // cout << "left : " << dcaz_l[j] << "  dcaz : " << dcaz[j] << "  right : " << dcaz_r[j] << endl;
            vector<double> errRangeSq12 = calc_ErrRange(dcaz_l[itrk], dcaz_r[itrk], dcaz_[itrk], true);
            dcaz_err_sq12.insert(dcaz_err_sq12.end(), errRangeSq12.begin(), errRangeSq12.end());
            TH1F *Hist_err_sq12 = fill_ErrRange(dcaz_err_sq12);
            h_dcaz_err_sq12->Add(Hist_err_sq12);
            delete Hist_err_sq12;
            errRangeSq12.clear();
        }

        zv_peak = calc_Peak(h_dcaz_);
        zv_mean = calc_Mean(dcaz_);
        zv_mean_3sgm = calu_SgmMean(h_dcaz_, dcaz_, 3);
        zv_mean_1sgm = calu_SgmMean(h_dcaz_, dcaz_, 1);

        zv_peak_err = calc_Peak(h_dcaz_err);
        zv_mean_err = calc_Mean(dcaz_err);
        zv_mean_3sgm_err = calu_SgmMean(h_dcaz_err, dcaz_err, 3);
        zv_mean_1sgm_err = calu_SgmMean(h_dcaz_err, dcaz_err, 1);

        zv_peak_err_sq12 = calc_Peak(h_dcaz_err_sq12);
        zv_mean_err_sq12 = calc_Mean(dcaz_err_sq12);
        zv_mean_3sgm_err_sq12 = calu_SgmMean(h_dcaz_err_sq12, dcaz_err_sq12, 3);
        zv_mean_1sgm_err_sq12 = calu_SgmMean(h_dcaz_err_sq12, dcaz_err_sq12, 1);

        double rzv[12] = {zv_peak, zv_mean, zv_mean_3sgm, zv_mean_1sgm, zv_peak_err, zv_mean_err, zv_mean_3sgm_err, zv_mean_1sgm_err, zv_peak_err_sq12, zv_mean_err_sq12, zv_mean_3sgm_err_sq12, zv_mean_1sgm_err_sq12};

        for (int j = 0; j < method; j++)
        {
            h_zvtx[j]->Fill(rzv[j]);
            // if (geant)
            // h_zvtx_reso[j]->Fill(rzv[j] - truthzvtx);
        }

        if (geant)
        {
            truthzvtx = zvsim;
            h_zvtx_truth->Fill(truthzvtx);
            // cout << "truth zvtx : " << truthzvtx << endl;
        }

        zvtx_CW = zv;
        h_zvtx_CW->Fill(zv);

        // cluster tree without tracking
        for (unsigned int i = 0; i < event.vclus.size(); i++)
        {
            if (event.vclus[i].layer < 2)
            {
                x_in.push_back(event.vclus[i].x);
                y_in.push_back(event.vclus[i].y);
                z_in.push_back(event.vclus[i].z);
                r_in.push_back(event.vclus[i].r);
                phi_in.push_back(event.vclus[i].getphi_clus());
                theta_in.push_back(event.vclus[i].gettheta_clus());
            }
            if (1 < event.vclus[i].layer)
            {
                x_out.push_back(event.vclus[i].x);
                y_out.push_back(event.vclus[i].y);
                z_out.push_back(event.vclus[i].z);
                r_out.push_back(event.vclus[i].r);
                phi_out.push_back(event.vclus[i].getphi_clus());
                theta_out.push_back(event.vclus[i].gettheta_clus());
            }
        }

        // track tree without tracking
        // ntrack = 0;
        // for (int itrk = 0; itrk < ntrack_; itrk++)
        // {
        //     slope_rz.push_back(event.vtrack[itrk]->track_rz->GetParameter(1));
        //     slope_xy.push_back(event.vtrack[itrk]->track_xy->GetParameter(1));
        //     phi_tracklets.push_back(event.vtrack[itrk]->getphi_tracklet());
        //     theta_tracklets.push_back(event.vtrack[itrk]->gettheta_tracklet());
        //     phi_track.push_back(event.vtrack[itrk]->getphi());
        //     theta_track.push_back(event.vtrack[itrk]->gettheta());
        //     dca_z.push_back(event.vtrack[itrk]->dca[2]);
        //     dca_2d.push_back(event.vtrack[itrk]->dca_2d);
        //     z_tracklets_out.push_back(event.vtrack[itrk]->p2.z());
        //     pT.push_back(event.vtrack[itrk]->pT);
        //     px.push_back(event.vtrack[itrk]->p_reco[0]);
        //     py.push_back(event.vtrack[itrk]->p_reco[1]);
        //     pz.push_back(event.vtrack[itrk]->p_reco[2]);
        //     rad.push_back(event.vtrack[itrk]->rad);
        //     charge.push_back(event.vtrack[itrk]->charge);
        //     // if (!(no_mc))
        //     // {
        //     //     pT_truth.push_back(event.vtruth[0]->m_truthpt);
        //     //     px_truth.push_back(event.vtruth[0]->m_truthpx);
        //     //     py_truth.push_back(event.vtruth[0]->m_truthpy);
        //     //     pz_truth.push_back(event.vtruth[0]->m_truthpz);
        //     // }

        //     is_deleted.push_back(event.vtrack[itrk]->is_deleted);
        //     dca2d_range_out.push_back(event.vtrack[itrk]->dca2d_range_out);
        //     dcaz_range_out.push_back(event.vtrack[itrk]->dcaz_range_out);
        //     dca_range_out.push_back(event.vtrack[itrk]->dca_range_out);

        //     x_vertex = event.vtrack[itrk]->dca_mean[0];
        //     y_vertex = event.vtrack[itrk]->dca_mean[1];
        //     z_vertex = event.vtrack[itrk]->dca_mean[2];

        //     if (event.vtrack[itrk]->is_deleted == true)
        //         continue;
        //     if (event.vtrack[itrk]->dca_range_out == true)
        //         continue;
        //     ntrack++;
        //     h_xvtx->Fill(x_vertex);
        //     h_yvtx->Fill(y_vertex);
        //     h_zvtx->Fill(z_vertex);
        // }

        truth_tree->Fill();

        // if (does_reverse)
        // {
        //     reverse(dca_x_);
        //     reverse(dca_y_);
        //     reverse(dca_z_);
        //     reverse(dca_z_l);
        //     reverse(dca_z_r);
        //     reverse(dca_2d_);
        //     reverse(d_phi_);
        //     reverse(d_theta_);
        //     reverse(phi_in_);
        // }
        temp_tree_->Fill();
        erase(dca_x_);
        erase(dca_y_);
        erase(dcaz_);
        erase(dcaz_l);
        erase(dcaz_r);
        erase(dca_2d_);
        erase(d_phi_);
        erase(d_theta_);
        erase(phi_in_);
        erase(ladnum_in);
        erase(ladnum_out);
        erase(dcaz_err);
        erase(dcaz_err_sq12);

        // dcaz.clear();
        // dcaz_l.clear();
        // dcaz_r.clear();
        // dcaz_err.clear();
        // dcaz_err_sq12.clear();

        // filling cluster tree without tracking
        clus_tree_->Fill();
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

        // filling track tree without tracking
        track_tree_->Fill();
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

        event.clear();
        sum_event++;
        // cout << sum_event << endl;
        if (debug && sum_event > nloop)
            break;
    }
    evt_all = sum_event;
    // cout << evt_all << endl;

    n_dotracking++;
    TH1F *h_dcax = new TH1F("h_dcax", "h_dcax", 100, -0.4, 0.4);
    temp_tree_->Draw("dca_x_>>h_dcax");
    TH1F *h_dcay = new TH1F("h_dcay", "h_dcay", 100, -0.4, 0.4);
    temp_tree_->Draw("dca_y_>>h_dcay");
    TH1F *h_dcaz = new TH1F("h_dcaz", "h_dcaz;DCA_z[cm]", 60, -150, 150);
    temp_tree_->Draw("dcaz_>>h_dcaz");
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

    /*
        // tracking
        int ihit = 0;
        int itruth = 0;
        int temp_evt = 0;
        for (int ievt = 0; ievt < sum_event; ievt++, ihit++, itruth++)
        {
            temp_tree_->GetEntry(ievt);
            cout << Form("  -----  event %d  -----  ", ievt) << endl;
            ntruth = 0;
            ntruth_cut = 0;

            clustEvent event;
            event.ievt = ievt;
            event.mag_on = mag_on;
            event.run_nu = run_nu;
            // event.bco_full = bco_full;

            if (ihit < ntp_clus->GetEntries())
            {
                ntp_clus->GetEntry(ihit);

                // if (!(no_mc))
                // {
                //     hepmctree->GetEntry(itruth);
                //     while (m_evt != evt)
                //     {
                //         itruth++;
                //         hepmctree->GetEntry(itruth);
                //     }
                //     temp_evt = m_evt;
                // }
                cluster clust{};

                clust.set(evt, 0, x, y, z, adc, size, lay, lad, sen);
                event.vclus.push_back(clust);

                for (int j = 1; j < nclus; j++) // second~ cluster in one event
                {
                    ntp_clus->GetEntry(++ihit);
                    cluster clust2{};

                    clust2.set(evt, 0, x, y, z, adc, size, lay, lad, sen);
                    event.vclus.push_back(clust2);
                }

                // if (!(no_mc))
                // {
                //     while (m_evt == temp_evt)
                //     {
                //         ntruth++;
                //         truth *tru = new truth();
                //         tru->set_truth(m_truthpx, m_truthpy, m_truthpz, m_truthpt, m_status, m_trutheta, m_truthpid, m_truththeta, m_truthphi, m_xvtx, m_yvtx, m_zvtx);
                //         event.vtruth.push_back(tru);
                //         itruth++;
                //         if (itruth == hepmctree->GetEntries())
                //             break;

                //         hepmctree->GetEntry(itruth);
                //     }
                //     itruth--;
                // }
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

            // if (!(no_mc))
            // {
            //     for (int itrt = 0; itrt < ntruth; itrt++)
            //     {
            //         event.vtruth[itrt]->dca_mean[0] = dcax_mean;
            //         event.vtruth[itrt]->dca_mean[1] = dcay_mean;
            //         event.vtruth[itrt]->dca_mean[2] = dcaz_mean_one;

            //         event.vtruth[itrt]->calc_line();
            //         event.vtruth[itrt]->calc_center();
            //     }
            // }
            h_nclus->Fill(event.vclus.size());

            // pushing back to cluster tree with tracking
            // for (int iclus = 0; iclus < event.vclus.size(); iclus++)
            // {
            //     event.vclus[iclus].dca_mean[0] = dcax_mean;
            //     event.vclus[iclus].dca_mean[1] = dcay_mean;
            //     event.vclus[iclus].dca_mean[2] = dcaz_mean_one;

            //     if (event.vclus[iclus].layer < 2)
            //     {
            //         x_in.push_back(event.vclus[iclus].x);
            //         y_in.push_back(event.vclus[iclus].y);
            //         z_in.push_back(event.vclus[iclus].z);
            //         r_in.push_back(event.vclus[iclus].r);
            //         phi_in.push_back(event.vclus[iclus].getphi_clus());
            //         theta_in.push_back(event.vclus[iclus].gettheta_clus());
            //     }
            //     if (1 < event.vclus[iclus].layer)
            //     {
            //         x_out.push_back(event.vclus[iclus].x);
            //         y_out.push_back(event.vclus[iclus].y);
            //         z_out.push_back(event.vclus[iclus].z);
            //         r_out.push_back(event.vclus[iclus].r);
            //         phi_out.push_back(event.vclus[iclus].getphi_clus());
            //         theta_out.push_back(event.vclus[iclus].gettheta_clus());
            //     }
            // }

            // if (does_reverse)
            // {
            //     reverse(x_out);
            //     reverse(y_out);
            //     reverse(z_out);
            //     reverse(r_out);
            //     reverse(phi_out);
            //     reverse(theta_out);

            //     reverse(x_in);
            //     reverse(y_in);
            //     reverse(z_in);
            //     reverse(r_in);
            //     reverse(phi_in);
            //     reverse(theta_in);
            // }

             filling cluster tree with tracking
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

            // pushing back to track tree with tracking
            ntrack = 0;
             for (int itrk = 0; itrk < ntrack_; itrk++)
             {
                 slope_rz.push_back(event.vtrack[itrk]->track_rz->GetParameter(1));
                 slope_xy.push_back(event.vtrack[itrk]->track_xy->GetParameter(1));
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
                 // if (!(no_mc))
                 // {
                 //     pT_truth.push_back(event.vtruth[0]->m_truthpt);
                 //     px_truth.push_back(event.vtruth[0]->m_truthpx);
                 //     py_truth.push_back(event.vtruth[0]->m_truthpy);
                 //     pz_truth.push_back(event.vtruth[0]->m_truthpz);
                 // }

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

            // if (does_reverse)
            // {
            //     reverse(slope_rz);
            //     reverse(slope_xy);
            //     reverse(phi_tracklets);
            //     reverse(theta_tracklets);
            //     reverse(phi_track);
            //     reverse(theta_track);
            //     reverse(dca_z);
            //     reverse(dca_2d);
            //     reverse(is_deleted);
            //     reverse(dca_range_out);
            //     reverse(dca2d_range_out);
            //     reverse(dcaz_range_out);
            //     reverse(z_tracklets_out);
            //     reverse(pT);
            //     reverse(px);
            //     reverse(py);
            //     reverse(pz);
            //     reverse(pT_truth);
            //     reverse(px_truth);
            //     reverse(py_truth);
            //     reverse(pz_truth);
            //     reverse(charge);
            //     reverse(rad);
            // }

            // filling track tree with tracking

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
            // if (fabs(event.vclus.size() - 2 * ntrack) < 5 && debug && 10 < event.vclus.size() && event.vclus.size() < 50  && && geant && ievt < 500)
            if (ievt < 100)
            {
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

                c->cd(1);
                event.draw_frame(0);
                event.draw_intt(0);
                // event.draw_truthline(0, false, TColor::GetColor(232, 85, 98));
                // event.draw_truthcurve(0, false, TColor::GetColor(232, 85, 98));
                // event.draw_tracklets(0, false, kGray + 1, false, false);
                // event.draw_tracklets(0, true, TColor::GetColor(0, 118, 186), true, true);
                event.draw_clusters(0, true, kBlack);
                // event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
                if (mag_on)
                {
                    event.draw_trackcurve(0, true, TColor::GetColor(88, 171, 141), true, true, false);
                    // if (!(no_mc))
                    //     event.draw_truthcurve(0, true, TColor::GetColor(232, 85, 98));
                }
                else
                {
                    event.draw_trackline(0, true, TColor::GetColor(88, 171, 141), true, true, false);
                    // if (!(no_mc))
                    //     event.draw_truthline(0, true, TColor::GetColor(232, 85, 98));
                }
                // event.draw_tracklets(0, true, kGray + 1, true, true);
                c->cd(2);
                event.draw_frame(1);
                event.draw_intt(1);
                // event.draw_tracklets(1, false, kGray + 1, false, false);
                // event.draw_tracklets(1, true, TColor::GetColor(0, 118, 186), true, true);
                event.draw_clusters(1, true, kBlack);
                event.draw_trackline(1, true, TColor::GetColor(88, 171, 141), true, true, false);
                // if (!(no_mc))
                //     event.draw_truthline(1, false, TColor::GetColor(232, 85, 98));
                // event.draw_tracklets(1, true, kGray + 1, true, true);

                c->Print(c->GetName());

                c->cd(1);
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

                c->Print(c->GetName());

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
        }*/

    // c->Print(c->GetName());
    if (geant)
        h_zvtx_truth->Draw();
    else
        h_zvtx_CW->Draw();
    c->Print(c->GetName());

    for (int j = 0; j < method; j++)
    {
        // gStyle->SetOptFit(1111);
        if (j < 4)
            c->cd(j + 1);
        else if (j < 8)
            c->cd(j - 3);
        else if (j < 12)
            c->cd(j - 7);

        // double Std = h_zvtx[j]->GetStdDev();

        // h_zvtx[j]->Fit("gaus", "", "", -(Std * 3), Std * 3);

        h_zvtx[j]->SetXTitle("z_{vtx}^{INTT} [cm]");
        h_zvtx[j]->SetYTitle(Form("Counts / (%dcm)", binw));
        // h_zvtx[j]->SetLineColor(TColor::GetColor(255, 105, 180));
        h_zvtx[j]->SetLineColor(1);
        h_zvtx[j]->Draw();
        if (geant)
        {
            h_zvtx_truth->SetLineColor(TColor::GetColor(50, 205, 50));
            h_zvtx_truth->Draw("Same");
            h_zvtx[j]->Draw("same");
        }
        if (j == 3 || j == 7 || j == 11)
            c->Print(c->GetName());
    }
    c->Print(((string)c->GetName() + "]").c_str());

    froot->Write();
    for (int ihst = 0; ihst < h_.size(); ihst++)
    {
        froot->WriteTObject(h_[ihst], h_[ihst]->GetName());
    }
    froot->Close();
}
