#include "../INTTDSTchain.C"
vector<string> read_list (string MC_directory, string MC_list_name)
{
    string list_buffer;
    ifstream data_list;
	data_list.open((MC_directory + "/" + MC_list_name).c_str());

    vector<string> output_vec; output_vec.clear();

	while (1)
	{
		data_list >> list_buffer;
		if (!data_list.good())
		{
			break;
		}
		output_vec.push_back(list_buffer);
	}
	cout<<"size : "<<output_vec.size()<<endl;

    return output_vec;
}

void sanity_check()
{
    string folder_direction = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams";
    string MC_list_name = "file_list.txt";
    vector <string> file_list = read_list(folder_direction,MC_list_name);

    TChain * chain_in = new TChain("EventTree");
    INTTDSTchain inttDSTMC(chain_in,folder_direction,file_list);
    std::printf("inttDSTMC N event : %lli \n", chain_in->GetEntries());

    TH2F * MC_vtx_xy = new TH2F("MC_vtx_xy","",100,-0.2,0.2,100,-0.,0.4);
    MC_vtx_xy -> GetXaxis() -> SetTitle("X axis [cm]");
    MC_vtx_xy -> GetYaxis() -> SetTitle("Y axis [cm]");

    TH1F * MC_vtx_z = new TH1F("MC_vtx_z","",100,-35,35);
    MC_vtx_z -> GetXaxis() -> SetTitle("Z axis [cm]");
    MC_vtx_z -> GetYaxis() -> SetTitle("Entry");

    TH1F * MC_vtx_Npart = new TH1F("Npart","",100,0,10000);
    MC_vtx_Npart -> GetXaxis() -> SetTitle("Npart");
    MC_vtx_Npart -> GetYaxis() -> SetTitle("Entry");

    TH2F * MC_Npart_Ntrack = new TH2F("MC_Npart_Ntrack","",200,0,10000, 200, 0, 4000);
    MC_Npart_Ntrack -> GetXaxis() -> SetTitle("Npart");
    MC_Npart_Ntrack -> GetYaxis() -> SetTitle("Ntrack");

    TH1F * MC_INTT_multiplicity = new TH1F("MC_INTT_multiplicity","",100,0,8000);
    MC_INTT_multiplicity -> GetXaxis() -> SetTitle("INTT N cluster");
    MC_INTT_multiplicity -> GetYaxis() -> SetTitle("Entry");

    TH1F * MC_INTT_CluPhiSize = new TH1F("MC_INTT_CluPhiSize","",25,0,25);
    MC_INTT_CluPhiSize -> GetXaxis() -> SetTitle("Size");
    MC_INTT_CluPhiSize -> GetYaxis() -> SetTitle("Entry");

    TH1F * MC_INTT_CluZSize = new TH1F("MC_INTT_CluZSize","",25,0,25);
    MC_INTT_CluZSize -> GetXaxis() -> SetTitle("Size");
    MC_INTT_CluZSize -> GetYaxis() -> SetTitle("Entry");

    TH1F * MC_INTT_CluADC = new TH1F("MC_INTT_CluADC","",150,0,3000);
    MC_INTT_CluADC -> GetXaxis() -> SetTitle("ADC");
    MC_INTT_CluADC -> GetYaxis() -> SetTitle("Entry");

    TH2F * MC_INTT_clu_nhit_corr = new TH2F("MC_INTT_clu_nhit_corr","",200,0,45000,200,0,9000);
    MC_INTT_clu_nhit_corr -> GetXaxis() -> SetTitle("NTrkrhits");
    MC_INTT_clu_nhit_corr -> GetYaxis() -> SetTitle("NClu");

    TH2F * MC_Ntrack_NClu = new TH2F("MC_Ntrack_NClu","",200,0,9000, 200, 0, 4000);
    MC_Ntrack_NClu -> GetXaxis() -> SetTitle("NClus");
    MC_Ntrack_NClu -> GetYaxis() -> SetTitle("Ntrack");

    TH1F * MC_Ntrack_1D = new TH1F("MC_Ntrack_1D","",200,0,4000);
    MC_Ntrack_1D -> GetXaxis() -> SetTitle("Ntrack");
    MC_Ntrack_1D -> GetYaxis() -> SetTitle("Entry");
    
    TH1F * MC_true_track_eta = new TH1F("MC_true_track_eta","",29,-2.9,2.9);
    MC_true_track_eta -> GetXaxis() -> SetTitle("#eta");
    MC_true_track_eta -> GetYaxis() -> SetTitle("dN/d#eta");

    TH2F * MC_NClu_Zvtx = new TH2F("MC_NClu_Zvtx","",200,0,9000, 200, -500, 500);
    MC_NClu_Zvtx -> GetXaxis() -> SetTitle("NClus");
    MC_NClu_Zvtx -> GetYaxis() -> SetTitle("Z vertex [mm]");

    TH1F * MC_inner_phi_3 = new TH1F("MC_inner_phi_3","",100,0,360);
    MC_inner_phi_3 -> GetXaxis() -> SetTitle("Inner phi");
    MC_inner_phi_3 -> GetYaxis() -> SetTitle("Entry");

    TH1F * MC_inner_phi_4 = new TH1F("MC_inner_phi_4","",100,0,360);
    MC_inner_phi_4 -> GetXaxis() -> SetTitle("Inner phi");
    MC_inner_phi_4 -> GetYaxis() -> SetTitle("Entry");

    TH1F * MC_outer_phi_5 = new TH1F("MC_outer_phi_5","",100,0,360);
    MC_outer_phi_5 -> GetXaxis() -> SetTitle("Outer phi");
    MC_outer_phi_5 -> GetYaxis() -> SetTitle("Entry");

    TH1F * MC_outer_phi_6 = new TH1F("MC_outer_phi_6","",100,0,360);
    MC_outer_phi_6 -> GetXaxis() -> SetTitle("Outer phi");
    MC_outer_phi_6 -> GetYaxis() -> SetTitle("Entry");

    int good_evt_counter = 0;
    int eta_one_track_counter = 0;
    int eta_one_track_counter_evt = 0;
    int eta_two_track_counter_evt = 0;

    for (int i = 0; i < chain_in->GetEntries(); i++) {
        inttDSTMC.LoadTree(i);
        inttDSTMC.GetEntry(i);

        if (i % 1000 == 0){
            cout<<"evnet running : "<<i<<endl;
            cout<<"----------------------------------------------- -----------------------------------------------"<<endl;
            printf(" event : %i, NTruthVtx : %i, NClus : %i, NTrkrhits : %i \n", inttDSTMC.event, inttDSTMC.NTruthVtx, inttDSTMC.NClus, inttDSTMC.NTrkrhits);
            printf(" size of ClusX : %zu \n", inttDSTMC.ClusX -> size());
            printf("triggered VTX ? : %.2f, %.2f, %.2f \n", inttDSTMC.TruthPV_trig_x, inttDSTMC.TruthPV_trig_y, inttDSTMC.TruthPV_trig_z);
        }

        // MC_Ntrack_1D   -> Fill(inttDSTMC.UniqueAncG4P_TrackID -> size());

        if (inttDSTMC.NTruthVtx == 1){

            // for (int track_i = 0; track_i < inttDSTMC.UniqueAncG4P_TrackID -> size(); track_i++){
            //     if (fabs(inttDSTMC.UniqueAncG4P_Eta -> at(track_i)) <= 1.){
            //         eta_one_track_counter_evt += 1;
            //     }

            //     if (fabs(inttDSTMC.UniqueAncG4P_Eta -> at(track_i)) <= 2.){
            //         eta_two_track_counter_evt += 1;
            //     }

            // }
            MC_NClu_Zvtx -> Fill(inttDSTMC.NClus, (inttDSTMC.TruthPV_trig_z) * 10.);
            // MC_Ntrack_1D   -> Fill(inttDSTMC.UniqueAncG4P_TrackID -> size());
            MC_Ntrack_NClu -> Fill(inttDSTMC.NClus, eta_two_track_counter_evt);
            eta_one_track_counter_evt = 0;
            eta_two_track_counter_evt = 0;
            
            // if (0/*good_evt_counter*/ == 0) {
            //     if (inttDSTMC.NClus > 4813){
            //         good_evt_counter += 1;
            //         // cout<<"test : "<<inttDSTMC.UniqueAncG4P_TrackID -> size()<<" "<<inttDSTMC.NClus<<endl;
            //         for (int track_i = 0; track_i < inttDSTMC.UniqueAncG4P_TrackID -> size(); track_i++){
            //             MC_true_track_eta -> Fill(inttDSTMC.UniqueAncG4P_Eta -> at(track_i));
            //             if (fabs(inttDSTMC.UniqueAncG4P_Eta -> at(track_i)) <= 1.){
            //                 eta_one_track_counter += 1;
            //             }
            //         }
            //     }
            // }
            
            
            // cout<<i<<" z vertex : "<<inttDSTMC.TruthPV_trig_z<<endl;
            MC_vtx_xy -> Fill(inttDSTMC.TruthPV_trig_x, inttDSTMC.TruthPV_trig_y);
            MC_vtx_z  -> Fill( inttDSTMC.TruthPV_trig_z );
            MC_vtx_Npart -> Fill( inttDSTMC.TruthPV_trig_Npart );
            // MC_Npart_Ntrack -> Fill(inttDSTMC.TruthPV_trig_Npart, inttDSTMC.UniqueAncG4P_TrackID -> size());
            
            
            MC_INTT_multiplicity -> Fill( inttDSTMC.ClusX -> size() );
            MC_INTT_clu_nhit_corr -> Fill(inttDSTMC.NTrkrhits, inttDSTMC.ClusX -> size());
            

            for (int clu_i = 0; clu_i < inttDSTMC.ClusX -> size(); clu_i++){
                MC_INTT_CluPhiSize -> Fill( inttDSTMC.ClusPhiSize -> at(clu_i) );
                MC_INTT_CluZSize -> Fill( inttDSTMC.ClusZSize -> at(clu_i) );
                MC_INTT_CluADC -> Fill( inttDSTMC.ClusAdc -> at(clu_i) );
                
                if (inttDSTMC.ClusLayer -> at(clu_i) == 3){
                    MC_inner_phi_3 -> Fill( (inttDSTMC.ClusY -> at(clu_i) < 0) ? atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) + 360 : atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) );
                }
                else if (inttDSTMC.ClusLayer -> at(clu_i) == 4){
                    MC_inner_phi_4 -> Fill( (inttDSTMC.ClusY -> at(clu_i) < 0) ? atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) + 360 : atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) );
                }
                else if (inttDSTMC.ClusLayer -> at(clu_i) == 5){
                    MC_outer_phi_5 -> Fill( (inttDSTMC.ClusY -> at(clu_i) < 0) ? atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) + 360 : atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) );
                }
                else if (inttDSTMC.ClusLayer -> at(clu_i) == 6){
                    MC_outer_phi_6 -> Fill( (inttDSTMC.ClusY -> at(clu_i) < 0) ? atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) + 360 : atan2(inttDSTMC.ClusY -> at(clu_i), inttDSTMC.ClusX -> at(clu_i) ) * (180./TMath::Pi()) );
                }
                
               
            }


        
        }

        // for (int vtx_i = 0; vtx_i < inttDSTMC.TruthPV_x -> size(); vtx_i++){
        //     printf("VTX vec ID-%i : %.2f, %.2f, %.2f \n", vtx_i, inttDSTMC.TruthPV_x -> at(vtx_i), inttDSTMC.TruthPV_y -> at(vtx_i), inttDSTMC.TruthPV_z -> at(vtx_i));
        // }
        // cout<<"~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ "<<endl;
        // for (int clu_i = 0; clu_i < ClusX -> size(); clu_i++){
        //     printf("cluster %zu : X %.2f, Y %.2f, Z %.2f, sizePhi : %f, sizeZ : %f,  \n", clu_i, ClusX -> at(clu_i), ClusY -> at(clu_i), ClusZ -> at(clu_i), ClusPhiSize -> at(clu_i), ClusZSize -> at(clu_i));
        // }
    }

    MC_true_track_eta -> Scale(1./double(good_evt_counter));
    cout<<"N track in eta +- 1 : "<<eta_one_track_counter<<" good_evt_counter "<<good_evt_counter<<endl;
    // MC_true_track_eta -> Scale(1/0.2);

    TFile * INTT_MC_sanity_out = new TFile(Form("%s/MC_sanity_intt.root",folder_direction.c_str()), "recreate");
    MC_vtx_xy -> Write();
    MC_vtx_z -> Write();
    MC_NClu_Zvtx -> Write();
    MC_vtx_Npart -> Write();
    MC_Npart_Ntrack -> Write();
    MC_INTT_multiplicity -> Write();
    MC_INTT_CluPhiSize -> Write();
    MC_INTT_CluZSize -> Write();
    MC_INTT_CluADC -> Write();
    MC_INTT_clu_nhit_corr -> Write();
    MC_Ntrack_NClu -> Write();
    MC_Ntrack_1D -> Write();
    MC_true_track_eta -> Write();
    MC_inner_phi_3 -> Write();
    MC_inner_phi_4 -> Write();
    MC_outer_phi_5 -> Write();
    MC_outer_phi_6 -> Write();
    INTT_MC_sanity_out -> Close();

}