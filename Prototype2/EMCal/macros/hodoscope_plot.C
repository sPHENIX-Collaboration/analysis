static const int N_ENERGIES = 8;
#include "load_files.C"
void hodoscope_plot() {
    int scan = 2;
    int location = 1;

    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    int energies[N_ENERGIES];
    string filenames[N_ENERGIES];
    load_files(scan,location,energies,filenames);

    TFile *fin[N_ENERGIES];
    TTree *trees[N_ENERGIES];

    float E5by5_t, C1_t,  C2_t,  Horz_HODO_R0_t, Horz_HODO_R1_t, Horz_HODO_R2_t, Horz_HODO_R3_t, Horz_HODO_R4_t;
    float Horz_HODO_R5_t, Horz_HODO_R6_t, Horz_HODO_R7_t, Vert_HODO_R0_t, Vert_HODO_R1_t, Vert_HODO_R2_t,Vert_HODO_R3_t, Vert_HODO_R4_t, Vert_HODO_R5_t, Vert_HODO_R6_t, Vert_HODO_R7_t;
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
        //trees[i]->SetBranchAddress("C2_t",&C2_t);
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
    TH1D *hH[N_ENERGIES];
    TH2D *hH_row[N_ENERGIES], *hH_column[N_ENERGIES];
    TH1D *hV[N_ENERGIES];
    TH2D *hV_row[N_ENERGIES], *hV_column[N_ENERGIES];

    double hodoscope_cut = 30;

    TCanvas *ch = new TCanvas("ch","horizontal",1000,1000);
    ch->Divide(3,3);
    TCanvas *cv = new TCanvas("cv","vertical",1000,1000);
    cv->Divide(3,3);

    TCanvas *chr = new TCanvas("chr","horizontal row",1000,1000);
    chr->Divide(3,3);
    TCanvas *cvr = new TCanvas("cvr","vertical row",1000,1000);
    cvr->Divide(3,3);

    TCanvas *chc = new TCanvas("chc","horizontal column",1000,1000);
    chc->Divide(3,3);
    TCanvas *cvc = new TCanvas("cvc","vertical column",1000,1000);
    cvc->Divide(3,3);
    for(int i=0; i<N_ENERGIES; i++){
        
        cout << "starting: " << energies[i] << " GeV" << endl;
        int entries = trees[i]->GetEntries();

        ostringstream name;

        ostringstream title;
        title << " " << energies[i] << " GeV";
        name << "hH_" << i;
        hH[i] = new TH1D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5);
        hH[i]->GetXaxis()->SetTitle("horizontal hodoscope position");
        name.str("");
        name.clear();
        name << "hH_row_" << i;
        hH_row[i] = new TH2D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5,8,-0.5,7.5);
        hH_row[i]->GetXaxis()->SetTitle("horizontal hodoscope position");
        hH_row[i]->GetYaxis()->SetTitle("EMCal row ");
        name.str("");
        name.clear();
        name << "hH_column_" << i;
        hH_column[i] = new TH2D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5,8,-0.5,7.5);
        hH_column[i]->GetXaxis()->SetTitle("horizontal hodoscope position");
        hH_column[i]->GetYaxis()->SetTitle("EMCal column ");

        name.str("");
        name.clear();
        name << "hV_" << i;
        hV[i] = new TH1D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5);
        hV[i]->GetXaxis()->SetTitle("vertical hodoscope position");
        name.str("");
        name.clear();
        name << "hV_row_" << i;
        hV_row[i] = new TH2D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5,8,-0.5,7.5);
        hV_row[i]->GetXaxis()->SetTitle("vertical hodoscope position");
        hV_row[i]->GetYaxis()->SetTitle("EMCal row");

        name.str("");
        name.clear();
        name << "hV_column_" << i;
        hV_column[i] = new TH2D(name.str().c_str(),title.str().c_str(),24,-0.5,7.5,8,-0.5,7.5);
        hV_column[i]->GetXaxis()->SetTitle("vertical hodoscope position");
        hV_column[i]->GetYaxis()->SetTitle("EMCal column");
   	 
      	for(int j = 0; j<entries; j++){
            trees[i]->GetEntry(j);
            double position_h = 0.0;
            double energy_h = 0.0;
            double position_v = 0.0;
            double energy_v = 0.0;
            if(Horz_HODO_R0_t > hodoscope_cut){
                position_h += Horz_HODO_R0_t*0.0;
                energy_h += Horz_HODO_R0_t;
            }
            if(Horz_HODO_R1_t > hodoscope_cut){
                position_h += Horz_HODO_R1_t*1.0;
                energy_h += Horz_HODO_R1_t;
            }
            if(Horz_HODO_R2_t > hodoscope_cut){
                position_h += Horz_HODO_R2_t*2.0;
                energy_h += Horz_HODO_R2_t;
            }
            if(Horz_HODO_R3_t > hodoscope_cut){
                position_h += Horz_HODO_R3_t*3.0;
                energy_h += Horz_HODO_R3_t;
            }
            if(Horz_HODO_R4_t > hodoscope_cut){
                position_h += Horz_HODO_R4_t*4.0;
                energy_h += Horz_HODO_R4_t;
            }
            if(Horz_HODO_R5_t > hodoscope_cut){
                position_h += Horz_HODO_R5_t*5.0;
                energy_h += Horz_HODO_R5_t;
            }
            if(Horz_HODO_R6_t > hodoscope_cut){
                position_h += Horz_HODO_R6_t*6.0;
                energy_h += Horz_HODO_R6_t;
            }
            if(Horz_HODO_R7_t > hodoscope_cut){
                position_h += Horz_HODO_R7_t*7.0;
                energy_h += Horz_HODO_R7_t;
            }

            if(Vert_HODO_R0_t > hodoscope_cut){
                position_v += Vert_HODO_R0_t*0.0;
                energy_v += Vert_HODO_R0_t;
            }
            if(Vert_HODO_R1_t > hodoscope_cut){
                position_v += Vert_HODO_R1_t*1.0;
                energy_v += Vert_HODO_R1_t;
            }
            if(Vert_HODO_R2_t > hodoscope_cut){
                position_v += Vert_HODO_R2_t*2.0;
                energy_v += Vert_HODO_R2_t;
            }
            if(Vert_HODO_R3_t > hodoscope_cut){
                position_v += Vert_HODO_R3_t*3.0;
                energy_v += Vert_HODO_R3_t;
            }
            if(Vert_HODO_R4_t > hodoscope_cut){
                position_v += Vert_HODO_R4_t*4.0;
                energy_v += Vert_HODO_R4_t;
            }
            if(Vert_HODO_R5_t > hodoscope_cut){
                position_v += Vert_HODO_R5_t*5.0;
                energy_v += Vert_HODO_R5_t;
            }
            if(Vert_HODO_R6_t > hodoscope_cut){
                position_v += Vert_HODO_R6_t*6.0;
                energy_v += Vert_HODO_R6_t;
            }
            if(Vert_HODO_R7_t > hodoscope_cut){
                position_v += Vert_HODO_R7_t*7.0;
                energy_v += Vert_HODO_R7_t;
            }

            position_h /= energy_h;
            position_v /= energy_v;
            hH[i]->Fill(position_h);
            hV[i]->Fill(position_v);

            hH_row[i]->Fill(position_h,0,TowerE_row_0_t);
            hH_row[i]->Fill(position_h,1,TowerE_row_1_t);
            hH_row[i]->Fill(position_h,2,TowerE_row_2_t);
            hH_row[i]->Fill(position_h,3,TowerE_row_3_t);
            hH_row[i]->Fill(position_h,4,TowerE_row_4_t);
            hH_row[i]->Fill(position_h,5,TowerE_row_5_t);
            hH_row[i]->Fill(position_h,6,TowerE_row_6_t);
            hH_row[i]->Fill(position_h,7,TowerE_row_7_t);
	   
            hH_column[i]->Fill(position_h,0,TowerE_column_0_t);
            hH_column[i]->Fill(position_h,1,TowerE_column_1_t);
            hH_column[i]->Fill(position_h,2,TowerE_column_2_t);
            hH_column[i]->Fill(position_h,3,TowerE_column_3_t);
            hH_column[i]->Fill(position_h,4,TowerE_column_4_t);
            hH_column[i]->Fill(position_h,5,TowerE_column_5_t);
            hH_column[i]->Fill(position_h,6,TowerE_column_6_t);
            hH_column[i]->Fill(position_h,7,TowerE_column_7_t);

            hV_row[i]->Fill(position_v,0,TowerE_row_0_t);
            hV_row[i]->Fill(position_v,1,TowerE_row_1_t);
            hV_row[i]->Fill(position_v,2,TowerE_row_2_t);
            hV_row[i]->Fill(position_v,3,TowerE_row_3_t);
            hV_row[i]->Fill(position_v,4,TowerE_row_4_t);
            hV_row[i]->Fill(position_v,5,TowerE_row_5_t);
            hV_row[i]->Fill(position_v,6,TowerE_row_6_t);
            hV_row[i]->Fill(position_v,7,TowerE_row_7_t);
	   
            hV_column[i]->Fill(position_v,0,TowerE_column_0_t);
            hV_column[i]->Fill(position_v,1,TowerE_column_1_t);
            hV_column[i]->Fill(position_v,2,TowerE_column_2_t);
            hV_column[i]->Fill(position_v,3,TowerE_column_3_t);
            hV_column[i]->Fill(position_v,4,TowerE_column_4_t);
            hV_column[i]->Fill(position_v,5,TowerE_column_5_t);
            hV_column[i]->Fill(position_v,6,TowerE_column_6_t);
            hV_column[i]->Fill(position_v,7,TowerE_column_7_t);
        }
        ch->cd(i+1);
        hH[i]->Draw("colz");
        cv->cd(i+1);
        hV[i]->Draw("colz");

        chc->cd(i+1);
        hH_column[i]->Draw("colz");
        cvc->cd(i+1);
        hV_column[i]->Draw("colz");
	
        chr->cd(i+1);
        hH_row[i]->Draw("colz");
        cvr->cd(i+1);
        hV_row[i]->Draw("colz");
    }

    ostringstream outname;
     
    outname << "plots/horizontal_hodoscope_scan" << scan << "_location_" << location << ".pdf";
    ch->Print(outname.str().c_str());
    outname.str("");
    outname.clear();
    outname << "plots/vertical_hodoscope_scan" << scan << "_location_" << location << ".pdf";
    cv->Print(outname.str().c_str());
    outname.str("");
    outname.clear();
    outname << "plots/horizontal_hodoscope_column_scan" << scan << "_location_" << location << ".pdf";
    chc->Print(outname.str().c_str());
    outname.str("");
    outname.clear();
    outname << "plots/horizontal_hodoscope_row_scan" << scan << "_location_" << location << ".pdf";
    chr->Print(outname.str().c_str());
    outname.str("");
    outname.clear();
    outname << "plots/vertical_hodoscope_column_scan" << scan << "_location_" << location << ".pdf";
    cvc->Print(outname.str().c_str());
    outname.str("");
    outname.clear();
    outname << "plots/vertical_hodoscope_raw_scan" << scan << "_location_" << location << ".pdf";
    cvr->Print(outname.str().c_str());
}


