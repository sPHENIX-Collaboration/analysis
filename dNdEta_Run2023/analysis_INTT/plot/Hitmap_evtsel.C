#include <TCanvas.h>
#include <TFile.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TTree.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

float TickSize = 0.03;
float AxisTitleSize = 0.05;
float AxisLabelSize = 0.04;
float LeftMargin = 0.15;
float RightMargin = 0.14;
float TopMargin = 0.08;
float BottomMargin = 0.13;

int m_inttFeeOffset = 23; // 23 is the offset for INTT in streaming mode; https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/offline/packages/intt/InttCombinedRawDataDecoder.h#L67C3-L67C76

std::vector<int> colorlist = {kTVibBlue, kTVibTeal, kTVibOrange, kTBriPurple};

// void Draw_2Dhist(TH2F *hist, bool IsData, bool logz, bool norm1, float rmargin, const char *XaxisName, const char *YaxisName, const char *ZaxisName, const char *drawopt, const char *outname)
// {
//     // get the INTT BCO number from the histogram name: split the name by "_", the bco number is the last element
//     std::string histname = hist->GetName();
//     std::string bco = histname.substr(histname.find_last_of("_") + 1);

//     TCanvas *c = new TCanvas("c", "c", 1000, 700);
//     if (logz)
//     {
//         c->SetLogz();
//     }
//     c->cd();
//     if (strcmp(ZaxisName, "") == 0)
//     {
//         gPad->SetRightMargin(rmargin);
//     }
//     else
//     {
//         gPad->SetRightMargin(rmargin + 0.03);
//     }

//     gPad->SetTopMargin(TopMargin);
//     gPad->SetLeftMargin(LeftMargin);
//     gPad->SetBottomMargin(BottomMargin);
//     if (norm1)
//     {
//         hist->Scale(1. / hist->Integral(-1, -1, -1, -1));
//     }
//     hist->GetXaxis()->SetTitle(XaxisName);
//     hist->GetYaxis()->SetTitle(YaxisName);
//     hist->GetXaxis()->SetTickSize(TickSize);
//     hist->GetYaxis()->SetTickSize(TickSize);
//     hist->GetXaxis()->SetTitleSize(AxisTitleSize);
//     hist->GetYaxis()->SetTitleSize(AxisTitleSize);
//     hist->GetXaxis()->SetLabelSize(AxisLabelSize);
//     hist->GetYaxis()->SetLabelSize(AxisLabelSize);
//     if (strcmp(ZaxisName, "") != 0)
//     {
//         hist->GetZaxis()->SetTitle(ZaxisName);
//         hist->GetZaxis()->SetTitleSize(AxisTitleSize);
//         hist->GetZaxis()->SetTitleOffset(1.1);
//     }

//     hist->GetXaxis()->SetTitleOffset(1.1);
//     hist->GetYaxis()->SetTitleOffset(1.3);
//     hist->GetZaxis()->SetLabelSize(AxisLabelSize);
//     // hist->GetZaxis()->SetRangeUser(hist->GetMinimum(0), hist->GetMaximum());
//     hist->SetContour(1000);
//     hist->Draw(drawopt);

//     // Draw the INTT BCO number
//     TLatex *tex = new TLatex();
//     tex->SetNDC();
//     // tex->SetTextFont(43);
//     tex->SetTextSize(0.035);
//     tex->DrawLatex(0.2, 0.8, Form("INTT BCO: %s", bco.c_str()));

//     c->RedrawAxis();
//     c->Draw();
//     c->SaveAs((std::string(outname) + ".pdf").c_str());
//     c->SaveAs((std::string(outname) + ".png").c_str());
//     if (c)
//     {
//         c->Close();
//         gSystem->ProcessEvents();
//         delete c;
//         c = nullptr;
//         gSystem->ProcessEvents();
//     }
// }

