#ifndef LOC_LIB_H
#define LOC_LIB_H

// local library for jet unfolding

#include "noiBinVec.h"
#include "pAu_bins.h"
#include "noi_fnc.h"


TH1D* norm_binwidths(TH1D* hg) {
    auto ax = hg->GetXaxis();
    for (int i=1;i<=ax->GetNbins();++i){
        double _val = hg->GetBinContent(i);
        double _err = hg->GetBinError(i);
        double weight = 1./ax->GetBinWidth(i);
        hg->SetBinContent(i,_val*weight);
        hg->SetBinError  (i,_err*weight);
    }
    return hg;
}

   float calc_dphi(float a, float b) {
     float _ = a-b;
     while (_ <-M_PI) _ += 2*M_PI;
     while (_ > M_PI) _ -= 2*M_PI;
     return _;
   }

TH1D* rebin_TH1D(TH1D* hg_in, vector<int> i_bins, string _name="", bool delete_hg_in=true, bool norm_by_cnt=true, bool norm_by_width = true) {
    // input: bins numbers for left-hand most bin in each new bin. Example: 1, 2, 3, 4, 20, would have 5 bins, with bin 1-1, 2-2, 3-3, 4-20 in each of the new bins
    double cnt = hg_in->Integral();
    string name = (_name=="") ? noiUniqueName() : _name;
    auto ax = hg_in->GetXaxis();
    vector<double> edges;
    int nbins = i_bins.size()-1;

    if (nbins<1) return hg_in;
    for (int i=0; i<=nbins; ++i) {
        edges.push_back(ax->GetBinLowEdge(i_bins[i]));
    }
    tuBinVec new_bins { edges };
    auto hg_out = new TH1D(name.c_str(), Form("%s;%s;%s",hg_in->GetTitle(),
            hg_in->GetXaxis()->GetTitle(), hg_in->GetYaxis()->GetTitle()),
            new_bins, new_bins);
    for (int i=0; i<nbins; ++i) {
        int i0 = i_bins[i];
        int i1 = i_bins[i+1];
        double sum=0;
        for (int k=i0;k<i1;++k) {
            sum += hg_in->GetBinContent(k);
        }
        hg_out->SetBinContent(i+1, sum);
        hg_out->SetBinError(i+1, pow(sum,0.5));
    }
    if (norm_by_width) {
        for (int i=1; i<=nbins; ++i) {
            auto c = hg_out->GetBinContent(i);
            auto e = hg_out->GetBinError  (i);
            auto w = hg_out->GetXaxis()->GetBinWidth(i);
            hg_out->SetBinContent(i, c/w);
            hg_out->SetBinError  (i, e/w);
        }
    }
    if (norm_by_cnt) {
        hg_out->Scale(1./cnt);
    }
    if (delete_hg_in) delete hg_in;
    return hg_out;
}

void div_by_W (TH1D* hg, TH1D* w) {
    for (int i=1; i<=hg->GetNbinsX(); ++i) {
        double W = w->GetBinContent(i);
        if (W==0) {
            hg->SetBinContent(i, 0.);
            hg->SetBinError  (i, 0.);
        } else {
            hg->SetBinContent(i, hg->GetBinContent(i)/W);
            hg->SetBinError  (i, hg->GetBinError  (i)/W);
        }
    }
}

void set_range(int axis, int bin0, int bin1, vector<THnSparseD*> vec_sparse, bool print=false) {
    bool first = true;
    for (auto& sparse : vec_sparse) {
        TAxis* ax = sparse->GetAxis(axis);
        sparse->GetAxis(axis)->SetRange(bin0, bin1);
        if (first && print) {
            first = false;
            cout << Form("Set range(%2i) to %7.2f-%7.2f",axis,ax->GetBinLowEdge(bin0),ax->GetBinUpEdge(bin1)) << endl;
        }
    }
}

pair<double,double> sparse_integral(THnSparseD* sp) {
    auto hg = sp->Projection(0);
    double _val, _err;
    _val = hg->IntegralAndError(1, hg->GetNbinsX(), _err);
    delete hg;
    return {_val,_err};
}

TH1D* sparse_proj(THnSparseD* sp, int i_ax, string name="", bool norm_bin_width=false) {
    auto hg = (TH1D*) sp->Projection(i_ax);
    if (name != "") hg->SetName(name.c_str());
    else              hg->SetName(noiUniqueName());
    if (norm_bin_width) norm_binwidths(hg);
    return hg;
}

void set_track_cuts(vector<THnSparseD*> data) {
    set_range(_dca,     1,  5,  data);
    set_range(_nhitfit, 20, 48, data);
    set_range(_nhitrat, 6,  10, data);
}

