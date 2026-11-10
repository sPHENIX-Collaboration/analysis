int GetPtBin(float ptval)
{   
    int ptbin = int(ptval*2-1);
    return ptbin;
}

// int GetPhiBin(float phival)
// {   
//     float PI = 3.1415;
//     int phibin = int((phival+PI/2)*(8/PI));
//     return phibin;
// }

//[-pi,-5pi/6]: 0
//[-5pi/6,-4pi/6]: 1
//[-4pi/6,-3pi/6]: 2
//[-3pi/6,-2pi/6]: 3
//[-2pi/6,-pi/6]: 4
//[-pi/6,0]: 5
//[0,pi/6]: 6
//[pi/6,2pi/6]: 7
//[2pi/6,3pi/6]: 8
//[3pi/6,4pi/6]: 9
//[4pi/6,5pi/6]: 10
//[5pi/6,pi]: 11

int GetPhiBin(float phival)
{   
    float PI = 3.1415;
    int phibin = int((phival+PI)*(6/PI));
    return phibin;
}

float ConvertPhi(float phival)
{   
    float PI = 3.1415;
    float phi_convert;
    if (phival < -1*PI/2)
    {
        phi_convert = phival + 3*PI/2;
    }
    else
    {
        phi_convert = phival - PI/2;
    }
    return phi_convert;
}

float sqrtasym(float N1, float N2, float N3, float N4){
  float asym = (sqrt(N1*N4)-sqrt(N3*N2))/(sqrt(N1*N4)+sqrt(N3*N2));
  return asym;
}

float sqrtasymerr(float N1, float N2, float N3, float N4, float E1, float E2, float E3, float E4){
  float D1 = sqrt(N2*N3*N4/N1)*E1;
  float D2 = sqrt(N1*N3*N4/N2)*E2;
  float D3 = sqrt(N1*N2*N4/N3)*E3;
  float D4 = sqrt(N1*N2*N3/N4)*E4;
  float asym_err = (1/pow(sqrt(N1*N4)+sqrt(N3*N2),2))*sqrt(pow(D1,2)+pow(D2,2)+pow(D3,2)+pow(D4,2));
  return asym_err;
}


float polasym(float N1, float N2, float rellum = 1.0){
  float asym;
  if (rellum == 1){float asym = (N1-N2)/(N1+N2);}
  else{float asym = (N1-rellum*N2)/(N1+rellum*N2);}
  return asym;
}

float polasymerr(float N1, float N2, float E1, float E2,float rellum = 1.0){
  float asym_err;
  if (rellum == 1){float asym_err = (1/pow(N1+N2,2))*2*sqrt(pow(N2*E1,2)+pow(N1*E2,2));}
  else{float asym_err = (1/pow(N1+rellum*N2,2))*2*rellum*sqrt(pow(N2*E1,2)+pow(N1*E2,2));}
  return asym_err;
}

