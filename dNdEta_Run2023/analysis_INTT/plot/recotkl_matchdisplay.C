std::vector<std::string> v_color{"#99cc99", "#6699cc", "#f2777a", "#7F167F"};

void drawAndSaveCanvas(std::pair<int, int> canvasSize,                   //
                       std::tuple<float, float, float, float> axisrange, //
                       std::vector<TScatter *> &vec_tscat,               //
                       std::vector<TLine *> &tline,                      //
                       const std::vector<std::string> &v_leg,            //
                       const std::string &xtitle,                        //
                       const std::string &ytitle,                        //
                       const std::string &plotname                       //

)
{
    TCanvas *c = new TCanvas("c", "c", canvasSize.first, canvasSize.second);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.05 * vec_tscat.size() + 0.03);
    c->cd();
    TH2F *h = new TH2F("h", "", 1, std::get<0>(axisrange), std::get<1>(axisrange), 1, std::get<2>(axisrange), std::get<3>(axisrange));
    h->GetXaxis()->SetTitle(xtitle.c_str());
    h->GetYaxis()->SetTitle(ytitle.c_str());
    h->Draw("axis");
    for (size_t i = 0; i < vec_tscat.size(); ++i)
    {
        vec_tscat[i]->SetMarkerSize(0.5);
        // vec_tscat[i]->SetMarkerColor(TColor::GetColor(v_color[i].c_str()));
        vec_tscat[i]->Draw();
    }

    gPad->Update();
    gPad->Modified();

    for (auto &line : tline)
    {
        line->Draw("same");
    }

    // redraw the TScatter object of "Event vertex" to make it on top of the TGraph2D objects
    auto it_evtvtx = std::find(v_leg.begin(), v_leg.end(), "Event vertex");
    if (it_evtvtx != v_leg.end())
    {
        int index = std::distance(v_leg.begin(), it_evtvtx);
        vec_tscat[index]->Draw();
    }

    TLegend *leg = new TLegend(gPad->GetLeftMargin(),           //
                               1 - gPad->GetTopMargin() + 0.01, //
                               gPad->GetLeftMargin() + 0.3,     //
                               0.98                             //
    );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    for (size_t i = 0; i < vec_tscat.size(); ++i)
    {
        leg->AddEntry(vec_tscat[i], v_leg[i].c_str(), "p");
    }
    leg->Draw();

    c->SaveAs(Form("%s.pdf", plotname.c_str()));
    c->SaveAs(Form("%s.png", plotname.c_str()));
}

void drawTGraph2D(std::pair<int, int> canvasSize,                                 //
                  std::tuple<float, float, float, float, float, float> axisrange, //
                  std::vector<TGraph2D *> &vec_tg2D,                              //
                  std::vector<TPolyLine3D *> &tline,                              //
                  const std::vector<std::string> &v_leg,                          //
                  const std::string &xtitle,                                      //
                  const std::string &ytitle,                                      //
                  const std::string &ztitle,                                      //
                  const std::string &plotname                                     //
)
{
    TCanvas *c = new TCanvas("c", "c", canvasSize.first, canvasSize.second);
    c->SetTheta(-30);
    c->SetPhi(-30);
    gPad->SetTopMargin(0.05 * vec_tg2D.size() + 0.03);
    c->cd();

    // Unpack axis range values (assumed order: x_min, x_max, y_min, y_max, z_min, z_max)
    double xlow = std::get<0>(axisrange);
    double xhigh = std::get<1>(axisrange);
    double ylow = std::get<2>(axisrange);
    double yhigh = std::get<3>(axisrange);
    double zlow = std::get<4>(axisrange);
    double zhigh = std::get<5>(axisrange);

    // Create a dummy histogram to set the frame.
    // Use enough bins (e.g. 50) so that the full range is defined without precision issues.
    int nbinsX = 1, nbinsY = 1, nbinsZ = 1;
    TH3F *dummy = new TH3F("dummy", "", nbinsX, xlow, xhigh, nbinsY, ylow, yhigh, nbinsZ, zlow, zhigh);
    // Remove fill and stats so only the frame is drawn.
    dummy->SetFillStyle(0);
    dummy->SetStats(0);

    // Set the axis titles
    dummy->GetXaxis()->SetTitle(xtitle.c_str());
    dummy->GetYaxis()->SetTitle(ytitle.c_str());
    dummy->GetZaxis()->SetTitle(ztitle.c_str());

    // Draw the frame (using "HIST" draws the histogram frame, including the titles)
    dummy->Draw("HIST");

    // Now draw each TGraph2D object on top of the dummy frame.
    for (size_t i = 0; i < vec_tg2D.size(); ++i)
    {
        // vec_tg2D[i]->SetMarkerStyle(20);
        vec_tg2D[i]->SetMarkerSize(0.5);
        // vec_tg2D[i]->SetMarkerColor(TColor::GetColor(v_color[i].c_str()));
        vec_tg2D[i]->Draw("P SAME");
    }

    // Draw any additional TPolyLine3D objects
    for (auto &line : tline)
    {
        line->Draw("SAME");
    }

    auto it_evtvtx = std::find(v_leg.begin(), v_leg.end(), "Event vertex");
    if (it_evtvtx != v_leg.end())
    {
        int index = std::distance(v_leg.begin(), it_evtvtx);
        vec_tg2D[index]->Draw("P SAME");
    }

    TLegend *leg = new TLegend(gPad->GetLeftMargin(),           //
                               1 - gPad->GetTopMargin() + 0.01, //
                               gPad->GetLeftMargin() + 0.3,     //
                               0.98                             //
    );
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    for (size_t i = 0; i < vec_tg2D.size(); ++i)
    {
        leg->AddEntry(vec_tg2D[i], v_leg[i].c_str(), "p");
    }
    leg->Draw();

    c->SaveAs(Form("%s.pdf", plotname.c_str()));
    c->SaveAs(Form("%s.png", plotname.c_str()));
}

