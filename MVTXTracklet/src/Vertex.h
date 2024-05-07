#ifndef VERTEX_H
#define VERTEX_H

#include <Riostream.h>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <stdlib.h>
#include <vector>
#include <tuple>

#include <TCanvas.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMarker.h>
#include <TRandom3.h>
#include <TText.h>
#include <TTree.h>
#include <TVector3.h>

#include "Utilities.h"
#include "Hit.h"

#include "/sphenix/user/hjheng/TrackletAna/analysis/plot/sPHENIXStyle/sPhenixStyle.C"

#define NZGAP 0

int Ncluster_toplot = 1300;

using namespace std;

struct MvtxClusComb
{
    MvtxClusComb(float x1, float y1, float z1, float x2, float y2, float z2, float extz) : x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2), extz(extz){};

    float x1, y1, z1, x2, y2, z2, extz;
};

struct Cluster
{
    Cluster(uint32_t index, uint32_t nz) : index(index), nz(nz){};

    uint32_t index;
    uint32_t nz;
};

struct Vertex
{
    Vertex(float vz, float sigma2, vector<float> hitx1, vector<float> hity1, vector<float> hitz1, vector<float> hitx2, vector<float> hity2, vector<float> hitz2, vector<float> candz)
        : vz(vz), sigma2(sigma2), hitx1(hitx1), hity1(hity1), hitz1(hitz1), hitx2(hitx2), hity2(hity2), hitz2(hitz2), candz(candz){};

    float vz;
    float sigma2;
    vector<float> hitx1, hity1, hitz1, hitx2, hity2, hitz2, candz;
};

struct VtxData
{
    bool isdata;
    int event, NhitsLayer1, NTruthVtx;
    float TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z, TruthPV_mostNpart_x, TruthPV_mostNpart_y, TruthPV_mostNpart_z;
    // PV positions from two MVTX halves
    float PV_x_halves1, PV_y_halves1, PV_z_halves1, PV_x_halves2, PV_y_halves2, PV_z_halves2;
    float Centrality_bimp, Centrality_impactparam, Centrality_mbd, Centrality_mbdquantity;
};

// https://github.com/bi-ran/tracklet/blob/master/analysis/include/tracklet.h
// float TrackletPV_cluster(int evt, vector<Hit *> layer1, vector<Hit *> layer2, int dPhiCutbin, int dZCutbin, bool verbose)
// {
//     float dPhi_cut = dPhiCutbin * 0.01;
//     float dZ_cut = dZCutbin * 0.01;

//     float vertex = -999.;

//     vector<double> vertices;
//     vertices.clear();

//     for (size_t i = 0; i < layer1.size(); i++)
//     {
//         for (size_t j = 0; j < layer2.size(); j++)
//         {
//             if (fabs(deltaPhi(layer1[i]->Phi(), layer2[j]->Phi())) > dPhi_cut)
//                 continue;

//             float z = layer1[i]->posZ() - (layer2[j]->posZ() - layer1[i]->posZ()) / (layer2[j]->rho() - layer1[i]->rho()) * layer1[i]->rho();
//             vertices.push_back(z);
//         }
//     }

//     /* vertex clustering */
//     if (vertices.size())
//     {
//         sort(vertices.begin(), vertices.end());

//         std::vector<Cluster> clusters;
//         std::size_t z = 0, y = 0;
//         for (; z < vertices.size(); ++z)
//         {
//             for (; y < vertices.size() && vertices[y] - vertices[z] < dZ_cut; ++y)
//                 ;
//             clusters.emplace_back(z, y - z); // index, nz
//         }

//         std::sort(clusters.begin(), clusters.end(), [](const Cluster &a, const Cluster &b) -> bool { return a.nz > b.nz; });

//         std::vector<Vertex> candidates;
//         uint32_t maxnz = clusters[0].nz;

//         for (auto &cluster : clusters)
//         {
//             uint32_t nz = cluster.nz;
//             if (nz + NZGAP < maxnz)
//             {
//                 break;
//             }

//             uint32_t index = cluster.index;

//             float vz = 0;
//             float vz2 = 0;
//             float sigma2 = 0;
//             for (uint32_t y = 0; y < nz; ++y)
//             {
//                 vz += vertices[index + y];
//                 vz2 += vertices[index + y] * vertices[index + y];
//             }
//             vz /= nz;
//             sigma2 = vz2 / nz - (vz * vz);

