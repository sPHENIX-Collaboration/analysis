void plotembedpion()
{
TChain* ntp_gtrack = new TChain("ntp_gtrack");
ntp_gtrack->Add("/sphenix/u/sphnxpro/embed/embedpionsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04004_g4svtx_eval.root");
ntp_gtrack->Add("/sphenix/u/sphnxpro/embed/embedpionsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04008_g4svtx_eval.root");

cout << "number of entries = " << ntp_gtrack->GetEntries() << endl;

ntp_gtrack->SetMarkerStyle(24);
ntp_gtrack->SetMarkerSize(0.2);

ntp_gtrack->Draw("pt:gpt","pt<13");

}

