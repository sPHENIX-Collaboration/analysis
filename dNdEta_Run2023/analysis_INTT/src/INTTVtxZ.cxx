#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <TCanvas.h>
#include <TF1.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TKDE.h>
#include <TRandom.h>
#ifndef __CINT__
#include "Math/DistFunc.h"
#endif
#include <TLegend.h>
#include <TLine.h>
#include <TMath.h>
#include <TObjString.h>
#include <TProfile.h>
#include <TRandom3.h>
#include <TText.h>
#include <TTree.h>
#include <TTreeIndex.h>

#include "Hit.h"
#include "Utilities.h"
#include "Vertex.h"

int NevtToPlot = 10;

void draw_demoplot(TH1F *h, TF1 *f, float dcacut, TString plotname)
{
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    h->GetXaxis()->SetTitle("v_{z}^{candidate} segments [cm]");
    h->GetYaxis()->SetTitle("Counts");
    h->GetYaxis()->SetTitleOffset(1.5);
    h->SetMaximum(h->GetMaximum() * 1.2);
    h->SetLineColor(1);
    h->SetLineWidth(2);
    h->Draw("hist");
    f->SetLineColor(kRed);
    f->SetLineWidth(2);
    f->Draw("L same");
    TLegend *leg = new TLegend(0.47, 0.8, 0.9, 0.92);
    leg->SetHeader();
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.035);
    leg->AddEntry((TObject*)0, Form("DCA < %.2f cm", dcacut), "");
    leg->AddEntry((TObject*)0, Form("#mu_{Gaussian} = %.2f #pm %.2f cm", f->GetParameter(1), f->GetParError(1)), "");
    leg->Draw();
    c->SaveAs(Form("%s.pdf", plotname.Data()));
    c->SaveAs(Form("%s.png", plotname.Data()));

    delete c;
}