//             candidates.emplace_back(vz, sigma2);
//         }

//         vertex = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->vz;
//     }

//     return vertex;
// }

// TCanvas *Canv_RecoVtxZTklZRho(vector<TLine *> v_line, vector<Hit *> hit_l1, vector<Hit *> hit_l2, float PVz)
void Draw_RecoVtxZTklZRho(TH2F *hM_clusZRho_half1, TH2F *hM_clusZRho_half2, vector<TLine *> vtl_half1, vector<TLine *> vtl_half2, float PVz_half1, float PVz_half2, float PVz, bool isdata, TString plotname_tstr)
{
    SetsPhenixStyle();
    gStyle->SetPalette(kThermometer);

    int nTLines = vtl_half1.size() + vtl_half2.size();
    float tlinealpha = (nTLines > 500) ? 0.05 : 0.4;

    // merge histograms
    TH2F *hM_clusZRho = (TH2F *)hM_clusZRho_half1->Clone();
    hM_clusZRho->Add(hM_clusZRho_half2);
    int Nclus = hM_clusZRho->GetEntries();

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    gPad->SetRightMargin(0.08);
    gPad->SetTopMargin(0.05);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.13);
    hM_clusZRho->GetXaxis()->SetTitle("Position Z [cm]");
    hM_clusZRho->GetYaxis()->SetTitle("Radius [cm]");
    hM_clusZRho->GetXaxis()->SetTitleOffset(1.1);
    hM_clusZRho->GetYaxis()->SetTitleOffset(1.3);
    hM_clusZRho->SetMarkerSize(0.5);
    hM_clusZRho->Draw("SCAT");

    for (size_t i = 0; i < vtl_half1.size(); i++)
    {
        vtl_half1[i]->SetLineColorAlpha(14, tlinealpha);
        vtl_half1[i]->Draw("same");
    }
    for (size_t i = 0; i < vtl_half2.size(); i++)
    {
        vtl_half2[i]->SetLineColorAlpha(14, tlinealpha);
        vtl_half2[i]->Draw("same");
    }

    hM_clusZRho->Draw("SCATsame");

    // MVTX dimension
    float MVTXLength = 27.1; // cm
    float MVTXL1_min = 2.461;
    float MVTXL1_mid = 2.523;
    float MVTXL1_max = 2.793;
    float MVTXL2_min = 3.198;
    float MVTXL2_mid = 3.335;
    float MVTXL2_max = 3.625;
    TLine *beamline = new TLine(-20, 0, 20, 0);
    TLine *MVTXl1_in = new TLine(-(MVTXLength / 2), MVTXL1_min, MVTXLength / 2, MVTXL1_min);
    TLine *MVTXl1_out = new TLine(-(MVTXLength / 2), MVTXL1_max, MVTXLength / 2, MVTXL1_max);
    TLine *MVTXl2_in = new TLine(-(MVTXLength / 2), MVTXL2_min, MVTXLength / 2, MVTXL2_min);
    TLine *MVTXl2_out = new TLine(-(MVTXLength / 2), MVTXL2_max, MVTXLength / 2, MVTXL2_max);
    MVTXl1_in->SetLineStyle(kDashed);
    MVTXl1_out->SetLineStyle(kDashed);
    MVTXl2_in->SetLineStyle(kDashed);
    MVTXl2_out->SetLineStyle(kDashed);
    beamline->Draw("same");
    MVTXl1_in->Draw("same");
    MVTXl1_out->Draw("same");
    MVTXl2_in->Draw("same");
    MVTXl2_out->Draw("same");

    TMarker *m_PVz_half1 = new TMarker(PVz_half1, 0.0, 20);
    TMarker *m_PVz_half2 = new TMarker(PVz_half2, 0.0, 20);
    TMarker *m_PVz = new TMarker(PVz, 0.0, 20);
    m_PVz_half1->SetMarkerSize(1);
    m_PVz_half1->SetMarkerColor(38);
    m_PVz_half2->SetMarkerSize(1);
    m_PVz_half2->SetMarkerColor(38);
    m_PVz->SetMarkerSize(1);
    m_PVz->SetMarkerColor(46);
    if (fabs(PVz_half1) <= 20.)
        m_PVz_half1->Draw("same");
    if (fabs(PVz_half2) <= 20.)
        m_PVz_half2->Draw("same");
    if (fabs(PVz) <= 20.)
        m_PVz->Draw("same");

    // TText *t_l1 = new TText(MVTXLength / 2, MVTXL1_min, "Layer 0");
    // TText *t_l2 = new TText(MVTXLength / 2, MVTXL2_min, "Layer 1");
    // t_l1->SetTextSize(0.04);
    // t_l2->SetTextSize(0.04);
    // t_l1->SetTextAlign(11);
    // t_l2->SetTextAlign(11);
    // t_l1->Draw("same");
    // t_l2->Draw("same");

    TLegend *leg = new TLegend(0.6, 0.8, 0.88, 0.92);
    // leg->SetNColumns(3);
    leg->SetTextSize(0.03);
    leg->SetFillStyle(0);
    leg->AddEntry(hM_clusZRho, "Clusters", "p");
    leg->AddEntry(m_PVz_half1, "Estimate of PV_{Reco}", "p");
    leg->AddEntry(m_PVz, "Final PV_{Reco}", "p");
    leg->Draw("same");

    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(kHAlignLeft + kVAlignBottom);
    if (isdata)
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, "#it{#bf{sPHENIX}} Preliminary");
    else
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, "#it{#bf{sPHENIX}} Simulation");
    ltx->SetTextSize(0.04);
    // ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.13, "Au+Au #sqrt{s_{NN}}=200 GeV");
    ltx->SetTextSize(0.035);
    ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "2023-08-30");

    ltx->SetTextSize(0.025);
    ltx->SetTextAlign(kHAlignLeft + kVAlignBottom);
    ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, gPad->GetBottomMargin() + 0.05, Form("N_{Clusters}^{1st + 2nd layer} = %d; Cluster positions assume ideal detector geometry", Nclus));

    c->SaveAs(plotname_tstr + ".pdf");
    c->SaveAs(plotname_tstr + ".png");
    c->Close();
}

