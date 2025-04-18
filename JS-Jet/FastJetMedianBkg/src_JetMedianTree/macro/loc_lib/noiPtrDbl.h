#ifndef noiPtrDbl__h
#define noiPtrDbl__h

#ifndef tuClass__h
// David Stewart, Dec 1 2022
// Stripped down version of a convenience class that provide's *double and int for histograms binning
// initialized with vector<double>
struct noiPtrDbl {
    // internal
    vector<double> vec{};
    double*        ptr{nullptr};
    int            size{0};

    operator int () { return size; };
    operator double* () { return ptr; };
    operator vector<double> () { return vec; };

    noiPtrDbl(TAxis* ax, double bin_loc=0.5, bool get_widths=false) {
        int n_bins = ax->GetNbins();
        if (get_widths) {
            for (int i{1}; i<=n_bins; ++i) vec.push_back(ax->GetBinWidth(i)*bin_loc);
        } else if (bin_loc == 0.5) {
            for (int i{1}; i<=n_bins; ++i) vec.push_back(ax->GetBinCenter(i));
        } else if (bin_loc == 0.) {
            for (int i{1}; i<=n_bins; ++i) vec.push_back(ax->GetBinLowEdge(i));
        } else if (bin_loc == 1.) {
            for (int i{1}; i<=n_bins; ++i) vec.push_back(ax->GetBinUpEdge(i));
        } else {
            for (int i{1}; i<=n_bins; ++i) {
                double W = ax->GetBinWidth(i);
                double L  = ax->GetBinLowEdge(i);
                vec.push_back(L+W*bin_loc);
            }
        }
        size = vec.size();
        ptr = new double[size];
        for (int i{0}; i<size; ++i) ptr[i] = vec[i];
    }
    noiPtrDbl(TH1* hg, bool get_errors=false) {
        if (get_errors) {
            for (auto i{1}; i<= hg->GetXaxis()->GetNbins(); ++i) {
                vec.push_back(hg->GetBinError(i));
            }
        } else {
            for (auto i{1}; i<= hg->GetXaxis()->GetNbins(); ++i) {
                vec.push_back(hg->GetBinContent(i));
            }
        }
        size = vec.size();
        ptr = new double[size];
        for (int i{0}; i<size; ++i) ptr[i] = vec[i];
    }
};

TGraphAsymmErrors* TGASE_errors (TH1D* hg, array<double,4> x_rat={-1,-1,0.5}) {
    int size;
    noiPtrDbl x     {hg->GetXaxis()};
    noiPtrDbl err_x {hg->GetXaxis(), 0.5, true};

    noiPtrDbl y     {hg};
    noiPtrDbl err_y {hg,true};

    noiPtrDbl err_y_lo = err_y;
    noiPtrDbl err_y_hi = err_y;
    
    auto tgase = new TGraphAsymmErrors (x.size,x,y,err_x,err_x,err_y_lo,err_y_hi);
    /* tgase->SetMarkerColor(hg->GetMarkerColor()); */
    /* tgase->SetMarkerSize(hg->GetMarkerSize()); */
    /* tgase->SetMarkerStyle(hg->GetMarkerStyle()); */
    /* tgase->SetLineColor(hg->GetLineColor()); */
    /* tgase->SetLineStyle(hg->GetLineStyle()); */
    size = x.size;

    if (x_rat[0]>=0) {
        double r_left   { x_rat[0] };
        double r_right  { x_rat[1] };
        double r_center { x_rat[2] }; // relative position between left and right
        double offset_c { x_rat[3] };

        double* x = tgase->GetX();
        for (int i{0}; i<size; ++i) {
            double deltaX = tgase->GetErrorXlow(i) + tgase->GetErrorXhigh(i);
            double anchor  = x[i]-tgase->GetErrorXlow(i);
            double p_left  = anchor + r_left  * deltaX;
            double p_right = anchor + r_right * deltaX;
            double p_center = anchor + r_center * deltaX + offset_c;
            tgase->SetPointEXlow (i,  p_center-p_left);
            tgase->SetPointEXhigh(i,  p_right-p_center);
            tgase->SetPoint(i,p_center,tgase->GetY()[i]);
        }
    }
    return tgase;
};

#endif // endif noiclass noibinvec definition

#endif
