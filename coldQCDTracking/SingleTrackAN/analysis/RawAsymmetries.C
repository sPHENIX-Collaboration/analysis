#include <TTree.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

void RawAsymmetries(int Verbosity = 0)
{
    TFile *infile = TFile::Open("raw_trackTSSA_inputs.root");

    // Create output file
    TFile *outfile = new TFile("raw_trackTSSA.root", "RECREATE");
    outfile->cd();

    const int nptbins = 7;
    const int nxfbins = 8;
    const int nphibins = 12;
    const int netabins = 12;

    // Asymmetry Inputs
    TH1D *hphi_bspinup = (TH1D*)infile->Get("hphi_bup");
    TH1D *hphi_bspindown = (TH1D*)infile->Get("hphi_bdown");
    TH1D *hphi_yspinup = (TH1D*)infile->Get("hphi_yup");
    TH1D *hphi_yspindown = (TH1D*)infile->Get("hphi_ydown");

    TH2D *h2ptphi_bspinup = (TH2D*)infile->Get("hptphi_bup");
    TH2D *h2ptphi_bspindown = (TH2D*)infile->Get("hptphi_bdown");
    TH2D *h2ptphi_yspinup = (TH2D*)infile->Get("hptphi_yup");
    TH2D *h2ptphi_yspindown = (TH2D*)infile->Get("hptphi_ydown");

    TH2D *h2xfphi_bspinup = (TH2D*)infile->Get("hxfphi_bup");
    TH2D *h2xfphi_bspindown = (TH2D*)infile->Get("hxfphi_bdown");
    TH2D *h2xfphi_yspinup = (TH2D*)infile->Get("hxfphi_yup");
    TH2D *h2xfphi_yspindown = (TH2D*)infile->Get("hxfphi_ydown");

    //TH2D *h2etaphi_bspinup = (TH2D*)infile->Get("hetaphi_bup");
    //TH2D *h2etaphi_bspindown = (TH2D*)infile->Get("hetaphi_bdown");
    //TH2D *h2etaphi_yspinup = (TH2D*)infile->Get("hetaphi_yup");
    //TH2D *h2etaphi_yspindown = (TH2D*)infile->Get("hetaphi_ydown");


    // For pT asymmetry
    TH2D *h2ptphi_bspindown_pishift = (TH2D*)h2ptphi_bspindown->Clone("h2ptphi_bspindown_pishift");
    for (int i = 1; i <= h2ptphi_bspindown->GetNbinsX(); i++) {
        double phiCenter = h2ptphi_bspindown->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2ptphi_bspindown->GetXaxis()->FindBin(phiShifted);
        std::cout << "phiBin : " << phiBin << std::endl;
        for (int j = 1; j <= h2ptphi_bspindown->GetNbinsY(); j++) {
            h2ptphi_bspindown_pishift->SetBinContent(i, j, h2ptphi_bspindown->GetBinContent(phiBin, j));
            h2ptphi_bspindown_pishift->SetBinError(i, j, h2ptphi_bspindown->GetBinError(phiBin, j));
        } 
    }

    TH2D *h2ptphi_bspinup_pishift = (TH2D*)h2ptphi_bspinup->Clone("h2ptphi_bspinup_pishift");
    for (int i = 1; i <= h2ptphi_bspinup->GetNbinsX(); i++) {
        double phiCenter = h2ptphi_bspinup->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        //std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2ptphi_bspinup->GetXaxis()->FindBin(phiShifted);
        for (int j = 1; j <= h2ptphi_bspinup->GetNbinsY(); j++) {
            h2ptphi_bspinup_pishift->SetBinContent(i, j, h2ptphi_bspinup->GetBinContent(phiBin, j));
            h2ptphi_bspinup_pishift->SetBinError(i, j, h2ptphi_bspinup->GetBinError(phiBin, j));
        } 
    }

    TH2D *h2ptphi_yspindown_pishift = (TH2D*)h2ptphi_yspindown->Clone("h2ptphi_yspindown_pishift");
    for (int i = 1; i <= h2ptphi_yspindown->GetNbinsX(); i++) {
        double phiCenter = h2ptphi_yspindown->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        //std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2ptphi_yspindown->GetXaxis()->FindBin(phiShifted);
        for (int j = 1; j <= h2ptphi_yspindown->GetNbinsY(); j++) {
            h2ptphi_yspindown_pishift->SetBinContent(i, j, h2ptphi_yspindown->GetBinContent(phiBin, j));
            h2ptphi_yspindown_pishift->SetBinError(i, j, h2ptphi_yspindown->GetBinError(phiBin, j));
        } 
    }

    TH2D *h2ptphi_yspinup_pishift = (TH2D*)h2ptphi_yspinup->Clone("h2ptphi_yspinup_pishift");
    for (int i = 1; i <= h2ptphi_yspinup->GetNbinsX(); i++) {
        double phiCenter = h2ptphi_yspinup->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        //std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2ptphi_yspinup->GetXaxis()->FindBin(phiShifted);
        for (int j = 1; j <= h2ptphi_yspinup->GetNbinsY(); j++) {
            h2ptphi_yspinup_pishift->SetBinContent(i, j, h2ptphi_yspinup->GetBinContent(phiBin, j));
            h2ptphi_yspinup_pishift->SetBinError(i, j, h2ptphi_yspinup->GetBinError(phiBin, j));
        } 
    }

    TH2D *h2ptphi_blue_rawAN = (TH2D*)h2ptphi_bspinup->Clone("h2ptphi_blue_rawAN");
    TH2D *h2ptphi_yellow_rawAN = (TH2D*)h2ptphi_yspinup->Clone("h2ptphi_yellow_rawAN");

    for (int i = 1; i <= h2ptphi_blue_rawAN->GetNbinsX(); i++) {
        for (int j = 1; j <= h2ptphi_blue_rawAN->GetNbinsY(); j++) {
            double Nup = h2ptphi_bspinup->GetBinContent(i,j);
            double Ndown = h2ptphi_bspindown->GetBinContent(i,j);
            double Nup_pishift = h2ptphi_bspinup_pishift->GetBinContent(i,j);
            double Ndown_pishift = h2ptphi_bspindown_pishift->GetBinContent(i,j);
            double num = std::sqrt(Nup*Ndown_pishift) - std::sqrt(Ndown*Nup_pishift);
            // Add in check to ensure counts aren't too small!!
            double denom = std::sqrt(Nup*Ndown_pishift) + std::sqrt(Ndown*Nup_pishift);
            double content = num/denom;
            h2ptphi_blue_rawAN->SetBinContent(i, j, content);
            // CHECK ASYM ERROR CALCULATION
            double err = std::sqrt(Nup * Ndown_pishift * Ndown * Nup_pishift) / std::pow(denom, 2);
            err *= std::sqrt(1./Nup + 1./Ndown + 1./Nup_pishift + 1./Ndown_pishift);
            h2ptphi_blue_rawAN->SetBinError(i, j, err);
           
        }
    }

    for (int i = 1; i <= h2ptphi_yellow_rawAN->GetNbinsX(); i++) {
        for (int j = 1; j <= h2ptphi_yellow_rawAN->GetNbinsY(); j++) {
            double Nup = h2ptphi_yspinup->GetBinContent(i,j);
            double Ndown = h2ptphi_yspindown->GetBinContent(i,j);
            double Nup_pishift = h2ptphi_yspinup_pishift->GetBinContent(i,j);
            double Ndown_pishift = h2ptphi_yspindown_pishift->GetBinContent(i,j);
            double num = std::sqrt(Nup*Ndown_pishift) - std::sqrt(Ndown*Nup_pishift);
            // Add in check to ensure counts aren't too small!!
            double denom = std::sqrt(Nup*Ndown_pishift) + std::sqrt(Ndown*Nup_pishift);
            double content = num/denom;
            h2ptphi_yellow_rawAN->SetBinContent(i, j, content);
            // CHECK ASYM ERROR CALCULATION
            double err = std::sqrt(Nup * Ndown_pishift * Ndown * Nup_pishift) / std::pow(denom, 2);
            err *= std::sqrt(1./Nup + 1./Ndown + 1./Nup_pishift + 1./Ndown_pishift);
            h2ptphi_yellow_rawAN->SetBinError(i, j, err);

        }
    }

    TH1D *h1phi_blue_ptslice_rawAN[nptbins];
    TH1D *h1phi_yellow_ptslice_rawAN[nptbins];


    TF1 *AN_blue_pt[nptbins];
    TF1 *AN_yellow_pt[nptbins];
    for (int i=0; i<nptbins; i++)
    {
        TString fitname_blue_pt = TString::Format("fit_blue_pt_%d", i);
        TString fitname_yellow_pt = TString::Format("fit_yellow_pt_%d", i);
        AN_blue_pt[i] = new TF1(fitname_blue_pt, "[0]*sin(x)", -3.14, 0);
        AN_yellow_pt[i] = new TF1(fitname_yellow_pt, "[0]*sin(x)", -3.14, 0);

        TString h1phiname_blue_ptslice = TString::Format("h1phi_blue_pt_%d", i);
        TString h1phiname_yellow_ptslice = TString::Format("h1phi_yellow_pt_%d", i);
        
        h1phi_blue_ptslice_rawAN[i] = h2ptphi_blue_rawAN->ProjectionX(h1phiname_blue_ptslice, i+1, i+1);
        h1phi_yellow_ptslice_rawAN[i] = h2ptphi_yellow_rawAN->ProjectionX(h1phiname_yellow_ptslice, i+1, i+1);

        h1phi_blue_ptslice_rawAN[i]->Fit(AN_blue_pt[i], "R");
        h1phi_yellow_ptslice_rawAN[i]->Fit(AN_yellow_pt[i], "R");


    }

    // For xF asymmetry
    TH2D *h2xfphi_bspindown_pishift = (TH2D*)h2xfphi_bspindown->Clone("h2xfphi_bspindown_pishift");
    for (int i = 1; i <= h2xfphi_bspindown->GetNbinsX(); i++) {
        double phiCenter = h2xfphi_bspindown->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2xfphi_bspindown->GetXaxis()->FindBin(phiShifted);
        std::cout << "phiBin : " << phiBin << std::endl;
        for (int j = 1; j <= h2xfphi_bspindown->GetNbinsY(); j++) {
            h2xfphi_bspindown_pishift->SetBinContent(i, j, h2xfphi_bspindown->GetBinContent(phiBin, j));
            h2xfphi_bspindown_pishift->SetBinError(i, j, h2xfphi_bspindown->GetBinError(phiBin, j));
        } 
    }

    TH2D *h2xfphi_bspinup_pishift = (TH2D*)h2xfphi_bspinup->Clone("h2xfphi_bspinup_pishift");
    for (int i = 1; i <= h2xfphi_bspinup->GetNbinsX(); i++) {
        double phiCenter = h2xfphi_bspinup->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        //std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2xfphi_bspinup->GetXaxis()->FindBin(phiShifted);
        for (int j = 1; j <= h2xfphi_bspinup->GetNbinsY(); j++) {
            h2xfphi_bspinup_pishift->SetBinContent(i, j, h2xfphi_bspinup->GetBinContent(phiBin, j));
            h2xfphi_bspinup_pishift->SetBinError(i, j, h2xfphi_bspinup->GetBinError(phiBin, j));
        } 
    }

    TH2D *h2xfphi_yspindown_pishift = (TH2D*)h2xfphi_yspindown->Clone("h2xfphi_yspindown_pishift");
    for (int i = 1; i <= h2xfphi_yspindown->GetNbinsX(); i++) {
        double phiCenter = h2xfphi_yspindown->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        //std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2xfphi_yspindown->GetXaxis()->FindBin(phiShifted);
        for (int j = 1; j <= h2xfphi_yspindown->GetNbinsY(); j++) {
            h2xfphi_yspindown_pishift->SetBinContent(i, j, h2xfphi_yspindown->GetBinContent(phiBin, j));
            h2xfphi_yspindown_pishift->SetBinError(i, j, h2xfphi_yspindown->GetBinError(phiBin, j));
        } 
    }

    TH2D *h2xfphi_yspinup_pishift = (TH2D*)h2xfphi_yspinup->Clone("h2xfphi_yspinup_pishift");
    for (int i = 1; i <= h2xfphi_yspinup->GetNbinsX(); i++) {
        double phiCenter = h2xfphi_yspinup->GetXaxis()->GetBinCenter(i);
        double phiShifted = phiCenter + 3.14 < 3.14 ? phiCenter + 3.14 : phiCenter - 3.14;
        //std::cout << "phiShifted : " << phiShifted << std::endl;
        int phiBin = h2xfphi_yspinup->GetXaxis()->FindBin(phiShifted);
        for (int j = 1; j <= h2xfphi_yspinup->GetNbinsY(); j++) {
            h2xfphi_yspinup_pishift->SetBinContent(i, j, h2xfphi_yspinup->GetBinContent(phiBin, j));
            h2xfphi_yspinup_pishift->SetBinError(i, j, h2xfphi_yspinup->GetBinError(phiBin, j));
        } 
    }
 
    TH2D *h2xfphi_blue_rawAN = (TH2D*)h2xfphi_bspinup->Clone("h2xfphi_blue_rawAN");
    TH2D *h2xfphi_yellow_rawAN = (TH2D*)h2xfphi_yspinup->Clone("h2xfphi_yellow_rawAN");

    for (int i = 1; i <= h2xfphi_blue_rawAN->GetNbinsX(); i++) {
        for (int j = 1; j <= h2xfphi_blue_rawAN->GetNbinsY(); j++) {
            double Nup = h2xfphi_bspinup->GetBinContent(i,j);
            double Ndown = h2xfphi_bspindown->GetBinContent(i,j);
            double Nup_pishift = h2xfphi_bspinup_pishift->GetBinContent(i,j);
            double Ndown_pishift = h2xfphi_bspindown_pishift->GetBinContent(i,j);
            double num = std::sqrt(Nup*Ndown_pishift) - std::sqrt(Ndown*Nup_pishift);
            // Add in check to ensure counts aren't too small!!
            double denom = std::sqrt(Nup*Ndown_pishift) + std::sqrt(Ndown*Nup_pishift);
            double content = num/denom;
            h2xfphi_blue_rawAN->SetBinContent(i, j, content);
            // CHECK ASYM ERROR CALCULATION
            double err = std::sqrt(Nup * Ndown_pishift * Ndown * Nup_pishift) / std::pow(denom, 2);
            err *= std::sqrt(1./Nup + 1./Ndown + 1./Nup_pishift + 1./Ndown_pishift);
            h2xfphi_blue_rawAN->SetBinError(i, j, err);
           
        }
    }

    for (int i = 1; i <= h2xfphi_yellow_rawAN->GetNbinsX(); i++) {
        for (int j = 1; j <= h2xfphi_yellow_rawAN->GetNbinsY(); j++) {
            double Nup = h2xfphi_yspinup->GetBinContent(i,j);
            double Ndown = h2xfphi_yspindown->GetBinContent(i,j);
            double Nup_pishift = h2xfphi_yspinup_pishift->GetBinContent(i,j);
            double Ndown_pishift = h2xfphi_yspindown_pishift->GetBinContent(i,j);
            double num = std::sqrt(Nup*Ndown_pishift) - std::sqrt(Ndown*Nup_pishift);
            // Add in check to ensure counts aren't too small!!
            double denom = std::sqrt(Nup*Ndown_pishift) + std::sqrt(Ndown*Nup_pishift);
            double content = num/denom;
            h2xfphi_yellow_rawAN->SetBinContent(i, j, content);
            // CHECK ASYM ERROR CALCULATION
            double err = std::sqrt(Nup * Ndown_pishift * Ndown * Nup_pishift) / std::pow(denom, 2);
            err *= std::sqrt(1./Nup + 1./Ndown + 1./Nup_pishift + 1./Ndown_pishift);
            h2xfphi_yellow_rawAN->SetBinError(i, j, err);

        }
    }

    TH1D *h1phi_blue_xfslice_rawAN[nxfbins];
    TH1D *h1phi_yellow_xfslice_rawAN[nxfbins];


    TF1 *AN_blue_xf[nxfbins];
    TF1 *AN_yellow_xf[nxfbins];
    for (int i=0; i<nxfbins; i++)
    {
        TString fitname_blue_xf = TString::Format("fit_blue_xf_%d", i);
        TString fitname_yellow_xf = TString::Format("fit_yellow_xf_%d", i);
        AN_blue_xf[i] = new TF1(fitname_blue_xf, "[0]*sin(x)", -3.14, 0);
        AN_yellow_xf[i] = new TF1(fitname_yellow_xf, "[0]*sin(x)", -3.14, 0);

        TString h1phiname_blue_xfslice = TString::Format("h1phi_blue_xf_%d", i);
        TString h1phiname_yellow_xfslice = TString::Format("h1phi_yellow_xf_%d", i);
        
        h1phi_blue_xfslice_rawAN[i] = h2xfphi_blue_rawAN->ProjectionX(h1phiname_blue_xfslice, i+1, i+1);
        h1phi_yellow_xfslice_rawAN[i] = h2xfphi_yellow_rawAN->ProjectionX(h1phiname_yellow_xfslice, i+1, i+1);

        h1phi_blue_xfslice_rawAN[i]->Fit(AN_blue_xf[i], "R");
        h1phi_yellow_xfslice_rawAN[i]->Fit(AN_yellow_xf[i], "R");


    }    

    outfile->Write();
}