// TCanvas *Canv_RecoVtxTklXY(vector<TLine *> v_line, vector<Hit *> hit_l1, vector<Hit *> hit_l2, float PVx, float PVy)
void Draw_RecoVtxTklXY(TH2F *hM_clusXY_half1, TH2F *hM_clusXY_half2, vector<TLine *> vtl_half1, vector<TLine *> vtl_half2, float PVx_half1, float PVx_half2, float PVx, float PVy_half1, float PVy_half2, float PVy, int dZbin, bool isdata,
                       TString plotname_tstr)
{
    SetsPhenixStyle();
    gStyle->SetPalette(kThermometer);

    int nTLines = vtl_half1.size() + vtl_half2.size();
    float tlinealpha = (nTLines > 50) ? 0.3 : 0.4;
    // merge histograms
    TH2F *hM_clusXY = (TH2F *)hM_clusXY_half1->Clone();
    hM_clusXY->Add(hM_clusXY_half2);
    int Nclus = hM_clusXY->GetEntries();

    TCanvas *c = new TCanvas("c", "c", 800, 780);
    c->cd();
    gPad->SetRightMargin(0.08);
    gPad->SetTopMargin(0.05);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.13);
    hM_clusXY->GetXaxis()->SetTitle("Position X [cm]");
    hM_clusXY->GetYaxis()->SetTitle("Position Y [cm]");
    hM_clusXY->GetXaxis()->SetTitleOffset(1.1);
    hM_clusXY->GetYaxis()->SetTitleOffset(1.3);
    hM_clusXY->SetMarkerSize(0.5);
    hM_clusXY->Draw("SCAT");

    for (size_t i = 0; i < vtl_half1.size(); i++)
    {
        vtl_half1[i]->SetLineColorAlpha(14, tlinealpha);
        vtl_half1[i]->Draw("same");
    }
    for (size_t i = 0; i < vtl_half2.size(); i++)
    {
        vtl_half2[i]->SetLineColorAlpha(14, tlinealpha);
        vtl_half2[i]->Draw("same");
    }

    hM_clusXY->Draw("SCATsame");

    TMarker *m_PV1 = new TMarker(PVx_half1, PVy_half1, 20);
    m_PV1->SetMarkerSize(1);
    m_PV1->SetMarkerColor(38);
    m_PV1->Draw("same");
    TMarker *m_PV2 = new TMarker(PVx_half2, PVy_half2, 20);
    m_PV2->SetMarkerSize(1);
    m_PV2->SetMarkerColor(38);
    m_PV2->Draw("same");
    TMarker *m_PV = new TMarker(PVx, PVy, 20);
    m_PV->SetMarkerSize(1);
    m_PV->SetMarkerColor(46);
    m_PV->Draw("same");

    TLine *MVTX_geo = new TLine();
    MVTX_geo->SetLineWidth(2);
    MVTX_geo->SetLineColorAlpha(kBlack, 0.3);
    vector<tuple<float, float, float, float>> MVTXstavepointsXY = MVTXStavePositionXY();
    for (auto &stave : MVTXstavepointsXY)
    {
        // exclude the staves which radius is larger than 3.625 cm
        float xmiddle = (get<0>(stave) + get<2>(stave)) / 2.;
        float ymiddle = (get<1>(stave) + get<3>(stave)) / 2.;
        float radius = sqrt(xmiddle * xmiddle + ymiddle * ymiddle);
        if (radius > 3.625)
            continue;
        MVTX_geo->DrawLine(get<0>(stave), get<1>(stave), get<2>(stave), get<3>(stave));
    }

    TLegend *leg = new TLegend(0.6, 0.8, 0.88, 0.92);
    // leg->SetNColumns(3);
    leg->SetTextSize(0.03);
    leg->SetFillStyle(0);
    leg->AddEntry(hM_clusXY, "Clusters", "p");
    leg->AddEntry(m_PV1, "Estimate of PV_{Reco}", "p");
    leg->AddEntry(m_PV, "Final PV_{Reco}", "p");
    leg->Draw("same");

    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.04);
    ltx->SetTextAlign(kHAlignLeft + kVAlignBottom);
    if (isdata)
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, "#it{#bf{sPHENIX}} Preliminary");
    else
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, "#it{#bf{sPHENIX}} Simulation");
    ltx->SetTextSize(0.035);
    // ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.13, "Au+Au #sqrt{s_{NN}}=200 GeV");
    ltx->SetTextSize(0.035);
    ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, "2023-08-30");

    ltx->SetTextSize(0.025);
    ltx->SetTextAlign(kHAlignLeft + kVAlignBottom);
    ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, gPad->GetBottomMargin() + 0.09, Form("Tracklets of the chosen vertex cluster (|#Deltaz| < %g cm)", dZbin * 0.01));
    ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, gPad->GetBottomMargin() + 0.05, Form("N_{Clusters}^{1st + 2nd layer} = %d; Cluster positions assume ideal detector geometry", Nclus));

    c->SaveAs(plotname_tstr + ".pdf");
    c->SaveAs(plotname_tstr + ".png");
    c->Close();
}

