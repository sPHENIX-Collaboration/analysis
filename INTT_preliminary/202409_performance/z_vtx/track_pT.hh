#pragma once
#include "least_square2.cc"

class track
{
public:
    TF1 *track_xy;
    TF1 *track_rz;
    TF1 *track_rz_inv;
    double phi_in;
    double phi_out;
    double theta_in;
    double theta_out;
    double d_phi;
    double d_theta;
    double dca[5];
    double dca_mean[3];
    TVector3 p1;
    TVector3 p2;
    double dca_2d;
    double phi;
    double theta;
    double phi_tracklet;
    double theta_tracklet;
    double charge;

    int ladder1;
    int ladder2;

    vector<double> x;
    vector<double> y;
    vector<double> z;
    vector<double> r;
    vector<double> y_err;
    vector<double> z_err;

    double pT;
    double rad;
    double cx, cy;
    double p_reco[3];

    double A[3][3];
    double B[1][3];
    double beta[1][3];

    bool dca2d_range_out = false;
    bool dcaz_range_out = false;
    bool dca_range_out = false;
    bool is_deleted = false;

    track() : track_xy(nullptr),
              track_rz(nullptr),
              track_rz_inv(nullptr)

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
        // cout << "d_phi : " << d_phi / M_PI * 180 << endl;
        // d_phi -= -2 * (M_PI * (int)(d_phi / (M_PI)));

        if (fabs(d_phi) > M_PI)
        {
            // cout << "fabs(d_phi) > M_PI  " << d_phi << endl;
            // cout << (M_PI * (int)(d_phi / (M_PI))) << endl;
            // d_phi -=  (M_PI * (int)(d_phi / (M_PI)));
            d_phi -= 2 * (M_PI * (int)(d_phi / (M_PI)));

            // d_phi -=d_phi * fabs(d_phi) *  M_PI * (int)(d_phi / M_PI);
        }
        // cout << " d_phi : " << d_phi / M_PI * 180 << endl;

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
        // cout << "phi : " << phi_p + phi_offset << endl;

        p_reco[0] = /*sign_x **/ fabs(pT) * cos(phi_p + phi_offset);
        p_reco[1] = /*sign_y **/ fabs(pT) * sin(phi_p + phi_offset);
        // cout << "p1 : " << p1.x() - dca_mean[0] << "  " << p1.y() - dca_mean[1] << endl;
        // cout << "p  : " << p_reco[0] << "  " << p_reco[1] << endl;
        p_reco[2] = fabs(pT) / t_ * p1.z();
        // cout << endl;
    }

    void calc_line()
    {
        x.push_back(p1.x());
        x.push_back(p2.x());
        x.push_back(dca_mean[0]);

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

        // r-z plane
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

        //        track_rz->SetParameter(1, least_square_zr.GetSlope());
        // track_rz->SetParameter(0, least_square_zr.GetIntercept());
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
            r = (p1.y() / fabs(p1.y())) * sqrt(p1.x() * p1.x() + p1.y() * p1.y());
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

        if (getpointr(1) < getpointr(3))
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

    /*void print()
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
             << "slope = " << track_rz->GetParameter(1) << "  Intercept = " << track_rz->GetParameter(0)
             << endl;
    }*/
};

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
        double z_outer = (y - truth_rz->GetParameter(0)) / truth_rz->GetParameter(1);

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
            r = m_yvtx / fabs(m_yvtx) * sqrt(m_xvtx * m_xvtx + m_yvtx * m_yvtx);
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
};