#include <filesystem>

// std::string ntpdir = "/sphenix/tg/tg01/hf/hjheng/ppg02/dst/Run54280_combinedNtuple/";
std::string ntpdir = "/sphenix/tg/tg01/commissioning/INTT/work/cwshih/seflgendata/run_54280/From_official_INTTRAWHIT_DST_ColdHotChannelMask/completed/";
std::string vtxmapdir = "/sphenix/user/hjheng/sPHENIXRepo/analysis/dNdEta_Run2023/analysis_INTT/minitree/VtxEvtMap_Data_CombinedNtuple_Run54280_HotChannel_BCO/";
std::string plotdir = "./RecoPV_ana/Run54280_trigcomp";

std::map<uint64_t, vector<float>> EvtVtx_map_inttbco(const char *vtxfname)
{
    std::map<uint64_t, vector<float>> EvtVtx_map;

    TFile *f = new TFile(vtxfname, "READ");
    TTree *t = (TTree *)f->Get("minitree");
    uint64_t intt_bco;
    float PV_x, PV_y, PV_z;
    t->SetBranchAddress("INTT_BCO", &intt_bco);
    t->SetBranchAddress("PV_x", &PV_x);
    t->SetBranchAddress("PV_y", &PV_y);
    t->SetBranchAddress("PV_z", &PV_z);
    for (int ev = 0; ev < t->GetEntriesFast(); ev++)
    {
        t->GetEntry(ev);
        EvtVtx_map[intt_bco] = {PV_x, PV_y, PV_z};
    }

    return EvtVtx_map;
}

