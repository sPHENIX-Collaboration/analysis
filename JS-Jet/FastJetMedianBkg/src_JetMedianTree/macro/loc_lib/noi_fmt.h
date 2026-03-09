// David Stewart, Dec 1 2022
// A way to format plots

#ifndef noi_fmt__h
#define noi_fmt__h
#include "noiDict.h"

noiDict _tu_fmt__hg_dict() {
    return {{
        "MarkerStyle", kFullCircle,
        "MarkerColor", kBlack,
        "MarkerAlpha", 1.,
        "MarkerSize",  1.,

        "LineWidth",   1 ,
        "LineStyle",   1 ,

        /* "FillStyle", 4000, */
        /* {"LineColor",   kBlack }, */
        /* {"LineAlpha",   1. }, */

        "xAxisTitleFont",     43,
        "xAxisTitleSize",     22,
        "xAxisTitleOffset", 1.6 ,

        "xAxisLabelFont",      43,
        "xAxisLabelSize",      18,
        "xAxisLabelOffset",  0.02,

        "yAxisTitleFont",     43,
        "yAxisTitleSize",     22,
        "yAxisTitleOffset", 1.85,

        "yAxisLabelFont",     43,
        "yAxisLabelSize",     18,
        "yAxisLabelOffset",  0.02,

        "zAxisTitleFont", 43,
        "zAxisTitleSize", 22,
        "zAxisTitleOffset", 2.4 ,

        "zAxisLabelFont", 43,
        "zAxisLabelSize", 18,
        "zAxisLabelOffset", 0.02 ,

        "SetStats", 0
    }};
};