TCanvas *set2Dhist(TH2F *hist, bool IsBadCorr, bool IsData, bool logz, bool norm1, float rmargin, const char *XaxisName, const char *YaxisName, const char *ZaxisName, const char *drawopt, std::vector<std::string> plotinfo)
{
    // get the INTT BCO number from the histogram name: split the name by "_", the bco number is the last element
    std::string histname = hist->GetName();
    std::string bco = histname.substr(histname.find_last_of("_") + 1);

    TCanvas *c = new TCanvas(Form("c_%s", histname.c_str()), Form("c_%s", histname.c_str()), 1000, 700);
    if (logz)
    {
        c->SetLogz();
    }
    c->cd();
    if (strcmp(ZaxisName, "") == 0)
    {
        gPad->SetRightMargin(rmargin);
    }
    else
    {
        gPad->SetRightMargin(rmargin + 0.03);
    }

    gPad->SetTopMargin(TopMargin);
    gPad->SetLeftMargin(LeftMargin);
    gPad->SetBottomMargin(BottomMargin);
    if (norm1)
    {
        hist->Scale(1. / hist->Integral(-1, -1, -1, -1));
    }
    hist->GetXaxis()->SetTitle(XaxisName);
    hist->GetYaxis()->SetTitle(YaxisName);
    hist->GetXaxis()->SetTickSize(TickSize);
    hist->GetYaxis()->SetTickSize(TickSize);
    hist->GetXaxis()->SetTitleSize(AxisTitleSize);
    hist->GetYaxis()->SetTitleSize(AxisTitleSize);
    hist->GetXaxis()->SetLabelSize(AxisLabelSize);
    hist->GetYaxis()->SetLabelSize(AxisLabelSize);
    if (strcmp(ZaxisName, "") != 0)
    {
        hist->GetZaxis()->SetTitle(ZaxisName);
        hist->GetZaxis()->SetTitleSize(AxisTitleSize);
        hist->GetZaxis()->SetTitleOffset(1.1);
    }

    hist->GetXaxis()->SetTitleOffset(1.1);
    hist->GetYaxis()->SetTitleOffset(1.3);
    hist->GetZaxis()->SetLabelSize(AxisLabelSize);
    // hist->GetZaxis()->SetRangeUser(hist->GetMinimum(0), hist->GetMaximum());
    hist->SetContour(1000);
    hist->Draw(drawopt);

    // Draw the plotinfo strings
    TLatex *tex = new TLatex();
    tex->SetNDC();
    // tex->SetTextFont(43);
    tex->SetTextSize(0.03);
    if (IsBadCorr)
    {
        tex->SetTextColor(kRed);
        for (int i = 0; i < plotinfo.size(); i++)
        {
            tex->DrawLatex(0.18, 0.87 - 0.03 * i, plotinfo[i].c_str());
        }
    }
    else
    {
        for (int i = 0; i < plotinfo.size(); i++)
        {
            tex->DrawLatex(0.18, 0.87 - 0.03 * i, plotinfo[i].c_str());
        }
    }

    c->RedrawAxis();

    return c;
}

