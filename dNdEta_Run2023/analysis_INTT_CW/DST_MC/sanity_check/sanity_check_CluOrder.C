#include "../INTTDSTchain.C"
vector<string> read_list (string MC_directory, string MC_list_name)
{
    string list_buffer;
    ifstream data_list;
	data_list.open((MC_directory + MC_list_name).c_str());

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

void sanity_check_CluOrder()
{
    string folder_direction = "/sphenix/user/ChengWei/sPH_dNdeta/dNdEta_INTT_MC";
    // string MC_list_name = "dst_INTTdNdEta.list";
    // vector <string> file_list = read_list(folder_direction,MC_list_name);
    vector <string> file_list = {"INTTRecoClusters_test_400_0.root"};

    TChain * chain_in = new TChain("EventTree");
    INTTDSTchain inttDSTMC(chain_in,folder_direction,file_list);
    std::printf("inttDSTMC N event : %lli \n", chain_in->GetEntries());

    const int chekced_evt = 24; 
    TH2F * MC_XY_2D = new TH2F(Form("MC_XY_2D"),Form("MC_XY_2D"),200,-150,150,200,-150,150);
    MC_XY_2D -> GetXaxis() -> SetTitle("X axis [mm]");
    MC_XY_2D -> GetYaxis() -> SetTitle("Y axis [mm]");

    TH2F * MC_X_order = new TH2F(Form("MC_X_order"),Form("MC_X_order"),200,0,200,200,-150,150);
    MC_X_order -> GetXaxis() -> SetTitle("Cluster order");
    MC_X_order -> GetYaxis() -> SetTitle("X axis [mm]");

    TH2F * MC_Y_order = new TH2F(Form("MC_Y_order"),Form("MC_Y_order"),200,0,200,200,-150,150);
    MC_Y_order -> GetXaxis() -> SetTitle("Cluster order");
    MC_Y_order -> GetYaxis() -> SetTitle("Y axis [mm]");

    TH2F * MC_Z_order = new TH2F(Form("MC_Z_order"),Form("MC_Z_order"),200,0,200,200,-250,250);
    MC_Z_order -> GetXaxis() -> SetTitle("Cluster order");
    MC_Z_order -> GetYaxis() -> SetTitle("Z axis [mm]");

    TH2F * MC_phi_order = new TH2F(Form("MC_phi_order"),Form("MC_phi_order"),200,0,200,360,0,360);
    MC_phi_order -> GetXaxis() -> SetTitle("Cluster order");
    MC_phi_order -> GetYaxis() -> SetTitle("Cluster #phi [degree]");



    for (int i = chekced_evt; i < chekced_evt+1; i++) {
        inttDSTMC.LoadTree(i);
        inttDSTMC.GetEntry(i);

        if (i % 1000 == 0){
            cout<<"evnet running : "<<i<<endl;
            cout<<"----------------------------------------------- -----------------------------------------------"<<endl;
            printf(" event : %i, NTruthVtx : %i, NClus : %i, NTrkrhits : %i \n", inttDSTMC.event, inttDSTMC.NTruthVtx, inttDSTMC.NClus, inttDSTMC.NTrkrhits);
            printf(" size of ClusX : %zu \n", inttDSTMC.ClusX -> size());
            printf("triggered VTX ? : %.2f, %.2f, %.2f \n", inttDSTMC.TruthPV_trig_x, inttDSTMC.TruthPV_trig_y, inttDSTMC.TruthPV_trig_z);
        }


        for (int clu_i = 0; clu_i < inttDSTMC.ClusX -> size(); clu_i++){
            
            double clu_x = inttDSTMC.ClusX -> at(clu_i) * 10; // note : change the unit from cm to mm
            double clu_y = inttDSTMC.ClusY -> at(clu_i) * 10;
            double clu_z = inttDSTMC.ClusZ -> at(clu_i) * 10;
            double clu_phi = (clu_y < 0) ? atan2(clu_y,clu_x) * (180./TMath::Pi()) + 360 : atan2(clu_y,clu_x) * (180./TMath::Pi());

            MC_XY_2D     -> Fill(clu_x,clu_y);
            MC_X_order   -> Fill(clu_i, clu_x);
            MC_Y_order   -> Fill(clu_i, clu_y);
            MC_Z_order   -> Fill(clu_i, clu_z);
            MC_phi_order -> Fill(clu_i, clu_phi);
        }
    }

    TFile * INTT_MC_sanity_out = new TFile(Form("%s/MC_CluOrder_evt_%i.root",folder_direction.c_str(),chekced_evt), "recreate");
    MC_XY_2D -> Write();
    MC_X_order -> Write();
    MC_Y_order -> Write();
    MC_Z_order -> Write();
    MC_phi_order -> Write();
    INTT_MC_sanity_out -> Close();

}