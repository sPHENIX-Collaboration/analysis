/*
   Auther: Chong Kim (ckim.phenix@gmail.com)

   - Target: study resolution of 1st/2nd leading jets reconstructed in fsPHENIX calorimeter
   - Environments:
     a. Requires root file contains reconstructed forward jets, which is results of
	 	/macros/macros/g4simulations/Fun4All_G4_fsPHENIX.C
		* modified Fun4All_G4_fsPHENIX.C used for this code can be found at:
		/gpfs/mnt/gpfs04/sphenix/user/ckim/fjets/Fun4All_G4_fsPHENIX.C
	 b. Uses root branch "ntp_truthjet"

   - How code works:
   	 a. List up 1st/2nd leading jets using true pT (gpt) which satisfying min pT cut (cutPT)
	 	(function searchLeadingJets)
	 b. Study target variable (ex. energy) using collected 1st/2nd leading jets in each event
	 	For now only energy is checked but any variable in "ntp_truthjet" can be studied
	 c. Loop through multiple events and root files

   - Parameters (ex. Pythia version, beam energy, Anti_kT radius, etc) can be arranged via arrays in top
   - Tested by using output files in "/gpfs/mnt/gpfs04/sphenix/user/ckim/fjets/Ana/results_1121" at Mar.2, 2017
*/

#include <TCanvas.h>
#include <TFile.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TNtuple.h>
#include <TString.h>
#include <TStyle.h>
#include <iostream>
using namespace std;

std::vector<int> searchLeadingJets(TNtuple *T, float cutPT, const char *targetV = "gpt");

