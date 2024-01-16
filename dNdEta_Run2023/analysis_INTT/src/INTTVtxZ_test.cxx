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

int main(int argc, char *argv[])
{
    SetsPhenixStyle();
    gStyle->SetPalette(kThermometer);

    // Average vertex X and Y positions in cm
    float avgVtxX = -0.0015, avgVtxY = 0.0012;

    bool IsData = false;
    TString infilename = "/sphenix/user/hjheng/TrackletAna/data/INTT/ana382_zvtx-20cm/INTTRecoClusters_sim_merged.root";
    // TString infilename = "/sphenix/user/hjheng/TrackletAna/data/INTT/ana376_zvtx0cm/INTTRecoClusters_merged.root";
    TString outfilename = "/sphenix/user/hjheng/TrackletAna/minitree/INTT/VtxEvtMap_ana382_zvtx-20cm/INTTVtxZ.root";
    // TString outfilename = "/sphenix/user/hjheng/TrackletAna/minitree/INTT/ana376_zvtx0cm/INTTVtxZ.root";
    TString demoplotname = "./plot/RecoPV_demo/RecoPV_Sim/INTTVtxZ_ana382_zvtx-20cm";
    // TString demoplotname = "./plot/RecoPV_demo/RecoPV_Sim/INTTVtxZ_ana376_zvtx0cm";
    system("mkdir -p " + demoplotname);

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
    t->SetBranchAddress("NTruthVtx", &NTruthVtx);
    t->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
    t->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
    t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
    t->SetBranchAddress("centrality_bimp", &centrality_bimp);
    t->SetBranchAddress("centrality_impactparam", &centrality_impactparam);
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

    TFile *outfile = new TFile(outfilename, "RECREATE");
    TTree *minitree = new TTree("minitree", "Minitree of reconstructed vertices");
    SetVtxMinitree(minitree, vtxdata);

    float dPhi_cut = 0.1;
    TH1F *hM_vtxzreso = new TH1F("hM_vtxzreso", "hM_vtxzreso", 100, -5, 5);
    TH2F *hM_truth_recotruthdiff = new TH2F("hM_truth_recotruthdiff", "hM_truth_recotruthdiff", 100, -50, 0, 100, -5, 5);

    // for (int ev = 0; ev < index->GetN(); ev++)
    for (int ev = 0; ev < 2000; ev++)
    // for (Long64_t ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[ev]);
        t->GetEntry(local);
        // cout << "event = " << event << " local = " << local << endl;
        cout << "event=" << event << " has a total of " << ClusLayer->size() << " clusters" << endl;

        // if (ClusLayer->size() < 20)
        //     continue;

        CleanVec(INTTlayer1);
        CleanVec(INTTlayer2);

        for (size_t ihit = 0; ihit < ClusLayer->size(); ihit++)
        {
            if (ClusLayer->at(ihit) == 3 || ClusLayer->at(ihit) == 4)
            {
                Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), 0., 0., 0., 0);
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
                Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), 0., 0., 0., 1);
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

        vector<float> dca_cuts = {0.01, 0.03, 0.05, 0.1, 0.2};
        vector<TH1F *> hM_vtxcandz_dca, hM_vtxzedge_dca;
        for (size_t i = 0; i < dca_cuts.size(); i++)
        {
            TH1F *hM_vtxcandz = new TH1F(Form("hM_vtxcandz_ev%d_dcacut%d", ev, i), Form("hM_vtxcandz_ev%d_dcacut%d", ev, i), 50, -70, 70);
            hM_vtxcandz_dca.push_back(hM_vtxcandz);
            TH1F *hM_vtxzedgproj = new TH1F(Form("hM_vtxzedgproj_ev%d_dcacut%d", ev, i), Form("hM_vtxzedgproj_ev%d_dcacut%d", ev, i), 2800, -70, 70);
            hM_vtxzedge_dca.push_back(hM_vtxzedgproj);
        }

        cout << "# of clusters in Layer 1 = " << INTTlayer1.size() << ", Layer 2 = " << INTTlayer2.size() << endl;

        int Ntkl = 0;
        for (size_t i = 0; i < INTTlayer1.size(); i++)
        {
            for (size_t j = 0; j < INTTlayer2.size(); j++)
            {
                if (fabs(deltaPhi(INTTlayer1[i]->Phi(), INTTlayer2[j]->Phi())) > dPhi_cut)
                    continue;

                // find the equation connecting two hits in the X-Y plane
                // y = mx + b
                float m = (INTTlayer2[j]->posY() - INTTlayer1[i]->posY()) / (INTTlayer2[j]->posX() - INTTlayer1[i]->posX());
                float b = INTTlayer1[i]->posY() - m * INTTlayer1[i]->posX();
                // calculate the distance of closest approach from the line to (avgVtxX, avgVtxY)
                float dca = fabs(m * avgVtxX - avgVtxY + b) / sqrt(m * m + 1);

                for (size_t k = 0; k < dca_cuts.size(); k++)
                {
                    if (dca < dca_cuts[k])
                    {
                        float z = INTTlayer1[i]->posZ() - (INTTlayer2[j]->posZ() - INTTlayer1[i]->posZ()) / (INTTlayer2[j]->rho() - INTTlayer1[i]->rho()) * INTTlayer1[i]->rho();
                        // extrapolate the edge of the hit
                        // the right edge is the extrapolation of the left edge of the hit in the outer layer and the right edge of the hit in the inner layer
                        // the left edge is the extrapolation of the right edge of the hit in the outer layer and the left edge of the hit in the inner layer
                        float edge1 = INTTlayer1[i]->Edge().first - (INTTlayer2[j]->Edge().second - INTTlayer1[i]->Edge().first) / (INTTlayer2[j]->rho() - INTTlayer1[i]->rho()) * INTTlayer1[i]->rho();
                        float edge2 = INTTlayer1[i]->Edge().second - (INTTlayer2[j]->Edge().first - INTTlayer1[i]->Edge().second) / (INTTlayer2[j]->rho() - INTTlayer1[i]->rho()) * INTTlayer1[i]->rho();
                        // print out the information of clusters
                        if (fabs(z) < 70)
                        {
                            // cout << "INTTlayer1[" << i << "] (posZ,rho,edge1,edge2)= (" << INTTlayer1[i]->posZ() << "," << INTTlayer1[i]->rho() << "," << INTTlayer1[i]->Edge().first << "," << INTTlayer1[i]->Edge().second << ")" << endl;
                            // cout << "INTTlayer2[" << j << "] (posZ,rho,edge1,edge2)= (" << INTTlayer2[j]->posZ() << "," << INTTlayer2[j]->rho() << "," << INTTlayer2[j]->Edge().first << "," << INTTlayer2[j]->Edge().second << ")" << endl;
                            // cout << "Vertex candidate (center,edge1,edge2) = (" << z << "," << edge1 << "," << edge2 << "), difference = " << edge2 - edge1 << endl;

                            // find the bin number of the edge1 and edge2
                            int bin1 = hM_vtxzedge_dca[k]->FindBin(edge1);
                            int bin2 = hM_vtxzedge_dca[k]->FindBin(edge2);
                            // cout << "bin1 = " << bin1 << ", bin2 = " << bin2 << endl;

                            // fill the histogram
                            for (int ibin = bin1; ibin <= bin2; ibin++)
                            {
                                float bincenter = hM_vtxzedge_dca[k]->GetBinCenter(ibin);
                                float binwidth = hM_vtxzedge_dca[k]->GetBinWidth(ibin);
                                // for bin1 and bin2, find the distance of the edge1 and edge2 to the respective bin edge
                                float w;
                                if (ibin == bin1)
                                    w = (bincenter + 0.5 * binwidth) - edge1;
                                else if (ibin == bin2)
                                    w = edge2 - (bincenter - 0.5 * binwidth);
                                else
                                    w = 1.;
                                hM_vtxzedge_dca[k]->Fill(bincenter, w);
                                // cout << "bincenter = " << bincenter << ", bincontent = " << bincontent << endl;
                            }
                        }

                        hM_vtxcandz_dca[k]->Fill(z);
                    }
                }

                // if (dca > 0.3)
                //     continue;

                // float z = INTTlayer1[i]->posZ() - (INTTlayer2[j]->posZ() - INTTlayer1[i]->posZ()) / (INTTlayer2[j]->rho() - INTTlayer1[i]->rho()) * INTTlayer1[i]->rho();
                // hM_vtxcandz->Fill(z);
                // cout << "Ntkl = " << Ntkl << " z = " << z << endl;

                // Ntkl++;
            }
        }

        cout << "Event " << ev << "(Truth PVx, Truth PVy, Truth PVz) = (" << TruthPV_trig_x << ", " << TruthPV_trig_y << ", " << TruthPV_trig_z << ")" << endl;

        // find the maximum bin of the histogram hM_vtxzedge_dca[2] and fit the histogram with a Gaussian function around the maximum bin
        int fitbin = 4;
        float maxbincenter = hM_vtxzedge_dca[fitbin]->GetBinCenter(hM_vtxzedge_dca[fitbin]->GetMaximumBin());
        TF1 *f1 = new TF1("f1", "[0]*exp(-0.5*((x-[1])/[2])^2) + [3] + [4]*x", maxbincenter - 7, maxbincenter + 7);
        f1->SetParName(0, "Norm");
        f1->SetParName(1, "#mu");
        f1->SetParName(2, "#sigma");
        f1->SetParName(3, "p0");
        f1->SetParName(4, "p1");
        f1->SetParameter(0, hM_vtxzedge_dca[fitbin]->GetMaximum());
        f1->SetParameter(1, maxbincenter);
        f1->SetParameter(2, 5);
        f1->SetParLimits(0, hM_vtxzedge_dca[fitbin]->GetMaximum() * 0.01, hM_vtxzedge_dca[fitbin]->GetMaximum() * 100);
        f1->SetParLimits(1, maxbincenter - 5, maxbincenter + 5);
        f1->SetParLimits(2, 0.1, 10);
        hM_vtxzedge_dca[fitbin]->Fit("f1", "R");
        float mean = f1->GetParameter(1);
        float sigma = f1->GetParameter(2);
        float mean_err = f1->GetParError(1);
        float sigma_err = f1->GetParError(2);

        if (ev < 0)
        {
            TCanvas *c = new TCanvas("c", "c", 800, 700);
            // c->cd();
            // // get the maximum bin content
            // float max = 0;
            // for (size_t i = 0; i < dca_cuts.size(); i++)
            // {
            //     if (hM_vtxcandz_dca[i]->GetMaximum() > max)
            //         max = hM_vtxcandz_dca[i]->GetMaximum();
            // }
            // hM_vtxcandz_dca[0]->GetXaxis()->SetTitle("v_{z}^{candidate} [cm]");
            // hM_vtxcandz_dca[0]->GetYaxis()->SetTitle("Counts");
            // hM_vtxcandz_dca[0]->GetYaxis()->SetTitleOffset(1.5);
            // hM_vtxcandz_dca[0]->SetMaximum(max * 1.2);
            // hM_vtxcandz_dca[0]->SetLineWidth(2);
            // hM_vtxcandz_dca[0]->Draw("hist PLC");
            // for (size_t i = 1; i < dca_cuts.size(); i++)
            // {
            //     hM_vtxcandz_dca[i]->SetLineWidth(2);
            //     hM_vtxcandz_dca[i]->Draw("hist PLC same");
            // }
            // TLegend *leg = new TLegend(0.53, 0.7, 0.9, 0.9);
            // leg->SetBorderSize(0);
            // leg->SetFillStyle(0);
            // leg->SetTextSize(0.035);
            // for (size_t i = 0; i < dca_cuts.size(); i++)
            //     leg->AddEntry(hM_vtxcandz_dca[i], Form("DCA < %.2f cm", dca_cuts[i]), "l");
            // leg->Draw();
            // c->SaveAs(Form("%s/hM_vtxcandz_ev%d.pdf", demoplotname.Data(), ev));
            // c->SaveAs(Form("%s/hM_vtxcandz_ev%d.png", demoplotname.Data(), ev));

            c->cd();
            // get the maximum bin content
            float max = 0;
            for (size_t i = 0; i < dca_cuts.size(); i++)
            {
                if (hM_vtxzedge_dca[i]->GetMaximum() > max)
                    max = hM_vtxzedge_dca[i]->GetMaximum();
            }
            hM_vtxzedge_dca[0]->GetXaxis()->SetTitle("v_{z}^{candidate} segments [cm]");
            hM_vtxzedge_dca[0]->GetYaxis()->SetTitle("Counts");
            hM_vtxzedge_dca[0]->GetYaxis()->SetTitleOffset(1.5);
            hM_vtxzedge_dca[0]->SetMaximum(max * 1.2);
            hM_vtxzedge_dca[0]->SetLineWidth(2);
            hM_vtxzedge_dca[0]->Draw("hist PLC");
            for (size_t i = 1; i < dca_cuts.size(); i++)
            {
                hM_vtxzedge_dca[i]->SetLineWidth(2);
                hM_vtxzedge_dca[i]->Draw("hist PLC same");
            }
            TLegend *leg = new TLegend(0.53, 0.7, 0.9, 0.9);
            leg->SetBorderSize(0);
            leg->SetFillStyle(0);
            leg->SetTextSize(0.035);
            for (size_t i = 0; i < dca_cuts.size(); i++)
                leg->AddEntry(hM_vtxzedge_dca[i], Form("DCA < %.2f cm", dca_cuts[i]), "l");
            leg->Draw();

            f1->Draw("L same");
            // TLatex to display the mean and sigma of the Gaussian fit
            TLatex *tl = new TLatex(0.55, 0.67, Form("#mu_{Gaussian} = %.2f #pm %.2f cm", mean, mean_err));
            tl->SetNDC();
            tl->SetTextSize(0.035);
            tl->Draw();
            c->SaveAs(Form("%s/hM_vtxzedge_ev%d.pdf", demoplotname.Data(), ev));
            c->SaveAs(Form("%s/hM_vtxzedge_ev%d.png", demoplotname.Data(), ev));

            delete c;
        }

        hM_vtxzreso->Fill(mean - TruthPV_trig_z);
        hM_truth_recotruthdiff->Fill(TruthPV_trig_z, mean - TruthPV_trig_z);

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

    TCanvas *c1 = new TCanvas("c1", "c1", 800, 700);
    c1->cd();
    hM_vtxzreso->GetXaxis()->SetTitle("v_{z}^{reco} - v_{z}^{truth} [cm]");
    hM_vtxzreso->GetYaxis()->SetTitle("Counts");
    hM_vtxzreso->GetYaxis()->SetTitleOffset(1.5);
    hM_vtxzreso->SetLineWidth(2);
    hM_vtxzreso->Draw("hist");
    // fit the histogram with a Gaussian function
    TF1 *f1 = new TF1("f1", "gaus", hM_vtxzreso->GetMean() - 0.6 * hM_vtxzreso->GetRMS(), hM_vtxzreso->GetMean() + 0.6 * hM_vtxzreso->GetRMS());
    f1->SetLineColor(kRed);
    hM_vtxzreso->Fit("f1", "R");
    f1->Draw("L same");
    float mean = f1->GetParameter(1);
    float sigma = f1->GetParameter(2);
    float mean_err = f1->GetParError(1);
    float sigma_err = f1->GetParError(2);
    // TLatex to display the mean and sigma of the Gaussian fit
    TLatex *tl1 = new TLatex(0.6, 0.67, Form("#mu_{Gaussian} = %.2f #pm %.2f cm", mean, mean_err));
    tl1->SetNDC();
    tl1->SetTextSize(0.035);
    tl1->Draw();
    TLatex *tl2 = new TLatex(0.6, 0.62, Form("#sigma_{Gaussian} = %.2f #pm %.2f cm", sigma, sigma_err));
    tl2->SetNDC();
    tl2->SetTextSize(0.035);
    tl2->Draw();
    c1->SaveAs(Form("%s/hM_vtxzreso.pdf", demoplotname.Data()));
    c1->SaveAs(Form("%s/hM_vtxzreso.png", demoplotname.Data()));

    TCanvas *c2 = new TCanvas("c2", "c2", 800, 700);
    gPad->SetRightMargin(0.15);
    c2->cd();
    hM_truth_recotruthdiff->GetXaxis()->SetTitle("v_{z}^{truth} [cm]");
    hM_truth_recotruthdiff->GetYaxis()->SetTitle("v_{z}^{reco} - v_{z}^{truth} [cm]");
    hM_truth_recotruthdiff->Draw("colz");
    c2->SaveAs(Form("%s/hM_truth_recotruthdiff.pdf", demoplotname.Data()));
    c2->SaveAs(Form("%s/hM_truth_recotruthdiff.png", demoplotname.Data()));

    outfile->cd();
    minitree->Write();
    outfile->Close();

    f->Close();

    return 0;
}