TH1D* emb_eff_per_zdcx_binnedby3(THnSparseD* reco, THnSparseD* truth, string name) {
    array<int,5> i0_zdcx { 5, 8,  11, 14, 17 }; // binning by 3 for zdcx
    array<int,5> i1_zdcx { 7, 10, 13, 16, 19 };
   
    tuBinVec bins_5by3to20 {{ 5000., 5000., 5, 20000 }};
    //
    /* array<int,7> i0_zdcx { 4, 7, 10, 13, 16, 19, 22 }; */
    /* array<int,7> i1_zdcx { 4, 7, 10, 13, 16, 19, 22 }; */
    
    /* tuBinVec bins_5by3to20 {{ 5000., 5000., 5, 20000 }}; */
    TH1D* hg_out = new TH1D(name.c_str(), ";ZDCx;Tracking Efficiency", bins_5by3to20, bins_5by3to20 );

    for (int i=0; i<5; ++i) {
        set_range(_zdcx, i0_zdcx[i], i1_zdcx[i], {reco, truth});
        auto h_reco  = (TH1D*) reco->Projection(_pt);  h_reco ->SetName(noiUniqueName());
        auto h_truth = (TH1D*) truth->Projection(_pt); h_truth->SetName(noiUniqueName());
        double _val, _err;
        _val = h_reco->IntegralAndError(3,h_reco->GetNbinsX(),_err);
        cout << " i("<<i<<") val("<<_val<<" +/- "<<_err<<")   ----> " << _err/_val << endl;
        double norm = h_truth->Integral(3,h_truth->GetNbinsX());
        delete h_reco;
        delete h_truth;
        if (norm == 0) continue;
        _val /= norm;
        _err /= norm;
        hg_out->SetBinContent(i+1,_val);
        hg_out->SetBinError  (i+1,_err);
    }
    return hg_out;
}

TH1D* hg_abs_max(vector<TH1D*> vdat) {
    if (vdat.size()==0) return nullptr;
    auto hg_new = (TH1D*) vdat[0]->Clone(noiUniqueName());
    for (auto bin=1; bin<=hg_new->GetNbinsX(); ++bin) {
        double val0 = abs(hg_new->GetBinContent(bin));
        for (auto ihg=0; ihg<vdat.size(); ++ihg) {
            double val1 = abs(vdat[ihg]->GetBinContent(bin));
            if (val1>val0) val0 = val1;
        }
        hg_new->SetBinContent(bin,val0);
        hg_new->SetBinError(bin,0);
    }
    return hg_new;
}

TH1D* hg_abs_deltarat(TH1D* nom, TH1D* comp) {
    auto hg_new = (TH1D*) nom->Clone(noiUniqueName());
    hg_new->Reset();
    for (int i=1; i<=nom->GetNbinsX(); ++i) {
        if (nom->GetBinContent(i) != 0) {
            auto v_nom  = nom->GetBinContent(i);
            auto v_comp = comp->GetBinContent(i);
            hg_new->SetBinContent(i, TMath::Abs(v_comp-v_nom)/v_nom);
        }
    }
    return hg_new;
}

TH1D* hg_abs_delta(TH1D* nom, TH1D* comp) {
    auto hg_new = (TH1D*) nom->Clone(noiUniqueName());
    hg_new->Reset();
    for (int i=1; i<=nom->GetNbinsX(); ++i) {
        /* if (nom->GetBinContent(i) != 0) { */
            auto v_nom  = nom->GetBinContent(i);
            auto v_comp = comp->GetBinContent(i);
            hg_new->SetBinContent(i, TMath::Abs(v_comp-v_nom));
        /* } */
    }
    return hg_new;
}

TH1D* hg_const(TH1D* nom, double val, string name) {
    auto hg_new = (TH1D*) nom->Clone(name.c_str());
    hg_new->Reset();
    for (int i=1; i<=hg_new->GetNbinsX(); ++i) {
        hg_new->SetBinContent(i, val); 
    }
    return hg_new;
}

TH1D* hg_RSS(vector<TH1D*> vec, string name) {
    auto hg_new = (TH1D*) vec[0]->Clone(name.c_str());
    hg_new->Reset();
    for (auto& hg : vec) {
        for (int i=1; i<=hg_new->GetNbinsX(); ++i) {
            hg_new->SetBinContent(i, hg_new->GetBinContent(i) + TMath::Sq(hg->GetBinContent(i)));
        }
    }
    for (int i=1; i<=hg_new->GetNbinsX(); ++i) {
        hg_new->SetBinContent(i, TMath::Sqrt(hg_new->GetBinContent(i)));
    }
    return hg_new;
}

TH1D* hg_syserr(TH1D* hg_nom, TH1D* hg_rss, string name) {
    auto hg_new = (TH1D*) hg_nom->Clone(name.c_str());
    for (int i=1; i<=hg_new->GetNbinsX(); ++i) {
        hg_new->SetBinError(i, hg_nom->GetBinContent(i) * hg_rss->GetBinContent(i));
        /* hg_new->SetBinError(i, .4); */
        /* cout << "i " << i << " -> " << hg_new->GetBinContent(i) << "  " << hg_new->GetBinError(i) << endl; */
    }
    return hg_new;
}

/* int noi_geant05(int geantid) { */
/*     switch (geantid) { */
/*         case 8: return 0; */
/*         case 9: return 1; */
/*         case 11: return 2; */
/*         case 12: return 3; */
/*         case 14: return 4; */
/*         case 15: return 5; */
/*     } */
/*     return -1; */
/* }; */



// make tuBinVec avialable locally

#endif
