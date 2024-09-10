#pragma once
#include "include_tracking/least_square2.cc"

class truth
{
public:
    double m_truthpx;
    double m_truthpy;
    double m_truthpz;
    double m_truthpt;
    double m_trutheta;
    double m_truththeta;
    double m_truthphi;
    double m_truthz_out;

    double m_xvtx;
    double m_yvtx;
    double m_zvtx;

    int m_truthpid;
    int m_status;

    TF1 *truth_xy = nullptr;
    TF1 *truth_rz = nullptr;

    double dca_mean[3];

    double center[2];
    double m_rad;

    bool is_intt = false;
    bool is_charged = false;
    bool have_cluster = false;
    bool have_track = false;

    void set_truth(double M_truthpx, double M_truthpy, double M_truthpz, double M_truthpt, int M_status, double M_trutheta, int M_truthpid, double M_truththeta, double M_truthphi, double M_xvtx, double M_yvtx, double M_zvtx)
    {
        m_truthpx = M_truthpx;
        m_truthpy = M_truthpy;
        m_truthpz = M_truthpz;
        m_truthpt = M_truthpt;
        m_status = M_status;
        m_trutheta = M_trutheta;
        m_truthpid = M_truthpid;
        m_truthphi = M_truthphi;
        m_truththeta = M_truththeta;
        m_xvtx = M_xvtx;
        m_yvtx = M_yvtx;
        m_zvtx = M_zvtx;
    }

    double getzout()
    {
        double y = m_truthpy / fabs(m_truthpy) * 10;
        // cout << "truth_rz->GetParameter(0) : " << truth_rz->GetParameter(0) << endl;
        // cout<<"y : "<<y<<endl;
        double z_outer = (y - truth_rz->GetParameter(0)) / truth_rz->GetParameter(1);
        // cout << "z_outer : " << z_outer << endl;

        return z_outer;
    }

    double getpointr(int mode)
    {
        double r = 0;
        if (mode == 1) // truth
        {
            r = m_truthpy / fabs(m_truthpy) * m_truthpt;
        }
        else if (mode == 2) // dca_mean
        {
            r = dca_mean[1] / fabs(dca_mean[1]) * sqrt(dca_mean[0] * dca_mean[0] + dca_mean[1] * dca_mean[1]);
        }
        else if (mode == 3) // vertex
        {
            r = /*m_yvtx / fabs(m_yvtx) **/ sqrt(m_xvtx * m_xvtx + m_yvtx * m_yvtx);
        }

        return r;
    }

    void calc_line()
    {
        if (truth_xy != nullptr)
        {
            delete truth_xy;
            truth_xy = nullptr;
        }

        truth_xy = new TF1("truth_xy", "pol1", -1000, 1000);

        truth_xy->SetParameter(1, m_truthpy / m_truthpx);
        truth_xy->SetParameter(0, -m_yvtx / m_truthpx * m_xvtx + m_yvtx);

        if (truth_rz != nullptr)
        {
            delete truth_rz;
            truth_rz = nullptr;
        }

        truth_rz = new TF1("truth_rz", "pol1", -1000, 1000);

        // from the truth vertex
        double slope = getpointr(1) / m_truthpz;
        double intercept = -slope * m_zvtx + getpointr(3);
        // cout << slope << "  " << m_zvtx << "  " << getpointr(3) << endl;

        truth_rz->SetParameter(1, slope);
        truth_rz->SetParameter(0, intercept);

        m_truthz_out = getzout();
    }

    void calc_center()
    {
        m_rad = m_truthpt / (0.3 * 1.4) * 100;
        double sign = m_truthpid / fabs(m_truthpid);
        center[0] = -sign * (m_xvtx + m_rad * (-sin(m_truthphi)));
        center[1] = -sign * (m_yvtx + m_rad * cos(m_truthphi));
    }

    /////////////////////////////////////////

    // vector<double> get_crossline(double x1, double y1, double r1)
    // {
    //     vector<double> cross_co;
    //     // ax + by + c = 0
    //     cross_co.push_back(2 * (cx - x1));                                                           // a
    //     cross_co.push_back(2 * (cy - y1));                                                           // b
    //     cross_co.push_back((x1 + cx) * (x1 - cx) + (y1 + cy) * (y1 - cy) + (rad + r1) * (rad - r1)); // c

    //     return cross_co;
    // }

    // vector<double> get_crosspoint(const vector<double> &V) // ax + by + c = 0
    // {
    //     vector<double> cross;
    //     double a = V[0];
    //     double b = V[1];
    //     double c = V[2];

    //     double d = fabs(a * cx + b * cy + c) / sqrt(a * a + b * b);
    //     cout << "d ; " << d << " " << rad << endl;

    //     double theta = atan2(b, a);

    //     if (d > rad)
    //     {
    //         cout << "d > rad" << endl;
    //     }
    //     else if (d == rad)
    //     {
    //         cout << "d == rad" << endl;