//=========================================================
void fjetResol(
		const char* path   = "/gpfs/mnt/gpfs04/sphenix/user/ckim/fjets/Ana/results_1121",
		const char* var    = "e",
		const char* suffix = "",
		bool print = true
		)
{

	//Parameters
	const int   setPy[]    = {8/*, 6*/}; //Pythia8 or 6
	const int   setBeamE[] = {200/*, 510*/};
	const int   setR[]     = {4, 6}; //Anti-kT radius
	const int   setMinPT[] = {/*3, 4,*/ 5, 10, 15}; //Minimum parton pT in pythia cfg
	const float setEta[]   = {1.2, 1.6, 2.0, 2.4, 2.8, 3.2, 3.6};
	const float setE[]     = {20,25,30,35,40,45,50, 60,70,80,90,100,110,120,130,140,150,160, 180,200}; //True jet E

	const int nsetPy    = sizeof(setPy)   /sizeof(setPy[0]);
	const int nsetBeamE = sizeof(setBeamE)/sizeof(setBeamE[0]);
	const int nsetR     = sizeof(setR)    /sizeof(setR[0]);
	const int nsetMinPT = sizeof(setMinPT)/sizeof(setMinPT[0]);
	const int nsetEta   = sizeof(setEta)  /sizeof(setEta[0]) - 1;
	const int nsetE     = sizeof(setE)    /sizeof(setE[0]) - 1;

	//----------------------------------------------

	//Histograms to be filled
	TH2F *h2[nsetPy][nsetBeamE][nsetR][nsetEta];
	TH1F *h1[nsetPy][nsetBeamE][nsetR][nsetEta][nsetE];
	for (int a=0; a<nsetPy;    a++)
	for (int b=0; b<nsetBeamE; b++)
	for (int c=0; c<nsetR;     c++)
	for (int x=0; x<nsetEta;   x++)
	{
		const char* h2Name  = Form("sanity_py%i_%i_r0%i_eta%i", setPy[a],setBeamE[b],setR[c],x);
		h2[a][b][c][x] = new TH2F(h2Name, "", 100,0,b==0?100:200, 60,0,2);
		for (int y=0; y<nsetE; y++)
		{
			const char* h1Name = Form("h1_%s_py%i_%i_r0%i_eta%i_e%i", var,setPy[a],setBeamE[b],setR[c],x,y);
			h1[a][b][c][x][y] = new TH1F(h1Name, "", 150,-1.5,1.5);
		}
	}

	//----------------------------------------------

	//Iterate through each file and tree
	for (int a=0; a<nsetPy;    a++)
	for (int b=0; b<nsetBeamE; b++)
	for (int c=0; c<nsetR;     c++)
	for (int d=0; d<nsetMinPT; d++)
	{
		//Link input
		const char *finName = Form("%s/pythia%i_%i_r0%i_pT%i.root", path,setPy[a],setBeamE[b],setR[c],setMinPT[d]);
		TFile   *F = TFile::Open(finName);
		TNtuple *T = (TNtuple*)F->Get("ntp_truthjet");
		cout <<Form("Open %s from %s...", T->GetName(), F->GetName()) <<endl;

		//Search 1st/2nd leading jets in this file
		std::vector<int> jetList = searchLeadingJets(T, (float)setMinPT[d]);
		//for (unsigned int i=0; i<jetList.size(); i++) cout <<i <<" " <<jetList[i] <<endl;

		//Link branches
		float event, geta, gphi, ge, eta, phi, e;
		T->SetBranchAddress("event", &event);
		T->SetBranchAddress("geta",  &geta);
		T->SetBranchAddress("gphi",  &gphi);
		T->SetBranchAddress("ge",    &ge);
		T->SetBranchAddress("eta",   &eta);
		T->SetBranchAddress("phi",   &phi);
		T->SetBranchAddress("e",     &e);

		//Iterate through for leading jets found
		for (unsigned int i=0; i<jetList.size(); i++)
		{
			T->GetEntry(jetList[i]);

			int id_eta = -1;
			for (int x=0; x<nsetEta; x++) { if (geta > setEta[x] && geta < setEta[x+1]) id_eta = x;	}
			if (id_eta == -1) continue;

			h2[a][b][c][id_eta]->Fill(ge, e/ge); //Sanity plots

			int id_e = -1;
			for (int y=0; y<nsetE; y++) { if (ge > setE[y] && ge < setE[y+1]) id_e = y;	}
			if (id_e == -1) continue;

			if (!strcmp(var, "e")) h1[a][b][c][id_eta][id_e]->Fill(e/ge);
		}//i, leading jets

		//Cleanup
		T->ResetBranchAddresses();
		T->Delete();
		F->Delete();
	}//d, c, b, a

	//----------------------------------------------

    gStyle->SetOptStat("e");

	//Sanity check plots
	TCanvas *c1[nsetPy][nsetBeamE][nsetR];
	for (int a=0; a<nsetPy;    a++)
	for (int b=0; b<nsetBeamE; b++)
	for (int c=0; c<nsetR;     c++)
	{
		const char* c1Name = Form("sanity_py%i_%i_r0%i", setPy[a],setBeamE[b],setR[c]);
		c1[a][b][c] = new TCanvas(c1Name, c1Name, 1280, 800);
		c1[a][b][c]->Divide(nsetEta/2, 2, 0.015, 0.015, 10);
		for (int x=0; x<nsetEta; x++)
		{
			c1[a][b][c]->cd(x+1)->SetGrid();
            h2[a][b][c][x]->SetTitle(Form("%2.1f < #eta < %2.1f;True e;Reco e/True e", setEta[x], setEta[x+1]));
            h2[a][b][c][x]->GetYaxis()->SetTitleOffset(1.45);
            h2[a][b][c][x]->Draw("colz");
		}
        if (print) c1[a][b][c]->Print(Form("%s%s.png", c1[a][b][c]->GetName(),suffix)); 
	}//c, b, a

	//Get resolutions
	TCanvas *c2 = new TCanvas(Form("resol_%s", var), var, 1280, 800);
	c2->Divide(nsetEta/2, 2, 0.015, 0.015, 10);
	TF1 *f1Gaus = new TF1("f1Gaus", "[0]*TMath::Gaus(x, [1], [2])", -0.2, 0.2);
	TLegend *leg1 = new TLegend(0.4, 0.6/*0.9 - 0.15*nsetPy*/, 0.9, 0.9);
	for (int a=0; a<nsetPy;    a++)
	for (int b=0; b<nsetBeamE; b++)
	for (int c=0; c<nsetR;     c++)
	for (int x=0; x<nsetEta;   x++)
	{
		TGraphErrors *g1 = new TGraphErrors();
		g1->SetName(Form("g1_%s_py%i_%i_r0%i_eta%i_e%i", var,setPy[a],setBeamE[b],setR[c],x));
		g1->SetLineColor(2 * (b+1));
		g1->SetMarkerColor(2 * (b+1));
		g1->SetMarkerSize(1.);
		g1->SetMarkerStyle(20 + 4*c);

		for (int y=0; y<nsetE; y++)
		{
			if (h1[a][b][c][x][y]->GetEntries() < 50)
			{
				h1[a][b][c][x][y]->Delete();
				continue;
			}
			f1Gaus->SetParameters(
					h1[a][b][c][x][y]->GetMaximum(),
					h1[a][b][c][x][y]->GetMean(),
					h1[a][b][c][x][y]->GetRMS()
					);
			h1[a][b][c][x][y]->Fit(
					f1Gaus->GetName(), "QR0", "",
					h1[a][b][c][x][y]->GetMean() - h1[a][b][c][x][y]->GetRMS() * 3,
					h1[a][b][c][x][y]->GetMean() + h1[a][b][c][x][y]->GetRMS() * 3
					);
			g1->SetPoint(g1->GetN(), (setE[y]+setE[y+1])/2, f1Gaus->GetParameter(2));
			g1->SetPointError(g1->GetN()-1, 0, f1Gaus->GetParError(2));
		}//y (energy)

		c2->cd(x+1)->SetGrid();
		if (a==0 && b==0 && c==0)
		{
			const char *yTitle = "#sigma (Reco e / True e)";
			const char *gTitle = Form("%2.1f < #eta < %2.1f;True e;%s", setEta[x],setEta[x+1], yTitle);
			gPad->DrawFrame(20-5, 0.05, 200+5, 0.3, gTitle);
			gPad->Modified();
		}
		if (a==0 && x==0)
		{
			leg1->AddEntry(g1, Form("PYTHIA%i, #sqrt{s} = %i, R = 0.%i", setPy[a], setBeamE[b], setR[c]), "p");
		}
		g1->Draw("pe same");
		if (a==(nsetPy-1) && b==(nsetBeamE-1) && c==(nsetR-1) && x==0) leg1->Draw("same");
	}//x (eta), c, b, a
	if (print) c2->Print(Form("%s%s.png", c2->GetName(),suffix)); 

	return;
}//Main

