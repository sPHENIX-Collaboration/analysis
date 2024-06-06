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
#include "misalignment.h"

float precision(float f, int places)
{
    float n = std::pow(10.0f, places);
    return std::round(f * n) / n;
}

int main(int argc, char *argv[])
{
    bool IsData = (TString(argv[1]).Atoi() == 1) ? true : false;
    int dPhiCutbin = TString(argv[2]).Atoi();
    int dZCutbin = TString(argv[3]).Atoi();
    float gap_north = TString(argv[4]).Atof();  // Nominal: 2.9mm
    float gap_upper = TString(argv[5]).Atof();  // Nominal: (gap_north / 2.) mm
    float cent_shift = TString(argv[6]).Atof(); // Nominal: 0.0mm
    TString gap_north_str = TString(argv[4]).Replace(TString(argv[4]).Index("."), 1, "p");
    TString gap_upper_str = TString(argv[5]).Replace(TString(argv[5]).Index("."), 1, "p");
    TString cent_shift_str = TString(argv[6]).Replace(TString(argv[6]).Index("."), 1, "p");

    // float dalpha_num = (gap_north-1.5)/2.;

    SetsPhenixStyle();

    TString infilename = "";
    TString outfilename = "";
    TString demoplotname = "";
    if (IsData)
    {
        cout << "Running on data" << endl;
        // Field Off - Mock Data
        // infilename = "/sphenix/user/hjheng/TrackletAna/data/MVTXRecoClusters/HijingMBwoPUB0_private/MockData/MVTXRecoClusters_HijingMBwoPU0T_MockData.root";
        // outfilename = Form("/sphenix/user/hjheng/TrackletAna/minitree/HijingAuAuMB_NoPileup_0T_RecoVtx_Optimization_MockData/TrackletAna_RecoClusters_RecoVtx_TklCluster_dPhiCutbin%d_dZCutbin%d_GapNorth%s_GapUpper%s_CentShift%s_3DVertex_twohalves.root", dPhiCutbin, dZCutbin, gap_north_str.Data(), gap_upper_str.Data(), cent_shift_str.Data());

        // Real collisions - Run 20124
        infilename = "/sphenix/user/hjheng/macros/detectors/sPHENIX/OutFile/ClusterTree/ClusterTree_Run20124_0.root";
        // infilename = "/sphenix/user/hjheng/macros/detectors/sPHENIX/OutFile/ClusterTree/ClusterTree_Run20124_0_larger_subset.root";
        outfilename = Form("/sphenix/user/hjheng/TrackletAna/minitree/Data_Run2023_AuAu/TrackletAna_RecoClusters_RecoVtx_TklCluster_dPhiCutbin%d_dZCutbin%d_GapNorth%s_GapUpper%s_CentShift%s_3DVertex_twohalves.root", dPhiCutbin, dZCutbin, gap_north_str.Data(), gap_upper_str.Data(), cent_shift_str.Data());
        // outfilename = Form("/sphenix/user/hjheng/TrackletAna/minitree/Data_Run2023_AuAu_larger_subset/TrackletAna_RecoClusters_RecoVtx_TklCluster_dPhiCutbin%d_dZCutbin%d_GapNorth%s_GapUpper%s_CentShift%s_3DVertex_twohalves.root", dPhiCutbin, dZCutbin, gap_north_str.Data(), gap_upper_str.Data(), cent_shift_str.Data());
        demoplotname = "./plot/RecoPV_demo/RecoPV_Data/Run20124";
        // demoplotname = "./plot/RecoPV_demo/RecoPV_Data/Run20124_larger_subset";
    }
    else
    {
        cout << "Running on MC" << endl;

        // Field Off - Mock Sim
        // infilename = "/sphenix/user/hjheng/TrackletAna/data/MVTXRecoClusters/HijingMBwoPUB0_private/MockSim/MVTXRecoClusters_HijingMBwoPU0T_MockSim.root";
        // outfilename = Form("/sphenix/user/hjheng/TrackletAna/minitree/HijingAuAuMB_NoPileup_0T_RecoVtx_Optimization_MockSim/TrackletAna_RecoClusters_RecoVtx_TklCluster_dPhiCutbin%d_dZCutbin%d_GapNorth%s_GapUpper%s_CentShift%s_3DVertex_twohalves.root", dPhiCutbin, dZCutbin, gap_north_str.Data(), gap_upper_str.Data(), cent_shift_str.Data());

        // Field On - Mock Sim
        infilename = "/sphenix/user/hjheng/TrackletAna/data/MVTXRecoClusters/NoPileup_Nevt500_ana325private_singleEvtDst/MVTXRecoClusters.root";
        outfilename = Form("/sphenix/user/hjheng/TrackletAna/minitree/AuAu_ana325private_NoPileup_RecoVtx_Optimization/TrackletAna_RecoClusters_RecoVtx_TklCluster_dPhiCutbin%d_dZCutbin%d_GapNorth%s_GapUpper%s_CentShift%s_3DVertex_twohalves.root", dPhiCutbin, dZCutbin, gap_north_str.Data(), gap_upper_str.Data(), cent_shift_str.Data());
        demoplotname = "./plot/RecoPV_demo/RecoPV_Sim/AuAuHijing_FieldOn";

        // Simple pion events
        // infilename = "/sphenix/user/hjheng/TrackletAna/data/MVTXRecoClusters/Pion/Ntuple_Pions_Npart500_VtxZmean0cm_VtxZwidth0cm_pT0p001to5GeV.root";
        // outfilename = Form("/sphenix/user/hjheng/TrackletAna/minitree/Pion_RecoVtx_Optimization/SimplePion_Npart500_VtxZmean0cm_VtxZwidth0cm_pT0p001to5GeV/TrackletAna_RecoClusters_SimplePion_RecoVtx_TklCluster_dPhiCutbin%d_dZCutbin%d.root", dPhiCutbin, dZCutbin);
    }

    vector<Hit *> MVTXlayer1_halves1, MVTXlayer1_halves2, MVTXlayer2_halves1, MVTXlayer2_halves2;

    VtxData vtxdata = {};

    TFile *f = new TFile(infilename, "READ");
    TTree *t = (TTree *)f->Get("EventTree");
    t->BuildIndex("event"); // Reference: https://root-forum.cern.ch/t/sort-ttree-entries/13138
    TTreeIndex *index = (TTreeIndex *)t->GetTreeIndex();
    int event, NTruthVtx;
    float TruthPV_trig_x, TruthPV_trig_y, TruthPV_trig_z, TruthPV_mostNpart_x, TruthPV_mostNpart_y, TruthPV_mostNpart_z;
    float centrality_bimp, centrality_impactparam, centrality_mbd, centrality_mbdquantity;
    vector<int> *ClusLayer = 0;
    vector<float> *ClusX = 0, *ClusY = 0, *ClusZ = 0;
    t->SetBranchAddress("event", &event);
    if (!IsData)
    {
        t->SetBranchAddress("NTruthVtx", &NTruthVtx);
        t->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        t->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        t->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        t->SetBranchAddress("TruthPV_mostNpart_x", &TruthPV_mostNpart_x);
        t->SetBranchAddress("TruthPV_mostNpart_y", &TruthPV_mostNpart_y);
        t->SetBranchAddress("TruthPV_mostNpart_z", &TruthPV_mostNpart_z);
        // t->SetBranchAddress("centrality_bimp", &centrality_bimp);
        // t->SetBranchAddress("centrality_impactparam", &centrality_impactparam);
    }
    // t->SetBranchAddress("centrality_mbd", &centrality_mbd);
    // t->SetBranchAddress("centrality_mbdquantity", &centrality_mbdquantity);
    t->SetBranchAddress("ClusLayer", &ClusLayer);
    t->SetBranchAddress("ClusX", &ClusX);
    t->SetBranchAddress("ClusY", &ClusY);
    t->SetBranchAddress("ClusZ", &ClusZ);

    TFile *outfile = new TFile(outfilename, "RECREATE");
    TTree *minitree = new TTree("minitree", "Minitree of reconstructed vertices");
    SetVtxMinitree(minitree, vtxdata);

    for (int i = 0; i < index->GetN(); i++)
    // for (int i = 0; i < 1; i++)
    // for (Long64_t ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        Long64_t local = t->LoadTree(index->GetIndex()[i]);
        t->GetEntry(local);
        // cout << "event = " << event << " local = " << local << endl;
        cout << "event=" << event << " has a total of " << ClusLayer->size() << " clusters" << endl;

        CleanVec(MVTXlayer1_halves1);
        CleanVec(MVTXlayer1_halves2);
        CleanVec(MVTXlayer2_halves1);
        CleanVec(MVTXlayer2_halves2);

        for (size_t ihit = 0; ihit < ClusLayer->size(); ihit++)
        {
            // Misalignment study for MVTX
            float x0 = ClusX->at(ihit);
            float y0 = ClusY->at(ihit);
            float z0 = ClusZ->at(ihit);
            vector<float> cpos = {x0, y0, z0};
            UpdatePos_GapTwoHalves(cpos, gap_north, cent_shift, gap_upper);

            if (ClusLayer->at(ihit) == 0)
            {
                Hit *hit = new Hit(cpos[0], cpos[1], cpos[2], 0., 0., 0., 0);
                // Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), 0., 0., 0., 0);
                if (x0 > 0)
                {
                    MVTXlayer1_halves1.push_back(hit);
                }

                else
                    MVTXlayer1_halves2.push_back(hit);
            }
            else if (ClusLayer->at(ihit) == 1)
            {
                Hit *hit = new Hit(cpos[0], cpos[1], cpos[2], 0., 0., 0., 1);
                // Hit *hit = new Hit(ClusX->at(ihit), ClusY->at(ihit), ClusZ->at(ihit), 0., 0., 0., 1);
                if (x0 > 0)
                    MVTXlayer2_halves1.push_back(hit);
                else
                    MVTXlayer2_halves2.push_back(hit);
            }
            else
                continue;
        }

        // cout << "# of clusters in Layer 1 = " << MVTXlayer1.size() << ", Layer 2 = " << MVTXlayer2.size() << endl;
        cout << "# of clusters in Layer 1 = " << MVTXlayer1_halves1.size() + MVTXlayer1_halves2.size() << ", Layer 2 = " << MVTXlayer2_halves1.size() + MVTXlayer2_halves2.size() << endl;

        // float PV_z = TrackletPV_cluster(event, MVTXlayer1, MVTXlayer2, dPhiCutbin, dZCutbin, false);
        // Tracklet3DPV(int evt, vector<Hit *> layer1, vector<Hit *> layer2, int dPhiCutbin, int dZCutbin, const char *plotname)
        // vector<float> PV_halves1 = Tracklet3DPV(event, MVTXlayer1_halves1, MVTXlayer2_halves1, dPhiCutbin, dZCutbin, Form("./plot/RecoPV_Data/Run20124_Evt%d_RecoPV_halve1", event));
        // vector<float> PV_halves2 = Tracklet3DPV(event, MVTXlayer1_halves2, MVTXlayer2_halves2, dPhiCutbin, dZCutbin, Form("./plot/RecoPV_Data/Run20124_Evt%d_RecoPV_halve2", event));
        // tuples for (1) vector of PV estimates, (2) TH2F of cluster positions in Z-Rho, (3) TH2F of cluster positions in X-Y, (4) vector of TLines in Z-Rho, (5) vector of TLines in X-Y, (6) TH1F of difference between extrapolated z and PV z, (7) TH2F of difference between extrapolated x/y and PV x/y
        tuple<vector<float>, int, TH2F *, TH2F *, vector<TLine *>, vector<TLine *>, TH1F *, TH2F *, TH1F *, TH2F *, TH1F *, TH2F *, TH2F *> tuple_PVest1 = Tracklet3DPV(MVTXlayer1_halves1, MVTXlayer2_halves1, dPhiCutbin, dZCutbin);
        tuple<vector<float>, int, TH2F *, TH2F *, vector<TLine *>, vector<TLine *>, TH1F *, TH2F *, TH1F *, TH2F *, TH1F *, TH2F *, TH2F *> tuple_PVest2 = Tracklet3DPV(MVTXlayer1_halves2, MVTXlayer2_halves2, dPhiCutbin, dZCutbin);
        vector<float> PV_halves1 = get<0>(tuple_PVest1);
        vector<float> PV_halves2 = get<0>(tuple_PVest2);
        int Nvtxcandidates_halves1 = get<1>(tuple_PVest1);
        int Nvtxcandidates_halves2 = get<1>(tuple_PVest2);
        TH2F *hM_clusterzrho_halves1 = get<2>(tuple_PVest1);
        TH2F *hM_clusterzrho_halves2 = get<2>(tuple_PVest2);
        TH2F *hM_clusterxy_halves1 = get<3>(tuple_PVest1);
        TH2F *hM_clusterxy_halves2 = get<3>(tuple_PVest2);
        vector<TLine *> tl_zrho_halves1 = get<4>(tuple_PVest1);
        vector<TLine *> tl_zrho_halves2 = get<4>(tuple_PVest2);
        vector<TLine *> tl_xy_halves1 = get<5>(tuple_PVest1);
        vector<TLine *> tl_xy_halves2 = get<5>(tuple_PVest2);
        TH1F *hM_diffExptzPVz_halves1 = get<6>(tuple_PVest1);
        TH1F *hM_diffExptzPVz_halves2 = get<6>(tuple_PVest2);
        TH2F *hM_diffExptxyPVxy_halves1 = get<7>(tuple_PVest1);
        TH2F *hM_diffExptxyPVxy_halves2 = get<7>(tuple_PVest2);
        TH1F *hM_VtxCandZ_halves1 = get<8>(tuple_PVest1);
        TH1F *hM_VtxCandZ_halves2 = get<8>(tuple_PVest2);
        TH2F *hM_VtxCandXY_halves1 = get<9>(tuple_PVest1);
        TH2F *hM_VtxCandXY_halves2 = get<9>(tuple_PVest2);
        TH1F *hM_DCAxy_VtxClus_halves1 = get<10>(tuple_PVest1);
        TH1F *hM_DCAxy_VtxClus_halves2 = get<10>(tuple_PVest2);
        TH2F *hM_DCAxy_DCAz_VtxClus_halves1 = get<11>(tuple_PVest1);
        TH2F *hM_DCAxy_DCAz_VtxClus_halves2 = get<11>(tuple_PVest2);
        TH2F *hM_DCAx_DCAy_VtxClus_halves1 = get<12>(tuple_PVest1);
        TH2F *hM_DCAx_DCAy_VtxClus_halves2 = get<12>(tuple_PVest2);

        // weighted average of the PV z position from the 1st and 2nd halves
        float PV_x = (PV_halves1[0] * Nvtxcandidates_halves1 + PV_halves2[0] * Nvtxcandidates_halves2) / (Nvtxcandidates_halves1 + Nvtxcandidates_halves2);
        float PV_y = (PV_halves1[1] * Nvtxcandidates_halves1 + PV_halves2[1] * Nvtxcandidates_halves2) / (Nvtxcandidates_halves1 + Nvtxcandidates_halves2);
        float PV_z = (PV_halves1[2] * Nvtxcandidates_halves1 + PV_halves2[2] * Nvtxcandidates_halves2) / (Nvtxcandidates_halves1 + Nvtxcandidates_halves2);

        if (MVTXlayer1_halves1.size() + MVTXlayer1_halves2.size() + MVTXlayer2_halves1.size() + MVTXlayer2_halves2.size() < Ncluster_toplot && i <= 60)
        {
            Draw_RecoVtxZTklZRho(hM_clusterzrho_halves1, hM_clusterzrho_halves2, tl_zrho_halves1, tl_zrho_halves2, PV_halves1[2], PV_halves2[2], PV_z, IsData, Form("%s_Evt%d_RecoPV_ZRho", demoplotname.Data(), event));
            Draw_RecoVtxTklXY(hM_clusterxy_halves1, hM_clusterxy_halves2, tl_xy_halves1, tl_xy_halves2, PV_halves1[0], PV_halves2[0], PV_x, PV_halves1[1], PV_halves2[1], PV_y, dZCutbin, IsData, Form("%s_Evt%d_RecoPV_XY", demoplotname.Data(), event));
            // Draw_diffExptzPVz(hM_diffExptzPVz_halves1, hM_diffExptzPVz_halves2, IsData, Form("%s_Evt%d_diffExptzPVz", demoplotname.Data(), event));
            // Draw_diffExptxyPVxy(hM_diffExptxyPVxy_halves1, hM_diffExptxyPVxy_halves2, IsData, Form("%s_Evt%d_diffExptxyPVxy", demoplotname.Data(), event));
        }

        // merge the histograms
        hM_clusterzrho_halves1->Add(hM_clusterzrho_halves2);
        Draw_1DHistVtxDemo(hM_diffExptzPVz_halves1, "v_{Z}^{Candidate} - PV_{Z}^{Estimate} [cm]", "cm", 510, IsData, true, "2023-08-30", Form("%s_Evt%d_diffExptzPVz", demoplotname.Data(), event));
        hM_diffExptxyPVxy_halves1->Add(hM_diffExptxyPVxy_halves2);
        Draw_2DHistVtxDemo(hM_diffExptxyPVxy_halves1, "v_{X}^{Candidate} - PV_{X}^{Estimate} [cm]", "v_{Y}^{Candidate} - PV_{Y}^{Estimate} [cm]", 510, IsData, true, "2023-08-30", Form("%s_Evt%d_diffExptxyPVxy", demoplotname.Data(), event));

        hM_VtxCandZ_halves1->Add(hM_VtxCandZ_halves2);
        Draw_1DHistVtxDemo(hM_VtxCandZ_halves1, "v_{Z}^{Candidate} [cm]", "cm", 510, IsData, true, "2023-08-30", Form("%s_Evt%d_VtxCandZall", demoplotname.Data(), event));
        hM_VtxCandXY_halves1->Add(hM_VtxCandXY_halves2);
        Draw_2DHistVtxDemo(hM_VtxCandXY_halves1, "v_{X}^{Candidate} [cm]", "v_{Y}^{Candidate} [cm]", 510, IsData, true, "2023-08-30", Form("%s_Evt%d_VtxCandXYcluster", demoplotname.Data(), event));

        hM_DCAxy_VtxClus_halves1->Add(hM_DCAxy_VtxClus_halves2);
        Draw_1DHistVtxDemo(hM_DCAxy_VtxClus_halves1, "DCA_{xy} [cm]", "cm", 505, IsData, false, "", Form("%s_Evt%d_DCAxy", demoplotname.Data(), event));
        hM_DCAxy_DCAz_VtxClus_halves1->Add(hM_DCAxy_DCAz_VtxClus_halves2);
        Draw_2DHistVtxDemo(hM_DCAxy_DCAz_VtxClus_halves1, "DCA_{xy} [cm]", "DCA_{z} [cm]", 505, IsData, false, "", Form("%s_Evt%d_DCAxyDCAz", demoplotname.Data(), event));
        hM_DCAx_DCAy_VtxClus_halves1->Add(hM_DCAx_DCAy_VtxClus_halves2);
        Draw_2DHistVtxDemo(hM_DCAx_DCAy_VtxClus_halves1, "DCA_{x} [cm]", "DCA_{y} [cm]", 505, IsData, false, "", Form("%s_Evt%d_DCAxDCAy", demoplotname.Data(), event));

        CleanVec(tl_zrho_halves1);
        CleanVec(tl_zrho_halves2);
        CleanVec(tl_xy_halves1);
        CleanVec(tl_xy_halves2);
        delete hM_clusterzrho_halves1;
        delete hM_clusterzrho_halves2;
        delete hM_clusterxy_halves1;
        delete hM_clusterxy_halves2;
        delete hM_diffExptzPVz_halves1;
        delete hM_diffExptzPVz_halves2;
        delete hM_diffExptxyPVxy_halves1;
        delete hM_diffExptxyPVxy_halves2;
        delete hM_VtxCandZ_halves1;
        delete hM_VtxCandZ_halves2;
        delete hM_VtxCandXY_halves1;
        delete hM_VtxCandXY_halves2;
        delete hM_DCAxy_VtxClus_halves1;
        delete hM_DCAxy_VtxClus_halves2;
        delete hM_DCAxy_DCAz_VtxClus_halves1;
        delete hM_DCAxy_DCAz_VtxClus_halves2;
        delete hM_DCAx_DCAy_VtxClus_halves1;
        delete hM_DCAx_DCAy_VtxClus_halves2;

        if (!IsData)
        {
            cout << "NTruthVtx = " << NTruthVtx << endl
                 << "Truth primary (most N particles) Z position = " << TruthPV_trig_z << endl
                 << "Recontructed primary vertex Z position from 1st halve= " << PV_halves1[2] << endl
                 << "Recontructed primary vertex Z position from 2nd halve= " << PV_halves2[2] << endl
                 << "Average of the Z positions from 1st and 2nd halve= " << (PV_halves1[2] + PV_halves2[2]) / 2. << endl
                 << "(Truth-Reco) = " << TruthPV_trig_z - (PV_halves1[2] + PV_halves2[2]) / 2. << endl;

            if (NTruthVtx == 1)
            {
                if (TruthPV_trig_z != TruthPV_mostNpart_z)
                    cout << "For events with only 1 truth vertex, (TruthPV_trig_z,TruthPV_mostNpart_z) = (" << TruthPV_trig_z << "," << TruthPV_mostNpart_z << "). Check!" << endl;
            }
        }
        else
        {
            cout << "Reconstructed primary vertex X position from 1st halve= " << PV_halves1[0] << endl << "Reconstructed primary vertex X position from 2nd halve= " << PV_halves2[0] << endl << "Average of the X positions from 1st and 2nd halve= " << (PV_halves1[0] + PV_halves2[0]) / 2. << endl;
            cout << "Reconstructed primary vertex Y position from 1st halve= " << PV_halves1[1] << endl << "Reconstructed primary vertex Y position from 2nd halve= " << PV_halves2[1] << endl << "Average of the Y positions from 1st and 2nd halve= " << (PV_halves1[1] + PV_halves2[1]) / 2. << endl;
            cout << "Recontructed primary vertex Z position from 1st halve= " << PV_halves1[2] << endl << "Recontructed primary vertex Z position from 2nd halve= " << PV_halves2[2] << endl << "Average of the Z positions from 1st and 2nd halve= " << (PV_halves1[2] + PV_halves2[2]) / 2. << endl;
        }

        vtxdata.event = event;
        if (!IsData)
        {
            vtxdata.NTruthVtx = NTruthVtx;
            vtxdata.TruthPV_trig_x = TruthPV_trig_x;
            vtxdata.TruthPV_trig_y = TruthPV_trig_y;
            vtxdata.TruthPV_trig_z = TruthPV_trig_z;
            vtxdata.TruthPV_mostNpart_x = TruthPV_mostNpart_x;
            vtxdata.TruthPV_mostNpart_y = TruthPV_mostNpart_y;
            vtxdata.TruthPV_mostNpart_z = TruthPV_mostNpart_z;
            vtxdata.Centrality_bimp = centrality_bimp;
            vtxdata.Centrality_impactparam = centrality_impactparam;
        }
        vtxdata.NhitsLayer1 = (int)MVTXlayer1_halves1.size() + (int)MVTXlayer1_halves2.size();
        vtxdata.PV_x_halves1 = PV_halves1[0];
        vtxdata.PV_y_halves1 = PV_halves1[1];
        vtxdata.PV_z_halves1 = PV_halves1[2];
        vtxdata.PV_x_halves2 = PV_halves2[0];
        vtxdata.PV_y_halves2 = PV_halves2[1];
        vtxdata.PV_z_halves2 = PV_halves2[2];
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