void recotkl_matchdisplay()
{
    // directory to save the output plots; if the directory does not exist, create it using
    std::string plotdir = "./recotkl_matchEff/display";
    system(Form("mkdir -p %s", plotdir.c_str()));

    // TH3F *hM_tklposition3D = new TH3F("hM_tklposition3D", "Tracklet position 3D", 250, -25, 25, 240, -12, 12, 240, -12, 12);

    TFile *f = new TFile("/sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_Sim_SIMPLE_ana466_20250214/dRcut999p0_NominalVtxZ_RandomClusSet0_clusAdcCutSet0_clusPhiSizeCutSet0/minitree_00000.root", "READ");
    TTree *t = (TTree *)f->Get("minitree");

    // get the vectors from the TTree
    int event;
    float PV_x, PV_y, PV_z;
    std::vector<float> *tklclus1x = 0, *tklclus1y = 0, *tklclus1z = 0, *tklclus2x = 0, *tklclus2y = 0, *tklclus2z = 0, *tklclus1phi = 0, *tklclus1eta = 0, *tklclus2phi = 0, *tklclus2eta = 0;
    std::vector<float> *PrimaryG4P_eta = 0, *PrimaryG4P_phi = 0, *GenHadron_eta = 0, *GenHadron_phi = 0;
    std::vector<int> *recotkl_clus1_matchedtrackID = 0, *recotkl_clus2_matchedtrackID = 0;
    std::vector<float> *recotkl_matchedPG4P_eta = 0, *recotkl_matchedPG4P_phi = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("PV_x", &PV_x);
    t->SetBranchAddress("PV_y", &PV_y);
    t->SetBranchAddress("PV_z", &PV_z);
    t->SetBranchAddress("tklclus1x", &tklclus1x);
    t->SetBranchAddress("tklclus1y", &tklclus1y);
    t->SetBranchAddress("tklclus1z", &tklclus1z);
    t->SetBranchAddress("tklclus2x", &tklclus2x);
    t->SetBranchAddress("tklclus2y", &tklclus2y);
    t->SetBranchAddress("tklclus2z", &tklclus2z);
    t->SetBranchAddress("tklclus1Phi", &tklclus1phi);
    t->SetBranchAddress("tklclus1Eta", &tklclus1eta);
    t->SetBranchAddress("tklclus2Phi", &tklclus2phi);
    t->SetBranchAddress("tklclus2Eta", &tklclus2eta);
    t->SetBranchAddress("PrimaryG4P_eta", &PrimaryG4P_eta);
    t->SetBranchAddress("PrimaryG4P_phi", &PrimaryG4P_phi);
    t->SetBranchAddress("GenHadron_eta", &GenHadron_eta);
    t->SetBranchAddress("GenHadron_phi", &GenHadron_phi);
    t->SetBranchAddress("recotkl_clus1_matchedtrackID", &recotkl_clus1_matchedtrackID);
    t->SetBranchAddress("recotkl_clus2_matchedtrackID", &recotkl_clus2_matchedtrackID);
    t->SetBranchAddress("recotkl_matchedPG4P_eta", &recotkl_matchedPG4P_eta);
    t->SetBranchAddress("recotkl_matchedPG4P_phi", &recotkl_matchedPG4P_phi);

    for (int i = 0; i < 1; i++)
    {
        t->GetEntry(i);

        int ntkl = tklclus1x->size();
        int ntkl_matchedtrack = 0, ntkl_unmatchedtrack = 0, ntkl_matchedtrack_positiveID = 0, ntkl_matchedtrack_negativeID = 0;
        std::vector<double> tkl_clusx_matched_posTrackID, tkl_clusy_matched_posTrackID, tkl_clusz_matched_posTrackID, tkl_clusrho_matched_posTrackID;
        std::vector<double> tkl_clusx_matched_negTrackID, tkl_clusy_matched_negTrackID, tkl_clusz_matched_negTrackID, tkl_clusrho_matched_negTrackID;
        std::vector<double> tkl_clusx_unmatched, tkl_clusy_unmatched, tkl_clusz_unmatched, tkl_clusrho_unmatched;
        std::vector<double> tkl_cluseta_matched_posTrackID, tkl_clusphi_matched_posTrackID, tkl_cluseta_matched_negTrackID, tkl_clusphi_matched_negTrackID, tkl_cluseta_unmatched, tkl_clusphi_unmatched;
        std::vector<double> pg4p_eta, pg4p_phi, gh_eta, gh_phi;
        std::vector<double> tklmatchedg4p_eta_posTrackID, tklmatchedg4p_phi_posTrackID, tklmatchedg4p_eta_negTrackID, tklmatchedg4p_phi_negTrackID;

        std::vector<TLine *> tklline, tklline_zrho, tline_etaphi, tline_etaphi_matched_posTrackID, tline_etaphi_matched_negTrackID;
        std::vector<TPolyLine3D *> tkl3Dline;

        for (size_t j = 0; j < tklclus1x->size(); j++)
        {
            std::cout << "(x1, y1, matched trackID)=(" << tklclus1x->at(j) << ", " << tklclus1y->at(j) << ", " << recotkl_clus1_matchedtrackID->at(j) << "); (x2, y2, matched trackID)=(" << tklclus2x->at(j) << ", " << tklclus2y->at(j) << ", " << recotkl_clus2_matchedtrackID->at(j) << ")" << std::endl;
            // rho is calculated relative to (PV_x, PV_y)
            float rho1 = ((tklclus1y->at(j) - PV_y) > 0) ? sqrt(pow(tklclus1x->at(j) - PV_x, 2) + pow(tklclus1y->at(j) - PV_y, 2)) : -sqrt(pow(tklclus1x->at(j) - PV_x, 2) + pow(tklclus1y->at(j) - PV_y, 2));
            float rho2 = ((tklclus2y->at(j) - PV_y) > 0) ? sqrt(pow(tklclus2x->at(j) - PV_x, 2) + pow(tklclus2y->at(j) - PV_y, 2)) : -sqrt(pow(tklclus2x->at(j) - PV_x, 2) + pow(tklclus2y->at(j) - PV_y, 2));

            if (recotkl_clus1_matchedtrackID->at(j) == recotkl_clus2_matchedtrackID->at(j) && recotkl_clus1_matchedtrackID->at(j) > 0)
            {
                tkl_clusx_matched_posTrackID.push_back(tklclus1x->at(j));
                tkl_clusx_matched_posTrackID.push_back(tklclus2x->at(j));
                tkl_clusy_matched_posTrackID.push_back(tklclus1y->at(j));
                tkl_clusy_matched_posTrackID.push_back(tklclus2y->at(j));
                tkl_clusz_matched_posTrackID.push_back(tklclus1z->at(j));
                tkl_clusz_matched_posTrackID.push_back(tklclus2z->at(j));
                tkl_clusrho_matched_posTrackID.push_back(rho1);
                tkl_clusrho_matched_posTrackID.push_back(rho2);
                tkl_cluseta_matched_posTrackID.push_back(tklclus1eta->at(j));
                tkl_cluseta_matched_posTrackID.push_back(tklclus2eta->at(j));
                tkl_clusphi_matched_posTrackID.push_back(tklclus1phi->at(j));
                tkl_clusphi_matched_posTrackID.push_back(tklclus2phi->at(j));

                tklmatchedg4p_eta_posTrackID.push_back(recotkl_matchedPG4P_eta->at(j));
                tklmatchedg4p_phi_posTrackID.push_back(recotkl_matchedPG4P_phi->at(j));
            }
            else if (recotkl_clus1_matchedtrackID->at(j) == recotkl_clus2_matchedtrackID->at(j) && recotkl_clus1_matchedtrackID->at(j) < 0)
            {
                tkl_clusx_matched_negTrackID.push_back(tklclus1x->at(j));
                tkl_clusx_matched_negTrackID.push_back(tklclus2x->at(j));
                tkl_clusy_matched_negTrackID.push_back(tklclus1y->at(j));
                tkl_clusy_matched_negTrackID.push_back(tklclus2y->at(j));
                tkl_clusz_matched_negTrackID.push_back(tklclus1z->at(j));
                tkl_clusz_matched_negTrackID.push_back(tklclus2z->at(j));
                tkl_clusrho_matched_negTrackID.push_back(rho1);
                tkl_clusrho_matched_negTrackID.push_back(rho2);
                tkl_cluseta_matched_negTrackID.push_back(tklclus1eta->at(j));
                tkl_cluseta_matched_negTrackID.push_back(tklclus2eta->at(j));
                tkl_clusphi_matched_negTrackID.push_back(tklclus1phi->at(j));
                tkl_clusphi_matched_negTrackID.push_back(tklclus2phi->at(j));

                tklmatchedg4p_eta_negTrackID.push_back(recotkl_matchedPG4P_eta->at(j));
                tklmatchedg4p_phi_negTrackID.push_back(recotkl_matchedPG4P_phi->at(j));
            }
            else
            {
                tkl_clusx_unmatched.push_back(tklclus1x->at(j));
                tkl_clusx_unmatched.push_back(tklclus2x->at(j));
                tkl_clusy_unmatched.push_back(tklclus1y->at(j));
                tkl_clusy_unmatched.push_back(tklclus2y->at(j));
                tkl_clusz_unmatched.push_back(tklclus1z->at(j));
                tkl_clusz_unmatched.push_back(tklclus2z->at(j));
                tkl_clusrho_unmatched.push_back(rho1);
                tkl_clusrho_unmatched.push_back(rho2);
                tkl_cluseta_unmatched.push_back(tklclus1eta->at(j));
                tkl_cluseta_unmatched.push_back(tklclus2eta->at(j));
                tkl_clusphi_unmatched.push_back(tklclus1phi->at(j));
                tkl_clusphi_unmatched.push_back(tklclus2phi->at(j));
            }

            TLine *tkl = new TLine(tklclus1x->at(j), tklclus1y->at(j), tklclus2x->at(j), tklclus2y->at(j));
            TLine *tkl_zrho = new TLine(tklclus1z->at(j), rho1, tklclus2z->at(j), rho2);
            TLine *tkl2vtx_xy = new TLine(tklclus1x->at(j), tklclus1y->at(j), PV_x, PV_y);
            TLine *tkl2vtx_zrho = new TLine(tklclus1z->at(j), rho1, PV_z, 0);
            TLine *tkl_etaphi = new TLine(tklclus1eta->at(j), tklclus1phi->at(j), tklclus2eta->at(j), tklclus2phi->at(j));
            TLine *assoc_tklclus1_g4p = new TLine(tklclus1eta->at(j), tklclus1phi->at(j), recotkl_matchedPG4P_eta->at(j), recotkl_matchedPG4P_phi->at(j));
            TLine *assoc_tklclus2_g4p = new TLine(tklclus2eta->at(j), tklclus2phi->at(j), recotkl_matchedPG4P_eta->at(j), recotkl_matchedPG4P_phi->at(j));
            if (recotkl_clus1_matchedtrackID->at(j) == recotkl_clus2_matchedtrackID->at(j))
            {
                ntkl_matchedtrack++;
                if (recotkl_clus1_matchedtrackID->at(j) > 0)
                {
                    ntkl_matchedtrack_positiveID++;
                    tkl->SetLineColor(TColor::GetColor("#99cc99"));
                    tkl_zrho->SetLineColor(TColor::GetColor("#99cc99"));
                    tkl_etaphi->SetLineColor(TColor::GetColor("#99cc99"));
                    tline_etaphi_matched_posTrackID.push_back(tkl_etaphi);
                    tkl2vtx_xy->SetLineColorAlpha(TColor::GetColor("#99cc99"), 0.4);
                    tkl2vtx_xy->SetLineStyle(2);
                    tkl2vtx_zrho->SetLineColorAlpha(TColor::GetColor("#99cc99"), 0.4);
                    tkl2vtx_zrho->SetLineStyle(2);
                    assoc_tklclus1_g4p->SetLineColorAlpha(TColor::GetColor("#5E8B7E"), 0.5);
                    assoc_tklclus2_g4p->SetLineColorAlpha(TColor::GetColor("#5E8B7E"), 0.5);
                    assoc_tklclus1_g4p->SetLineStyle(2);
                    assoc_tklclus2_g4p->SetLineStyle(2);
                    tline_etaphi_matched_posTrackID.push_back(assoc_tklclus1_g4p);
                    tline_etaphi_matched_posTrackID.push_back(assoc_tklclus2_g4p);
                }
                else
                {
                    ntkl_matchedtrack_negativeID++;
                    tkl->SetLineColor(TColor::GetColor("#6699cc"));
                    tkl_zrho->SetLineColor(TColor::GetColor("#6699cc"));
                    tkl_etaphi->SetLineColor(TColor::GetColor("#6699cc"));
                    tline_etaphi_matched_negTrackID.push_back(tkl_etaphi);
                    tkl2vtx_xy->SetLineColorAlpha(TColor::GetColor("#6699cc"), 0.4);
                    tkl2vtx_xy->SetLineStyle(2);
                    tkl2vtx_zrho->SetLineColorAlpha(TColor::GetColor("#6699cc"), 0.4);
                    tkl2vtx_zrho->SetLineStyle(2);
                    assoc_tklclus1_g4p->SetLineColorAlpha(TColor::GetColor("#0F4C75"), 0.8);
                    assoc_tklclus2_g4p->SetLineColorAlpha(TColor::GetColor("#0F4C75"), 0.8);
                    assoc_tklclus1_g4p->SetLineStyle(2);
                    assoc_tklclus2_g4p->SetLineStyle(2);
                    tline_etaphi_matched_negTrackID.push_back(assoc_tklclus1_g4p);
                    tline_etaphi_matched_negTrackID.push_back(assoc_tklclus2_g4p);
                }
            }
            else
            {
                ntkl_unmatchedtrack++;
                tkl->SetLineColor(TColor::GetColor("#f2777a"));
                tkl_zrho->SetLineColor(TColor::GetColor("#f2777a"));
                tkl_etaphi->SetLineColor(TColor::GetColor("#f2777a"));

                tkl2vtx_xy->SetLineColorAlpha(TColor::GetColor("#f2777a"), 0.4);
                tkl2vtx_xy->SetLineStyle(2);
                tkl2vtx_zrho->SetLineColorAlpha(TColor::GetColor("#f2777a"), 0.4);
                tkl2vtx_zrho->SetLineStyle(2);
            }
            tklline.push_back(tkl);
            tklline.push_back(tkl2vtx_xy);
            tklline_zrho.push_back(tkl_zrho);
            tklline_zrho.push_back(tkl2vtx_zrho);
            tline_etaphi.push_back(tkl_etaphi);

            // z-y-x
            // hM_tklposition3D->Fill(tklclus1z->at(j), tklclus1y->at(j), tklclus1x->at(j));
            // hM_tklposition3D->Fill(tklclus2z->at(j), tklclus2y->at(j), tklclus2x->at(j));
            TPolyLine3D *tkl3D = new TPolyLine3D(2);
            tkl3D->SetPoint(0, tklclus1z->at(j), tklclus1y->at(j), tklclus1x->at(j));
            tkl3D->SetPoint(1, tklclus2z->at(j), tklclus2y->at(j), tklclus2x->at(j));
            TPolyLine3D *tkl2vtx3D = new TPolyLine3D(2);
            tkl2vtx3D->SetPoint(0, tklclus1z->at(j), tklclus1y->at(j), tklclus1x->at(j));
            tkl2vtx3D->SetPoint(1, PV_z, PV_y, PV_x);
            TPolyLine3D *vtxline = new TPolyLine3D(2);
            vtxline->SetPoint(0, -25, PV_y, PV_x);
            vtxline->SetPoint(1, 25, PV_y, PV_x);
            vtxline->SetLineColorAlpha(kBlack, 0.4);
            vtxline->SetLineStyle(1);
            if (recotkl_clus1_matchedtrackID->at(j) == recotkl_clus2_matchedtrackID->at(j) && recotkl_clus1_matchedtrackID->at(j) > 0)
            {
                tkl3D->SetLineColor(TColor::GetColor("#99cc99"));
                tkl2vtx3D->SetLineColorAlpha(TColor::GetColor("#99cc99"), 0.2);
                tkl2vtx3D->SetLineStyle(2);
            }
            else if (recotkl_clus1_matchedtrackID->at(j) == recotkl_clus2_matchedtrackID->at(j) && recotkl_clus1_matchedtrackID->at(j) < 0)
            {
                tkl3D->SetLineColor(TColor::GetColor("#6699cc"));
                tkl2vtx3D->SetLineColorAlpha(TColor::GetColor("#6699cc"), 0.2);
                tkl2vtx3D->SetLineStyle(2);
            }
            else
            {
                tkl3D->SetLineColor(TColor::GetColor("#f2777a"));
                tkl2vtx3D->SetLineColorAlpha(TColor::GetColor("#f2777a"), 0.2);
                tkl2vtx3D->SetLineStyle(2);
            }
            tkl3Dline.push_back(vtxline);
            tkl3Dline.push_back(tkl3D);
            tkl3Dline.push_back(tkl2vtx3D);
        }

        for (size_t j = 0; j < PrimaryG4P_eta->size(); j++)
        {
            pg4p_eta.push_back(PrimaryG4P_eta->at(j));
            pg4p_phi.push_back(PrimaryG4P_phi->at(j));
        }

        for (size_t j = 0; j < GenHadron_eta->size(); j++)
        {
            gh_eta.push_back(GenHadron_eta->at(j));
            gh_phi.push_back(GenHadron_phi->at(j));
        }

        std::cout << "ntkl = " << ntkl << "; ntkl_matchedtrack = " << ntkl_matchedtrack << "; ntkl_unmatchedtrack = " << ntkl_unmatchedtrack << "; ntkl_matchedtrack_positiveID = " << ntkl_matchedtrack_positiveID << "; ntkl_matchedtrack_negativeID = " << ntkl_matchedtrack_negativeID << std::endl;

        // TScatter objectt
        vector<double> evtvtx_x = {PV_x};
        vector<double> evtvtx_y = {PV_y};
        vector<double> evtvtx_z = {PV_z};
        vector<double> evtvtx_rho = {sqrt(PV_x * PV_x + PV_y * PV_y)};
        TScatter *scat_evtvertex_xy = new TScatter(1, &evtvtx_x[0], &evtvtx_y[0]);
        TScatter *scat_evtvertex_zrho = new TScatter(1, &evtvtx_z[0], &evtvtx_rho[0]);

        TScatter *scat_xy_matched_posTrackID = new TScatter(tkl_clusx_matched_posTrackID.size(), tkl_clusx_matched_posTrackID.data(), tkl_clusy_matched_posTrackID.data());
        TScatter *scat_xy_matched_negTrackID = new TScatter(tkl_clusx_matched_negTrackID.size(), tkl_clusx_matched_negTrackID.data(), tkl_clusy_matched_negTrackID.data());
        TScatter *scat_xy_unmatched = new TScatter(tkl_clusx_unmatched.size(), tkl_clusx_unmatched.data(), tkl_clusy_unmatched.data());
        TScatter *scat_zrho_matched_posTrackID = new TScatter(tkl_clusz_matched_posTrackID.size(), tkl_clusz_matched_posTrackID.data(), tkl_clusrho_matched_posTrackID.data());
        TScatter *scat_zrho_matched_negTrackID = new TScatter(tkl_clusz_matched_negTrackID.size(), tkl_clusz_matched_negTrackID.data(), tkl_clusrho_matched_negTrackID.data());
        TScatter *scat_zrho_unmatched = new TScatter(tkl_clusz_unmatched.size(), tkl_clusz_unmatched.data(), tkl_clusrho_unmatched.data());

        TScatter *scat_recotkl_clusetaphi_matched_posTrackID = new TScatter(tkl_cluseta_matched_posTrackID.size(), tkl_cluseta_matched_posTrackID.data(), tkl_clusphi_matched_posTrackID.data());
        TScatter *scat_recotkl_clusetaphi_matched_negTrackID = new TScatter(tkl_cluseta_matched_negTrackID.size(), tkl_cluseta_matched_negTrackID.data(), tkl_clusphi_matched_negTrackID.data());
        TScatter *scat_recotkl_clusetaphi_unmatched = new TScatter(tkl_cluseta_unmatched.size(), tkl_cluseta_unmatched.data(), tkl_clusphi_unmatched.data());
        TScatter *scat_primaryG4P_etaphi = new TScatter(pg4p_eta.size(), pg4p_eta.data(), pg4p_phi.data());
        TScatter *scat_genHadron_etaphi = new TScatter(gh_eta.size(), gh_eta.data(), gh_phi.data());
        TScatter *scat_tklmatchedg4p_etaphi_posTrackID = new TScatter(tklmatchedg4p_eta_posTrackID.size(), tklmatchedg4p_eta_posTrackID.data(), tklmatchedg4p_phi_posTrackID.data());
        TScatter *scat_tklmatchedg4p_etaphi_negTrackID = new TScatter(tklmatchedg4p_eta_negTrackID.size(), tklmatchedg4p_eta_negTrackID.data(), tklmatchedg4p_phi_negTrackID.data());

        TGraph2D *gr2D_evtvtx = new TGraph2D(evtvtx_x.size(), evtvtx_z.data(), evtvtx_y.data(), evtvtx_x.data());
        TGraph2D *gr2D_matched_posTrackID = new TGraph2D(tkl_clusx_matched_posTrackID.size(), tkl_clusz_matched_posTrackID.data(), tkl_clusy_matched_posTrackID.data(), tkl_clusx_matched_posTrackID.data());
        TGraph2D *gr2D_matched_negTrackID = new TGraph2D(tkl_clusx_matched_negTrackID.size(), tkl_clusz_matched_negTrackID.data(), tkl_clusy_matched_negTrackID.data(), tkl_clusx_matched_negTrackID.data());
        TGraph2D *gr2D_unmatched = new TGraph2D(tkl_clusx_unmatched.size(), tkl_clusz_unmatched.data(), tkl_clusy_unmatched.data(), tkl_clusx_unmatched.data());

        // drawAndSaveCanvas(std::pair<int, int> canvasSize, std::vector<TScatter *> &vec_tscat, std::vector<TLine *> &tline, std::vector<std::string> &v_leg, const std::string &xtitle, const std::string &ytitle, const std::string &plotname)
        std::pair<int, int> canvasSize_xy{550, 700};
        scat_evtvertex_xy->SetMarkerColor(TColor::GetColor("#393E46"));
        scat_evtvertex_xy->SetMarkerStyle(34);
        scat_xy_matched_posTrackID->SetMarkerColor(TColor::GetColor("#99cc99"));
        scat_xy_matched_negTrackID->SetMarkerColor(TColor::GetColor("#6699cc"));
        scat_xy_unmatched->SetMarkerColor(TColor::GetColor("#f2777a"));
        std::vector<TScatter *> vec_tscat_xy{scat_evtvertex_xy, scat_xy_matched_posTrackID, scat_xy_matched_negTrackID, scat_xy_unmatched};
        std::tuple<float, float, float, float> axisrange_xy{-12, 12, -12, 12};
        drawAndSaveCanvas(canvasSize_xy, axisrange_xy, vec_tscat_xy, tklline, std::vector<std::string>{"Event vertex", "Tracklet - matched to G4P with trackID>0", "Tracklet - matched to G4P with trackID<0", "Tracklet - not matched to G4P"}, "x [cm]", "y [cm]", plotdir + "/tkl_xy_scatter");

        std::pair<int, int> canvasSize_zrho{800, 500};
        scat_evtvertex_zrho->SetMarkerColor(TColor::GetColor("#393E46"));
        scat_evtvertex_zrho->SetMarkerStyle(34);
        scat_zrho_matched_posTrackID->SetMarkerColor(TColor::GetColor("#99cc99"));
        scat_zrho_matched_negTrackID->SetMarkerColor(TColor::GetColor("#6699cc"));
        scat_zrho_unmatched->SetMarkerColor(TColor::GetColor("#f2777a"));
        vector<TScatter *> vec_tscat_zrho{scat_evtvertex_zrho, scat_zrho_matched_posTrackID, scat_zrho_matched_negTrackID, scat_zrho_unmatched};
        std::tuple<float, float, float, float> axisrange_zrho{-24, 24, -12, 12};
        drawAndSaveCanvas(canvasSize_zrho, axisrange_zrho, vec_tscat_zrho, tklline_zrho, std::vector<std::string>{"Event vertex", "Tracklet - matched to G4P with trackID>0", "Tracklet - matched to G4P with trackID<0", "Tracklet - not matched to G4P"}, "z [cm]", "Radius [cm]", plotdir + "/tkl_zrho_scatter");

        std::pair<int, int> canvasSize_etaphi{700, 500};
        scat_recotkl_clusetaphi_matched_posTrackID->SetMarkerColor(TColor::GetColor("#99cc99"));
        scat_recotkl_clusetaphi_matched_negTrackID->SetMarkerColor(TColor::GetColor("#6699cc"));
        scat_recotkl_clusetaphi_unmatched->SetMarkerColor(TColor::GetColor("#f2777a"));
        scat_primaryG4P_etaphi->SetMarkerColor(TColor::GetColor("#7F167F"));
        scat_primaryG4P_etaphi->SetMarkerStyle(21);
        std::vector<TScatter *> vec_tscat_etaphi_primaryG4P{scat_recotkl_clusetaphi_matched_posTrackID, scat_recotkl_clusetaphi_matched_negTrackID, scat_recotkl_clusetaphi_unmatched, scat_primaryG4P_etaphi};
        std::tuple<float, float, float, float> axisrange_etaphi{-2, 2, -3.5, 3.5};
        drawAndSaveCanvas(canvasSize_etaphi, axisrange_etaphi, vec_tscat_etaphi_primaryG4P, tline_etaphi, std::vector<std::string>{"Tracklet - matched to G4P with trackID>0", "Tracklet - matched to G4P with trackID<0", "Tracklet - not matched to G4P", "Primary PHG4Particle"}, "#eta", "#phi", plotdir + "/tkl_etaphi_scatter_primaryG4P");

        scat_recotkl_clusetaphi_matched_posTrackID->SetMarkerColor(TColor::GetColor("#99cc99"));
        scat_recotkl_clusetaphi_matched_negTrackID->SetMarkerColor(TColor::GetColor("#6699cc"));
        scat_recotkl_clusetaphi_unmatched->SetMarkerColor(TColor::GetColor("#f2777a"));
        scat_genHadron_etaphi->SetMarkerColor(TColor::GetColor("#7F167F"));
        scat_genHadron_etaphi->SetMarkerStyle(21);
        std::vector<TScatter *> vec_tscat_etaphi_genhadron{scat_recotkl_clusetaphi_matched_posTrackID, scat_recotkl_clusetaphi_matched_negTrackID, scat_recotkl_clusetaphi_unmatched, scat_genHadron_etaphi};
        drawAndSaveCanvas(canvasSize_etaphi, axisrange_etaphi, vec_tscat_etaphi_genhadron, tline_etaphi, std::vector<std::string>{"Tracklet - matched to G4P with trackID>0", "Tracklet - matched to G4P with trackID<0", "Tracklet - not matched to G4P", "Generated charged hadron"}, "#eta", "#phi", plotdir + "/tkl_etaphi_scatter_genhadron");

        scat_recotkl_clusetaphi_matched_posTrackID->SetMarkerColor(TColor::GetColor("#99cc99"));
        scat_tklmatchedg4p_etaphi_posTrackID->SetMarkerColor(TColor::GetColor("#5E8B7E"));
        scat_tklmatchedg4p_etaphi_posTrackID->SetMarkerStyle(21);
        std::vector<TScatter *> vec_tscat_etaphi_tklmatchedg4p_posTrackID{scat_recotkl_clusetaphi_matched_posTrackID, scat_tklmatchedg4p_etaphi_posTrackID};
        drawAndSaveCanvas(canvasSize_etaphi, axisrange_etaphi, vec_tscat_etaphi_tklmatchedg4p_posTrackID, tline_etaphi_matched_posTrackID, std::vector<std::string>{"Tracklet - matched to G4P with trackID>0", "Matched PHG4Particle with trackID>0"}, "#eta", "#phi", plotdir + "/tkl_etaphi_scatter_tklmatchedg4p_posTrackID");

        scat_recotkl_clusetaphi_matched_negTrackID->SetMarkerColor(TColor::GetColor("#6699cc"));
        scat_tklmatchedg4p_etaphi_negTrackID->SetMarkerColor(TColor::GetColor("#0F4C75"));
        scat_tklmatchedg4p_etaphi_negTrackID->SetMarkerStyle(21);
        std::vector<TScatter *> vec_tscat_etaphi_tklmatchedg4p_negTrackID{scat_recotkl_clusetaphi_matched_negTrackID, scat_tklmatchedg4p_etaphi_negTrackID};
        drawAndSaveCanvas(canvasSize_etaphi, axisrange_etaphi, vec_tscat_etaphi_tklmatchedg4p_negTrackID, tline_etaphi_matched_negTrackID, std::vector<std::string>{"Tracklet - matched to G4P with trackID<0", "Matched PHG4Particle with trackID<0"}, "#eta", "#phi", plotdir + "/tkl_etaphi_scatter_tklmatchedg4p_negTrackID");

        std::pair<int, int> canvasSize_3D{700, 350};
        gr2D_evtvtx->SetMarkerColor(TColor::GetColor("#393E46"));
        gr2D_evtvtx->SetMarkerStyle(34);
        gr2D_matched_posTrackID->SetMarkerColor(TColor::GetColor("#99cc99"));
        gr2D_matched_negTrackID->SetMarkerColor(TColor::GetColor("#6699cc"));
        gr2D_unmatched->SetMarkerColor(TColor::GetColor("#f2777a"));
        std::vector<TGraph2D *> vec_tg2D{gr2D_evtvtx, gr2D_matched_posTrackID, gr2D_matched_negTrackID, gr2D_unmatched};
        std::tuple<float, float, float, float, float, float> axisrange_3D{-25, 25, -12, 12, -12, 12};
        drawTGraph2D(canvasSize_3D, axisrange_3D, vec_tg2D, tkl3Dline, std::vector<std::string>{"Event vertex", "Tracklet - matched to G4P with trackID>0", "Tracklet - matched to G4P with trackID<0", "Tracklet - not matched to G4P"}, "z [cm]", "y [cm]", "x [cm]", plotdir + "/tkl_3D_scatter");
    }

    f->Close();
}