TH1* noi_fmt (TH1* hg, noiDict _override={{}}, noiDict dict=_tu_fmt__hg_dict()) {
    // if don't want defaults, add {} to override value
    dict += _override;

    /* if (dict["noTitle"])      hg->SetTitle(""); */
    if (dict["Name"])     hg->SetName(dict("Name"));

    if (dict["SetStats"])     hg->SetStats((int)dict("SetStats"));

    if (dict["no_errors"]) for (int i=1; i<=hg->GetNbinsX(); ++i) hg->SetBinError(i,0.);


    if (dict["MarkerStyle"]) hg->SetMarkerStyle(dict("MarkerStyle"));

    if (dict["MarkerColor"])   hg->SetMarkerColor(dict("MarkerColor"));
    if (dict["MarkerColorAlpha"]) hg->SetMarkerColorAlpha(dict("MarkerColorAlpha",1),dict("MarkerColorAlpha",2));


    if (dict["MarkerSize"]) hg->SetMarkerSize(dict("MarkerSize"));

    if (dict["LineWidth"]) hg->SetLineWidth(dict("LineWidth"));
    if (dict["LineStyle"]) hg->SetLineStyle(dict("LineStyle"));

    if (dict["LineColor"]) hg->SetLineColor(dict("LineColor"));
    if (dict["LineColorAlpha"]) hg->SetLineColorAlpha(dict("LineColorAlpha",1),dict("LineColorAlpha",2));

    if (!dict["LineColor"] && !dict["LineColorAlpha"] && (dict["MarkerColor"] || dict["MarkerColorAlpha"])) {
        if (dict["MarkerColor"]) hg->SetLineColor(dict("MarkerColor"));
        if (dict["MarkerColorAlpha"]) hg->SetLineColorAlpha(dict("MarkerColorAlpha",1),dict("MarkerColorAlpha",2));
    }
    /* else if (dict["MarkerColor"]) */
        /* hg->SetLineColorAlpha(dict("MarkerColor"), dict("LineAlpha",dict("MarkerAlpha",1.))); */
    if (dict["FillColor"]) hg->SetFillColor(dict("FillColor"));
    if (dict["FillColorAlpha"]) hg->SetFillColorAlpha(dict("FillColorAlpha",1),dict("FillColorAlpha",2));

    if (dict["FillStyle"]) hg->SetFillStyle(dict("FillStyle"));

    // Set titles
    if (dict["Title"]) hg->SetTitle(dict("Title"));
    if (dict["xAxisTitle"]) hg->GetXaxis()->SetTitle(dict("xAxisTitle"));
    if (dict["yAxisTitle"]) hg->GetYaxis()->SetTitle(dict("yAxisTitle"));

    // Set axes styles

    if (dict["TitleSize"]) hg->SetTitleSize(dict("TitleSize"));
    if (dict["TitleFont"]) hg->SetTitleFont(dict("TitleFont"));
    // x Axis
    if (dict["xAxisTitleFont"])
        hg->GetXaxis()->SetTitleFont(dict("xAxisTitleFont"));
    if (dict["xAxisTitleSize"])
        hg->GetXaxis()->SetTitleSize(dict("xAxisTitleSize"));
    /* cout << " beta " << endl; */
    /* if (dict["xAxisTitleOffset"]) cout << " Setting xaxistitleoffset " <<  dict("xAxisTitleOffset") << endl; */
    if (dict["xAxisTitleOffset"])
        hg->GetXaxis()->SetTitleOffset(dict("xAxisTitleOffset"));
    if (dict["xAxisLabelFont"])
        hg->GetXaxis()->SetLabelFont(dict("xAxisLabelFont"));
    if (dict["xAxisLabelSize"])
        hg->GetXaxis()->SetLabelSize(dict("xAxisLabelSize"));
    if (dict["xAxisLabelOffset"])
        hg->GetXaxis()->SetLabelOffset(dict("xAxisLabelOffset"));

    // y Axis
    if (dict["yAxisTitleFont"])
        hg->GetYaxis()->SetTitleFont(dict("yAxisTitleFont"));
    if (dict["yAxisTitleSize"])
        hg->GetYaxis()->SetTitleSize(dict("yAxisTitleSize"));
    if (dict["yAxisTitleOffset"])
        hg->GetYaxis()->SetTitleOffset(dict("yAxisTitleOffset"));
    if (dict["yAxisLabelFont"])
        hg->GetYaxis()->SetLabelFont(dict("yAxisLabelFont"));
    if (dict["yAxisLabelSize"])
        hg->GetYaxis()->SetLabelSize(dict("yAxisLabelSize"));
    if (dict["yAxisLabelOffset"])
        hg->GetYaxis()->SetLabelOffset(dict("yAxisLabelOffset"));


    // z Axis
    if (dict["zAxisTitleFont"])
        hg->GetZaxis()->SetTitleFont(dict("zAxisTitleFont"));
    if (dict["zAxisTitleSize"])
        hg->GetZaxis()->SetTitleSize(dict("zAxisTitleSize"));
    if (dict["zAxisTitleOffset"])
        hg->GetZaxis()->SetTitleOffset(dict("zAxisTitleOffset"));
    if (dict["zAxisLabelFont"])
        hg->GetZaxis()->SetLabelFont(dict("zAxisLabelFont"));
    if (dict["zAxisLabelSize"])
        hg->GetZaxis()->SetLabelSize(dict("zAxisLabelSize"));
    if (dict["zAxisLabelOffset"])
        hg->GetZaxis()->SetLabelOffset(dict("zAxisLabelOffset"));
    
    if (dict["xAxisRange"]) hg->GetXaxis()->SetRangeUser(dict("xAxisRange",1),dict("xAxisRange",2));
    // Set Axis ranges with {x||y||z}AxisRange{Lo||Hi}
    if (dict["xAxisRangeLo"] || dict["xAxisRangeHi"]) {
        if (!dict["xAxisRangeLo"] || !dict["xAxisRangeHi"]) {
            cout << " Warning in noi_fmt: has xAxisRange{lo||hi} but not both. Needs both."<<endl;
            cout << " -> Not setting xAxisRange." << endl;
        } else {
            hg->GetXaxis()->SetRangeUser(dict("xAxisRangeLo"), dict("xAxisRangeHi"));
        }
    }

    if (dict["yAxisRange"]) hg->GetYaxis()->SetRangeUser(dict("yAxisRange",1),dict("yAxisRange",2));
    if (dict["yAxisRangeLo"] || dict["yAxisRangeHi"]) {
        if (!dict["yAxisRangeLo"] || !dict["yAxisRangeHi"]) {
            cout << " Warning in noi_fmt: has yAxisRange{Lo||Hi} but not both. Needs both."<<endl;
            cout << " -> Not setting yAxisRange." << endl;
        } else {
            hg->GetYaxis()->SetRangeUser(dict("yAxisRangeLo"), dict("yAxisRangeHi"));
        }
    }
    if (dict["zAxisRangeLo"] || dict["zAxisRangeHi"]) {
        if (!dict["zAxisRangeLo"] || !dict["zAxisRangeHi"]) {
            cout << " Warning in noi_fmt: has zAxisRange{lo||hi} but not both. Needs both."<<endl;
            cout << " -> Not setting zAxisRange." << endl;
        } else {
            hg->GetZaxis()->SetRangeUser(dict("zAxisRangeLo"), dict("zAxisRangeHi"));
        }
    }
    // Set Ndivisions
    if (dict["xAxisNdivisions"]) hg->GetXaxis()->SetNdivisions(dict("xAxisNdivisions"));
    if (dict["yAxisNdivisions"]) hg->GetYaxis()->SetNdivisions(dict("yAxisNdivisions"));
    if (dict["zAxisNdivisions"]) hg->GetZaxis()->SetNdivisions(dict("zAxisNdivisions"));
    return hg;
};

