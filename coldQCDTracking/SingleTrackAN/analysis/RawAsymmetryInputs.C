#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

void RawAsymmetryInputs(int Verbosity = 0)
{
    TFile *infile = TFile::Open("/sphenix/tg/tg01/coldqcd/dloomis/singletrackan/Aug12_singletracks/79507-all.root");

    int bunchnum;
    int sphnxbunchnum;
    int multiplicity; 
    int crossing;
  
    // spin stuff
    static const int NBUNCHES = 120;
    int xingshift;
    int spinPatternBlue[NBUNCHES] = {0};
    int spinPatternYellow[NBUNCHES] = {0};
    int trackBunch = 0;
    int bspin = 0;
    int yspin = 0;
    float bpol;
    float bpolerr;
    float ypol;
    float ypolerr;
  
  
    // track stuff
    float p;
    float pt;
    float pz;
    float xf;
    float eta;
    float phi;
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
    singleTrackTree->SetBranchAddress("eta", &eta);
    singleTrackTree->SetBranchAddress("phi", &phi);
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
    TH1D *hp = new TH1D("hp", ";p;", 1000, 0., 100.);
    TH1D *heta = new TH1D("heta", ";#eta;", 100, -2., 2.);
    TH1D *hphi = new TH1D("hphi", ";#phi;", 100, -3.5, 3.5);
    TH1D *hxf = new TH1D("hxf", ";#x_{F};", 100, -0.4, 0.4);
    TH1D *hquality = new TH1D("hquality", ";quality;", 1000, 0., 500.);
    TH2D *h2ptp = new TH2D("h2ptp", ";p;p_{T}", 1000, 0., 100., 1000, 0., 100.);
    TH2D *h2etaphi = new TH2D("h2etaphi", ";phi;eta", 100, -3.5, 3.5, 100, -2., 2.);
    TH1I *h_trigger_bias = new TH1I("h_trigger_bias", ";trigger bias;", 2, -0.5, 1.5);
    TH1I *hdoublecount = new TH1I("hdoublecount", ";doublecount;", 2, -0.5, 1.5);

    // Asymmetry Inputs
    TH1D *hphi_bspinup = new TH1D("hphi_bup", ";#phi;", 12, -3.5, 3.5);
    TH1D *hphi_bspindown = new TH1D("hphi_bdown", ";#phi;", 12, -3.5, 3.5);
    TH1D *hphi_yspinup = new TH1D("hphi_yup", ";#phi;", 12, -3.5, 3.5);
    TH1D *hphi_yspindown = new TH1D("hphi_ydown", ";#phi;", 12, -3.5, 3.5);

    TH2D *h2ptphi_bspinup = new TH2D("hptphi_bup", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_bspindown = new TH2D("hptphi_bdown", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspinup = new TH2D("hptphi_yup", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);
    TH2D *h2ptphi_yspindown = new TH2D("hptphi_ydown", ";#phi;p_{T}", nphibins, phibins, nptbins, ptbins);

    TH2D *h2xfphi_bspinup = new TH2D("hxfphi_bup", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_bspindown = new TH2D("hxfphi_bdown", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspinup = new TH2D("hxfphi_yup", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);
    TH2D *h2xfphi_yspindown = new TH2D("hxfphi_ydown", ";#phi;x_{F}", nphibins, phibins, nxfbins, xfbins);

    TH2D *h2etaphi_bspinup = new TH2D("hetaphi_bup", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_bspindown = new TH2D("hetaphi_bdown", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspinup = new TH2D("hetaphi_yup", ";#phi;#eta", nphibins, phibins, netabins, etabins);
    TH2D *h2etaphi_yspindown = new TH2D("hetaphi_ydown", ";#phi;#eta", nphibins, phibins, netabins, etabins);


    for (int i = 0; i<singleTrackTree->GetEntries(); ++i)
    //for(int i=0; i<1000; ++i)
    {
        // TEMPORARY -- need to calculate xf from pT and eta until we save it in the tree on the next iteration // 
        pz = pt * std::sinh(eta);
        xf = ( 2 * pz ) / 200;
        singleTrackTree->GetEntry(i);
        if (Verbosity > 1)
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

        if (!usable_bco_tag && crossing == 0)
        {
            continue;
        }

        if (doublecount) 
        {
            continue;
        }

        // Making some QA hists to get oriented 

        hpt->Fill(pt);
        hp->Fill(p);
        hxf->Fill(xf);
        heta->Fill(eta);
        hphi->Fill(phi);
        hquality->Fill(quality);
        h2ptp->Fill(p, pt);
        h2etaphi->Fill(phi, eta);

        // Spin sorted asymmetry inputs

        if (bspin == 1)
        {
            hphi_bspinup->Fill(phi);
            h2ptphi_bspinup->Fill(phi, pt);
            h2xfphi_bspinup->Fill(phi, xf);
            h2etaphi_bspinup->Fill(phi, eta);

        }
        if (bspin == -1)
        {
            hphi_bspindown->Fill(phi);
            h2ptphi_bspindown->Fill(phi, pt);
            h2xfphi_bspindown->Fill(phi, xf);
            h2etaphi_bspindown->Fill(phi, eta);
        }
        if (yspin == 1)
        {
            hphi_yspinup->Fill(phi);
            h2ptphi_yspinup->Fill(phi, pt);
            h2xfphi_yspinup->Fill(phi, xf);
            h2etaphi_yspinup->Fill(phi, eta);
        }
        if (yspin == -1)
        {
            hphi_yspindown->Fill(phi);
            h2ptphi_yspindown->Fill(phi, pt);
            h2xfphi_yspindown->Fill(phi, xf);
            h2etaphi_yspindown->Fill(phi, eta);
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
