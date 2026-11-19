#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

void RawAsymmetryInputs(int Verbosity = 0)
{
    TFile *infile = TFile::Open("/sphenix/tg/tg01/coldqcd/dillfitz/singletrackan/Nov5/79516.root");

    int bunchnum;
    int sphnxbunchnum;
    int multiplicity; 
    int crossing;
    int runnum;
  
    // spin stuff
    static const int NBUNCHES = 120;
    int xingshift;
    std::array<int, NBUNCHES> spinPatternBlue{0};
    std::array<int, NBUNCHES> spinPatternYellow{0};
    int trackBunch = 0;
    int bspin = 0;
    int yspin = 0;
    float bpol;
    float bpolerr;
    float ypol;
    float ypolerr;

    // lumi
    std::array<double, NBUNCHES> bunchnumber_lumi{0.};
    double lumi{0.};
    double lumi_up_blue{0.};
    double lumi_down_blue{0.};
    double lumi_up_yellow{0.};
    double lumi_down_yellow{0.};
    double rel_lumi_blue{0.};
    double rel_lumi_yellow{0.};
  
  
    // track stuff
    float p;
    float pt;
    float pz;
    float xf;
    float eta;
    float phi, bphi, yphi;
    float quality;
    float charge;

    // bco stuff 
    int candidate_crossing;
    // IMPORTANT!! THE WRONG VALUES ARE READ OUT IF uint64_t is used here... Root TTrees are very particular. 
    // Perhaps test RDataFrame workflow?? It is far more efficient but the syntax is very different
    ULong64_t bco;
    ULong64_t bcowindowlow;
    ULong64_t bcowindowhigh;
    bool usable_bco_tag;
    bool doublecount;

    TTree *runTree = (TTree*)infile->Get("runTree");

    runTree->SetBranchAddress("runnum", &runnum);
    runTree->SetBranchAddress("bpol", &bpol);
    runTree->SetBranchAddress("bpolerr", &bpolerr);
    runTree->SetBranchAddress("ypol", &ypol);
    runTree->SetBranchAddress("ypolerr", &ypolerr);
    runTree->SetBranchAddress("lumi", &lumi);
    runTree->SetBranchAddress("bunchnumber_lumi", &bunchnumber_lumi);
    runTree->SetBranchAddress("spin_pattern_blue", &spinPatternBlue);
    runTree->SetBranchAddress("spin_pattern_yellow", &spinPatternYellow);


    TTree *singleTrackTree = (TTree*)infile->Get("singleTrackTree");

    singleTrackTree->SetBranchAddress("bunchnum", &bunchnum);
    singleTrackTree->SetBranchAddress("sphnxbunchnum", &sphnxbunchnum);
    singleTrackTree->SetBranchAddress("multiplicity", &multiplicity);
    singleTrackTree->SetBranchAddress("crossing", &crossing);
    singleTrackTree->SetBranchAddress("trackBunch", &trackBunch);
    singleTrackTree->SetBranchAddress("bspin", &bspin);
    singleTrackTree->SetBranchAddress("yspin", &yspin);
    singleTrackTree->SetBranchAddress("bco", &bco);
    singleTrackTree->SetBranchAddress("bcowindowlow", &bcowindowlow);
    singleTrackTree->SetBranchAddress("bcowindowhigh", &bcowindowhigh);
    singleTrackTree->SetBranchAddress("usable_bco_tag", &usable_bco_tag);
    singleTrackTree->SetBranchAddress("doublecount", &doublecount);
    singleTrackTree->SetBranchAddress("p", &p);
    singleTrackTree->SetBranchAddress("pt", &pt);
    singleTrackTree->SetBranchAddress("xf", &xf);
    singleTrackTree->SetBranchAddress("eta", &eta);
    singleTrackTree->SetBranchAddress("phi", &phi);
    singleTrackTree->SetBranchAddress("bphi", &bphi);
    singleTrackTree->SetBranchAddress("yphi", &yphi);
    singleTrackTree->SetBranchAddress("quality", &quality);
    singleTrackTree->SetBranchAddress("charge", &charge);



    // Questions // 
    // high and low pt thresholds for good tracks??
    // What does the quality distribution represent? Should we cut on it? 
    // What are bspin quantities that are not +/- 1?? e.g. singleTrackTree->Draw("bspin","bspin > 100")
      // For this... All crossings are negative. Though note all negative crossings give weird values... 
    // Similarly for yspin, though these are not as randomly distributed. There are ~41k entries at -10 though.
    // Looks like mutliplicity quantity should be the same for every track in the track map for a given "event"

    // IMPORTANT -- I THINK I FOUND SOLUTION, DISCUSS WITH DEVON //
    // This block is hugely reducing statistics, need to understand what is going on
    // if (!usable_bco_tag || doublecount) 
    // {
    //     continue;
    // }
    // usable_bco_tag requirement reducing track count from 6e7 to 1.8e7
    // together with !doublecount requirement the track count is further reduced to 1.58e7
    // note that the !doublecount requirement alone reduces the track count to 5.24e7
    // SOMETHING TO CHECK
    // is the usable_bco_tag doing what we want when using a continue statement? If a trigger frame has usable_bco_tag set to false, then ONLY tracks from crossing 0 should be skipped. I suspect the way I have it implemented it is skipping all tracks in these trigger frames...
    // This seems to be the case, I have updated the cut to only be applied for crossing 0. Now will all cuts applied the number of surviving tracks is 2.54e7, which is suspiciously close to the number of trigger frames (could be coincidence)
    // This line reproduces the numbes exacly from the singleTrackTree
    // singleTrackTree->Draw("phi", "((!usable_bco_tag&&crossing!=0)||(usable_bco_tag))&&(!doublecount)")
    // This is very puzzling to us... It looks like ony ~5M are being removed from crossing 0, and we don't observe changes in other crossing numbers

    // FOUND IN MEETING WITH DEVON //
    // The real effect (without the doublecount) is about ~5M removed tracks. IMPORTANT! GetEntries for any hist in the TTree has a misleading number of counts, it quotes 6e7 when in reality it is about 3.7e7 (double check number)

    // In total, it looks like there are ~25M trigger frames (2.5e7) in run 79507, leading to 6e7 tracks (does this sound right?)
    // Of this, ~1.8e7 have crossing 0 tagged as unusable, and 0.7e7 have crossing 0 tagged as usable

    // Create output file
    TFile *outfile = new TFile("raw_trackTSSA_inputs.root", "RECREATE");
    outfile->cd();


    const int nptbins = 7;
    const int nxfbins = 8;
    const int netabins = 12;
    const int nphibins = 8;
    double ptbins[nptbins + 1] = {1., 2., 3., 4., 5., 6., 8., 10.0}; 
    double xfbins[nxfbins + 1] = {-0.2, -0.048, -0.035, -0.022, 0., 0.022, 0.035, 0.048, 0.2};
    double etabins[netabins + 1] = {-2, -1.5, -1.0, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 1.0, 1.5, 2};
    double phibins[nphibins + 1] = {-3.140, -2.355, -1.570, -0.785, 0.000, 0.785, 1.570, 2.355, 3.140};

    // Declare histograms
    TH1D *hpt = new TH1D("hpt", ";p_{T};", 1000, 0., 100.);
    TH1D *hpt_1GeV = new TH1D("hpt_1GeV", ";p_{T};", 1000, 0., 100.);
    TH1D *hpt_1GeV_10GeV = new TH1D("hpt_1GeV_10GeV", ";p_{T};", 1000, 0., 100.);
    TH1D *hp = new TH1D("hp", ";p;", 1000, 0., 100.);
    TH1D *heta = new TH1D("heta", ";#eta;", 100, -2., 2.);
    TH1D *hphi = new TH1D("hphi", ";#phi;", 100, -3.5, 3.5);
    TH1D *hxf = new TH1D("hxf", ";x_{F};", 100, -0.4, 0.4);
    TH1D *hxf_cut = new TH1D("hxf_cut", ";x_{F};", 100, -0.2, 0.2);
    TH1D *hquality = new TH1D("hquality", ";quality;", 1000, 0., 500.);
    TH2D *h2ptp = new TH2D("h2ptp", ";p;p_{T}", 1000, 0., 100., 1000, 0., 100.);
    TH2D *h2etaphi = new TH2D("h2etaphi", ";phi;eta", 100, -3.5, 3.5, 100, -2., 2.);
    TH1I *h_trigger_bias = new TH1I("h_trigger_bias", ";trigger bias;", 2, -0.5, 1.5);
    TH1I *hdoublecount = new TH1I("hdoublecount", ";doublecount;", 2, -0.5, 1.5);
    TH1I *hcrossing_all = new TH1I("hcrossing_all", ";crossing;", 360, -20, 340);
    TH1I *hcrossing_nobias = new TH1I("hcrossing_nobias", ";crossing;", 360, -20, 340);
    TH1I *hcrossing_nobias_nodups = new TH1I("hcrossing_nobias_nodups", ";crossing;", 360, -20, 340);

    TH1I *hsphnx_bunchnum = new TH1I("hsphnxbunchnum", "; sphnx bunchnum;", 120, 0, 120);
    TH1I *hbunchnum = new TH1I("hbunchnum", ";bunchnum;", 120, 0, 120);
    TH1I *htrackbunchnum = new TH1I("htrackbunchnum", ";track bunchnum;", 120, 0, 120);




    // Asymmetry Inputs
    TH1D *hphi_bspinup = new TH1D("hphi_bup", ";#phi;", 12, -3.5, 3.5);
    TH1D *hphi_bspindown = new TH1D("hphi_bdown", ";#phi;", 12, -3.5, 3.5);
    TH1D *hphi_yspinup = new TH1D("hphi_yup", ";#phi;", 12, -3.5, 3.5);
    TH1D *hphi_yspindown = new TH1D("hphi_ydown", ";#phi;", 12, -3.5, 3.5);

    TH2D *h2ptphi_bspinup = new TH2D("hptphi_bup", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_bspindown = new TH2D("hptphi_bdown", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspinup = new TH2D("hptphi_yup", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspindown = new TH2D("hptphi_ydown", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);

    TH2D *h2ptphi_bspinup_pos = new TH2D("hptphi_bup_pos", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_bspindown_pos = new TH2D("hptphi_bdown_pos", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspinup_pos = new TH2D("hptphi_yup_pos", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspindown_pos = new TH2D("hptphi_ydown_pos", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);

    TH2D *h2ptphi_bspinup_neg = new TH2D("hptphi_bup_neg", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_bspindown_neg = new TH2D("hptphi_bdown_neg", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspinup_neg = new TH2D("hptphi_yup_neg", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspindown_neg = new TH2D("hptphi_ydown_neg", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);

    TH2D *h2xfphi_bspinup = new TH2D("hxfphi_bup", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_bspindown = new TH2D("hxfphi_bdown", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspinup = new TH2D("hxfphi_yup", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspindown = new TH2D("hxfphi_ydown", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);

    TH2D *h2xfphi_bspinup_pos = new TH2D("hxfphi_bup_pos", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_bspindown_pos = new TH2D("hxfphi_bdown_pos", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspinup_pos = new TH2D("hxfphi_yup_pos", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspindown_pos = new TH2D("hxfphi_ydown_pos", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);

    TH2D *h2xfphi_bspinup_neg = new TH2D("hxfphi_bup_neg", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_bspindown_neg = new TH2D("hxfphi_bdown_neg", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspinup_neg = new TH2D("hxfphi_yup_neg", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspindown_neg = new TH2D("hxfphi_ydown_neg", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);

    TH2D *h2etaphi_bspinup = new TH2D("hetaphi_bup", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_bspindown = new TH2D("hetaphi_bdown", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspinup = new TH2D("hetaphi_yup", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspindown = new TH2D("hetaphi_ydown", ";#phi;#eta", nphibins, phibins, netabins, etabins);

    TH2D *h2etaphi_bspinup_pos = new TH2D("hetaphi_bup_pos", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_bspindown_pos = new TH2D("hetaphi_bdown_pos", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspinup_pos = new TH2D("hetaphi_yup_pos", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspindown_pos = new TH2D("hetaphi_ydown_pos", ";#phi;#eta", nphibins, phibins, netabins, etabins);

    TH2D *h2etaphi_bspinup_neg = new TH2D("hetaphi_bup_neg", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_bspindown_neg = new TH2D("hetaphi_bdown_neg", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspinup_neg = new TH2D("hetaphi_yup_neg", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspindown_neg = new TH2D("hetaphi_ydown_neg", ";#phi;#eta", nphibins, phibins, netabins, etabins);

    // Extract relevant information from the run tree (e.g. relative luminosity and polarization)
    for (int i = 0; i<runTree->GetEntries(); ++i)
    {
        runTree->GetEntry(i);
        // Need to revisit this if the run tree is more than 1 run long, at which point these values need to be zeroed out and the rel_lumi_blue and rel_lumi_yellow need to be saved into some kind of data structure
        // The current workflow requires all track DST segments to be processed to calculate the lumi, so for run 79516 alone there were already ~38000 trees to combine. 
        /*
        lumi_up_blue = 0.;
        lumi_down_blue = 0.;
        lumi_up_yellow = 0.;
        lumi_down_yellow = 0.;
        rel_lumi_blue = 0.;
        rel_lumi_yellow = 0.;
        */
        if (Verbosity > 1)
        {
            std::cout << "runnumber : " << runnum << std::endl;
            std::cout << " bpol +/- bpolerr : " << bpol << " +/- " << bpolerr << std::endl;
            std::cout << " ypol +/- ypolerr : " << ypol << " +/- " << ypolerr << std::endl;
            std::cout << " lumi : " << lumi << std::endl;
        }
        for (int bunch = 0; bunch < NBUNCHES; ++bunch)
        {

            if (spinPatternBlue[bunch] == 1)
            {
                lumi_up_blue += bunchnumber_lumi[bunch];
            }
            else if (spinPatternBlue[bunch] == -1)
            {
                lumi_down_blue += bunchnumber_lumi[bunch];
            }
            if (spinPatternYellow[bunch] == 1)
            {
                lumi_up_yellow += bunchnumber_lumi[bunch];
            }
            else if (spinPatternYellow[bunch] == -1)
            {
                lumi_down_yellow += bunchnumber_lumi[bunch];
            }
        }
        // This is currently designed for a run tree with 1 entry so that these values are not overwritten when looping over the single track tree
        rel_lumi_blue = lumi_up_blue / lumi_down_blue;
        rel_lumi_yellow = lumi_up_yellow / lumi_down_yellow;
    }
    // If the worflow changes to processing multiple runs simultaneously, need to change to a TGraph with runnum, rel_lumi_(blue, yellow) or something
    // How do we get an error estimate on the luminosity?
    // Not currently saving this in the lumi workflow, ask Jin and Joe!
    TParameter<double> rl_blue("rel_lumi_blue", rel_lumi_blue);
    TParameter<double> rl_yellow("rel_lumi_yellow", rel_lumi_yellow);

    rl_blue.Write();
    rl_yellow.Write();

    for (int i = 0; i<singleTrackTree->GetEntries(); ++i)
    //for(int i=0; i<1000000; ++i)
    {
        if (i%10000000 == 0) { std::cout << "processing entry : " << i << " of " << singleTrackTree->GetEntries() << std::endl; }
        singleTrackTree->GetEntry(i);
        if (Verbosity > 5)
        {
            std::cout << "bco  : " << bco << std::endl;
            std::cout << "usable_bco_tag : " << usable_bco_tag << std::endl;
            std::cout << "bcowindowlow : " << bcowindowlow << std::endl;
            std::cout << "bcowindowhigh : " << bcowindowhigh << std::endl;
            std::cout << "bcowindowlength : " << bcowindowhigh - bcowindowlow << std::endl;
            std::cout << "bspin : " << bspin << " yspin : " << yspin << std::endl;
        }

        bool trigger_bias = usable_bco_tag&&crossing==0;
        h_trigger_bias->Fill(trigger_bias);
        hdoublecount->Fill(doublecount);
        hcrossing_all->Fill(crossing);

        if (!usable_bco_tag && crossing == 0)
        {
            continue;
        }
        
        hcrossing_nobias->Fill(crossing);

        if (doublecount) 
        {
            continue;
        }
        hcrossing_nobias_nodups->Fill(crossing);

        // Making some QA hists to get oriented 

        hpt->Fill(pt);
        if (pt > 1)
        {
            hpt_1GeV->Fill(pt);
        }
        if (pt > 1 && pt < 10)
        {
            hpt_1GeV_10GeV->Fill(pt);
        }
        hp->Fill(p);
        hxf->Fill(xf);
        if (fabs(xf) < 0.2)
        {
            hxf_cut->Fill(xf);
        }
        heta->Fill(eta);
        hphi->Fill(phi);
        hquality->Fill(quality);
        h2ptp->Fill(p, pt);
        h2etaphi->Fill(phi, eta);
        hsphnx_bunchnum->Fill(sphnxbunchnum);
        hbunchnum->Fill(bunchnum);
        htrackbunchnum->Fill(trackBunch);

        // Spin sorted asymmetry inputs

        if (bspin == 1)
        {
            hphi_bspinup->Fill(bphi);
            h2ptphi_bspinup->Fill(bphi, pt);
            h2xfphi_bspinup->Fill(bphi, xf);
            h2etaphi_bspinup->Fill(bphi, eta);
            if (charge == 1)
            {
                h2ptphi_bspinup_pos->Fill(bphi, pt);
                h2xfphi_bspinup_pos->Fill(bphi, xf);
                h2etaphi_bspinup_pos->Fill(bphi, eta);
            }
            if (charge == -1)
            {
                h2ptphi_bspinup_neg->Fill(bphi, pt);
                h2xfphi_bspinup_neg->Fill(bphi, xf);
                h2etaphi_bspinup_neg->Fill(bphi, eta);
            }
        }
        if (bspin == -1)
        {
            hphi_bspindown->Fill(bphi);
            h2ptphi_bspindown->Fill(bphi, pt);
            h2xfphi_bspindown->Fill(bphi, xf);
            h2etaphi_bspindown->Fill(bphi, eta);
            if (charge == 1)
            {
                h2ptphi_bspindown_pos->Fill(bphi, pt);
                h2xfphi_bspindown_pos->Fill(bphi, xf);
                h2etaphi_bspindown_pos->Fill(bphi, eta);
            }
            if (charge == -1)
            {
                h2ptphi_bspindown_neg->Fill(bphi, pt);
                h2xfphi_bspindown_neg->Fill(bphi, xf);
                h2etaphi_bspindown_neg->Fill(bphi, eta);
            }
        }
        if (yspin == 1)
        {
            // double check this is correct
            xf = -1*xf;
            hphi_yspinup->Fill(yphi);
            h2ptphi_yspinup->Fill(yphi, pt);
            h2xfphi_yspinup->Fill(yphi, xf);
            h2etaphi_yspinup->Fill(yphi, eta);
            if (charge == 1)
            {
                h2ptphi_yspinup_pos->Fill(yphi, pt);
                h2xfphi_yspinup_pos->Fill(yphi, xf);
                h2etaphi_yspinup_pos->Fill(yphi, eta);
            }
            if (charge == -1)
            {
                h2ptphi_yspinup_neg->Fill(yphi, pt);
                h2xfphi_yspinup_neg->Fill(yphi, xf);
                h2etaphi_yspinup_neg->Fill(yphi, eta);
            }
        }
        if (yspin == -1)
        {
            // double check this is correct
            xf = -1*xf;
            hphi_yspindown->Fill(yphi);
            h2ptphi_yspindown->Fill(yphi, pt);
            h2xfphi_yspindown->Fill(yphi, xf);
            h2etaphi_yspindown->Fill(yphi, eta);
            if (charge == 1)
            {
                h2ptphi_yspindown_pos->Fill(yphi, pt);
                h2xfphi_yspindown_pos->Fill(yphi, xf);
                h2etaphi_yspindown_pos->Fill(yphi, eta);
            }
            if (charge == -1)
            {
                h2ptphi_yspindown_neg->Fill(yphi, pt);
                h2xfphi_yspindown_neg->Fill(yphi, xf);
                h2etaphi_yspindown_neg->Fill(yphi, eta);
            }
        }
        


        // a quick glance tells me the phi distribution is not flat -- is there a phi dependent track efficiecny effect?   
    
    }

/*
    hpt->Write();
    hp->Write();
    heta->Write();
    hphi->Write();
    hquality->Write();
    h2ptp->Write();
    h2etaphi->Write();
    hphi_bspinup->Write();
    hphi_bspindown->Write();
    hphi_yspinup->Write();
    hphi_yspindown->Write();
    h2piphi_bspinup->Write();
    h2ptphi_bspindown->Write();
    h2ptphi_yspinup->Write();
    h2ptphi_yspindown->Write();


    hpt->Delete();
    hp->Delete();
    heta->Delete();
    hphi->Delete();
    hquality->Delete();
    h2ptp->Delete();
    h2etaphi->Delete();
    hphi_bspinup->Delete();
    hphi_bspindown->Delete();
    hphi_yspinup->Delete();
    hphi_yspindown->Delete();
    h2piphi_bspinup->Delete();
    h2ptphi_bspindown->Delete();
    h2ptphi_yspinup->Delete();
    h2ptphi_yspindown->Delete();
*/
    outfile->Write();
}