void Draw_1DHistVtxDemo(TH1F *hM, const char *Xtitle, const char *Ytitleunit, int ndivision, bool isdata, bool ispreliminary, const char *date, TString plotname_tstr)
{
    SetsPhenixStyle();
    // Get bin size of the histogram
    double binwidth = hM->GetBinWidth(1);

    TCanvas *c = new TCanvas("c", "c", 800, 750);
    c->cd();
    gPad->SetRightMargin(0.06);
    gPad->SetTopMargin(0.05);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.13);
    hM->GetXaxis()->SetTitle(Xtitle);
    hM->GetYaxis()->SetTitle(Form("Counts / %g %s", binwidth, Ytitleunit));
    hM->GetXaxis()->SetTitleOffset(1.2);
    hM->GetYaxis()->SetTitleOffset(1.55);
    hM->GetYaxis()->SetRangeUser(0, hM->GetMaximum() * 1.35);
    hM->GetXaxis()->SetNdivisions(ndivision, kTRUE);
    hM->SetLineColor(1);
    hM->SetLineWidth(2);
    hM->Draw("hist");
    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(kHAlignLeft + kVAlignBottom);
    if (isdata)
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, Form("#it{#bf{sPHENIX}} %s", (ispreliminary) ? "Preliminary" : "Internal"));
    else
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, "#it{#bf{sPHENIX}} Simulation");
    ltx->SetTextSize(0.04);
    // ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.13, "Au+Au #sqrt{s_{NN}}=200 GeV");
    ltx->SetTextSize(0.035);
    ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, date);
    c->SaveAs(plotname_tstr + ".pdf");
    c->SaveAs(plotname_tstr + ".png");
    delete c;
}