int main(int argc, char *argv[])
{
    SetsPhenixStyle();
    gStyle->SetPalette(kThermometer);

    bool IsData = (TString(argv[1]).Atoi() == 1) ? true : false;
    int NevtToRun_ = TString(argv[2]).Atoi();
    float avgVtxX = TString(argv[3]).Atof();  // float avgVtxX = -0.0015
    float avgVtxY = TString(argv[4]).Atof();  // avgVtxY = 0.0012;
    float dPhi_cut = TString(argv[5]).Atof(); // Example 0.11;
    float dca_cut = TString(argv[6]).Atof();
    TString infilename = TString(argv[7]);   // /sphenix/user/hjheng/TrackletAna/data/INTT/ana382_zvtx-20cm_dummyAlignParams/sim/INTTRecoClusters_sim_merged.root
    TString outfilepath = TString(argv[8]);  // /sphenix/user/hjheng/TrackletAna/minitree/INTT/VtxEvtMap_ana382_zvtx-20cm_dummyAlignParams
    TString demoplotpath = TString(argv[9]); // ./plot/RecoPV_demo/RecoPV_sim/INTTVtxZ_ana382_zvtx-20cm_dummyAlignParams
    bool debug = (TString(argv[10]).Atoi() == 1) ? true : false;

    system(Form("mkdir -p %s", outfilepath.Data()));
    system(Form("mkdir -p %s", demoplotpath.Data()));

    vector<Hit *> INTTlayer1, INTTlayer2;
    VtxData vtxdata = {};

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex("event"); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NTruthVtx;
    float TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z, centrality_bimp, centrality_impactparam, centrality_mbd, centrality_mbdquantity;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0, *ClusPhiSize = 0, *ClusZSize = 0;
    vector<uint8_t> *ClusLadderZId = 0, *ClusLadderPhiId = 0;
    t->SetBranchAddress("event", &event);
    if (!IsData)
    {
        t->SetBranchAddress("NTruthVtx", &NTruthVtx);
        t->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        t->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        t->SetBranchAddress("centrality_bimp", &centrality_bimp);
        t->SetBranchAddress("centrality_impactparam", &centrality_impactparam);
    }
    t->SetBranchAddress("centrality_mbd", &centrality_mbd);
    t->SetBranchAddress("centrality_mbdquantity", &centrality_mbdquantity);
    t->SetBranchAddress("ClusLayer", &ClusLayer);
    t->SetBranchAddress("ClusX", &ClusX);
    t->SetBranchAddress("ClusY", &ClusY);
    t->SetBranchAddress("ClusZ", &ClusZ);
    t->SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    t->SetBranchAddress("ClusZSize", &ClusZSize);
    t->SetBranchAddress("ClusLadderZId", &ClusLadderZId);
    t->SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId);

    TFile *outfile = new TFile(Form("%s/INTTVtxZ.root", outfilepath.Data()), "RECREATE");
    TTree *minitree = new TTree("minitree", "Minitree of reconstructed vertices");
    SetVtxMinitree(minitree, vtxdata);

    for (int ev = 0; ev < NevtToRun_; ev++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[ev]);
        t->GetEntry(local);
        // cout << "event = " << event << " local = " << local << endl;
        cout << "event=" << event << " has a total of " << ClusLayer->size() << " clusters" << endl;

        CleanVec(INTTlayer1);
        CleanVec(INTTlayer2);

        for (size_t ihit = 0; ihit < ClusLayer->size(); ihit++)
        {
            if (ClusLayer->at(ihit) == 3 || ClusLayer->at(ihit) == 4)
            {
                Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), avgVtxX, avgVtxY, 0., 0);
                // Set edge
                if (ClusLadderZId->at(ihit) == 0 || ClusLadderZId->at(ihit) == 2)
                    hit->SetEdge(ClusZ->at(ihit) - 0.8, ClusZ->at(ihit) + 0.8);
                else if (ClusLadderZId->at(ihit) == 1 || ClusLadderZId->at(ihit) == 3)
                    hit->SetEdge(ClusZ->at(ihit) - 0.5, ClusZ->at(ihit) + 0.5);
                else
                    continue;

                if (ClusPhiSize->at(ihit) > 5)
                    continue;

                INTTlayer1.push_back(hit);
            }
            else if (ClusLayer->at(ihit) == 5 || ClusLayer->at(ihit) == 6)
            {
                Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), avgVtxX, avgVtxY, 0., 1);
                // Set edge
                if (ClusLadderZId->at(ihit) == 0 || ClusLadderZId->at(ihit) == 2)
                    hit->SetEdge(ClusZ->at(ihit) - 0.8, ClusZ->at(ihit) + 0.8);
                else if (ClusLadderZId->at(ihit) == 1 || ClusLadderZId->at(ihit) == 3)
                    hit->SetEdge(ClusZ->at(ihit) - 0.5, ClusZ->at(ihit) + 0.5);
                else
                    continue;

                if (ClusPhiSize->at(ihit) > 5)
                    continue;

                INTTlayer2.push_back(hit);
            }
            else
                continue;
        }

        cout << "# of clusters in 1st layer (layer ID 3+4, after cluster phi size selection) = " << INTTlayer1.size() << ", 2nd layer (layer ID 5+6) = " << INTTlayer2.size() << endl;

        TH1F *hM_vtxzprojseg = new TH1F(Form("hM_vtxzprojseg_ev%d", ev), Form("hM_vtxzprojseg_ev%d", ev), 2800, -70, 70);
        for (size_t i = 0; i < INTTlayer1.size(); i++)
        {
            for (size_t j = 0; j < INTTlayer2.size(); j++)
            {
                if (debug)
                {
                    cout << "----------" << endl
                         << "Cluster pair (i,j) = (" << i << "," << j << "); position (x1,y1,z1,x2,y2,z2)=(" << INTTlayer1[i]->posX() << "," << INTTlayer1[i]->posY() << "," << INTTlayer1[i]->posZ() << "," << INTTlayer2[j]->posX() << ","
                         << INTTlayer2[j]->posY() << "," << INTTlayer2[j]->posZ() << ")" << endl;
                    cout << "Cluster (phi1,eta1,phi2,eta2)=(" << INTTlayer1[i]->Phi() << "," << INTTlayer1[i]->Eta() << "," << INTTlayer2[j]->Phi() << "," << INTTlayer2[j]->Eta() << ")" << endl
                         << "delta phi = " << fabs(deltaPhi(INTTlayer1[i]->Phi(), INTTlayer2[j]->Phi())) << "-> pass dPhi selection (<" << dPhi_cut << " rad)=" << ((fabs(deltaPhi(INTTlayer1[i]->Phi(), INTTlayer2[j]->Phi())) < dPhi_cut) ? 1 : 0)
                         << endl;
                }

                if (fabs(deltaPhi(INTTlayer1[i]->Phi(), INTTlayer2[j]->Phi())) > dPhi_cut)
                    continue;

                // find the equation connecting two hits in the X-Y plane
                // y = mx + b
                float m = (INTTlayer2[j]->posY() - INTTlayer1[i]->posY()) / (INTTlayer2[j]->posX() - INTTlayer1[i]->posX());
                float b = INTTlayer1[i]->posY() - m * INTTlayer1[i]->posX();
                // calculate the distance of closest approach from the line to (avgVtxX, avgVtxY)
                float dca = fabs(m * avgVtxX - avgVtxY + b) / sqrt(m * m + 1);

                // print out the information of clusters
                if (debug)
                    cout << "dca w.r.t (avgVtxX [cm], avgVtxY [cm]) = " << dca << endl;

                if (dca > dca_cut)
                    continue;

                float rho1 = sqrt(pow((INTTlayer1[i]->posX() - avgVtxX), 2) + pow((INTTlayer1[i]->posY() - avgVtxY), 2));
                float rho2 = sqrt(pow((INTTlayer2[j]->posX() - avgVtxX), 2) + pow((INTTlayer2[j]->posY() - avgVtxY), 2));
                float z = INTTlayer1[i]->posZ() - (INTTlayer2[j]->posZ() - INTTlayer1[i]->posZ()) / (rho2 - rho1) * rho1;
                float edge1 = INTTlayer1[i]->Edge().first - (INTTlayer2[j]->Edge().second - INTTlayer1[i]->Edge().first) / (rho2 - rho1) * rho1;
                float edge2 = INTTlayer1[i]->Edge().second - (INTTlayer2[j]->Edge().first - INTTlayer1[i]->Edge().second) / (rho2 - rho1) * rho1;

                if (debug)
                    cout << "DCA cut = " << dca_cut << " [cm]; vertex candidate (center,edge1,edge2) = (" << z << "," << edge1 << "," << edge2 << "), difference = " << edge2 - edge1 << endl;
                
                if (fabs(z) < 70)
                {
                    int bin1 = hM_vtxzprojseg->FindBin(edge1);
                    int bin2 = hM_vtxzprojseg->FindBin(edge2);

                    for (int ibin = bin1; ibin <= bin2; ibin++)
                    {
                        float bincenter = hM_vtxzprojseg->GetBinCenter(ibin);
                        float binwidth = hM_vtxzprojseg->GetBinWidth(ibin);
                        // for bin1 and bin2, find the distance of the edge1 and edge2 to the respective bin edge
                        float w;
                        if (ibin == bin1)
                            w = (bincenter + 0.5 * binwidth) - edge1;
                        else if (ibin == bin2)
                            w = edge2 - (bincenter - 0.5 * binwidth);
                        else
                            w = 1.;

                        hM_vtxzprojseg->Fill(bincenter, w);
                        // cout << "bincenter = " << bincenter << ", bincontent = " << bincontent << endl;
                    }
                }
            }
        }

        if (debug && !IsData)
            cout << "Event " << ev << "(Truth PVx, Truth PVy, Truth PVz) = (" << TruthPV_trig_x << ", " << TruthPV_trig_y << ", " << TruthPV_trig_z << ")" << endl;

        // find the maximum bin of the histogram hM_vtxzedge_dca[2] and fit the histogram with a Gaussian function around the maximum bin
        float maxbincenter = hM_vtxzprojseg->GetBinCenter(hM_vtxzprojseg->GetMaximumBin());
        TF1 *f1 = new TF1("f1", "[0]*exp(-0.5*((x-[1])/[2])^2) + [3] + [4]*x", maxbincenter - 7, maxbincenter + 7);
        f1->SetParName(0, "Norm");
        f1->SetParName(1, "#mu");
        f1->SetParName(2, "#sigma");
        f1->SetParName(3, "p0");
        f1->SetParName(4, "p1");
        f1->SetParameter(0, hM_vtxzprojseg->GetMaximum());
        f1->SetParameter(1, maxbincenter);
        f1->SetParameter(2, 5);
        f1->SetParLimits(0, hM_vtxzprojseg->GetMaximum() * 0.01, hM_vtxzprojseg->GetMaximum() * 100);
        f1->SetParLimits(1, maxbincenter - 10, maxbincenter + 10);
        f1->SetParLimits(2, 0.1, 100);
        hM_vtxzprojseg->Fit("f1", "R");

        float mean = f1->GetParameter(1);

        if (ev < NevtToPlot)
        {
            TString pname = Form("%s/hM_vtxzprojseg_ev%d", demoplotpath.Data(), event);
            draw_demoplot(hM_vtxzprojseg, f1, dca_cut, pname);
        }

        vtxdata.isdata = IsData;
        vtxdata.event = event;
        if (!IsData)
        {
            vtxdata.NTruthVtx = NTruthVtx;
            vtxdata.TruthPV_x = TruthPV_trig_x;
            vtxdata.TruthPV_y = TruthPV_trig_y;
            vtxdata.TruthPV_z = TruthPV_trig_z;
            vtxdata.Centrality_bimp = centrality_bimp;
            vtxdata.Centrality_impactparam = centrality_impactparam;
        }
        vtxdata.NClusLayer1 = (int)INTTlayer1.size();
        vtxdata.PV_x = avgVtxX;
        vtxdata.PV_y = avgVtxY;
        vtxdata.PV_z = mean;
        vtxdata.Centrality_mbd = centrality_mbd;
        vtxdata.Centrality_mbdquantity = centrality_mbdquantity;

        minitree->Fill();
    }

    outfile->cd();
    minitree->Write();
    outfile->Close();

    f->Close();

    return 0;
}
