#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

void piShift(TH2 *h2, TH2 *h2_pishift)
{
    // Should only be used for asymmetry input histograms binned in (phi, X) //
    for (int i = 1; i <= h2->GetNbinsX(); i++) {
        double phiCenter = h2->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2->GetXaxis()->FindBin(phiShifted);
        std::cout << "phiBin : " << phiBin << std::endl;
        for (int j = 1; j <= h2->GetNbinsY(); j++) {
            h2_pishift->SetBinContent(i, j, h2->GetBinContent(phiBin, j));
            h2_pishift->SetBinError(i, j, h2->GetBinError(phiBin, j));
        } 
    }
}

void RawAsym(TH2 *h2_rawAN, TH2 *h2_up, TH2 *h2_up_pishift, TH2 *h2_down, TH2 *h2_down_pishift)
{
    // Should only be used for asymmetry input histograms binned in (phi, X) //
    for (int i = 1; i <= h2_rawAN->GetNbinsX(); i++) {
        for (int j = 1; j <= h2_rawAN->GetNbinsY(); j++) {
            double Nup = h2_up->GetBinContent(i,j);
            double Ndown = h2_down->GetBinContent(i,j);
            double Nup_pishift = h2_up_pishift->GetBinContent(i,j);
            double Ndown_pishift = h2_down_pishift->GetBinContent(i,j);
            double num = std::sqrt(Nup*Ndown_pishift) - std::sqrt(Ndown*Nup_pishift);
            // Add in check to ensure counts aren't too small!!
            double denom = std::sqrt(Nup*Ndown_pishift) + std::sqrt(Ndown*Nup_pishift);
            double content = num/denom;
            h2_rawAN->SetBinContent(i, j, content);
            // CHECK ASYM ERROR CALCULATION
            double err = std::sqrt(Nup * Ndown_pishift * Ndown * Nup_pishift) / std::pow(denom, 2);
            err *= std::sqrt(1./Nup + 1./Ndown + 1./Nup_pishift + 1./Ndown_pishift);
            h2_rawAN->SetBinError(i, j, err);
        }
    }
}

void RawAsym_RelLumi(TH2 *h2_rawAN, TH2 *h2_up, TH2 *h2_down, double rel_lumi)
{
    // Should only be used for asymmetry input histograms binned in (phi, X) //
    for (int i = 1; i <= h2_rawAN->GetNbinsX(); i++) {
        for (int j = 1; j <= h2_rawAN->GetNbinsY(); j++) {
            double Nup = h2_up->GetBinContent(i,j);
            double Ndown = h2_down->GetBinContent(i,j);
            double content = (Nup - rel_lumi * Ndown) / (Nup + rel_lumi * Ndown);
            h2_rawAN->SetBinContent(i, j, content);
            double err = 2 * rel_lumi * Nup * Ndown / std::pow((Nup + rel_lumi * Ndown), 2);
            err *= std::sqrt(1./Nup + 1./Ndown);
            h2_rawAN->SetBinError(i, j, err);
        }
    }

}