TLine* noi_fmt(TLine* line, noiDict options={{}}, noiDict dict={{
        "LineColor",kBlack, "LineColorAlpha",kBlack, 1., "LineStyle",1,
        "LineWidth",1 }}) 
{
    dict += options;
    line->SetLineColorAlpha(dict("LineColorAlpha",1),dict("LineColorAlpha",2));
    line->SetLineStyle(dict("LineStyle"));
    line->SetLineWidth(dict("LineWidth"));
    return line;
}

TLegend* noi_fmt (TLegend* leg, noiDict _override={{}}, 
        // default values:
        noiDict dict={{ "LineColor", kWhite, "LineColorAlpha",0., "FillColorAlpha",kWhite, 0. }}
) {
    dict += _override;
    if (dict["LineColor"]) 
        leg->SetLineColorAlpha(dict("LineColor"),dict("LineColorAlpha",0.));
    if (dict["FillColor"]) 
        leg->SetLineColorAlpha(dict("FillColor"),dict("FillColorAlpha",0.));
    if (dict["FillStyle"])
        leg->SetFillStyle(dict("FillStyle"));
    if (dict["TextSize"]) leg->SetTextSize(dict("TextSize"));
    return leg;
};

TGraph* noi_fmt (TGraph* hg, noiDict _override={{}}, noiDict dict=_tu_fmt__hg_dict()) {
    // if don't want defaults, add {} to override value
    dict += _override;

    if (dict["MarkerStyle"]) hg->SetMarkerStyle(dict("MarkerStyle"));
    if (dict["MarkerColor"])   hg->SetMarkerColor(dict("MarkerColor"));
    if (dict["MarkerColorAlpha"]) hg->SetMarkerColorAlpha(dict("MarkerColorAlpha",1),dict("MarkerColorAlpha",2));

    if (dict["MarkerSize"]) hg->SetMarkerSize(dict("MarkerSize"));
    if (dict["LineWidth"]) hg->SetLineWidth(dict("LineWidth"));
    if (dict["LineStyle"]) hg->SetLineStyle(dict("LineStyle"));

    if (dict["LineColor"]) hg->SetLineColor(dict("LineColor"));
    if (dict["LineColorAlpha"]) hg->SetLineColorAlpha(dict("LineColorAlpha",1),dict("LineColorAlpha",2));

    if (!dict["LineColor"] && !dict["LineColorAlpha"] && (dict["MarkerColor"] || dict["MarkerColorAlpha"])) {
        if (dict["MarkerColor"]) hg->SetLineColor(dict("MarkerColor"));
        if (dict["MarkerColorAlpha"]) hg->SetLineColorAlpha(dict("MarkerColorAlpha",1),dict("MarkerColorAlpha",2));
    }
    /* else if (dict["MarkerColor"]) */
        /* hg->SetLineColorAlpha(dict("MarkerColor"), dict("LineAlpha",dict("MarkerAlpha",1.))); */
    if (dict["FillColor"]) hg->SetFillColor(dict("FillColor"));
    if (dict["FillColorAlpha"]) hg->SetFillColorAlpha(dict("FillColorAlpha",1),dict("FillColorAlpha",2));

    if (dict["FillStyle"]) hg->SetFillStyle(dict("FillStyle"));

    // Set titles
    if (dict["Title"]) hg->SetTitle(dict("Title"));
    if (dict["xAxisTitle"]) hg->GetXaxis()->SetTitle(dict("xAxisTitle"));
    if (dict["yAxisTitle"]) hg->GetYaxis()->SetTitle(dict("yAxisTitle"));

    // Set axes styles

    // x Axis
    if (dict["xAxisTitleFont"])
        hg->GetXaxis()->SetTitleFont(dict("xAxisTitleFont"));
    if (dict["xAxisTitleSize"])
        hg->GetXaxis()->SetTitleSize(dict("xAxisTitleSize"));
    /* cout << " beta " << endl; */
    /* if (dict["xAxisTitleOffset"]) cout << " Setting xaxistitleoffset " <<  dict("xAxisTitleOffset") << endl; */
    if (dict["xAxisTitleOffset"])
        hg->GetXaxis()->SetTitleOffset(dict("xAxisTitleOffset"));
    if (dict["xAxisLabelFont"])
        hg->GetXaxis()->SetLabelFont(dict("xAxisLabelFont"));
    if (dict["xAxisLabelSize"])
        hg->GetXaxis()->SetLabelSize(dict("xAxisLabelSize"));
    if (dict["xAxisLabelOffset"])
        hg->GetXaxis()->SetLabelOffset(dict("xAxisLabelOffset"));

    // y Axis
    if (dict["yAxisTitleFont"])
        hg->GetYaxis()->SetTitleFont(dict("yAxisTitleFont"));
    if (dict["yAxisTitleSize"])
        hg->GetYaxis()->SetTitleSize(dict("yAxisTitleSize"));
    if (dict["yAxisTitleOffset"])
        hg->GetYaxis()->SetTitleOffset(dict("yAxisTitleOffset"));
    if (dict["yAxisLabelFont"])
        hg->GetYaxis()->SetLabelFont(dict("yAxisLabelFont"));
    if (dict["yAxisLabelSize"])
        hg->GetYaxis()->SetLabelSize(dict("yAxisLabelSize"));
    if (dict["yAxisLabelOffset"])
        hg->GetYaxis()->SetLabelOffset(dict("yAxisLabelOffset"));

    if (dict["xAxisRange"]) hg->GetXaxis()->SetRangeUser(dict("xAxisRange",1),dict("xAxisRange",2));
    // Set Axis ranges with {x||y||z}AxisRange{Lo||Hi}
    if (dict["xAxisRangeLo"] || dict["xAxisRangeHi"]) {
        if (!dict["xAxisRangeLo"] || !dict["xAxisRangeHi"]) {
            cout << " Warning in noi_fmt: has xAxisRange{lo||hi} but not both. Needs both."<<endl;
            cout << " -> Not setting xAxisRange." << endl;
        } else {
            hg->GetXaxis()->SetRangeUser(dict("xAxisRangeLo"), dict("xAxisRangeHi"));
        }
    }

    if (dict["yAxisRange"]) hg->GetYaxis()->SetRangeUser(dict("yAxisRange",1),dict("yAxisRange",2));
    if (dict["yAxisRangeLo"] || dict["yAxisRangeHi"]) {
        if (!dict["yAxisRangeLo"] || !dict["yAxisRangeHi"]) {
            cout << " Warning in noi_fmt: has yAxisRange{Lo||Hi} but not both. Needs both."<<endl;
            cout << " -> Not setting yAxisRange." << endl;
        } else {
            hg->GetYaxis()->SetRangeUser(dict("yAxisRangeLo"), dict("yAxisRangeHi"));
        }
    }
    // Set Ndivisions
    if (dict["xAxisNdivisions"]) hg->GetXaxis()->SetNdivisions(dict("xAxisNdivisions"));
    if (dict["yAxisNdivisions"]) hg->GetYaxis()->SetNdivisions(dict("yAxisNdivisions"));
    return hg;
};
    
#endif