void Draw_2DHistVtxDemo(TH2F *hM, const char *Xtitle, const char *Ytitle, int ndivision, bool isdata, bool ispreliminary, const char *date, TString plotname_tstr)
{
    SetsPhenixStyle();
    gStyle->SetPalette(kThermometer);

    TCanvas *c = new TCanvas("c", "c", 800, 750);
    c->cd();
    gPad->SetRightMargin(0.13);
    gPad->SetTopMargin(0.05);
    gPad->SetLeftMargin(0.15);
    gPad->SetBottomMargin(0.13);
    hM->GetXaxis()->SetTitle(Xtitle);
    hM->GetYaxis()->SetTitle(Ytitle);
    hM->GetXaxis()->SetTitleOffset(1.2);
    hM->GetYaxis()->SetTitleOffset(1.45);
    hM->GetXaxis()->SetNdivisions(ndivision, kTRUE);
    hM->GetYaxis()->SetNdivisions(ndivision, kTRUE);
    hM->Draw("colz");
    TLatex *ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(kHAlignLeft + kVAlignBottom);
    if (isdata)
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, Form("#it{#bf{sPHENIX}} %s", (ispreliminary) ? "Preliminary" : "Internal"));
    else
        ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.08, "#it{#bf{sPHENIX}} Simulation");
    ltx->SetTextSize(0.04);
    // ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(gPad->GetLeftMargin() + 0.05, 1 - gPad->GetTopMargin() - 0.13, "Au+Au #sqrt{s_{NN}}=200 GeV");
    ltx->SetTextSize(0.035);
    ltx->SetTextAlign(kHAlignRight + kVAlignBottom);
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, date);
    c->SaveAs(plotname_tstr + ".pdf");
    c->SaveAs(plotname_tstr + ".png");
    delete c;
}

// Calculate the DCA between the (extrapolated) tracklets and the PV
TVector3 DCA_tklPV(vector<float> pos_clus1, vector<float> pos_clus2, vector<float> pos_PV)
{
    TVector3 v_clus1(pos_clus1[0], pos_clus1[1], pos_clus1[2]); // B
    TVector3 v_clus2(pos_clus2[0], pos_clus2[1], pos_clus2[2]); // A
    TVector3 v_PV(pos_PV[0], pos_PV[1], pos_PV[2]);             // C

    TVector3 u = (v_clus1 - v_clus2).Unit(); // unit vextor from cluster 2 to cluster 1
    TVector3 AC = v_PV - v_clus2;
    TVector3 v_DCA = AC - (AC.Dot(u)) * u; // vector from cluster 2 to the closest point on the line of cluster 1 and cluster 2

    return v_DCA;
}

