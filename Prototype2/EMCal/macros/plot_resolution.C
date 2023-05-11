static const int N_ENERGIES = 8;
static const int N_CUTLEVELS = 8;
#include "load_files.C"
void plot_resolution() {

    gStyle->SetOptFit(1111);
    gStyle->SetOptTitle(1);
    gStyle->SetStatW(0.3);
    
    int scan = 2;
    int location = 1;
    int energies[N_ENERGIES];
    
    string filenames[N_ENERGIES];
    
    load_files(scan,location,energies,filenames);

    TFile *fin[N_ENERGIES];
    TTree *trees[N_ENERGIES];

    float Etotal_t, Emax_t, E3by3_t, E5by5_t, C1_t,  C2_inner_t,  C2_outer_t, C2_inner_new_t, C2_outer_new_t;
    float Horz_HODO_R0_t, Horz_HODO_R1_t, Horz_HODO_R2_t, Horz_HODO_R3_t, Horz_HODO_R4_t;
    float Horz_HODO_R5_t, Horz_HODO_R6_t, Horz_HODO_R7_t, Vert_HODO_R0_t, Vert_HODO_R1_t;
    float Vert_HODO_R2_t, Vert_HODO_R3_t, Vert_HODO_R4_t, Vert_HODO_R5_t, Vert_HODO_R6_t, Vert_HODO_R7_t;
    float Veto1_t, Veto2_t, Veto3_t, Veto4_t, TowerID_t,Tower_column_t,Tower_row_t;
    float TowerE_column_0_t;
    float TowerE_column_1_t;
    float TowerE_column_2_t;
    float TowerE_column_3_t;
    float TowerE_column_4_t;
    float TowerE_column_5_t;
    float TowerE_column_6_t;
    float TowerE_column_7_t;
    float TowerE_row_0_t;
    float TowerE_row_1_t;
    float TowerE_row_2_t;
    float TowerE_row_3_t;
    float TowerE_row_4_t;
    float TowerE_row_5_t;
    float TowerE_row_6_t;
    float TowerE_row_7_t;
    float HorzTowerID_t;
    float VertTowerID_t;
    float SaveHoriz_TowerID0_t;
    float SaveHoriz_TowerID1_t;
    float SaveHoriz_TowerID2_t;
    float SaveHoriz_TowerID3_t;
    float SaveHoriz_TowerID4_t;
    float SaveHoriz_TowerID5_t;
    float SaveHoriz_TowerID6_t;
    float SaveHoriz_TowerID7_t;

    float SaveVert_TowerID0_t;
    float SaveVert_TowerID1_t;
    float SaveVert_TowerID2_t;
    float SaveVert_TowerID3_t;
    float SaveVert_TowerID4_t;
    float SaveVert_TowerID5_t;
    float SaveVert_TowerID6_t;
    float SaveVert_TowerID7_t;

    for(int i=0; i<N_ENERGIES; i++){
         fin[i] = new TFile(filenames[i].c_str());

         ostringstream name;
         name << "test1";
         trees[i] = (TTree *)fin[i]->Get(name.str().c_str());

         name.str("");
         name.clear();

         name << "tree_" << energies[i];
         trees[i]->SetName(name.str().c_str());
         trees[i]->SetBranchAddress("Etotal_t",&Etotal_t);
         trees[i]->SetBranchAddress("E3by3_t",&E3by3_t);
         trees[i]->SetBranchAddress("Emax_t",&Emax_t);

         trees[i]->SetBranchAddress("E5by5_t",&E5by5_t);
         trees[i]->SetBranchAddress("C1_t",&C1_t);
         trees[i]->SetBranchAddress("C2_inner_t",&C2_inner_t);
         trees[i]->SetBranchAddress("C2_outer_t",&C2_outer_t);

        if (scan==3) {
            trees[i]->SetBranchAddress("C2_inner_new_t",&C2_inner_new_t);
            trees[i]->SetBranchAddress("C2_outer_new_t",&C2_outer_new_t);
        }
 
         trees[i]->SetBranchAddress("Horz_HODO_R0_t",&Horz_HODO_R0_t);
         trees[i]->SetBranchAddress("Horz_HODO_R1_t",&Horz_HODO_R1_t);
         trees[i]->SetBranchAddress("Horz_HODO_R2_t",&Horz_HODO_R2_t);
         trees[i]->SetBranchAddress("Horz_HODO_R3_t",&Horz_HODO_R3_t);
         trees[i]->SetBranchAddress("Horz_HODO_R4_t",&Horz_HODO_R4_t);
         trees[i]->SetBranchAddress("Horz_HODO_R5_t",&Horz_HODO_R5_t);
         trees[i]->SetBranchAddress("Horz_HODO_R6_t",&Horz_HODO_R6_t);
         trees[i]->SetBranchAddress("Horz_HODO_R7_t",&Horz_HODO_R7_t);
         trees[i]->SetBranchAddress("Vert_HODO_R0_t",&Vert_HODO_R0_t);
         trees[i]->SetBranchAddress("Vert_HODO_R1_t",&Vert_HODO_R1_t);
         trees[i]->SetBranchAddress("Vert_HODO_R2_t",&Vert_HODO_R2_t);
         trees[i]->SetBranchAddress("Vert_HODO_R3_t",&Vert_HODO_R3_t);
         trees[i]->SetBranchAddress("Vert_HODO_R4_t",&Vert_HODO_R4_t);
         trees[i]->SetBranchAddress("Vert_HODO_R5_t",&Vert_HODO_R5_t);
         trees[i]->SetBranchAddress("Vert_HODO_R6_t",&Vert_HODO_R6_t);
         trees[i]->SetBranchAddress("Vert_HODO_R7_t",&Vert_HODO_R7_t);
         trees[i]->SetBranchAddress("Veto1_t",&Veto1_t);
         trees[i]->SetBranchAddress("Veto2_t",&Veto2_t);
         trees[i]->SetBranchAddress("Veto3_t",&Veto3_t);
         trees[i]->SetBranchAddress("Veto4_t",&Veto4_t);
         trees[i]->SetBranchAddress("TowerID_t",&TowerID_t);
         trees[i]->SetBranchAddress("Tower_column_t",&Tower_column_t);
         trees[i]->SetBranchAddress("Tower_row_t",&Tower_row_t);
         trees[i]->SetBranchAddress("TowerE_column_0_t",&TowerE_column_0_t);
         trees[i]->SetBranchAddress("TowerE_column_1_t",&TowerE_column_1_t);
         trees[i]->SetBranchAddress("TowerE_column_2_t",&TowerE_column_2_t);
         trees[i]->SetBranchAddress("TowerE_column_3_t",&TowerE_column_3_t);
         trees[i]->SetBranchAddress("TowerE_column_4_t",&TowerE_column_4_t);
         trees[i]->SetBranchAddress("TowerE_column_5_t",&TowerE_column_5_t);
         trees[i]->SetBranchAddress("TowerE_column_6_t",&TowerE_column_6_t);
         trees[i]->SetBranchAddress("TowerE_column_7_t",&TowerE_column_7_t);
         trees[i]->SetBranchAddress("TowerE_row_0_t",&TowerE_row_0_t);
         trees[i]->SetBranchAddress("TowerE_row_1_t",&TowerE_row_1_t);
         trees[i]->SetBranchAddress("TowerE_row_2_t",&TowerE_row_2_t);
         trees[i]->SetBranchAddress("TowerE_row_3_t",&TowerE_row_3_t);
         trees[i]->SetBranchAddress("TowerE_row_4_t",&TowerE_row_4_t);
         trees[i]->SetBranchAddress("TowerE_row_5_t",&TowerE_row_5_t);
         trees[i]->SetBranchAddress("TowerE_row_6_t",&TowerE_row_6_t);
         trees[i]->SetBranchAddress("TowerE_row_7_t",&TowerE_row_7_t);
         trees[i]->SetBranchAddress("HorzTowerID_t",&HorzTowerID_t);
         trees[i]->SetBranchAddress("VertTowerID_t",&VertTowerID_t);

         trees[i]->SetBranchAddress("SaveHoriz_TowerID0_t",&SaveHoriz_TowerID0_t);
         trees[i]->SetBranchAddress("SaveHoriz_TowerID1_t",&SaveHoriz_TowerID1_t);
         trees[i]->SetBranchAddress("SaveHoriz_TowerID2_t",&SaveHoriz_TowerID2_t);
         trees[i]->SetBranchAddress("SaveHoriz_TowerID3_t",&SaveHoriz_TowerID3_t);
         trees[i]->SetBranchAddress("SaveHoriz_TowerID4_t",&SaveHoriz_TowerID4_t);
         trees[i]->SetBranchAddress("SaveHoriz_TowerID5_t",&SaveHoriz_TowerID5_t);
         trees[i]->SetBranchAddress("SaveHoriz_TowerID6_t",&SaveHoriz_TowerID6_t);
         trees[i]->SetBranchAddress("SaveHoriz_TowerID7_t",&SaveHoriz_TowerID7_t);
         
         trees[i]->SetBranchAddress("SaveVert_TowerID0_t",&SaveVert_TowerID0_t);
         trees[i]->SetBranchAddress("SaveVert_TowerID1_t",&SaveVert_TowerID1_t);
         trees[i]->SetBranchAddress("SaveVert_TowerID2_t",&SaveVert_TowerID2_t);
         trees[i]->SetBranchAddress("SaveVert_TowerID3_t",&SaveVert_TowerID3_t);
         trees[i]->SetBranchAddress("SaveVert_TowerID4_t",&SaveVert_TowerID4_t);
         trees[i]->SetBranchAddress("SaveVert_TowerID5_t",&SaveVert_TowerID5_t);
         trees[i]->SetBranchAddress("SaveVert_TowerID6_t",&SaveVert_TowerID6_t);
         trees[i]->SetBranchAddress("SaveVert_TowerID7_t",&SaveVert_TowerID7_t);
     }

    TF1 *fgauss[N_CUTLEVELS][N_ENERGIES];  //for the fit arrays
    TH1D *hE5x5[N_CUTLEVELS][N_ENERGIES]; //for the histogram arrays
     
    double range_histogram[2][N_ENERGIES];
    range_histogram[0][0] = 0;
    range_histogram[1][0] = 5;
    range_histogram[0][1] = 0;
    range_histogram[1][1] = 15;
    range_histogram[0][2] = 0;
    range_histogram[1][2] = 15;
    range_histogram[0][3] = 0;
    range_histogram[1][3] = 18;
    range_histogram[0][4] = 1;
    range_histogram[1][4] = 22;
    range_histogram[0][5] = 1;
    range_histogram[1][5] = 30;
    range_histogram[0][6] = 1;
    range_histogram[1][6] = 22;
    range_histogram[0][7] = 1;
    range_histogram[1][7] = 25;

    double range_fit[2][N_ENERGIES];
    //scan 3
    if (scan==3) {
        range_fit[0][0] = 1.7;
        range_fit[1][0] = 2.7;
        range_fit[0][1] = 3.5;
        range_fit[1][1] = 5.5;
        range_fit[0][2] = 5.7;
        range_fit[1][2] = 7.8;
        range_fit[0][3] = 8.0;
        range_fit[1][3] = 10.8;
        range_fit[0][4] = 11.5;
        range_fit[1][4] = 15.8;
        range_fit[0][5] = 15.0;
        range_fit[1][5] = 20;
        range_fit[0][6] = 10.2;
        range_fit[1][6] = 13.4;
        range_fit[0][7] = 11.6;
        range_fit[1][7] = 16.0;
    }
    
    if (scan==2) {
        if (location==1) {
            range_fit[0][0] = 0.7;
            range_fit[1][0] = 1.4;
            range_fit[0][1] = 1.5;
            range_fit[1][1] = 2.7;
            range_fit[0][2] = 2.5;
            range_fit[1][2] = 4.0;
            range_fit[0][3] = 3.3;
            range_fit[1][3] = 4.9;
            range_fit[0][4] = 5.2;
            range_fit[1][4] = 7.5;
            range_fit[0][5] = 7.3;
            range_fit[1][5] = 9.9;
            range_fit[0][6] = 10.9;
            range_fit[1][6] = 15.0;
            range_fit[0][7] = 14.7;
            range_fit[1][7] = 19.2;

        }
        if (location==3) {
            range_fit[0][0] = 1.8;
            range_fit[1][0] = 3.5;
            range_fit[0][1] = 1.8;
            range_fit[1][1] = 2.8;
            range_fit[0][2] = 2.8;
            range_fit[1][2] = 4.5;
            range_fit[0][3] = 4.0;
            range_fit[1][3] = 5.8;
            range_fit[0][4] = 6.5;
            range_fit[1][4] = 8.0;
            range_fit[0][5] = 8.0;
            range_fit[1][5] = 11.5;
            range_fit[0][6] = 13.5;
            range_fit[1][6] = 16.5;
            range_fit[0][7] = 17.0;
            range_fit[1][7] = 21.5;
        }
    }
    
     /*
        cutlevel 0: no cuts
        cutlevel 1: cherenkov
        cutlevel 2: veto
        cutlevel 3: hodo
        cutlevel 4: cherenkov + veto
        cutlevel 5: cherenkov + hodo
        cutlevel 6: veto + hodo
        cutlevel 7: cherenkov + veto + hodo
      */
    string cutlevel[N_CUTLEVELS];
    cutlevel[0] = "no_cuts";
    cutlevel[1] = "cherenkov";
    cutlevel[2] = "veto";
    cutlevel[3] = "hodoscope";
    cutlevel[4] = "cherenkov+veto";
    cutlevel[5] = "cherenkov+hodo";
    cutlevel[6] = "veto+hodo";
    cutlevel[7] = "cherenkov+veto+hodo";

    for(int i=0; i<N_ENERGIES; i++){

        cout << "starting: " << energies[i] << " GeV" << endl;

        //setup the energy distribution plots
        int entries = trees[i]->GetEntries();
        for(int j=0; j<N_CUTLEVELS; j++){
            //name the plots
            ostringstream name;
            name << "hE5x5_" << cutlevel[j] << "_" << energies[i]<< " GeV";
            hE5x5[j][i] = new TH1D(name.str().c_str(),name.str().c_str(),200,range_histogram[0][i],range_histogram[1][i]);
            hE5x5[j][i]->Sumw2();

            name.str("");
            name.clear();
            name << "fgauss_" << j << "_" << i;
            fgauss[j][i] = new TF1(name.str().c_str(),"gaus",range_fit[0][i],range_fit[1][i]);
        }
    
        for(j = 0; j<entries; j++){//loop to fill histograms
            trees[i]->GetEntry(j);

            bool passCherenkov = false;
            bool passVeto = false;
            //no cuts
            hE5x5[0][i]->Fill(E5by5_t);
          
            //Cherenkov cut
            if (scan==3) {
                if(fabs(C2_inner_new_t + C2_outer_new_t) > 100){
                    passCherenkov = true;
                    hE5x5[1][i]->Fill(E5by5_t);
                }
            }
            if (scan==2) {
                if(fabs(C2_inner_t + C2_outer_t) > 100){
                    passCherenkov = true;
                    hE5x5[1][i]->Fill(E5by5_t);
                }
            }
            //veto cut
            if (Veto1_t < 15 || Veto2_t < 15 || Veto3_t < 15 || Veto4_t < 15) {
                passVeto =  true;
                hE5x5[2][i]->Fill(E5by5_t);
            }

            //vertical hodoscope cut
            bool passVertical = false;
            if (scan==2) {
                if (location==1) {
                    if (fabs(Vert_HODO_R2_t) > 30||fabs(Vert_HODO_R3_t)>30) {
                        passVertical = true;
                    }
                }
                if (location==2) {
                    if (fabs(Vert_HODO_R2_t) > 30||fabs(Vert_HODO_R3_t)>30) {
                        passVertical = true;
                    }
                }
                if (location==3) {
                    if (fabs(Vert_HODO_R4_t) > 30) {
                        passVertical = true;
                    }
                }
            }
            if (scan==3) {
                if (fabs(Vert_HODO_R5_t) > 30||fabs(Vert_HODO_R6_t)>30) {
                    passVertical = true;
                }
            }
            //horizontal hodoscope cut
            bool passHorizontal = false;
            if (scan==2) {
                if (location==1) {
                    if (fabs(Horz_HODO_R5_t) > 30) {
                        passHorizontal = true;
                    }
                }
                if (location==2) {
                    if (fabs(Horz_HODO_R3_t) > 30)||fabs(Horz_HODO_R4_t) > 30) {
                        passHorizontal = true;
                    }
                }
                if (location==3) {
                    if (fabs(Horz_HODO_R3_t) > 30)||fabs(Horz_HODO_R4_t) > 30) {
                        passHorizontal = true;
                    }
                }
            }
            if (scan==3) {
                if (fabs(Horz_HODO_R4_t) > 30) {
                    passHorizontal = true;
                }
            }
            
            if (passHorizontal && passVertical) {
                hE5x5[3][i]->Fill(E5by5_t);
            }

            //veto+chrenkov cut
            if(passCherenkov && passVeto){
                hE5x5[4][i]->Fill(E5by5_t);
            }
            //cherenkov+hodo
            if (passVertical && passHorizontal && passCherenkov) {
                hE5x5[5][i]->Fill(E5by5_t);
            }
            //veto+hodo
            if (passVeto && passVertical && passHorizontal) {
                hE5x5[6][i]->Fill(E5by5_t);
            }
            //veto + hodo +cherenkov
            if(passCherenkov && passVeto && passVertical && passHorizontal) {
                hE5x5[7][i]->Fill(E5by5_t);
            }
        }
    }

    TCanvas *c2[N_CUTLEVELS];  //canvas with energy resolutions
    
    //~~~
    for(int i=0; i<N_CUTLEVELS; i++){
        ostringstream name;
        if (i==0)
            name << "no cuts_";
        if (i==1)
            name<< "cherenkov cuts ";
        if (i==2)
            name<< "veto cuts ";
        if (i==3)
            name<<" hodoscope cuts";
        if (i==4)
            name<< "chrenkov+veto";
        if (i==5)
            name<<"cherenkov+hodoscopes";
        if (i==6)
            name<<"veto+hodoscopes";
        if (i==7)
            name<<"cherenkov + veto + hodoscopes";

        c2[i] = new TCanvas(name.str().c_str(),name.str().c_str(),1500,700);//for the histograms of energies, all the cuts
        c2[i]->Divide(4,2);
        //define the naming convaention on the overall canvas
        for(int j =0; j<N_ENERGIES; j++)//loop through the enrgies on the same plot
        {
            c2[i]->cd(j+1);
            gPad->SetLogy();
            hE5x5[i][j]->GetXaxis()->SetTitle("energy in 5x5 (GeV) ");
            hE5x5[i][j]->Draw();
            hE5x5[i][j]->Fit(fgauss[i][j],"R");

        }//end j loop through 8 energy plots
        
    }//end of i loop

     //==========================================================================================
    TCanvas *c3 = new TCanvas("c3","EMCAL Energy Resolution",1500,700);
    c3->Divide(4,2);
    
    Float_t *mean [N_ENERGIES];
    
    for (int j=0; j<N_CUTLEVELS; j++)
    {
        Float_t  getMEAN[N_ENERGIES];
        Float_t getSIGMA[N_ENERGIES];
        Float_t getSIGMAerror[N_ENERGIES];
        Float_t getMEANerror[N_ENERGIES];
        
        Float_t getAllerror[N_ENERGIES];
        Float_t y_1[N_ENERGIES];  //stores the ratio of sigma/mean
        Float_t y_2[N_ENERGIES];  //stores the mean
        
        cout<<cutlevel[j]<<":"<<endl;
        for (int i=0;i<N_ENERGIES;i++)
        {
            getMEAN[i]=fgauss[j][i]->GetParameter(1);
            getSIGMA[i]=fgauss[j][i]->GetParameter(2);
            getMEANerror[i]=fgauss[j][i]->GetParError(1);
            getSIGMAerror[i]=fgauss[j][i]->GetParError(2);
            
            getAllerror[i]=(getSIGMA[i]/getMEAN[i])*TMath::Sqrt( (getSIGMAerror[i]/getSIGMA[i])*(getSIGMAerror[i]/getSIGMA[i])+(getMEANerror[i]/getMEAN[i])*(getMEANerror[i]/getMEAN[i]) );
            y_1[i]=getSIGMA[i]/getMEAN[i];
            y_2[i]=getMEAN[i];
            cout<<"All error "<<i<<" is :"<<getAllerror[i]<<endl;
            cout<<"All y "<<i<<" is :"<<y_1[i]<<endl;
        
        }  //end of energy loop
        c3->cd(j+1);
        c3->SetFillColor(0);
        c3->SetGrid();
        c3->GetFrame()->SetBorderSize(12);
        
        const Int_t n1=8;
        TF1 *f2=new TF1("f2","TMath::Sqrt([0]*[0]+([1]*[1])*TMath::Power(x,-1))",0.5,28);  //convolved
        if (scan==3) {
            Float_t x1[n1]  = {2,4,6,8,12,16,24,28};
        }
        if (scan==2) {
            Float_t x1[n1]  = {1,2,3,4,6,8,12,16};
        }
        Float_t ex1[n1] = {0,0,0,0,0,0,0,0};
        TGraphErrors *grEtotal = new TGraphErrors(n1,x1,y_1,ex1,getAllerror);
        grEtotal->Fit("f2","R");//fit the Etotal convolved
        grEtotal->SetMarkerColor(kBlack);
        grEtotal->SetMarkerStyle(20);
        ostringstream name;
        name<<cutlevel[j];
        grEtotal->SetTitle(name.str().c_str());
        grEtotal->GetXaxis()->SetTitle("Energy (GeV)");
        grEtotal->GetXaxis()->SetRangeUser(0,20);
        grEtotal->GetYaxis()->SetTitle("#sigma(E)/<E>");
        grEtotal->GetHistogram()->SetMaximum(0.25);    // along
        grEtotal->GetHistogram()->SetMinimum(0.02);   //   Y
        grEtotal->Draw("AP");
    }//end of N_cutlevels
    //c3->SaveAs("EMCal_Energy_Resolution_scan3.pdf");

    TCanvas *c6 = new TCanvas("c6","EMCAL Linearity & Energy Resolution ",1500,700);
    
    c6->Divide(2,1);
    c6->cd(2);
    const Int_t n1=8;
    TF1 *f2=new TF1("f2","TMath::Sqrt([0]*[0]+([1]*[1])*TMath::Power(x,-1))",0.5,28);  //convolved
    if (scan==3) {
        Float_t x1[n1]  = {2,4,6,8,12,16,24,28};
    }
    if (scan==2) {
        Float_t x1[n1]  = {1,2,3,4,6,8,12,16};
    }
    Float_t ex1[n1] = {0,0,0,0,0,0,0,0};
    TGraphErrors *grEtotal = new TGraphErrors(n1,x1,y_1,ex1,getAllerror);
    grEtotal->Fit("f2","R");//fit the Etotal convolved
    grEtotal->SetMarkerColor(kBlack);
    grEtotal->SetMarkerStyle(20);
    ostringstream name;
    name<<cutlevel[j];
    grEtotal->SetTitle("cherenkov+veto+hodo");
    grEtotal->GetXaxis()->SetTitle("Energy (GeV)");
    grEtotal->GetXaxis()->SetRangeUser(0,30);
    grEtotal->GetYaxis()->SetTitle("#sigma(E)/<E>");
    grEtotal->GetHistogram()->SetMaximum(0.3);    // along
    grEtotal->GetHistogram()->SetMinimum(-0.1);   //   Y
    grEtotal->Draw("AP");
    c6->cd(1);
    //make a linearity plot
    gStyle->SetStatX(0.9);
    gStyle->SetStatY(0.35);

    TF1 * f_calo_l = new TF1("f_calo_l", "pol1", 0.5, 20);
    TGraphErrors *grLinearity = new TGraphErrors(n1,x1,y_2,ex1,getAllerror);
    grLinearity->Fit("f_calo_l","R");//fit the Etotal convolved
    grLinearity->SetMarkerColor(kBlack);
    grLinearity->SetMarkerStyle(20);
    grLinearity->SetTitle("Linearity 3rd scan");
    grLinearity->GetXaxis()->SetTitle("Input Energy (GeV)");
    grLinearity->GetXaxis()->SetRangeUser(1,30);
    grLinearity->GetYaxis()->SetTitle("Measured Energy (GeV)");
    grLinearity->Draw("AP");
    TGraphErrors *y_x = new TGraphErrors(n1,x1,x1,ex1,ex1);
    y_x->SetLineColor(kGray);
    y_x->Draw("same");
    
}


