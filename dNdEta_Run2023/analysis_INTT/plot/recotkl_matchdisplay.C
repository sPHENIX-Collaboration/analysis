void recotkl_matchdisplay()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "./recotkl_matchEff/display";
    system(Form("mkdir -p %s", plotdir.c_str()));

    TH2F *hM_tklposition = new TH2F("hM_tklposition", "Tracklet position", 240, -12, 12, 240, -12, 12);
    TH2F *hM_tklposition_zrho = new TH2F("hM_tklposition_zrho", "Tracklet position z-rho", 250, -25, 25, 240, -12, 12);
    TH3F *hM_tklposition3D = new TH3F("hM_tklposition3D", "Tracklet position 3D", 250, -25, 25, 240, -12, 12, 240, -12, 12);
    std::vector<TLine *> tklline, tklline_zrho;
    std::vector<TPolyLine3D *> tkl3Dline;

    TFile *f = new TFile("/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_SIMPLE_ana466_20250214/dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_00000.root", "READ");
    TTree *t = (TTree *)f->Get("minitree");

    // get the vectors from the TTree
    int event;
    std::vector<float> *tklclus1x = 0, *tklclus1y = 0, *tklclus1z = 0, *tklclus2x = 0, *tklclus2y = 0, *tklclus2z = 0, *tklclus1phi = 0, *tklclus1eta = 0, *tklclus2phi = 0, *tklclus2eta = 0;
    std::vector<int> *recotkl_clus1_matchedtrackID = 0, *recotkl_clus2_matchedtrackID = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("tklclus1x", &tklclus1x);
    t->SetBranchAddress("tklclus1y", &tklclus1y);
    t->SetBranchAddress("tklclus1z", &tklclus1z);
    t->SetBranchAddress("tklclus2x", &tklclus2x);
    t->SetBranchAddress("tklclus2y", &tklclus2y);
    t->SetBranchAddress("tklclus2z", &tklclus2z);
    t->SetBranchAddress("tklclus1phi", &tklclus1phi);
    t->SetBranchAddress("tklclus1eta", &tklclus1eta);
    t->SetBranchAddress("tklclus2phi", &tklclus2phi);
    t->SetBranchAddress("tklclus2eta", &tklclus2eta);
    t->SetBranchAddress("recotkl_clus1_matchedtrackID", &recotkl_clus1_matchedtrackID);
    t->SetBranchAddress("recotkl_clus2_matchedtrackID", &recotkl_clus2_matchedtrackID);

    for (int i = 0; i < 1; i++)
    {
        t->GetEntry(i);

        int ntkl = tklclus1x->size();
        int ntkl_matchedtrack = 0, ntkl_unmatchedtrack = 0, ntkl_matchedtrack_positiveID = 0, ntkl_matchedtrack_negativeID = 0;
        for (size_t j = 0; j < tklclus1x->size(); j++)
        {
            std::cout << "(x1, y1, matched trackID)=(" << tklclus1x->at(j) << ", " << tklclus1y->at(j) << ", " << recotkl_clus1_matchedtrackID->at(j) << "); (x2, y2, matched trackID)=(" << tklclus2x->at(j) << ", " << tklclus2y->at(j) << ", " << recotkl_clus2_matchedtrackID->at(j) << ")" << std::endl;
            hM_tklposition->SetBinContent(hM_tklposition->FindBin(tklclus1x->at(j), tklclus1y->at(j)), recotkl_clus1_matchedtrackID->at(j));
            hM_tklposition->SetBinContent(hM_tklposition->FindBin(tklclus2x->at(j), tklclus2y->at(j)), recotkl_clus2_matchedtrackID->at(j));
            float rho1 = (tklclus1y->at(j) > 0) ? sqrt(tklclus1x->at(j) * tklclus1x->at(j) + tklclus1y->at(j) * tklclus1y->at(j)) : -sqrt(tklclus1x->at(j) * tklclus1x->at(j) + tklclus1y->at(j) * tklclus1y->at(j));
            float rho2 = (tklclus2y->at(j) > 0) ? sqrt(tklclus2x->at(j) * tklclus2x->at(j) + tklclus2y->at(j) * tklclus2y->at(j)) : -sqrt(tklclus2x->at(j) * tklclus2x->at(j) + tklclus2y->at(j) * tklclus2y->at(j));
            hM_tklposition_zrho->SetBinContent(hM_tklposition_zrho->FindBin(tklclus1z->at(j), rho1), recotkl_clus1_matchedtrackID->at(j));
            hM_tklposition_zrho->SetBinContent(hM_tklposition_zrho->FindBin(tklclus2z->at(j), rho2), recotkl_clus2_matchedtrackID->at(j));

            TLine *tkl = new TLine(tklclus1x->at(j), tklclus1y->at(j), tklclus2x->at(j), tklclus2y->at(j));
            TLine *tkl_zrho = new TLine(tklclus1z->at(j), rho1, tklclus2z->at(j), rho2);
            if (recotkl_clus1_matchedtrackID->at(j) == recotkl_clus2_matchedtrackID->at(j))
            {
                ntkl_matchedtrack++;
                if (recotkl_clus1_matchedtrackID->at(j) > 0)
                {
                    ntkl_matchedtrack_positiveID++;
                    tkl->SetLineColor(kBlue);
                    tkl_zrho->SetLineColor(kBlue);
                }
                else
                {
                    ntkl_matchedtrack_negativeID++;
                    tkl->SetLineColor(kBlack);
                    tkl_zrho->SetLineColor(kBlack);
                }
            }
            else
            {
                ntkl_unmatchedtrack++;
                tkl->SetLineColor(kRed);
                tkl_zrho->SetLineColor(kRed);
            }
            tklline.push_back(tkl);
            tklline_zrho.push_back(tkl_zrho);

            // z-y-x
            hM_tklposition3D->Fill(tklclus1z->at(j), tklclus1y->at(j), tklclus1x->at(j));
            hM_tklposition3D->Fill(tklclus2z->at(j), tklclus2y->at(j), tklclus2x->at(j));
            TPolyLine3D *tkl3D = new TPolyLine3D(2);
            tkl3D->SetPoint(0, tklclus1z->at(j), tklclus1y->at(j), tklclus1x->at(j));
            tkl3D->SetPoint(1, tklclus2z->at(j), tklclus2y->at(j), tklclus2x->at(j));
            if (recotkl_clus1_matchedtrackID->at(j) == recotkl_clus2_matchedtrackID->at(j))
            {
                tkl3D->SetLineColor(kBlack);
            }
            else
            {
                tkl3D->SetLineColor(kRed);
            }
            tkl3Dline.push_back(tkl3D);
        }

        std::cout << "ntkl = " << ntkl << "; ntkl_matchedtrack = " << ntkl_matchedtrack << "; ntkl_unmatchedtrack = " << ntkl_unmatchedtrack << "; ntkl_matchedtrack_positiveID = " << ntkl_matchedtrack_positiveID << "; ntkl_matchedtrack_negativeID = " << ntkl_matchedtrack_negativeID << std::endl;
    }

    f->Close();

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    gPad->SetRightMargin(0.15);
    c->cd();
    hM_tklposition->GetXaxis()->SetTitle("x [cm]");
    hM_tklposition->GetYaxis()->SetTitle("y [cm]");
    hM_tklposition->SetMarkerSize(0.7);
    hM_tklposition->Draw("text ");
    for (auto &line : tklline)
    {
        line->Draw();
    }
    c->SaveAs(Form("%s/hM_tklposition.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/hM_tklposition.png", plotdir.c_str()));

    c->Clear();
    c->cd();
    hM_tklposition3D->GetXaxis()->SetTitle("z [cm]");
    hM_tklposition3D->GetYaxis()->SetTitle("y [cm]");
    hM_tklposition3D->GetZaxis()->SetTitle("x [cm]");
    hM_tklposition3D->Draw();
    for (auto &line : tkl3Dline)
    {
        line->Draw();
    }
    c->Draw();
    c->SaveAs(Form("%s/hM_tklposition3D.pdf", plotdir.c_str()));
    c->SaveAs(Form("%s/hM_tklposition3D.png", plotdir.c_str()));

    TCanvas *c2 = new TCanvas("c2", "c2", 1500, 700);
    c2->cd();
    hM_tklposition_zrho->GetXaxis()->SetTitle("z [cm]");
    hM_tklposition_zrho->GetYaxis()->SetTitle("#rho [cm]");
    hM_tklposition_zrho->SetMarkerSize(0.7);
    hM_tklposition_zrho->Draw("text ");
    for (auto &line : tklline_zrho)
    {
        line->Draw();
    }
    c2->SaveAs(Form("%s/hM_tklposition_zrho.pdf", plotdir.c_str()));
    c2->SaveAs(Form("%s/hM_tklposition_zrho.png", plotdir.c_str()));

    TFile *fout = new TFile(Form("%s/hM_tklposition.root", plotdir.c_str()), "RECREATE");
    hM_tklposition->Write();
    hM_tklposition3D->Write();
    fout->Close();
}