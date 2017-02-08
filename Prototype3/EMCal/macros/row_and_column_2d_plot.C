#include <algorithm>
#include "load_files.C"
static const int N_ENERGIES = 8;
void row_and_column_2d_plot() {
    

    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    int scan = 3;
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

    TH2D *h_row_col [N_ENERGIES];
    TH2D *h_no_cuts [N_ENERGIES];


    TCanvas *crc = new TCanvas("crc","with cuts",1000,1000);
    crc->Divide(3,3);
    
    TCanvas *crc2 = new TCanvas("crc2","without cuts",1000,1000);
    crc2->Divide(3,3);
    
    for(int i=0; i<N_ENERGIES; i++){
        int entries = trees[i]->GetEntries();

        ostringstream name;
        ostringstream title;

        name << "h_" << i;
        title<<energies[i]<<" GeV";
        h_row_col[i] = new TH2D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5,8,-0.5,7.5);
        h_row_col[i]->GetXaxis()->SetTitle("EMCal row");
        h_row_col[i]->GetYaxis()->SetTitle("EMCal column");
         
        name.str("");
        name.clear();
         
        name<<"h_2_"<<i;
         
        h_no_cuts[i] = new TH2D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5,8,-0.5,7.5);
        h_no_cuts[i]->GetXaxis()->SetTitle("EMCal row");
        h_no_cuts[i]->GetYaxis()->SetTitle("EMCal column");
        
        for(int j = 0; j<entries; j++){
            trees[i]->GetEntry(j);
            bool passCherenkov = false;
            bool passVeto = false;
            bool passVertical = false;
            bool passHorizontal = false;
            
            //Cherenkov cut
            if (scan==3) {
                if(fabs(C2_inner_new_t + C2_outer_new_t) > 100){
                    passCherenkov = true;
                }
            }
            if (scan==2) {
                if(fabs(C2_inner_t + C2_outer_t) > 100){
                    passCherenkov = true;
                }
            }
            //veto cut
            if (Veto1_t < 15 || Veto2_t < 15 || Veto3_t < 15 || Veto4_t < 15) {
                passVeto =  true;
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
            
            //with all cuts
            if (passCherenkov && passHorizontal && passVertical && passVeto) {
                float max_row_E=0;
                float rowname[8];
                int max_row;
                float max_col_E=0;
                float colname[8];
                int max_col;
                    
                rowname[0] = TowerE_row_0_t;
                rowname[1] = TowerE_row_1_t;
                rowname[2] = TowerE_row_2_t;
                rowname[3] = TowerE_row_3_t;
                rowname[4] = TowerE_row_4_t;
                rowname[5] = TowerE_row_5_t;
                rowname[6] = TowerE_row_6_t;
                rowname[7] = TowerE_row_7_t;
                    
                colname[0] = TowerE_column_0_t;
                colname[1] = TowerE_column_1_t;
                colname[2] = TowerE_column_2_t;
                colname[3] = TowerE_column_3_t;
                colname[4] = TowerE_column_4_t;
                colname[5] = TowerE_column_5_t;
                colname[6] = TowerE_column_6_t;
                colname[7] = TowerE_column_7_t;
                    
                for (int k=0; k<8; k++) {
                    if (rowname[k]>max_row_E) {
                        max_row_E = rowname[k];
                        max_row = k;
                    }
                    if (colname[k]>max_col_E) {
                        max_col_E = colname[k];
                        max_col = k;
                    }
                }
                h_row_col[i]->Fill(max_row,max_col);
            }
            //max row and column without cuts
            float max_row_E=0;
            float rowname[8];
            int max_row;
            float max_col_E=0;
            float colname[8];
            int max_col;
            
            rowname[0] = TowerE_row_0_t;
            rowname[1] = TowerE_row_1_t;
            rowname[2] = TowerE_row_2_t;
            rowname[3] = TowerE_row_3_t;
            rowname[4] = TowerE_row_4_t;
            rowname[5] = TowerE_row_5_t;
            rowname[6] = TowerE_row_6_t;
            rowname[7] = TowerE_row_7_t;
            
            colname[0] = TowerE_column_0_t;
            colname[1] = TowerE_column_1_t;
            colname[2] = TowerE_column_2_t;
            colname[3] = TowerE_column_3_t;
            colname[4] = TowerE_column_4_t;
            colname[5] = TowerE_column_5_t;
            colname[6] = TowerE_column_6_t;
            colname[7] = TowerE_column_7_t;
            
            for (int k=0; k<8; k++) {
                if (rowname[k]>max_row_E) {
                    max_row_E = rowname[k];
                    max_row = k;
                }
                if (colname[k]>max_col_E) {
                    max_col_E = colname[k];
                    max_col = k;
                }
            }
            h_no_cuts[i]->Fill(max_row,max_col);
        }
        
         crc->cd(i+1);
         h_row_col[i]->Draw("colz");
        
        
        crc2->cd(i+1);
        h_no_cuts[i]->Draw("colz");
     }
    crc->SaveAs("Row_and_Tower_after_cuts_THP.pdf");
    crc2->SaveAs("Row_and_Tower_before_cuts_THP.pdf");
}