void RawAsymmetries(int Verbosity = 0)
{
    TFile *infile = TFile::Open("raw_trackTSSA_inputs_79516_full.root");
    //TFile *infile = TFile::Open("raw_trackTSSA_inputs_100M.root");

    // Create output file
    TFile *outfile = new TFile("raw_trackTSSA.root", "RECREATE");
    outfile->cd();

    const int nptbins = 7;
    const int nxfbins = 8;
    double ptbins[nptbins + 1] = {1., 2., 3., 4., 5., 6., 8., 10.0}; 
    double xfbins[nxfbins + 1] = {-0.2, -0.048, -0.035, -0.022, 0., 0.022, 0.035, 0.048, 0.2};
    const int nphibins = 12;
    const int netabins = 12;

    TParameter<double> *rel_lumi_param_blue = (TParameter<double>*)infile->Get("rel_lumi_blue");
    TParameter<double> *rel_lumi_param_yellow = (TParameter<double>*)infile->Get("rel_lumi_yellow");

    std::cout << "rel_lumi_blue : " << rel_lumi_param_blue->GetVal() << std::endl;
    double rel_lumi_blue, rel_lumi_yellow;
    if (rel_lumi_param_blue)
    {    
        rel_lumi_blue = rel_lumi_param_blue->GetVal();
    }
    else
    {
        std::cout << "rel_lumi_blue not found in input file! Terminating execution!" << std::endl;
        return;
    }

    if (rel_lumi_param_yellow)
    {
        rel_lumi_yellow = rel_lumi_param_yellow->GetVal();
    }
    else
    {
        std::cout << "rel_lumi_yellow not found in input file! Terminating execution!" << std::endl;
        return;
    }


    // Asymmetry Inputs
    TH1D *hphi_bspinup = (TH1D*)infile->Get("hphi_bup");
    TH1D *hphi_bspindown = (TH1D*)infile->Get("hphi_bdown");
    TH1D *hphi_yspinup = (TH1D*)infile->Get("hphi_yup");
    TH1D *hphi_yspindown = (TH1D*)infile->Get("hphi_ydown");

    TH2D *h2ptphi_bspinup = (TH2D*)infile->Get("hptphi_bup");
    TH2D *h2ptphi_bspindown = (TH2D*)infile->Get("hptphi_bdown");
    TH2D *h2ptphi_yspinup = (TH2D*)infile->Get("hptphi_yup");
    TH2D *h2ptphi_yspindown = (TH2D*)infile->Get("hptphi_ydown");

    TH2D *h2ptphi_bspinup_pos = (TH2D*)infile->Get("hptphi_bup_pos");
    TH2D *h2ptphi_bspindown_pos = (TH2D*)infile->Get("hptphi_bdown_pos");
    TH2D *h2ptphi_yspinup_pos = (TH2D*)infile->Get("hptphi_yup_pos");
    TH2D *h2ptphi_yspindown_pos = (TH2D*)infile->Get("hptphi_ydown_pos");

    TH2D *h2ptphi_bspinup_neg = (TH2D*)infile->Get("hptphi_bup_neg");
    TH2D *h2ptphi_bspindown_neg = (TH2D*)infile->Get("hptphi_bdown_neg");
    TH2D *h2ptphi_yspinup_neg = (TH2D*)infile->Get("hptphi_yup_neg");
    TH2D *h2ptphi_yspindown_neg = (TH2D*)infile->Get("hptphi_ydown_neg");

    TH2D *h2xfphi_bspinup = (TH2D*)infile->Get("hxfphi_bup");
    TH2D *h2xfphi_bspindown = (TH2D*)infile->Get("hxfphi_bdown");
    TH2D *h2xfphi_yspinup = (TH2D*)infile->Get("hxfphi_yup");
    TH2D *h2xfphi_yspindown = (TH2D*)infile->Get("hxfphi_ydown");

    TH2D *h2xfphi_bspinup_pos = (TH2D*)infile->Get("hxfphi_bup_pos");
    TH2D *h2xfphi_bspindown_pos = (TH2D*)infile->Get("hxfphi_bdown_pos");
    TH2D *h2xfphi_yspinup_pos = (TH2D*)infile->Get("hxfphi_yup_pos");
    TH2D *h2xfphi_yspindown_pos = (TH2D*)infile->Get("hxfphi_ydown_pos");

    TH2D *h2xfphi_bspinup_neg = (TH2D*)infile->Get("hxfphi_bup_neg");
    TH2D *h2xfphi_bspindown_neg = (TH2D*)infile->Get("hxfphi_bdown_neg");
    TH2D *h2xfphi_yspinup_neg = (TH2D*)infile->Get("hxfphi_yup_neg");
    TH2D *h2xfphi_yspindown_neg = (TH2D*)infile->Get("hxfphi_ydown_neg");

    //TH2D *h2etaphi_bspinup = (TH2D*)infile->Get("hetaphi_bup");
    //TH2D *h2etaphi_bspindown = (TH2D*)infile->Get("hetaphi_bdown");
    //TH2D *h2etaphi_yspinup = (TH2D*)infile->Get("hetaphi_yup");
    //TH2D *h2etaphi_yspindown = (TH2D*)infile->Get("hetaphi_ydown");


    // For pT asymmetry
    TH2D *h2ptphi_bspindown_pishift = (TH2D*)h2ptphi_bspindown->Clone("h2ptphi_bspindown_pishift");
    piShift(h2ptphi_bspindown, h2ptphi_bspindown_pishift);

    TH2D *h2ptphi_bspinup_pishift = (TH2D*)h2ptphi_bspinup->Clone("h2ptphi_bspinup_pishift");
    piShift(h2ptphi_bspinup, h2ptphi_bspinup_pishift);

    TH2D *h2ptphi_yspindown_pishift = (TH2D*)h2ptphi_yspindown->Clone("h2ptphi_yspindown_pishift");
    piShift(h2ptphi_yspindown, h2ptphi_yspindown_pishift);

    TH2D *h2ptphi_yspinup_pishift = (TH2D*)h2ptphi_yspinup->Clone("h2ptphi_yspinup_pishift");
    piShift(h2ptphi_yspinup, h2ptphi_yspinup_pishift);

    TH2D *h2ptphi_bspindown_pishift_pos = (TH2D*)h2ptphi_bspindown_pos->Clone("h2ptphi_bspindown_pishift_pos");
    piShift(h2ptphi_bspindown_pos, h2ptphi_bspindown_pishift_pos);

    TH2D *h2ptphi_bspindown_pishift_neg = (TH2D*)h2ptphi_bspindown_neg->Clone("h2ptphi_bspindown_pishift_neg");
    piShift(h2ptphi_bspindown_neg, h2ptphi_bspindown_pishift_neg);

    TH2D *h2ptphi_bspinup_pishift_pos = (TH2D*)h2ptphi_bspinup_pos->Clone("h2ptphi_bspinup_pishift_pos");
    piShift(h2ptphi_bspinup_pos, h2ptphi_bspinup_pishift_pos);

    TH2D *h2ptphi_bspinup_pishift_neg = (TH2D*)h2ptphi_bspinup_neg->Clone("h2ptphi_bspinup_pishift_neg");
    piShift(h2ptphi_bspinup_neg, h2ptphi_bspinup_pishift_neg);

    TH2D *h2ptphi_yspindown_pishift_pos = (TH2D*)h2ptphi_yspindown_pos->Clone("h2ptphi_yspindown_pishift_pos");
    piShift(h2ptphi_yspindown_pos, h2ptphi_yspindown_pishift_pos);

    TH2D *h2ptphi_yspindown_pishift_neg = (TH2D*)h2ptphi_yspindown_neg->Clone("h2ptphi_yspindown_pishift_neg");
    piShift(h2ptphi_yspindown_neg, h2ptphi_yspindown_pishift_neg);

    TH2D *h2ptphi_yspinup_pishift_pos = (TH2D*)h2ptphi_yspinup_pos->Clone("h2ptphi_yspinup_pishift_pos");
    piShift(h2ptphi_yspinup_pos, h2ptphi_yspinup_pishift_pos);

    TH2D *h2ptphi_yspinup_pishift_neg = (TH2D*)h2ptphi_yspinup_neg->Clone("h2ptphi_yspinup_pishift_neg");
    piShift(h2ptphi_yspinup_neg, h2ptphi_yspinup_pishift_neg);

    TH2D *h2ptphi_blue_rawAN = (TH2D*)h2ptphi_bspinup->Clone("h2ptphi_blue_rawAN");
    TH2D *h2ptphi_blue_pos_rawAN = (TH2D*)h2ptphi_bspinup_pos->Clone("h2ptphi_blue_pos_rawAN");
    TH2D *h2ptphi_blue_neg_rawAN = (TH2D*)h2ptphi_bspinup_neg->Clone("h2ptphi_blue_neg_rawAN");

    TH2D *h2ptphi_yellow_rawAN = (TH2D*)h2ptphi_yspinup->Clone("h2ptphi_yellow_rawAN");
    TH2D *h2ptphi_yellow_pos_rawAN = (TH2D*)h2ptphi_yspinup_pos->Clone("h2ptphi_yellow_pos_rawAN");
    TH2D *h2ptphi_yellow_neg_rawAN = (TH2D*)h2ptphi_yspinup_neg->Clone("h2ptphi_yellow_neg_rawAN");

    RawAsym(h2ptphi_blue_rawAN, h2ptphi_bspinup, h2ptphi_bspinup_pishift, h2ptphi_bspindown, h2ptphi_bspindown_pishift);
    RawAsym(h2ptphi_blue_pos_rawAN, h2ptphi_bspinup_pos, h2ptphi_bspinup_pishift_pos, h2ptphi_bspindown_pos, h2ptphi_bspindown_pishift_pos);
    RawAsym(h2ptphi_blue_neg_rawAN, h2ptphi_bspinup_neg, h2ptphi_bspinup_pishift_neg, h2ptphi_bspindown_neg, h2ptphi_bspindown_pishift_neg);

    RawAsym(h2ptphi_yellow_rawAN, h2ptphi_yspinup, h2ptphi_yspinup_pishift, h2ptphi_yspindown, h2ptphi_yspindown_pishift);
    RawAsym(h2ptphi_yellow_pos_rawAN, h2ptphi_yspinup_pos, h2ptphi_yspinup_pishift_pos, h2ptphi_yspindown_pos, h2ptphi_yspindown_pishift_pos);
    RawAsym(h2ptphi_yellow_neg_rawAN, h2ptphi_yspinup_neg, h2ptphi_yspinup_pishift_neg, h2ptphi_yspindown_neg, h2ptphi_yspindown_pishift_neg);

    TH2D *h2ptphi_blue_rawAN_RL = (TH2D*)h2ptphi_bspinup->Clone("h2ptphi_blue_rawAN_RL");
    TH2D *h2ptphi_blue_pos_rawAN_RL = (TH2D*)h2ptphi_bspinup_pos->Clone("h2ptphi_blue_pos_rawAN_RL");
    TH2D *h2ptphi_blue_neg_rawAN_RL = (TH2D*)h2ptphi_bspinup_neg->Clone("h2ptphi_blue_neg_rawAN_RL");

    TH2D *h2ptphi_yellow_rawAN_RL = (TH2D*)h2ptphi_yspinup->Clone("h2ptphi_yellow_rawAN_RL");
    TH2D *h2ptphi_yellow_pos_rawAN_RL = (TH2D*)h2ptphi_yspinup_pos->Clone("h2ptphi_yellow_pos_rawAN_RL");
    TH2D *h2ptphi_yellow_neg_rawAN_RL = (TH2D*)h2ptphi_yspinup_neg->Clone("h2ptphi_yellow_neg_rawAN_RL");

    RawAsym_RelLumi(h2ptphi_blue_rawAN_RL, h2ptphi_bspinup, h2ptphi_bspindown, rel_lumi_blue);
    RawAsym_RelLumi(h2ptphi_blue_pos_rawAN_RL, h2ptphi_bspinup_pos, h2ptphi_bspindown_pos, rel_lumi_blue);
    RawAsym_RelLumi(h2ptphi_blue_neg_rawAN_RL, h2ptphi_bspinup_neg, h2ptphi_bspindown_neg, rel_lumi_blue);

    RawAsym_RelLumi(h2ptphi_yellow_rawAN_RL, h2ptphi_yspinup, h2ptphi_yspindown, rel_lumi_yellow);
    RawAsym_RelLumi(h2ptphi_yellow_pos_rawAN_RL, h2ptphi_yspinup_pos, h2ptphi_yspindown_pos, rel_lumi_yellow);
    RawAsym_RelLumi(h2ptphi_yellow_neg_rawAN_RL, h2ptphi_yspinup_neg, h2ptphi_yspindown_neg, rel_lumi_yellow);

    TH1D *h1phi_blue_ptslice_rawAN[nptbins];
    TH1D *h1phi_blue_pos_ptslice_rawAN[nptbins];
    TH1D *h1phi_blue_neg_ptslice_rawAN[nptbins];

    TH1D *h1phi_yellow_ptslice_rawAN[nptbins];
    TH1D *h1phi_yellow_pos_ptslice_rawAN[nptbins];
    TH1D *h1phi_yellow_neg_ptslice_rawAN[nptbins];

    TF1 *AN_blue_pt[nptbins];
    TF1 *AN_blue_pos_pt[nptbins];
    TF1 *AN_blue_neg_pt[nptbins];

    TF1 *AN_yellow_pt[nptbins];
    TF1 *AN_yellow_pos_pt[nptbins];
    TF1 *AN_yellow_neg_pt[nptbins];

    TH1D *h1pt_blue_rawANfit = new TH1D("h1pt_blue_rawANfit", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_blue_pos_rawANfit = new TH1D("h1pt_blue_pos_rawANfit", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_blue_neg_rawANfit = new TH1D("h1pt_blue_neg_rawANfit", ";p_{T};A_{N}*P", nptbins, ptbins);

    TH1D *h1pt_yellow_rawANfit = new TH1D("h1pt_yellow_rawANfit", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_yellow_pos_rawANfit = new TH1D("h1pt_yellow_pos_rawANfit", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_yellow_neg_rawANfit = new TH1D("h1pt_yellow_neg_rawANfit", ";p_{T};A_{N}*P", nptbins, ptbins);

    for (int i=0; i<nptbins; i++)
    {
        // Extract square root asymmetries

        TString fitname_blue_pt = TString::Format("fit_blue_pt_%d", i);
        TString fitname_blue_pos_pt = TString::Format("fit_blue_pos_pt_%d", i);
        TString fitname_blue_neg_pt = TString::Format("fit_blue_neg_pt_%d", i);

        TString fitname_yellow_pt = TString::Format("fit_yellow_pt_%d", i);
        TString fitname_yellow_pos_pt = TString::Format("fit_yellow_pos_pt_%d", i);
        TString fitname_yellow_neg_pt = TString::Format("fit_yellow_neg_pt_%d", i);

        AN_blue_pt[i] = new TF1(fitname_blue_pt, "[0]*sin(x)", -3.14, 0);
        AN_blue_pos_pt[i] = new TF1(fitname_blue_pos_pt, "[0]*sin(x)", -3.14, 0);
        AN_blue_neg_pt[i] = new TF1(fitname_blue_neg_pt, "[0]*sin(x)", -3.14, 0);

        AN_yellow_pt[i] = new TF1(fitname_yellow_pt, "[0]*sin(x)", -3.14, 0);
        AN_yellow_pos_pt[i] = new TF1(fitname_yellow_pos_pt, "[0]*sin(x)", -3.14, 0);
        AN_yellow_neg_pt[i] = new TF1(fitname_yellow_neg_pt, "[0]*sin(x)", -3.14, 0);

        TString h1phiname_blue_ptslice_rawAN = TString::Format("h1phi_blue_rawAN_pt_%d", i);
        TString h1phiname_blue_pos_ptslice_rawAN = TString::Format("h1phi_blue_pos_rawAN_pt_%d", i);
        TString h1phiname_blue_neg_ptslice_rawAN = TString::Format("h1phi_blue_neg_rawAN_pt_%d", i);

        TString h1phiname_yellow_ptslice_rawAN = TString::Format("h1phi_yellow_rawAN_pt_%d", i);
        TString h1phiname_yellow_pos_ptslice_rawAN = TString::Format("h1phi_yellow_pos_rawAN_pt_%d", i);
        TString h1phiname_yellow_neg_ptslice_rawAN = TString::Format("h1phi_yellow_neg_rawAN_pt_%d", i);
        
        h1phi_blue_ptslice_rawAN[i] = h2ptphi_blue_rawAN->ProjectionX(h1phiname_blue_ptslice_rawAN, i+1, i+1);
        h1phi_blue_pos_ptslice_rawAN[i] = h2ptphi_blue_pos_rawAN->ProjectionX(h1phiname_blue_pos_ptslice_rawAN, i+1, i+1);
        h1phi_blue_neg_ptslice_rawAN[i] = h2ptphi_blue_neg_rawAN->ProjectionX(h1phiname_blue_neg_ptslice_rawAN, i+1, i+1);

        h1phi_yellow_ptslice_rawAN[i] = h2ptphi_yellow_rawAN->ProjectionX(h1phiname_yellow_ptslice_rawAN, i+1, i+1);
        h1phi_yellow_pos_ptslice_rawAN[i] = h2ptphi_yellow_pos_rawAN->ProjectionX(h1phiname_yellow_pos_ptslice_rawAN, i+1, i+1);
        h1phi_yellow_neg_ptslice_rawAN[i] = h2ptphi_yellow_neg_rawAN->ProjectionX(h1phiname_yellow_neg_ptslice_rawAN, i+1, i+1);

        h1phi_blue_ptslice_rawAN[i]->Fit(AN_blue_pt[i], "R");
        h1phi_blue_pos_ptslice_rawAN[i]->Fit(AN_blue_pos_pt[i], "R");
        h1phi_blue_neg_ptslice_rawAN[i]->Fit(AN_blue_neg_pt[i], "R");

        h1phi_yellow_ptslice_rawAN[i]->Fit(AN_yellow_pt[i], "R");
        h1phi_yellow_pos_ptslice_rawAN[i]->Fit(AN_yellow_pos_pt[i], "R");
        h1phi_yellow_neg_ptslice_rawAN[i]->Fit(AN_yellow_neg_pt[i], "R");

        h1pt_blue_rawANfit->SetBinContent( i+1, AN_blue_pt[i]->GetParameter(0) );
        h1pt_blue_rawANfit->SetBinError( i+1, AN_blue_pt[i]->GetParError(0) );

        h1pt_blue_pos_rawANfit->SetBinContent( i+1, AN_blue_pos_pt[i]->GetParameter(0) );
        h1pt_blue_pos_rawANfit->SetBinError( i+1, AN_blue_pos_pt[i]->GetParError(0) );

        h1pt_blue_neg_rawANfit->SetBinContent( i+1, AN_blue_neg_pt[i]->GetParameter(0) );
        h1pt_blue_neg_rawANfit->SetBinError( i+1, AN_blue_neg_pt[i]->GetParError(0) );

        h1pt_yellow_rawANfit->SetBinContent( i+1, AN_yellow_pt[i]->GetParameter(0) );
        h1pt_yellow_rawANfit->SetBinError( i+1, AN_yellow_pt[i]->GetParError(0) );

        h1pt_yellow_pos_rawANfit->SetBinContent( i+1, AN_yellow_pos_pt[i]->GetParameter(0) );
        h1pt_yellow_pos_rawANfit->SetBinError( i+1, AN_yellow_pos_pt[i]->GetParError(0) );

        h1pt_yellow_neg_rawANfit->SetBinContent( i+1, AN_yellow_neg_pt[i]->GetParameter(0) );
        h1pt_yellow_neg_rawANfit->SetBinError( i+1, AN_yellow_neg_pt[i]->GetParError(0) );

    }

    TH1D *h1phi_blue_ptslice_rawAN_RL[nptbins];
    TH1D *h1phi_blue_pos_ptslice_rawAN_RL[nptbins];
    TH1D *h1phi_blue_neg_ptslice_rawAN_RL[nptbins];

    TH1D *h1phi_yellow_ptslice_rawAN_RL[nptbins];
    TH1D *h1phi_yellow_pos_ptslice_rawAN_RL[nptbins];
    TH1D *h1phi_yellow_neg_ptslice_rawAN_RL[nptbins];

    TF1 *AN_RL_blue_pt[nptbins];
    TF1 *AN_RL_blue_pos_pt[nptbins];
    TF1 *AN_RL_blue_neg_pt[nptbins];

    TF1 *AN_RL_yellow_pt[nptbins];
    TF1 *AN_RL_yellow_pos_pt[nptbins];
    TF1 *AN_RL_yellow_neg_pt[nptbins];

    TH1D *h1pt_blue_rawANfit_RL = new TH1D("h1pt_blue_rawANfit_RL", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_blue_pos_rawANfit_RL = new TH1D("h1pt_blue_pos_rawANfit_RL", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_blue_neg_rawANfit_RL = new TH1D("h1pt_blue_neg_rawANfit_RL", ";p_{T};A_{N}*P", nptbins, ptbins);

    TH1D *h1pt_yellow_rawANfit_RL = new TH1D("h1pt_yellow_rawANfit_RL", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_yellow_pos_rawANfit_RL = new TH1D("h1pt_yellow_pos_rawANfit_RL", ";p_{T};A_{N}*P", nptbins, ptbins);
    TH1D *h1pt_yellow_neg_rawANfit_RL = new TH1D("h1pt_yellow_neg_rawANfit_RL", ";p_{T};A_{N}*P", nptbins, ptbins);


    for (int i=0; i<nptbins; i++)
    {
        // Extract relative luminosity asymmetries
        
        TString fitname_RL_blue_pt = TString::Format("fit_RL_blue_pt_%d", i);
        TString fitname_RL_blue_pos_pt = TString::Format("fit_RL_blue_pos_pt_%d", i);
        TString fitname_RL_blue_neg_pt = TString::Format("fit_RL_blue_neg_pt_%d", i);

        TString fitname_RL_yellow_pt = TString::Format("fit_RL_yellow_pt_%d", i);
        TString fitname_RL_yellow_pos_pt = TString::Format("fit_RL_yellow_pos_pt_%d", i);
        TString fitname_RL_yellow_neg_pt = TString::Format("fit_RL_yellow_neg_pt_%d", i);

        AN_RL_blue_pt[i] = new TF1(fitname_RL_blue_pt, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_blue_pos_pt[i] = new TF1(fitname_RL_blue_pos_pt, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_blue_neg_pt[i] = new TF1(fitname_RL_blue_neg_pt, "[0]*sin(x)", -3.14, 3.14);

        AN_RL_yellow_pt[i] = new TF1(fitname_RL_yellow_pt, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_yellow_pos_pt[i] = new TF1(fitname_RL_yellow_pos_pt, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_yellow_neg_pt[i] = new TF1(fitname_RL_yellow_neg_pt, "[0]*sin(x)", -3.14, 3.14);

        TString h1phiname_blue_ptslice_rawAN_RL = TString::Format("h1phi_blue_rawAN_RL_pt_%d", i);
        TString h1phiname_blue_pos_ptslice_rawAN_RL = TString::Format("h1phi_blue_pos_rawAN_RL_pt_%d", i);
        TString h1phiname_blue_neg_ptslice_rawAN_RL = TString::Format("h1phi_blue_neg_rawAN_RL_pt_%d", i);

        TString h1phiname_yellow_ptslice_rawAN_RL = TString::Format("h1phi_yellow_rawAN_RL_pt_%d", i);
        TString h1phiname_yellow_pos_ptslice_rawAN_RL = TString::Format("h1phi_yellow_pos_rawAN_RL_pt_%d", i);
        TString h1phiname_yellow_neg_ptslice_rawAN_RL = TString::Format("h1phi_yellow_neg_rawAN_RL_pt_%d", i);
        
        h1phi_blue_ptslice_rawAN_RL[i] = h2ptphi_blue_rawAN_RL->ProjectionX(h1phiname_blue_ptslice_rawAN_RL, i+1, i+1);
        h1phi_blue_pos_ptslice_rawAN_RL[i] = h2ptphi_blue_pos_rawAN_RL->ProjectionX(h1phiname_blue_pos_ptslice_rawAN_RL, i+1, i+1);
        h1phi_blue_neg_ptslice_rawAN_RL[i] = h2ptphi_blue_neg_rawAN_RL->ProjectionX(h1phiname_blue_neg_ptslice_rawAN_RL, i+1, i+1);

        h1phi_yellow_ptslice_rawAN_RL[i] = h2ptphi_yellow_rawAN_RL->ProjectionX(h1phiname_yellow_ptslice_rawAN_RL, i+1, i+1);
        h1phi_yellow_pos_ptslice_rawAN_RL[i] = h2ptphi_yellow_pos_rawAN_RL->ProjectionX(h1phiname_yellow_pos_ptslice_rawAN_RL, i+1, i+1);
        h1phi_yellow_neg_ptslice_rawAN_RL[i] = h2ptphi_yellow_neg_rawAN_RL->ProjectionX(h1phiname_yellow_neg_ptslice_rawAN_RL, i+1, i+1);

        h1phi_blue_ptslice_rawAN_RL[i]->Fit(AN_RL_blue_pt[i], "R");
        h1phi_blue_pos_ptslice_rawAN_RL[i]->Fit(AN_RL_blue_pos_pt[i], "R");
        h1phi_blue_neg_ptslice_rawAN_RL[i]->Fit(AN_RL_blue_neg_pt[i], "R");

        h1phi_yellow_ptslice_rawAN_RL[i]->Fit(AN_RL_yellow_pt[i], "R");
        h1phi_yellow_pos_ptslice_rawAN_RL[i]->Fit(AN_RL_yellow_pos_pt[i], "R");
        h1phi_yellow_neg_ptslice_rawAN_RL[i]->Fit(AN_RL_yellow_neg_pt[i], "R");

        h1pt_blue_rawANfit_RL->SetBinContent( i+1, AN_RL_blue_pt[i]->GetParameter(0) );
        h1pt_blue_rawANfit_RL->SetBinError( i+1, AN_RL_blue_pt[i]->GetParError(0) );

        h1pt_blue_pos_rawANfit_RL->SetBinContent( i+1, AN_RL_blue_pos_pt[i]->GetParameter(0) );
        h1pt_blue_pos_rawANfit_RL->SetBinError( i+1, AN_RL_blue_pos_pt[i]->GetParError(0) );

        h1pt_blue_neg_rawANfit_RL->SetBinContent( i+1, AN_RL_blue_neg_pt[i]->GetParameter(0) );
        h1pt_blue_neg_rawANfit_RL->SetBinError( i+1, AN_RL_blue_neg_pt[i]->GetParError(0) );

        h1pt_yellow_rawANfit_RL->SetBinContent( i+1, AN_RL_yellow_pt[i]->GetParameter(0) );
        h1pt_yellow_rawANfit_RL->SetBinError( i+1, AN_RL_yellow_pt[i]->GetParError(0) );

        h1pt_yellow_pos_rawANfit_RL->SetBinContent( i+1, AN_RL_yellow_pos_pt[i]->GetParameter(0) );
        h1pt_yellow_pos_rawANfit_RL->SetBinError( i+1, AN_RL_yellow_pos_pt[i]->GetParError(0) );

        h1pt_yellow_neg_rawANfit_RL->SetBinContent( i+1, AN_RL_yellow_neg_pt[i]->GetParameter(0) );
        h1pt_yellow_neg_rawANfit_RL->SetBinError( i+1, AN_RL_yellow_neg_pt[i]->GetParError(0) );
    }

    // For xF asymmetry
    TH2D *h2xfphi_bspindown_pishift = (TH2D*)h2xfphi_bspindown->Clone("h2xfphi_bspindown_pishift");
    piShift(h2xfphi_bspindown, h2xfphi_bspindown_pishift);

    TH2D *h2xfphi_bspinup_pishift = (TH2D*)h2xfphi_bspinup->Clone("h2xfphi_bspinup_pishift");
    piShift(h2xfphi_bspinup, h2xfphi_bspinup_pishift);

    TH2D *h2xfphi_yspindown_pishift = (TH2D*)h2xfphi_yspindown->Clone("h2xfphi_yspindown_pishift");
    piShift(h2xfphi_yspindown, h2xfphi_yspindown_pishift);

    TH2D *h2xfphi_yspinup_pishift = (TH2D*)h2xfphi_yspinup->Clone("h2xfphi_yspinup_pishift");
    piShift(h2xfphi_yspinup, h2xfphi_yspinup_pishift);

    TH2D *h2xfphi_bspindown_pishift_pos = (TH2D*)h2xfphi_bspindown_pos->Clone("h2xfphi_bspindown_pishift_pos");
    piShift(h2xfphi_bspindown_pos, h2xfphi_bspindown_pishift_pos);

    TH2D *h2xfphi_bspindown_pishift_neg = (TH2D*)h2xfphi_bspindown_neg->Clone("h2xfphi_bspindown_pishift_neg");
    piShift(h2xfphi_bspindown_neg, h2xfphi_bspindown_pishift_neg);

    TH2D *h2xfphi_bspinup_pishift_pos = (TH2D*)h2xfphi_bspinup_pos->Clone("h2xfphi_bspinup_pishift_pos");
    piShift(h2xfphi_bspinup_pos, h2xfphi_bspinup_pishift_pos);

    TH2D *h2xfphi_bspinup_pishift_neg = (TH2D*)h2xfphi_bspinup_neg->Clone("h2xfphi_bspinup_pishift_neg");
    piShift(h2xfphi_bspinup_neg, h2xfphi_bspinup_pishift_neg);

    TH2D *h2xfphi_yspindown_pishift_pos = (TH2D*)h2xfphi_yspindown_pos->Clone("h2xfphi_yspindown_pishift_pos");
    piShift(h2xfphi_yspindown_pos, h2xfphi_yspindown_pishift_pos);

    TH2D *h2xfphi_yspindown_pishift_neg = (TH2D*)h2xfphi_yspindown_neg->Clone("h2xfphi_yspindown_pishift_neg");
    piShift(h2xfphi_yspindown_neg, h2xfphi_yspindown_pishift_neg);

    TH2D *h2xfphi_yspinup_pishift_pos = (TH2D*)h2xfphi_yspinup_pos->Clone("h2xfphi_yspinup_pishift_pos");
    piShift(h2xfphi_yspinup_pos, h2xfphi_yspinup_pishift_pos);

    TH2D *h2xfphi_yspinup_pishift_neg = (TH2D*)h2xfphi_yspinup_neg->Clone("h2xfphi_yspinup_pishift_neg");
    piShift(h2xfphi_yspinup_neg, h2xfphi_yspinup_pishift_neg);
 
    TH2D *h2xfphi_blue_rawAN = (TH2D*)h2xfphi_bspinup->Clone("h2xfphi_blue_rawAN");
    TH2D *h2xfphi_blue_pos_rawAN = (TH2D*)h2xfphi_bspinup_pos->Clone("h2xfphi_blue_pos_rawAN");
    TH2D *h2xfphi_blue_neg_rawAN = (TH2D*)h2xfphi_bspinup_neg->Clone("h2xfphi_blue_neg_rawAN");


    TH2D *h2xfphi_yellow_rawAN = (TH2D*)h2xfphi_yspinup->Clone("h2xfphi_yellow_rawAN");
    TH2D *h2xfphi_yellow_pos_rawAN = (TH2D*)h2xfphi_yspinup_pos->Clone("h2xfphi_yellow_pos_rawAN");
    TH2D *h2xfphi_yellow_neg_rawAN = (TH2D*)h2xfphi_yspinup_neg->Clone("h2xfphi_yellow_neg_rawAN");

    RawAsym(h2xfphi_blue_rawAN, h2xfphi_bspinup, h2xfphi_bspinup_pishift, h2xfphi_bspindown, h2xfphi_bspindown_pishift);
    RawAsym(h2xfphi_blue_pos_rawAN, h2xfphi_bspinup_pos, h2xfphi_bspinup_pishift_pos, h2xfphi_bspindown_pos, h2xfphi_bspindown_pishift_pos);
    RawAsym(h2xfphi_blue_neg_rawAN, h2xfphi_bspinup_neg, h2xfphi_bspinup_pishift_neg, h2xfphi_bspindown_neg, h2xfphi_bspindown_pishift_neg);

    RawAsym(h2xfphi_yellow_rawAN, h2xfphi_yspinup, h2xfphi_yspinup_pishift, h2xfphi_yspindown, h2xfphi_yspindown_pishift);
    RawAsym(h2xfphi_yellow_pos_rawAN, h2xfphi_yspinup_pos, h2xfphi_yspinup_pishift_pos, h2xfphi_yspindown_pos, h2xfphi_yspindown_pishift_pos);
    RawAsym(h2xfphi_yellow_neg_rawAN, h2xfphi_yspinup_neg, h2xfphi_yspinup_pishift_neg, h2xfphi_yspindown_neg, h2xfphi_yspindown_pishift_neg);

    TH2D *h2xfphi_blue_rawAN_RL = (TH2D*)h2xfphi_bspinup->Clone("h2xfphi_blue_rawAN_RL");
    TH2D *h2xfphi_blue_pos_rawAN_RL = (TH2D*)h2xfphi_bspinup_pos->Clone("h2xfphi_blue_pos_rawAN_RL");
    TH2D *h2xfphi_blue_neg_rawAN_RL = (TH2D*)h2xfphi_bspinup_neg->Clone("h2xfphi_blue_neg_rawAN_RL");

    TH2D *h2xfphi_yellow_rawAN_RL = (TH2D*)h2xfphi_yspinup->Clone("h2xfphi_yellow_rawAN_RL");
    TH2D *h2xfphi_yellow_pos_rawAN_RL = (TH2D*)h2xfphi_yspinup_pos->Clone("h2xfphi_yellow_pos_rawAN_RL");
    TH2D *h2xfphi_yellow_neg_rawAN_RL = (TH2D*)h2xfphi_yspinup_neg->Clone("h2xfphi_yellow_neg_rawAN_RL");

    RawAsym_RelLumi(h2xfphi_blue_rawAN_RL, h2xfphi_bspinup, h2xfphi_bspindown, rel_lumi_blue);
    RawAsym_RelLumi(h2xfphi_blue_pos_rawAN_RL, h2xfphi_bspinup_pos, h2xfphi_bspindown_pos, rel_lumi_blue);
    RawAsym_RelLumi(h2xfphi_blue_neg_rawAN_RL, h2xfphi_bspinup_neg, h2xfphi_bspindown_neg, rel_lumi_blue);

    RawAsym_RelLumi(h2xfphi_yellow_rawAN_RL, h2xfphi_yspinup, h2xfphi_yspindown, rel_lumi_yellow);
    RawAsym_RelLumi(h2xfphi_yellow_pos_rawAN_RL, h2xfphi_yspinup_pos, h2xfphi_yspindown_pos, rel_lumi_yellow);
    RawAsym_RelLumi(h2xfphi_yellow_neg_rawAN_RL, h2xfphi_yspinup_neg, h2xfphi_yspindown_neg, rel_lumi_yellow);

    TH1D *h1phi_blue_xfslice_rawAN[nxfbins];
    TH1D *h1phi_blue_pos_xfslice_rawAN[nxfbins];
    TH1D *h1phi_blue_neg_xfslice_rawAN[nxfbins];

    TH1D *h1phi_yellow_xfslice_rawAN[nxfbins];
    TH1D *h1phi_yellow_pos_xfslice_rawAN[nxfbins];
    TH1D *h1phi_yellow_neg_xfslice_rawAN[nxfbins];


    TF1 *AN_blue_xf[nxfbins];
    TF1 *AN_blue_pos_xf[nxfbins];
    TF1 *AN_blue_neg_xf[nxfbins];

    TF1 *AN_yellow_xf[nxfbins];
    TF1 *AN_yellow_pos_xf[nxfbins];
    TF1 *AN_yellow_neg_xf[nxfbins];
    
    TH1D *h1xf_blue_rawANfit = new TH1D("h1xf_blue_rawANfit", ";x_{F};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_blue_pos_rawANfit = new TH1D("h1xf_blue_pos_rawANfit", ";x_{F};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_blue_neg_rawANfit = new TH1D("h1xf_blue_neg_rawANfit", ";x_{F};A_{N}*P", nxfbins, xfbins);

    TH1D *h1xf_yellow_rawANfit = new TH1D("h1xf_yellow_rawANfit", ";x_{F};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_yellow_pos_rawANfit = new TH1D("h1xf_yellow_pos_rawANfit", ";x_{F};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_yellow_neg_rawANfit = new TH1D("h1xf_yellow_neg_rawANfit", ";x_{F};A_{N}*P", nxfbins, xfbins);

    for (int i=0; i<nxfbins; i++)
    {

        // Extract square root asymmetries

        TString fitname_blue_xf = TString::Format("fit_blue_xf_%d", i);
        TString fitname_blue_pos_xf = TString::Format("fit_blue_pos_xf_%d", i);
        TString fitname_blue_neg_xf = TString::Format("fit_blue_neg_xf_%d", i);

        TString fitname_yellow_xf = TString::Format("fit_yellow_xf_%d", i);
        TString fitname_yellow_pos_xf = TString::Format("fit_yellow_pos_xf_%d", i);
        TString fitname_yellow_neg_xf = TString::Format("fit_yellow_neg_xf_%d", i);

        AN_blue_xf[i] = new TF1(fitname_blue_xf, "[0]*sin(x)", -3.14, 0);
        AN_blue_pos_xf[i] = new TF1(fitname_blue_pos_xf, "[0]*sin(x)", -3.14, 0);
        AN_blue_neg_xf[i] = new TF1(fitname_blue_neg_xf, "[0]*sin(x)", -3.14, 0);

        AN_yellow_xf[i] = new TF1(fitname_yellow_xf, "[0]*sin(x)", -3.14, 0);
        AN_yellow_pos_xf[i] = new TF1(fitname_yellow_pos_xf, "[0]*sin(x)", -3.14, 0);
        AN_yellow_neg_xf[i] = new TF1(fitname_yellow_neg_xf, "[0]*sin(x)", -3.14, 0);

        TString h1phiname_blue_xfslice_rawAN = TString::Format("h1phi_blue_rawAN_xf_%d", i);
        TString h1phiname_blue_pos_xfslice_rawAN = TString::Format("h1phi_blue_pos_rawAN_xf_%d", i);
        TString h1phiname_blue_neg_xfslice_rawAN = TString::Format("h1phi_blue_neg_rawAN_xf_%d", i);

        TString h1phiname_yellow_xfslice_rawAN = TString::Format("h1phi_yellow_rawAN_xf_%d", i);
        TString h1phiname_yellow_pos_xfslice_rawAN = TString::Format("h1phi_yellow_pos_rawAN_xf_%d", i);
        TString h1phiname_yellow_neg_xfslice_rawAN = TString::Format("h1phi_yellow_neg_rawAN_xf_%d", i);

        
        h1phi_blue_xfslice_rawAN[i] = h2xfphi_blue_rawAN->ProjectionX(h1phiname_blue_xfslice_rawAN, i+1, i+1);
        h1phi_blue_pos_xfslice_rawAN[i] = h2xfphi_blue_pos_rawAN->ProjectionX(h1phiname_blue_pos_xfslice_rawAN, i+1, i+1);
        h1phi_blue_neg_xfslice_rawAN[i] = h2xfphi_blue_neg_rawAN->ProjectionX(h1phiname_blue_neg_xfslice_rawAN, i+1, i+1);

        h1phi_yellow_xfslice_rawAN[i] = h2xfphi_yellow_rawAN->ProjectionX(h1phiname_yellow_xfslice_rawAN, i+1, i+1);
        h1phi_yellow_pos_xfslice_rawAN[i] = h2xfphi_yellow_pos_rawAN->ProjectionX(h1phiname_yellow_pos_xfslice_rawAN, i+1, i+1);
        h1phi_yellow_neg_xfslice_rawAN[i] = h2xfphi_yellow_neg_rawAN->ProjectionX(h1phiname_yellow_neg_xfslice_rawAN, i+1, i+1);


        h1phi_blue_xfslice_rawAN[i]->Fit(AN_blue_xf[i], "R");
        h1phi_blue_pos_xfslice_rawAN[i]->Fit(AN_blue_pos_xf[i], "R");
        h1phi_blue_neg_xfslice_rawAN[i]->Fit(AN_blue_neg_xf[i], "R");


        h1phi_yellow_xfslice_rawAN[i]->Fit(AN_yellow_xf[i], "R");
        h1phi_yellow_pos_xfslice_rawAN[i]->Fit(AN_yellow_pos_xf[i], "R");
        h1phi_yellow_neg_xfslice_rawAN[i]->Fit(AN_yellow_neg_xf[i], "R");

        h1xf_blue_rawANfit->SetBinContent( i+1, AN_blue_xf[i]->GetParameter(0) );
        h1xf_blue_rawANfit->SetBinError( i+1, AN_blue_xf[i]->GetParError(0) );

        h1xf_blue_pos_rawANfit->SetBinContent( i+1, AN_blue_pos_xf[i]->GetParameter(0) );
        h1xf_blue_pos_rawANfit->SetBinError( i+1, AN_blue_pos_xf[i]->GetParError(0) );

        h1xf_blue_neg_rawANfit->SetBinContent( i+1, AN_blue_neg_xf[i]->GetParameter(0) );
        h1xf_blue_neg_rawANfit->SetBinError( i+1, AN_blue_neg_xf[i]->GetParError(0) );

        h1xf_yellow_rawANfit->SetBinContent( i+1, AN_yellow_xf[i]->GetParameter(0) );
        h1xf_yellow_rawANfit->SetBinError( i+1, AN_yellow_xf[i]->GetParError(0) );

        h1xf_yellow_pos_rawANfit->SetBinContent( i+1, AN_yellow_pos_xf[i]->GetParameter(0) );
        h1xf_yellow_pos_rawANfit->SetBinError( i+1, AN_yellow_pos_xf[i]->GetParError(0) );

        h1xf_yellow_neg_rawANfit->SetBinContent( i+1, AN_yellow_neg_xf[i]->GetParameter(0) );
        h1xf_yellow_neg_rawANfit->SetBinError( i+1, AN_yellow_neg_xf[i]->GetParError(0) );  
        
    }

    TH1D *h1phi_blue_xfslice_rawAN_RL[nxfbins];
    TH1D *h1phi_blue_pos_xfslice_rawAN_RL[nxfbins];
    TH1D *h1phi_blue_neg_xfslice_rawAN_RL[nxfbins];

    TH1D *h1phi_yellow_xfslice_rawAN_RL[nxfbins];
    TH1D *h1phi_yellow_pos_xfslice_rawAN_RL[nxfbins];
    TH1D *h1phi_yellow_neg_xfslice_rawAN_RL[nxfbins];

    TF1 *AN_RL_blue_xf[nxfbins];
    TF1 *AN_RL_blue_pos_xf[nxfbins];
    TF1 *AN_RL_blue_neg_xf[nxfbins];

    TF1 *AN_RL_yellow_xf[nxfbins];
    TF1 *AN_RL_yellow_pos_xf[nxfbins];
    TF1 *AN_RL_yellow_neg_xf[nxfbins];

    TH1D *h1xf_blue_rawANfit_RL = new TH1D("h1xf_blue_rawANfit_RL", ";p_{T};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_blue_pos_rawANfit_RL = new TH1D("h1xf_blue_pos_rawANfit_RL", ";p_{T};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_blue_neg_rawANfit_RL = new TH1D("h1xf_blue_neg_rawANfit_RL", ";p_{T};A_{N}*P", nxfbins, xfbins);

    TH1D *h1xf_yellow_rawANfit_RL = new TH1D("h1xf_yellow_rawANfit_RL", ";p_{T};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_yellow_pos_rawANfit_RL = new TH1D("h1xf_yellow_pos_rawANfit_RL", ";p_{T};A_{N}*P", nxfbins, xfbins);
    TH1D *h1xf_yellow_neg_rawANfit_RL = new TH1D("h1xf_yellow_neg_rawANfit_RL", ";p_{T};A_{N}*P", nxfbins, xfbins);

    // There is a problem with the final xF bin that I need to figure out... 
    for (int i=0; i<nxfbins; i++)
    {

        // Extract relative luminosity asymmetries
        
        TString fitname_RL_blue_xf = TString::Format("fit_RL_blue_xf_%d", i);
        TString fitname_RL_blue_pos_xf = TString::Format("fit_RL_blue_pos_xf_%d", i);
        TString fitname_RL_blue_neg_xf = TString::Format("fit_RL_blue_neg_xf_%d", i);

        TString fitname_RL_yellow_xf = TString::Format("fit_RL_yellow_xf_%d", i);
        TString fitname_RL_yellow_pos_xf = TString::Format("fit_RL_yellow_pos_xf_%d", i);
        TString fitname_RL_yellow_neg_xf = TString::Format("fit_RL_yellow_neg_xf_%d", i);

        AN_RL_blue_xf[i] = new TF1(fitname_RL_blue_xf, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_blue_pos_xf[i] = new TF1(fitname_RL_blue_pos_xf, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_blue_neg_xf[i] = new TF1(fitname_RL_blue_neg_xf, "[0]*sin(x)", -3.14, 3.14);

        AN_RL_yellow_xf[i] = new TF1(fitname_RL_yellow_xf, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_yellow_pos_xf[i] = new TF1(fitname_RL_yellow_pos_xf, "[0]*sin(x)", -3.14, 3.14);
        AN_RL_yellow_neg_xf[i] = new TF1(fitname_RL_yellow_neg_xf, "[0]*sin(x)", -3.14, 3.14);

        TString h1phiname_blue_xfslice_rawAN_RL = TString::Format("h1phi_blue_rawAN_RL_xf_%d", i);
        TString h1phiname_blue_pos_xfslice_rawAN_RL = TString::Format("h1phi_blue_pos_rawAN_RL_xf_%d", i);
        TString h1phiname_blue_neg_xfslice_rawAN_RL = TString::Format("h1phi_blue_neg_rawAN_RL_xf_%d", i);

        TString h1phiname_yellow_xfslice_rawAN_RL = TString::Format("h1phi_yellow_rawAN_RL_xf_%d", i);
        TString h1phiname_yellow_pos_xfslice_rawAN_RL = TString::Format("h1phi_yellow_pos_rawAN_RL_xf_%d", i);
        TString h1phiname_yellow_neg_xfslice_rawAN_RL = TString::Format("h1phi_yellow_neg_rawAN_RL_xf_%d", i);
        
        h1phi_blue_xfslice_rawAN_RL[i] = h2xfphi_blue_rawAN_RL->ProjectionX(h1phiname_blue_xfslice_rawAN_RL, i+1, i+1);
        h1phi_blue_pos_xfslice_rawAN_RL[i] = h2xfphi_blue_pos_rawAN_RL->ProjectionX(h1phiname_blue_pos_xfslice_rawAN_RL, i+1, i+1);
        h1phi_blue_neg_xfslice_rawAN_RL[i] = h2xfphi_blue_neg_rawAN_RL->ProjectionX(h1phiname_blue_neg_xfslice_rawAN_RL, i+1, i+1);

        h1phi_yellow_xfslice_rawAN_RL[i] = h2xfphi_yellow_rawAN_RL->ProjectionX(h1phiname_yellow_xfslice_rawAN_RL, i+1, i+1);
        h1phi_yellow_pos_xfslice_rawAN_RL[i] = h2xfphi_yellow_pos_rawAN_RL->ProjectionX(h1phiname_yellow_pos_xfslice_rawAN_RL, i+1, i+1);
        h1phi_yellow_neg_xfslice_rawAN_RL[i] = h2xfphi_yellow_neg_rawAN_RL->ProjectionX(h1phiname_yellow_neg_xfslice_rawAN_RL, i+1, i+1);

        h1phi_blue_xfslice_rawAN_RL[i]->Fit(AN_RL_blue_xf[i], "R");
        h1phi_blue_pos_xfslice_rawAN_RL[i]->Fit(AN_RL_blue_pos_xf[i], "R");
        h1phi_blue_neg_xfslice_rawAN_RL[i]->Fit(AN_RL_blue_neg_xf[i], "R");

        h1phi_yellow_xfslice_rawAN_RL[i]->Fit(AN_RL_yellow_xf[i], "R");
        h1phi_yellow_pos_xfslice_rawAN_RL[i]->Fit(AN_RL_yellow_pos_xf[i], "R");
        h1phi_yellow_neg_xfslice_rawAN_RL[i]->Fit(AN_RL_yellow_neg_xf[i], "R");

        h1xf_blue_rawANfit_RL->SetBinContent( i+1, AN_RL_blue_xf[i]->GetParameter(0) );
        h1xf_blue_rawANfit_RL->SetBinError( i+1, AN_RL_blue_xf[i]->GetParError(0) );

        h1xf_blue_pos_rawANfit_RL->SetBinContent( i+1, AN_RL_blue_pos_xf[i]->GetParameter(0) );
        h1xf_blue_pos_rawANfit_RL->SetBinError( i+1, AN_RL_blue_pos_xf[i]->GetParError(0) );

        h1xf_blue_neg_rawANfit_RL->SetBinContent( i+1, AN_RL_blue_neg_xf[i]->GetParameter(0) );
        h1xf_blue_neg_rawANfit_RL->SetBinError( i+1, AN_RL_blue_neg_xf[i]->GetParError(0) );

        h1xf_yellow_rawANfit_RL->SetBinContent( i+1, AN_RL_yellow_xf[i]->GetParameter(0) );
        h1xf_yellow_rawANfit_RL->SetBinError( i+1, AN_RL_yellow_xf[i]->GetParError(0) );

        h1xf_yellow_pos_rawANfit_RL->SetBinContent( i+1, AN_RL_yellow_pos_xf[i]->GetParameter(0) );
        h1xf_yellow_pos_rawANfit_RL->SetBinError( i+1, AN_RL_yellow_pos_xf[i]->GetParError(0) );

        h1xf_yellow_neg_rawANfit_RL->SetBinContent( i+1, AN_RL_yellow_neg_xf[i]->GetParameter(0) );
        h1xf_yellow_neg_rawANfit_RL->SetBinError( i+1, AN_RL_yellow_neg_xf[i]->GetParError(0) );      

    }


    outfile->Write();
}