std::tuple<vector<float>, int, TH2F *, TH2F *, vector<TLine *>, vector<TLine *>, TH1F *, TH2F *, TH1F *, TH2F *, TH1F *, TH2F *, TH2F *> Tracklet3DPV(vector<Hit *> layer1, vector<Hit *> layer2, int dPhiCutbin, int dZCutbin)
{
    // get a random number of integer
    TRandom3 *r = new TRandom3(0);
    int random_int = r->Integer(1E9);
    // tuples for (1) vector of PV estimates, (2) number of candidates in the vertex cluster, (3) TH2F of cluster positions in Z-Rho, (4) TH2F of cluster positions in X-Y, (5) vector of TLines in Z-Rho, (6) vector of TLines in X-Y, (7) TH1F of
    // difference between extrapolated z and PV z, (8) TH2F of difference between extrapolated x/y and PV x/y, (9) TH1F of z position of vertex candidates, (10) TH2F of x/y position of vertex candidates
    tuple<vector<float>, int, TH2F *, TH2F *, vector<TLine *>, vector<TLine *>, TH1F *, TH2F *, TH1F *, TH2F *, TH1F *, TH2F *, TH2F *> t;
    TH2F *hM_ZRho = new TH2F(Form("hM_ZRho_%d", random_int), Form("hM_ZRho_%d", random_int), 2000, -20, 20, 900, -0.6, 4.7);
    TH2F *hM_XY = new TH2F(Form("hM_XY_%d", random_int), Form("hM_XY_%d", random_int), 1100, -5.5, 5.5, 1100, -5.5, 5.5);
    TH1F *hM_Zdiff = new TH1F(Form("hM_Zdiff_%d", random_int), Form("hM_Zdiff_%d", random_int), 60, -0.06, 0.06);
    TH2F *hM_XYdiff = new TH2F(Form("hM_XYdiff_%d", random_int), Form("hM_XYdiff_%d", random_int), 100, -2, 2, 100, -2, 2);
    TH1F *hM_vtxcandz = new TH1F(Form("hM_vtxcandz_%d", random_int), Form("hM_vtxcandz_%d", random_int), 100, -70, 70);
    TH2F *hM_vtxcandxy = new TH2F(Form("hM_vtxcandxy_%d", random_int), Form("hM_vtxcandxy_%d", random_int), 100, -5, 5, 100, -5, 5);
    TH1F *hM_DCAxy = new TH1F(Form("hM_DCAxy_%d", random_int), Form("hM_DCAxy_%d", random_int), 100, 0, 0.5);
    TH2F *hM_DCAxy_DCAz = new TH2F(Form("hM_DCAxy_DCAz_%d", random_int), Form("hM_DCAxy_DCAz_%d", random_int), 100, 0, 0.5, 100, -0.05, 0.05);
    TH2F *hM_DCAx_DCAy = new TH2F(Form("hM_DCAx_DCAy_%d", random_int), Form("hM_DCAx_DCAy_%d", random_int), 100, -0.5, 0.5, 100, -0.5, 0.5);

    int Ncandidates_incluster = 0;

    for (size_t i = 0; i < layer1.size(); i++)
    {
        hM_ZRho->Fill(layer1[i]->posZ(), layer1[i]->rho());
    }
    for (size_t i = 0; i < layer2.size(); i++)
    {
        hM_ZRho->Fill(layer2[i]->posZ(), layer2[i]->rho());
    }

    for (size_t i = 0; i < layer1.size(); i++)
    {
        hM_XY->Fill(layer1[i]->posX(), layer1[i]->posY());
    }
    for (size_t i = 0; i < layer2.size(); i++)
    {
        hM_XY->Fill(layer2[i]->posX(), layer2[i]->posY());
    }

    float dPhi_cut = dPhiCutbin * 0.01;
    float dZ_cut = dZCutbin * 0.01;

    vector<float> vertex = {-999., -999., -999.};

    vector<MvtxClusComb> vertices;
    vertices.clear();
    vector<TLine *> v_tlkprojZ; // for debugging
    v_tlkprojZ.clear();
    vector<TLine *> v_tlkprojXY; // for debugging
    v_tlkprojXY.clear();

    for (size_t i = 0; i < layer1.size(); i++)
    {
        for (size_t j = 0; j < layer2.size(); j++)
        {
            if (fabs(deltaPhi(layer1[i]->Phi(), layer2[j]->Phi())) > dPhi_cut)
                continue;

            float z = layer1[i]->posZ() - (layer2[j]->posZ() - layer1[i]->posZ()) / (layer2[j]->rho() - layer1[i]->rho()) * layer1[i]->rho();
            vertices.emplace_back(layer1[i]->posX(), layer1[i]->posY(), layer1[i]->posZ(), layer2[j]->posX(), layer2[j]->posY(), layer2[j]->posZ(), z);

            // find the equation connecting two hits in the X-Y plane
            // y = mx + b
            float m = (layer2[j]->posY() - layer1[i]->posY()) / (layer2[j]->posX() - layer1[i]->posX());
            float b = layer1[i]->posY() - m * layer1[i]->posX();
            // calculate the distance of closest approach from the line to (0,0)
            float d = fabs(b) / sqrt(1 + m * m);

            if (d > 0.3)
                continue;

            // for debugging - print out the z and rho of the clusters and the extrpolated z position
            // cout << "layer1[i]->posZ() = " << layer1[i]->posZ() << ", layer1[i]->rho() = " << layer1[i]->rho() << ", layer2[j]->posZ() = " << layer2[j]->posZ() << ", layer2[j]->rho() = " << layer2[j]->rho() << ", z = " << z << endl;

            hM_vtxcandz->Fill(z);

            if (z <= 30. && z >= -30.)
            {
                TLine *l = new TLine(layer2[j]->posZ(), layer2[j]->rho(), z, 0.0);
                v_tlkprojZ.push_back(l);
            }
        }
    }

    /* vertex clustering */
    if (vertices.size())
    {
        // Sort the vertices based on the extrapolated z position
        sort(vertices.begin(), vertices.end(), [](const MvtxClusComb &a, const MvtxClusComb &b) -> bool { return a.extz < b.extz; });
        // sort(vertices.begin(), vertices.end());

        // vertex candidate clusters
        std::vector<Cluster> clusters;
        std::size_t z = 0, y = 0;
        for (; z < vertices.size(); ++z)
        {
            for (; y < vertices.size() && vertices[y].extz - vertices[z].extz < dZ_cut; ++y)
                ;
            clusters.emplace_back(z, y - z); // index, nz
        }

        std::sort(clusters.begin(), clusters.end(), [](const Cluster &a, const Cluster &b) -> bool { return a.nz > b.nz; });

        std::vector<Vertex> candidates;
        uint32_t maxnz = clusters[0].nz;

        for (auto &cluster : clusters)
        {
            uint32_t nz = cluster.nz;
            if (nz + NZGAP < maxnz)
            {
                break;
            }

            uint32_t index = cluster.index;

            float vz = 0;
            float vz2 = 0;
            float sigma2 = 0;
            vector<float> hitl1x, hitl1y, hitl1z, hitl2x, hitl2y, hitl2z, candidatez;

            for (uint32_t y = 0; y < nz; ++y)
            {
                vz += vertices[index + y].extz;
                vz2 += vertices[index + y].extz * vertices[index + y].extz;
                candidatez.push_back(vertices[index + y].extz);
                hitl1x.push_back(vertices[index + y].x1);
                hitl1y.push_back(vertices[index + y].y1);
                hitl1z.push_back(vertices[index + y].z1);
                hitl2x.push_back(vertices[index + y].x2);
                hitl2y.push_back(vertices[index + y].y2);
                hitl2z.push_back(vertices[index + y].z2);
            }

            vz /= nz;
            sigma2 = vz2 / nz - (vz * vz);

            candidates.emplace_back(vz, sigma2, hitl1x, hitl1y, hitl1z, hitl2x, hitl2y, hitl2z, candidatez);
        }

        // PV z position
        vertex[2] = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->vz;
        vector<float> vtxcandz = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->candz;
        vector<float> hit1x = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->hitx1;
        vector<float> hit1y = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->hity1;
        vector<float> hit1z = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->hitz1;
        vector<float> hit2x = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->hitx2;
        vector<float> hit2y = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->hity2;
        vector<float> hit2z = std::min_element(candidates.begin(), candidates.end(), [](const Vertex &a, const Vertex &b) -> bool { return a.sigma2 < b.sigma2; })->hitz2;
        vector<float> vtxcandx, vtxcandy;

        for (size_t i = 0; i < hit1x.size(); i++)
        {
            float extpx = (vertex[2] - hit1z[i]) / (hit1z[i] - hit2z[i]) * (hit1x[i] - hit2x[i]) + hit1x[i];
            float extpy = (vertex[2] - hit1z[i]) / (hit1z[i] - hit2z[i]) * (hit1y[i] - hit2y[i]) + hit1y[i];
            vtxcandx.push_back(extpx);
            vtxcandy.push_back(extpy);

            hM_vtxcandxy->Fill(extpx, extpy);

            TLine *l_tlkprojXY = new TLine(hit2x[i], hit2y[i], extpx, extpy);
            v_tlkprojXY.push_back(l_tlkprojXY);
        }

        // copy vector vtxcandx and vtxcandy to a new vector for sorting
        vector<float> vtxcandx_sorted = vtxcandx;
        vector<float> vtxcandy_sorted = vtxcandy;
        // find the median of the extrapolated x and y positions as the PV_x and PV_y
        std::sort(vtxcandx_sorted.begin(), vtxcandx_sorted.end());
        std::sort(vtxcandy_sorted.begin(), vtxcandy_sorted.end());
        vertex[0] = (vtxcandx_sorted.size() % 2 == 0) ? (vtxcandx_sorted[vtxcandx_sorted.size() / 2 - 1] + vtxcandx_sorted[vtxcandx_sorted.size() / 2]) / 2 : vtxcandx_sorted[vtxcandx_sorted.size() / 2];
        vertex[1] = (vtxcandy_sorted.size() % 2 == 0) ? (vtxcandy_sorted[vtxcandy_sorted.size() / 2 - 1] + vtxcandy_sorted[vtxcandy_sorted.size() / 2]) / 2 : vtxcandy_sorted[vtxcandy_sorted.size() / 2];
        Ncandidates_incluster = vtxcandx.size();

        // fill the histograms
        for (size_t i = 0; i < vtxcandx.size(); i++)
        {
            hM_Zdiff->Fill(vtxcandz[i] - vertex[2]);
            hM_XYdiff->Fill(vtxcandx[i] - vertex[0], vtxcandy[i] - vertex[1]);
            // Calculate the DCA between the (extrapolated) tracklets and the PV in the vertex cluster
            TVector3 v_DCA = DCA_tklPV({hit1x[i], hit1y[i], hit1z[i]}, {hit2x[i], hit2y[i], hit2z[i]}, vertex);
            hM_DCAxy->Fill(v_DCA.XYvector().Mod());
            hM_DCAxy_DCAz->Fill(v_DCA.XYvector().Mod(), v_DCA.Z());
            hM_DCAx_DCAy->Fill(v_DCA.X(), v_DCA.Y());
        }

        // clean up
        CleanVec(clusters);
        CleanVec(candidates);
        CleanVec(vtxcandz);
        CleanVec(hit1x);
        CleanVec(hit1y);
        CleanVec(hit1z);
        CleanVec(hit2x);
        CleanVec(hit2y);
        CleanVec(hit2z);
        CleanVec(vtxcandx);
        CleanVec(vtxcandy);
        CleanVec(vtxcandx_sorted);
        CleanVec(vtxcandy_sorted);
    }

    CleanVec(vertices);

    // setup the tuple
    get<0>(t) = vertex;
    get<1>(t) = Ncandidates_incluster;
    get<2>(t) = hM_ZRho;
    get<3>(t) = hM_XY;
    get<4>(t) = v_tlkprojZ;
    get<5>(t) = v_tlkprojXY;
    get<6>(t) = hM_Zdiff;
    get<7>(t) = hM_XYdiff;
    get<8>(t) = hM_vtxcandz;
    get<9>(t) = hM_vtxcandxy;
    get<10>(t) = hM_DCAxy;
    get<11>(t) = hM_DCAxy_DCAz;
    get<12>(t) = hM_DCAx_DCAy;

    return t;
}

