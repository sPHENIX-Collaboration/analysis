static const int N_ENERGIES = 8;
#include "load_files.C"
void cherenkov_analysis() {
    int scan = 2;
    int location = 1;
    
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    int energies[N_ENERGIES];
    string filenames[N_ENERGIES];
    load_files(scan,location,energies,filenames);

    double hodoscope_cut = 30;

    TFile *fin[N_ENERGIES];
    TTree *trees[N_ENERGIES];

    float E5by5_t, C1_t,  C2_inner_t, C2_outer_t,  Horz_HODO_R0_t, Horz_HODO_R1_t, Horz_HODO_R2_t, Horz_HODO_R3_t, Horz_HODO_R4_t;
    float C2_inner_new_t, C2_outer_new_t;
    float Horz_HODO_R5_t, Horz_HODO_R6_t, Horz_HODO_R7_t, Vert_HODO_R0_t, Vert_HODO_R1_t, Vert_HODO_R2_t, Vert_HODO_R3_t, Vert_HODO_R4_t, Vert_HODO_R5_t, Vert_HODO_R6_t, Vert_HODO_R7_t;
    float Veto1_t, Veto2_t, Veto3_t, Veto4_t;
    float TowerE_row_0_t, TowerE_row_1_t, TowerE_row_2_t, TowerE_row_3_t, TowerE_row_4_t, TowerE_row_5_t, TowerE_row_6_t, TowerE_row_7_t;
    float TowerE_column_0_t, TowerE_column_1_t, TowerE_column_2_t, TowerE_column_3_t, TowerE_column_4_t, TowerE_column_5_t, TowerE_column_6_t, TowerE_column_7_t;

    for(int i=0; i<N_ENERGIES; i++){
        fin[i] = new TFile(filenames[i].c_str());

        ostringstream name;
        name << "test1";
        trees[i] = (TTree *)fin[i]->Get(name.str().c_str());

        name.str("");
        name.clear();

        name << "tree_" << energies[i];
        trees[i]->SetName(name.str().c_str());

        trees[i]->SetBranchAddress("E5by5_t",&E5by5_t);
        trees[i]->SetBranchAddress("C1_t",&C1_t);
        trees[i]->SetBranchAddress("C2_inner_t",&C2_inner_t);
        trees[i]->SetBranchAddress("C2_outer_t",&C2_outer_t);
        if (scan==3) {
            trees[i]->SetBranchAddress("C2_outer_new_t",&C2_outer_new_t);
            trees[i]->SetBranchAddress("C2_inner_new_t",&C2_inner_new_t);
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
        trees[i]->SetBranchAddress("TowerE_row_0_t",&TowerE_row_0_t);
        trees[i]->SetBranchAddress("TowerE_row_1_t",&TowerE_row_1_t);
        trees[i]->SetBranchAddress("TowerE_row_2_t",&TowerE_row_2_t);
        trees[i]->SetBranchAddress("TowerE_row_3_t",&TowerE_row_3_t);
        trees[i]->SetBranchAddress("TowerE_row_4_t",&TowerE_row_4_t);
        trees[i]->SetBranchAddress("TowerE_row_5_t",&TowerE_row_5_t);
        trees[i]->SetBranchAddress("TowerE_row_6_t",&TowerE_row_6_t);
        trees[i]->SetBranchAddress("TowerE_row_7_t",&TowerE_row_7_t);
        trees[i]->SetBranchAddress("TowerE_column_0_t",&TowerE_column_0_t);
        trees[i]->SetBranchAddress("TowerE_column_1_t",&TowerE_column_1_t);
        trees[i]->SetBranchAddress("TowerE_column_2_t",&TowerE_column_2_t);
        trees[i]->SetBranchAddress("TowerE_column_3_t",&TowerE_column_3_t);
        trees[i]->SetBranchAddress("TowerE_column_4_t",&TowerE_column_4_t);
        trees[i]->SetBranchAddress("TowerE_column_5_t",&TowerE_column_5_t);
        trees[i]->SetBranchAddress("TowerE_column_6_t",&TowerE_column_6_t);
        trees[i]->SetBranchAddress("TowerE_column_7_t",&TowerE_column_7_t);
    }

    TH1D *hCherenkov[N_ENERGIES];
    TH2D *hCherenkov_corr[N_ENERGIES];
    TCanvas *c2 = new TCanvas("c2","c2",1000,500);
    c2->Divide(4,2);
    for(int i=0; i<N_ENERGIES; i++){

        cout << "starting: " << energies[i] << " GeV" << endl;
        int entries = trees[i]->GetEntries();

        ostringstream name;
        name << "hCherenkov_" << i;
        ostringstream title;
        title << " " << energies[i] << " GeV";
   	 
        hCherenkov[i] = new TH1D(name.str().c_str(),title.str().c_str(),100,-500,1000);

        name.str("");
        name.clear();
        name << "hCherenkov_corr_" << i;

        hCherenkov_corr[i] = new TH2D(name.str().c_str(),name.str().c_str(),100,-500,2500,2500,-500,2500);

      	for(int j = 0; j<entries; j++){
            trees[i]->GetEntry(j);

            //cut on hodoscope
            bool horizontal_hodoscope = false;
            bool vertical_hodoscope = false;
            if(Horz_HODO_R0_t > hodoscope_cut)horizontal_hodoscope = true;
            if(Horz_HODO_R1_t > hodoscope_cut)horizontal_hodoscope = true;
            if(Horz_HODO_R2_t > hodoscope_cut)horizontal_hodoscope = true;
            if(Horz_HODO_R3_t > hodoscope_cut)horizontal_hodoscope = true;
            if(Horz_HODO_R4_t > hodoscope_cut)horizontal_hodoscope = true;
            if(Horz_HODO_R5_t > hodoscope_cut)horizontal_hodoscope = true;
            if(Horz_HODO_R6_t > hodoscope_cut)horizontal_hodoscope = true;
            if(Horz_HODO_R7_t > hodoscope_cut)horizontal_hodoscope = true;
	   
            if(Vert_HODO_R0_t > hodoscope_cut)vertical_hodoscope = true;
            if(Vert_HODO_R1_t > hodoscope_cut)vertical_hodoscope = true;
            if(Vert_HODO_R2_t > hodoscope_cut)vertical_hodoscope = true;
            if(Vert_HODO_R3_t > hodoscope_cut)vertical_hodoscope = true;
            if(Vert_HODO_R4_t > hodoscope_cut)vertical_hodoscope = true;
            if(Vert_HODO_R5_t > hodoscope_cut)vertical_hodoscope = true;
            if(Vert_HODO_R6_t > hodoscope_cut)vertical_hodoscope = true;
            if(Vert_HODO_R7_t > hodoscope_cut)vertical_hodoscope = true;
            if(!horizontal_hodoscope || !vertical_hodoscope)continue;


            if (scan==3) {
                double c2_total = C2_inner_new_t + C2_outer_new_t;
            }
            if (scan==2) {
                double c2_total = C2_inner_t + C2_outer_t;
            }
            hCherenkov[i]->Fill(c2_total);
        }
        c2->cd(i+1);
        c2->GetPad(i+1)->SetLogy();
        hCherenkov[i]->Draw("colz");
    }

    ostringstream outname;

    outname << "plots/cherenkov_scan" << scan << ".pdf";
    c2->Print(outname.str().c_str());
}