    //         if (a * cx + b * cy + c > 0)
    //             theta += M_PI;
    //         cross.push_back(rad * cos(theta) + cx);
    //         cross.push_back(rad * sin(theta) + cy);
    //     }
    //     else
    //     {
    //         cout << "d < rad" << endl;
    //         double alpha, beta, phi;
    //         phi = acos(d / rad);
    //         alpha = theta - phi;
    //         beta = theta + phi;
    //         if (a * cx + b * cy + c > 0)
    //         {
    //             alpha += M_PI;
    //             beta += M_PI;
    //         }
    //         // double temp_cross[2][2];
    //         vector<vector<double>> temp_cross = {{rad * cos(alpha) + cx, rad * cos(beta) + cx}, {rad * sin(alpha) + cy, rad * sin(beta) + cy}};
    //         // for (unsigned int j = 0; j < temp_cross.at(0).size(); j++)
    //         // {
    //         //     cout << "temp_cross : ";
    //         //     for (unsigned int i = 0; i < temp_cross.size(); i++)
    //         //         cout << temp_cross.at(j).at(i) << "  ";
    //         //     cout << endl;
    //         // }
    //         cross = get_closestpoint(temp_cross);
    //     }
    //     // cout << "cross size : " << cross.size() << endl;
    //     // for (int i = 0; i < cross.size(); i++)
    //     //     cout << cross[i] << endl;
    //     return cross;
    // }

    // vector<double> get_closestpoint(const vector<vector<double>> VV)
    // {
    //     vector<double> closest;
    //     double pre_phi = 999;
    //     double phi_p1 = atan2(p1.y() - dca_mean[1], p1.x() - dca_mean[0]);
    //     for (unsigned int i = 0; i < VV.at(0).size(); i++)
    //     {
    //         cout << "dca_mean : " << dca_mean[0] << "  " << dca_mean[1] << endl;
    //         cout << "VV : " << VV[0][i] << "  " << VV[1][i] << endl;
    //         cout << "pVV : " << VV[0][i] - dca_mean[0] << "  " << VV[1][i] - dca_mean[1] << endl;
    //         cout << "p1  : " << p1.x() - dca_mean[0] << "  " << p1.y() - dca_mean[1] << endl;
    //         // cout << VV.at(i).at(0) << endl;
    //         double dot = (VV[0][i] - dca_mean[0]) * (p1.x() - dca_mean[0]) + (VV[1][i] - dca_mean[1]) * (p1.y() - dca_mean[1]);
    //         double temp_phi = atan2(VV[1][i] - dca_mean[1], VV[0][i] - dca_mean[0]);
    //         cout << "dot : " << dot << endl;
    //         if (dot > 0)
    //         {
    //             if (fabs(temp_phi - phi_p1) < fabs(pre_phi - phi_p1))
    //             {
    //                 closest.erase(closest.begin(), closest.end());
    //                 cout << "VV size : " << VV.size() << " " << VV.at(0).size() << endl;
    //                 for (int j = 0; j < 2; j++)
    //                     closest.push_back(VV[j][i]);
    //                 pre_phi = temp_phi;
    //             }
    //         }
    //     }
    //     // closest = temp_closest;
    //     // for (unsigned int i = 0; i < VV.at(0).size(); i++)
    //     // {
    //     //     for (int j = 0; j < 2; j++)
    //     //         closest.push_back(VV[j][i]);
    //     // }
    //     // for (unsigned int i = 0; i < closest.size(); i++)
    //     //     cout << closest[i] << endl;

    //     return closest;
    // }

    // vector<double> get_crossframe(int Detect)
    // {
    //     cout << endl;
    //     cout << "calc frame" << endl;
    //     double size;
    //     if (Detect == 0)
    //         size = 15;
    //     else if (Detect == 1)
    //         size = 120;
    //     vector<vector<double>> crossframe(2, vector<double>(0));
    //     for (int j = 0; j < 2; j++)
    //     {
    //         for (int i = 0; i < 2; i++)
    //         {
    //             double a_ = (j == 0) ? 1 : 0;
    //             double b_ = (j == 0) ? 0 : 1;
    //             double c_ = (2 * i - 1) * size;
    //             cout << "kesu : " << a_ << " " << b_ << " " << c_ << endl;
    //             vector<double> co_ = {a_, b_, c_};
    //             vector<double> temp_crossframe = get_crosspoint(co_);
    //             if (temp_crossframe.size() == 2)
    //             {
    //                 cout << "temp_crossframe : " << temp_crossframe[0] << "  " << temp_crossframe[1] << endl;
    //                 crossframe.at(0).push_back(temp_crossframe[0]);
    //                 crossframe.at(1).push_back(temp_crossframe[1]);
    //             }
    //         }
    //     }
    //     cout << "crossframe" << endl;
    //     cout << "size : " << crossframe.size() << "  " << crossframe.at(0).size() << endl;
    //     for (int j = 0; j < crossframe.at(0).size(); j++)
    //     {
    //         for (int i = 0; i < crossframe.size(); i++)
    //         {
    //             cout << crossframe[i][j] << ", ";
    //         }
    //         cout << endl;
    //     }
    //     vector<double> result;
    //     if (crossframe.at(0).size() != 0)
    //         result = get_closestpoint(crossframe);
    //     cout << "closest frame" << endl;
    //     for (int i = 0; i < result.size(); i++)
    //     {
    //         cout << result[i] << ", ";
    //     }
    //     cout << endl;
    //     cout << "calc end" << endl;

    //     return result;
    // }
};
