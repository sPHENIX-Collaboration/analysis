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

#include "acceptance.h"

#include "/sphenix/user/hjheng/TrackletAna/analysis/plot/sPHENIXStyle/sPhenixStyle.C"

using namespace std;

void calccorr(const TString infilename,              //
              int CentLow = -1,                      //
              int CentHigh = 10,                     //
              float PvzMin = -30.,                   //
              float PvzMax = -10.,                   //
              bool applyc = false,                   //
              bool applyg = false,                   //
              bool applym = false,                   //
              const TString estag = "null",          //
              TString aselstr = "",                  //
              const TString corfiletag = "baseline", //
              const TString outfiletag = "baseline", //
              bool debug = true                      //
)
{
    SetsPhenixStyle();
    gStyle->SetOptTitle(0);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(kBird);

    TString outbasedir = TString::Format("./plot/corrections/%s", outfiletag.Data());
    TString corfiledir = TString::Format("./plot/corrections/%s", corfiletag.Data());
    TString zvtxmin_str = TString::Format("%.1f", PvzMin).ReplaceAll(".", "p").ReplaceAll("-", "m");
    TString zvtxmax_str = TString::Format("%.1f", PvzMax).ReplaceAll(".", "p").ReplaceAll("-", "m");
    TString asel_tag = (aselstr.EqualTo("") || aselstr.EqualTo("none")) ? "noasel" : aselstr.ReplaceAll(" ", "_").ReplaceAll("&&", "AND").ReplaceAll("||", "OR").ReplaceAll("(", "").ReplaceAll(")", "").ReplaceAll("==", "eq").ReplaceAll(">", "gt").ReplaceAll("<", "lt").ReplaceAll(">=", "geq").ReplaceAll("<=", "leq").ReplaceAll("!", "not");
    TString outdirtag = TString::Format("Centrality%dto%d_Zvtx%sto%s_%s", CentLow, CentHigh, zvtxmin_str.Data(), zvtxmax_str.Data(), asel_tag.Data());

    std::cout << "[INFO] Output directory tag: " << outdirtag << std::endl;

    TString outdir = Form("%s/%s", outbasedir.Data(), outdirtag.Data());
    system(Form("mkdir -p %s", outdir.Data()));

    bool IsData = (infilename.Contains("Data")) ? true : false;

    TString legtitle_head = (IsData) ? "Data" : "Simulation, closure test";
    TString legtitle_centrality = TString::Format("Centrality %d-%d", CentLow, CentHigh) + "%";
    TString legtitle_zvtx = TString::Format("Z_{vtx} [%.1f, %.1f] cm", PvzMin, PvzMax);
    TString legtitle = legtitle_head + TString::Format(": centrality %d-%d", CentLow, CentHigh) + "%, " + legtitle_zvtx;

    TFile *finput = new TFile(infilename, "read");
    TTree *tinput = (TTree *)finput->Get("minitree");

    TFile *fcorr = 0;
    TFile *faccep = 0;
    TFile *fes = 0;

    if (applyc)
    {
        cout << "[INFO] Applying correction factors" << endl;
        fcorr = new TFile(Form("%s/%s/correction_hists.root", corfiledir.Data(), outdirtag.Data()), "READ");
        if (!fcorr || fcorr->IsZombie())
        {
            cout << "[ERROR] No correction file found - exit" << endl;
            exit(1);
        }

        if (applyg)
        {
            faccep = new TFile("./plot/corrections/GeoAccepCorr_Run54280.root", "READ");
            if (!faccep || faccep->IsZombie())
            {
                cout << "[ERROR] No geometric correction file found - exit" << endl;
                exit(1);
            }
            cout << "[INFO] Applying geometric correction" << endl;
        }
    }

    if (applym)
    {
        cout << "[INFO] Applying external acceptance maps" << endl;
    }

    if (estag.EqualTo("")) // for the single-diffractive event fraction
    {
        // fes = new TFile(Form("output/correction-%s-%i.root", estag, type));
        if (!fes || fes->IsZombie())
        {
            cout << "[ERROR] No external event selection correction file found - exit" << endl;
            exit(1);
        }
        cout << "[INFO] Applying external event selection corrections" << endl;
    }

    TCut asel = (aselstr.EqualTo("")) ? "1" : aselstr.Data();                                                                 // Additional event selection (for example, BCO ranges for cross check)
    TCut vsel = Form("(PV_z<=%f && PV_z>=%f)", PvzMax, PvzMin);                                                               // Vertex selection
    TCut ssel = "1";                                                                                                          // Signal range selection (tracklets)
    TCut csel = Form("(MBD_centrality>%f && MBD_centrality<=%f)", static_cast<float>(CentLow), static_cast<float>(CentHigh)); // Centrality selection
    TCut osel = (IsData) ? "firedTrig10_MBDSNgeq2==1 && is_min_bias==1" : "is_min_bias==1";                                   // Offline event selection
    TCut psel = (IsData) ? "1" : "1";                                                                                         // Single diffractive process (simulation) - not used for now
    TCut esel = asel && vsel && csel && osel;
    TCut gsel = asel && vsel && csel && psel && ("is_min_bias==1");

    // alpha factor criteria
    float alpha_min = 0.0;
    float alpha_max = 3.6;
    float alpha_pull = 5.0;

    TH1::SetDefaultSumw2();

    /* Setup bins for correction histograms */
#define INCLUDE_VZ_BINS
#define INCLUDE_ETA_BINS
#define INCLUDE_MULT_BINS
#include "bins.h"

    /* Setup histograms */
    TH3F *h3WEhadron = new TH3F("h3WEhadron", "h3WEhadron", neta, etab, nmult, multb, nvz, vzb); // Gen-hadron - passing event selection
    TH3F *h3WGhadron = new TH3F("h3WGhadron", "h3WGhadron", neta, etab, nmult, multb, nvz, vzb); // Gen-hadron - passing gen-level selection
    TH3F *h3WEtruth;

    TH3F *h3WEraw = new TH3F("h3WEraw", "h3WEraw", neta, etab, nmult, multb, nvz, vzb);    // raw reco-tracklets, before alpha correction, passing esel (vz cut, trigger)
    TH3F *h3WEcorr = new TH3F("h3WEcorr", "h3WEcorr", neta, etab, nmult, multb, nvz, vzb); // after alpha correction; corr = raw * alpha
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
    /* Trigger/event selection efficiency */
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
    TH2F *haccepmc = 0;
    TH2F *haccepdata = 0;
    TH2F *haccepratio = 0;
    // TH2F *hgaccep = 0;

    if (applyg)
    {
        haccepmc = (TH2F *)faccep->Get("hM_sim_coarse");
        haccepdata = (TH2F *)faccep->Get("hM_data_coarse");
        haccepratio = (TH2F *)faccep->Get("hM_ratio");
        // hgaccep = (TH2F *)haccepmc->Clone("hgaccep");
        // hgaccep->Divide(haccepdata);
    }
    /*------------------------------------------------------------------------------------------------------*/
    /* nevents: for normalization */
    // Number of events passing event selection and gen-level selection
    TH1F *h1WEGevent = new TH1F("h1WEGevent", "", nvz, vzb);
    int nWEGentry = tinput->Draw("PV_z>>h1WEGevent", "vtxzwei" * (esel && gsel), "goff");
    float nWEGevent = h1WEGevent->Integral(0, h1WEGevent->GetNbinsX() + 1);
    // Number of events passing gen-level selection
    TH1F *h1WGevent = new TH1F("h1WGevent", "", nvz, vzb);
    tinput->Draw("PV_z>>h1WGevent", "vtxzwei" * (gsel), "goff");
    float nWGevent = h1WGevent->Integral(0, h1WGevent->GetNbinsX() + 1);

    cout << "[INFO] weighted events: " << nWEGevent << ", entries: " << nWEGentry << endl;
    cout << "[INFO] weighted gen-level events: " << nWGevent << endl;
    if (nWEGentry < 1)
    {
        cout << "[ERROR] No events selected - stopping" << endl;
        return;
    }

    /* set acceptance maps */
    cout << "[INFO] Setup the acceptance maps" << endl;
    tinput->Project("h1WEvz", "PV_z", "vtxzwei" * (esel));
    tinput->Project("h2WEvzmult", "NRecotkl_Raw:PV_z", "vtxzwei" * (esel));

    const int *amap = 0;
    if (applym)
    {
        amap = ext_accep_map();
    }

    for (int i = 1; i <= neta; i++)
    {
        for (int j = 1; j <= nvz; j++)
        {
            if (!applyc || h2amapxev->GetBinContent(i, j) != 0)
            {
                if (applym && !amap[(nvz - j) * neta + i - 1])
                {
                    h2amapxev->SetBinContent(i, j, 0);
                    h2amapxev->SetBinError(i, j, 0);
                    continue;
                }

                h2amapxev->SetBinContent(i, j, h1WEvz->GetBinContent(j)); // vertex distribution
                h2amapxev->SetBinError(i, j, 0);
                for (int k = 1; k <= nmult; k++)
                    h3amapxemv->SetBinContent(i, k, j, h2WEvzmult->GetBinContent(j, k));
            }
        }
    }

    h2amapxev_prealpha = (TH2F *)h2amapxev->Clone("h2amapxev_prealpha");

    /* vertex distribution */
    h1WEvz->Scale(1. / h1WEvz->GetEntries());
    h1WEvz->Fit("gaus");

    /* generator-level hadrons */
    tinput->Project("h3WEhadron", "PV_z:NRecotkl_Raw:GenHadron_eta", "vtxzwei" * (esel && "abs(GenHadron_eta)<4"));
    tinput->Project("h3WGhadron", "PV_z:NRecotkl_Raw:GenHadron_eta", "vtxzwei" * (gsel && "abs(GenHadron_eta)<4"));
    // print out number of entries
    cout << "[INFO] Entries in h3WGhadron: " << h3WGhadron->GetEntries() << endl;

    h3WEtruth = (TH3F *)h3WEhadron->Clone("h3WEtruth");

    /* reconstructed tracklets */
    tinput->Project("h3WEraw", "PV_z:NRecotkl_Raw:recotklraw_eta", "vtxzwei" * (ssel && esel));

    /* calculate alpha corrections */
    if (!applyc)
    {
        cout << "[INFO] Calculate the alpha correction" << endl;
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

                        if (alpha > alpha_min && ((alpha / alphaerr > alpha_pull && alpha < 3) || (alpha < 2)))
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

                if (CentHigh - CentLow > 95 && count < nmult / 2)
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
        tinput->Project("h1WGOXteff", "NRecotkl_Raw", "vtxzwei" * (gsel && osel));
        tinput->Project("h1WGXteff", "NRecotkl_Raw", "vtxzwei" * (gsel));
        std::cout << "h1WGOXteff: " << h1WGOXteff->Integral(-1, -1) << " h1WGXteff: " << h1WGXteff->Integral(-1, -1) << " Trigger&offline selection efficiency = " << h1WGOXteff->Integral(-1, -1) / h1WGXteff->Integral(-1, -1) << std::endl;
        h1teff = (TH1F *)h1WGOXteff->Clone("h1teff");
        h1teff->Divide(h1WGXteff);

        /* Single diffractive event fraction (complement of gen selection) */
        cout << "[INFO] Calculate the single diffractive event fraction" << endl;
        tinput->Project("h1WENGsdf", "NRecotkl_Raw", "vtxzwei" * (esel && !gsel));
        tinput->Project("h1WEsdf", "NRecotkl_Raw", "vtxzwei" * (esel));
        h1sdf = (TH1F *)h1WENGsdf->Clone("h1sdf");
        h1sdf->Divide(h1WEsdf);

        /* vertexing efficiency */
    }

    /* external single-diffractive fraction */
    if (fes)
    {
        delete h1sdf;
        // delete h1teff;
        delete h1empty;
        h1sdf = (TH1F *)fes->Get("h1sdf")->Clone();
        // h1teff = (TH1F *)fes->Get("h1teff")->Clone();
        h1empty = (TH1F *)fes->Get("h1empty")->Clone();
    }

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

                if (alpha <= alpha_min || alpha > alpha_max)
                {
                    if (debug)
                        printf("     ! invalid value: %.3f, set to 1\n", alpha);
                    alpha = 1;
                }

                if (applyg)
                {
                    double gaccepdata = haccepdata->GetBinContent(x, z);
                    double gaccepmc = haccepmc->GetBinContent(x, z);
                    double gaccepratio = haccepratio->GetBinContent(x, z);
                    double gaccepratio_err = haccepratio->GetBinError(x, z);

                    if (gaccepdata && gaccepmc)
                    {
                        alpha = alpha * gaccepratio;
                        // alphaerr = sqrt(pow(alphaerr, 2) + pow(alpha * gaccepratio_err, 2));
                        alphaerr = alphaerr * gaccepratio;
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
                // double ncorrerr = rawerr * alpha;
                double ncorrerr = sqrt(pow(rawerr * alpha, 2) + pow(raw * alphaerr, 2)); // statistical (+) systematic uncertainties

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
            }
        }
    }

    /* 2d corrected, raw tracklets - project to eta and v_z*/
    TH2D *h2WEcorr = (TH2D *)h3WEcorr->Project3D("zx");
    h2WEcorr->SetName("h2WEcorr");
    TH2D *h2WEraw = (TH2D *)h3WEraw->Project3D("zx");
    h2WEraw->SetName("h2WEraw");

    /* project 1d acceptance */
    TH1F *h1accep2xe_original = (TH1F *)h2amapxev->ProjectionX();
    h1accep2xe_original->SetName("h1accep2xe_original");
    h1accep2xe_original->Scale(1. / h1accep2xe_original->GetMaximum());
    TH1F *h1accep2xe = (TH1F *)h2amapxev->ProjectionX();
    h1accep2xe->SetName("h1accep2xe");
    h1accep2xe->Scale(1. / h1accep2xe->GetMaximum());
    // loop over the bins to check the acceptance. If the bin content is less than 0.8 (8%), set the bin content to 0 (exclude from the analysis)
    for (int i = 1; i <= h1accep2xe->GetNbinsX(); i++)
    {
        if (h1accep2xe->GetBinContent(i) < 0.8)
        {
            h1accep2xe->SetBinContent(i, 0);
            h1accep2xe->SetBinError(i, 0);
        }
    }

    /* truth (hadrons within acceptance) */
    TH1F *h1WEtruth = (TH1F *)h3WEtruth->Project3D("x");
    h1WEtruth->SetName("h1WEtruth");
    h1WEtruth->Scale(1. / nWEGevent, "width");
    h1WEtruth->Divide(h1accep2xe);

    /* reconstructed tracklets */
    TH1F *h1WEraw = (TH1F *)h3WEraw->Project3D("x");
    h1WEraw->SetName("h1WEraw");
    h1WEraw->Scale(1. / nWEGevent, "width");
    // h1WEraw->Divide(h1accep2xe);

    /* reconstructed tracklets + acceptance correction */
    TH1F *h1WErawacc = (TH1F *)h3WEraw->Project3D("x");
    h1WErawacc->SetName("h1WErawacc");
    h1WErawacc->Scale(1. / nWEGevent, "width");
    h1WErawacc->Divide(h1accep2xe);

    /* reconstructed tracklets + acceptance correction + alpha correction */
    TH1F *h1WEcorr = (TH1F *)h3WEcorr->Project3D("x");
    h1WEcorr->SetName("h1WEcorr");
    // print out the bin content of the h1WEcorr histogram and h1accep2xe
    if (debug)
    {
        for (int i = 1; i <= h1WEcorr->GetNbinsX(); i++)
        {
            printf("   ^ after alpha correction: bin %2i, h1WEcorr: %.3f, h1accep2xe: %.3f\n", i, h1WEcorr->GetBinContent(i), h1accep2xe->GetBinContent(i));
        }
    }
    h1WEcorr->Scale(1. / nWEGevent, "width");
    h1WEcorr->Divide(h1accep2xe);

    /* generator-level hadrons */
    TH1F *h1WGhadron = (TH1F *)h3WGhadron->Project3D("x");
    h1WGhadron->SetName("h1WGhadron");
    h1WGhadron->Scale(1. / nWGevent, "width");

    TH2F *h2WGhadron = (TH2F *)h3WGhadron->Project3D("zx");
    h2WGhadron->SetName("h2WGhadron");

    TH1F *h1WGhadronxm = (TH1F *)h3WGhadron->Project3D("y");
    h1WGhadronxm->SetName("h1WGhadronxm");
    h1WGhadronxm->Scale(1. / nWGevent, "width");

    /*------------------------------------------------------------------------------------------------------*/
    /* calculate final results */
    cout << "[INFO] Calculate final results with trigger and SDF corrections" << endl;
    TH2F *h2WEtcorr = new TH2F("h2WEtcorr", "", neta, etab, nmult, multb);
    TH2F *h2WEttruth = new TH2F("h2WEttruth", "", neta, etab, nmult, multb);

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

                    mcsum += h3WEtruth->GetBinContent(x, y, z);
                    double mcerr = h3WEtruth->GetBinError(x, y, z);
                    mcsumerr = sqrt(mcsumerr * mcsumerr + mcerr * mcerr);
                }
            }

            h2WEtcorr->SetBinContent(x, y, sum);
            h2WEtcorr->SetBinError(x, y, sumerr);
            h2WEttruth->SetBinContent(x, y, mcsum);
            h2WEttruth->SetBinError(x, y, mcsumerr);
            if (debug)
                printf("   ^ before appling trigger correction: eta: %2i, mult: %2i, h2WEtcorr bin content: %.3f +- %.3f\n", x, y, h2WEtcorr->GetBinContent(x, y), h2WEtcorr->GetBinError(x, y));
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
                if (debug)
                    printf("   ^ apply trigger correction: eta: %2i, mult: %2i, trigeff: %.3f, sdfrac: %.3f, totalc: %.3f, h2WEtcorr bin content: %.3f +- %.3f\n", x, y, trigeff, sdfrac, totalc, h2WEtcorr->GetBinContent(x, y), h2WEtcorr->GetBinError(x, y));
                h2WEttruth->SetBinContent(x, y, h2WEttruth->GetBinContent(x, y) * totalc);
                h2WEttruth->SetBinError(x, y, h2WEttruth->GetBinError(x, y) * totalc);
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
    TH1F *h1accep3xe_norm = (TH1F *)h3amapxemv->Project3D("x");
    h1accep3xe_norm->SetName("h1accep3xe_norm");
    h1accep3xe_norm->Scale(1. / h1accep3xe_norm->GetMaximum());
    for (int i = 1; i <= h1accep3xe_norm->GetNbinsX(); i++)
    {
        if (h1accep3xe_norm->GetBinContent(i) < 0.8)
        {
            h1accep3xe->SetBinContent(i, 0);
            h1accep3xe->SetBinError(i, 0);
        }
    }

    TH1F *h1accep3xm = (TH1F *)h3amapxemv->Project3D("y");
    h1accep3xm->SetName("h1accep3xm");

    TH1F *h1WEtcorr = (TH1F *)h2WEtcorr->ProjectionX();
    h1WEtcorr->SetName("h1WEtcorr");
    // print out the bin content of the h1WEtcorr histogram and h1accep3xe
    if (debug)
    {
        for (int i = 1; i <= h1WEtcorr->GetNbinsX(); i++)
        {
            printf("   ^ after trigger correction: bin %2i, h1WEtcorr: %.3f, h1accep3xe: %.3f\n", i, h1WEtcorr->GetBinContent(i), h1accep3xe->GetBinContent(i));
        }
    }
    h1WEtcorr->Scale(1., "width");
    h1WEtcorr->Divide(h1accep3xe);

    /* Calculate/apply empty correction */
    if (!applyc && !fes)
    {
        cout << "[INFO] Calculate and apply the empty correction" << endl;
        h1empty = (TH1F *)h1WGhadron->Clone("h1empty");
        h1empty->Divide(h1WEtcorr);
    }

    // The eta bin with an empty correction larger than 1.2 or less than 0.8 should be excluded
    for (int i = 1; i <= h1empty->GetNbinsX(); i++)
    {
        if (h1empty->GetBinContent(i) > 1.2 || h1empty->GetBinContent(i) < 0.8)
        {
            h1empty->SetBinContent(i, 0);
            h1empty->SetBinError(i, 0);
        }
    }

    cout << "[INFO] Apply the empty correction" << endl;
    TH1F *h1WEprefinal = (TH1F *)h1WEtcorr->Clone("h1WEprefinal");
    h1WEprefinal->Multiply(h1empty);

    TH1F *h1WEfinal = (TH1F *)h1WEprefinal->Clone("h1WEfinal");

    /*------------------------------------------------------------------------------------------------------*/
    /* Make diagonostic plots */
    cout << "[INFO] Making checking plots: intermediate steps" << endl;
    TGraphAsymmErrors *trigeff = new TGraphAsymmErrors();
    trigeff->Divide(h1WGOXteff, h1WGXteff, "cl=0.683 b(1,1) mode");
    TGraphAsymmErrors *sdfrac = new TGraphAsymmErrors();
    sdfrac->Divide(h1WENGsdf, h1WEsdf, "cl=0.683 b(1,1) mode");

    TCanvas *ccheck = new TCanvas("ccheck", "ccheck", 600, 600);
    gPad->SetTopMargin(0.1);
    gPad->SetRightMargin(0.15);
    gPad->SetLeftMargin(0.15);
    TLatex *t = new TLatex();
    t->SetTextAlign(23);

    /* Vertex distribution (passing esel)*/
    ccheck->cd();
    gPad->SetRightMargin(0.09);
    h1WEvz->GetXaxis()->SetTitle("vtx_{Z} [cm]");
    h1WEvz->GetYaxis()->SetTitle("Entries");
    h1WEvz->GetXaxis()->SetRangeUser(-12, 12);
    h1WEvz->GetYaxis()->SetRangeUser(0, h1WEvz->GetMaximum() * 1.2);
    h1WEvz->GetYaxis()->SetTitleOffset(1.4);
    h1WEvz->SetLineColor(1);
    h1WEvz->Draw("histtext");
    t->DrawLatexNDC(0.5, 0.97, h1WEvz->GetName());
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h1WEvz->GetName()));
    ccheck->Clear();

    /* Vertex distribution (passing esel) in 2D (eta,v_z), before applying alpha */
    ccheck->cd();
    gPad->SetRightMargin(0.17);
    h2amapxev_prealpha->GetXaxis()->SetTitle("Tracklet #eta");
    h2amapxev_prealpha->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    h2amapxev_prealpha->Draw("colz");
    t->DrawLatexNDC(0.5, 0.97, h2amapxev_prealpha->GetName());
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h2amapxev_prealpha->GetName()));
    ccheck->Clear();

    /* vz v.s tracklet multiplicity distribution passing esel; for 3D acceptance map */
    ccheck->cd();
    gPad->SetLeftMargin(0.18);
    gPad->SetLogy(0);
    h2WEvzmult->GetYaxis()->SetMoreLogLabels();
    h2WEvzmult->GetXaxis()->SetTitle("vtx_{Z} [cm]");
    h2WEvzmult->GetYaxis()->SetTitle("Multiplicity");
    h2WEvzmult->GetYaxis()->SetTitleOffset(1.8);
    h2WEvzmult->Draw("colz");
    t->DrawLatexNDC(0.5, 0.97, h2WEvzmult->GetName());
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h2WEvzmult->GetName()));
    ccheck->Clear();

    /* Trigger efficiency */
    ccheck->cd();
    gPad->SetRightMargin(0.1);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    // trigeff->GetXaxis()->SetMoreLogLabels();
    trigeff->GetXaxis()->SetMaxDigits(2);
    trigeff->GetXaxis()->SetTitle("Multiplicity");
    trigeff->GetYaxis()->SetTitle("Efficiency");
    trigeff->GetYaxis()->SetTitleOffset(1.4);
    trigeff->SetMinimum(0);
    trigeff->SetMaximum(1.2);
    trigeff->SetMarkerStyle(20);
    trigeff->SetMarkerColor(1);
    trigeff->SetLineColor(1);
    trigeff->Draw("ALPE");
    t->DrawLatexNDC(0.5, 0.97, "Offline event selection efficiency");
    ccheck->SaveAs(Form("%s/TriggerEfficiency.pdf", outdir.Data(), trigeff->GetName()));
    ccheck->Clear();

    /* Single-diffractive fraction */
    ccheck->cd();
    gPad->SetRightMargin(0.1);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    // sdfrac->GetXaxis()->SetMoreLogLabels();
    sdfrac->GetXaxis()->SetMaxDigits(2);
    sdfrac->GetXaxis()->SetTitle("Multiplicity");
    sdfrac->GetYaxis()->SetTitle("SD event fraction");
    sdfrac->GetYaxis()->SetTitleOffset(1.4);
    sdfrac->SetMinimum(0);
    sdfrac->SetMaximum(1.2);
    sdfrac->SetMarkerStyle(20);
    sdfrac->SetMarkerColor(1);
    sdfrac->SetLineColor(1);
    sdfrac->Draw("ALPE");
    t->DrawLatexNDC(0.5, 0.97, "SD event fraction");
    ccheck->SaveAs(Form("%s/SingleDiffractiveFraction.pdf", outdir.Data(), sdfrac->GetName()));
    ccheck->Clear();

    /* Acceptance, after applying alpha */
    ccheck->cd();
    gPad->SetRightMargin(0.18);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    h2amapxev->GetXaxis()->SetTitle("Tracklet #eta");
    h2amapxev->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    h2amapxev->Draw("colz");
    t->DrawLatexNDC(0.5, 0.97, Form("%s_postalpha", h2amapxev->GetName()));
    ccheck->SaveAs(Form("%s/%s_postalpha.pdf", outdir.Data(), h2amapxev->GetName()));
    ccheck->Clear();

    /* Project the 2D acceptance to 1D*/
    ccheck->cd();
    gPad->SetRightMargin(0.09);
    // gStyle->SetPaintTextFormat("1.3f");
    h1accep2xe_original->GetXaxis()->SetTitle("Tracklet #eta");
    h1accep2xe_original->GetYaxis()->SetTitle("A.U");
    h1accep2xe_original->GetYaxis()->SetRangeUser(0, h1accep2xe_original->GetMaximum() * 1.7);
    h1accep2xe_original->SetLineColor(1);
    h1accep2xe_original->Draw("histtext");
    h1accep2xe->SetLineColor(2);
    h1accep2xe->Draw("hist same");
    t->DrawLatexNDC(0.5, 0.97, legtitle_centrality.Data());
    TLegend *leg = new TLegend(0.3, 0.77, 0.85, 0.87);
    leg->AddEntry(h1accep2xe_original, "Acceptance correction (Original)", "l");
    leg->AddEntry(h1accep2xe, "Acceptance correction (>0.8)", "l");
    leg->SetTextSize(0.035);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->Draw();
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h1accep2xe->GetName()));
    ccheck->Clear();

    ccheck->cd();
    gPad->SetRightMargin(0.09);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    h1accep3xe->GetXaxis()->SetTitle("Tracklet #eta");
    h1accep3xe->GetYaxis()->SetTitle("A.U");
    h1accep3xe->GetYaxis()->SetRangeUser(0, h1accep3xe->GetMaximum() * 1.2);
    h1accep3xe->SetLineColor(1);
    h1accep3xe->Draw("histtext");
    t->DrawLatexNDC(0.5, 0.97, legtitle_centrality.Data());
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h1accep3xe->GetName()));
    ccheck->Clear();

    /* Raw tracklets, eta v.s vz*/
    ccheck->cd();
    gPad->SetRightMargin(0.18);
    gPad->SetLeftMargin(0.18);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    h2WEraw->GetXaxis()->SetTitle("Tracklet #eta");
    h2WEraw->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    h2WEraw->GetYaxis()->SetTitleOffset(1.8);
    h2WEraw->Draw("colz");
    t->DrawLatexNDC(0.5, 0.97, "Reco-tracklets before corrections (h2WEraw)");
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h2WEraw->GetName()));
    ccheck->Clear();

    /* Gen hadron; eta v.s vz */
    ccheck->cd();
    gPad->SetRightMargin(0.18);
    gPad->SetLeftMargin(0.18);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    h2WGhadron->GetXaxis()->SetTitle("#eta");
    h2WGhadron->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    h2WGhadron->GetYaxis()->SetTitleOffset(1.8);
    h2WGhadron->Draw("colz");
    t->DrawLatexNDC(0.5, 0.97, "Generated hadrons (h2WGhadron)");
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h2WGhadron->GetName()));

    /* Reco-tracklet after alpha, trigger, and SDF correction in (eta, tracklet multiplicity) */
    ccheck->cd();
    gPad->SetRightMargin(0.18);
    gPad->SetLeftMargin(0.18);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    h2WEtcorr->GetYaxis()->SetMoreLogLabels();
    h2WEtcorr->GetXaxis()->SetTitle("Tracklet #eta");
    h2WEtcorr->GetYaxis()->SetTitle("Multiplicity");
    h2WEtcorr->GetYaxis()->SetTitleOffset(1.8);
    h2WEtcorr->Draw("colz");
    t->DrawLatexNDC(0.5, 0.97, h2WEtcorr->GetName());
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h2WEtcorr->GetName()));
    ccheck->Clear();

    /* Empty event correction */
    ccheck->cd();
    gPad->SetRightMargin(0.09);
    gPad->SetLogx(0);
    gPad->SetLogy(0);
    h1empty->GetXaxis()->SetTitle("Tracklet #eta");
    h1empty->GetYaxis()->SetTitle("Ratio");
    h1empty->GetYaxis()->SetRangeUser(0, h1empty->GetMaximum() * 1.2);
    h1empty->SetLineColor(1);
    h1empty->Draw("PE1");
    t->DrawLatexNDC(0.5, 0.97, "Empty correction");
    ccheck->SaveAs(Form("%s/%s.pdf", outdir.Data(), h1empty->GetName()));
    ccheck->Clear();

    /* Draw 1D alpha and fits */
    // cout << "[INFO] Making checking plots: 1D alpha and fits" << endl;
    // TLatex *t1 = new TLatex();
    // t1->SetTextAlign(23);
    // TCanvas *cfalphavz = new TCanvas("cfalphavz", "", 2000, 1600);
    // cfalphavz->Divide(5, 4);

    // for (int x = 1; x <= neta; x++)
    // {
    //     cfalphavz->Clear("d");
    //     for (int z = 1; z <= nvz; z++)
    //     {
    //         cfalphavz->cd(z);
    //         gPad->SetLogx(0);
    //         gPad->SetTickx();
    //         gPad->SetTicky();
    //         h1alpha[x - 1][z - 1]->GetYaxis()->SetRangeUser(0, h1alpha[x - 1][z - 1]->GetMaximum() * 2.5);
    //         h1alpha[x - 1][z - 1]->SetMarkerStyle(20);
    //         h1alpha[x - 1][z - 1]->SetMarkerSize(1);
    //         h1alpha[x - 1][z - 1]->SetMarkerColor(1);
    //         h1alpha[x - 1][z - 1]->SetLineColor(1);
    //         h1alpha[x - 1][z - 1]->Draw();

    //         TLine *l = new TLine(multb[0], 1, multb[nmult], 1);
    //         l->SetLineColorAlpha(38, 0.7);
    //         l->Draw("same");

    //         t1->DrawLatexNDC(0.5, 1.0, Form("%.1f < #eta < %.1f, %.1f < vtx_{Z} < %.1f", etab[x - 1], etab[x], vzb[z - 1], vzb[z]));
    //     }

    //     cfalphavz->SaveAs(Form("%s/alpha1Dfit-etabin%i.pdf", outdir.Data(), x - 1));
    // }

    // TCanvas *cfalphaeta = new TCanvas("cfalphaeta", "", 2400, 2400);
    // cfalphaeta->Divide(6, 6);

    // for (int z = 1; z <= nvz; z++)
    // {
    //     cfalphaeta->Clear("d");
    //     for (int x = 1; x <= neta; x++)
    //     {
    //         cfalphaeta->cd(x);
    //         gPad->SetLogx(0);
    //         gPad->SetTickx();
    //         gPad->SetTicky();
    //         h1alpha[x - 1][z - 1]->SetMarkerStyle(20);
    //         h1alpha[x - 1][z - 1]->SetMarkerSize(1);
    //         h1alpha[x - 1][z - 1]->SetMarkerColor(1);
    //         h1alpha[x - 1][z - 1]->SetLineColor(1);
    //         h1alpha[x - 1][z - 1]->Draw();

    //         TLine *l = new TLine(multb[0], 1, multb[nmult], 1);
    //         l->SetLineColorAlpha(38, 0.7);
    //         l->Draw("same");

    //         t1->DrawLatexNDC(0.5, 1.0, Form("%.1f < #eta < %.1f, %.1f < vtx_{Z} < %.1f", etab[x - 1], etab[x], vzb[z - 1], vzb[z]));
    //     }

    //     cfalphaeta->SaveAs(Form("%s/alpha1Dfit-vzbin%i.pdf", outdir.Data(), z));
    // }

    /* Draw 2D alpha (eta, vz), inclusive tracklet multiplicity */
    cout << "[INFO] Making checking plots: 2D alpha (eta, vz), inclusive tracklet multiplicity" << endl;
    TCanvas *calpha = new TCanvas("calpha", "calpha", CANVASW, CANVASH);
    gPad->SetRightMargin(0.18);
    gPad->SetLeftMargin(0.13);
    calpha->cd();
    TH2D *h2alphafinal = (TH2D *)h2WEcorr->Clone("h2alphafinal");
    h2alphafinal->Divide(h2WEraw);
    h2alphafinal->GetXaxis()->SetTitle("#eta");
    h2alphafinal->GetYaxis()->SetTitle("vtx_{Z} [cm]");
    h2alphafinal->GetYaxis()->SetTitleOffset(1.3);
    // h2alphafinal->GetYaxis()->SetRangeUser(-12, 12);
    h2alphafinal->GetZaxis()->SetTitle("#alpha (#eta, vtx_{Z})");
    h2alphafinal->GetZaxis()->SetTitleOffset(1.3);
    h2alphafinal->GetZaxis()->SetRangeUser(alpha_min, alpha_max);
    gStyle->SetPaintTextFormat("1.3f");
    h2alphafinal->SetMarkerSize(0.6);
    h2alphafinal->SetContour(1000);
    h2alphafinal->Draw("colztext45");
    // TLegend *la = new TLegend(0.18, 0.18, 0.4, 0.3);
    // la->SetHeader(legtitle_centrality.Data(), "l");
    // la->Draw();
    TText *tt = new TText();
    // right and bottom adjusted
    tt->SetTextAlign(31);
    tt->SetTextSize(0.04);
    tt->DrawTextNDC(1 - gPad->GetRightMargin(), (1 - gPad->GetTopMargin()) + 0.01, legtitle_centrality.Data());
    calpha->SaveAs(Form("%s/alpha2D_inclTklMult.pdf", outdir.Data()));

    for (int k = 0; k < nmult; k++)
    {
        calpha->Clear();
        calpha->cd();
        h2alpha_multb[k]->GetXaxis()->SetTitle("#eta");
        h2alpha_multb[k]->GetYaxis()->SetTitle("vtx_{Z} [cm]");
        h2alpha_multb[k]->GetYaxis()->SetTitleOffset(1.3);
        h2alpha_multb[k]->GetZaxis()->SetTitle("#alpha (#eta, vtx_{Z})");
        h2alpha_multb[k]->GetZaxis()->SetTitleOffset(1.3);
        h2alpha_multb[k]->GetZaxis()->SetRangeUser(alpha_min, alpha_max);
        gStyle->SetPaintTextFormat("1.3f");
        h2alpha_multb[k]->SetMarkerSize(0.6);
        h2alpha_multb[k]->SetContour(1000);
        h2alpha_multb[k]->Draw("colztext45");
        // la->Draw();
        tt->DrawTextNDC(1 - gPad->GetRightMargin(), (1 - gPad->GetTopMargin()) + 0.01, legtitle_centrality.Data());
        calpha->SaveAs(Form("%s/alpha2D_TklMultb%d.pdf", outdir.Data(), k));
    }

    /* Analysis stages - Closure test*/
    // vector<TH1F *> vechist = {h1WGhadron, h1WEraw, h1WErawacc, h1WEcorr, h1WEtcorr, h1WEfinal};
    // vector<TH1F *> vechist = {h1WGhadron, h1WEraw, h1WErawacc, h1WEtcorr, h1WEfinal};
    vector<TH1F *> vechist = {h1WGhadron, h1WEraw, h1WErawacc, h1WEfinal};
    // vector<TH1F *> vechist = {h1WGhadron, h1WEraw, h1WEfinal};
    // vector<const char *> vcolor{"#20262E", "#7209b7", "#D04848", "#1C82AD", "#1F8A70", "#FC7300"};
    // vector<const char *> vcolor{"#D04848", "#1C82AD", "#1F8A70", "#FC7300", "#20262E"};
    vector<const char *> vcolor{"#D04848", "#1C82AD", "#1F8A70", "#20262E"};
    // vector<const char *> vcolor{"#D04848", "#1C82AD", "#20262E"};
    // vector<int> vlwidth = {3, 3, 3, 3, 2, 2};
    // vector<int> vlwidth = {3, 3, 3, 3, 3};
    vector<int> vlwidth = {3, 3, 3, 3};
    // vector<int> vlwidth = {3, 3, 3};
    // vector<int> vstyle = {1, 1, 1, 1, 2, 1};
    // vector<int> vstyle = {1, 1, 1, 1, 1};
    vector<int> vstyle = {1, 1, 1, 1};
    // vector<int> vstyle = {1, 1, 1};
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
    hframe->GetYaxis()->SetTitle("dN_{ch}/d#eta");
    hframe->GetYaxis()->SetTitleOffset(1.4);
    hframe->Draw();
    for (size_t i = 0; i < vechist.size(); i++)
    {
        vechist[i]->SetLineColor(TColor::GetColor(vcolor[i]));
        vechist[i]->SetLineWidth(vlwidth[i]);
        vechist[i]->SetLineStyle(vstyle[i]);
        if (i == 0)
            vechist[i]->Draw("hist same");
        else
        {
            vechist[i]->SetFillColorAlpha(TColor::GetColor(vcolor[i]), 0.6);
            vechist[i]->SetMarkerStyle(54);
            // vechist[i]->SetMarkerSize(0.6);
            vechist[i]->SetMarkerColor(TColor::GetColor(vcolor[i]));
            vechist[i]->Draw("E5 same");
        }
    }

    TLegend *l1 = new TLegend(0.18, 0.68, 0.5, 0.92);
    l1->SetHeader(legtitle.Data(), "l");
    l1->AddEntry(h1WGhadron, "Truth", "l");
    l1->AddEntry(h1WEraw, "Reco-tracklets before corrections", "pl");
    l1->AddEntry(h1WErawacc, "Corrected for acceptance", "pl");
    // l1->AddEntry(h1WEcorr, "Corrected for accep. & #alpha", "pl");
    // l1->AddEntry(h1WEtcorr, "Corrected for accep. & #alpha, evt. sel.", "pl");
    l1->AddEntry(h1WEfinal, "After corrections", "pl");
    l1->SetTextSize(0.03);
    l1->SetFillStyle(0);
    l1->SetBorderSize(0);
    l1->Draw();
    cstage->Draw();
    cstage->SaveAs(Form("%s/Closure.pdf", outdir.Data()));

    /* Write histograms to file*/
    TFile *outf = new TFile(Form("%s/correction_hists.root", outdir.Data()), "recreate");
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
    if (argc != 14)
    {
        std::cout << "Usage: ./Corrections [infile] [CentLow] [CentHigh] [pvzmin] [pvzmax] [applyc] [applyg] [applym] [estag] [aselstring] [correctionfiletag] [outfilepath] [debug]" << std::endl;
        return 0;
    }

    for (int i = 0; i < argc; i++)
    {
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
    }

    const TString input = TString(argv[1]);
    int CentLow = TString(argv[2]).Atoi();
    int CentHigh = TString(argv[3]).Atoi();
    float PVzMin = TString(argv[4]).Atof();
    float PVzMax = TString(argv[5]).Atof();
    bool applyc = (TString(argv[6]).Atoi() == 1) ? true : false;
    bool applyg = (TString(argv[7]).Atoi() == 1) ? true : false; // geometric difference; applied on data only
    bool applym = (TString(argv[8]).Atoi() == 1) ? true : false; // geometric difference map; applied on data only
    const TString estag = TString(argv[9]);
    const TString asel_string = TString(argv[10]);
    const TString correctionfiletag = TString(argv[11]);
    const TString outfilepath = TString(argv[12]);
    bool debug = (TString(argv[13]).Atoi() == 1) ? true : false;

    gStyle->SetPaintTextFormat();

    const TString aselstring = (asel_string == "null") ? "" : asel_string;
    calccorr(input, CentLow, CentHigh, PVzMin, PVzMax, applyc, applyg, applym, estag, aselstring, correctionfiletag, outfilepath, debug);

    return 0;
}