void Hitmap_evtsel()
{
    bool plothitmap = true;

    gStyle->SetPalette(kRainBow);

    // TString filename = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/From_official_INTTRAWHIT_DST_ColdHotChannelMask/completed/Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_00000.root";
    // TString filename = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/From_official_INTTRAWHIT_DST_ColdHotChannelMask/completed/Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_merged.root";
    std::string plotdir = "./TrkrHit/Hitmap_InnerOuterCorrBranch_perBCO_Run54280/";
    system(("mkdir -p " + plotdir).c_str());

    int z_offset[4] = {5, 0, 13, 21};
    int layer_offset[4] = {0, 12, 24, 40};
    int layerhitset_offset[4] = {0, 48, 96, 160};

    // integrated hit map for non-corr branch
    TH2F *hM_hitmap_RowColumnFlatten_noncorr = new TH2F("hM_hitmap_RowColumnFlatten_noncorr", "hM_hitmap_RowColumnFlatten_noncorr", 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);
    TH2F *hM_hitmap_RowColumnFlatten_noncorr_weightAdc = new TH2F("hM_hitmap_RowColumnFlatten_noncorr_weightAdc", "hM_hitmap_RowColumnFlatten_noncorr_weightAdc", 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);
    TH2F *hM_hitmap_RowColumnFlatten_pm4noncorr = new TH2F("hM_hitmap_RowColumnFlatten_pm4noncorr", "hM_hitmap_RowColumnFlatten_pm4noncorr", 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);
    TH2F *hM_hitmap_RowColumnFlatten_pm4noncorr_weightAdc = new TH2F("hM_hitmap_RowColumnFlatten_pm4noncorr_weightAdc", "hM_hitmap_RowColumnFlatten_pm4noncorr_weightAdc", 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);

    // hit adc distribution
    TH1F *hM_TrkrhitADC_noncorr = new TH1F("hM_TrkrhitADC_noncorr", "hM_TrkrhitADC_noncorr", 251, -0.5, 250.5);
    TH1F *hM_TrkrhitADC_pm4noncorr = new TH1F("hM_TrkrhitADC_pm4noncorr", "hM_TrkrhitADC_pm4noncorr", 251, -0.5, 250.5);

    // event bco difference = BCO of (i+1)th event - BCO of ith event
    TH1F *hM_BCOdiff_noncorrnext = new TH1F("hM_BCOdiff_noncorrnext", "hM_BCOdiff_noncorrnext", 131, -0.5, 130.5);

    // TH2F *hM_HitTimeBucket_Layer_noncorrnext = new TH2F("hM_HitTimeBucket_Layer_noncorrnext", "hM_HitTimeBucket_Layer_noncorrnext", 131, -0.5, 130.5, 4, 2.5, 6.5);
    std::vector<TH1F *> list_hM_HitTimeBucket_Layer_noncorrnext;
    for (int l = 0; l < 4; l++)
    {
        list_hM_HitTimeBucket_Layer_noncorrnext.push_back(new TH1F(Form("hM_HitTimeBucket_Layer_noncorrnext_Layer%d", l + 3), Form("hM_HitTimeBucket_Layer_noncorrnext_Layer%d", l + 3), 131, -0.5, 130.5));
    }

    // output text file
    std::ofstream outtxtfile(Form("%s/EventList.txt", plotdir.c_str()), std::ios::out);

    // TFile *fin = new TFile(filename, "READ");
    // TTree *t = (TTree *)fin->Get("EventTree");
    TChain *t = new TChain("EventTree");
    t->Add("/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Data_Run54280_20241127_ProdA2024/ntuple_00*.root");
    int event;
    float MBD_z_vtx;
    int NTrkrhits, NClus_Layer1, NClus;
    uint64_t INTT_BCO;
    bool is_min_bias;
    vector<int> *firedTriggers = 0;
    std::vector<uint64_t> *InttRawHit_bco = 0;
    std::vector<uint16_t> *InttRawHit_FPHX_BCO = 0;
    std::vector<uint16_t> *TrkrHitRow = 0, *TrkrHitColumn = 0, *TrkrHitADC = 0;
    std::vector<uint8_t> *TrkrHitLadderZId = 0, *TrkrHitLadderPhiId = 0, *TrkrHitLayer = 0;
    std::vector<float> *ClusPhiSize = 0;
    t->SetBranchAddress("event", &event);
    t->SetBranchAddress("is_min_bias", &is_min_bias);
    t->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    t->SetBranchAddress("INTT_BCO", &INTT_BCO);
    t->SetBranchAddress("NTrkrhits", &NTrkrhits);
    t->SetBranchAddress("NClus_Layer1", &NClus_Layer1);
    t->SetBranchAddress("NClus", &NClus);
    t->SetBranchAddress("firedTriggers", &firedTriggers);
    if (plothitmap)
    {
        t->SetBranchAddress("InttRawHit_bco", &InttRawHit_bco);
        t->SetBranchAddress("InttRawHit_FPHX_BCO", &InttRawHit_FPHX_BCO);
        t->SetBranchAddress("TrkrHitRow", &TrkrHitRow);
        t->SetBranchAddress("TrkrHitColumn", &TrkrHitColumn);
        t->SetBranchAddress("TrkrHitADC", &TrkrHitADC);
        t->SetBranchAddress("TrkrHitLadderZId", &TrkrHitLadderZId);
        t->SetBranchAddress("TrkrHitLadderPhiId", &TrkrHitLadderPhiId);
        t->SetBranchAddress("TrkrHitLayer", &TrkrHitLayer);
        t->SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    }

    // Pre-allocate vectors to avoid reallocations
    int Nbco2plot = 1500;
    std::map<int, uint64_t> map_evtbco;

    bool fillbcodiffnext = false;
    uint64_t bco_ofinterest = 0;

    // Get total entries once instead of calling each time
    Long64_t nEntries = t->GetEntries();
    std::cout << "Total entries: " << nEntries << std::endl;

    // Cache divisions and comparisons
    const float outer_inner_ratio_threshold = 1.2f;
    const float mbd_z_vtx_threshold = 10.0f;
    const int nclus_layer1_threshold = 500;

    for (Long64_t ev = 0; ev < nEntries; ++ev)
    {
        if (map_evtbco.size() >= Nbco2plot)
            break;

        t->GetEntry(ev);

        if (fillbcodiffnext)
        {
            hM_BCOdiff_noncorrnext->Fill(INTT_BCO - bco_ofinterest);
            fillbcodiffnext = false;
        }

        // Combine conditions to reduce branches
        // bool passes_cuts = (is_min_bias) && (std::abs(MBD_z_vtx) <= mbd_z_vtx_threshold) && (NClus_Layer1 >= nclus_layer1_threshold) && ((static_cast<float>(NClus - NClus_Layer1) / NClus_Layer1) >= outer_inner_ratio_threshold);
        // (NClus_Layer1<(NClus-NClus_Layer1)-5*TMath::Sqrt(NClus-NClus_Layer1))
        bool passes_cuts = (is_min_bias && NClus_Layer1 < (NClus - NClus_Layer1) - 5 * TMath::Sqrt(NClus - NClus_Layer1));

        if (!passes_cuts)
            continue;

        cout << "Event: " << ev << " , map_evtbco.size(): " << map_evtbco.size() << ", INTT_BCO: " << INTT_BCO << " NClus (inner): " << NClus_Layer1 << " NClus (outer): " << (NClus - NClus_Layer1) << " Outer/Inner: " << static_cast<float>(NClus - NClus_Layer1) / static_cast<float>(NClus_Layer1) << endl;
        cout << "Event: " << ev << " has trigger bit: ";
        for (int j = 0; j < firedTriggers->size(); j++)
        {
            cout << firedTriggers->at(j) << " ";
        }
        cout << endl;

        if (plothitmap)
            map_evtbco[ev] = INTT_BCO;

        bco_ofinterest = INTT_BCO;
        fillbcodiffnext = true;
    }

    if (plothitmap)
    {
        for (auto [ev, inttbco] : map_evtbco)
        {
            uint64_t bco_ofinterest = inttbco;

            TH1F *hM_hitTimebucket_noncorr = new TH1F(Form("hM_hitTimebucket_noncorr_%lu", bco_ofinterest), Form("hM_hitTimebucket_noncorr_%lu", bco_ofinterest), 131, -0.5, 130.5);
            TH1F *hM_hitTimebucket_noncorrnext = new TH1F(Form("hM_hitTimebucket_noncorrnext_%lu", bco_ofinterest), Form("hM_hitTimebucket_noncorrnext_%lu", bco_ofinterest), 131, -0.5, 130.5);
            TH1F *hM_hitTimebucket_noncorrnext_wbcointerest = new TH1F(Form("hM_hitTimebucket_noncorrnext_wbcointerest_%lu", bco_ofinterest), Form("hM_hitTimebucket_noncorrnext_wbcointerest_%lu", bco_ofinterest), 131, -0.5, 130.5);

            // look at +-5 events around the selected event
            for (int i = -5; i <= 5; i++)
            {
                if (ev + i < 0 || ev + i >= t->GetEntriesFast())
                    continue;
                
                std::cout << "Processing event, BCO " << inttbco << std::endl;
                t->GetEntry(ev + i);

                TH2F *hM_hitmap_RowColumnFlatten = new TH2F(Form("hM_hitmap_RowColumnFlatten_%lu", INTT_BCO), Form("hM_hitmap_RowColumnFlatten_%lu", INTT_BCO), 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);
                TH2F *hM_hitmap_RowColumnFlatten_weighttimebucket = new TH2F(Form("hM_hitmap_RowColumnFlatten_weighttimebucket_%lu", INTT_BCO), Form("hM_hitmap_RowColumnFlatten_weighttimebucket_%lu", INTT_BCO), 4 * 32, 0, 4 * 32, 360 * 16, 0, 360 * 16);

                float avg_trkrhitadc = 0;
                for (int h = 0; h < NTrkrhits; h++)
                {
                    int flat_x = TrkrHitColumn->at(h) + z_offset[TrkrHitLadderZId->at(h)] + 32 * (TrkrHitLayer->at(h) - 3);
                    int flat_y = TrkrHitRow->at(h) + 360 * TrkrHitLadderPhiId->at(h);
                    int time_bucket = (InttRawHit_FPHX_BCO->at(h) - (InttRawHit_bco->at(h) & 0x7fU) - m_inttFeeOffset + 128) % 128;

                    hM_hitmap_RowColumnFlatten->Fill(flat_x, flat_y, TrkrHitADC->at(h) + 1);
                    hM_hitmap_RowColumnFlatten_weighttimebucket->Fill(flat_x, flat_y, time_bucket);
                    avg_trkrhitadc += (TrkrHitADC->at(h) + 1);

                    if (i == 0)
                    {
                        if (h == 0)
                        {
                            // print to output text file
                            outtxtfile << "EOI, Event: " << event << ", INTT_BCO: " << INTT_BCO << " has trigger bit: ";
                            for (int j = 0; j < firedTriggers->size(); j++)
                            {
                                outtxtfile << firedTriggers->at(j) << " ";
                            }
                            outtxtfile << endl;
                        }

                        if (is_min_bias)
                        {
                            hM_hitmap_RowColumnFlatten_noncorr->Fill(flat_x, flat_y);
                            hM_hitmap_RowColumnFlatten_noncorr_weightAdc->Fill(flat_x, flat_y, TrkrHitADC->at(h) + 1);
                            hM_TrkrhitADC_noncorr->Fill(TrkrHitADC->at(h) + 1);
                        }

                        int time_bucket = (InttRawHit_FPHX_BCO->at(h) - (InttRawHit_bco->at(h) & 0x7fU) - m_inttFeeOffset + 128) % 128;
                        hM_hitTimebucket_noncorr->Fill(time_bucket);
                    }
                    else
                    {
                        if (i == 1)
                        {
                            if (h == 0)
                            {
                                outtxtfile << "Event after EOI, Event: " << event << ", INTT_BCO: " << INTT_BCO << " has trigger bit: ";
                                for (int j = 0; j < firedTriggers->size(); j++)
                                {
                                    outtxtfile << firedTriggers->at(j) << " ";
                                }
                                outtxtfile << endl;
                            }

                            int time_bucket = (InttRawHit_FPHX_BCO->at(h) - (InttRawHit_bco->at(h) & 0x7fU) - m_inttFeeOffset + 128) % 128;
                            hM_hitTimebucket_noncorrnext->Fill(time_bucket);
                            int time_bucket_wbcointerest = (InttRawHit_FPHX_BCO->at(h) - (bco_ofinterest & 0x7fU) - m_inttFeeOffset + 128) % 128;
                            hM_hitTimebucket_noncorrnext_wbcointerest->Fill(time_bucket_wbcointerest);

                            // hM_HitTimeBucket_Layer_noncorrnext->Fill(time_bucket, TrkrHitLayer->at(h));
                            list_hM_HitTimeBucket_Layer_noncorrnext[TrkrHitLayer->at(h) - 3]->Fill(time_bucket);
                        }

                        if (is_min_bias)
                        {
                            hM_hitmap_RowColumnFlatten_pm4noncorr->Fill(flat_x, flat_y);
                            hM_hitmap_RowColumnFlatten_pm4noncorr_weightAdc->Fill(flat_x, flat_y, TrkrHitADC->at(h) + 1);
                            hM_TrkrhitADC_pm4noncorr->Fill(TrkrHitADC->at(h) + 1);
                        }
                    }
                }

                avg_trkrhitadc /= NTrkrhits;

                // plotinfo strings
                std::string evtinfo = Form("Event Id: %d", event);
                std::string bcoinfo = (INTT_BCO == bco_ofinterest) ? Form("BCO*: %lu", INTT_BCO) : Form("BCO: %lu", INTT_BCO);
                std::string nclusinfo1 = Form("Number of clusters (inner): %d", NClus_Layer1);
                std::string nclusinfo2 = Form("Number of clusters (outer): %d", NClus - NClus_Layer1);
                std::string nclusinfo3 = Form("Outer/Inner: %.4f", static_cast<float>(NClus - NClus_Layer1) / static_cast<float>(NClus_Layer1));
                std::string avgadcinfo = Form("Average TrkrHit ADC: %.2f", avg_trkrhitadc);
                std::string minbiasinfo = (is_min_bias) ? "Is Min Bias" : "Is not Min Bias";
                std::vector<std::string> plotinfo = {evtinfo, bcoinfo, nclusinfo1, nclusinfo2, nclusinfo3, minbiasinfo};

                TCanvas *c = set2Dhist(hM_hitmap_RowColumnFlatten, (INTT_BCO == bco_ofinterest), true, false, false, RightMargin, "(z and layer) index", "phi index", "TrkrHit ADC", "colz", plotinfo);
                hM_hitmap_RowColumnFlatten_weighttimebucket->GetZaxis()->SetRangeUser(0, 128);
                // hM_hitmap_RowColumnFlatten_weighttimebucket->SetContour(6);
                TCanvas *c_weighttimebucket = set2Dhist(hM_hitmap_RowColumnFlatten_weighttimebucket, (INTT_BCO == bco_ofinterest), true, false, false, RightMargin, "(z and layer) index", "phi index", "Time bucket", "colz", plotinfo);

                if (i == -5)
                {
                    c->Print((plotdir + Form("Hitmap_evtsel_%lu.pdf[", bco_ofinterest)).c_str());
                    c_weighttimebucket->Print((plotdir + Form("Hitmap_evtsel_weighttimebucket_%lu.pdf[", bco_ofinterest)).c_str());
                }
                else if (i == 5)
                {
                    c->Print((plotdir + Form("Hitmap_evtsel_%lu.pdf]", bco_ofinterest)).c_str());
                    c_weighttimebucket->Print((plotdir + Form("Hitmap_evtsel_weighttimebucket_%lu.pdf]", bco_ofinterest)).c_str());
                }
                else
                {
                    c->Print((plotdir + Form("Hitmap_evtsel_%lu.pdf", bco_ofinterest)).c_str());
                    c_weighttimebucket->Print((plotdir + Form("Hitmap_evtsel_weighttimebucket_%lu.pdf", bco_ofinterest)).c_str());
                }

                delete hM_hitmap_RowColumnFlatten;
                delete hM_hitmap_RowColumnFlatten_weighttimebucket;

                if (c)
                {
                    c->Close();
                    gSystem->ProcessEvents();
                    delete c;
                    c = nullptr;
                    gSystem->ProcessEvents();
                }

                if (c_weighttimebucket)
                {
                    c_weighttimebucket->Close();
                    gSystem->ProcessEvents();
                    delete c_weighttimebucket;
                    c_weighttimebucket = nullptr;
                    gSystem->ProcessEvents();
                }
            }

            TCanvas *c_timebucket = new TCanvas("c_timebucket", "c_timebucket", 800, 700);
            c_timebucket->cd();
            // c_timebucket->SetLogy();
            hM_hitTimebucket_noncorr->GetXaxis()->SetTitle("Time bucket");
            hM_hitTimebucket_noncorr->GetYaxis()->SetTitle("Entries");
            hM_hitTimebucket_noncorr->GetXaxis()->SetTitleOffset(1.3);
            hM_hitTimebucket_noncorr->GetYaxis()->SetTitleOffset(1.5);
            hM_hitTimebucket_noncorr->GetYaxis()->SetRangeUser(0, hM_hitTimebucket_noncorr->GetMaximum() * 1.4);
            hM_hitTimebucket_noncorr->SetLineColor(kTBriBlue);
            hM_hitTimebucket_noncorr->SetLineWidth(2);
            hM_hitTimebucket_noncorr->SetMarkerColor(kTBriBlue);
            hM_hitTimebucket_noncorr->SetMarkerStyle(20);
            hM_hitTimebucket_noncorr->SetMarkerSize(1);
            hM_hitTimebucket_noncorrnext->SetLineColor(kTBriRed);
            hM_hitTimebucket_noncorrnext->SetLineWidth(2);
            hM_hitTimebucket_noncorrnext->SetMarkerColor(kTBriRed);
            hM_hitTimebucket_noncorrnext->SetMarkerStyle(20);
            hM_hitTimebucket_noncorrnext->SetMarkerSize(1);
            hM_hitTimebucket_noncorrnext_wbcointerest->SetLineColor(kTBriGreen);
            hM_hitTimebucket_noncorrnext_wbcointerest->SetLineWidth(2);
            hM_hitTimebucket_noncorrnext_wbcointerest->SetMarkerColor(kTBriGreen);
            hM_hitTimebucket_noncorrnext_wbcointerest->SetMarkerStyle(20);
            hM_hitTimebucket_noncorrnext_wbcointerest->SetMarkerSize(1);
            hM_hitTimebucket_noncorr->Draw("PE");
            hM_hitTimebucket_noncorrnext->Draw("PE same");
            hM_hitTimebucket_noncorrnext_wbcointerest->Draw("PE same");
            c_timebucket->RedrawAxis();

            static TLegend *leg = nullptr;
            if (!leg)
            {
                leg = new TLegend(0.35, 0.8, 0.65, 0.9);
                leg->AddEntry(hM_hitTimebucket_noncorr, "Event of interest (EOI)", "pel");
                leg->AddEntry(hM_hitTimebucket_noncorrnext, "Next event", "pel");
                leg->AddEntry(hM_hitTimebucket_noncorrnext_wbcointerest, "Next event, w.r.t the BCO of EOI", "pel");
                leg->SetBorderSize(0);
                leg->SetFillStyle(0);
                leg->SetTextSize(0.04);
            }
            leg->Draw();

            std::string pdfPath = plotdir + Form("HitTimebucket_evtsel_%lu.pdf", bco_ofinterest);
            std::string pngPath = plotdir + Form("HitTimebucket_evtsel_%lu.png", bco_ofinterest);
            c_timebucket->SaveAs(pdfPath.c_str());
            c_timebucket->SaveAs(pngPath.c_str());

            if (c_timebucket)
            {
                c_timebucket->Close();
                gSystem->ProcessEvents();
                delete c_timebucket;
                c_timebucket = nullptr;
                gSystem->ProcessEvents();
            }
        }

        // Create and save hit map plots
        auto saveHitMapPlot = [&](TH2F *hist, const std::string &name, const std::vector<std::string> &labels)
        {
            TCanvas *c = set2Dhist(hist, false, true, false, false, RightMargin, "(z and layer) index", "phi index", "Average Trkrhit ADC", "colz", labels);
            std::string pdfPath = plotdir + name + ".pdf";
            std::string pngPath = plotdir + name + ".png";
            c->SaveAs(pdfPath.c_str());
            c->SaveAs(pngPath.c_str());
            delete c;
        };

        TH2F *hM_hitmap_RowColumnFlatten_noncorr_avgAdc = (TH2F *)hM_hitmap_RowColumnFlatten_noncorr_weightAdc->Clone("hM_hitmap_RowColumnFlatten_noncorr_avgAdc");
        hM_hitmap_RowColumnFlatten_noncorr_avgAdc->Divide(hM_hitmap_RowColumnFlatten_noncorr);
        hM_hitmap_RowColumnFlatten_noncorr_avgAdc->GetZaxis()->SetRangeUser(0, 210);
        saveHitMapPlot(hM_hitmap_RowColumnFlatten_noncorr_avgAdc, "Hitmap_evtsel_noncorr", {"Integrated hit map", "|MBD Zvtx|#leq10cm", "NClus_{inner}#geq1000", "Outer/Inner#geq1.2", "Is Min Bias"});

        TH2F *hM_hitmap_RowColumnFlatten_pm4noncorr_avgAdc = (TH2F *)hM_hitmap_RowColumnFlatten_pm4noncorr_weightAdc->Clone("hM_hitmap_RowColumnFlatten_pm4noncorr_avgAdc");
        hM_hitmap_RowColumnFlatten_pm4noncorr_avgAdc->Divide(hM_hitmap_RowColumnFlatten_pm4noncorr);
        hM_hitmap_RowColumnFlatten_pm4noncorr_avgAdc->GetZaxis()->SetRangeUser(0, 210);
        saveHitMapPlot(hM_hitmap_RowColumnFlatten_pm4noncorr_avgAdc, "Hitmap_evtsel_pm4noncorr", {"Integrated hit map", "[-4, -1] and [+1, +4] events", "Is Min Bias"});

        // Normalize and save Trkrhit ADC plots
        hM_TrkrhitADC_noncorr->Scale(1. / hM_TrkrhitADC_noncorr->Integral(-1, -1));
        hM_TrkrhitADC_pm4noncorr->Scale(1. / hM_TrkrhitADC_pm4noncorr->Integral(-1, -1));
        float binmax_adc = std::max(hM_TrkrhitADC_noncorr->GetMaximum(), hM_TrkrhitADC_pm4noncorr->GetMaximum());

        TCanvas *c_adc = new TCanvas("c_adc", "c_adc", 800, 700);
        c_adc->cd();
        hM_TrkrhitADC_noncorr->GetXaxis()->SetTitle("TrkrHit ADC");
        hM_TrkrhitADC_noncorr->GetYaxis()->SetTitle("Normalized entries");
        hM_TrkrhitADC_noncorr->GetXaxis()->SetTitleOffset(1.1);
        hM_TrkrhitADC_noncorr->GetYaxis()->SetTitleOffset(1.4);
        hM_TrkrhitADC_noncorr->GetYaxis()->SetRangeUser(0, binmax_adc * 1.5);
        hM_TrkrhitADC_noncorr->SetLineColor(kTVibBlue);
        hM_TrkrhitADC_noncorr->SetLineWidth(2);
        hM_TrkrhitADC_noncorr->SetMarkerColor(kTVibBlue);
        hM_TrkrhitADC_noncorr->SetMarkerStyle(20);
        hM_TrkrhitADC_noncorr->SetMarkerSize(1);
        hM_TrkrhitADC_pm4noncorr->SetLineColor(kTVibRed);
        hM_TrkrhitADC_pm4noncorr->SetLineWidth(2);
        hM_TrkrhitADC_pm4noncorr->SetMarkerColor(kTVibRed);
        hM_TrkrhitADC_pm4noncorr->SetMarkerStyle(20);
        hM_TrkrhitADC_pm4noncorr->SetMarkerSize(1);
        hM_TrkrhitADC_noncorr->Draw("PE");
        hM_TrkrhitADC_pm4noncorr->Draw("PE same");
        TLegend *leg = new TLegend(0.3, 0.8, 0.7, 0.9);
        leg->AddEntry(hM_TrkrhitADC_noncorr, "Events of interest (Is MB)", "pel");
        leg->AddEntry(hM_TrkrhitADC_pm4noncorr, "[-4, -1] and [+1, +4] events (Is MB)", "pel");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetTextSize(0.04);
        leg->Draw();
        c_adc->SaveAs((plotdir + "TrkrHitADC_noncorr.pdf").c_str());
        c_adc->SaveAs((plotdir + "TrkrHitADC_noncorr.png").c_str());

        // hM_HitTimeBucket_Layer_noncorrnext->Scale(1. / hM_HitTimeBucket_Layer_noncorrnext->Integral(-1, -1, -1, -1));
        float binmax_timebucket = 0;
        for (int l = 0; l < 4; l++)
        {
            // list_hM_HitTimeBucket_Layer_noncorrnext[l]->Scale(1. / list_hM_HitTimeBucket_Layer_noncorrnext[l]->Integral(-1, -1));
            if (list_hM_HitTimeBucket_Layer_noncorrnext[l]->GetMaximum() > binmax_timebucket)
            {
                binmax_timebucket = list_hM_HitTimeBucket_Layer_noncorrnext[l]->GetMaximum();
            }
        }
        TCanvas *c_timebucketlayer = new TCanvas("c_timebucketlayer", "c_timebucketlayer", 800, 700);
        TLegend *leg_timebucketlayer = new TLegend(0.6, 0.7, 0.85, 0.9);
        leg_timebucketlayer->SetHeader("Next event of EOI");
        c_timebucketlayer->cd();
        c_timebucketlayer->SetTopMargin(0.07);
        // hM_HitTimeBucket_Layer_noncorrnext->GetXaxis()->SetTitle("Time bucket");
        // hM_HitTimeBucket_Layer_noncorrnext->GetYaxis()->SetTitle("Layer");
        // hM_HitTimeBucket_Layer_noncorrnext->GetXaxis()->SetTitleOffset(1.3);
        // hM_HitTimeBucket_Layer_noncorrnext->GetYaxis()->SetTitleOffset(1.4);
        // hM_HitTimeBucket_Layer_noncorrnext->GetYaxis()->SetNdivisions(4);
        // hM_HitTimeBucket_Layer_noncorrnext->Draw("colz");
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->GetXaxis()->SetTitle("Time bucket");
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->GetYaxis()->SetTitle("Entries");
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->GetXaxis()->SetTitleOffset(1.3);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->GetYaxis()->SetTitleOffset(1.4);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->GetYaxis()->SetRangeUser(0, binmax_timebucket * 1.3);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->SetLineColor(colorlist[0]);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->SetLineWidth(1);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->SetMarkerColor(colorlist[0]);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->SetMarkerStyle(20);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->SetMarkerSize(0.1);
        list_hM_HitTimeBucket_Layer_noncorrnext[0]->Draw("E hist");
        leg_timebucketlayer->AddEntry(list_hM_HitTimeBucket_Layer_noncorrnext[0], "Layer 3", "pel");
        for (int l = 1; l < 4; l++)
        {
            // TLine *line = new TLine(-0.5, 2.5+i, 130.5, 2.5+i);
            // line->SetLineColor(kBlack);
            // line->SetLineWidth(1);
            // line->Draw();
            list_hM_HitTimeBucket_Layer_noncorrnext[l]->SetLineWidth(1);
            list_hM_HitTimeBucket_Layer_noncorrnext[l]->SetLineColor(colorlist[l]);
            list_hM_HitTimeBucket_Layer_noncorrnext[l]->SetMarkerSize(0.1);
            list_hM_HitTimeBucket_Layer_noncorrnext[l]->SetMarkerStyle(20);
            list_hM_HitTimeBucket_Layer_noncorrnext[l]->SetMarkerColor(colorlist[l]);
            list_hM_HitTimeBucket_Layer_noncorrnext[l]->Draw("E hist same");
            leg_timebucketlayer->AddEntry(list_hM_HitTimeBucket_Layer_noncorrnext[l], Form("Layer %d", l + 3), "pel");
        }
        leg_timebucketlayer->SetBorderSize(0);
        leg_timebucketlayer->SetFillStyle(0);
        leg_timebucketlayer->SetTextSize(0.04);
        leg_timebucketlayer->Draw();
        c_timebucketlayer->RedrawAxis();
        c_timebucketlayer->SaveAs((plotdir + "HitTimebucketLayer_evtsel_noncorrnext.pdf").c_str());
        c_timebucketlayer->SaveAs((plotdir + "HitTimebucketLayer_evtsel_noncorrnext.png").c_str());
    }

    outtxtfile.close();

    TCanvas *c_bcodiff = new TCanvas("c_bcodiff", "c_bcodiff", 800, 700);
    c_bcodiff->cd();
    hM_BCOdiff_noncorrnext->GetXaxis()->SetTitle("BCO_{next} - BCO_{of interest}");
    hM_BCOdiff_noncorrnext->GetYaxis()->SetTitle("Entries");
    // Draw overflow bin
    hM_BCOdiff_noncorrnext->GetXaxis()->SetRange(1, hM_BCOdiff_noncorrnext->GetNbinsX());
    hM_BCOdiff_noncorrnext->GetXaxis()->SetTitleOffset(1.3);
    hM_BCOdiff_noncorrnext->GetYaxis()->SetTitleOffset(1.4);
    hM_BCOdiff_noncorrnext->SetLineColor(kBlack);
    hM_BCOdiff_noncorrnext->SetLineWidth(2);
    hM_BCOdiff_noncorrnext->Draw("hist");
    c_bcodiff->SaveAs((plotdir + "BCOdiff_noncorrnext.pdf").c_str());
    c_bcodiff->SaveAs((plotdir + "BCOdiff_noncorrnext.png").c_str());
}