

void upsilon_raa_sphenix_estimator() {

  //gROOT->LoadMacro("sPHENIXStyle/sPhenixStyle.C");
  //SetsPhenixStyle();

gStyle->SetOptStat(0);
gStyle->SetOptTitle(0);
gStyle->SetOptFit(0);

 bool yrs13 = true;
 bool yrs15 = true;
 bool ups3s = true;

 double offset = 0.0;
 if(yrs13 && yrs15)
   offset = 3;

 // Input assumptions about level of suppression are taken from the theory paper: 
  // Michael Strickland, Dennis Bazow, Nucl.Phys. A879 (2012) 25-58
  //==============================================
  // Read in Strickland curves for different eta/s values
  // These are exclusive - i.e. they are the 
  // suppression of that state with no feed-down
  // effects included 
  //==============================================
  // The curves vs eta/s read in below were provided by Strickland privately, since
  // the paper contains only the eta/s = 1 cases for RHIC energy

  // Get the exclusive raa values for the Y1S, Y2S, Y3S, chib1, chib2
  // Checked that this is being read in correctly
  double str_npart[101];
  double str_raa[5][3][101];
  for(int istate=0;istate<5;istate++)
    {
      for(int ietas=0;ietas<3;ietas++)
	{
	  if(istate < 3)
	    {
	      char fname[500];
	      sprintf(fname,"./strickland_calculations/Y%is-potb-eta%i-npart.dat",istate+1,ietas+1);
	      ifstream fin(fname);
	      
	      for(int inpart=0;inpart<101;inpart++)
		{
		  fin >> str_npart[inpart] >> str_raa[istate][ietas][inpart];
		  //cout << istate << " " << ietas << " " << inpart << " " << str_npart[inpart] << " " << str_raa[istate][ietas][inpart] << endl;
		}
	      fin.close();	  
	    }

	  if(istate > 2)
	    {
	      char fname[500];
	      sprintf(fname,"./strickland_calculations/chib%i-potb-eta%i-npart.dat",istate-2,ietas+1);
	      ifstream fin(fname);
	      
	      for(int inpart=0;inpart<101;inpart++)
		{
		  fin >> str_npart[inpart] >> str_raa[istate][ietas][inpart];
		  //cout << istate << " " << ietas << " " << inpart << " " << str_npart[inpart] << " " << str_raa[istate][ietas][inpart] << endl;
		}
	      fin.close();	  
	    }
	}

    }

  // Now construct the inclusive RAA values from Strickland's exclusive modifications
  // ff's from table II of the paper for the 1S state are as follows
  // They add up to 1.0
  double ff1S[5] = {0.51, 0.107, 0.008, 0.27, 0.105};  // Y1S, Y2S, Y3S, chib1, chib2 respectively
  // These are from arXiv:1210.7512
  double ff2S[2] = {0.5, 0.5};                         // Y2S and Y3S respectively

  double str_raa_inclusive[3][3][101];

  // checked the Y1S inclusive result against figure 10 of arXiv:1112.2761
  // There is no plot available for the 2S and 3S 
  for(int ietas=0;ietas<3;ietas++)
    for(int inpart=0;inpart<101;inpart++)
      {
	str_raa_inclusive[0][ietas][inpart] = 
	  str_raa[0][ietas][inpart] * ff1S[0]
	  + str_raa[1][ietas][inpart] * ff1S[1]
	  + str_raa[2][ietas][inpart] * ff1S[2]
	  + str_raa[3][ietas][inpart] * ff1S[3]
	  + str_raa[4][ietas][inpart] * ff1S[4];

	str_raa_inclusive[1][ietas][inpart] = 
	  str_raa[1][ietas][inpart] * ff2S[0]
	  + str_raa[2][ietas][inpart] * ff2S[1];

	str_raa_inclusive[2][ietas][inpart] = str_raa[2][ietas][inpart];
      }

  TGraph *grth[3][3];
  for(int is = 0;is<3;is++)
    {
      for(int ieta =0; ieta < 3; ++ieta)
	{
	  grth[is][ieta]=new TGraph(101,str_npart,str_raa_inclusive[is][ieta]);
	}
    }


 static const int NCENT = 7;
 // static const int NCENT3S = 4;
 static const int NCENT3S = 3;

 double Ncoll[NCENT] = {1067, 858, 610, 378, 224, 94.2, 14.5};
 double Npart[NCENT] = {351, 302, 236, 168, 116, 61.6, 14.4};
 double Npart_plot[2][NCENT];
 for(int i=0;i<NCENT;++i)
   {
     Npart_plot[0][i] = Npart[i] + offset;
     Npart_plot[1][i] = Npart[i] - offset; 
   }

 // separate binning for Y(3S)
 //double Npart3S[NCENT3S] = {281, 142, 62, 14.4};
 double Npart3S[NCENT3S] = {243, 80.0, 14.4};
 double Npart3S_plot[2][NCENT3S];
 for(int i=0;i<NCENT3S;++i)
   {
     Npart3S_plot[0][i] = Npart3S[i] + offset;
     Npart3S_plot[1][i] = Npart3S[i] - offset; 
   }

 
 double raa1[2][NCENT]; 
 double raa2[2][NCENT]; 
 for(int iset=0;iset<2;++iset) 
   for(int i=0; i<NCENT; ++i)
     {
       raa1[iset][i] = grth[0][1]->Eval(Npart_plot[iset][i]);
       raa2[iset][i] = grth[1][1]->Eval(Npart_plot[iset][i]);
       cout << " icent " << i << " Npart " << Npart_plot[iset][i] << " raa1 " << raa1[iset][i] << " raa2 " << raa2[iset][i] << endl; 
  }

 double raa3[2][NCENT3S];
 for(int iset=0;iset<2;++iset) 
   for(int i=0; i<NCENT3S; ++i)
     {
       raa3[iset][i] = grth[2][1]->Eval(Npart3S_plot[iset][i]);
       cout << " 3S:  icent " << i << " Npart " << Npart3S_plot[iset][i] << " raa1 " << " raa3 " << raa3[iset][i] << endl; 
  }

 // output the RAA values to feed back into the previous macro
 cout << " double raa1[" << NCENT << "] = {";
 for(int i=0; i< NCENT; ++i)
   {
     if(i != NCENT-1)
       cout << raa1[0][i] << ", ";
     else
       cout << raa1[0][i] << "}; ";
   } 
 cout << endl;

 // output the RAA values to feed back into the previous macro
 cout << " double raa2[" << NCENT << "] = {";
 for(int i=0; i< NCENT; ++i)
   {
     if(i != NCENT-1)
       cout << raa2[0][i] << ", ";
     else
       cout << raa2[0][i] << "}; ";
   } 
 cout << endl;

 // output the RAA values to feed back into the previous macro
 cout << " double raa3[" << NCENT3S << "] = {";
 for(int i=0; i< NCENT3S; ++i)
   {
     if(i != NCENT3S-1)
       cout << raa3[0][i] << ", ";
     else
       cout << raa3[0][i] << "}; ";
   } 
 cout << endl;

 // These errors are the only thing that change with different luminosity
 // updated 8/26/20
 double yrs13_erraa1[NCENT] = {0.021, 0.0227, 0.0211, 0.0251, 0.0315, 0.0365, 0.0665};
 double yrs13_erraa2[NCENT] = {0.0327, 0.0424, 0.0335, 0.0401, 0.0537, 0.0662, 0.157};
 double yrs13_erraa3[NCENT3S] = {0.042 ,0.094, 0.346};
 
 //double  yrs15_erraa1[NCENT] = {0.012, 0.013, 0.012, 0.015, 0.019, 0.022, 0.042};
 double  yrs15_erraa1[NCENT] = {0.0136, 0.0147, 0.0137, 0.0163, 0.0203, 0.0235, 0.0435};
 //double yrs15_erraa2[NCENT] = {0.022, 0.024, 0.021, 0.025, 0.034, 0.041, 0.097};
 double yrs15_erraa2[NCENT] = {0.0235, 0.0233, 0.0207, 0.0277, 0.0364, 0.0424, 0.103};
 double yrs15_erraa3[NCENT3S] = {0.034, 0.0547,  0.208};

 double erraa1[2][NCENT];
 double erraa2[2][NCENT];
 for(int i=0; i<NCENT;++i)
   {
     erraa1[0][i] = yrs13_erraa1[i];
     erraa2[0][i] = yrs13_erraa2[i];
     erraa1[1][i] = yrs15_erraa1[i];
     erraa2[1][i] = yrs15_erraa2[i];
   }
 double erraa3[2][NCENT3S];
 for(int i=0; i<NCENT3S;++i)
   {
     erraa3[0][i] = yrs13_erraa3[i];
     erraa3[1][i] = yrs15_erraa3[i];
   }


 TCanvas *craa = new TCanvas("craa","craa",0,0,1000,800);
 TH1D *h = new TH1D("h","h",100,-10,400);
 h->GetXaxis()->SetTitle("N_{part}");
 h->GetYaxis()->SetTitle("R_{AA}");
 h->SetMaximum(1.2);
 h->SetMinimum(0);

 h->Draw();

 TGraphErrors *gr1[2];
 TGraphErrors *gr2[2];
 TGraphErrors *gr3[2];
 for(int i=0; i< 2;i++)
   {
     gr1[i] = new TGraphErrors(NCENT,Npart_plot[i], raa1[i], 0, erraa1[i]);
     gr1[i]->SetMarkerStyle(20);
     gr1[i]->SetMarkerSize(1.5);

     gr2[i] = new TGraphErrors(NCENT,Npart_plot[i], raa2[i], 0, erraa2[i]);
     gr2[i]->SetMarkerStyle(21);
     gr2[i]->SetMarkerSize(1.5);
     gr2[i]->SetMarkerColor(kRed);
     gr2[i]->SetLineColor(kRed);

     gr3[i] = new TGraphErrors(NCENT3S,Npart3S_plot[i], raa3[i], 0, erraa3[i]);
     gr3[i]->SetMarkerStyle(20);
     gr3[i]->SetMarkerSize(1.5);
     gr3[i]->SetMarkerColor(kBlue);
     gr3[i]->SetLineColor(kBlue);
   }

 if(yrs13)
   {
     gr1[0]->Draw("p");
     gr2[0]->Draw("p");
     if(ups3s) gr3[0]->Draw("p");
   }

 if(yrs15)
   {
     if(yrs13 && yrs15)
     { 
       gr1[0]->SetMarkerStyle(24);
       gr2[0]->SetMarkerStyle(25);
       gr3[0]->SetMarkerStyle(25);
     }
     gr1[1]->Draw("p");
     gr2[1]->Draw("p");
     if(ups3s) gr3[1]->Draw("p");
   }


 TLegend *lups = new TLegend(0.38, 0.72, 0.72, 0.87,"#bf{#it{sPHENIX}} Projection");
 lups->SetBorderSize(0);
 lups->SetFillColor(0);
 lups->SetFillStyle(0);
 if(yrs13 && yrs15)
   {
     lups->AddEntry(gr1[1],"Y(1S)","p");
     lups->AddEntry(gr2[1],"Y(2S)","p");
     if(ups3s) lups->AddEntry(gr3[1],"Y(3S)","p");
   }
 else 
   {
     lups->AddEntry(gr1[0],"Y(1S)","p");
     lups->AddEntry(gr2[0],"Y(2S)","p");
     if(ups3s) lups->AddEntry(gr3[0],"Y(3S)","p");
   }
 lups->Draw();
 
 TLatex *lat1[2];
 lat1[0] = new TLatex(0.66, 0.833,"#font[42]{Au+Au, Years 1-3}");
 lat1[0]->SetTextSize(0.038);
 lat1[0]->SetNDC(1);
 lat1[1] = new TLatex(0.66, 0.833,"#font[42]{Au+Au, Years 1-5}");
 lat1[1]->SetTextSize(0.038);
 lat1[1]->SetNDC(1);

 if(yrs13 && !yrs15)
   lat1[0]->Draw();
 if(yrs15 || (yrs13 && yrs15))
   lat1[1]->Draw();

 TLatex *lat2[2];
 //lat2[0] = new TLatex(0.66, 0.763,"#font[42]{#splitline{62 pb^{-1} trig. p+p}{142B rec. Au+Au}}");
 lat2[0] = new TLatex(0.65, 0.763,"#font[42]{#splitline{62 pb^{-1} trig. p+p}{21 nb^{-1} rec. Au+Au}}");
 lat2[0]->SetTextSize(0.038);
 lat2[0]->SetNDC(1);
 //lat2[1] = new TLatex(0.66, 0.763,"#font[42]{#splitline{142 pb^{-1} trig. p+p}{348B rec. Au+Au}}");
 lat2[1] = new TLatex(0.65, 0.763,"#font[42]{#splitline{142 pb^{-1} trig. p+p}{51 nb^{-1} rec. Au+Au}}");
 lat2[1]->SetTextSize(0.038);
 lat2[1]->SetNDC(1);

 if(yrs13 && !yrs15)
   lat2[0]->Draw();

 if(yrs15 || (yrs13 && yrs15))
   lat2[1]->Draw();

 TLatex *lat4 = new TLatex(0.57,0.625,"#font[42]{#it{open markers: Years 1-3}}");
 lat4->SetTextSize(0.038);
 lat4->SetNDC(1);
 if(yrs15 || (yrs13 && yrs15))
   lat4->Draw();
  
  // Plot the theory curves also
  double col[3] = {kBlack, kRed, kBlue};
  int lstyle[3] = {kDotted, kSolid, kDashDotted};
  int nstates = 2;
  if(ups3s) nstates = 3;
  for(int is = 0;is<nstates;is++)
    {
      for(int ieta =0; ieta < 3; ++ieta)
      {
	grth[is][ieta]->SetLineColor(col[is]);
	grth[is][ieta]->SetMarkerSize(0.5);
	grth[is][ieta]->SetMarkerColor(col[is]);
	grth[is][ieta]->SetMarkerStyle(20);
	grth[is][ieta]->SetLineStyle(lstyle[ieta]);
	grth[is][ieta]->SetLineWidth(2);
	grth[is][ieta]->Draw("l");
      }
    }

  TLatex *lat3 = new TLatex(0.4, 0.675,"#font[42]{Strickland,Bazow, N.P. A879 (2012) 25}");
  lat3->SetTextSize(0.038);
  lat3->SetNDC(1);
  lat3->Draw();

  TLegend *letas = new TLegend(0.14, 0.14, 0.31, 0.28,"");
  letas->SetBorderSize(0);
  letas->SetFillColor(0);
  letas->SetFillStyle(0);
  letas->AddEntry(grth[0][0], "4#pi#eta/S=1","l");
  letas->AddEntry(grth[0][1], "4#pi#eta/S=2","l");
  letas->AddEntry(grth[0][2], "4#pi#eta/S=3","l");
  letas->Draw();
}



