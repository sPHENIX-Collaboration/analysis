#define SDMULT 1
#define CANVASW 800
#define CANVASH 700

#include <TCanvas.h>
#include <TColor.h>
#include <TCut.h>
#include <TExec.h>
#include <TF1.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TLine.h>
#include <TMath.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TRandom3.h>
#include <TStyle.h>
#include <TTree.h>
#include <TTreeIndex.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "/sphenix/user/hjheng/TrackletAna/analysis/plot/sPHENIXStyle/sPhenixStyle.C"

using namespace std;

void calccorr(const TString infilename, int CentLow = -1, int CentHigh = 10, bool applyc = false, bool applyg = false, bool applym = false, TString estag = "", TString putag = "", bool debug = false)
{
    // Options
    // int layer = TString(argv[1]).Atoi();
    // int CentLow = TString(argv[2]).Atoi();
    // int CentHigh = TString(argv[3]).Atoi();
    // bool applyc = false; // external corrections
    // bool applyg = false; // geometric correction
    // bool applym = false; // acceptance map
    // TString estag = "";
    // TString putag = "";
    // bool debug = false; // printout for debugging
    SetsPhenixStyle();
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kThermometer);

    system(Form("mkdir -p ./plot/corrections/CentBin%dto%d", CentLow, CentHigh));

    TFile *finput = new TFile(infilename, "read");
    TTree *tinput = (TTree *)finput->Get("minitree");

    TFile *fcorr = 0;
    TFile *faccep = 0;
    TFile *fes = 0;
    TFile *fpu = 0;

    if (applyc)
    {
        fcorr = new TFile(Form("./plot/corrections/Cent%dto%d/correction_hists.root", CentLow, CentHigh));
        cout << "[INFO] Applying correction factors" << endl;

        // if (applyg)
        // {
        //     // faccep = new TFile(Form("output/acceptances/%s/acceptance-%i.root", accepdir, type));
        //     cout << "[INFO] Applying geometric correction" << endl;
        // }
    }

    // if (applym)
    // {
    //     cout << "[INFO] Applying external acceptance maps" << endl;
    // }

    // if (estag.EqualTo(""))
    // {
    //     // fes = new TFile(Form("output/correction-%s-%i.root", estag, type));
    //     cout << "[INFO] Applying external event selection corrections" << endl;
    // }

    // if (putag.EqualTo(""))
    // {
    //     // fpu = new TFile(Form("output/pileup-%s.root", putag));
    //     cout << "[INFO] Applying pileup corrections" << endl;
    // }

    TCut csel;
    if (CentLow == CentHigh)
    {
        csel = Form("Centrality_mbd==%d", CentLow);
    }
    else
    {
        csel = Form("Centrality_mbd>=%d && Centrality_mbd<=%d", CentLow, CentHigh);
    }
    TCut vsel = "PV_z<=0 && PV_z>=-50";
    TCut osel = "trig";
    TCut psel = "process==0";
    TCut esel = vsel && csel && osel;
    TCut gsel = vsel && csel && psel;

    TH1::SetDefaultSumw2();

    /* Setup bins for correction histograms */
    int neta = 68;
    float etamin = -3.4;
    float etamax = 3.4;
    float etab[neta + 1];
    for (int i = 0; i <= neta; i++)
        etab[i] = i * (etamax - etamin) / neta + etamin;
    int nvz = 50;
    float vzmin = -45;
    float vzmax = 5;
    float vzb[nvz + 1];
    for (int i = 0; i <= nvz; i++)
        vzb[i] = i * (vzmax - vzmin) / nvz + vzmin;
    int nmult = 15; // Tracklet multiplicity
    float multb[nmult + 1] = {0, 10, 25, 50, 100, 200, 350, 550, 800, 1100, 1500, 2000, 2500, 3100, 3800, 5000};
    /* Setup histograms */
    TH3F *h3WEhadron = new TH3F("h3WEhadron", "h3WEhadron", neta, etab, nmult, multb, nvz, vzb); // Gen-hadron - passing event selection
    TH3F *h3WGhadron = new TH3F("h3WGhadron", "h3WGhadron", neta, etab, nmult, multb, nvz, vzb); // Gen-hadron - passing gen-level selection
    TH3F *h3WEraw = new TH3F("h3WEraw", "h3WEraw", neta, etab, nmult, multb, nvz, vzb);          // raw reco-tracklets, before alpha correction, passing esel (vz cut, trigger)
    TH3F *h3WEcorr = new TH3F("h3WEcorr", "h3WEcorr", neta, etab, nmult, multb, nvz, vzb);       // after alpha correction; corr = raw * alpha
    /* Acceptance & efficiency correction - alpha */
    TH3F *h3alpha = new TH3F("h3alpha", "h3alpha", neta, etab, nmult, multb, nvz, vzb); // h3WEhadron (gen-hadron passing evt selection) / h3WEraw
    TH3F *h3alphafinal = new TH3F("h3alphafinal", "h3alphafinal", neta, etab, nmult, multb, nvz, vzb);
    /* Vertex distribution: reweighting */
    TH1F *h1WEvz = new TH1F("h1WEvz", "h1WEvz", nvz, vzb);                           // v_z distribution passing event selection
    TH2F *h2WEvzmult = new TH2F("h2WEvzmult", "h2WEvzmult", nvz, vzb, nmult, multb); // v_z v.s tracklet multiplicity distribution passing event selection
    /* Acceptance region */
    TH2F *h2amapxev_prealpha; // Before alpha correction is applied, for checking
    TH2F *h2amapxev = new TH2F("h2amapxev", "h2amapxev", neta, etab, nvz, vzb);
    TH3F *h3amapxemv = new TH3F("h3amapxemv", "h3amapxemv", neta, etab, nmult, multb, nvz, vzb);
    /* Trigger efficiency */
    TH1F *h1teff = new TH1F("h1teff", "h1teff", nmult, multb);
    TH1F *h1WGOXteff = new TH1F("h1WGOXteff", "h1WGOXteff", nmult, multb); // passing (gsel && osel)
    TH1F *h1WGXteff = new TH1F("h1WGXteff", "h1WGXteff", nmult, multb);    // passing gsel
    /* Single diffractive event fraction */
    TH1F *h1sdf = new TH1F("h1sdf", "h1sdf", nmult, multb);
    TH1F *h1WEsdf = new TH1F("h1WEsdf", "h1WEsdf", nmult, multb);
    TH1F *h1WENGsdf = new TH1F("h1WENGsdf", "h1WENGsdf", nmult, multb);
    /* Empty correction */
    TH1F *h1empty = new TH1F("h1empty", "h1empty", neta, etab);
    /* Alpha factor in 1D and 2D (for checking) */
    TH1F *h1alpha[neta][nvz];
    TF1 *falpha[neta][nvz];
    TH2F *h2alpha_multb[nmult];
    for (int i = 0; i < neta; i++)
    {
        for (int j = 0; j < nvz; j++)
        {
            h1alpha[i][j] = new TH1F(Form("alpha_etabin%i_vzbin%i", i, j), Form("alpha_etabin%i_vzbin%i", i, j), nmult, multb);
            falpha[i][j] = new TF1(Form("falpha_etabin%i_vzbin%i", i, j), "[0]+[1]/(x+[2])+[3]/(x*x)", 1, 12000);
        }
    }
    for (int k = 0; k < nmult; k++)
    {
        h2alpha_multb[k] = new TH2F(Form("alphafinal2D_multbin%d", k), Form("alphafinal2D_multbin%d", k), neta, etab, nvz, vzb);
    }
    /*------------------------------------------------------------------------------------------------------*/

    if (applyc)
    {
        delete h2amapxev;
        delete h1teff;
        delete h1sdf;
        delete h1empty;

        h2amapxev = (TH2F *)fcorr->Get("h2amapxev")->Clone();
        h1teff = (TH1F *)fcorr->Get("h1teff")->Clone();
        h1sdf = (TH1F *)fcorr->Get("h1sdf")->Clone();
        h1empty = (TH1F *)fcorr->Get("h1empty")->Clone();

        for (int i = 0; i < neta; i++)
        {
            for (int j = 0; j < nvz; j++)
            {
                delete h1alpha[i][j];
                delete falpha[i][j];

                h1alpha[i][j] = (TH1F *)fcorr->Get(Form("alpha_etabin%i_vzbin%i", i, j));
                falpha[i][j] = (TF1 *)fcorr->Get(Form("falpha_etabin%i_vzbin%i", i, j));
            }
        }
    }

    /*------------------------------------------------------------------------------------------------------*/
    //! Not really used (?)
    TH2F *haccepmc = 0;
    TH2F *haccepdata = 0;
    TH2F *hgaccep = 0;

    if (applyg)
    {
        haccepmc = (TH2F *)faccep->Get("hmccoarse");
        haccepdata = (TH2F *)faccep->Get("hdatacoarse");
        hgaccep = (TH2F *)haccepmc->Clone("hgaccep");
        hgaccep->Divide(haccepdata);
    }
    /*------------------------------------------------------------------------------------------------------*/
    /* nevents: for normalization */
    // Number of events passing event selection and gen-level selection
    TH1F *h1WEGevent = new TH1F("h1WEGevent", "", nvz, vzb);
    int nWEGentry = tinput->Draw("PV_z>>h1WEGevent", "pu0_sel" * (esel && gsel), "goff");
    float nWEGevent = h1WEGevent->Integral(-1, -1);
    // Number of events passing gen-level selection
    TH1F *h1WGevent = new TH1F("h1WGevent", "", nvz, vzb);
    tinput->Draw("PV_z>>h1WGevent", "pu0_sel" * (gsel), "goff");
    float nWGevent = h1WGevent->Integral(-1, -1);

    cout << "[INFO] weighted events: " << nWEGevent << ", entries: " << nWEGentry << endl;
    if (nWEGentry < 1)
    {
        cout << "[ERROR] No events selected - stopping" << endl;
        return;
    }

    cout << "[INFO] Setup the acceptance maps" << endl;
    /* set acceptance maps */
    tinput->Project("h1WEvz", "PV_z", "pu0_sel" * (esel));
    tinput->Project("h2WEvzmult", "NRecotkl_Raw:PV_z", "pu0_sel" * (esel));

    // const int *amap = 0;
    // if (applym)
    // {
    //     amap = ext_accep_map(type);
    // }

    for (int i = 1; i <= neta; i++)
    {
        for (int j = 1; j <= nvz; j++)
        {
            // cout << "[check] setup h2amapxev and h3amapxemv: (nvz - j) * neta + i - 1 = " << (nvz - j) * neta + i - 1 << endl;
            if (!applyc || h2amapxev->GetBinContent(i, j) != 0)
            {
                // if (applym && !amap[(nvz - j) * neta + i - 1])
                // {
                //     h2amapxev->SetBinContent(i, j, 0);
                //     h2amapxev->SetBinError(i, j, 0);
                //     continue;
                // }

                h2amapxev->SetBinContent(i, j, h1WEvz->GetBinContent(j));
                h2amapxev->SetBinError(i, j, 0);
                for (int k = 1; k <= nmult; k++)
                    h3amapxemv->SetBinContent(i, k, j, h2WEvzmult->GetBinContent(j, k));
            }
        }
    }

    h2amapxev_prealpha = (TH2F *)h2amapxev->Clone("h2amapxev_prealpha");

    /* generator-level hadrons */
    tinput->Project("h3WEhadron", "PV_z:NRecotkl_Raw:GenHadron_eta", "pu0_sel && abs(GenHadron_eta)<4" * (esel));
    tinput->Project("h3WGhadron", "PV_z:NRecotkl_Raw:GenHadron_eta", "pu0_sel && abs(GenHadron_eta)<4" * (gsel));
    /* reconstructed tracklets */
    tinput->Project("h3WEraw", "PV_z:NRecotkl_Raw:recotklraw_eta", "pu0_sel" * (esel));

    /* calculate alpha corrections */
    cout << "[INFO] Calculate the alpha correction" << endl;
    if (!applyc)
    {
        for (int x = 1; x <= neta; x++)
        {
            for (int z = 1; z <= nvz; z++)
            {
                if (h2amapxev->GetBinContent(x, z) == 0)
                {
                    if (debug)
                        printf("   # acceptance region: eta: %2i, vz: %2i          is 0\n", x, z);
                    continue;
                }

                int count = 0;

                for (int y = 1; y <= nmult; y++)
                {
                    h1alpha[x - 1][z - 1]->SetBinContent(y, 0);
                    h1alpha[x - 1][z - 1]->SetBinError(y, 0);

                    if (h3WEraw->GetBinContent(x, y, z) != 0 && h3WEhadron->GetBinContent(x, y, z) != 0)
                    {
                        double raw = h3WEraw->GetBinContent(x, y, z);
                        double rawerr = h3WEraw->GetBinError(x, y, z);
                        double truth = h3WEhadron->GetBinContent(x, y, z);
                        double trutherr = h3WEhadron->GetBinError(x, y, z);

                        double alpha = truth / raw;
                        double alphaerr = alpha * sqrt(rawerr / raw * rawerr / raw + trutherr / truth * trutherr / truth);
                        if (debug)
                            printf("   ^ alpha calculation: eta: %2i, vz: %2i, ntl: %2i, alpha: %5.3f [%5.3f], alpha/alphaerr: %5.3f, raw/truth: %9.2f/%9.2f\n", x, z, y, alpha, alphaerr, (alpha / alphaerr), raw, truth);

                        // if (alpha > 0 && ((alpha / alphaerr > 5 && alpha < 3) || (alpha < 2)))
                        if (alpha > 0)
                        {
                            h3alpha->SetBinContent(x, y, z, alpha);
                            h3alpha->SetBinError(x, y, z, alphaerr);
                            h1alpha[x - 1][z - 1]->SetBinContent(y, alpha);
                            h1alpha[x - 1][z - 1]->SetBinError(y, alphaerr);
                        }
                        else
                        {
                            if (debug)
                                printf("     ! alpha not used\n");
                        }

                        ++count;
                    }
                }

                // if (cmax - cmin > 16 && count < nmult / 2)
                // if (count < nmult / 2)
                if (0)
                {
                    if (debug)
                        printf("   # ! acceptance map:  eta: %2i, vz: %2i          set to 0 (bad acceptance/statistics)\n", x, z);
                    h2amapxev->SetBinContent(x, z, 0);
                }
            }
        }

        for (int j = 0; j < nvz; j++)
        {
            for (int i = 0; i < neta; i++)
            {
                falpha[i][j]->SetParameter(0, 0.84);
                falpha[i][j]->SetParLimits(1, 1, 256);
                falpha[i][j]->SetParLimits(2, 0, 512);
                falpha[i][j]->SetParLimits(3, -64, 128);
                falpha[i][j]->SetLineWidth(1);
                falpha[i][j]->SetLineColor(46);
                h1alpha[i][j]->Fit(falpha[i][j], "M Q", "", 8, 7000);
                h1alpha[i][j]->Fit(falpha[i][j], "M E Q", "", 8, 7000);
                falpha[i][j] = h1alpha[i][j]->GetFunction(Form("falpha_%i_%i", i, j));
            }
        }

        /* Trigger & offline selection */
        cout << "[INFO] Calculate the trigger & offline selection efficiency" << endl;
        tinput->Project("h1WGXteff", "NRecotkl_Raw", "pu0_sel" * (gsel));
        tinput->Project("h1WGOXteff", "NRecotkl_Raw", "pu0_sel" * (gsel && osel));
        h1teff = (TH1F *)h1WGOXteff->Clone("h1teff");
        h1teff->Divide(h1WGXteff);

        /* Single diffractive event fraction (complement of gen selection) */
        cout << "[INFO] Calculate the single diffractive event fraction" << endl;
        tinput->Project("h1WENGsdf", "NRecotkl_Raw", "pu0_sel" * (esel && !gsel));
        tinput->Project("h1WEsdf", "NRecotkl_Raw", "pu0_sel" * (esel));
        h1sdf = (TH1F *)h1WENGsdf->Clone("h1sdf");
        h1sdf->Divide(h1WEsdf);

        /* vertexing efficiency */
    }

    // /* external single-diffractive fraction */
    // if (fes)
    // {
    //     delete h1sdf;
    //     delete h1teff;
    //     delete h1empty;
    //     h1sdf = (TH1F *)fes->Get("h1sdf")->Clone();
    //     h1teff = (TH1F *)fes->Get("h1teff")->Clone();
    //     h1empty = (TH1F *)fes->Get("h1empty")->Clone();
    // }

    /* Apply alpha correction */
    cout << "-------------------------------------------------------------" << endl << "[INFO] Apply the alpha correction" << endl;
    for (int x = 1; x <= neta; x++)
    {
        for (int z = 1; z <= nvz; z++)
        {
            if (h2amapxev->GetBinContent(x, z) == 0)
                continue;

            for (int y = 1; y <= nmult; y++)
            {
                double raw = h3WEraw->GetBinContent(x, y, z);
                double rawerr = h3WEraw->GetBinError(x, y, z);

                double alpha = h1alpha[x - 1][z - 1]->GetBinContent(y);
                double alphaerr = h1alpha[x - 1][z - 1]->GetBinError(y);

                if (debug)
                    printf("   ^ alpha application: eta: %2i, vz: %2i, ntl: %2i, alpha: %5.3f [%5.3f], raw: %9.2f\n", x, z, y, alpha, alphaerr, raw);

                if (alpha == 0 && falpha[x - 1][z - 1] != 0)
                {
                    alpha = falpha[x - 1][z - 1]->Eval((multb[y] + multb[y - 1]) / 2);
                    if (debug)
                        printf("     # alpha from histogram is 0 -> check fit value: %.3f\n", alpha);
                }

                if (alpha == 0)
                {
                    for (int k = 0; k < y; k++)
                    {
                        alpha = h1alpha[x - 1][z - 1]->GetBinContent(y - k);
                        alphaerr = h1alpha[x - 1][z - 1]->GetBinError(y - k);
                        if (debug)
                            printf("     # check other bin: %.3f, bin: %2i\n", alpha, y - k);
                        if (alpha != 0)
                            break;
                    }
                }

                // if (alpha <= 0 || alpha > 3.6)
                // {
                //     if (debug)
                //         printf("     ! invalid value: %.3f, reset to 1\n", alpha);
                //     alpha = 1;
                // }

                if (applyg)
                {
                    double gaccepdata = haccepdata->GetBinContent(x, z);
                    double gaccepmc = haccepmc->GetBinContent(x, z);

                    if (gaccepdata && gaccepmc)
                    {
                        alpha = alpha * gaccepmc / gaccepdata;
                        alphaerr = alphaerr * gaccepmc / gaccepdata;
                        if (debug)
                            printf("     & apply geo accep: %.3f", gaccepmc / gaccepdata);
                    }
                    else
                    {
                        if (debug)
                            printf("     ! geo accep error: eta: %2i, vz: %2i\n", x, z);
                    }
                }

                if (debug)
                    printf("     + alpha applied: [ %.3f ]\n", alpha);

                h3alphafinal->SetBinContent(x, y, z, alpha);
                h3alphafinal->SetBinError(x, y, z, alphaerr);
                h2alpha_multb[y - 1]->SetBinContent(x, z, alpha);
                h2alpha_multb[y - 1]->SetBinError(x, z, alphaerr);

                double ncorr = raw * alpha;
                double ncorrerr = rawerr * alpha;

                h3WEcorr->SetBinContent(x, y, z, ncorr);
                h3WEcorr->SetBinError(x, y, z, ncorrerr);
            }

            bool valid = false;
            for (int y = 1; y <= nmult; y++)
            {
                if (h3alphafinal->GetBinContent(x, y, z) != 1)
                    valid = true;
            }

            if (!valid)
            {
                if (debug)
                    printf("   # ! acceptance map:  eta: %2i, vz: %2i          set to 0 (invalid alpha)\n", x, z);
                h2amapxev->SetBinContent(x, z, 0);
            }
        }

        for (int z = 1; z <= nvz; z++)
        {
            if (h2amapxev->GetBinContent(x, z) == 0)
            {
                for (int y = 1; y <= nmult; y++)
                {
                    h3alphafinal->SetBinContent(x, y, z, 0);
                    h3alphafinal->SetBinError(x, y, z, 0);
                    h2alpha_multb[y - 1]->SetBinContent(x, z, 0);
                    h2alpha_multb[y - 1]->SetBinError(x, z, 0);

                    h3WEcorr->SetBinContent(x, y, z, 0);
                    h3WEcorr->SetBinError(x, y, z, 0);
                }

                if (faccep)
                    hgaccep->SetBinContent(x, z, 0);
            }
        }
    }

    /* 2d corrected, raw tracklets - project to eta and v_z*/
    TH2D *h2WEcorr = (TH2D *)h3WEcorr->Project3D("zx");
    h2WEcorr->SetName("h2WEcorr");
    TH2D *h2WEraw = (TH2D *)h3WEraw->Project3D("zx");
    h2WEraw->SetName("h2WEraw");

    /* draw geometric acceptance */
    // if (faccep)
    // {
    //     TCanvas *cga = new TCanvas("cga", "", CANVASW, CANVASH);
    //     hgaccep->Draw("colz");
    //     cga->SaveAs(Form("./plot/corrections/layer%d_CentBin%dto%d/ga.png", layer, CentLow + 1, CentHigh - 1));
    // }

    /* project 1d acceptance */
    TH1F *h1accep2xe = (TH1F *)h2amapxev->ProjectionX();
    h1accep2xe->SetName("h1accep2xe");
    h1accep2xe->Scale(1. / h1accep2xe->GetMaximum());

    /* reconstructed tracklets */
    TH1F *h1WEraw = (TH1F *)h3WEraw->Project3D("x");
    h1WEraw->SetName("h1WEraw");
    h1WEraw->Scale(1. / nWEGevent, "width");
    h1WEraw->Divide(h1accep2xe);

    TH1F *h1WEcorr = (TH1F *)h3WEcorr->Project3D("x");
    h1WEcorr->SetName("h1WEcorr");
    h1WEcorr->Scale(1. / nWEGevent, "width");
    h1WEcorr->Divide(h1accep2xe);

    /* generator-level hadrons */
    TH1F *h1WGhadron = (TH1F *)h3WGhadron->Project3D("x");
    h1WGhadron->SetName("h1WGhadron");
    h1WGhadron->Scale(1. / nWGevent, "width");

    /*------------------------------------------------------------------------------------------------------*/
    /* calculate final results */
    cout << "[INFO] Calculate final results with trigger and SDF corrections" << endl;
    TH2F *h2WEtcorr = new TH2F("h2WEtcorr", "", neta, etab, nmult, multb);

    for (int x = 1; x <= neta; x++)
    {
        for (int y = 1; y <= nmult; y++)
        {
            double sum = 0, sumerr = 0;
            double mcsum = 0, mcsumerr = 0;

            for (int z = 1; z <= nvz; z++)
            {
                if (h2amapxev->GetBinContent(x, z) != 0)
                {
                    sum += h3WEcorr->GetBinContent(x, y, z);
                    double err = h3WEcorr->GetBinError(x, y, z);
                    sumerr = sqrt(sumerr * sumerr + err * err);
                }
            }

            h2WEtcorr->SetBinContent(x, y, sum);
            h2WEtcorr->SetBinError(x, y, sumerr);
        }
    }

    /* Apply trigger, sd fraction corrections */
    for (int y = 1; y <= nmult; y++)
    {
        double sdfrac = h1sdf->GetBinContent(y);
        double trigeff = h1teff->GetBinContent(y);

        double totalc = (1 - sdfrac * SDMULT) / trigeff;

        for (int x = 1; x <= neta; x++)
        {
            if (trigeff != 0)
            {
                h2WEtcorr->SetBinContent(x, y, h2WEtcorr->GetBinContent(x, y) * totalc);
                h2WEtcorr->SetBinError(x, y, h2WEtcorr->GetBinError(x, y) * totalc);
            }

            for (int z = 1; z <= nvz; z++)
            {
                if (h2amapxev->GetBinContent(x, z) != 0)
                {
                    if (trigeff != 0)
                        h3amapxemv->SetBinContent(x, y, z, h3amapxemv->GetBinContent(x, y, z) * totalc);
                }
                else
                {
                    h3amapxemv->SetBinContent(x, y, z, 0);
                }
            }
        }
    }

    /* project 2d acceptances */
    TH1F *h1accep3xe = (TH1F *)h3amapxemv->Project3D("x");
    h1accep3xe->SetName("h1accep3xe");

    TH1F *h1WEtcorr = (TH1F *)h2WEtcorr->ProjectionX();
    h1WEtcorr->SetName("h1WEtcorr");
    h1WEtcorr->Scale(1., "width");
    h1WEtcorr->Divide(h1accep3xe);

    /* Calculate/apply empty correction */
    cout << "[INFO] Calculate and apply the empty correction" << endl;
    if (!applyc && !fes)
    {
        h1empty = (TH1F *)h1WGhadron->Clone("h1empty");
        h1empty->Divide(h1WEtcorr);
    }

    TH1F *h1WEprefinal = (TH1F *)h1WEtcorr->Clone("h1WEprefinal");
    h1WEprefinal->Multiply(h1empty);

    TH1F *h1WEfinal = (TH1F *)h1WEprefinal->Clone("h1WEfinal");
    if (fpu)
    {
        TH1F *h1pu = (TH1F *)fpu->Get("h1pu")->Clone("h1pu");
        h1WEfinal->Multiply(h1pu);
    }

    /*------------------------------------------------------------------------------------------------------*/
    /* Make checking plots */
    cout << "[INFO] Making checking plots: intermediate steps" << endl;
    TGraphAsymmErrors *trigeff = new TGraphAsymmErrors(h1WGXteff, h1WGOXteff);
    TGraphAsymmErrors *sdfrac = new TGraphAsymmErrors(h1WENGsdf, h1WEsdf);
    int nw = 3, nh = 3;
    TCanvas *ccheck = new TCanvas("ccheck", "ccheck", nw * 400, nh * 400);
    ccheck->Divide(nw, nh);
    TLatex *t = new TLatex();
    t->SetTextAlign(23);
    /* Vertex distribution (passing esel)*/
    ccheck->cd(1);
    h1WEvz->GetXaxis()->SetTitle("v_{z} (cm)");
    h1WEvz->GetYaxis()->SetTitle("Entries");
    h1WEvz->GetYaxis()->SetRangeUser(0, h1WEvz->GetMaximum() * 1.2);
    h1WEvz->SetLineColor(1);
    h1WEvz->Draw("histtext");
    t->DrawLatexNDC(0.5, 1.0, h1WEvz->GetName());
    /* Vertex distribution (passing esel) in 2D (eta,v_z), before applying alpha */
    ccheck->cd(2);
    gPad->SetRightMargin(0.13);
    h2amapxev_prealpha->GetXaxis()->SetTitle("#eta");
    h2amapxev_prealpha->GetYaxis()->SetTitle("v_{z} (cm)");
    h2amapxev_prealpha->Draw("colztext45");
    t->DrawLatexNDC(0.5, 1.0, h2amapxev_prealpha->GetName());
    /* vz v.s tracklet multiplicity distribution passing esel; for 3D acceptance map */
    ccheck->cd(3);
    gPad->SetLogy();
    gPad->SetRightMargin(0.13);
    gPad->SetLeftMargin(0.13);
    h2WEvzmult->GetYaxis()->SetMoreLogLabels();
    h2WEvzmult->GetXaxis()->SetTitle("v_{z} (cm)");
    h2WEvzmult->GetYaxis()->SetTitle("Tracklet multiplicity");
    h2WEvzmult->GetYaxis()->SetTitleOffset(2.1);
    h2WEvzmult->Draw("colztext");
    t->DrawLatexNDC(0.5, 1.0, h2WEvzmult->GetName());
    /* Trigger efficiency */
    ccheck->cd(4);
    gPad->SetLogx();
    gPad->SetLeftMargin(0.12);
    trigeff->GetXaxis()->SetMoreLogLabels();
    trigeff->GetXaxis()->SetTitle("Tracklet multiplicity");
    trigeff->GetYaxis()->SetTitle("Efficiency");
    trigeff->GetXaxis()->SetTitleOffset(1.4);
    trigeff->SetMarkerStyle(20);
    trigeff->SetMarkerColor(1);
    trigeff->SetLineColor(1);
    trigeff->Draw("ALPE");
    t->DrawLatexNDC(0.5, 1.0, "Trigger efficiency");
    /* Single-diffractive fraction */
    ccheck->cd(5);
    gPad->SetLogx();
    gPad->SetLeftMargin(0.12);
    sdfrac->GetXaxis()->SetMoreLogLabels();
    sdfrac->GetXaxis()->SetTitle("Tracklet multiplicity");
    sdfrac->GetYaxis()->SetTitle("SD event fraction");
    sdfrac->GetXaxis()->SetTitleOffset(1.4);
    sdfrac->SetMarkerStyle(20);
    sdfrac->SetMarkerColor(1);
    sdfrac->SetLineColor(1);
    sdfrac->Draw("ALPE");
    t->DrawLatexNDC(0.5, 1.0, "SD event fraction");
    /* Acceptance, after applying alpha */
    ccheck->cd(6);
    gPad->SetRightMargin(0.13);
    h2amapxev->GetXaxis()->SetTitle("#eta");
    h2amapxev->GetYaxis()->SetTitle("v_{z} (cm)");
    h2amapxev->Draw("colztext45");
    t->DrawLatexNDC(0.5, 1.0, Form("%s_postalpha", h2amapxev->GetName()));
    /* Project the 2D acceptance to 1D*/
    ccheck->cd(7);
    // gStyle->SetPaintTextFormat("1.3f");
    h1accep2xe->GetXaxis()->SetTitle("#eta");
    h1accep2xe->GetYaxis()->SetTitle("A.U");
    h1accep2xe->GetYaxis()->SetRangeUser(0, h1accep2xe->GetMaximum() * 1.2);
    h1accep2xe->SetLineColor(1);
    h1accep2xe->Draw("histtext");
    t->DrawLatexNDC(0.5, 1.0, h1accep2xe->GetName());
    /* Reco-tracklet after alpha, trigger, and SDF correction in (eta, tracklet multiplicity) */
    ccheck->cd(8);
    gPad->SetLogy();
    gPad->SetRightMargin(0.13);
    gPad->SetLeftMargin(0.13);
    h2WEtcorr->GetYaxis()->SetMoreLogLabels();
    h2WEtcorr->GetXaxis()->SetTitle("#eta");
    h2WEtcorr->GetYaxis()->SetTitle("Tracklet multiplicity");
    h2WEtcorr->GetYaxis()->SetTitleOffset(2.1);
    h2WEtcorr->Draw("colz");
    t->DrawLatexNDC(0.5, 1.0, h2WEtcorr->GetName());
    /* Empty event correction */
    ccheck->cd(9);
    h1empty->GetXaxis()->SetTitle("#eta");
    h1empty->GetYaxis()->SetTitle("Ratio");
    h1empty->GetYaxis()->SetRangeUser(h1empty->GetMinimum(0) * 0.7, h1empty->GetMaximum() * 1.2);
    h1empty->SetLineColor(1);
    h1empty->Draw("histtext");
    t->DrawLatexNDC(0.5, 1.0, h1empty->GetName());
    for (int i = 0; i < nw * nw; i++)
    {
        ccheck->cd(i + 1);
        gPad->SetTickx();
        gPad->SetTicky();
    }
    ccheck->SaveAs(Form("./plot/corrections/CentBin%dto%d/Check_Intermediate.pdf", CentLow, CentHigh));

    /* Draw 1D alpha and fits */
    cout << "[INFO] Making checking plots: 1D alpha and fits" << endl;
    TLatex *t1 = new TLatex();
    t1->SetTextAlign(23);
    TCanvas *cfalphavz = new TCanvas("cfalphavz", "", 2000, 1600);
    cfalphavz->Divide(5, 4);

    for (int x = 1; x <= neta; x++)
    {
        cfalphavz->Clear("d");
        for (int z = 1; z <= nvz; z++)
        {
            cfalphavz->cd(z);
            gPad->SetLogx();
            gPad->SetTickx();
            gPad->SetTicky();
            h1alpha[x - 1][z - 1]->GetYaxis()->SetRangeUser(0, h1alpha[x - 1][z - 1]->GetMaximum() * 2.5);
            h1alpha[x - 1][z - 1]->SetMarkerStyle(20);
            h1alpha[x - 1][z - 1]->SetMarkerSize(1);
            h1alpha[x - 1][z - 1]->SetMarkerColor(1);
            h1alpha[x - 1][z - 1]->SetLineColor(1);
            h1alpha[x - 1][z - 1]->Draw();

            TLine *l = new TLine(multb[0], 1, multb[nmult], 1);
            l->SetLineColorAlpha(38, 0.7);
            l->Draw("same");

            t1->DrawLatexNDC(0.5, 1.0, Form("%.1f < #eta < %.1f, %.1f < v_{z} < %.1f", etab[x - 1], etab[x], vzb[z - 1], vzb[z]));
        }

        cfalphavz->SaveAs(Form("./plot/corrections/CentBin%dto%d/alpha1Dfit-etabin%i.pdf", CentLow, CentHigh, x - 1));
    }

    TCanvas *cfalphaeta = new TCanvas("cfalphaeta", "", 2400, 2400);
    cfalphaeta->Divide(6, 6);

    for (int z = 1; z <= nvz; z++)
    {
        cfalphaeta->Clear("d");
        for (int x = 1; x <= neta; x++)
        {
            cfalphaeta->cd(x);
            gPad->SetLogx();
            gPad->SetTickx();
            gPad->SetTicky();
            h1alpha[x - 1][z - 1]->SetMarkerStyle(20);
            h1alpha[x - 1][z - 1]->SetMarkerSize(1);
            h1alpha[x - 1][z - 1]->SetMarkerColor(1);
            h1alpha[x - 1][z - 1]->SetLineColor(1);
            h1alpha[x - 1][z - 1]->Draw();

            TLine *l = new TLine(multb[0], 1, multb[nmult], 1);
            l->SetLineColorAlpha(38, 0.7);
            l->Draw("same");

            t1->DrawLatexNDC(0.5, 1.0, Form("%.1f < #eta < %.1f, %.1f < v_{z} < %.1f", etab[x - 1], etab[x], vzb[z - 1], vzb[z]));
        }

        cfalphaeta->SaveAs(Form("./plot/corrections/CentBin%dto%d/alpha1Dfit-vzbin%i.pdf", CentLow, CentHigh, z));
    }

    /* Draw 2D alpha (eta, vz), inclusive tracklet multiplicity */
    cout << "[INFO] Making checking plots: 2D alpha (eta, vz), inclusive tracklet multiplicity" << endl;
    TCanvas *calpha = new TCanvas("calpha", "calpha", CANVASW, CANVASH);
    gPad->SetRightMargin(0.18);
    gPad->SetLeftMargin(0.12);
    calpha->cd();
    TH2D *h2alphafinal = (TH2D *)h2WEcorr->Clone("h2alphafinal");
    h2alphafinal->Divide(h2WEraw);
    h2alphafinal->GetXaxis()->SetTitle("#eta");
    h2alphafinal->GetYaxis()->SetTitle("v_{z} (cm)");
    h2alphafinal->GetZaxis()->SetTitle("#alpha (#eta, v_{z})");
    h2alphafinal->GetZaxis()->SetTitleOffset(1.2);
    h2alphafinal->GetZaxis()->SetRangeUser(0, 3);
    gStyle->SetPaintTextFormat("1.3f");
    h2alphafinal->SetMarkerSize(0.6);
    h2alphafinal->SetContour(1000);
    h2alphafinal->Draw("colz");
    calpha->SaveAs(Form("./plot/corrections/CentBin%dto%d/alpha2D_inclTklMult.pdf", CentLow, CentHigh));

    for (int k = 0; k < nmult; k++)
    {
        calpha->Clear();
        calpha->cd();
        h2alpha_multb[k]->GetXaxis()->SetTitle("#eta");
        h2alpha_multb[k]->GetYaxis()->SetTitle("v_{z} (cm)");
        h2alpha_multb[k]->GetYaxis()->SetTitleOffset(1.1);
        h2alpha_multb[k]->GetZaxis()->SetTitle("#alpha (#eta, v_{z})");
        h2alpha_multb[k]->GetZaxis()->SetTitleOffset(1.2);
        // h2alpha_multb[k]->GetZaxis()->SetRangeUser(h2alphafinal->GetMinimum(0), h2alphafinal->GetMaximum());
        h2alpha_multb[k]->GetZaxis()->SetRangeUser(0, 3);
        gStyle->SetPaintTextFormat("1.3f");
        h2alpha_multb[k]->SetMarkerSize(0.6);
        h2alpha_multb[k]->SetContour(1000);
        h2alpha_multb[k]->Draw("colz");
        calpha->SaveAs(Form("./plot/corrections/CentBin%dto%d/alpha2D_TklMultb%d.pdf", CentLow, CentHigh, k));
    }

    /* Analysis stages - Closure test*/
    vector<TH1F *> vechist = {h1WGhadron, h1WEraw, h1WEcorr, h1WEtcorr, h1WEfinal};
    vector<const char *> vcolor{"#20262E", "#1C82AD", "#1F8A70", "#FC7300", "#EFA3C8"};
    vector<int> vlwidth = {3, 3, 3, 2, 2};
    vector<int> vstyle = {1, 1, 1, 2, 2};
    TCanvas *cstage = new TCanvas("cstage", "cstage", CANVASW, CANVASH);
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.05);
    gPad->SetTickx();
    gPad->SetTicky();
    // cstage->SetLogy();
    TH1F *hframe = new TH1F("hframe", "", 1, etamin, etamax);
    float ymax = -1;
    for (size_t i = 0; i < vechist.size(); i++)
    {
        if (vechist[i]->GetMaximum() > ymax)
            ymax = vechist[i]->GetMaximum();
    }
    hframe->GetYaxis()->SetRangeUser(0, ymax * 1.6);
    // hframe->GetYaxis()->SetRangeUser(0.1, ymax * 150);
    hframe->GetXaxis()->SetTitle("#eta");
    hframe->GetYaxis()->SetTitle("dN/d#eta");
    hframe->Draw();
    for (size_t i = 0; i < vechist.size(); i++)
    {
        vechist[i]->SetLineColor(TColor::GetColor(vcolor[i]));
        vechist[i]->SetLineWidth(vlwidth[i]);
        vechist[i]->SetLineStyle(vstyle[i]);
        vechist[i]->Draw("hist same");
    }

    TLegend *l1 = new TLegend(0.18, 0.68, 0.5, 0.92);
    l1->SetHeader(Form("Closure test: Centrality %d to %d", CentLow, CentHigh, "l"));
    l1->AddEntry(h1WGhadron, "Truth", "l");
    l1->AddEntry(h1WEraw, "Reco-tracklets before corrections", "l");
    l1->AddEntry(h1WEcorr, "Corrected for #alpha", "l");
    l1->AddEntry(h1WEtcorr, "Corrected for #alpha, trigger & sdf", "l");
    l1->AddEntry(h1WEfinal, "Corrected for #alpha, trigger & sdf, and empty event (final)", "l");
    l1->SetTextSize(0.033);
    l1->SetFillStyle(0);
    l1->SetBorderSize(0);
    l1->Draw();
    cstage->Draw();
    cstage->SaveAs(Form("./plot/corrections/CentBin%dto%d/Closure.pdf", CentLow, CentHigh));

    /* Write histograms to file*/
    TFile *outf = new TFile(Form("./plot/corrections/CentBin%dto%d/correction_hists.root", CentLow, CentHigh), "recreate");
    outf->cd();
    h2amapxev->Write();
    h1WGhadron->Write();
    h1WEraw->Write();
    h1WEcorr->Write();
    h1WEtcorr->Write();
    h1WEfinal->Write(); // Final result, reco-tracklet after alpha, trg&sdf, empty event corrections
    trigeff->Write();
    sdfrac->Write();
    h1teff->Write();
    h1WGOXteff->Write();
    h1WGXteff->Write();
    h1sdf->Write();
    h1WEsdf->Write();
    h1WENGsdf->Write();
    h1empty->Write();
    for (int i = 0; i < neta; i++)
    {
        for (int j = 0; j < nvz; j++)
        {
            if (falpha[i][j])
                falpha[i][j]->Write();
            if (h1alpha[i][j])
                h1alpha[i][j]->Write();
        }
    }

    for (int k = 0; k < nmult; k++)
        h2alpha_multb[k]->Write();

    outf->Write("", TObject::kOverwrite);
    outf->Close();
}

int main(int argc, char *argv[])
{
    // Usage: ./result 12 -1 20
    // calccorr(int layer = 12, int CentLow = -1, int CentHigh = 20, bool applyc = false, bool applyg = false, bool applym = false, TString estag = "", TString putag = "", bool debug = false)

    gStyle->SetPaintTextFormat();
    TString input = "/sphenix/user/hjheng/TrackletAna/minitree/INTT/TrackletMinitree_ana398_zvtx-20cm_dummyAlignParams/sim/TrackletAna_minitree_merged.root";
    calccorr(input, 5, 95, false, false, false, "", "", false);
    for (int i = 0; i < 10; i++)
    {
        gStyle->SetPaintTextFormat();
        int cent = 5 * (2 * i + 1);
        calccorr(input, cent, cent, false, false, false, "", "", false);
    }

    return 0;
}