void aN()
{
    float PI = 3.1415;

    int pdg; float mass; float pt; float pz; float xf;
    float e; float eta; float phi; float chi2; int ndf;
    float decaylength; float decaylengthxy;
    int bluespin; int yellspin; int candidate_crossing;
    float bluepolarization; float yellowpolarization;

    TFile *infile = new TFile("/sphenix/tg/tg01/coldqcd/dloomis/kfp_singlespinasymmetries/pipi/outputKFPSSA_pipi_reco_00053877.root");
    TTree *t = (TTree*)infile->Get("Ks_Tree");
    t->SetBranchAddress("pdg",&pdg);
    t->SetBranchAddress("mass",&mass);
    t->SetBranchAddress("pt",&pt);
    t->SetBranchAddress("pz",&pz);
    t->SetBranchAddress("xf",&xf);
    t->SetBranchAddress("e",&e);
    t->SetBranchAddress("eta",&eta);
    t->SetBranchAddress("phi",&phi);
    t->SetBranchAddress("chi2",&chi2);
    t->SetBranchAddress("ndf",&ndf);
    t->SetBranchAddress("decaylength",&decaylength);
    t->SetBranchAddress("decaylengthxy",&decaylengthxy);
    t->SetBranchAddress("candidate_crossing",&candidate_crossing);
    t->SetBranchAddress("bluespin",&bluespin);
    t->SetBranchAddress("yellspin",&yellspin);
    t->SetBranchAddress("bluepol",&bluepolarization);
    t->SetBranchAddress("yellpol",&yellowpolarization);

    int nentries = t->GetEntries();
    std::cout << nentries << std::endl;

    TH1D *h[2][2][5][12]; //blue/yellow, up/down, ptbins, phibins

    std::string beams[2] = {"blue","yellow"};
    std::string spins[2] = {"up","down"};

    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        for (int k = 0; k < 5; k++)
        {
          for (int l = 0; l < 12; l++)
          {
            h[i][j][k][l] = new TH1D(Form("h_%s_%s_%d_%d",beams[i].c_str(),spins[j].c_str(),k,l),Form("h_%s_%s_%d_%d",beams[i].c_str(),spins[j].c_str(),k,l),50,0.4,0.6);

          }
        }
      } 
    }

    // nentries = 100;
    for (int i = 0; i < nentries; i++)
    {
        if (i%100000 == 0){std::cout << "Candidate: " << i << std::endl;}
        t->GetEntry(i);
        if (pt < 0.5 || pt > 3){continue;}
        float bluephi = ConvertPhi(phi);
        float yellphi = -1*bluephi;

        // std::cout << bluespin << " " << phi << " " << bluephi << " " << GetPhiBin(bluephi) << std::endl;
        if (bluespin == 1)
        {
            h[0][0][GetPtBin(pt)][GetPhiBin(bluephi)]->Fill(mass);
        }
        else if (bluespin == -1)
        {
            h[0][1][GetPtBin(pt)][GetPhiBin(bluephi)]->Fill(mass);
        }

        if (yellspin == 1)
        {
            h[1][0][GetPtBin(pt)][GetPhiBin(yellphi)]->Fill(mass);
        }
        else if (yellspin == -1)
        {
            h[1][1][GetPtBin(pt)][GetPhiBin(yellphi)]->Fill(mass);
        }
    }


    TGraphErrors *g_rawasym[2][5]; // blue/yellow, ptbins

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            g_rawasym[i][j] = new TGraphErrors();
            g_rawasym[i][j]->SetTitle(Form("grawasym_%s_%d",beams[i].c_str(),j));
            g_rawasym[i][j]->SetName(Form("grawasym_%s_%d",beams[i].c_str(),j));
            for (int k = 3; k < 9; k++) // will go over -pi/2 to pi/2 which means 3 to 9 in phi indices
            {
                float phival = (-1*PI + PI/12)+k*(PI/6);
                float N1 = h[i][0][j][k%12]->Integral(h[i][0][j][k%12]->FindBin(0.47),h[i][0][j][k%12]->FindBin(0.51)); // This is Nup(phi)
                float N2 = h[i][1][j][k%12]->Integral(h[i][1][j][k%12]->FindBin(0.47),h[i][1][j][k%12]->FindBin(0.51)); // This is Ndown(phi)
                float N3 = h[i][0][j][(k+6)%12]->Integral(h[i][0][j][(k+6)%12]->FindBin(0.47),h[i][0][j][(k+6)%12]->FindBin(0.51)); // This is Nup(phi+pi)
                float N4 = h[i][1][j][(k+6)%12]->Integral(h[i][1][j][(k+6)%12]->FindBin(0.47),h[i][1][j][(k+6)%12]->FindBin(0.51)); // This is Ndown(phi+pi)
                float E1 = sqrt(N1);
                float E2 = sqrt(N2);
                float E3 = sqrt(N3);
                float E4 = sqrt(N4);
                float asym = sqrtasym(N1,N2,N3,N4);
                float asymerr = sqrtasymerr(N1,N2,N3,N4,E1,E2,E3,E4);
                g_rawasym[i][j]->SetPoint(g_rawasym[i][j]->GetN(),phival,asym);
                g_rawasym[i][j]->SetPointError(g_rawasym[i][j]->GetN()-1,0,asymerr);
            }
        }
    }

    // fit raw asymmetries 
    TF1 *fit_rawasym[2][5];
    TGraphErrors *gsignalan[2];
    
    for (int i = 0; i < 2; i++)
    {
        gsignalan[i] = new TGraphErrors();
        gsignalan[i]->SetTitle(Form("gsignalan_%s",beams[i].c_str()));
        gsignalan[i]->SetName(Form("gsignalan_%s",beams[i].c_str()));
        for (int j = 0; j < 5; j++)
        {
            float ptval = 0.75+j*0.5;
            fit_rawasym[i][j] = new TF1(Form("fit_rawasym_%d_%d",i,j),"-[0]*sin(x)",-1*PI/2,PI/2);
            g_rawasym[i][j]->Fit(fit_rawasym[i][j],"MR");
            gsignalan[i]->SetPoint(gsignalan[i]->GetN(),ptval,fit_rawasym[i][j]->GetParameter(0));
            gsignalan[i]->SetPointError(gsignalan[i]->GetN()-1,0,fit_rawasym[i][j]->GetParError(0));
        }
    }

    TGraphErrors *gsignalan_combined = new TGraphErrors();
    gsignalan_combined->SetTitle("gsignalan_combined");
    gsignalan_combined->SetName("gsignalan_combined");
    for (int i = 0; i < 5; i++)
    {
        float ptval = 0.75+i*0.5;
        double y1 = gsignalan[0]->GetY()[i] / pow(gsignalan[0]->GetErrorY(i), 2) +  gsignalan[1]->GetY()[i] / pow(gsignalan[1]->GetErrorY(i), 2);
        double y2 = 1 / pow(gsignalan[0]->GetErrorY(i), 2) + 1 / pow (gsignalan[1]->GetErrorY(i), 2);
        float combasym = y1/y2;
        float combasymerr = sqrt(1/(1 / pow(gsignalan[0]->GetErrorY(i), 2) + 1 / pow(gsignalan[1]->GetErrorY(i), 2)));
        gsignalan_combined->SetPoint(gsignalan_combined->GetN(),ptval,combasym);
        gsignalan_combined->SetPointError(gsignalan_combined->GetN()-1,0,combasymerr);
    }
    


    TFile *outfile = new TFile("asym_hists.root","RECREATE");
    gsignalan_combined->Write();
    for (int i = 0; i < 2; i++)
    {
      gsignalan[i]->Write();
      for (int j = 0; j < 5; j++)
      {
        g_rawasym[i][j]->Write();
      } 
    }
    
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        for (int k = 0; k < 5; k++)
        {
          for (int l = 0; l < 12; l++)
          {
            h[i][j][k][l]->Write();

          }
        }
      } 
    }
    outfile->Write();
    outfile->Close();

}

