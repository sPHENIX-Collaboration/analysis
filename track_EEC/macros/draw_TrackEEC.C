const float jet_pTs[6] = {10,20,30,40,60,1000};
const int marks[5] = {20, 21, 33, 34, 47};
const int colors[5] = {1, 632, 600, 418, 617};


void draw_TrackEEC()
{
    TFile *f = new TFile("/sphenix/tg/tg01/jets/bkimelman/track_EEC/trackDSTs/trackEECs_July15_2026_cuts.root","READ");

    TH1D *inJet[5];

    double maxC = 0.0;

    for(int i=0; i<5; i++)
    {
        inJet[i] = (TH1D*)f->Get(std::format("hEEC_inJetAll_Q2_{:.0f}_{:.0f}",jet_pTs[i], jet_pTs[i+1]).c_str());
        //inJet[i] = (TH1D*)f->Get(std::format("hEEC_inJet_{:.0f}_{:.0f}",jet_pTs[i], jet_pTs[i+1]).c_str());

        inJet[i]->Scale(1.0/inJet[i]->Integral());
        inJet[i]->Scale(1.0,"width");

        for(int j=1; j<=inJet[i]->GetNbinsX(); j++)
        {
            double bc = inJet[i]->GetBinContent(j);
            if(bc > maxC) maxC = bc;
        }

        inJet[i]->SetMarkerStyle(marks[i]);
        inJet[i]->SetMarkerColor(colors[i]);
        inJet[i]->SetLineColor(colors[i]);
    }

    TH2D *h = new TH2D("h",";#Delta R;EEC",1,inJet[0]->GetXaxis()->GetXmin(),inJet[0]->GetXaxis()->GetXmax(),1,1e-3,1.5*maxC);

    TCanvas *c1 = new TCanvas();
    gStyle->SetOptStat(0);
    c1->SetLogx();
    c1->SetLogy();

    TLegend *leg = new TLegend(0.15,0.15,0.5,0.45);

    h->Draw();

    for(int i=0; i<5; i++)
    {
        inJet[i]->Draw("PSAME");
        leg->AddEntry(inJet[i],std::format("{:.0f} < p_{{T,jet}} < {:.0f} GeV",jet_pTs[i], jet_pTs[i+1]).c_str(),"p");
    }

    leg->Draw("same");

    c1->SaveAs("/sphenix/tg/tg01/jets/bkimelman/track_EEC/trackDSTs/inJetEEC_pT_July15_2026_cuts.pdf");


}