//==============================================================================
std::vector<int> searchLeadingJets(TNtuple *T, float cutPT, const char *targetV)
{
	std::vector<int> jetList; //Return (event # of target jets)

	float event, gpt, var;
	T->SetBranchAddress("event", &event);
	if (strcmp("gpt", targetV))
	{
		T->SetBranchAddress("gpt", &gpt);
		cout <<"Search leading jets by using " <<targetV <<endl;
	}
	T->SetBranchAddress(targetV, &var); //gpt or ge (truth info)

	//Note: high1_v should always higher than high2_v
	int evtCheck = -1;
	int high1_n  = -1; //1st leading jet's entry # in the tree
	int high2_n  = -1;
	float high1_v = -1; //1st leading jet's value
	float high2_v = -2;

	const int allEntries = T->GetEntries();
	for (int a=0; a<allEntries; a++)
	{
		/*
		   Iterate through all entries with same event number,
		   find 1st and 2nd leading jets of the event,
		   then save them when next event (different event #) found
		*/

		T->GetEntry(a);

		//pT cut
		float tempPT = (strcmp("gpt", targetV))?gpt:var;
		if (tempPT < cutPT) continue;

        if (evtCheck != event || a == allEntries) //Save jet entries
        {
            if (high1_n != -1 && high2_n != -1)
            {
                if (high1_n < high2_n)
                {
                    jetList.push_back(high1_n);
                    jetList.push_back(high2_n);
                }
                else
                {
                    jetList.push_back(high2_n);
                    jetList.push_back(high1_n);
                }
            }

            //Update dummy indices for next iteration
            evtCheck = event;
            high1_n = a;
			high2_n = -1;
            high1_v = var;
			high2_v = -1;
        }
        else //Iterate & Update
        {
			if (var > high1_v)
			{
				high2_n = high1_n;
				high2_v = high1_v;
				high1_n = a;
				high1_v = var;
			}
			else if (var > high2_v)
			{
				high2_n = a;
				high2_v = var;
			}
        }
	}//a, nEvents

	T->ResetBranchAddresses();
	return jetList;
}//searchLeadingJets