void SetVtxMinitree(TTree *outTree, VtxData &vtxdata)
{
    outTree->Branch("event", &vtxdata.event);
    outTree->Branch("NhitsLayer1", &vtxdata.NhitsLayer1);
    if (!vtxdata.isdata)
    {
        outTree->Branch("NTruthVtx", &vtxdata.NTruthVtx);
        outTree->Branch("TruthPV_trig_x", &vtxdata.TruthPV_trig_x);
        outTree->Branch("TruthPV_trig_y", &vtxdata.TruthPV_trig_y);
        outTree->Branch("TruthPV_trig_z", &vtxdata.TruthPV_trig_z);
        outTree->Branch("TruthPV_mostNpart_x", &vtxdata.TruthPV_mostNpart_x);
        outTree->Branch("TruthPV_mostNpart_y", &vtxdata.TruthPV_mostNpart_y);
        outTree->Branch("TruthPV_mostNpart_z", &vtxdata.TruthPV_mostNpart_z);
        outTree->Branch("Centrality_bimp", &vtxdata.Centrality_bimp);
        outTree->Branch("Centrality_impactparam", &vtxdata.Centrality_impactparam);
    }
    outTree->Branch("Centrality_mbd", &vtxdata.Centrality_mbd);
    outTree->Branch("Centrality_mbdquantity", &vtxdata.Centrality_mbdquantity);
    outTree->Branch("PV_x_halves1", &vtxdata.PV_x_halves1);
    outTree->Branch("PV_y_halves1", &vtxdata.PV_y_halves1);
    outTree->Branch("PV_z_halves1", &vtxdata.PV_z_halves1);
    outTree->Branch("PV_x_halves2", &vtxdata.PV_x_halves2);
    outTree->Branch("PV_y_halves2", &vtxdata.PV_y_halves2);
    outTree->Branch("PV_z_halves2", &vtxdata.PV_z_halves2);
}

std::map<int, float> EvtVtx_map_tklcluster(const char *vtxfname)
{
    std::map<int, float> EvtVtx_map;

    TFile *f = new TFile(vtxfname, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    int event;
    float PV_z_halves1, PV_z_halves2;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("PV_z_halves1", &PV_z_halves1);
    t->SetBranchAddress("PV_z_halves2", &PV_z_halves2);
    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        t->GetEntry(ev);
        // cout << ev << " " << event << " " << PV_z << endl;
        EvtVtx_map[event] = (PV_z_halves1 + PV_z_halves2) / 2.;
    }

    return EvtVtx_map;
}

#endif