void quickdraw_mbdvtx()
{
    // mkdir if not exist
    system(Form("mkdir -p %s", plotdir.c_str()));

    TH1F *hM_MbdZvtx_trig12 = new TH1F("hM_MbdZvtx_trig12", "M_MbdZvtx_trig12", 160, -40, 40);
    TH1F *hM_MbdZvtx_trig12_InttZvtxle10 = new TH1F("hM_MbdZvtx_trig12_InttZvtxle10", "M_MbdZvtx_trig12_InttZvtxle10", 160, -40, 40);
    TH1F *hM_MbdZvtx_trig13 = new TH1F("hM_MbdZvtx_trig13", "hM_MbdZvtx_trig13", 160, -40, 40);
    TH1F *hM_MbdZvtx_trig13_InttZvtxle10 = new TH1F("hM_MbdZvtx+trig13_InttZvtxle10", "hM_MbdZvtx+trig13_InttZvtxle10", 160, -40, 40);

    // loop over all files in the directory, sort by filename
    int nfilestorun = 10;
    for (int i = 0; i < nfilestorun; i++)
    {
        std::string formatidx = std::string(TString::Format("%05d", i).Data());
        std::string filename = Form("%s/Trkr_DST_physics_intt-54280_HotChannel_BCOCut_CaloCombined_%s.root", ntpdir.c_str(), formatidx.c_str());
        // check if the file exists
        std::filesystem::path entry(filename);
        if (!std::filesystem::exists(entry))
            continue;

        std::cout << "Processing " << filename << std::endl;

        std::map<uint64_t, vector<float>> evtvtxmap = EvtVtx_map_inttbco(Form("%s/minitree_%s.root", vtxmapdir.c_str(), formatidx.c_str()));

        TFile *f = new TFile(filename.c_str(), "READ");
        TTree *t = (TTree *)f->Get("EventTree");
        uint64_t INTT_BCO;
        std::vector<int> *firedTriggers = 0;
        float MBD_z_vtx, MBD_south_charge_sum, MBD_north_charge_sum;
        t->SetBranchAddress("LiveTrigger_Vec", &firedTriggers);
        t->SetBranchAddress("INTT_BCO", &INTT_BCO);
        t->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
        t->SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
        t->SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
        for (int ev = 0; ev < t->GetEntriesFast(); ev++)
        {
            t->GetEntry(ev);

            // try to get the vertex from the map. If not found, set to nan
            float INTT_z_vtx = std::numeric_limits<float>::quiet_NaN();
            if (evtvtxmap.find(INTT_BCO) != evtvtxmap.end())
                INTT_z_vtx = evtvtxmap[INTT_BCO][2];

            bool validInttVtx = (INTT_z_vtx == INTT_z_vtx);
            bool validMbdVtx = (MBD_z_vtx == MBD_z_vtx);
            bool MbdNScharge = (MBD_north_charge_sum > 0 || MBD_south_charge_sum > 0);
            bool firedTrig12 = (std::find(firedTriggers->begin(), firedTriggers->end(), 12) != firedTriggers->end());
            bool firedTrig13 = (std::find(firedTriggers->begin(), firedTriggers->end(), 13) != firedTriggers->end());

            if (!validMbdVtx || !MbdNScharge || !validInttVtx)
                continue;

            if (firedTrig12)
            {
                hM_MbdZvtx_trig12->Fill(MBD_z_vtx);
                if (fabs(INTT_z_vtx) < 10)
                    hM_MbdZvtx_trig12_InttZvtxle10->Fill(MBD_z_vtx);
            }
                
            if (firedTrig13)
            {
                hM_MbdZvtx_trig13->Fill(MBD_z_vtx);
                if (fabs(INTT_z_vtx) < 10)
                    hM_MbdZvtx_trig13_InttZvtxle10->Fill(MBD_z_vtx);
            }
        }

        f->Close();
    }

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    hM_MbdZvtx_trig13->GetXaxis()->SetTitle("MBD Z vertex [cm]");
    hM_MbdZvtx_trig13->GetYaxis()->SetTitle("Counts");
    hM_MbdZvtx_trig13->GetYaxis()->SetTitleOffset(1.5);
    hM_MbdZvtx_trig13->GetYaxis()->SetRangeUser(0, hM_MbdZvtx_trig13->GetMaximum() * 1.5);
    hM_MbdZvtx_trig13->SetLineWidth(2);
    hM_MbdZvtx_trig13->SetLineColor(kTBriRed);
    hM_MbdZvtx_trig13->SetMarkerColor(kTBriRed);
    hM_MbdZvtx_trig13->SetMarkerSize(0.7);
    hM_MbdZvtx_trig13->Draw("PE");
    hM_MbdZvtx_trig12->SetLineWidth(2);
    hM_MbdZvtx_trig12->SetLineColor(kTBriBlue);
    hM_MbdZvtx_trig12->SetMarkerColor(kTBriBlue);
    hM_MbdZvtx_trig12->SetMarkerSize(0.7);
    hM_MbdZvtx_trig12->Draw("PE same");
    hM_MbdZvtx_trig12_InttZvtxle10->SetLineWidth(2);
    hM_MbdZvtx_trig12_InttZvtxle10->SetLineColor(kTBriCyan);
    hM_MbdZvtx_trig12_InttZvtxle10->SetMarkerColor(kTBriCyan);
    hM_MbdZvtx_trig12_InttZvtxle10->SetMarkerSize(0.7);
    hM_MbdZvtx_trig12_InttZvtxle10->Draw("PE same");
    hM_MbdZvtx_trig13_InttZvtxle10->SetLineWidth(2);
    hM_MbdZvtx_trig13_InttZvtxle10->SetLineColor(kTBriPurple);
    hM_MbdZvtx_trig13_InttZvtxle10->SetMarkerColor(kTBriPurple);
    hM_MbdZvtx_trig13_InttZvtxle10->SetMarkerSize(0.7);
    hM_MbdZvtx_trig13_InttZvtxle10->Draw("PE same");
    TLegend *leg = new TLegend(0.2, 0.72, 0.5, 0.9);
    leg->AddEntry(hM_MbdZvtx_trig13, "Fired trigger 13 (MBD S&N#geq2, |vtx|<30cm)", "pel");
    leg->AddEntry(hM_MbdZvtx_trig12, "Fired trigger 12 (MBD S&N#geq2, |vtx|<10cm)", "pel");
    leg->AddEntry(hM_MbdZvtx_trig13_InttZvtxle10, "Fired trigger 13, |INTT Z vtx|<10cm", "pel");
    leg->AddEntry(hM_MbdZvtx_trig12_InttZvtxle10, "Fired trigger 12, |INTT Z vtx|<10cm", "pel");
    leg->SetTextSize(0.04);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();
    c->RedrawAxis();
    c->SaveAs(Form("%s/quickdraw_mbdvtx.png", plotdir.c_str()));
    c->SaveAs(Form("%s/quickdraw_mbdvtx.pdf", plotdir.c_str()));
}